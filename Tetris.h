#pragma once
#include "Tetramino.h"

class Tetris
{
public:
	Tetris();
	static int field[FIELD_HEIGHT][FIELD_WIDTH];
	static int tetraOnField[FIELD_HEIGHT][FIELD_WIDTH];
	static float currentDelay;

private:
	Tetramino* tetramino[4];

	Clock clockGame;

	int gameTimer;
	bool inGame;
	int linesRemoved;
	int gameLevel;
	int activeTetramino = 1;

	Sound* sound, * soundStart;
	SoundBuffer* endGameSound, * tetraChooseSound, * lineClearedSound, * levelUpSound, * gameStartSound;

	Texture* textureBrick, * textureBackground, * textureFrame, * textureGOBoard;
	Sprite* spriteBrick, * spriteBackground, * spriteFrame, * spriteGOBoard;

	Font tetrisFont;
	Text* textWithTetrisFont;

	void initNewGame();
	void eventsHandling(RenderWindow& window);

	void checkCompletedLines();
	void deleteLine(int line);

	void drawBackgroundAndField(RenderWindow& window, Sprite& spriteBackground, Sprite& spriteBrick);

	void drawResults(RenderWindow& window, Sprite& spriteFrame);
	void drawGameOver(RenderWindow& window);

	void loadSounds();
	void loadTexturesAndFonts();
};