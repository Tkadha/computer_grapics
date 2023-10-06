#define  _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <stdlib.h>
#include <random>

#define Width 1200
#define Height 800

void Setting();
void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char* file);


GLchar* vertexSource, * fragmentSource; //--- �ҽ��ڵ� ���� ����
GLuint vertexShader, fragmentShader; //--- ���̴� ��ü
GLuint shaderProgramID;
GLuint vao, vbo[2];
GLfloat shape[15][5][3];
GLfloat RGB[15][15];
int shape_type[15];

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dist(-1, 1);
std::uniform_real_distribution<> color(0, 1);

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
	Setting();
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(shape), shape, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(RGB), RGB, GL_STATIC_DRAW);
	//--- ���̴� �о�ͼ� ���̴� ���α׷� �����
	glutDisplayFunc(drawScene); //--- ��� �ݹ� �Լ�
	glutReshapeFunc(Reshape);
	glutMainLoop();
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	GLfloat rColor, gColor, bColor;
	rColor = gColor = bColor = 0.5;
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader�� 'layout (location = 0)' �κ�
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation);
	for (int i = 0; i < 15; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 15 * sizeof(GLfloat)));
		glEnableVertexAttribArray(PosLocation);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 15 * sizeof(GLfloat)));
		glEnableVertexAttribArray(ColorLocation);
		switch(shape_type[i]) {
		case 0:
			glPointSize(7);
			glDrawArrays(GL_POINTS, 0, 1);
			break;
		case 1:
			glLineWidth(5);
			glDrawArrays(GL_LINES, 0, 2);
			break;
		case 2:
			glDrawArrays(GL_TRIANGLES, 0, 3);
			break;
		case 3:
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			break;
		case 4:
			glDrawArrays(GL_TRIANGLE_FAN, 0, 5);
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
	if (!result)
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
void Setting()
{
	for (int i = 0; i < 15; ++i) {
		RGB[i][12] = RGB[i][9] = RGB[i][6] = RGB[i][3] = RGB[i][0] = color(gen);
		RGB[i][13] = RGB[i][10] = RGB[i][7] = RGB[i][4] = RGB[i][1] = color(gen);
		RGB[i][14] = RGB[i][11] = RGB[i][8] = RGB[i][5] = RGB[i][2] = color(gen);
	}
	for (int i = 0; i < 3; ++i) {
		shape[i][0][0] = dist(gen);
		shape[i][0][1] = dist(gen);
		shape_type[i] = 0;

		shape[i + 3][0][0] = dist(gen);
		shape[i + 3][0][1] = dist(gen);
		shape[i + 3][1][0] = shape[i + 3][0][0] + 0.1f;
		shape[i + 3][1][1] = shape[i + 3][0][1] + 0.1f;
		shape_type[i + 3] = 1;

		shape[i + 6][0][0] = dist(gen);
		shape[i + 6][0][1] = dist(gen);
		shape[i + 6][1][0] = shape[i + 6][0][0] + 0.1f;
		shape[i + 6][1][1] = shape[i + 6][0][1];
		shape[i + 6][2][0] = shape[i + 6][0][0] + 0.05f;
		shape[i + 6][2][1] = shape[i + 6][0][1] + 0.1f;
		shape_type[i + 6] = 2;

		shape[i + 9][0][0] = dist(gen);
		shape[i + 9][0][1] = dist(gen);
		shape[i + 9][1][0] = shape[i + 9][0][0] + 0.1f;
		shape[i + 9][1][1] = shape[i + 9][0][1];
		shape[i + 9][2][0] = shape[i + 9][0][0] + 0.1f;
		shape[i + 9][2][1] = shape[i + 9][0][1] + 0.1f;
		shape[i + 9][3][0] = shape[i + 9][0][0];
		shape[i + 9][3][1] = shape[i + 9][0][1] + 0.1f;
		shape_type[i + 9] = 3;

		shape[i + 12][0][0] = dist(gen);
		shape[i + 12][0][1] = dist(gen);
		shape[i + 12][1][0] = shape[i + 12][0][0] + 0.06f;
		shape[i + 12][1][1] = shape[i + 12][0][1];
		shape[i + 12][2][0] = shape[i + 12][0][0] + 0.08f;
		shape[i + 12][2][1] = shape[i + 12][0][1] + 0.06f;
		shape[i + 12][3][0] = shape[i + 12][0][0] + 0.03f;
		shape[i + 12][3][1] = shape[i + 12][0][1] + 0.1f;
		shape[i + 12][4][0] = shape[i + 12][0][0] - 0.02f;
		shape[i + 12][4][1] = shape[i + 12][0][1] + 0.06f;
		shape_type[i + 12] = 4;
	}
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(shape), shape, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(RGB), RGB, GL_STATIC_DRAW);
}
