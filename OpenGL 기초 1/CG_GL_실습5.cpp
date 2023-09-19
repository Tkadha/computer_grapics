#include <iostream>
#include <gl/glew.h> // �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>
#define MAXX 800
#define MAXY 600
#define Max 30

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

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void Mouse(int, int, int, int);
void Move(int, int);

RGB rgb[Max];
Rect rect[Max];
bool no_eating[Max];
bool erase;
Rect eraser;
RGB eraser_color;
GLfloat eat_rect;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dist(0, 1);


void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ���� 
{ //--- ������ �����ϱ�
	for (int i = 0; i < Max; ++i) {
		rgb[i].r = dist(gen);
		rgb[i].g = dist(gen);
		rgb[i].b = dist(gen);
	}
	std::uniform_real_distribution<> dist(-1, 0.95);
	for (int i = 0; i < Max; ++i) {
		rect[i].x1 = dist(gen);
		rect[i].y1 = dist(gen);
		rect[i].x2 = rect[i].x1 + 0.05f;
		rect[i].y2 = rect[i].y1 + 0.05f;
		no_eating[i] = true;
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
	glutMotionFunc(Move);
	glutMainLoop(); // �̺�Ʈ ó�� ���� 
}
GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ� 
{
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	// �׸��� �κ� ����
	for (int i = 0; i < Max; ++i) {
		if (no_eating[i]) {
			glColor3f(rgb[i].r, rgb[i].g, rgb[i].b);
			glRectf(rect[i].x1, rect[i].y1, rect[i].x2, rect[i].y2);
		}
	}
	if (erase) {
		glColor3f(eraser_color.r, eraser_color.g, eraser_color.b);
		glRectf(eraser.x1, eraser.y1, eraser.x2, eraser.y2);
	}
	//--- �׸��� ���� �κ��� ���⿡ ���Եȴ�.
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ� 
{
	glViewport(0, 0, w, h);
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'r':
	case 'R':
		for (int i = 0; i < Max; ++i) {
			rgb[i].r = dist(gen);
			rgb[i].g = dist(gen);
			rgb[i].b = dist(gen);
		}
		std::uniform_real_distribution<> dist(-1, 0.95);
		for (int i = 0; i < Max; ++i) {
			rect[i].x1 = dist(gen);
			rect[i].y1 = dist(gen);
			rect[i].x2 = rect[i].x1 + 0.05f;
			rect[i].y2 = rect[i].y1 + 0.05f;
			no_eating[i] = true;
		}
		break;
	}


	glutPostRedisplay();
}


void Mouse(int button, int state, int x, int y)
{
	GLfloat halfx = MAXX / 2;
	GLfloat halfy = MAXY / 2;
	GLclampf mousex = (x - halfx) / halfx;
	GLclampf mousey = 1 - (y / halfy);

	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			eraser.x1 = mousex - (0.05f + 0.025f * eat_rect);
			eraser.y1 = mousey - (0.05f + 0.025f * eat_rect);
			eraser.x2 = mousex + (0.05f + 0.025f * eat_rect);
			eraser.y2 = mousey + (0.05f + 0.025f * eat_rect);
			erase = true;
		}
	}
	else if (state == GLUT_UP)
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			eat_rect = 0;
			erase = false;
			eraser_color = { 0,0,0 };
		}
	}
	glutPostRedisplay();
}

void Move(int x, int y) {
	GLfloat halfx = MAXX / 2;
	GLfloat halfy = MAXY / 2;
	GLclampf mousex = (x - halfx) / halfx;
	GLclampf mousey = 1 - (y / halfy);

	if (erase) {
		eraser.x1 = mousex - (0.05f + 0.025f * eat_rect);
		eraser.y1 = mousey - (0.05f + 0.025f * eat_rect);
		eraser.x2 = mousex + (0.05f + 0.025f * eat_rect);
		eraser.y2 = mousey + (0.05f + 0.025f * eat_rect);
		for (int i = 0; i < Max; ++i) {
			if (no_eating[i]) {
				if ((rect[i].x1 <= eraser.x1) && (rect[i].x2 >= eraser.x1)) {
					if ((rect[i].y1 <= eraser.y1) && (rect[i].y2 >= eraser.y1)) {
						++eat_rect;
						no_eating[i] = false;
						eraser_color.r += rgb[i].r;
						eraser_color.g += rgb[i].g;
						eraser_color.b += rgb[i].b;
						break;
					}
					else if ((rect[i].y1 <= eraser.y2) && (rect[i].y2 >= eraser.y2))
					{
						++eat_rect;
						no_eating[i] = false;
						eraser_color.r += rgb[i].r;
						eraser_color.g += rgb[i].g;
						eraser_color.b += rgb[i].b;
						break;
					}
				}
				else if ((rect[i].x1 <= eraser.x2) && (rect[i].x2 >= eraser.x2))
				{
					if ((rect[i].y1 <= eraser.y1) && (rect[i].y2 >= eraser.y1)) {
						++eat_rect;
						no_eating[i] = false;
						eraser_color.r += rgb[i].r;
						eraser_color.g += rgb[i].g;
						eraser_color.b += rgb[i].b;
						break;
					}
					else if ((rect[i].y1 <= eraser.y2) && (rect[i].y2 >= eraser.y2))
					{
						++eat_rect;
						no_eating[i] = false;
						eraser_color.r += rgb[i].r;
						eraser_color.g += rgb[i].g;
						eraser_color.b += rgb[i].b;
						break;
					}
				}
				else if ((rect[i].x1 >= eraser.x1) && (rect[i].x2 <= eraser.x2)) {
					if ((rect[i].y1 >= eraser.y1) && (rect[i].y2 <= eraser.y2)) {
						++eat_rect;
						no_eating[i] = false;
						eraser_color.r += rgb[i].r;
						eraser_color.g += rgb[i].g;
						eraser_color.b += rgb[i].b;
						break;
					}
				}
			}
		}
	}
	glutPostRedisplay();
}