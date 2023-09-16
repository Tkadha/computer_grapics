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

class RGB {
public:
	GLclampf r;
	GLclampf g;
	GLclampf b;
};
class Rect{
public:
	GLfloat x1;
	GLfloat y1;
	GLfloat x2;
	GLfloat y2;
};
RGB basergb;
RGB rgb[4];
Rect rect[4];


void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{ //--- 윈도우 생성하기
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0, 1);
	for (int i = 0; i < 4; ++i) {
		rgb[i].r = dist(gen);
		rgb[i].g = dist(gen);
		rgb[i].b = dist(gen);
	}
	rect[0].x1 = -1.0f;
	rect[0].y1 = 0.0f;
	rect[0].x2 = 0.0f;
	rect[0].y2 = 1.0f;

	rect[1].x1 = 0.0f;
	rect[1].y1 = 0.0f;
	rect[1].x2 = 1.0f;
	rect[1].y2 = 1.0f;

	rect[2].x1 = -1.0f;
	rect[2].y1 = -1.0f;
	rect[2].x2 = 0.0f;
	rect[2].y2 = 0.0f;

	rect[3].x1 = 0.0f;
	rect[3].y1 = -1.0f;
	rect[3].x2 = 1.0f;
	rect[3].y2 = 0.0f;

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
	glClearColor(basergb.r, basergb.g, basergb.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	// 그리기 부분 구현
	for (int i = 0; i < 4; ++i) {
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
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0, 1);
	GLfloat halfx = MAXX / 2;
	GLfloat halfy = MAXY / 2;
	int found = 0;
	if (state == GLUT_DOWN)
	{
		std::cout << x << ' ' << y << '\n';
		std::cout << (x - halfx) / halfx << ' ' << 1 - (y / halfy) << '\n';
		if (button == GLUT_LEFT_BUTTON)		//색상 변경
		{
			for (int i = 0; i < 4; ++i)
			{
				if ((rect[i].x1 <= (x - halfx) / halfx) && (rect[i].x2 >= (x - halfx) / halfx))
				{
					if ((rect[i].y1 <= 1 - (y / halfy)) && (rect[i].y2 >= 1 - (y / halfy)))					//사각형 안이라면
					{
						rgb[i].r = dist(gen);
						rgb[i].g = dist(gen);
						rgb[i].b = dist(gen);
						found = 1;
						break;
					}
				}
			}
			if (found == 0)
			{
				basergb.r = dist(gen);
				basergb.g = dist(gen);
				basergb.b = dist(gen);
			}
		}
		else if (button == GLUT_RIGHT_BUTTON)	// 크기 변경
		{
			for (int i = 0; i < 4; ++i)
			{
				if ((rect[i].x1 <= (x - halfx) / halfx) && (rect[i].x2 >= (x - halfx) / halfx))
				{
					if ((rect[i].y1 <= 1 - (y / halfy)) && (rect[i].y2 >= 1 - (y / halfy)))					//사각형 안이라면
					{
						rect[i].x1 += 0.05f;
						rect[i].y1 += 0.05f;
						rect[i].x2 -= 0.05f;
						rect[i].y2 -= 0.05f;
						if (rect[i].x1 >= rect[i].x2)
						{
							rect[i].x1 -= 0.05f;
							rect[i].y1 -= 0.05f;
							rect[i].x2 += 0.05f;
							rect[i].y2 += 0.05f;
						}
						found = 1;
						break;
					}
				}
			}
			if (found == 0)
			{
				if (x <= halfx)
				{
					if (y <= halfy)		// 좌상단 - 0번
					{
						rect[0].x1 -= 0.05f;
						rect[0].y1 -= 0.05f;
						rect[0].x2 += 0.05f;
						rect[0].y2 += 0.05f;
					}
					else					// 좌하단 - 2번
					{
						rect[2].x1 -= 0.05f;
						rect[2].y1 -= 0.05f;
						rect[2].x2 += 0.05f;
						rect[2].y2 += 0.05f;
					}
				}
				else
				{
					if (y <= halfy)		// 우상단 - 1번
					{
						rect[1].x1 -= 0.05f;
						rect[1].y1 -= 0.05f;
						rect[1].x2 += 0.05f;
						rect[1].y2 += 0.05f;
					}
					else					// 우하단 - 3번
					{
						rect[3].x1 -= 0.05f;
						rect[3].y1 -= 0.05f;
						rect[3].x2 += 0.05f;
						rect[3].y2 += 0.05f;
					}
				}
			}
		}
		glutDisplayFunc(drawScene);
	}
}
