#include <iostream>
#include <gl/glew.h> // 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
void TimerFunction(int value);
GLclampf red;
GLclampf blue;
GLclampf green;
int stop_timer;

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{ //--- 윈도우 생성하기
	red = 1;
	blue = 1;
	green = 1;
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(0,0); // 윈도우의 위치 지정
	glutInitWindowSize(800, 600); // 윈도우의 크기 지정
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
	glutMainLoop(); // 이벤트 처리 시작 
}
GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수 
{
	glClearColor(red, green, blue, 1.0f); 
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	// 그리기 부분 구현
	//--- 그리기 관련 부분이 여기에 포함된다.
	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
{
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0, 1);

	switch (key) {
	case 'c': 
		stop_timer = 1;
		red = 0;
		blue = 1;
		green = 1;
		break; //--- 배경색을 청록색으로 설정
	case 'm':
		stop_timer = 1;
		red = 1;
		blue = 1;
		green = 0;
		break; //--- 배경색을 자홍색으로 설정
	case 'y':
		stop_timer = 1;
		red = 1;
		blue = 0;
		green = 1;
		break; //--- 배경색을 노랑색으로 설정
	case 'a':
		stop_timer = 1;
		red = dist(gen);
		blue = dist(gen);
		green = dist(gen);
		break;
	case 'w':
		stop_timer = 1;
		red = 1;
		blue = 1;
		green = 1;
		break;
	case 'k':
		stop_timer = 1;
		red = 0;
		blue = 0;
		green = 0;
		break;
	case 't':
		stop_timer = 0;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 's':
		stop_timer = 1;
		break;
	case 'q':
	case 'Q':
		stop_timer = 1;
		exit(0);
		break;
	}
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}

void TimerFunction(int value)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0, 1);

	red = dist(gen);
	blue = dist(gen);
	green = dist(gen);
	glutPostRedisplay();
	if (stop_timer == 0) {
		glutTimerFunc(100, TimerFunction, 1);
	}
}