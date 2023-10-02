#define  _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <stdlib.h>
#include <random>

#define Width 1200
#define Height 800

void reset_setting();
void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char* file);
GLvoid Keyboard(unsigned char key, int x, int y);

GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID;
GLuint vao, vbo[2], onevbo[2];
GLuint vbo_line[2];
GLfloat shape[4][3][9];
GLfloat one_shape[3][9];
GLfloat one_RGB[27];
GLfloat RGB[4][27];
GLfloat line[2][6];
GLfloat line_RGB[2][6];
bool draw;
int shape_type[4];
int one_shape_type;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dist(0, 1);
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(Width, Height);
	glutCreateWindow("Example1");
	glewExperimental = GL_TRUE;
	glewInit();
	make_shaderProgram();
	for (int i = 0; i < 4; ++i) {
		RGB[i][24] = RGB[i][21] = RGB[i][18] = RGB[i][15] = RGB[i][12] = RGB[i][9] = RGB[i][6] = RGB[i][3] = RGB[i][0] = dist(gen);
		RGB[i][25] = RGB[i][22] = RGB[i][19] = RGB[i][16] = RGB[i][13] = RGB[i][10] = RGB[i][7] = RGB[i][4] = RGB[i][1] = dist(gen);
		RGB[i][26] = RGB[i][23] = RGB[i][20] = RGB[i][17] = RGB[i][14] = RGB[i][11] = RGB[i][8] = RGB[i][5] = RGB[i][2] = dist(gen);
	}
	{
		line[0][1] = 1;
		line[0][4] = -1;
		line[1][0] = 1;
		line[1][3] = -1;
	}
	reset_setting();
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbo);
	glGenBuffers(2, onevbo);
	glGenBuffers(2, vbo_line);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(shape), shape, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(RGB), RGB, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_RGB), line_RGB, GL_STATIC_DRAW);
	//--- 세이더 읽어와서 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMainLoop();
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	GLfloat rColor, gColor, bColor;
	rColor = gColor = bColor = 0.5;
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader의 'layout (location = 0)' 부분
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
		glLineWidth(1);
		glDrawArrays(GL_LINES, 0, 2);
	}
	if (!draw) {
		for (int i = 0; i < 4; ++i) {
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 27 * sizeof(GLfloat)));
			glEnableVertexAttribArray(PosLocation);
			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 27 * sizeof(GLfloat)));
			glEnableVertexAttribArray(ColorLocation);
			if (shape_type[i] == 0)
			{
				glLineWidth(4);
				glDrawArrays(GL_LINES, 0, 2);
			}
			else if (shape_type[i] == 1){
				glDrawArrays(GL_TRIANGLES, 0, 3);
			}
			else if (shape_type[i] == 2) {
				glDrawArrays(GL_TRIANGLES, 0, 3);
				glDrawArrays(GL_TRIANGLES, 3, 3);
				glDrawArrays(GL_TRIANGLES, 6, 3);
			}
			else if (shape_type[i] == 3) {
				glDrawArrays(GL_TRIANGLES, 0, 3);
				glDrawArrays(GL_TRIANGLES, 3, 3);
				glDrawArrays(GL_TRIANGLES, 6, 3);
			}
			else if (shape_type[i] == 4)
			{
				glPointSize(5);
				glDrawArrays(GL_POINTS, 0, 1);
			}
		}
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, onevbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(PosLocation);
		glBindBuffer(GL_ARRAY_BUFFER, onevbo[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(ColorLocation);
		if (one_shape_type == 0)
		{
			glLineWidth(4);
			glDrawArrays(GL_LINES, 0, 2);
		}
		else if (one_shape_type == 1) {
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
		else if (one_shape_type == 2) {
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glDrawArrays(GL_TRIANGLES, 3, 3);
			glDrawArrays(GL_TRIANGLES, 6, 3);
		}
		else if (one_shape_type == 3) {
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glDrawArrays(GL_TRIANGLES, 3, 3);
			glDrawArrays(GL_TRIANGLES, 6, 3);
		}
		else if (one_shape_type == 4)
		{
			glPointSize(5);
			glDrawArrays(GL_POINTS, 0, 1);
		}
	}
	glDisableVertexAttribArray(PosLocation); // Disable 필수!
	glDisableVertexAttribArray(ColorLocation);

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
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'a':
		draw = false;
		reset_setting();
		break;
	case 'l':
		draw = true;
		reset_setting();
		one_shape_type = shape_type[0];
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 9; ++j) {
				one_shape[i][j] = shape[0][i][j];
			}
		}
		for (int i = 0; i < 27; ++i) {
			one_RGB[i] = RGB[0][i];
		}
		break;
	case 't':
		draw = true;
		reset_setting();
		one_shape_type = shape_type[1];
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 9; ++j) {
				one_shape[i][j] = shape[1][i][j];
			}
		}
		for (int i = 0; i < 27; ++i) {
			one_RGB[i] = RGB[1][i];
		}
		break;
	case 'r':
		draw = true;
		reset_setting();
		one_shape_type = shape_type[2];
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 9; ++j) {
				one_shape[i][j] = shape[2][i][j];
			}
		}
		for (int i = 0; i < 27; ++i) {
			one_RGB[i] = RGB[2][i];
		}
		break;
	case 'p':
		draw = true;
		reset_setting();
		one_shape_type = shape_type[3];
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 9; ++j) {
				one_shape[i][j] = shape[3][i][j];
			}
		}
		for (int i = 0; i < 27; ++i) {
			one_RGB[i] = RGB[3][i];
		}
		break;
	}
	glBindVertexArray(vao);
	if (draw) {
		glBindBuffer(GL_ARRAY_BUFFER, onevbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(one_shape), one_shape, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, onevbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(one_RGB), one_RGB, GL_STATIC_DRAW);
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(shape), shape, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(RGB), RGB, GL_STATIC_DRAW);
	}
	glutPostRedisplay();
}

void reset_setting()
{
	{
		shape_type[0] = 0;
		{
			shape[0][0][0] = -0.6f;
			shape[0][0][1] = 0.3f;
			shape[0][0][2] = 0.0f;

			shape[0][0][3] = -0.3f;
			shape[0][0][4] = 0.6f;
			shape[0][0][5] = 0.0f;

			shape[0][0][6] = -0.45f;
			shape[0][0][7] = 0.45f;
			shape[0][0][8] = 0.0f;
		}
		{
			shape[0][1][0] = -0.3f;
			shape[0][1][1] = 0.6f;
			shape[0][1][2] = 0.0f;

			shape[0][1][3] = -0.3f;
			shape[0][1][4] = 0.6f;
			shape[0][1][5] = 0.0f;

			shape[0][1][6] = -0.3f;
			shape[0][1][7] = 0.6f;
			shape[0][1][8] = 0.0f;
		}
		{
			shape[0][2][0] = -0.3f;
			shape[0][2][1] = 0.6f;
			shape[0][2][2] = 0.0f;

			shape[0][2][3] = -0.3f;
			shape[0][2][4] = 0.6f;
			shape[0][2][5] = 0.0f;

			shape[0][2][6] = -0.45f;
			shape[0][2][7] = 0.45f;
			shape[0][2][8] = 0.0f;
		}
	}
	{
		shape_type[1] = 1;
		{
			shape[1][0][0] = 0.3f;
			shape[1][0][1] = 0.3f;
			shape[1][0][2] = 0.0f;

			shape[1][0][3] = 0.45f;
			shape[1][0][4] = 0.6f;
			shape[1][0][5] = 0.0f;

			shape[1][0][6] = 0.6f;
			shape[1][0][7] = 0.3f;
			shape[1][0][8] = 0.0f;
		}
		{
			shape[1][1][0] = 0.45f;
			shape[1][1][1] = 0.6f;
			shape[1][1][2] = 0.0f;

			shape[1][1][3] = 0.45f;
			shape[1][1][4] = 0.6f;
			shape[1][1][5] = 0.0f;

			shape[1][1][6] = 0.45f;
			shape[1][1][7] = 0.6f;
			shape[1][1][8] = 0.0f;
		}
		{
			shape[1][2][0] = 0.45f;
			shape[1][2][1] = 0.6f;
			shape[1][2][2] = 0.0f;

			shape[1][2][3] = 0.45f;
			shape[1][2][4] = 0.6f;
			shape[1][2][5] = 0.0f;

			shape[1][2][6] = 0.6f;
			shape[1][2][7] = 0.3f;
			shape[1][2][8] = 0.0f;
		}
	}
	{
		shape_type[2] = 2;
		{
			shape[2][0][0] = -0.6f;
			shape[2][0][1] = -0.6f;
			shape[2][0][2] = 0.0f;

			shape[2][0][3] = -0.6f;
			shape[2][0][4] = -0.3f;
			shape[2][0][5] = 0.0f;

			shape[2][0][6] = -0.3f;
			shape[2][0][7] = -0.6f;
			shape[2][0][8] = 0.0f;
		}
		{
			shape[2][1][0] = -0.6f;
			shape[2][1][1] = -0.3f;
			shape[2][1][2] = 0.0f;

			shape[2][1][3] = -0.45f;
			shape[2][1][4] = -0.3f;
			shape[2][1][5] = 0.0f;

			shape[2][1][6] = -0.3f;
			shape[2][1][7] = -0.3f;
			shape[2][1][8] = 0.0f;
		}
		{
			shape[2][2][0] = -0.6f;
			shape[2][2][1] = -0.3f;
			shape[2][2][2] = 0.0f;

			shape[2][2][3] = -0.3f;
			shape[2][2][4] = -0.3f;
			shape[2][2][5] = 0.0f;

			shape[2][2][6] = -0.3f;
			shape[2][2][7] = -0.6f;
			shape[2][2][8] = 0.0f;
		}
	}
	{
		shape_type[3] = 3;
		{
			shape[3][0][0] = 0.35f;
			shape[3][0][1] = -0.6f;
			shape[3][0][2] = 0.0f;

			shape[3][0][3] = 0.3f;
			shape[3][0][4] = -0.4f;
			shape[3][0][5] = 0.0f;

			shape[3][0][6] = 0.55f;
			shape[3][0][7] = -0.6f;
			shape[3][0][8] = 0.0f;
		}
		{
			shape[3][1][0] = 0.3f;
			shape[3][1][1] = -0.4f;
			shape[3][1][2] = 0.0f;

			shape[3][1][3] = 0.45f;
			shape[3][1][4] = -0.25f;
			shape[3][1][5] = 0.0f;

			shape[3][1][6] = 0.6f;
			shape[3][1][7] = -0.4f;
			shape[3][1][8] = 0.0f;
		}
		{
			shape[3][2][0] = 0.3f;
			shape[3][2][1] = -0.4f;
			shape[3][2][2] = 0.0f;

			shape[3][2][3] = 0.6f;
			shape[3][2][4] = -0.4f;
			shape[3][2][5] = 0.0f;

			shape[3][2][6] = 0.55f;
			shape[3][2][7] = -0.6f;
			shape[3][2][8] = 0.0f;
		}
	}
}