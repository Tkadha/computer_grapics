#include <iostream>
#include <gl/glew.h> // 필요한 헤더파일 include
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
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{ //--- 윈도우 생성하기
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
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(MAXX, MAXY); // 윈도우의 크기 지정
	glutCreateWindow("Example1"); // 윈도우 생성(윈도우 이름)
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";
	glutDisplayFunc(drawScene); // 출력 함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutMainLoop(); // 이벤트 처리 시작 
}
GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
	glClearColor(basergb.r, basergb.g, basergb.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	// 그리기 부분 구현
	for (int i = 0; i < viewRect; ++i) {
		glColor3f(rgb[i].r, rgb[i].g, rgb[i].b);
		glRectf(rect[i].x1, rect[i].y1, rect[i].x2, rect[i].y2);
	}
	//--- 그리기 관련 부분이 여기에 포함된다.
	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
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