
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <iostream>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <stdio.h>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <ctime>
#include <stdlib.h>
#include <random>
#include <conio.h>
#include <unordered_map>
#include <algorithm>


std::unordered_map<int, int> y_count;

//Object IDs
enum IDS { PLAYER, BULLET, ENEMY };

//Our Player
struct SpaceShip
{
	int ID;
	int x;
	int y;
	int lives;
	int speed;
	int boundx;
	int boundy;
	int score;
	bool live;
};

struct Bullet
{
	int ID;
	int x;
	int x1;
	int y;
	bool live;
	int speed;
};

struct Comet
{
	int ID;
	int x;
	int y;
	bool live;
	int speed;
	int bt;
	int boundx;
	int boundy;
	bool stop;
};
struct Tetris {
	int x;
	int y;
	bool live;
	bool stop;
};
std::vector<Tetris> tetromino;
std::vector<std::vector<int>> line;






//GLOBALS==============================
const int WIDTH = 800;
const int HEIGHT = 600;
const int NUM_BULLETS = 50;
const int NUM_COMETS = 30;
const int STARS = 100;
int sec = 0;
int frameCount = 0;
bool imm = 0;
bool bt = 0;


int it = 0;
bool line1 = false;
int l = 0;

bool playExplosion = false;
float explosionX, explosionY;

const float SHOOT_DELAY = 0.3f; // Delay between shots in seconds
float lastFireTime = 0; // Time since last shot

int fireCooldown = 0;  // Time remaining before next bullet can be fired
const int fireRate = 10;  // Set a delay of 60 frames (1 second at 60 FPS)

enum MODE { SINGLE, LINE, SPREAD, DOUBLE_LINE };
bool mode[4] = { true , false, false, false };
float deltaTime = 1.0 / 60.0;

struct Explosion {
	float x, y;
	bool active;
	float frameTime;
	int currentFrame;
};

const int MAX_EXPLOSIONS = 10;  // Maximum number of simultaneous explosions
Explosion explosions[MAX_EXPLOSIONS];

// Initialize explosions
void InitExplosions() {
	for (int i = 0; i < MAX_EXPLOSIONS; i++) {
		explosions[i].active = false;
		explosions[i].frameTime = 0.0f;
		explosions[i].currentFrame = 0;
	}
}

// Activate a new explosion
void ActivateExplosion(float x, float y) {
	for (int i = 0; i < MAX_EXPLOSIONS; i++) {


		if (!explosions[i].active) {

			explosions[i].x = x;
			explosions[i].y = y;
			explosions[i].active = true;
			explosions[i].frameTime = 0.0f;
			explosions[i].currentFrame = 0;
			break;
		}
	}
}

// Update and draw all explosions
void UpdateAndDrawExplosions(ALLEGRO_BITMAP* spriteSheet, float deltaTime) {
	for (int i = 0; i < MAX_EXPLOSIONS; i++) {
		if (explosions[i].active) {
			explosions[i].frameTime += deltaTime;
			if (explosions[i].frameTime >= 0.1) {
				//std::cout << "update frame" << std::endl;
				explosions[i].frameTime -= 0.1;
				explosions[i].currentFrame++;

				if (explosions[i].currentFrame >= 13) {
					explosions[i].active = false;  // Stop explosion when animation ends
				}
			}

			// Draw explosion frame
			if (explosions[i].active) {
				//std::cout << "update draw" << std::endl;

				int frameWidth = 62;
				int frameHeight = 60;
				al_draw_bitmap_region(spriteSheet, explosions[i].currentFrame * frameWidth, 0,
					frameWidth, frameHeight,
					explosions[i].x - frameWidth / 2,
					explosions[i].y - frameHeight / 2, 0);
			}
		}
	}
}


//prototypes
void InitShip(SpaceShip& ship);
void DrawShip(SpaceShip& ship, ALLEGRO_BITMAP* shipImg, bool imm, int frameCount);


void InitBullet(Bullet bullet[], int size);
void DrawBullet(Bullet bullet[], int size, ALLEGRO_BITMAP* projectile);
void FireBullet(Bullet bullet[], int size, SpaceShip& ship, ALLEGRO_SAMPLE* shoot);
void FireBullet3(Bullet bullet[], int size, SpaceShip& ship, ALLEGRO_SAMPLE* shoot, float currentTime);
void FireBullet2(Bullet bullet[], int size, SpaceShip& ship, int& fireCooldown, int fireRate, ALLEGRO_SAMPLE* shoot, float currentTime);

void UpdateBulletSingle(Bullet bullet[], int size);
void UpdateBulletLine(Bullet bullet[], int size);
void UpdateBulletSpread(Bullet bullet[], int size);

void InitComet(Comet comets[], int size);
void DrawComet(Comet comets[], int size, ALLEGRO_BITMAP* comet);
void StartComet(Comet comets[], int size);
void UpdateComet(Comet comets[], int size);


// Function to draw the background image
void DrawBackground(ALLEGRO_BITMAP* background, float& background_y) {
	int image_height = al_get_bitmap_height(background);

	// Draw the first instance of the background
	al_draw_bitmap(background, 0, background_y, 0);

	// Draw the second instance of the background if necessary
	if (background_y >= 600 - image_height) { // Check if the first instance is out of view
		al_draw_bitmap(background, 0, background_y - image_height, 0);
	}

	// Scroll the background downwards
	background_y += 4; // Adjust this value for faster/slower scrolling

	// Continuously scroll without resetting
	// You can control how far to scroll by adjusting the background_y
	// If you want it to keep going, you can just limit how far it goes
	// For example:
	if (background_y >= image_height) {
		background_y -= image_height; // Shift up by one image height to continue the loop
	}
}

void CollideBullet(Bullet bullet[], int bSize, Comet comets[], int cSize, ALLEGRO_SAMPLE_INSTANCE* explosionInstance, ALLEGRO_BITMAP* spriteSheet, float deltaTime);
void CollideComet(Comet comets[], int cSize, SpaceShip& ship, ALLEGRO_SAMPLE_INSTANCE* explosionInstance);
// Declare constants globally
const int NUM_FRAMES = 13;
const float FRAME_DURATION = 0.1;  // 0.1 seconds per frame

// Declare animation variables globally or locally, depending on your design
float frameTime = 0.0;  // Keep track of time
int currentFrame = 0;   // Track current frame

void PlayAnimation(ALLEGRO_BITMAP* spriteSheet, float deltaTime, float x, float y) {
	int frameWidth = 31;  // Width of each frame
	int frameHeight = 30; // Height of each frame
	//std::cout << spriteSheet << deltaTime << x << y;
	// Update the frame based on elapsed time
	frameTime += deltaTime;
	if (frameTime >= FRAME_DURATION) {
		frameTime -= FRAME_DURATION;
		currentFrame = (currentFrame + 1) % NUM_FRAMES;
	}

	// Draw the current frame of the animation
	al_draw_bitmap_region(spriteSheet, currentFrame * frameWidth, 0, frameWidth, frameHeight, x, y, 0);

}

int main(void)
{
	srand(time(NULL));
	//primitive variable
	bool done = false;
	bool redraw = true;
	const int FPS = 20;
	int cntr = 0;
	int licznik = 0;
	int licznik2 = 0;
	int licznik_def = 580;
	int licztab = 0;
	int licz = 0;
	bool row = false;
	bool rozm = false;
	std::vector<std::vector <int>> line_def;

	std::vector<std::vector <int>> line;
	std::vector< std::vector<int>> sprawdz(15, std::vector<int>(2));
	std::vector< std::vector<int>> sprawdz1(15, std::vector<int>(2));

	line_def.push_back({ 20, 580 });
	int wspolrzedna_linii = 20;

	int tab[15][2];
	tab[0][0] = 0;
	tab[0][1] = 580;

	int traf = 0;


	//object variables
	SpaceShip ship;
	Bullet bullets[NUM_BULLETS];
	Comet comets[NUM_COMETS];
	//Allegro variables
	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_EVENT_QUEUE* event_queue = NULL;
	ALLEGRO_TIMER* timer = NULL;
	ALLEGRO_BITMAP* background = NULL;
	ALLEGRO_BITMAP* enemy = NULL;
	ALLEGRO_BITMAP* shipImg = NULL;
	ALLEGRO_BITMAP* projectile = NULL;
	ALLEGRO_BITMAP* spriteSheet = NULL;

	const int NUM_FRAMES = 13;
	const float FRAME_DURATION = 0.1;  // Each frame lasts 0.1 seconds

	int currentFrame = 0;
	float frameTime = 0.0;


	//Initialization Functionsv
	if (!al_init())										//initialize Allegro
		return -1;

	display = al_create_display(WIDTH, HEIGHT);			//create our display object

	if (!display)										//test display object
		return -1;

	al_init_primitives_addon();
	al_init_image_addon();
	al_install_keyboard();
	al_install_mouse();
	al_install_audio();
	al_init_acodec_addon();  // Add parentheses to correctly call the function
	al_reserve_samples(3);

	ALLEGRO_SAMPLE* shoot = al_load_sample("shoot2.mp3");
	ALLEGRO_SAMPLE* backgroundMusic = al_load_sample("music.mp3");
	ALLEGRO_SAMPLE* explosion = al_load_sample("explosion2.mp3");
	ALLEGRO_SAMPLE_INSTANCE* musicInstance = al_create_sample_instance(backgroundMusic);
	ALLEGRO_SAMPLE_INSTANCE* explosionInstance = al_create_sample_instance(explosion);
	ALLEGRO_SAMPLE_ID explosionSampleId;
	al_set_sample_instance_playmode(musicInstance, ALLEGRO_PLAYMODE_LOOP);
	al_set_sample_instance_playmode(explosionInstance, ALLEGRO_PLAYMODE_ONCE);

	//al_play_sample(backgroundMusic, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);

	al_attach_sample_instance_to_mixer(musicInstance, al_get_default_mixer());
	al_attach_sample_instance_to_mixer(explosionInstance, al_get_default_mixer());
	if (!al_install_audio()) {
		std::cerr << "Failed to initialize audio!" << std::endl;
		return -1;
	}

	if (!al_init_acodec_addon()) {  // Correctly call the acodec addon initialization
		std::cerr << "Failed to initialize acodec addon!" << std::endl;
		return -1;
	}

	if (!al_reserve_samples(1)) {
		std::cerr << "Failed to reserve samples!" << std::endl;
		return -1;
	}

	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / 60);

	InitShip(ship);
	InitBullet(bullets, NUM_BULLETS);
	InitComet(comets, NUM_COMETS);
	InitExplosions();
	//InitTetris


	//InitLine



	//InitFall






	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_mouse_event_source());

	al_hide_mouse_cursor(display);
	al_set_sample_instance_gain(musicInstance, 1);
	al_play_sample_instance(musicInstance);
	al_start_timer(timer);

	background = al_load_bitmap("new bg.png");
	enemy = al_load_bitmap("new comet.png");
	shipImg = al_load_bitmap("ship5.png");
	projectile = al_load_bitmap("projectile2.png");
	spriteSheet = al_load_bitmap("explosion2.png");
	// Variables for background scrolling
	float background_y = 0;
	//float scroll_speed = 2.0; // Speed of the scrolling





	if (!background) {
		std::cerr << "Failed to load background image!" << std::endl;
		return -1;
	}


	while (!done)
	{
		ALLEGRO_EVENT ev;
		ALLEGRO_MOUSE_STATE state;
		//std::cout << "x" << ship.x << "y" << ship.y << std::endl;

		//std::cout << "imm"<<imm <<"   "<<"live" <<ship.live<<"   "<<"sec"<<sec<<std::flush;



		al_get_mouse_state(&state);
		al_wait_for_event(event_queue, &ev);
		frameCount++;
		sec++;
		//std::cout << sec;
		if (ev.type == ALLEGRO_EVENT_TIMER) {
			redraw = true;

			// Scroll the background

		}



		if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
		{

			if (ev.mouse.button == 1)
			{
				if (mode[SINGLE])
				{
					//std::cout << "single";
					FireBullet(bullets, NUM_BULLETS, ship, shoot);
				}
			}

			else if (ev.mouse.button == 2)
			{
				cntr++;
				if (cntr == 4)

					cntr = 0;
				if (cntr > 0)
				{
					mode[cntr] = true;
					mode[cntr - 1] = false;
				}
				if (cntr == 0)
				{
					mode[0] = true;
					mode[1] = false;
					mode[2] = false;
					mode[3] = false;
				}
				//std::cout << "mode";
			}
			else if (ev.mouse.button == 3) { bt = !bt; }






		}
		//else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button & 2) for (int i = 0; i < 4; i++) std::cout << mode[i];
		else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES)
		{
			ship.x = ev.mouse.x;

			ship.y = ev.mouse.y;

		}
		if (state.buttons & 1 && mode[SPREAD])
		{

			//float currentTime = al_get_time(); // Get the current time
			float currentTime = al_get_time();
			FireBullet3(bullets, NUM_BULLETS, ship, shoot, currentTime);
		}
		if (state.buttons & 1 && mode[LINE])
		{
			//std::cout << "spread";
			float currentTime = al_get_time();
			FireBullet2(bullets, NUM_BULLETS, ship, fireCooldown, fireRate, shoot, currentTime);
		}
		if (state.buttons & 1 && mode[DOUBLE_LINE])
		{
			//std::cout << "line";
			float currentTime = al_get_time();
			FireBullet2(bullets, NUM_BULLETS, ship, fireCooldown, fireRate, shoot, currentTime);  // Pass fireCooldown and fireRate
		}
		//if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button & 2) FireBullet(bullets, 1, ship);
		if (ev.type == ALLEGRO_EVENT_TIMER)
		{
			redraw = true;
			CollideBullet(bullets, NUM_BULLETS, comets, NUM_COMETS, explosionInstance, spriteSheet, deltaTime);

			float deltaTime = 1.0 / 60.0;
			if (mode[SINGLE]) UpdateBulletSingle(bullets, NUM_BULLETS);
			if (mode[LINE] || mode[DOUBLE_LINE]) UpdateBulletLine(bullets, NUM_BULLETS);

			if (mode[SPREAD]) UpdateBulletSpread(bullets, NUM_BULLETS);

			StartComet(comets, NUM_COMETS);
			UpdateComet(comets, NUM_COMETS);
			CollideComet(comets, NUM_COMETS, ship, explosionInstance);
			//for (auto& v : tetromino) {
			//	if (!v.stop) {
			//		v.y += 5;  // Set stop to false for tetrominos directly above the cleared line
			//		std::cout << v.stop;
			//	}
			//}
							//std::cout << ship.x << ":" << ship.y << std::endl;

				for (int i = 0; i < tetromino.size(); i++)
				{
					if (tetromino[i].live && !imm)
					{
						if (tetromino[i].x - 20 < ship.x + ship.boundx &&
							tetromino[i].x + 20 > ship.x - ship.boundx &&
							tetromino[i].y - 20 < ship.y + ship.boundy &&
							tetromino[i].y + 20 > ship.y - ship.boundy)
						{
							//std::cout << "Collision detected" << std::endl;

							// Stop the current explosion sound before playing a new one
							//al_stop_sample_instance(explosionInstance);

							// Reduce ship's lives and set states
							//ship.lives--;
							tetromino[i].live = false;
							//ship.live = false;
							//imm = true;

							// Play the explosion sound
							al_play_sample_instance(explosionInstance);
						}
						sec = 0;
					}
				}
			

			for (int i = 0; i < tetromino.size(); i++)
				if (!tetromino[i].stop)
					tetromino[i].y += 5;
			for (int i = 0; i < tetromino.size(); i++)
				if (!tetromino[i].stop && tetromino[i].y + 20 == 600)
				{
					tetromino[i].stop = true;
					tetromino[i].y = 580;
				}
			for (int i = 0; i < tetromino.size(); i++)
				for (int j = 0; j < tetromino.size(); j++)
					if (tetromino[i].live && tetromino[j].live &&
						tetromino[i].stop && !tetromino[j].stop && tetromino[i].x == tetromino[j].x && tetromino[i].y - 40 <= tetromino[j].y)
					{
						tetromino[j].stop = true;
						tetromino[j].y = tetromino[i].y - 40;
					}
			//StartTetris
			// Process comets and add to tetromino
			for (int i = 0; i < NUM_COMETS; i++) {
				if (comets[i].live && comets[i].y == 580) {
					comets[i].live = false;
					tetromino.push_back({ comets[i].x, comets[i].y, true, true });
				}
			}

			// Compare comets and tetromino for the additional condition
			for (int i = 0; i < NUM_COMETS; i++) {
				for (size_t j = 0; j < tetromino.size(); j++) {
					if (comets[i].live && tetromino[j].stop && tetromino[j].live &&
						comets[i].x == tetromino[j].x && comets[i].y + 20 == tetromino[j].y - 20 && comets[i].y + 20 > -6) {
						comets[i].live = false;
						tetromino.push_back({ comets[i].x, comets[i].y, true, true });
					}
				}
			}

			// Process comets and add to tetromino
			for (int i = 0; i < NUM_COMETS; i++) {
				if (comets[i].live && comets[i].y == 580) {
					comets[i].live = false;
					tetromino.push_back({ comets[i].x, comets[i].y, true, true });
				}
			}

			// Compare comets and tetromino for the additional condition
			for (int i = 0; i < NUM_COMETS; i++) {
				for (size_t j = 0; j < tetromino.size(); j++) {
					if (comets[i].live && tetromino[j].stop && tetromino[j].live &&
						comets[i].x == tetromino[j].x && comets[i].y + 20 == tetromino[j].y - 20 && comets[i].y + 20 > -6) {
						comets[i].live = false;
						tetromino.push_back({ comets[i].x, comets[i].y, true, true });
					}
				}
			}

			// Count occurrences of each y value where both v.live and v.stop are true
			// Process comets and add to tetromino
			for (int i = 0; i < NUM_COMETS; i++) {
				if (comets[i].live && comets[i].y == 580) {
					comets[i].live = false;
					tetromino.push_back({ comets[i].x, comets[i].y, true, true });
				}
			}

			// Compare comets and tetromino for the additional condition
			for (int i = 0; i < NUM_COMETS; i++) {
				for (size_t j = 0; j < tetromino.size(); j++) {
					if (comets[i].live && tetromino[j].stop && tetromino[j].live &&
						comets[i].x == tetromino[j].x && comets[i].y + 20 == tetromino[j].y - 20 && comets[i].y + 20 > -6) {
						comets[i].live = false;
						tetromino.push_back({ comets[i].x, comets[i].y, true, true });
					}
				}
			}

			// Count occurrences of each y value where both v.live and v.stop are true
			// Step 1: Group tetrominos by their y value where both v.live and v.stop are true
			// Step 1: Group tetrominos by their y value where both v.live and v.stop are true
			// Step 1: Group tetrominos by their y value if both v.live and v.stop are true
			// Step 1: Group tetrominos by their y value if both v.live and v.stop are true
			// Step 1: Group tetrominos by their y value if both v.live and v.stop are true
			// Step 1: Group tetrominos by their y value if both v.live and v.stop are true
			// Step 1: Group tetrominos by their x and y values if both v.live and v.stop are true
			// Step 1: Group tetrominos by their y values and count how many are at the same y
			// Step 1: Group tetrominos by their y values and count how many are at the same y
			// Step 1: Group tetrominos by their y values and count how many are at the same y
			std::unordered_map<int, std::vector<Tetris*>> y_groups;

			// Group tetrominos by their y value
			for (auto& v : tetromino) {
				if (v.live && v.stop) {
					y_groups[v.y].push_back(&v);  // Store pointers to tetrominos grouped by y
				}
			}

			// Step 2: Detect if there are exactly 20 tetrominos at the same y value
			int target_y = -1;
			for (const auto& pair : y_groups) {
				if (pair.second.size() == 20) {  // If exactly 20 tetrominos have the same y value
					target_y = pair.first;  // Get the y value where we have 20 tetrominos
					break;  // Once found, no need to check further
				}
			}

			// Step 3: If we detected 20 tetrominos at the same y value
			if (target_y != -1) {
				// Step 4: Set live = false for all tetrominos at the target_y value
				for (auto* v : y_groups[target_y]) {
					v->live = false;
					//std::cout << "Set live = false for tetromino at x: " << v->x << ", y: " << v->y << std::endl;
				}

				// Step 5: Now set stop = false for all tetrominos with the same x and y < target_y
				for (auto* v : y_groups[target_y]) {
					int target_x = v->x;  // The x coordinate of the tetromino with live = false

					// Step 6: Iterate through all tetrominos and find the matching ones
					for (auto& other : tetromino) {
						if (other.x == target_x && other.y < target_y && other.live && other.stop) {
							other.stop = false;  // Set stop = false
							//std::cout << "Set stop = false for tetromino at x: " << other.x << ", y: " << other.y << std::endl;
						}
					}
				}

				// Step 7: Delete all tetrominos where live = false
				tetromino.erase(
					std::remove_if(tetromino.begin(), tetromino.end(), [](const Tetris& t) {
						return !t.live;  // Remove tetrominos where live = false
						}),
					tetromino.end()
							);
			}

			// New functionality: Set stop = false above live = false tetrominos
			for (auto& v : tetromino) {
				if (!v.live) {
					int target_x = v.x;
					int target_y = v.y;

					// Find tetrominos directly above and set stop = false
					for (auto& other : tetromino) {
						if (other.x == target_x && other.y < target_y && other.live && other.stop) {
							other.stop = false;  // Set stop = false for tetrominos above
							//std::cout << "Set stop = false for tetromino at x: " << other.x << ", y: " << other.y << std::endl;
						}
					}
				}
			}

			//PozaEkran


			//StartLine


			//SprawdzLinie


			//BlockAndFall


			//CollideRunningTetris
			//CollideTetris
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			done = true;
		}

		if (imm)
		{
			//std::cout << "imm";
			if (sec % 10 == 0) ship.live = true;
			else ship.live = false;
			if (sec == 120)
			{
				//std::cout << "120";
				ship.live = true;
				imm = false;
				sec = 0;
			}
		}

		if (redraw && al_is_event_queue_empty(event_queue))
		{
			redraw = false;

			//al_draw_bitmap(background, 0, background_y, 0);
			//al_draw_bitmap(background, 0, background_y - 600, 0);
			DrawBackground(background, background_y);
			DrawShip(ship, shipImg, imm, frameCount);
			DrawBullet(bullets, NUM_BULLETS, projectile);
			DrawComet(comets, NUM_COMETS, enemy);
			UpdateAndDrawExplosions(spriteSheet, deltaTime);

			//DrawTetris
			for (size_t i = 0; i < tetromino.size(); ++i)
				if (tetromino[i].live)
					al_draw_bitmap(enemy, tetromino[i].x - al_get_bitmap_width(enemy) / 2, tetromino[i].y - al_get_bitmap_height(enemy) / 2, 0);


			al_flip_display();
			al_clear_to_color(al_map_rgb(0, 0, 0));
		}
	}
	al_destroy_bitmap(background);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_display(display);						//destroy our display object

	return 0;
}

void InitShip(SpaceShip& ship)
{
	ship.x = 20;
	ship.y = HEIGHT / 2;
	ship.ID = PLAYER;

	ship.lives = 3;
	ship.speed = 7;
	ship.boundx = 35;
	ship.boundy = 50;
	ship.score = 0;
	ship.live = true;
}

void DrawShip(SpaceShip& ship, ALLEGRO_BITMAP* shipImg, bool imm, int frameCount)
{
	// Only draw the ship if imm is false, or if imm is true and the frame count is even (blinking effect)
	if (!imm || (imm && (frameCount / 10) % 2 == 0)) {
		float imgWidth = al_get_bitmap_width(shipImg);
		float imgHeight = al_get_bitmap_height(shipImg);
		al_draw_bitmap(shipImg, ship.x - imgWidth / 2, ship.y - imgHeight / 2, 0);
	}
}

void MoveShipUp(SpaceShip& ship)
{
	ship.y -= ship.speed;
	if (ship.y < 0)
		ship.y = 0;
}
void MoveShipDown(SpaceShip& ship)
{
	ship.y += ship.speed;
	if (ship.y > HEIGHT)
		ship.y = HEIGHT;
}
void MoveShipLeft(SpaceShip& ship)
{
	ship.x -= ship.speed;
	if (ship.x < 0)
		ship.x = 0;
}
void MoveShipRight(SpaceShip& ship)
{
	ship.x += ship.speed;
	if (ship.x > 600)
		ship.x = 600;
}

void InitBullet(Bullet bullet[], int size)
{
	for (int i = 0; i < size; i++)
	{
		bullet[i].ID = BULLET;
		bullet[i].speed = 10;
		bullet[i].live = false;
	}
}
void DrawBullet(Bullet bullet[], int size, ALLEGRO_BITMAP* projectile) {
	if (!projectile) {
		std::cerr << "Error: projectile bitmap is null!" << std::endl;
		return;
	}

	float imgWidth = al_get_bitmap_width(projectile);
	float imgHeight = al_get_bitmap_height(projectile);

	for (int i = 0; i < size; i++) {
		if (bullet[i].live) {
			if (mode[DOUBLE_LINE]) {
				al_draw_bitmap(projectile, bullet[i].x - imgWidth / 2 + 15, bullet[i].y, 0);
				al_draw_bitmap(projectile, bullet[i].x - imgWidth / 2 - 15, bullet[i].y, 0);
			}
			else {
				al_draw_bitmap(projectile, bullet[i].x - imgWidth / 2, bullet[i].y, 0);
			}
		}
	}
}

ALLEGRO_SAMPLE_ID shootSampleId; // Global variable to track the sound

void FireBullet(Bullet bullet[], int size, SpaceShip& ship, ALLEGRO_SAMPLE* shoot) {
	for (int i = 0; i < size; i++) {
		if (!bullet[i].live) {
			bullet[i].x = ship.x;
			bullet[i].y = ship.y - 17;
			bullet[i].live = true;

			// Stop the previous sound if it's still playing
			al_stop_sample(&shootSampleId);

			// Play the shoot sound and store the sample ID
			al_play_sample(shoot, 0.3, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &shootSampleId);
			break;
		}
	}
}
float lastPlayTime = 0.0f; // Variable to track the last play time

void FireBullet3(Bullet bullet[], int size, SpaceShip& ship, ALLEGRO_SAMPLE* shoot, float currentTime) {
	for (int i = 0; i < size; i++) {
		if (!bullet[i].live) {
			bullet[i].x = ship.x;
			bullet[i].y = ship.y - 17;
			bullet[i].live = true;

			// Check if 0.3 seconds have passed since the last sound was played
			if (currentTime - lastPlayTime >= 0.1f) {
				// Stop the previous sound if it's still playing
				al_stop_sample(&shootSampleId);

				// Play the shoot sound and store the sample ID
				al_play_sample(shoot, 0.3, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &shootSampleId);

				// Update last play time to the current time
				lastPlayTime = currentTime;
			}
			break;
		}
	}
}



void FireBullet2(Bullet bullet[], int size, SpaceShip& ship, int& fireCooldown, int fireRate, ALLEGRO_SAMPLE* shoot, float currentTime)
{
	// Decrease the cooldown every frame, but only if it's greater than 0
	if (fireCooldown > 0)
	{
		fireCooldown--;
	}

	// Only fire when the cooldown is 0
	if (fireCooldown == 0)
	{
		for (int i = 0; i < size; i++)
		{
			if (!bullet[i].live)  // Find an inactive bullet
			{
				bullet[i].x = ship.x;
				bullet[i].y = ship.y - 17;  // Adjust to fire from the ship's position
				bullet[i].live = true;
				//al_play_sample(shoot, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
				fireCooldown = fireRate;  // Reset the cooldown to fireRate after firing
				if (currentTime - lastPlayTime >= 0.1f) {
					// Stop the previous sound if it's still playing
					al_stop_sample(&shootSampleId);

					// Play the shoot sound and store the sample ID
					al_play_sample(shoot, 0.3, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &shootSampleId);

					// Update last play time to the current time
					lastPlayTime = currentTime;
				}
				break;  // Fire only one bullet per cooldown cycle
			}
		}
	}
}

void UpdateBulletSpread(Bullet bullet[], int size)
{
	int tempo = 0;
	for (int i = 0; i < size; i++)
	{
		tempo += 5;

		if (bullet[i].live)
		{
			bullet[i].y -= bullet[i].speed + tempo;
			if (bullet[i].y < 0)
				bullet[i].live = false;

		}
	}
}

void UpdateBulletLine(Bullet bullet[], int size)
{
	// Decrease cooldown over time (every frame)


	// Check if it's time to fire a bullet (only if cooldown is 0)

		// Find an inactive bullet and fire it



	// Move active bullets
	for (int i = 0; i < size; i++)
	{
		if (bullet[i].live)
		{
			bullet[i].y -= bullet[i].speed + 5;  // Move bullet upwards
			if (bullet[i].y < 0)
				bullet[i].live = false;  // Deactivate bullet if off screen
		}
	}
}


void UpdateBulletSingle(Bullet bullet[], int size)
{

	for (int i = 0; i < size; i++)
	{
		if (bullet[i].live)
		{
			bullet[i].y -= bullet[i].speed;
			if (bullet[i].y < 0)
				bullet[i].live = false;

		}
	}

}

void InitComet(Comet comets[], int size)
{

	for (int i = 0; i < size; i++)
	{
		if (comets[i].y < 0)
		{
			comets[i].ID = ENEMY;
			comets[i].live = false;
			comets[i].speed = 5;
			comets[i].boundx = 18;
			comets[i].boundy = 18;
			comets[i].stop = false;

		}
	}
}
void DrawComet(Comet comets[], int size, ALLEGRO_BITMAP* comet) {
	for (int i = 0; i < size; i++) {
		if (comets[i].live && comets[i].y > 0) {
			al_draw_bitmap(comet, comets[i].x - al_get_bitmap_width(comet) / 2, comets[i].y - al_get_bitmap_height(comet) / 2, 0);
		}
	}
}

// Start Comet Function
void StartComet(Comet comets[], int size) {
	bool ixy = false;
	int liczx = 0;
	int liczy = 0;
	int iksy[20];
	int ygreki[15];
	iksy[0] = 20;
	ygreki[0] = -580;

	// Initialize possible x and y positions (divisible by 40)
	for (int i = 60; i < 800; i += 40) {
		liczx++;
		iksy[liczx] = i;
	}

	for (int i = -540; i < 0; i += 40) {
		liczy++;
		ygreki[liczy] = i;
	}

	// Spawn comets
	for (int i = 0; i < size; i++) {
		if (!comets[i].live && !ixy) {
			bool validPosition = false;
			int attempts = 0;

			while (!validPosition && attempts < 100) { // Prevent infinite loops
				attempts++;
				validPosition = true;
				comets[i].live = true;
				comets[i].x = iksy[rand() % 20];
				comets[i].y = ygreki[rand() % 15];

				// Make sure y is divisible by 40 (this is already the case here)
				comets[i].y = (comets[i].y / 40) * 40;

				// Check if any other comet has the same x and y positions too close
				for (int j = 0; j < size; j++) {
					if (i != j && comets[j].live && comets[j].x == comets[i].x) {
						if (abs(comets[j].y - comets[i].y) < 40) {
							validPosition = false;
							comets[i].live = false;  // Reset live status if too close
							break;
						}
					}
				}
			}
		}
	}
}

// Update Comets Function
void UpdateComet(Comet comets[], int size) {
	for (int i = 0; i < size; i++) {
		if (comets[i].live) {
			if (bt == 1) comets[i].y++;
			else {
				comets[i].y += comets[i].speed;
			}

			/*if (comets[i].y + 20 == HEIGHT)
				comets[i].stop = true;*/
		}
	}
}

// Track the position of the explosion

void CollideBullet(Bullet bullet[], int bSize, Comet comets[], int cSize, ALLEGRO_SAMPLE_INSTANCE* explosionInstance, ALLEGRO_BITMAP* spriteSheet, float deltaTime) {
	for (int i = 0; i < cSize; i++) {
		if (comets[i].live) {
			for (int j = 0; j < bSize; j++) {
				if (bullet[j].live) {
					if ((bullet[j].x > (comets[i].x - comets[i].boundx) &&
						bullet[j].x < (comets[i].x + comets[i].boundx) &&
						bullet[j].y >(comets[i].y - comets[i].boundy) &&
						bullet[j].y < (comets[i].y + comets[i].boundy)))
					{
						bullet[j].live = false;
						comets[i].live = false;
						std::cout << j << ":" << bullet[j].live << std::endl;
						// Start a new explosion
						ActivateExplosion(bullet[j].x, bullet[j].y);

						al_stop_sample_instance(explosionInstance);
						al_set_sample_instance_gain(explosionInstance, 1);
						al_play_sample_instance(explosionInstance);
						break;
					}
				}
			}
		}
	}
}

void CollideComet(Comet comets[], int cSize, SpaceShip& ship, ALLEGRO_SAMPLE_INSTANCE* explosionInstance)
{
	//std::cout << ship.x << ":" << ship.y << std::endl;

	for (int i = 0; i < cSize; i++)
	{
		if (comets[i].live && !imm)
		{
			if (comets[i].x - comets[i].boundx < ship.x + ship.boundx &&
				comets[i].x + comets[i].boundx > ship.x - ship.boundx &&
				comets[i].y - comets[i].boundy < ship.y + ship.boundy &&
				comets[i].y + comets[i].boundy > ship.y - ship.boundy)
			{
				//std::cout << "Collision detected" << std::endl;

				// Stop the current explosion sound before playing a new one
				al_stop_sample_instance(explosionInstance);

				// Reduce ship's lives and set states
				ship.lives--;
				comets[i].live = false;
				ship.live = false;
				imm = true;

				// Play the explosion sound
				al_play_sample_instance(explosionInstance);
			}
			sec = 0;
		}
	}
}


