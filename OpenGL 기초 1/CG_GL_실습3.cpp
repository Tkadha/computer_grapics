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
void Motion(int x, int y);
void setting_rect();
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
RGB basergb;
RGB rgb[5];
Rect rect[5];
int viewRect;
bool left;
void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ���� 
{ //--- ������ �����ϱ�
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0, 1);
	for (int i = 0; i < 5; ++i) {
		rgb[i].r = dist(gen);
		rgb[i].g = dist(gen);
		rgb[i].b = dist(gen);
	}
	setting_rect();
	viewRect = 1;
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
	glutMotionFunc(Motion);
	glutMainLoop(); // �̺�Ʈ ó�� ���� 
}
GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ� 
{
	glClearColor(basergb.r, basergb.g, basergb.b, 1.0f);
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
	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON)
			left = true;
	}
	else if (state == GLUT_UP) {
		if (button == GLUT_LEFT_BUTTON)
			left = false;
	}

	glutDisplayFunc(drawScene);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'a':
		if (viewRect < 5) {
			++viewRect;
		}
		break;
	}


	glutPostRedisplay();
}

void setting_rect()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(-1, 0.8f);
	for (int i = 0; i < 5; ++i) {
		rect[i].x1 = dist(gen);
		rect[i].y1 = dist(gen);
		rect[i].x2 = rect[i].x1 + 0.2f;
		rect[i].y2 = rect[i].y1 + 0.2f;
	}
}

void Motion(int x, int y)
{
	GLfloat halfx = MAXX / 2;
	GLfloat halfy = MAXY / 2;
	GLclampf mousex = (x - halfx) / halfx;
	GLclampf mousey = 1 - (y / halfy);

	if (left == true) {
		for (int i = viewRect - 1; i >= 0; --i) {
			if ((mousex <= rect[i].x2) && (mousex >= rect[i].x1)) {
				if ((mousey <= rect[i].y2) && (mousey >= rect[i].y1)) {
					rect[i].x1 = mousex - 0.1f;
					rect[i].y1 = mousey - 0.1f;
					rect[i].x2 = mousex + 0.1f;
					rect[i].y2 = mousey + 0.1f;
					break;
				}
			}
		}
	}
	glutPostRedisplay();
}