#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <queue>
#include <string>

//=================================================================================================
// CALLBACKS
//=================================================================================================

//-----------------------------------------------------------------------------
// CALLBACK DOCUMENTATION
// https://www.opengl.org/resources/libraries/glut/spec3/node45.html
// http://freeglut.sourceforge.net/docs/api.php#WindowCallback
//-----------------------------------------------------------------------------

int points = 0;
bool gameOver = false;
int snakeSpeed = 95;
const int segmentSize = 30;

struct SnakeSegment {
	int x, y;
};


class Snake {
private:
	std::vector<SnakeSegment> segment;
	std::queue<char> directions;
	static const int segmentSize = 30;
	float r, g, b;
	char lastDirection;
public:
	Snake(int startX, int startY) : r(1.0f), g(1.0f), b(1.0f), lastDirection('d') {
		segment.push_back({ startX, startY });
	}

	~Snake() {}

	void reset() {
		segment.clear(); // Clear all segments
		segment.push_back({ 405, 315 }); // Reset to initial position

		directions = std::queue<char>(); // Clear direction queue
		lastDirection = 'd'; // Reset last direction
		snakeSpeed = 95; // Reset speed
		r = 1.0f;
		g = 1.0f;
		b = 1.0f;
	}

	void move() {
		char direction;

		if (!directions.empty()) {
			direction = directions.front();
			directions.pop();
			lastDirection = direction;
		}
		else {
			direction = lastDirection;
		}

		if (!segment.empty()) {
			int newX = segment.front().x;
			int newY = segment.front().y;

			switch (direction) {
			case 'w':
				newY += segmentSize;
				break;

			case 'a':
				newX -= segmentSize;
				break;
			case 's':
				newY -= segmentSize;
				break;

			case 'd':
				newX += segmentSize;
				break;
			}


			segment.insert(segment.begin(), { newX, newY });
			segment.pop_back();
		}

	}

	void grow() {
		int tailX = segment.back().x;
		int tailY = segment.back().y;

		switch (lastDirection) {
		case 'w':
			segment.push_back({ tailX, tailY + segmentSize });
			break;
		case 'a':
			segment.push_back({ tailX + segmentSize, tailY });
			break;
		case 's':
			segment.push_back({ tailX, tailY - segmentSize });
			break;
		case 'd':
			segment.push_back({ tailX - segmentSize, tailY });
			break;
		}
	}

	void setDirection(char newDirection) {
		if ((newDirection == 'w' && lastDirection != 's') || (newDirection == 'a' && lastDirection != 'd') || (newDirection == 's' && lastDirection != 'w')
			|| (newDirection == 'd' && lastDirection != 'a')) {
			directions.push(newDirection);
			lastDirection = newDirection;
		}
	}

	void changeColorToRandom() {
		r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

	}

	float getRed() const { return r; }
	float getGreen() const { return g; }
	float getBlue() const { return b; }

	std::vector<SnakeSegment> getSegments() const {
		return segment;
	}

};

Snake snake(405, 315);

void handleGameOver() {

	gameOver = true;
	snake.reset(); //Reset the snake because it was blocking the display screen
}

class GameObject {
public:
	virtual void draw() const = 0;
	virtual void foodEffect() const = 0;
	virtual ~GameObject() {}
};


class Food : public GameObject {
protected:
	int x, y;
	static const int segmentSize = 30;
public:
	Food() : x(0), y(0) {}
	~Food() {}
	virtual void placeRandom(int maxX, int maxY) {
		int cellX = rand() % (maxX / segmentSize);
		int cellY = rand() % (maxY / segmentSize);

		// Set food position to the center of the chosen cell
		x = cellX * segmentSize + segmentSize / 2;
		y = cellY * segmentSize + segmentSize / 2;
	}

	virtual void draw() const = 0;
	virtual void foodEffect() const = 0;
	virtual int getX() const { return x; }
	virtual int getY() const { return y; }
};


class Apple : public Food {
public:
	void draw() const override {
		const float size = 30.0f;
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_QUADS);
		glVertex2f(x - size / 2, y - size / 2);
		glVertex2f(x + size / 2, y - size / 2);
		glVertex2f(x + size / 2, y + size / 2);
		glVertex2f(x - size / 2, y + size / 2);
		glEnd();
	}

	void foodEffect() const override {
		points += 1;
		snakeSpeed -= 5;
		snake.grow();
	}
	~Apple() {}
};

class Orange : public Food {
public:
	void draw() const override {
		const float size = 30.0f;
		glColor3f(1.0f, 0.5f, 0.0f);
		glBegin(GL_QUADS);
		glVertex2f(x - size / 2, y - size / 2);
		glVertex2f(x + size / 2, y - size / 2);
		glVertex2f(x + size / 2, y + size / 2);
		glVertex2f(x - size / 2, y + size / 2);
		glEnd();
	}

	void foodEffect() const override {
		points += 1;
		snakeSpeed += 5;
		snake.grow();
	}
	~Orange() {}
};

class Grape : public Food {
public:
	void draw() const override {
		const float size = 30.0f;
		glColor3f(0.5f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
		glVertex2f(x - size / 2, y - size / 2);
		glVertex2f(x + size / 2, y - size / 2);
		glVertex2f(x + size / 2, y + size / 2);
		glVertex2f(x - size / 2, y + size / 2);
		glEnd();
	}

	void foodEffect() const override {
		snake.grow();
		points += 5;

	}
	Grape() {}
};


class Banana : public Food {
public:
	void draw() const override {
		const float size = 30.0f;
		glColor3f(1.0f, 1.0f, 0.0f);
		glBegin(GL_QUADS);
		glVertex2f(x - size / 2, y - size / 2);
		glVertex2f(x + size / 2, y - size / 2);
		glVertex2f(x + size / 2, y + size / 2);
		glVertex2f(x - size / 2, y + size / 2);
		glEnd();
	}

	void foodEffect() const override {
		points += 1;
		snake.changeColorToRandom();
		snake.grow();
	}
	~Banana() {}
};

Food* food = nullptr;


bool isCollision(int x1, int y1, int x2, int y2) {
	return x1 == x2 && y1 == y2;
}

void checkWallCollision() {
	//get position of the snakes head.
	int headX = snake.getSegments().front().x;
	int headY = snake.getSegments().front().y;

	//define game boundaries so the game knows when its out of bounds.
	int minX = 0;
	int maxX = 810;
	int minY = 0;
	int maxY = 600;

	//check if the head position exceeds boundaries.
	if (headX < minX || headX >= maxX || headY < minY || headY >= maxY) {
		handleGameOver();
	}
}

void checkSelfCollision() {
	// Get the position of the snake's head
	int headX = snake.getSegments().front().x;
	int headY = snake.getSegments().front().y;

	// Store the segments of the snake's body in a local variable
	const auto& segments = snake.getSegments();

	// Iterate through all segments of the snake's body except for the head
	for (auto it = std::next(segments.begin()); it != segments.end(); ++it) {
		// Check if the head collides with any other segment of the body
		if (headX == it->x && headY == it->y) {
			handleGameOver(); // Trigger game over if self-collision is detected
			return;
		}
	}
}

void update(int value) {

	if (!gameOver) {
		snake.move();
		checkWallCollision(); // added this line to check if the snake hit the wall.
		checkSelfCollision();

		if (isCollision(snake.getSegments().front().x, snake.getSegments().front().y,
			food->getX(), food->getY())) {
			food->foodEffect();

			delete food;

			switch (rand() % 4) {
			case 0:
				food = new Apple();
				food->placeRandom(810, 600);
				break;

			case 1:
				food = new Orange();
				food->placeRandom(810, 600);
				break;
			case 2:
				food = new Grape();
				food->placeRandom(810, 600);
				break;
			case 3:
				food = new Banana();
				food->placeRandom(810, 600);
				break;
			}
		}
	}

	glutPostRedisplay();
	glutTimerFunc(snakeSpeed, update, 0);
}

void reshape_func(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutPostRedisplay();
}

void restartGame(); // Function prototype declaration

void keyboard_func(unsigned char key, int x, int y)
{
	if (gameOver) {
		switch (key)
		{
		case 'r': // Restart the game when 'r' key is pressed
		{
			restartGame();
			break;
		}

		case 'e': // Exit the game when 'e' key is pressed
		{
			exit(EXIT_SUCCESS);
			break;
		}
		}
	}
	else {
		switch (key)
		{
		case 'w':
		{
			snake.setDirection('w');
			break;
		}

		case 'a':
		{
			snake.setDirection('a');
			break;
		}

		case 's':
		{
			snake.setDirection('s');
			break;
		}

		case 'd':
		{
			snake.setDirection('d');
			break;
		}
		}
	}

	glutPostRedisplay();
}

//=================================================================================================
// RENDERING
//=================================================================================================

void restartGame() {
	// Reset game state
	gameOver = false;
	points = 0;

}

void renderGameOverScreen() {

	// Render game over message
	glColor3f(1.0f, 0.0f, 0.0f);
	glRasterPos2i(330, 390);
	std::string gameOverMessage = "Game Over!";
	for (const char& c : gameOverMessage) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}

	// Render score
	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2i(350, 328);
	std::string scoreMessage = "Score: " + std::to_string(points);
	for (const char& c : scoreMessage) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}

	// Render restart text
	glColor3f(0.0f, 0.0f, 1.0f);
	glRasterPos2i(248, 270);
	std::string restartMessage = "Press R to Restart or E to exit";
	for (const char& c : restartMessage) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}
}


void display_func(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	if (!gameOver) {
		// grid
		glColor3f(0.0f, 0.0f, 0.0f);

		const float lineWidth = 2.0f;

		for (int x = 0; x <= 810; x += segmentSize) {
			glLineWidth(lineWidth);
			glBegin(GL_LINES);
			glVertex2f(static_cast<float>(x), 0.0f);
			glVertex2f(static_cast<float>(x), 600.0f);
			glEnd();

		}
		for (int y = 0; y <= 600; y += segmentSize) {
			glLineWidth(lineWidth);
			glBegin(GL_LINES);
			glVertex2f(0.0f, static_cast<float>(y));
			glVertex2f(810.0f, static_cast<float>(y));
			glEnd();

		}
		//draw snake
		const float size = 30.0f;
		glColor3f(snake.getRed(), snake.getGreen(), snake.getBlue()); //snake color (change)
		for (const auto& segment : snake.getSegments()) {
			glBegin(GL_QUADS);
			glVertex2f(segment.x - size / 2, segment.y - size / 2);
			glVertex2f(segment.x + size / 2, segment.y - size / 2);
			glVertex2f(segment.x + size / 2, segment.y + size / 2);
			glVertex2f(segment.x - size / 2, segment.y + size / 2);
			glEnd();
		}
	}
	else {
		// Render the game over screen
		renderGameOverScreen();
	}


	if (!gameOver && food) {
		food->draw();
	}

	glutSwapBuffers();
}

//=================================================================================================
// INIT
//=================================================================================================

void init(void)
{
	// Print some info
	std::cout << "Vendor:         " << glGetString(GL_VENDOR) << "\n";
	std::cout << "Renderer:       " << glGetString(GL_RENDERER) << "\n";
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n\n";

	// Set the background color (red, green, blue, alpha)
	glClearColor(0.3f, 0.5f, 0.2f, 0.5f);

	if (food) {
		delete food;
	}

	switch (rand() % 4) {
	case 0:
		food = new Apple();
		food->placeRandom(810, 600);
		break;
	case 1:
		food = new Orange();
		food->placeRandom(810, 600);
		break;
	case 2:
		food = new Grape();
		food->placeRandom(810, 600);
		break;
	case 3:
		food = new Banana();
		food->placeRandom(810, 600);
		break;
	}

	std::cout << "Finished initializing...\n\n";
}

//=================================================================================================
// MAIN
//=================================================================================================

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(810, 600);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutCreateWindow("Snake");

	glutDisplayFunc(display_func);
	glutReshapeFunc(reshape_func);
	glutKeyboardFunc(keyboard_func);

	init();

	glutTimerFunc(snakeSpeed, update, 0);
	glutMainLoop();

	return EXIT_SUCCESS;
}