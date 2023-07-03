#include "Tetris.h"

int Tetris::field[FIELD_HEIGHT][FIELD_WIDTH];
int Tetris::tetraOnField[FIELD_HEIGHT][FIELD_WIDTH];
float Tetris::currentDelay = DELAY_INIT;


Tetris::Tetris()
{
	srand(time(0));
	ContextSettings settings;
	settings.antialiasingLevel = 8;
	RenderWindow window(VideoMode(1511, 850), "Triple Tetris", Style::Default, settings);

	auto image = Image{};
	image.loadFromFile("./gamedata/icon.png");
	window.setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());

	window.setFramerateLimit(100);

	loadTexturesAndFonts();
	loadSounds();

	// Creating tetraminos and init game
	for (int tet = 1; tet <= NUMBER_OF_TETRAMINOS; tet++)
		tetramino[tet] = new Tetramino(tet);

	initNewGame();
		

	while (window.isOpen())
	{
		eventsHandling(window);

		if (inGame)
		{
			checkCompletedLines();

			for (int tet = 1; tet <= NUMBER_OF_TETRAMINOS; tet++)
				if (!tetramino[tet]->moveDown())
				{
					sound->setBuffer(*endGameSound);
					sound->play();
					inGame = false;
					break;
				}
		}
		//  Draw  //
		drawBackgroundAndField(window, *spriteBackground, *spriteBrick);
		for (int tet = 1; tet < 4; tet++)
			tetramino[tet]->drawTetramino(window, *spriteBrick);
		window.draw(*spriteFrame);
		drawResults(window, *spriteFrame);
		if (!inGame) drawGameOver(window);

		window.display();
	}
}

void Tetris::initNewGame()
{
	soundStart->setBuffer(*gameStartSound);
	soundStart->play();

	tetramino[activeTetramino]->isActive = false;

	for (int i = 0; i < FIELD_HEIGHT; i++)
		for (int j = 0; j < FIELD_WIDTH; j++)
		{
			field[i][j] = 0;
			tetraOnField[i][j] = 0;
		}

	for (int tet = 1; tet <= NUMBER_OF_TETRAMINOS; tet++)
		tetramino[tet]->startOnTop();

	inGame = true;
	gameTimer = 0;
	linesRemoved = 0;
	gameLevel = 1;
	activeTetramino = 1;
	tetramino[activeTetramino]->isActive = true;
	clockGame.restart();
}


void Tetris::eventsHandling(RenderWindow& window)
{
	Event event;

	int oldActiveTetramino = activeTetramino;

	while (window.pollEvent(event))
	{
		if ((!inGame) && (event.key.code == Keyboard::Enter)) initNewGame();
		if (event.type == Event::Closed) window.close();
		if (event.type == Event::KeyPressed)
		{
			if (event.key.code == Keyboard::F10) window.close();
			if (event.key.code == Keyboard::Num1) activeTetramino = 1;
			if (event.key.code == Keyboard::Num2) activeTetramino = 2;
			if (event.key.code == Keyboard::Num3) activeTetramino = 3;
			if (event.key.code == Keyboard::Space)
			{
				activeTetramino++;
				if (activeTetramino > NUMBER_OF_TETRAMINOS) activeTetramino = 1;
			}
			if ((event.key.code == Keyboard::Up) || (event.key.code == Keyboard::W)) tetramino[activeTetramino]->rotate();
			if ((event.key.code == Keyboard::Left) || (event.key.code == Keyboard::A)) tetramino[activeTetramino]->sideMove(-1);
			if ((event.key.code == Keyboard::Right) || (event.key.code == Keyboard::D)) tetramino[activeTetramino]->sideMove(1);
			if ((event.key.code == Keyboard::Down) || (event.key.code == Keyboard::S)) tetramino[activeTetramino]->dropDown(true);
		}
		if (event.type == Event::KeyReleased)
			if ((event.key.code == Keyboard::Down) || (event.key.code == Keyboard::S)) tetramino[activeTetramino]->dropDown(false);
		if (Mouse::isButtonPressed(Mouse::Left))
		{
			Vector2i mousePos = Mouse::getPosition(window);
			for (int tet = 1; tet <= NUMBER_OF_TETRAMINOS; tet++)
				if (tetramino[tet]->checkMouseSelected(mousePos)) activeTetramino = tet;
		}
	}
	if (oldActiveTetramino != activeTetramino)
	{
		tetramino[oldActiveTetramino]->isActive = false;
		tetramino[activeTetramino]->isActive = true;
		sound->setBuffer(*tetraChooseSound);
		sound->play();
	}
}


void Tetris::checkCompletedLines()
{

	for (int line = FIELD_HEIGHT - 1; line > 0; line--)
	{
		int numBricks = 0;
		for (int col = 0; col < FIELD_WIDTH; col++)
			if (field[line][col]) numBricks++;

		if (numBricks == FIELD_WIDTH)
		{
			sound->setBuffer(*lineClearedSound); sound->play();
			deleteLine(line);
			linesRemoved++;
			if (gameLevel != (linesRemoved / LINES_TO_LEVEL_CHANGE + 1))
			{
				sound->setBuffer(*levelUpSound); sound->play();
				gameLevel++;
				currentDelay = DELAY_INIT * (SPEED_INCREMENT / gameLevel);
				for (int tet = 1; tet < 4; tet++) tetramino[tet]->delay = currentDelay;
			}
			//return;
		}
	}
}


void Tetris::deleteLine(int lineStart)
{
	for (int line = lineStart; line > 0; line--)
		for (int col = 0; col < FIELD_WIDTH; col++)
			field[line][col] = field[line - 1][col];
}


void Tetris::drawBackgroundAndField(RenderWindow& window, Sprite& spriteBackground, Sprite& spriteBrick)
{
	window.clear(Color::Black);
	window.draw(spriteBackground);

	for (int i = 0; i < FIELD_HEIGHT; i++)
		for (int j = 0; j < FIELD_WIDTH; j++)
		{
			if (field[i][j] == 0) continue;
			spriteBrick.setTextureRect(IntRect(field[i][j] * UNIT_SIZE, 0, UNIT_SIZE, UNIT_SIZE));
			spriteBrick.setPosition(j * UNIT_SIZE, i * UNIT_SIZE);
			spriteBrick.move(FIELD_OFFSET_X, FIELD_OFFSET_Y);
			window.draw(spriteBrick);
		}
}



void Tetris::drawResults(RenderWindow& window, Sprite& spriteFrame)
{
	Font font;
	font.loadFromFile("./gamedata/fonts/calibri.ttf");
	Text stext;
	stext.setFont(font);
	stext.setCharacterSize(40);
	stext.setStyle(Text::Bold);
	stext.setFillColor(Color::Red);

	stext.setPosition(217, 140);
	stext.setString(std::to_string(linesRemoved));
	window.draw(stext);

	stext.setPosition(217, 295);
	stext.setString(std::to_string(gameLevel));
	window.draw(stext);

	if (inGame)
	{
		gameTimer = clockGame.getElapsedTime().asSeconds();
		if (gameTimer < 3.0)
		{
			window.draw(*spriteGOBoard);
			textWithTetrisFont->setFillColor(Color::Green);
			textWithTetrisFont->setString("NEW");
			textWithTetrisFont->setPosition(550, 300);
			window.draw(*textWithTetrisFont);
			textWithTetrisFont->setPosition(500, 500);
			textWithTetrisFont->setString("GAME");
			window.draw(*textWithTetrisFont);
		}
	}

	String minsStr = std::to_string(gameTimer / 60);
	if (gameTimer / 60 < 10) minsStr = "0" + minsStr;
	String secStr = std::to_string(gameTimer % 60);
	if (gameTimer % 60 < 10) secStr = "0" + secStr;
	stext.setPosition(1225, 140);
	stext.setString(minsStr + " : " + secStr);
	window.draw(stext);

	if (activeTetramino == 1)	stext.setFillColor(Color::Green);
	if (activeTetramino == 2)	stext.setFillColor(Color::Yellow);
	if (activeTetramino == 3)	stext.setFillColor(Color::Blue);

	stext.setCharacterSize(60);
	stext.setPosition(1263, 280);
	stext.setString(std::to_string(activeTetramino));
	window.draw(stext);
}

void Tetris::drawGameOver(RenderWindow& window)
{
	window.draw(*spriteGOBoard);

	textWithTetrisFont->setFillColor(Color::Red);

	textWithTetrisFont->setString("GAME");
	textWithTetrisFont->setPosition(500, 300);
	window.draw(*textWithTetrisFont);
	textWithTetrisFont->setPosition(500, 500);
	textWithTetrisFont->setString("OVER");
	window.draw(*textWithTetrisFont);
}


void Tetris::loadTexturesAndFonts()
{
	textureBrick = new Texture;
	textureBackground = new Texture;
	textureFrame = new Texture;
	textureGOBoard = new Texture;

	textureBrick->loadFromFile("./gamedata/textures/tiles.png");
	textureBackground->loadFromFile("./gamedata/textures/background.png");
	textureFrame->loadFromFile("./gamedata/textures/frame.png");
	textureGOBoard->loadFromFile("./gamedata/textures/go_board.png");

	spriteBrick = new Sprite(*textureBrick);
	spriteBackground = new Sprite(*textureBackground);
	spriteFrame = new Sprite(*textureFrame);
	spriteGOBoard = new Sprite(*textureGOBoard);

	tetrisFont.loadFromFile("./gamedata/fonts/tetris_block.ttf");
	textWithTetrisFont = new Text("", tetrisFont, 90);
	textWithTetrisFont->setStyle(Text::Bold);
}


void Tetris::loadSounds()
{
	sound = new Sound;
	soundStart = new Sound;

	endGameSound = new SoundBuffer;
	tetraChooseSound = new SoundBuffer;
	lineClearedSound = new SoundBuffer;
	levelUpSound = new SoundBuffer;
	gameStartSound = new SoundBuffer;

	endGameSound->loadFromFile("./gamedata/sounds/ominous.wav");
	tetraChooseSound->loadFromFile("./gamedata/sounds/click_x.wav");
	lineClearedSound->loadFromFile("./gamedata/sounds/neon_light.wav");
	levelUpSound->loadFromFile("./gamedata/sounds/fanfare_x.wav");
	gameStartSound->loadFromFile("./gamedata/sounds/imperial_march.wav");
}
