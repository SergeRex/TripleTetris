#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;

const int NUMBER_OF_TETRAMINOS = 3;  // triple tetris
const int FIELD_HEIGHT = 22;		// gamefield height, bricks
const int FIELD_WIDTH = 20;			// gamefield width, bricks 
const int UNIT_SIZE = 35;			// size of the brick, px

const int NUM_BLOCKS = 4;			// num elements of a tetramino

const float DELAY_DROP = 0.1;		 // speed of tetramino dropping
const float DELAY_INIT = 1.0;		 // start speed of the game
const int LINES_TO_LEVEL_CHANGE = 3; // num lines to clear for level up
const float SPEED_INCREMENT = 1.5;   // speed increasing for level up, 1.5 = 25%

const int FIELD_OFFSET_X = 403;		 // adjusting of the field origin to the design
const int FIELD_OFFSET_Y = 40;


const int figures[7][NUM_BLOCKS] =    // array of all tetramino's shapes 
{
	1,3,5,7, // I
	2,4,5,7, // Z
	3,5,4,6, // S
	3,5,4,7, // T
	2,3,5,7, // L
	3,5,7,6, // J
	2,3,4,5, // O
};


class Tetramino
{
public:
	Tetramino(int num);

	bool isActive = false;
	bool isCanMove;
	float delay;

	bool startOnTop();
	void sideMove(int direction);
	void rotate();
	bool moveDown();
	void dropDown(bool isDrop);

	void drawTetramino(RenderWindow& window, Sprite& spriteBrick);

	bool checkMouseSelected(Vector2i mousePos);


private:
	int colorNum;
	int shape;
	int startPosX;

	Vector2i tetBlock[NUM_BLOCKS];
	Vector2i tetBlockSave[NUM_BLOCKS];

	float timer;
	Clock clock;

	void saveCurrentPosition();
	void restoreCurrentPosition();
	bool checkInField();
	
};
