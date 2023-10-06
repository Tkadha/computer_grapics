#define  _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <stdlib.h>
#include <random>

#define Width 1200
#define Height 800

void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char* file);
void Mouse(int, int, int, int);
void move(int, int);
void Setting();
void diagonal(int value);
void bump(int,GLfloat,GLfloat);

GLchar* vertexSource, * fragmentSource; //--- �ҽ��ڵ� ���� ����
GLuint vertexShader, fragmentShader; //--- ���̴� ��ü
GLuint shaderProgramID;
GLuint vao, vbo[2];
GLfloat shape[15][5][3];
GLfloat RGB[15][15];
int shape_type[15];
GLfloat diag[15][2];

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dist(-0.9, 0.9);
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
	for (int i = 0; i < 15; ++i)
		for (int j = 0; j < 2; ++j)
			diag[i][j] = 0.005f;
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
	glutMouseFunc(Mouse);
	glutMotionFunc(move);
	glutMainLoop();
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	GLfloat rColor, gColor, bColor;
	rColor = gColor = bColor = 1.0f;
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
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 15 * sizeof(GLfloat)));
		switch(shape_type[i]) {
		case 0:
			glPointSize(10);
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
		default:
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
int select_shape;
int select_shape_type;
bool select[15];
bool ontimer[15];
bool playtimer[15];
GLfloat slope_point2;
void Mouse(int button, int state, int x, int y)
{
	GLfloat halfx = Width / 2;
	GLfloat halfy = Height / 2;
	GLclampf mousex = (x - halfx) / halfx;
	GLclampf mousey = 1 - (y / halfy);
	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {
			for (int i = 0; i < 15; ++i) {
				switch (shape_type[i]) {
				case 0:
					if ((mousex >= shape[i][0][0] - 0.03f) && (mousex <= shape[i][0][0] + 0.03f)) {
						if ((mousey >= shape[i][0][1] - 0.03f) && (mousey <= shape[i][0][1] + 0.03f)) {
							select_shape = i;
							select[i] = true;
							select_shape_type = 0;
						}
					}
					break;
				case 1:
					if ((mousey - shape[i][0][1] >= mousex - shape[i][0][0]-0.03f)&&(mousey - shape[i][0][1] <= mousex - shape[i][0][0] + 0.03f)) {
						select_shape = i;
						select[i] = true;
						select_shape_type = 1;
					}
					break;
				case 2:
					if ((mousex >= shape[i][0][0]) && (mousex <= shape[i][0][0] + 0.1f)) {
						if ((mousey >= shape[i][0][1]) && (mousey <= shape[i][0][1] + 0.1f)) {
							select_shape = i;
							select[i] = true;
							select_shape_type = 2;
						}
					}
					break;
				case 3:
					if ((mousex >= shape[i][0][0]) && (mousex <= shape[i][0][0] + 0.1f)) {
						if ((mousey >= shape[i][0][1]) && (mousey <= shape[i][0][1] + 0.1f)) {
							select_shape = i;
							select[i] = true;
							select_shape_type = 3;
						}
					}
					break;
				case 4:
					if ((mousex >= shape[i][0][0]) && (mousex <= shape[i][0][0] + 0.08f)) {
						if ((mousey >= shape[i][0][1]) && (mousey <= shape[i][0][1] + 0.08f)) {
							select_shape = i;
							select[i] = true;
							select_shape_type = 4;
						}
					}
					break;
				}
				if (select[select_shape])
					break;
			}
		}
	}
	else if (state == GLUT_UP) {
		if (button == GLUT_LEFT_BUTTON) {
			select[select_shape] = false;
			for (int i = 0; i < 15; ++i) {
				if(i!= select_shape)
					bump(i,mousex,mousey);
			}
			for (int i = 0; i < 15; ++i) {
				if (ontimer[i])
					if(!playtimer[i])
						if(shape_type[i]!=10)
							glutTimerFunc(50, diagonal, i);
			}
		}
	}
}
void move(int x, int y) {
	GLfloat halfx = Width / 2;
	GLfloat halfy = Height / 2;
	GLclampf mousex = (x - halfx) / halfx;
	GLclampf mousey = 1 - (y / halfy);
	if (select[select_shape]) {
		switch (select_shape_type) {
		case 0:
			shape[select_shape][0][0] = mousex;
			shape[select_shape][0][1] = mousey;
			break;
		case 1:
			shape[select_shape][0][0] = mousex - 0.05f;
			shape[select_shape][0][1] = mousey - 0.05f;
			shape[select_shape][1][0] = shape[select_shape][0][0] + 0.1f;
			shape[select_shape][1][1] = shape[select_shape][0][1] + 0.1f;
			break;
		case 2:
			shape[select_shape][0][0] = mousex - 0.05f;
			shape[select_shape][0][1] = mousey - 0.05f;
			shape[select_shape][1][0] = mousex + 0.05f;
			shape[select_shape][1][1] = mousey - 0.05f;
			shape[select_shape][2][0] = mousex;
			shape[select_shape][2][1] = mousey + 0.05f;
			break;
		case 3:
			shape[select_shape][0][0] = mousex - 0.05f;
			shape[select_shape][0][1] = mousey - 0.05f;
			shape[select_shape][1][0] = mousex + 0.05f;
			shape[select_shape][1][1] = mousey - 0.05f;
			shape[select_shape][2][0] = mousex + 0.05f;
			shape[select_shape][2][1] = mousey + 0.05f;
			shape[select_shape][3][0] = mousex - 0.05f;
			shape[select_shape][3][1] = mousey + 0.05f;
			break;
		case 4:
			shape[select_shape][0][0] = mousex - 0.03f;
			shape[select_shape][0][1] = mousey - 0.04f;
			shape[select_shape][1][0] = shape[select_shape][0][0] + 0.06f;
			shape[select_shape][1][1] = shape[select_shape][0][1];
			shape[select_shape][2][0] = shape[select_shape][0][0] + 0.08f;
			shape[select_shape][2][1] = shape[select_shape][0][1] + 0.06f;
			shape[select_shape][3][0] = shape[select_shape][0][0] + 0.03f;
			shape[select_shape][3][1] = shape[select_shape][0][1] + 0.1f;
			shape[select_shape][4][0] = shape[select_shape][0][0] - 0.02f;
			shape[select_shape][4][1] = shape[select_shape][0][1] + 0.06f;
			break;
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(shape), shape, GL_DYNAMIC_DRAW);
	glutPostRedisplay();
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

void diagonal(int value) {
	playtimer[value] = true;
	for (int i = 0; i < 5; ++i) {
		shape[value][i][0] += diag[value][0];
		shape[value][i][1] += diag[value][1];
	}
	switch (shape_type[value]) {
	case 0:
		if (shape[value][0][0] >= 1)
		{
			if (diag[value][0] > 0)
				diag[value][0] *= -1;
		}
		else if (shape[value][0][0] <= -1)
		{
			if (diag[value][0] < 0)
				diag[value][0] *= -1;
		}
		if (shape[value][0][1] >= 1)
		{
			if (diag[value][1] > 0)
				diag[value][1] *= -1;
		}
		else if (shape[value][0][1] <= -1)
		{
			if (diag[value][1] < 0)
				diag[value][1] *= -1;
		}
		break;
	case 1:
		if ((shape[value][0][0] >= 1) || (shape[value][1][0] >= 1))
		{
			if (diag[value][0] > 0)
				diag[value][0] *= -1;
		}
		else if ((shape[value][0][0] <= -1) || (shape[value][1][0] <= -1))
		{
			if (diag[value][0] < 0)
				diag[value][0] *= -1;
		}
		if ((shape[value][0][1] >= 1) || (shape[value][1][1] >= 1))
		{
			if (diag[value][1] > 0)
				diag[value][1] *= -1;
		}
		else if ((shape[value][0][1] <= -1) || (shape[value][1][1] <= -1))
		{
			if (diag[value][1] < 0)
				diag[value][1] *= -1;
		}
		break;
	case 2:
		if ((shape[value][0][0] >= 1) || (shape[value][1][0] >= 1) || (shape[value][2][0] >= 1))
		{
			if (diag[value][0] > 0)
				diag[value][0] *= -1;
		}
		else if ((shape[value][0][0] <= -1) || (shape[value][1][0] <= -1) || (shape[value][2][0] <= -1))
		{
			if (diag[value][0] < 0)
				diag[value][0] *= -1;
		}
		if ((shape[value][0][1] >= 1) || (shape[value][1][1] >= 1) || (shape[value][2][1] >= 1))
		{
			if (diag[value][1] > 0)
				diag[value][1] *= -1;
		}
		else if ((shape[value][0][1] <= -1) || (shape[value][1][1] <= -1) || (shape[value][2][1] <= -1))
		{
			if (diag[value][1] < 0)
				diag[value][1] *= -1;
		}
		break;
	case 3:
		if ((shape[value][0][0] >= 1) || (shape[value][1][0] >= 1) || (shape[value][2][0] >= 1) || (shape[value][3][0] >= 1))
		{
			if (diag[value][0] > 0)
				diag[value][0] *= -1;
		}
		else if ((shape[value][0][0] <= -1) || (shape[value][1][0] <= -1) || (shape[value][2][0] <= -1) || (shape[value][3][0] <= -1))
		{
			if (diag[value][0] < 0)
				diag[value][0] *= -1;
		}
		if ((shape[value][0][1] >= 1) || (shape[value][1][1] >= 1) || (shape[value][2][1] >= 1) || (shape[value][3][1] >= 1))
		{
			if (diag[value][1] > 0)
				diag[value][1] *= -1;
		}
		else if ((shape[value][0][1] <= -1) || (shape[value][1][1] <= -1) || (shape[value][2][1] <= -1) || (shape[value][3][1] <= -1))
		{
			if (diag[value][1] < 0)
				diag[value][1] *= -1;
		}
		break;
	case 4:
		if ((shape[value][0][0] >= 1) || (shape[value][1][0] >= 1) || (shape[value][2][0] >= 1) || (shape[value][3][0] >= 1) || (shape[value][4][0] >= 1))
		{
			if (diag[value][0] > 0)
				diag[value][0] *= -1;
		}
		else if ((shape[value][0][0] <= -1) || (shape[value][1][0] <= -1) || (shape[value][2][0] <= -1) || (shape[value][3][0] <= -1) || (shape[value][4][0] <= -1))
		{
			if (diag[value][0] < 0)
				diag[value][0] *= -1;
		}
		if ((shape[value][0][1] >= 1) || (shape[value][1][1] >= 1) || (shape[value][2][1] >= 1) || (shape[value][3][1] >= 1) || (shape[value][4][1] >= 1))
		{
			if (diag[value][1] > 0)
				diag[value][1] *= -1;
		}
		else if ((shape[value][0][1] <= -1) || (shape[value][1][1] <= -1) || (shape[value][2][1] <= -1) || (shape[value][3][1] <= -1) || (shape[value][4][1] <= -1))
		{
			if (diag[value][1] < 0)
				diag[value][1] *= -1;
		}
		break;
	}
	if (!select[value]) {
		glutTimerFunc(50, diagonal, value);
	}
	else {
		playtimer[value] = false;
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(shape), shape, GL_DYNAMIC_DRAW);
	glutPostRedisplay();
}
void bump(int base,GLfloat mx,GLfloat my) {
	bool catching;
	for (int j = 0; j < select_shape_type + 1; ++j) {
		for (int k = 0; k < shape_type[base] + 1; ++k) {
			if (shape_type[select_shape] == 0) {	//���̸�
				if (shape_type[base] != 10) {
					if (shape_type[base] == 0) {
						if ((shape[select_shape][j][0] >= shape[base][0][0] - 0.03f) && (shape[select_shape][j][0] <= shape[base][0][0] + 0.03f)) {
							if ((shape[select_shape][j][1] >= shape[base][0][1] - 0.03f) && (shape[select_shape][j][1] <= shape[base][0][1] + 0.03f)) {
								//��ġ��
								shape_type[base] = shape_type[base] + select_shape_type + 1;
								shape_type[base] %= 5;
								shape_type[select_shape] = 10;	//�׸��� �ʴ´�.
								switch (shape_type[base]) {
								case 0:
									shape[base][0][0] = mx;
									shape[base][0][1] = my;
									break;
								case 1:
									shape[base][0][0] = mx - 0.05f;
									shape[base][0][1] = my - 0.05f;
									shape[base][1][0] = shape[base][0][0] + 0.1f;
									shape[base][1][1] = shape[base][0][1] + 0.1f;
									break;
								case 2:
									shape[base][0][0] = mx - 0.05f;
									shape[base][0][1] = my - 0.05f;
									shape[base][1][0] = mx + 0.05f;
									shape[base][1][1] = my - 0.05f;
									shape[base][2][0] = mx;
									shape[base][2][1] = my + 0.05f;
									break;
								case 3:
									shape[base][0][0] = mx - 0.05f;
									shape[base][0][1] = my - 0.05f;
									shape[base][1][0] = mx + 0.05f;
									shape[base][1][1] = my - 0.05f;
									shape[base][2][0] = mx + 0.05f;
									shape[base][2][1] = my + 0.05f;
									shape[base][3][0] = mx - 0.05f;
									shape[base][3][1] = my + 0.05f;
									break;
								case 4:
									shape[base][0][0] = mx - 0.03f;
									shape[base][0][1] = my - 0.04f;
									shape[base][1][0] = shape[base][0][0] + 0.06f;
									shape[base][1][1] = shape[base][0][1];
									shape[base][2][0] = shape[base][0][0] + 0.08f;
									shape[base][2][1] = shape[base][0][1] + 0.06f;
									shape[base][3][0] = shape[base][0][0] + 0.03f;
									shape[base][3][1] = shape[base][0][1] + 0.1f;
									shape[base][4][0] = shape[base][0][0] - 0.02f;
									shape[base][4][1] = shape[base][0][1] + 0.06f;
									break;
								}
								select[select_shape] = false;
								ontimer[base] = true;
								catching = true;
								glutTimerFunc(50, diagonal, base);
							}
						}
					}
					else if (shape_type[base] == 1) {
						if ((shape[select_shape][j][1] - shape[base][0][1] >= shape[select_shape][j][0] - shape[base][0][0] - 0.03f) && (shape[select_shape][j][1] - shape[base][0][1] <= shape[select_shape][j][0] - shape[base][0][0] + 0.03f))
						{
							shape_type[base] = shape_type[base] + select_shape_type + 1;
							shape_type[base] %= 5;
							shape_type[select_shape] = 10;	//�׸��� �ʴ´�.
							switch (shape_type[base]) {
							case 0:
								shape[base][0][0] = mx;
								shape[base][0][1] = my;
								break;
							case 1:
								shape[base][0][0] = mx - 0.05f;
								shape[base][0][1] = my - 0.05f;
								shape[base][1][0] = shape[base][0][0] + 0.1f;
								shape[base][1][1] = shape[base][0][1] + 0.1f;
								break;
							case 2:
								shape[base][0][0] = mx - 0.05f;
								shape[base][0][1] = my - 0.05f;
								shape[base][1][0] = mx + 0.05f;
								shape[base][1][1] = my - 0.05f;
								shape[base][2][0] = mx;
								shape[base][2][1] = my + 0.05f;
								break;
							case 3:
								shape[base][0][0] = mx - 0.05f;
								shape[base][0][1] = my - 0.05f;
								shape[base][1][0] = mx + 0.05f;
								shape[base][1][1] = my - 0.05f;
								shape[base][2][0] = mx + 0.05f;
								shape[base][2][1] = my + 0.05f;
								shape[base][3][0] = mx - 0.05f;
								shape[base][3][1] = my + 0.05f;
								break;
							case 4:
								shape[base][0][0] = mx - 0.03f;
								shape[base][0][1] = my - 0.04f;
								shape[base][1][0] = shape[base][0][0] + 0.06f;
								shape[base][1][1] = shape[base][0][1];
								shape[base][2][0] = shape[base][0][0] + 0.08f;
								shape[base][2][1] = shape[base][0][1] + 0.06f;
								shape[base][3][0] = shape[base][0][0] + 0.03f;
								shape[base][3][1] = shape[base][0][1] + 0.1f;
								shape[base][4][0] = shape[base][0][0] - 0.02f;
								shape[base][4][1] = shape[base][0][1] + 0.06f;
								break;
							}
							select[select_shape] = false;
							ontimer[base] = true;
							catching = true;
							glutTimerFunc(50, diagonal, base);
						}
					}
					else {
						if ((shape[select_shape][j][0] >= shape[base][0][0]) && (shape[select_shape][j][0] <= shape[base][0][0] + 0.1f)) {
							if ((shape[select_shape][j][1] >= shape[base][0][1]) && (shape[select_shape][j][1] <= shape[base][0][1] + 0.1f)) {
								//��ġ��
								shape_type[base] = shape_type[base] + select_shape_type + 1;
								shape_type[base] %= 5;
								shape_type[select_shape] = 10;	//�׸��� �ʴ´�.
								switch (shape_type[base]) {
								case 0:
									shape[base][0][0] = mx;
									shape[base][0][1] = my;
									break;
								case 1:
									shape[base][0][0] = mx - 0.05f;
									shape[base][0][1] = my - 0.05f;
									shape[base][1][0] = shape[base][0][0] + 0.1f;
									shape[base][1][1] = shape[base][0][1] + 0.1f;
									break;
								case 2:
									shape[base][0][0] = mx - 0.05f;
									shape[base][0][1] = my - 0.05f;
									shape[base][1][0] = mx + 0.05f;
									shape[base][1][1] = my - 0.05f;
									shape[base][2][0] = mx;
									shape[base][2][1] = my + 0.05f;
									break;
								case 3:
									shape[base][0][0] = mx - 0.05f;
									shape[base][0][1] = my - 0.05f;
									shape[base][1][0] = mx + 0.05f;
									shape[base][1][1] = my - 0.05f;
									shape[base][2][0] = mx + 0.05f;
									shape[base][2][1] = my + 0.05f;
									shape[base][3][0] = mx - 0.05f;
									shape[base][3][1] = my + 0.05f;
									break;
								case 4:
									shape[base][0][0] = mx - 0.03f;
									shape[base][0][1] = my - 0.04f;
									shape[base][1][0] = shape[base][0][0] + 0.06f;
									shape[base][1][1] = shape[base][0][1];
									shape[base][2][0] = shape[base][0][0] + 0.08f;
									shape[base][2][1] = shape[base][0][1] + 0.06f;
									shape[base][3][0] = shape[base][0][0] + 0.03f;
									shape[base][3][1] = shape[base][0][1] + 0.1f;
									shape[base][4][0] = shape[base][0][0] - 0.02f;
									shape[base][4][1] = shape[base][0][1] + 0.06f;
									break;
								}
								select[select_shape] = false;
								ontimer[base] = true;
								catching = true;
								glutTimerFunc(50, diagonal, base);
							}
						}
					}
					if (catching)
						break;
				}
			}
			else {
				if (shape_type[base] != 10) {
					if ((shape[select_shape][j][0] >= shape[base][0][0]) && (shape[select_shape][j][0] <= shape[base][0][0] + 0.1f)) {
						if ((shape[select_shape][j][1] >= shape[base][0][1]) && (shape[select_shape][j][1] <= shape[base][0][1] + 0.1f)) {
							//��ġ��
							shape_type[base] = shape_type[base] + select_shape_type + 1;
							shape_type[base] %= 5;
							shape_type[select_shape] = 10;	//�׸��� �ʴ´�.
							switch (shape_type[base]) {
							case 0:
								shape[base][0][0] = mx;
								shape[base][0][1] = my;
								break;
							case 1:
								shape[base][0][0] = mx - 0.05f;
								shape[base][0][1] = my - 0.05f;
								shape[base][1][0] = shape[base][0][0] + 0.1f;
								shape[base][1][1] = shape[base][0][1] + 0.1f;
								break;
							case 2:
								shape[base][0][0] = mx - 0.05f;
								shape[base][0][1] = my - 0.05f;
								shape[base][1][0] = mx + 0.05f;
								shape[base][1][1] = my - 0.05f;
								shape[base][2][0] = mx;
								shape[base][2][1] = my + 0.05f;
								break;
							case 3:
								shape[base][0][0] = mx - 0.05f;
								shape[base][0][1] = my - 0.05f;
								shape[base][1][0] = mx + 0.05f;
								shape[base][1][1] = my - 0.05f;
								shape[base][2][0] = mx + 0.05f;
								shape[base][2][1] = my + 0.05f;
								shape[base][3][0] = mx - 0.05f;
								shape[base][3][1] = my + 0.05f;
								break;
							case 4:
								shape[base][0][0] = mx - 0.03f;
								shape[base][0][1] = my - 0.04f;
								shape[base][1][0] = shape[base][0][0] + 0.06f;
								shape[base][1][1] = shape[base][0][1];
								shape[base][2][0] = shape[base][0][0] + 0.08f;
								shape[base][2][1] = shape[base][0][1] + 0.06f;
								shape[base][3][0] = shape[base][0][0] + 0.03f;
								shape[base][3][1] = shape[base][0][1] + 0.1f;
								shape[base][4][0] = shape[base][0][0] - 0.02f;
								shape[base][4][1] = shape[base][0][1] + 0.06f;
								break;
							}
							select[select_shape] = false;
							ontimer[base] = true;
							catching = true;
							glutTimerFunc(50, diagonal, base);
						}
						if (catching)
							break;
					}
				}
			}
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(shape), shape, GL_DYNAMIC_DRAW);
	glutPostRedisplay();
}