#define  _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/ext.hpp>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext/matrix_transform.hpp>
#include <stdlib.h>
#include <random>

// ������� �߰��ϱ�
// ������ġ �����ϱ�
// ȸ����Ű��
// ���� Ȥ�� �������� �׸���
// ���� 0.5�� ����

#define Width 1200
#define Height 800
#define side_length 0.3

void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char* file);
void InitBuffer();
GLvoid Keyboard(unsigned char key, int x, int y);
GLchar* vertexSource, * fragmentSource; //--- �ҽ��ڵ� ���� ����
GLuint vertexShader, fragmentShader; //--- ���̴� ��ü
GLuint shaderProgramID;
GLuint vao, cubevbo[2],cube_ebo,tetravbo[2];

GLfloat cube[8][3] = {
	{ -side_length, side_length, side_length},
	{ -side_length, -side_length, side_length},
	{ side_length, side_length, side_length},
	{ side_length, -side_length, side_length},
	{ -side_length, side_length, -side_length},
	{ -side_length, -side_length, -side_length},
	{ side_length, side_length, -side_length},
	{ side_length, -side_length, -side_length},
};

unsigned int cube_index[] = {
	0,1,2,
	2,1,3,
	2,3,6,
	6,3,7,
	4,5,0,
	0,5,1,
	6,7,4,
	4,7,5,
	4,0,6,
	6,0,2,
	3,7,1,
	1,7,5
};
GLfloat cube_RGB[6][6][3];
GLfloat tetra[4][9];
GLfloat tetra_RGB[4][3];

GLuint vbo_line[2];
GLfloat line[2][6];
GLfloat line_RGB[2][6];

bool cube_draw;
bool cube_slice_draw[6];
bool tetra_draw;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> color(0, 1);

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(Width, Height);
	glutCreateWindow("Example1");
	glewExperimental = GL_TRUE;
	glewInit();
	make_shaderProgram();
	for (int i = 0; i < 6; ++i) {
		for (int k = 0; k < 3; ++k)
		{
			cube_RGB[i][0][k] = color(gen);
			cube_RGB[i][1][k] = color(gen);
			cube_RGB[i][2][k] = color(gen);
			cube_RGB[i][3][k] = color(gen);
			cube_RGB[i][4][k] = color(gen);
			cube_RGB[i][5][k] = color(gen);
		}
	}
	line[0][1] = 1;
	line[0][4] = -1;
	line[1][0] = 1;
	line[1][3] = -1;
	InitBuffer();
	//--- ���̴� �о�ͼ� ���̴� ���α׷� �����
	glutDisplayFunc(drawScene); //--- ��� �ݹ� �Լ�
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMainLoop();
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	GLfloat rColor, gColor, bColor;
	rColor = gColor = bColor = 0.8f;
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation);

	glm::mat4 lines = glm::mat4(1.0f);
	lines = glm::translate(lines, glm::vec3(0.0f, 0.0f, 0.0f));
	unsigned int lines_location = glGetUniformLocation(shaderProgramID, "transform");
	glUniformMatrix4fv(lines_location, 1, GL_FALSE, glm::value_ptr(lines));

	for (int i = 0; i < 2; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_line[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(PosLocation);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_line[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(ColorLocation);
		glDrawArrays(GL_LINES, 0, 2);
	}

	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, glm::radians(-20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	trans = glm::rotate(trans, glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	unsigned int trans_location = glGetUniformLocation(shaderProgramID, "transform");
	glUniformMatrix4fv(trans_location, 1, GL_FALSE, glm::value_ptr(trans));
	glEnable(GL_DEPTH_TEST);
	if (cube_draw) {
		glBindBuffer(GL_ARRAY_BUFFER, cubevbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, cubevbo[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		for (int i = 0; i < 6; ++i)
			if (cube_slice_draw[i])
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * i * sizeof(GLfloat)));
	}


	glDisableVertexAttribArray(ColorLocation);
	glDisableVertexAttribArray(PosLocation);


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
void InitBuffer() {

	glGenVertexArrays(1, &vao);
	glGenBuffers(2, cubevbo);
	glGenBuffers(2, tetravbo);
	glGenBuffers(2, vbo_line);
	glGenBuffers(1, &cube_ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_index), cube_index, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_RGB), cube_RGB, GL_STATIC_DRAW);

	/*
	glBindBuffer(GL_ARRAY_BUFFER, tetravbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tetra), tetra, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, tetravbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tetra_RGB), tetra_RGB, GL_STATIC_DRAW);
	*/

	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_RGB), line_RGB, GL_STATIC_DRAW);
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	std::uniform_int_distribution<> num(0, 4);
	int number1, number2;
	number1 = num(gen);
	number2 = num(gen);
	while (number1 == number2)
		number2 = num(gen);
	for (int i = 0; i < 6; ++i)
		cube_slice_draw[i] = false;
	switch (key) {
	case '1':
		cube_draw = true;
		tetra_draw = false;
		cube_slice_draw[0] = true;
		break;
	case '2':
		cube_draw = true;
		tetra_draw = false;
		cube_slice_draw[1] = true;
		break;
	case '3':
		cube_draw = true;
		tetra_draw = false;
		cube_slice_draw[2] = true;
		break;
	case '4':
		cube_draw = true;
		tetra_draw = false;
		cube_slice_draw[3] = true;
		break;
	case '5':
		cube_draw = true;
		tetra_draw = false;
		cube_slice_draw[4] = true;
		break;
	case '6':
		cube_draw = true;
		tetra_draw = false;
		cube_slice_draw[5] = true;
		break;

	case '7':
		cube_draw = false;
		tetra_draw = true;
		break;
	case '8':
		cube_draw = false;
		tetra_draw = true;
		break;
	case '9':
		cube_draw = false;
		tetra_draw = true;
		break;
	case '0':
		cube_draw = false;
		tetra_draw = true;
		break;

	case 'c':
		cube_draw = true;
		tetra_draw = false;
		cube_slice_draw[number1] = true;
		cube_slice_draw[number2] = true;
		break;
	case 't':
		cube_draw = false;
		tetra_draw = true;
		break;
	}
	glutPostRedisplay();
}