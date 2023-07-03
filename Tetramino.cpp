#include "Tetramino.h"
#include "Tetris.h"
#include <random>


Tetramino::Tetramino(int num)
{
	colorNum = num;
	if (num == 1) startPosX = 3;
	if (num == 2) startPosX = 9;
	if (num == 3) startPosX = 15;
}



bool Tetramino::startOnTop()
{
	shape = rand() % 7;
	for (int i = 0; i < NUM_BLOCKS; i++)
	{
		tetBlock[i].x = figures[shape][i] % 2 + startPosX;
		tetBlock[i].y = figures[shape][i] / 2;

		if ((Tetris::field[tetBlock[i].y][tetBlock[i].x]) != 0)
			return false; // game over
	}

	delay = Tetris::currentDelay;
	timer = 0;
	isCanMove = true;
	clock.restart();
	return true;
}

void Tetramino::sideMove(int direction)
{
	saveCurrentPosition();

	for (int i = 0; i < NUM_BLOCKS; i++)
	{
		tetBlock[i].x += direction; // -1 left, 1 right
	}

	if (!checkInField()) restoreCurrentPosition();	
}


void Tetramino::rotate()
{
	if (shape == 6) return; // do not rotate if shape "square" 

	saveCurrentPosition();

	int pX = tetBlock[1].x; // center of the roration
	int pY = tetBlock[1].y;

	for (int i = 0; i < NUM_BLOCKS; i++)
	{
		int x = tetBlock[i].y - pY; //y-y0
		int y = tetBlock[i].x - pX; //x-x0
		tetBlock[i].x = pX - x;
		tetBlock[i].y = pY + y;
	}

	if (!checkInField()) restoreCurrentPosition();
}



void Tetramino::dropDown(bool isDrop)
{
	if (isDrop) delay = DELAY_DROP;
	else delay = Tetris::currentDelay;
}


void Tetramino::drawTetramino(RenderWindow& window, Sprite& spriteBrick)
{
	int selectFlag = isActive;
	for (int i = 0; i < NUM_BLOCKS; i++)
	{
		spriteBrick.setTextureRect(IntRect(colorNum * UNIT_SIZE, selectFlag * UNIT_SIZE, UNIT_SIZE, UNIT_SIZE));
		spriteBrick.setPosition(tetBlock[i].x * UNIT_SIZE, tetBlock[i].y * UNIT_SIZE);
		spriteBrick.move(FIELD_OFFSET_X, FIELD_OFFSET_Y);

		window.draw(spriteBrick);
	}
}


bool Tetramino::moveDown()
{
	timer += clock.getElapsedTime().asSeconds();
	clock.restart();

	if (timer > delay)
	{
		saveCurrentPosition();

		for (int i = 0; i < NUM_BLOCKS; i++)
		{
			tetBlock[i].y += 1;
		}
		if (!checkInField())
		{
			restoreCurrentPosition();
			if (!isCanMove)
			{
				for (int i = 0; i < NUM_BLOCKS; i++)
				{
					Tetris::tetraOnField[tetBlock[i].y][tetBlock[i].x] = 0;
					Tetris::field[tetBlock[i].y][tetBlock[i].x] = colorNum;
				}
				bool isCanMoveOnStart = startOnTop();
				return isCanMoveOnStart;
			}
		}
		timer = 0;
	}
	return true;
}



bool Tetramino::checkInField()
{
	for (int i = 0; i < NUM_BLOCKS; i++)
	{
		if (tetBlock[i].x < 0 || tetBlock[i].x >= FIELD_WIDTH)
			return false;
		if (tetBlock[i].y >= FIELD_HEIGHT)
		{
			isCanMove = false;
			return false;
		}
		else if ((Tetris::field[tetBlock[i].y][tetBlock[i].x]) != 0)// if gamefiel cell is not empty
		{
			isCanMove = false;
			return false;
		}
		else if ((Tetris::tetraOnField[tetBlock[i].y][tetBlock[i].x] != 0) && (Tetris::tetraOnField[tetBlock[i].y][tetBlock[i].x] != colorNum))
		{
			return false;
		}
	}

	for (int i = 0; i < NUM_BLOCKS; i++)
	{
		Tetris::tetraOnField[tetBlock[i].y][tetBlock[i].x] = colorNum;
	}

	return true;
};

void Tetramino::saveCurrentPosition()
{
	for (int i = 0; i < NUM_BLOCKS; i++)
	{
		tetBlockSave[i].x = tetBlock[i].x;
		tetBlockSave[i].y = tetBlock[i].y;

		Tetris::tetraOnField[tetBlock[i].y][tetBlock[i].x] = 0;
	}
}

void Tetramino::restoreCurrentPosition()
{
	for (int i = 0; i < NUM_BLOCKS; i++)
	{
		tetBlock[i].x = tetBlockSave[i].x;
		tetBlock[i].y = tetBlockSave[i].y;

		Tetris::tetraOnField[tetBlock[i].y][tetBlock[i].x] = colorNum;
	}
}

bool Tetramino::checkMouseSelected(Vector2i mousePos)
{
	// convert position to field coordinates
	mousePos.x = (mousePos.x - FIELD_OFFSET_X) / UNIT_SIZE;
	mousePos.y = (mousePos.y - FIELD_OFFSET_Y) / UNIT_SIZE;
	// check every brick in tetramino	
	for (int i = 0; i < NUM_BLOCKS; i++)
		if ((tetBlock[i].x == mousePos.x) && tetBlock[i].y == mousePos.y)
			return true;

	return false;
}
