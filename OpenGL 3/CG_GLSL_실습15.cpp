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
void SpecialKeyboard(int, int, int);
void rotate_x(int);
void rotate_y(int);


GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID;
GLuint vao, cubevbo[2], cube_ebo, tetravbo[2], tetra_ebo;

GLfloat cube[8][3] = {
	{ -side_length, side_length, side_length},
	{ -side_length, -side_length, side_length},
	{ side_length, -side_length, side_length},
	{ side_length, side_length, side_length},
	{ -side_length, side_length, -side_length},
	{ -side_length, -side_length, -side_length},
	{ side_length, -side_length, -side_length},
	{ side_length, side_length, -side_length},
};

unsigned int cube_index[] = {
	0,1,2,
	2,6,7,
	7,6,5,
	5,4,7,
	7,3,2,
	2,1,5,
	5,6,2,
	2,3,0,
	0,3,7,
	7,4,0,
	0,4,5,
	5,1,0
};
GLfloat cube_RGB[6][6][3];
GLfloat tetra[5][3] = {
	{ -side_length, 0, side_length},
	{ -side_length, 0, -side_length},
	{ side_length, 0, -side_length},
	{ side_length, 0, side_length},
	{ 0, 2*side_length, 0},
};
unsigned int tetra_index[] = {
	0,1,3,
	3,1,2,
	4,1,2,
	4,0,1,
	4,2,3,
	4,3,0
};
GLfloat tetra_RGB[5][3];

GLuint vbo_line[2];
GLfloat line[2][6];
GLfloat line_RGB[2][6];

bool cube_draw;
bool tetra_draw;
bool depth_test;
bool triangle;
bool timer_x;
bool timer_y;
float x;
float y;
float addx;
float addy;
float movex;
float movey;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> color(0, 1);

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(Width, Height);
	glutCreateWindow("Example1");
	glewExperimental = GL_TRUE;
	glewInit();
	make_shaderProgram();
	x = 30.0f;
	y = 30.0f;
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
	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 3; ++j)
			tetra_RGB[i][j] = color(gen);
	line[0][1] = 1;
	line[0][4] = -1;
	line[1][0] = 1;
	line[1][3] = -1;
	InitBuffer();
	//--- 세이더 읽어와서 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutMainLoop();
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	GLfloat rColor, gColor, bColor;
	rColor = gColor = bColor = 0.8f;
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	if(depth_test)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
	if (cube_draw) {
		trans = glm::translate(trans, glm::vec3(movex, movey, 0.0f));
		trans = glm::rotate(trans, glm::radians(x), glm::vec3(1.0f, 0.0f, 0.0f));
		trans = glm::rotate(trans, glm::radians(y), glm::vec3(0.0f, 1.0f, 0.0f));
		unsigned int trans_location = glGetUniformLocation(shaderProgramID, "transform");
		glUniformMatrix4fv(trans_location, 1, GL_FALSE, glm::value_ptr(trans));
		glBindBuffer(GL_ARRAY_BUFFER, cubevbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, cubevbo[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		if(triangle)
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		else
			glDrawElements(GL_LINE_STRIP, 36, GL_UNSIGNED_INT, 0);
	}
	else if (tetra_draw) {
		trans = glm::translate(trans, glm::vec3(movex, movey, 0.0f));
		trans = glm::rotate(trans, glm::radians(x), glm::vec3(1.0f, 0.0f, 0.0f));
		trans = glm::rotate(trans, glm::radians(y), glm::vec3(0.0f, 1.0f, 0.0f));	
		trans = glm::translate(trans, glm::vec3(0.0f, -0.1f, 0.0f));

		unsigned int trans_location = glGetUniformLocation(shaderProgramID, "transform");
		glUniformMatrix4fv(trans_location, 1, GL_FALSE, glm::value_ptr(trans));
		glBindBuffer(GL_ARRAY_BUFFER, tetravbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, tetravbo[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		if(triangle)
			glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
		else
			glDrawElements(GL_LINE_STRIP, 18, GL_UNSIGNED_INT, 0);
	}


	glDisableVertexAttribArray(ColorLocation);
	glDisableVertexAttribArray(PosLocation);


	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}
void make_vertexShaders()
{
	vertexSource = filetobuf("vertex.glsl");
	//--- 버텍스 세이더 객체 만들기
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- 버텍스 세이더 컴파일하기
	glCompileShader(vertexShader);
	//--- 컴파일이 제대로 되지 않은 경우: 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cout << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");
	//--- 프래그먼트 세이더 객체 만들기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- 프래그먼트 세이더 컴파일
	glCompileShader(fragmentShader);
	//--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}
void make_shaderProgram()
{
	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	//-- shader Program
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);
	//--- 세이더 삭제하기
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//--- Shader Program 사용하기
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
	glGenBuffers(1, &tetra_ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_index), cube_index, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_RGB), cube_RGB, GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, tetravbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tetra), tetra, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tetra_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tetra_index), tetra_index, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, tetravbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tetra_RGB), tetra_RGB, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_RGB), line_RGB, GL_STATIC_DRAW);
}
GLvoid Keyboard(unsigned char key, int x, int y)
{


	switch (key) {
	case 'c':
		cube_draw = true;
		tetra_draw = false;
		break;
	case 'p':
		cube_draw = false;
		tetra_draw = true;
		break;
	case 'h':
		if (depth_test)
			depth_test = false;
		else
			depth_test = true;
		break;
	case 'w':
	case 'W':
		if (triangle)
			triangle = false;
		else
			triangle = true;
		break;
	case 'x':
		if (!timer_x)
		{
			timer_x = true;
			glutTimerFunc(50, rotate_x, 0);
		}
		addx = 1.0f;
		break;
	case 'X':
		if (!timer_x) {
			timer_x = true;
			glutTimerFunc(50, rotate_x, 0);
		}
		addx = -1.0f;
		break;
	case 'y':
		if (!timer_y) {
			timer_y = true;
			glutTimerFunc(50, rotate_y, 0);
		}
		addy = 1.0f;
		break;
	case 'Y':
		if (!timer_y) {
			timer_y = true;
			glutTimerFunc(50, rotate_y, 0);
		}
		addy = -1.0f;
		break;
	case 's':
		cube_draw = false;
		tetra_draw = false;
		depth_test = false;
		triangle = false;
		timer_x = false;
		timer_y = false;
		movex = 0;
		movey = 0;
		x=30.0f;
		y=30.0f;
		break;

	}
	if (cube_draw)
	{
		glBindBuffer(GL_ARRAY_BUFFER, cubevbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_index), cube_index, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, cubevbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_RGB), cube_RGB, GL_STATIC_DRAW);
	}
	else if (tetra_draw) {
		glBindBuffer(GL_ARRAY_BUFFER, tetravbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tetra), tetra, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tetra_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tetra_index), tetra_index, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, tetravbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tetra_RGB), tetra_RGB, GL_STATIC_DRAW);
	}
	glutPostRedisplay();
}
void rotate_x(int value){
	x += addx;
	if (timer_x)
		glutTimerFunc(1, rotate_x, value);
	glutPostRedisplay();
}
void rotate_y(int value) {
	y += addy;
	if (timer_y)
		glutTimerFunc(1, rotate_y, value);
	glutPostRedisplay();
}
void SpecialKeyboard(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_LEFT:
		movex -= 0.05f;
		break;
	case GLUT_KEY_RIGHT:
		movex += 0.05f;
		break;
	case GLUT_KEY_UP:
		movey += 0.05f;
		break;
	case GLUT_KEY_DOWN:
		movey -= 0.05f;
		break;
	}
	glutPostRedisplay();
}