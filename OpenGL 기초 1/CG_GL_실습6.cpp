#include <iostream>
#include <gl/glew.h> // 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>
#define MAXX 800
#define MAXY 600
#define Amount 0.01f
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Mouse(int, int, int, int);
void UDLR(int);
void Diagonal(int);
void Dia_UDLR(int);
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
GLclampf red;
GLclampf blue;
GLclampf green;

RGB rgb[5];
Rect rect[5];
Rect Divide_rect[5][8];
int divide[5];

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dist(0, 1);

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{ //--- 윈도우 생성하기
	red = 1;
	blue = 1;
	green = 1;
	for (int i = 0; i < 5; ++i) {
		rgb[i].r = dist(gen);
		rgb[i].g = dist(gen);
		rgb[i].b = dist(gen);
	}
	std::uniform_real_distribution<> dist(-1, 0.8);
	for (int i = 0; i < 5; ++i) {
		rect[i].x1 = dist(gen);
		rect[i].y1 = dist(gen);
		rect[i].x2 = rect[i].x1 + 0.2f;
		rect[i].y2 = rect[i].y1 + 0.2f;
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
	glutMouseFunc(Mouse);
	glutMainLoop(); // 이벤트 처리 시작 
}
GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
	glClearColor(red, green, blue, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	// 그리기 부분 구현
	for (int i = 0; i < 5; ++i) {
		if (divide[i] == 0) {
			glColor3f(rgb[i].r, rgb[i].g, rgb[i].b);
			glRectf(rect[i].x1, rect[i].y1, rect[i].x2, rect[i].y2);
		}
		else {
			glColor3f(rgb[i].r, rgb[i].g, rgb[i].b);
			if (divide[i] == 3) {
				for (int j = 0; j < 8; ++j) {
					glRectf(Divide_rect[i][j].x1, Divide_rect[i][j].y1, Divide_rect[i][j].x2, Divide_rect[i][j].y2);
				}
			}
			else
			{
				for (int j = 0; j < 4; ++j) {
					glRectf(Divide_rect[i][j].x1, Divide_rect[i][j].y1, Divide_rect[i][j].x2, Divide_rect[i][j].y2);
				}
			}
		}
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
	GLfloat halfx = MAXX / 2;
	GLfloat halfy = MAXY / 2;
	GLclampf mousex = (x - halfx) / halfx;
	GLclampf mousey = 1 - (y / halfy);

	GLfloat div;
	if (state == GLUT_DOWN)
	{
		if ((button == GLUT_LEFT_BUTTON) || (button == GLUT_RIGHT_BUTTON))
		{
			for (int i = 4; i >= 0; --i) {
				if ((mousex >= rect[i].x1) && (mousex <= rect[i].x2)) {
					if ((mousey >= rect[i].y1) && (mousey <= rect[i].y2)) {
						div = dist(gen);
						if (div < 0.3) {		//좌우상하 이동
							divide[i] = 1;
							Divide_rect[i][0] = { rect[i].x1, (rect[i].y1 + rect[i].y2) / 2, (rect[i].x1 + rect[i].x2) / 2, rect[i].y2 };
							Divide_rect[i][1] = { (rect[i].x1 + rect[i].x2) / 2, (rect[i].y1 + rect[i].y2) / 2, rect[i].x2, rect[i].y2 };
							Divide_rect[i][2] = { rect[i].x1, rect[i].y1, (rect[i].x1 + rect[i].x2) / 2, (rect[i].y1 + rect[i].y2) / 2 };
							Divide_rect[i][3] = { (rect[i].x1 + rect[i].x2) / 2, rect[i].y1, rect[i].x2, (rect[i].y1 + rect[i].y2) / 2 };
							glutTimerFunc(100, UDLR, i);
							break;
						}
						else if (div > 0.6) {	// 대각선 이동
							divide[i] = 2;
							Divide_rect[i][0] = { rect[i].x1, (rect[i].y1 + rect[i].y2) / 2, (rect[i].x1 + rect[i].x2) / 2, rect[i].y2 };
							Divide_rect[i][1] = { (rect[i].x1 + rect[i].x2) / 2, (rect[i].y1 + rect[i].y2) / 2, rect[i].x2, rect[i].y2 };
							Divide_rect[i][2] = { rect[i].x1, rect[i].y1, (rect[i].x1 + rect[i].x2) / 2, (rect[i].y1 + rect[i].y2) / 2 };
							Divide_rect[i][3] = { (rect[i].x1 + rect[i].x2) / 2, rect[i].y1, rect[i].x2, (rect[i].y1 + rect[i].y2) / 2 };
							glutTimerFunc(100, Diagonal, i);
							break;
						}
						else {					// 8개 분리
							divide[i] = 3;
							//대각선
							Divide_rect[i][0] = { rect[i].x1, (rect[i].y1 + rect[i].y2) / 2, (rect[i].x1 + rect[i].x2) / 2, rect[i].y2 };
							Divide_rect[i][1] = { (rect[i].x1 + rect[i].x2) / 2, (rect[i].y1 + rect[i].y2) / 2, rect[i].x2, rect[i].y2 };
							Divide_rect[i][2] = { rect[i].x1, rect[i].y1, (rect[i].x1 + rect[i].x2) / 2, (rect[i].y1 + rect[i].y2) / 2 };
							Divide_rect[i][3] = { (rect[i].x1 + rect[i].x2) / 2, rect[i].y1, rect[i].x2, (rect[i].y1 + rect[i].y2) / 2 }; 
							//상하좌우
							Divide_rect[i][4] = { rect[i].x1, (rect[i].y1 + rect[i].y2) / 2, (rect[i].x1 + rect[i].x2) / 2, rect[i].y2 };
							Divide_rect[i][5] = { (rect[i].x1 + rect[i].x2) / 2, (rect[i].y1 + rect[i].y2) / 2, rect[i].x2, rect[i].y2 };
							Divide_rect[i][6] = { rect[i].x1, rect[i].y1, (rect[i].x1 + rect[i].x2) / 2, (rect[i].y1 + rect[i].y2) / 2 };
							Divide_rect[i][7] = { (rect[i].x1 + rect[i].x2) / 2, rect[i].y1, rect[i].x2, (rect[i].y1 + rect[i].y2) / 2 };
							
							glutTimerFunc(100, Dia_UDLR, i);
							break;
						}
					}
				}
			}
		}
	}
	glutPostRedisplay();
}

void UDLR(int value)
{
	for (int i = 0; i < 4; ++i) {
		switch (i) {
		case 0:
			Divide_rect[value][i].x1 += 0.5f * Amount;
			Divide_rect[value][i].y1 += Amount;
			Divide_rect[value][i].y2 += 0.5f * Amount;
			break;
		case 1:
			Divide_rect[value][i].x1 += Amount;
			Divide_rect[value][i].x2 += 0.5f * Amount;
			Divide_rect[value][i].y2 -= 0.5f * Amount;
			break;
		case 2:
			Divide_rect[value][i].x1 -= 0.5f * Amount;
			Divide_rect[value][i].x2 -= Amount;
			Divide_rect[value][i].y1 += 0.5f * Amount;
			break;
		case 3:
			Divide_rect[value][i].x2 -= 0.5f * Amount;
			Divide_rect[value][i].y1 -= 0.5f * Amount;
			Divide_rect[value][i].y2 -= Amount;
			break;
		}
	}
	glutPostRedisplay();
	if (Divide_rect[value][0].x1 < Divide_rect[value][0].x2)
	{
		glutTimerFunc(100, UDLR, value);
	}
	else {
		for (int i = 0; i < 4; ++i) {
			Divide_rect[value][i].x1 = Divide_rect[value][i].x2;
			Divide_rect[value][i].y1 = Divide_rect[value][i].y2;
		}
	}

}
void Diagonal(int value)
{
	for (int i = 0; i < 4; ++i) {
		switch (i) {
		case 0:
			Divide_rect[value][i].x1 -= 0.5f * Amount;
			Divide_rect[value][i].x2 -= Amount;
			Divide_rect[value][i].y1 += Amount;
			Divide_rect[value][i].y2 += 0.5f * Amount;
			break;
		case 1:
			Divide_rect[value][i].x1 += Amount;
			Divide_rect[value][i].x2 += 0.5f * Amount;
			Divide_rect[value][i].y1 += Amount;
			Divide_rect[value][i].y2 += 0.5f * Amount;
			break;
		case 2:
			Divide_rect[value][i].x1 -= 0.5f * Amount;
			Divide_rect[value][i].x2 -= Amount;
			Divide_rect[value][i].y1 -= 0.5f * Amount;
			Divide_rect[value][i].y2 -= Amount;
			break;
		case 3:
			Divide_rect[value][i].x1 += Amount;
			Divide_rect[value][i].x2 += 0.5f * Amount;
			Divide_rect[value][i].y1 -= 0.5f * Amount;
			Divide_rect[value][i].y2 -= Amount;
			break;
		}
	}
	glutPostRedisplay();
	if (Divide_rect[value][0].x1 < Divide_rect[value][0].x2)
	{
		glutTimerFunc(100, Diagonal, value);
	}
	else {
		for (int i = 0; i < 4; ++i) {
			Divide_rect[value][i].x1 = Divide_rect[value][i].x2;
			Divide_rect[value][i].y1 = Divide_rect[value][i].y2;
		}
	}
}
void Dia_UDLR(int value)
{
	for (int i = 0; i < 8; ++i) {
		switch (i) {
		case 0:
			Divide_rect[value][i].x1 -= 0.5f * Amount;
			Divide_rect[value][i].x2 -= Amount;
			Divide_rect[value][i].y1 += Amount;
			Divide_rect[value][i].y2 += 0.5f * Amount;
			break;
		case 1:
			Divide_rect[value][i].x1 += Amount;
			Divide_rect[value][i].x2 += 0.5f * Amount;
			Divide_rect[value][i].y1 += Amount;
			Divide_rect[value][i].y2 += 0.5f * Amount;
			break;
		case 2:
			Divide_rect[value][i].x1 -= 0.5f * Amount;
			Divide_rect[value][i].x2 -= Amount;
			Divide_rect[value][i].y1 -= 0.5f * Amount;
			Divide_rect[value][i].y2 -= Amount;
			break;
		case 3:
			Divide_rect[value][i].x1 += Amount;
			Divide_rect[value][i].x2 += 0.5f * Amount;
			Divide_rect[value][i].y1 -= 0.5f * Amount;
			Divide_rect[value][i].y2 -= Amount;
			break;
		case 4:
			Divide_rect[value][i].x1 += 0.5f * Amount;
			Divide_rect[value][i].y1 += Amount;
			Divide_rect[value][i].y2 += 0.5f * Amount;
			break;
		case 5:
			Divide_rect[value][i].x1 += Amount;
			Divide_rect[value][i].x2 += 0.5f * Amount;
			Divide_rect[value][i].y2 -= 0.5f * Amount;
			break;
		case 6:
			Divide_rect[value][i].x1 -= 0.5f * Amount;
			Divide_rect[value][i].x2 -= Amount;
			Divide_rect[value][i].y1 += 0.5f * Amount;
			break;
		case 7:
			Divide_rect[value][i].x2 -= 0.5f * Amount;
			Divide_rect[value][i].y1 -= 0.5f * Amount;
			Divide_rect[value][i].y2 -= Amount;
			break;
		}
	}
	glutPostRedisplay();
	if (Divide_rect[value][0].x1 < Divide_rect[value][0].x2)
	{
		glutTimerFunc(100, Dia_UDLR, value);
	}
	else {
		for (int i = 0; i < 8; ++i) {
			Divide_rect[value][i].x1 = Divide_rect[value][i].x2;
			Divide_rect[value][i].y1 = Divide_rect[value][i].y2;
		}
	}
}