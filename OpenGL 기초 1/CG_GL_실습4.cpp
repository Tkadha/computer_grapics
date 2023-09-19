#include <iostream>
#include <gl/glew.h> // �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>
#define MAXX 800
#define MAXY 600


GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Mouse(int, int, int, int);
GLvoid Keyboard(unsigned char key, int x, int y);
void Move_diag(int);
void Move_zigzag(int);
void Change_color(int );
class RGB {
public:
	GLclampf r;
	GLclampf g;
	GLclampf b;
};
class Rect {
public:
	GLfloat x1;
	GLfloat y1;
	GLfloat x2;
	GLfloat y2;
};
RGB rgb[5];
Rect rect[5];
Rect first_rect[5];
int viewRect;
GLfloat x_add[5];
GLfloat y_add[5];
bool a_on;
bool i_on;
bool o_on;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dist(0, 1);


void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ���� 
{ //--- ������ �����ϱ�
	for (int i = 0; i < 5; ++i) {
		rgb[i].r = dist(gen);
		rgb[i].g = dist(gen);
		rgb[i].b = dist(gen);
		x_add[i] = 0.01f;
		y_add[i] = 0.01f;
	}
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
	glutInitWindowPosition(0, 0); // �������� ��ġ ����
	glutInitWindowSize(MAXX, MAXY); // �������� ũ�� ����
	glutCreateWindow("Example1"); // ������ ����(������ �̸�)
	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew �ʱ�ȭ
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";
	glutDisplayFunc(drawScene); // ��� �Լ��� ����
	glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMainLoop(); // �̺�Ʈ ó�� ���� 
}
GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ� 
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	// �׸��� �κ� ����
	for (int i = 0; i < viewRect; ++i) {
		glColor3f(rgb[i].r, rgb[i].g, rgb[i].b);
		glRectf(rect[i].x1, rect[i].y1, rect[i].x2, rect[i].y2);
	}
	//--- �׸��� ���� �κ��� ���⿡ ���Եȴ�.
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ� 
{
	glViewport(0, 0, w, h);
}


void Mouse(int button, int state, int x, int y)
{
	GLfloat halfx = MAXX / 2;
	GLfloat halfy = MAXY / 2;
	GLclampf mousex = (x - halfx) / halfx;
	GLclampf mousey = 1 - (y / halfy);
	if (state == GLUT_DOWN)
	{
		if ((button == GLUT_LEFT_BUTTON) || (button == GLUT_RIGHT_BUTTON))
		{
			if (viewRect < 5)
			{
				rect[viewRect].x1 = mousex - 0.1f;
				rect[viewRect].y1 = mousey - 0.1f;
				rect[viewRect].x2 = mousex + 0.1f;
				rect[viewRect].y2 = mousey + 0.1f;
				first_rect[viewRect] = rect[viewRect];
				++viewRect;
			}
		}
	}
	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'a':
	case 'A':
		i_on = false;
		glutTimerFunc(10, Move_diag, 1);
		if (a_on)
			a_on = false;
		else
			a_on = true;
		break;
	case 'i':
	case 'I':
		a_on = false;
		glutTimerFunc(10, Move_zigzag, 1);
		if (i_on)
			i_on = false;
		else
			i_on = true;
		break;
	case 'c':
	case 'C':

		break;
	case 'o':
	case 'O':
		glutTimerFunc(400, Change_color, 1);
		if (o_on)
			o_on = false;
		else
			o_on = true;

		break;
	case 's':
	case 'S':
		o_on = false;
		i_on = false;
		a_on = false;
		break;
	case 'm':
	case 'M':
		i_on = false;
		a_on = false;
		for (int i = 0; i < 5; ++i) {
			rect[i] = first_rect[i];
		}
		break;
	case 'r':
	case 'R':
		viewRect = 0;
		for (int i = 0; i < 5; ++i) {
			rgb[i].r = dist(gen);
			rgb[i].g = dist(gen);
			rgb[i].b = dist(gen);
		}
		break;
	case 'q':
	case 'Q':
		exit(EXIT_FAILURE);
		break;
	}


	glutPostRedisplay();
}
void Move_diag(int value)
{
	if (a_on)
	{
		for (int i = 0; i < viewRect; ++i) {
			rect[i].x1 += x_add[i];
			rect[i].x2 += x_add[i];
			rect[i].y1 += y_add[i];
			rect[i].y2 += y_add[i];
			if (rect[i].x2 >= 1.0f)
				x_add[i] *= -1;
			else if (rect[i].x1 <= -1.0f)
				x_add[i] *= -1;
			if (rect[i].y2 >= 1.0f)
				y_add[i] *= -1;
			else if (rect[i].y1 <= -1.0f)
				y_add[i] *= -1;
		}
		glutPostRedisplay();
		glutTimerFunc(10, Move_diag, 1);
	}
}

void Move_zigzag(int value)
{
	if (i_on)
	{
		for (int i = 0; i < viewRect; ++i) {
			rect[i].x1 += x_add[i];
			rect[i].x2 += x_add[i];
			rect[i].y1 += y_add[i] * 0.3f;
			rect[i].y2 += y_add[i] * 0.3f;
			if (rect[i].x2 >= 1.0f)
				x_add[i] *= -1;
			else if (rect[i].x1 <= -1.0f)
				x_add[i] *= -1;
			if (rect[i].y2 >= 1.0f)
				y_add[i] *= -1;
			else if (rect[i].y1 <= -1.0f)
				y_add[i] *= -1;
		}
		glutPostRedisplay();
		glutTimerFunc(10, Move_zigzag, 1);
	}
}
void Change_color(int value)
{
	if (o_on)
	{
		for (int i = 0; i < 5; ++i) {
			rgb[i].r = dist(gen);
			rgb[i].g = dist(gen);
			rgb[i].b = dist(gen);
		}
		glutPostRedisplay();
		glutTimerFunc(400, Change_color, 1);
	}
}