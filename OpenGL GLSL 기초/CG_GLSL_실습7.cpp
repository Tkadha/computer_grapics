#define  _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <stdlib.h>
#include <random>

#define Width 1000
#define Height 600
void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char* file);
void Mouse(int, int, int, int);
GLvoid Keyboard(unsigned char key, int x, int y);

GLchar* vertexSource, * fragmentSource; //--- �ҽ��ڵ� ���� ����
GLuint vertexShader, fragmentShader; //--- ���̴� ��ü
GLuint shaderProgramID; 
GLuint vao, vbo[2];
GLfloat point[10][12];
GLfloat RGB[10][12];
int shape_mode;
int shape_count;
int shape_type[10];

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{

	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(Width, Height);
	glutCreateWindow("Example1");
	glewExperimental = GL_TRUE;
	glewInit();
	make_shaderProgram();

	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbo);
	glBindVertexArray(vao);
	//--- ���̴� �о�ͼ� ���̴� ���α׷� �����
	glutDisplayFunc(drawScene); //--- ��� �ݹ� �Լ�
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMainLoop();
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	GLfloat rColor, gColor, bColor;
	rColor = gColor = bColor = 0.3;
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader�� 'layout (location = 0)' �κ�
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	glEnableVertexAttribArray(PosLocation); 
	glEnableVertexAttribArray(ColorLocation);
	for (int i = 0; i < shape_count; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 12 * sizeof(GLfloat)));
		glEnableVertexAttribArray(PosLocation);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 12 * sizeof(GLfloat)));
		glEnableVertexAttribArray(ColorLocation);
		switch (shape_type[i]) {
		case 0:	// ��
			glPointSize(10.0);
			glDrawArrays(GL_POINTS, 0, 1); //--- �������ϱ�: 0�� �ε������� 1���� ���ؽ��� ����Ͽ� �� �׸���
			break;
		case 1:	// ��
			glDrawArrays(GL_LINES, 0, 2);
			break;
		case 2:	// �ﰢ��
			glDrawArrays(GL_TRIANGLES, 0, 3);
			break;
		case 3:	// �簢�� (�ﰢ��2��)
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			break;
		}
	}
	glDisableVertexAttribArray(PosLocation); // Disable �ʼ�!
	glDisableVertexAttribArray(ColorLocation);

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}
void make_vertexShaders()
{
	vertexSource = filetobuf("vertex.glsl");
	//--- ���ؽ� ���̴� ��ü �����
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- ���ؽ� ���̴� �������ϱ�
	glCompileShader(vertexShader);
	//--- �������� ����� ���� ���� ���: ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if(!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cout << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");
	//--- �����׸�Ʈ ���̴� ��ü �����
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- �����׸�Ʈ ���̴� ������
	glCompileShader(fragmentShader);
	//--- �������� ����� ���� ���� ���: ������ ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
void make_shaderProgram()
{
	make_vertexShaders(); //--- ���ؽ� ���̴� �����
	make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	//-- shader Program
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);
	//--- ���̴� �����ϱ�
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//--- Shader Program ����ϱ�
	glUseProgram(shaderProgramID);
}
char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading
	if (!fptr) // Return NULL on failure
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file
	length = ftell(fptr); // Find out how many bytes into the file we are
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
	fclose(fptr); // Close the file
	buf[length] = 0; // Null terminator
	return buf; // Return the buffer
}
void Mouse(int button, int state, int x, int y)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0, 1);
	GLfloat halfx = Width / 2;
	GLfloat halfy = Height / 2;
	GLclampf mousex = (x - halfx) / halfx;
	GLclampf mousey = 1 - (y / halfy);
	if (shape_count < 10) {
		if (state == GLUT_DOWN) {
			if (button == GLUT_LEFT_BUTTON) {
				shape_type[shape_count] = shape_mode;
				if (shape_mode == 0) {
					point[shape_count][0] = mousex;
					point[shape_count][1] = mousey;
					point[shape_count][2] = 0.0f;
					for (int i = 0; i < 3; ++i) {
						RGB[shape_count][i] = dist(gen);
					}
					++shape_count;
				}
				else if (shape_mode == 1) {
					point[shape_count][0] = mousex - 0.05f;
					point[shape_count][1] = mousey - 0.05f;
					point[shape_count][2] = 0.0f;
					point[shape_count][3] = mousex + 0.05f;
					point[shape_count][4] = mousey + 0.05f;
					point[shape_count][5] = 0.0f;
					for (int i = 0; i < 3; ++i) {
						RGB[shape_count][i] = dist(gen);
					}
					for (int i = 0; i < 3; ++i) {
						RGB[shape_count][i+3] = RGB[shape_count][i];
					}
					++shape_count;
				}
				else if (shape_mode == 2) {
					point[shape_count][0] = mousex-0.05f;
					point[shape_count][1] = mousey-0.05f;
					point[shape_count][2] = 0.0f;
					point[shape_count][3] = mousex + 0.05f;
					point[shape_count][4] = mousey - 0.05f;
					point[shape_count][5] = 0.0f;
					point[shape_count][6] = mousex;
					point[shape_count][7] = mousey + 0.05f;
					point[shape_count][8] = 0.0f;
					for (int i = 0; i < 3; ++i) {
						RGB[shape_count][i] = dist(gen);
					}
					for (int i = 0; i < 3; ++i) {
						RGB[shape_count][i + 6] = RGB[shape_count][i + 3] = RGB[shape_count][i];
					}
					++shape_count;
				}
				else if (shape_mode == 3) {

					point[shape_count][0] = mousex - 0.05f;
					point[shape_count][1] = mousey - 0.05f;
					point[shape_count][2] = 0.0f;
					point[shape_count][3] = mousex - 0.05f;
					point[shape_count][4] = mousey + 0.05f;
					point[shape_count][5] = 0.0f;
					point[shape_count][6] = mousex + 0.05f;
					point[shape_count][7] = mousey - 0.05f;
					point[shape_count][8] = 0.0f;
					point[shape_count][9] = mousex + 0.05f;
					point[shape_count][10] = mousey + 0.05f;
					point[shape_count][11] = 0.0f;
					for (int i = 0; i < 3; ++i) {
						RGB[shape_count][i] = dist(gen);
					}
					for (int i = 0; i < 3; ++i) {
						RGB[shape_count][i + 9] = RGB[shape_count][i + 6] = RGB[shape_count][i + 3] = RGB[shape_count][i];
					}
					++shape_count;
				}
				glBindVertexArray(vao);
				glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
				glBufferData(GL_ARRAY_BUFFER, shape_count * 12 * sizeof(GLfloat), point, GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
				glBufferData(GL_ARRAY_BUFFER, shape_count * 12 * sizeof(GLfloat), RGB, GL_STATIC_DRAW);
			}
		}
	}
	glutPostRedisplay();
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'p':
		shape_mode = 0;
		break;
	case 'l':
		shape_mode = 1;
		break;
	case 't':
		shape_mode = 2;
		break;
	case 'r':
		shape_mode = 3;
		break;
	case 'w':
		for (int i = 0; i < 10; ++i) {
			for (int j = 1; j < 12; j+=3) {
				point[i][j] += 0.05f;
			}
		}
		break;
	case 's':
		for (int i = 0; i < 10; ++i) {
			for (int j = 1; j < 12; j += 3) {
				point[i][j] -= 0.05f;
			}
		}
		break;
	case 'a':
		for (int i = 0; i < 10; ++i) {
			for (int j = 0; j < 12; j += 3) {
				point[i][j] -= 0.05f;
			}
		}
		break;
	case 'd':
		for (int i = 0; i < 10; ++i) {
			for (int j = 0; j < 12; j += 3) {
				point[i][j] += 0.05f;
			}
		}
		break;
	case 'c':
		shape_count = 0;
		break;
	}
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, shape_count * 12 * sizeof(GLfloat), point, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, shape_count * 12 * sizeof(GLfloat), RGB, GL_STATIC_DRAW);
	glutPostRedisplay();
}