#include <GL/freeglut.h>
//#include <external/GL/freeglut.h>
//#include <../external/freeglut/include/GL/freeglut.h>
#include <iostream>
#include <vector>
#include <cstdlib>

//=================================================================================================
// CALLBACKS
//=================================================================================================

//-----------------------------------------------------------------------------
// CALLBACK DOCUMENTATION
// https://www.opengl.org/resources/libraries/glut/spec3/node45.html
// http://freeglut.sourceforge.net/docs/api.php#WindowCallback
//-----------------------------------------------------------------------------



#include <cstdlib> // for exit()

void gameOver() {
	// Stop the game loop (if using glut, you may need to use glutIdleFunc(nullptr))
	// Example: glutIdleFunc(nullptr);

	// Display game over message
	std::cout << "Game Over! Press any key to restart.\n";

	// Optionally reset the game state (you may need to implement this)
	// Example: resetGameState();

	// Exit the program (if using glut, you may need to use glutLeaveMainLoop())
	exit(EXIT_SUCCESS);
}

struct SnakeSegment {
	int x, y;
};

class Snake {
private:
	std::vector<SnakeSegment> segment;
	char direction;
public:
	Snake(int startX, int startY) {
		segment.push_back({ startX, startY });
		direction = 'd';
	}
	void move() {
		int newX = segment.front().x;
		int newY = segment.front().y;

		switch (direction) {
		case 'w':
			newY++;
			break;

		case 'a':
			newX--;
			break;
		case 's':
			newY--;
			break;

		case 'd':
			newX++;
			break;
		}

		segment.pop_back();
		segment.insert(segment.begin(), { newX, newY });
	}

	void setDirection(char newDirection) {
		if ((newDirection == 'w' && direction != 's') || (newDirection == 'a' && direction != 'd') || (newDirection == 's' && direction != 'w')
			|| (newDirection == 'd' && direction != 'a')) {
			direction = newDirection;
		}
	}

	std::vector<SnakeSegment> getSegments() const {
		return segment;
	}

};



Snake snake(400, 300);

const int snakeSpeed = 15;

class GameObject {
public:
	virtual void draw() const = 0;
	virtual void playSound() const = 0;
};


class Food : public GameObject {
protected:
	int x, y;
public: 
	Food() : x(0), y(0) {}
	virtual void placeRandom(int maxX, int maxY) {
		x = rand() % maxX;
		y = rand() % maxY;
	}

	virtual void draw() const = 0;
	virtual void playSound() const = 0;
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

	void playSound() const override {
		std::cout << "Eating apple sound \n";  // to be replaced!!
	}
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

	void playSound() const override {
		std::cout << "Eating orange sound \n";  // to be replaced!!
	}
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

	void playSound() const override {
		std::cout << "Eating grape sound \n";  // to be replaced!!
	}
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

	void playSound() const override {
		std::cout << "Eating banana sound \n";  // to be replaced!!
	}
};

Food* food;

void checkWallCollision();


void update(int value) {
	snake.move();
	checkWallCollision(); // added this line to check if the snake hit the wall.

	if (snake.getSegments().front().x == food->getX() && snake.getSegments().front().y == food->getY()) {
		food->playSound();

		snake.getSegments().push_back(snake.getSegments().back());

		delete food;

		switch (rand() % 4) {
		case 0:
			food = new Apple();
			break;

		case 1:
			food = new Orange();
			break;
		case 2: 
			food = new Grape();
			break;
		case 3: 
			food = new Banana();
			break;
		}
		food->placeRandom(800, 600);
	}

	glutPostRedisplay();
	glutTimerFunc(snakeSpeed, update, 0);
}

//the game doesnt end when the tip of the snake hits the wall currently, it ends when the middle of the snake hits. at least that what its looks like to me. I will think about a solution to that for now but i think it looks good anyways.
void checkWallCollision() {
	//get position of the snakes head.
	int headX = snake.getSegments().front().x;
	int headY = snake.getSegments().front().y;

	//define game boundaries so the game knows when its out of bounds.
	int minX = 0;
	int maxX = 800;
	int minY = 0;
	int maxY = 600;

	//check if the head position exceeds boundaries.
	if (headX < minX || headX >= maxX || headY < minY || headY >= maxY) {
		gameOver();
	}
}

void idle_func()
{
	//snake.move();
	//uncomment below to repeatedly draw new frames
	//glutPostRedisplay();
}
 
void reshape_func( int width, int height )
{
	glViewport( 0, 0, width, height );
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutPostRedisplay();
}

void keyboard_func( unsigned char key, int x, int y )
{
	switch( key )
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

	// Exit on escape key press
	case '\x1B':
	{
		exit(EXIT_SUCCESS);
		break;
	}
	}

	snake.move();

	glutPostRedisplay();
}

void key_released(unsigned char key, int x, int y)
{
}

void key_special_pressed(int key, int x, int y)
{
}

void key_special_released(int key, int x, int y)
{
}

void mouse_func(int button, int state, int x, int y)
{
}

void passive_motion_func(int x, int y)
{
}

void active_motion_func(int x, int y)
{
}

//=================================================================================================
// RENDERING
//=================================================================================================

void display_func(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	food->draw();

	//draw snake
	const float size = 30.0f;
	glColor3f(1.0f, 1.0f, 1.0f); //snake color (change)
	for (const auto& segment : snake.getSegments()) {
		glBegin(GL_QUADS);
		glVertex2f(segment.x - size / 2, segment.y - size / 2);
		glVertex2f(segment.x + size / 2, segment.y - size / 2);
		glVertex2f(segment.x + size / 2, segment.y + size / 2);
		glVertex2f(segment.x - size / 2, segment.y + size / 2);
		glEnd();
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

	switch (rand() % 4) {
	case 0:
		food = new Apple();
		break;
	case 1:
		food = new Orange();
		break;
	case 2:
		food = new Grape();
		break;
	case 3: 
		food = new Banana();
		break;
	}

	food->placeRandom(800, 600);

	std::cout << "Finished initializing...\n\n";
}

//=================================================================================================
// MAIN
//=================================================================================================

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutCreateWindow("Snake");

	glutDisplayFunc(display_func);
	glutIdleFunc(idle_func);
	glutReshapeFunc(reshape_func);
	glutKeyboardFunc(keyboard_func);
	glutKeyboardUpFunc(key_released);
	glutSpecialFunc(key_special_pressed);
	glutSpecialUpFunc(key_special_released);
	glutMouseFunc(mouse_func);
	glutMotionFunc(active_motion_func);
	glutPassiveMotionFunc(passive_motion_func);

	init();

	glutTimerFunc(snakeSpeed, update, 0);
	glutMainLoop();

	return EXIT_SUCCESS;
}