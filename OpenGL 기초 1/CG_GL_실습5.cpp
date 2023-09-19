#include <iostream>
#include <gl/glew.h> // 필요한 헤더파일 include
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


void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{ //--- 윈도우 생성하기
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
	glutMotionFunc(Move);
	glutMainLoop(); // 이벤트 처리 시작 
}
GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	// 그리기 부분 구현
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
	//--- 그리기 관련 부분이 여기에 포함된다.
	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
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