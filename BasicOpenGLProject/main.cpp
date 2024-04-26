#include <GL/freeglut.h>
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


void gameOver() {

	// Display game over message
	std::cout << "Game Over! Press any key to restart.\n";

	// Optionally reset the game state (you may need to implement this)
	// Example: resetGameState();

	exit(EXIT_SUCCESS);
}

struct SnakeSegment {
	int x, y;
};

class Snake {
private:
	std::vector<SnakeSegment> segment;
	char direction;
	static const int segmentSize = 30;
	float r, g, b;
public:
	Snake(int startX, int startY) : r(1.0f), g(1.0f), b(1.0f){
		segment.push_back({ startX, startY });
		direction = 'd';
	}

	~Snake(){}

	void move() {
		int newX = segment.front().x;
		int newY = segment.front().y;

		switch (direction) {
		case 'w':
			newY+= segmentSize;
			break;

		case 'a':
			newX-= segmentSize;
			break;
		case 's':
			newY-= segmentSize;
			break;

		case 'd':
			newX+= segmentSize;
			break;
		}

		segment.insert(segment.begin(), { newX, newY });
		segment.pop_back();
	}

	void grow() {
		int tailX = segment.back().x;
		int tailY = segment.back().y;

		switch (direction) {
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
		if ((newDirection == 'w' && direction != 's') || (newDirection == 'a' && direction != 'd') || (newDirection == 's' && direction != 'w')
			|| (newDirection == 'd' && direction != 'a')) {
			direction = newDirection;
		}
	}

	void changeColorToRandom() {
		 r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		 g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		 b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

	}

	float getRed() const { return r; }
	float getGreen() const { return g; }
	float getBlue() const { return b;  }

	std::vector<SnakeSegment> getSegments() const {
		return segment;
	}

};



Snake snake(405, 315);

int snakeSpeed = 95;

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
	~Food(){}
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
		snakeSpeed -= 5;
		snake.grow();
	}
	~Apple(){}
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
		snakeSpeed += 5;
		snake.grow();
	}
	~Orange(){}
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
		snake.grow();
		snake.grow();
	}
	Grape(){}
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
		snake.changeColorToRandom();
		snake.grow();
	}
	~Banana(){}
};

Food* food = nullptr;

const int segmentSize = 30;

bool isCollision(int x1, int y1, int x2, int y2) {
	return x1 == x2 && y1 == y2;
}

void checkWallCollision();

void update(int value) {
	snake.move();
	checkWallCollision(); // added this line to check if the snake hit the wall.

	if (isCollision(snake.getSegments().front().x, snake.getSegments().front().y,
		food->getX(), food->getY())) {
		food->foodEffect();
		
	//	snake.grow();
		
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

	//snake.move();

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

	//grid
	glColor3f(0.0f, 0.0f, 0.0f);

	const float lineWidth = 2.0f;

	for (int x = 0; x <= 800; x += segmentSize) {
		glLineWidth(lineWidth); 
		glBegin(GL_LINES);
		glVertex2f(x, 0);
		glVertex2f(x, 600);
		glEnd();

	}
	for (int y = 0; y <= 600; y += segmentSize) {
		glLineWidth(lineWidth);
		glBegin(GL_LINES);
		glVertex2f(0, y);
		glVertex2f(800, y);
		glEnd();

	}


	if (food) {
		food->draw();
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