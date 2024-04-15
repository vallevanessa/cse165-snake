#include <GL/freeglut.h>
#include <iostream>
#include <vector>

//=================================================================================================
// CALLBACKS
//=================================================================================================

//-----------------------------------------------------------------------------
// CALLBACK DOCUMENTATION
// https://www.opengl.org/resources/libraries/glut/spec3/node45.html
// http://freeglut.sourceforge.net/docs/api.php#WindowCallback
//-----------------------------------------------------------------------------

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
			|| (newDirection == 'd' && direction != 'd')) {
			direction = newDirection;
		}
	}

	std::vector<SnakeSegment> getSegments() const {
		return segment;
	}

};



Snake snake(400, 300);

const int snakeSpeed = 20;
void update(int value) {
	snake.move();
	glutPostRedisplay();
	glutTimerFunc(snakeSpeed, update, 0);
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
			exit( EXIT_SUCCESS );
			break;
		}
	}

	snake.move();

	glutPostRedisplay();
}

void key_released( unsigned char key, int x, int y )
{
}

void key_special_pressed( int key, int x, int y )
{
}

void key_special_released( int key, int x, int y )
{
}

void mouse_func( int button, int state, int x, int y )
{
}

void passive_motion_func( int x, int y )
{
}

void active_motion_func( int x, int y )
{
}

//=================================================================================================
// RENDERING
//=================================================================================================

void display_func( void )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity();

	//draw snake
	const float size = 10.0f;
	glColor3f(1.0f, 1.0f, 1.0f); //snake color (change)
	for (const auto& segment : snake.getSegments()) {
		glBegin(GL_QUADS);
		glVertex2f(segment.x - size/2, segment.y - size/2);
		glVertex2f(segment.x + size/2, segment.y - size/2);
		glVertex2f(segment.x + size/2, segment.y + size/2);
		glVertex2f(segment.x - size/2, segment.y + size/2);
		glEnd();
	}
	glutSwapBuffers();
}

//=================================================================================================
// INIT
//=================================================================================================

void init( void )
{
	// Print some info
	std::cout << "Vendor:         " << glGetString( GL_VENDOR   ) << "\n";
	std::cout << "Renderer:       " << glGetString( GL_RENDERER ) << "\n";
	std::cout << "OpenGL Version: " << glGetString( GL_VERSION  ) << "\n\n";

	// Set the background color (red, green, blue, alpha)
	glClearColor( 0.3f, 0.5f, 0.2f, 0.5f );

	std::cout << "Finished initializing...\n\n";
}

//=================================================================================================
// MAIN
//=================================================================================================

int main( int argc, char** argv )
{
	glutInit( &argc, argv );

	glutInitWindowPosition( 100, 100 );
	glutInitWindowSize( 800, 600 );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );

	glutCreateWindow( "Snake" );

	glutDisplayFunc( display_func );
	glutIdleFunc( idle_func );
	glutReshapeFunc( reshape_func );
	glutKeyboardFunc( keyboard_func );
	glutKeyboardUpFunc( key_released );
	glutSpecialFunc( key_special_pressed );
	glutSpecialUpFunc( key_special_released );
	glutMouseFunc( mouse_func );
	glutMotionFunc( active_motion_func );
	glutPassiveMotionFunc( passive_motion_func );

	init();

	glutTimerFunc(snakeSpeed, update, 0);
	glutMainLoop();

	return EXIT_SUCCESS;
}
