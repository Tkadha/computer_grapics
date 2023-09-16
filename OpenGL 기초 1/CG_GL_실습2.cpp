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


void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ���� 
{ //--- ������ �����ϱ�
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
	glutMouseFunc(Mouse);
	glutMainLoop(); // �̺�Ʈ ó�� ���� 
}
GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ� 
{
	glClearColor(basergb.r, basergb.g, basergb.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	// �׸��� �κ� ����
	for (int i = 0; i < 4; ++i) {
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
		if (button == GLUT_LEFT_BUTTON)		//���� ����
		{
			for (int i = 0; i < 4; ++i)
			{
				if ((rect[i].x1 <= (x - halfx) / halfx) && (rect[i].x2 >= (x - halfx) / halfx))
				{
					if ((rect[i].y1 <= 1 - (y / halfy)) && (rect[i].y2 >= 1 - (y / halfy)))					//�簢�� ���̶��
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
		else if (button == GLUT_RIGHT_BUTTON)	// ũ�� ����
		{
			for (int i = 0; i < 4; ++i)
			{
				if ((rect[i].x1 <= (x - halfx) / halfx) && (rect[i].x2 >= (x - halfx) / halfx))
				{
					if ((rect[i].y1 <= 1 - (y / halfy)) && (rect[i].y2 >= 1 - (y / halfy)))					//�簢�� ���̶��
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
					if (y <= halfy)		// �»�� - 0��
					{
						rect[0].x1 -= 0.05f;
						rect[0].y1 -= 0.05f;
						rect[0].x2 += 0.05f;
						rect[0].y2 += 0.05f;
					}
					else					// ���ϴ� - 2��
					{
						rect[2].x1 -= 0.05f;
						rect[2].y1 -= 0.05f;
						rect[2].x2 += 0.05f;
						rect[2].y2 += 0.05f;
					}
				}
				else
				{
					if (y <= halfy)		// ���� - 1��
					{
						rect[1].x1 -= 0.05f;
						rect[1].y1 -= 0.05f;
						rect[1].x2 += 0.05f;
						rect[1].y2 += 0.05f;
					}
					else					// ���ϴ� - 3��
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
