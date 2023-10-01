#define  _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <stdlib.h>
#include <random>
#include <cmath>

#define Width 1000
#define Height 600
#define PI 3.1415926

void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char* file);
void Mouse(int, int, int, int);
GLvoid Keyboard(unsigned char key, int x, int y);
void diagonal(int);
void zigzag(int);
void rec_spiral(int);
void circle_spiral(int);


GLchar* vertexSource, * fragmentSource; //--- �ҽ��ڵ� ���� ����
GLuint vertexShader, fragmentShader; //--- ���̴� ��ü
GLuint shaderProgramID;
GLuint vao, vbo[2];
GLuint vbo_line[2];
GLfloat triangle[4][9];
GLfloat RGB[4][9];
GLfloat line[2][6];
GLfloat line_RGB[2][6];
GLfloat diag[4][2];
GLfloat zig[4][2];
int zigzag_count[4];
GLfloat r[4];
GLfloat h[4];

int mode_timer;
int mode;

int max_count[4];
int count[4];
int way[4];
GLfloat Plus_Min[2];
GLfloat pos_triangle[4][9];

int angle[4];
GLfloat radius[4];
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
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 2; ++j)
		{
			diag[i][j] = 0.005f;
			zig[i][j] = 0.005f;
		}
	for (int i = 0; i < 2; ++i)
		Plus_Min[i] = 0.005f;
	{
		line[0][1] = 1;
		line[0][4] = -1;
		line[1][0] = 1;
		line[1][3] = -1;
	}
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbo);
	glGenBuffers(2, vbo_line);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), triangle, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), RGB, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[0]);
	glBufferData(GL_ARRAY_BUFFER, 2 * 6 * sizeof(GLfloat), line, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[1]);
	glBufferData(GL_ARRAY_BUFFER, 2 * 6 * sizeof(GLfloat), line_RGB, GL_STATIC_DRAW);
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
	rColor = gColor = bColor = 1.0;
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader�� 'layout (location = 0)' �κ�
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation);
	for (int i = 0; i < 2; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_line[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(PosLocation);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_line[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(ColorLocation);
		glDrawArrays(GL_LINES, 0, 2);
	}
	for (int i = 0; i < 4; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 9 * sizeof(GLfloat)));
		glEnableVertexAttribArray(PosLocation);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 9 * sizeof(GLfloat)));
		glEnableVertexAttribArray(ColorLocation);
		if (mode == 1) {
			glPolygonMode(GL_FRONT, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT, GL_FILL);
		}
		glDrawArrays(GL_TRIANGLES, 0, 3);
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
void Mouse(int button, int state, int x, int y)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0.05, 0.3);
	GLfloat halfx = Width / 2;
	GLfloat halfy = Height / 2;
	GLclampf mousex = (x - halfx) / halfx;
	GLclampf mousey = 1 - (y / halfy);
	GLfloat tri_size = dist(gen);
	std::uniform_real_distribution<> color(0, 1);
	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {
			if (mousex < 0) {
				if (mousey < 0) {	//3��и�
					triangle[2][0] = mousex - tri_size / 2;
					triangle[2][1] = mousey - tri_size;
					triangle[2][2] = 0.0f;
					triangle[2][3] = mousex + tri_size / 2;
					triangle[2][4] = mousey - tri_size;
					triangle[2][5] = 0.0f;
					triangle[2][6] = mousex;
					triangle[2][7] = mousey + tri_size;
					triangle[2][8] = 0.0f;
					r[2] = tri_size;
					h[2] = tri_size * 2;
					for (int i = 0; i < 3; ++i) {
						RGB[2][i + 6] = RGB[2][i + 3] = RGB[2][i] = color(gen);
					}
				}
				else if (mousey >= 0) {	//2��и�
					triangle[1][0] = mousex - tri_size / 2;
					triangle[1][1] = mousey - tri_size;
					triangle[1][2] = 0.0f;
					triangle[1][3] = mousex + tri_size / 2;
					triangle[1][4] = mousey - tri_size;
					triangle[1][5] = 0.0f;
					triangle[1][6] = mousex;
					triangle[1][7] = mousey + tri_size;
					triangle[1][8] = 0.0f;
					r[1] = tri_size;
					h[1] = tri_size * 2;
					for (int i = 0; i < 3; ++i) {
						RGB[1][i + 6] = RGB[1][i + 3] = RGB[1][i] = color(gen);
					}
				}
			}
			else if (mousex >= 0) {
				if (mousey < 0) {		//4��и�
					triangle[3][0] = mousex - tri_size / 2;
					triangle[3][1] = mousey - tri_size;
					triangle[3][2] = 0.0f;
					triangle[3][3] = mousex + tri_size / 2;
					triangle[3][4] = mousey - tri_size;
					triangle[3][5] = 0.0f;
					triangle[3][6] = mousex;
					triangle[3][7] = mousey + tri_size;
					triangle[3][8] = 0.0f;
					r[3] = tri_size;
					h[3] = tri_size * 2;
					for (int i = 0; i < 3; ++i) {
						RGB[3][i + 6] = RGB[3][i + 3] = RGB[3][i] = color(gen);
					}
				}
				else if (mousey >= 0) {	//1��и�
					triangle[0][0] = mousex - tri_size / 2;
					triangle[0][1] = mousey - tri_size;
					triangle[0][2] = 0.0f;
					triangle[0][3] = mousex + tri_size / 2;
					triangle[0][4] = mousey - tri_size;
					triangle[0][5] = 0.0f;
					triangle[0][6] = mousex;
					triangle[0][7] = mousey + tri_size;
					triangle[0][8] = 0.0f;
					r[0] = tri_size;
					h[0] = tri_size * 2;
					for (int i = 0; i < 3; ++i) {
						RGB[0][i + 6] = RGB[0][i + 3] = RGB[0][i] = color(gen);
					}
				}
			}
		}
	}
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), triangle, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), RGB, GL_STATIC_DRAW);
	glutPostRedisplay();
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'a':
		mode = 1;
		break;
	case 'b':
		mode = 0;
		break;
	case '1':
		mode_timer = 1;
		for (int i = 0; i < 4; ++i)
			glutTimerFunc(10, diagonal, i);
		break;
	case '2':
		mode_timer = 2;
		for (int i = 0; i < 4; ++i)
		{
			triangle[i][0] = triangle[i][3] - r[i];
			triangle[i][1] = triangle[i][4];
			triangle[i][6] = triangle[i][3] - r[i]/2;
			triangle[i][7] = triangle[i][4] + h[i];
			glutTimerFunc(10, zigzag, i);
		}
		break;
	case '3':
		mode_timer = 3;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 9; ++j)
				pos_triangle[i][j] = triangle[i][j];
		for (int i = 0; i < 4; ++i)
		{
			max_count[i] = 10;
			glutTimerFunc(1, rec_spiral, i);
		}
		break;
	case '4':
		mode_timer = 4;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 9; ++j)
				pos_triangle[i][j] = triangle[i][j];
		for (int i = 0; i < 4; ++i)
		{
			angle[i] = 10;
			radius[i] = 0.001f;
			glutTimerFunc(100, circle_spiral, i);
		}
		break;
	}
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), triangle, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), RGB, GL_STATIC_DRAW);
	glutPostRedisplay();
}
void diagonal(int value)
{
	triangle[value][0] += diag[value][0];
	triangle[value][3] += diag[value][0];
	triangle[value][6] += diag[value][0];
	triangle[value][1] += diag[value][1];
	triangle[value][4] += diag[value][1];
	triangle[value][7] += diag[value][1];
	if (diag[value][1] > 0) {
		if (triangle[value][1] >= 1) {
			triangle[value][0] = triangle[value][3];
			triangle[value][1] = triangle[value][4] - r[value];
			triangle[value][6] = triangle[value][3] - h[value] / 2;
			triangle[value][7] = triangle[value][4] - r[value] / 2;
			diag[value][1] *= -1;
		}
		else if (triangle[value][4] >= 1)
		{
			triangle[value][0] = triangle[value][3] + r[value];
			triangle[value][1] = triangle[value][4];
			triangle[value][6] = triangle[value][3] + r[value] / 2;
			triangle[value][7] = triangle[value][4] - h[value];
			diag[value][1] *= -1;
		}
		else if (triangle[value][7] >= 1) {
			triangle[value][0] = triangle[value][3];
			triangle[value][1] = triangle[value][4] - r[value];
			triangle[value][6] = triangle[value][3] - h[value] / 2;
			triangle[value][7] = triangle[value][4] - r[value] / 2;
			diag[value][1] *= -1;
		}
	}
	if (diag[value][1] < 0) {
		if ((triangle[value][1] <= -1)||(triangle[value][4] <= -1)) {
			triangle[value][3] = triangle[value][0];
			triangle[value][4] = triangle[value][1] + r[value];
			triangle[value][6] = triangle[value][0] - h[value] / 2;
			triangle[value][7] = triangle[value][1] + r[value] / 2;
			diag[value][1] *= -1;
		}
		else if (triangle[value][7] <= -1)
		{
			triangle[value][3] = triangle[value][0];
			triangle[value][4] = triangle[value][1] + r[value];
			triangle[value][6] = triangle[value][0] - h[value] / 2;
			triangle[value][7] = triangle[value][1] + r[value] / 2;
			diag[value][1] *= -1;
		}
	}
	if (diag[value][0] > 0) {
		if ((triangle[value][0] >= 1) || (triangle[value][3] >= 1) || (triangle[value][6] >= 1)) {
			triangle[value][0] = triangle[value][3];
			triangle[value][1] = triangle[value][4];
			triangle[value][3] = triangle[value][0] - r[value];
			triangle[value][4] = triangle[value][1];
			triangle[value][6] = triangle[value][0] - r[value] / 2;
			triangle[value][7] = triangle[value][1] - h[value];
			diag[value][0] *= -1;
		}
	}
	if (diag[value][0] < 0) {
		if ((triangle[value][0] <= -1) || (triangle[value][3] <= -1)) {
			triangle[value][3] = triangle[value][0];
			triangle[value][4] = triangle[value][1] - r[value];
			triangle[value][6] = triangle[value][0] + h[value]/2;
			triangle[value][7] = triangle[value][1] - r[value] / 2;
			diag[value][0] *= -1;
		}
		else if (triangle[value][6] <= -1)
		{
			triangle[value][0] = triangle[value][3];
			triangle[value][1] = triangle[value][4] + r[value];
			triangle[value][6] = triangle[value][3] + h[value]/2;
			triangle[value][7] = triangle[value][4] + r[value]/2;
			diag[value][0] *= -1;
		}
	}
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), triangle, GL_DYNAMIC_DRAW);
	glutPostRedisplay();
	if (mode_timer == 1) {
		glutTimerFunc(10, diagonal, value);
	}
}
void zigzag(int value) 
{
	if (zigzag_count[value] == 0) {
		triangle[value][0] += zig[value][0];
		triangle[value][3] += zig[value][0];
		triangle[value][6] += zig[value][0];
		if (zig[value][0] < 0) {
			if (triangle[value][3] < -1) {
				++zigzag_count[value];
				triangle[value][0] = triangle[value][3];
				triangle[value][1] = triangle[value][4] - r[value];
				triangle[value][6] = triangle[value][3] + h[value] / 2;
				triangle[value][7] = triangle[value][4] - r[value] / 2;
				zig[value][0] *= -1;
				if (triangle[value][4] < -1) {
					zig[value][1] *= -1;
				}
			}
		}
		else {
			if (triangle[value][3] > 1) {
				++zigzag_count[value];
				triangle[value][0] = triangle[value][3];
				triangle[value][1] = triangle[value][4] - r[value];
				triangle[value][6] = triangle[value][3] - h[value] / 2;
				triangle[value][7] = triangle[value][4] - r[value] / 2;
				zig[value][0] *= -1;
				if (triangle[value][4] > 1) {
					zig[value][1] *= -1;
				}
			}

		}
	}
	else {
		++zigzag_count[value];
		triangle[value][1] += zig[value][1];
		triangle[value][4] += zig[value][1];
		triangle[value][7] += zig[value][1];
		if (zig[value][1] > 0)
		{
			if (triangle[value][4] > 1) {
				zigzag_count[value] = 100;
				zig[value][1] *= -1;
			}
		}
		else if (zig[value][1] < 0) {
			if (triangle[value][4] < -1) {
				zigzag_count[value] = 100;
				zig[value][1] *= -1;
			}
		}
		if (zigzag_count[value] > 100) {
			if (triangle[value][6] < triangle[value][0]) {
				triangle[value][3] = triangle[value][0] - r[value];
				triangle[value][4] = triangle[value][1];
				triangle[value][6] = triangle[value][0] - r[value] / 2;
				triangle[value][7] = triangle[value][1] - h[value];
			}
			else
			{
				triangle[value][3] = triangle[value][0] + r[value];
				triangle[value][4] = triangle[value][1];
				triangle[value][6] = triangle[value][0] + r[value] / 2;
				triangle[value][7] = triangle[value][1] + h[value];
			}
			zigzag_count[value] = 0;
		}
	}
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), triangle, GL_DYNAMIC_DRAW);
	glutPostRedisplay();
	if (mode_timer == 2) {
		glutTimerFunc(10, zigzag, value);
	}
}
void rec_spiral(int value)
{
	if (way[value] == 0) {
		triangle[value][0] += Plus_Min[0];
		triangle[value][3] += Plus_Min[0];
		triangle[value][6] += Plus_Min[0];
	}
	else if (way[value] == 1) {
		triangle[value][1] += Plus_Min[1];
		triangle[value][4] += Plus_Min[1];
		triangle[value][7] += Plus_Min[1];
	}
	else if (way[value] == 2) {
		triangle[value][0] -= Plus_Min[0];
		triangle[value][3] -= Plus_Min[0];
		triangle[value][6] -= Plus_Min[0];
	}
	else if (way[value] == 3) {
		triangle[value][1] -= Plus_Min[1];
		triangle[value][4] -= Plus_Min[1];
		triangle[value][7] -= Plus_Min[1];
	}
	++count[value];
	if (count[value] > max_count[value]) {
		max_count[value] += 10;
		++way[value];
		count[value] = 0;
		if (way[value] > 3)
		{
			way[value] = 0;
		}
	}
	if (max_count[value] > 300) {
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 9; ++j)
				triangle[i][j] = pos_triangle[i][j];
		max_count[value] = 10;
	}
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), triangle, GL_DYNAMIC_DRAW);
	glutPostRedisplay();
	if (mode_timer == 3) {
		glutTimerFunc(1, rec_spiral, value);
	}
}
void circle_spiral(int value)
{
	for (int i = 0; i < 3; i++) {
		triangle[value][i * 3] = triangle[value][i * 3] + cos(angle[value] * PI / 180) * radius[value];
		triangle[value][i * 3+1] = triangle[value][i * 3 + 1] + sin(angle[value] * PI / 180) * radius[value];
	}
	angle[value]+=10;
	radius[value] += 0.001f;
	if (angle[value] > 1500)
	{
		radius[value] = 0.001f;
		angle[value] = 0;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 9; ++j)
				triangle[i][j] = pos_triangle[i][j];
	}
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), triangle, GL_DYNAMIC_DRAW);
	glutPostRedisplay();
	if (mode_timer == 4) {
		glutTimerFunc(10, circle_spiral, value);
	}
	else
	{
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 9; ++j)
				triangle[i][j] = pos_triangle[i][j];
	}
}