/* 2021 */

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <iomanip>

#define UUID(x, y)	((x) + NUM_COLS * (y))

struct Cell
{
	bool bIsAlive = false;
	bool bMustChange = false;
};

const sf::Vector2u WINDOW_SIZE = { 1280, 720 };

unsigned int NUM_COLS;
unsigned int NUM_ROWS;
unsigned int SCALE;

Cell* cell;

sf::Event   event;
sf::Mouse   mouse;

sf::RectangleShape rect;

sf::Clock   game_clock;
sf::Clock   elapsed_clock;
sf::Time    t;
sf::Time	wt;

std::string title	= "Game Of Life  -  VirtualExeption";
std::string dbgtxt	= "null";
sf::Vector2f MousePoint;
sf::View view;
bool bIsRunning	= true;

int     nh      = 0;
int     uuid    = 0;
int		cuuid	= 0;
double	TPS     = 10;
double  zoom	= 1;
double	crnt_z	= 1;
int		LCells	= 0;

void Reset(sf::RenderWindow *window);
void Update();
void Events(sf::RenderWindow *window);
std::string* Split(std::string string);


std::string* Split(std::string string)
{
	std::string* words = new std::string[16];
	size_t w = 0;
	for (size_t i = 0; i <= string.length(); i++)
	{
		if (string[i] == ' ') w++;
		else words[w] += string[i];
	}
	return words;
}

void Reset(sf::RenderWindow *window)
{
	for (size_t i = 0; i < NUM_COLS * NUM_ROWS; i++)
	{
		cell[i].bIsAlive = 0;
		cell[i].bMustChange = 0;
	}

	view = window->getDefaultView();
	crnt_z = 1;

	TPS = 10;
}

void Events(sf::RenderWindow *window)
{
	// Events
	while (window->pollEvent(event))
	{
		switch (event.type)
		{
		// Window "X" is pressed
		case sf::Event::Closed:
			Reset(window);
			window->close();
			break;

		// Keyboard keys
		case sf::Event::KeyPressed:
			switch (event.key.code)
			{
			case sf::Keyboard::Space:
				// Pauses the updating
				bIsRunning = !bIsRunning;
				break;

			case sf::Keyboard::Delete:
				Reset(window);
				break;

			case sf::Keyboard::Up:
				TPS+= 1;
				game_clock.restart();
				elapsed_clock.restart();
				break;

			case sf::Keyboard::Down:
				TPS-= 1;
				game_clock.restart();
				elapsed_clock.restart();
				break;

			default:
				break;
			}
			break;

		// Mouse Wheel
		case sf::Event::MouseWheelMoved:
			zoom = 1 - double(event.mouseWheel.delta)/10;
			crnt_z += (1-zoom);
			view.zoom(zoom);
			break;

		// Window resized
		case sf::Event::Resized:
			//WINDOW_SIZE.x = window->getSize().x;
			//WINDOW_SIZE.y = window->getSize().x * 0.5625;
			window->setSize(sf::Vector2u(window->getSize().x, window->getSize().x * 0.5625));
			break;

		default:
			break;
		}
	}

	MousePoint = window->mapPixelToCoords(mouse.getPosition(*window));

	// Event-extern mouse handle
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		cuuid = UUID(	int(MousePoint.x) / SCALE,
						int(MousePoint.y) / SCALE);
		// Make the cell alive
		if (cuuid <= NUM_COLS * NUM_ROWS)
		{
			cell[cuuid].bIsAlive = true;
		}
	}
	else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
	{
		cuuid = UUID(	int(MousePoint.x) / SCALE,
						int(MousePoint.y) / SCALE);
		// Make the cell alive
		if (cuuid <= NUM_COLS * NUM_ROWS)
		{
			cell[cuuid].bIsAlive = false;
		}
	}

	// Event-extern key handle
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
	{
		view.move(1 + view.getSize().x * 0.008f, 0);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
	{
		view.move(-1 - view.getSize().x * 0.008f, 0);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
	{
		view.move(0, 1 + view.getSize().x * 0.008f);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
	{
		view.move(0, -1 - view.getSize().x * 0.008f);
	}

	return;

}

void Update()
{
	LCells = 0;

	//Change state of cells
	for (size_t x = 1; x < NUM_COLS-1; x++)
	{
		for (size_t y = 1; y < NUM_ROWS-1; y++)
		{

			nh =  ( cell[UUID(x - 1, y)].bIsAlive       + cell[UUID(x + 1, y)].bIsAlive +
					cell[UUID(x, y - 1)].bIsAlive       + cell[UUID(x, y + 1)].bIsAlive +
					cell[UUID(x + 1, y + 1)].bIsAlive   + cell[UUID(x - 1, y - 1)].bIsAlive +
					cell[UUID(x - 1, y + 1)].bIsAlive   + cell[UUID(x + 1, y - 1)].bIsAlive );
	
			uuid = UUID(x, y);
	
			if (cell[uuid].bIsAlive)
			{
				if (nh < 2)
				{
					// Any live cell with fewer than two live neighbours dies, as if by underpopulation
					cell[uuid].bMustChange = true;
				}
				else if (nh == 2 || nh == 3)
				{
					// Any live cell with two or three live neighbours lives on to the next generation
					cell[uuid].bMustChange = false;
				}
				else if (nh > 3)
				{
					// Any live cell with more than three live neighbours dies, as if by overpopulation
					cell[uuid].bMustChange = true;
				}

				LCells++;
			}
			else if (nh == 3)
			{
				// Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
				cell[uuid].bMustChange = true;
			}

		}
	}
	
	// Update cells
	for (size_t i = NUM_COLS; i < NUM_COLS * NUM_ROWS; i++)
	{
		if (cell[i].bMustChange)
		{
			cell[i].bIsAlive = !cell[i].bIsAlive;
		}
		cell[i].bMustChange = false;
	}
}

// #Main method#

int main()
{
	//std::string* args = Split(lpCmdLine);
	//sf::VideoMode::getFullscreenModes()[0]

	sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE.x, WINDOW_SIZE.y),
		title,
		sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize );
	window.setFramerateLimit(240);

	SCALE = 8;

	// Cells array
	NUM_COLS = WINDOW_SIZE.x / SCALE * 5;
	NUM_ROWS = WINDOW_SIZE.y / SCALE * 5;
	cell = new Cell[NUM_COLS * NUM_ROWS];

	// Cell array pixel size
	sf::Vector2f world;
	world.x = NUM_COLS * SCALE;
	world.y = NUM_ROWS * SCALE;

	// Shapes
	rect.setFillColor(sf::Color(210, 210, 210));
	rect.setSize({ 8, 8 });

	// Stringstream variable for bIsRunning
	std::stringstream strIsRuning;
	// Stringstream variable for crnt_z
	std::stringstream strcrnt_z;

	// Outline of the world
	sf::RectangleShape outline;
	outline.setPosition(0, 0);
	outline.setSize(world);
	outline.setOutlineThickness(2);
	outline.setOutlineColor(sf::Color(255, 0, 0));
	outline.setFillColor(sf::Color(0, 0, 0, 0));

	// View-related
	view = window.getDefaultView();
	view.setSize(WINDOW_SIZE.x, WINDOW_SIZE.y);
	sf::View text_view = window.getDefaultView();

	// Text on screen
	sf::Text text;
	sf::Font font;
	if (font.loadFromFile("./consola.ttf"))
	{
		text.setFont(font);
		text.setCharacterSize(20);
		text.setPosition(10, 10);
		text.setString(dbgtxt);
	}
	else font.~Font();

	// Icon of window
	sf::Image icon;
	if (icon.loadFromFile("./icon"))
	{
		window.setIcon(128, 128, icon.getPixelsPtr());
	}
	else icon.~Image();

	//GAME LOOP
	while (window.isOpen())
	{
		// Clear to cyan
		window.clear(sf::Color(25, 25, 25));

		// Function that handles all events
		Events(&window);

		// Draw every cell with the UUID in coords
		for (size_t x = 0; x < NUM_COLS; x++)
		{
			for (size_t y = 0; y < NUM_ROWS; y++)
			{	
				if (cell[UUID(x, y)].bIsAlive)
				{
					rect.setPosition(x * SCALE, y * SCALE);
					window.draw(rect);
				}
			}
		}

		// Get elapsed time
		t = game_clock.getElapsedTime();
		wt = elapsed_clock.getElapsedTime();

		// Change debug text
		strcrnt_z.str("");
		strcrnt_z << std::fixed <<  std::setprecision(1) << crnt_z;

		strIsRuning.str("");
		strIsRuning << std::boolalpha << bool(!bIsRunning);
		dbgtxt =	"FPS: "			+ std::to_string(int(1 / wt.asSeconds())) + " (240fps)" +
					"\nTPS: "		+ std::to_string(int(TPS)) +
					"\nN Cells: "	+ std::to_string(LCells) +
					"\ncoords: "	+ std::to_string(int(MousePoint.x/SCALE)) +
							   ", " + std::to_string(int(MousePoint.y/SCALE)) + 
					"\nzoom: "		+ strcrnt_z.str() + "t" +
					"\npaused: "	+ strIsRuning.str();

		text.setString(dbgtxt);

		// Tick-rate update
		if (bIsRunning && t.asSeconds() >= 1/TPS)
		{
			// Update game cells
			Update();
			// Restarting counter
			game_clock.restart();
		}

		elapsed_clock.restart();

		outline.setOutlineThickness(2 + (view.getSize().x) * 0.002f);
		window.draw(outline);

		window.setView(text_view);
		window.draw(text);
		window.setView(view);

		window.display();
	}

	return 0;
}
