#include <iostream>
#include <gl/glew.h> // �ʿ��� ������� include
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

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ���� 
{ //--- ������ �����ϱ�
	red = 1;
	blue = 1;
	green = 1;
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
	glutInitWindowPosition(0,0); // �������� ��ġ ����
	glutInitWindowSize(800, 600); // �������� ũ�� ����
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
	glutMainLoop(); // �̺�Ʈ ó�� ���� 
}
GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ� 
{
	glClearColor(red, green, blue, 1.0f); 
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	// �׸��� �κ� ����
	//--- �׸��� ���� �κ��� ���⿡ ���Եȴ�.
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ� 
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
		break; //--- ������ û�ϻ����� ����
	case 'm':
		stop_timer = 1;
		red = 1;
		blue = 1;
		green = 0;
		break; //--- ������ ��ȫ������ ����
	case 'y':
		stop_timer = 1;
		red = 1;
		blue = 0;
		green = 1;
		break; //--- ������ ��������� ����
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
	glutPostRedisplay(); //--- ������ �ٲ� ������ ��� �ݹ� �Լ��� ȣ���Ͽ� ȭ���� refresh �Ѵ�
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