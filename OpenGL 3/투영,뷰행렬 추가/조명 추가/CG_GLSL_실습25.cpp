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
GLuint vao, cubevbo[2], tetravbo[2], tetra_ebo;

GLfloat cube[6][12] = {
	{ -side_length, -side_length, side_length,
	  -side_length,  side_length, side_length,	//앞
	   side_length,  side_length, side_length,
	   side_length, -side_length, side_length},

	{ -side_length, -side_length,  -side_length,
	  -side_length,  side_length,  -side_length,	//오른쪽
	  -side_length,  side_length, side_length,
	  -side_length, -side_length, side_length },

	{  side_length, -side_length, side_length,
	   side_length,  side_length, side_length,	//왼쪽
	   side_length,  side_length,  -side_length,
	   side_length, -side_length,  -side_length },

	{  side_length, -side_length,  -side_length,
	   side_length,  side_length,  -side_length,	//뒤
	  -side_length,  side_length,  -side_length,
	  -side_length, -side_length,  -side_length },

	{ -side_length,  side_length,  side_length,
	  -side_length,  side_length,  -side_length,	//위
	   side_length,  side_length,  -side_length,
	   side_length,  side_length,  side_length},

	{ -side_length, -side_length,  -side_length,
	  -side_length, -side_length, side_length,	//아래
	   side_length, -side_length, side_length,
	   side_length, -side_length,  -side_length },
};
const float cube_normal[] = {
	0,0,1,0,0,1,0,0,1,0,0,1,
	1,0,0,1,0,0,1,0,0,1,0,0,
	-1,0,0,-1,0,0,-1,0,0,-1,0,0,
	0,0,-1,0,0,-1,0,0,-1,0,0,-1,
	0,1,0,0,1,0,0,1,0,0,1,0,
	0,-1,0,0,-1,0,0,-1,0,0,-1,0,
};

GLuint cube_normal_vbo;
GLfloat cube_RGB[6][12];

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
const float tetra_normal[] = {
	0,-1,0,0,-1,0,0,-1,0,
	0,-1,0,0,-1,0,0,-1,0,
	0,0,-1,0,0,-1,0,0,-1,
	-1,0,0,-1,0,0,-1,0,0,
	1,0,0,1,0,0,1,0,0,
	0,0,1,0,0,1,0,0,1,

};
GLuint tetra_normal_vbo;

GLfloat tetra_RGB[5][3];

GLuint vbo_line[2];
GLfloat line[2][6];
GLfloat line_RGB[2][6];

bool cube_draw;
bool tetra_draw;
bool timer_x;
bool timer_y;
float x_1;
float y_1;
float addx_1;
float addy_1;
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
	x_1 = 0.0f;
	y_1 = 0.0f;
	for (int i = 0; i < 6; ++i)
	{
		cube_RGB[i][0] = color(gen);
		cube_RGB[i][1] = color(gen);
		cube_RGB[i][2] = color(gen);


		for (int k = 1; k < 4; ++k) {
			cube_RGB[i][k * 3] = cube_RGB[i][0];
			cube_RGB[i][k * 3 + 1] = cube_RGB[i][1];
			cube_RGB[i][k * 3 + 2] = cube_RGB[i][2];
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
	rColor = gColor = bColor = 0.0f;
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	int NormalLocation = glGetAttribLocation(shaderProgramID, "in_Normal"); //	: 2
	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation);
	glEnableVertexAttribArray(NormalLocation);


	unsigned int proj_location = glGetUniformLocation(shaderProgramID, "projection");
	unsigned int view_location = glGetUniformLocation(shaderProgramID, "view");

	//투영행렬
	glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
	proj = glm::translate(proj, glm::vec3(0.f, 0.f, -5.0f));
	glUniformMatrix4fv(proj_location, 1, GL_FALSE, &proj[0][0]);

	//뷰 행렬
	glm::vec3 cameraPos = glm::vec3(0.f, 1.f, 3.f); //--- 카메라 위치
	glm::vec3 cameraDirection = glm::vec3(0.f, 0.f, 0.f); //--- 카메라 바라보는 방향
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	view = glm::rotate(view, glm::radians(30.f), glm::vec3(0.f, 1.f, 0.f));
	glUniformMatrix4fv(view_location, 1, GL_FALSE, &view[0][0]);



	unsigned int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos");
	glUniform3f(lightPosLocation, 0.f, 0.f, 0.4f);
	unsigned int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor");
	glUniform3f(lightColorLocation, 1.0, 1.0, 1.0);
	unsigned int objectColorLocation = glGetUniformLocation(shaderProgramID, "objColor");
	glUniform3f(objectColorLocation, 0.5, 1.0, 0.0);
	unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos");
	glUniform3f(viewPosLocation, cameraPos.x, cameraPos.y, cameraPos.z);

	glFrontFace(GL_CW);
	glm::mat4 trans = glm::mat4(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	if (tetra_draw) {
		trans = glm::translate(trans, glm::vec3(movex, movey, 0.0f));
		trans = glm::rotate(trans, glm::radians(x_1), glm::vec3(1.0f, 0.0f, 0.0f));
		trans = glm::rotate(trans, glm::radians(y_1), glm::vec3(0.0f, 1.0f, 0.0f));	
		trans = glm::translate(trans, glm::vec3(0.0f, -0.1f, 0.0f));

		unsigned int trans_location = glGetUniformLocation(shaderProgramID, "transform");
		glUniformMatrix4fv(trans_location, 1, GL_FALSE, glm::value_ptr(trans));
		glBindBuffer(GL_ARRAY_BUFFER, tetravbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, tetravbo[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, tetra_normal_vbo);
		glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
	}
	else {
		trans = glm::translate(trans, glm::vec3(movex, movey, 0.0f));
		trans = glm::rotate(trans, glm::radians(x_1), glm::vec3(1.0f, 0.0f, 0.0f));
		trans = glm::rotate(trans, glm::radians(y_1), glm::vec3(0.0f, 1.0f, 0.0f));
		unsigned int trans_location = glGetUniformLocation(shaderProgramID, "transform");
		glUniformMatrix4fv(trans_location, 1, GL_FALSE, glm::value_ptr(trans));
		glBindBuffer(GL_ARRAY_BUFFER, cubevbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, cubevbo[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, cube_normal_vbo);
		glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), 0);
		for (int i = 0; i < 6; ++i) {
			glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
		}
	}


	glUniform3f(objectColorLocation, 0.5, 0.5, 0.5);
	glm::mat4 camera_box = glm::mat4(1.0f);
	camera_box = glm::translate(camera_box, glm::vec3(0.2f, 0.f, 0.5f));
	camera_box = glm::scale(camera_box, glm::vec3(0.3, 0.3, 0.3));
	unsigned int cameras_location = glGetUniformLocation(shaderProgramID, "transform");
	glUniformMatrix4fv(cameras_location, 1, GL_FALSE, glm::value_ptr(camera_box));
	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[0]);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[1]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(2 * 12 * sizeof(GLfloat)));
	glBindBuffer(GL_ARRAY_BUFFER, cube_normal_vbo);
	glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	for (int i = 0; i < 6; ++i) {
		glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
	}

	glDisableVertexAttribArray(ColorLocation);
	glDisableVertexAttribArray(PosLocation);
	glDisableVertexAttribArray(NormalLocation);


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
	glGenBuffers(1, &tetra_ebo);
	glGenBuffers(1, &cube_normal_vbo);
	glGenBuffers(1, &tetra_normal_vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_DYNAMIC_DRAW);

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

	glBindBuffer(GL_ARRAY_BUFFER, cube_normal_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_normal), cube_normal, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, tetra_normal_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tetra_normal), tetra_normal, GL_STATIC_DRAW);

}
GLvoid Keyboard(unsigned char key, int x, int y)
{


	switch (key) {
	case 'n':
		if (cube_draw) {
			cube_draw = false;
			tetra_draw = true;
		}
		else {
			cube_draw = true;
			tetra_draw = false;
		}
		break;
	case 'x':
		if (!timer_x)
		{
			timer_x = true;
			glutTimerFunc(50, rotate_x, 0);
		}
		else {
			timer_x = false;
		}
		addx_1 = 1.0f;
		break;
	case 'X':
		if (!timer_x) {
			timer_x = true;
			glutTimerFunc(50, rotate_x, 0);
		}
		else {
			timer_x = false;
		}
		addx_1 = -1.0f;
		break;
	case 'y':
		if (!timer_y) {
			timer_y = true;
			glutTimerFunc(50, rotate_y, 0);
		}
		else {
			timer_y = false;
		}
		addy_1 = 1.0f;
		break;
	case 'Y':
		if (!timer_y) {
			timer_y = true;
			glutTimerFunc(50, rotate_y, 0);
		}
		else {
			timer_y = false;
		}
		addy_1 = -1.0f;
		break;
	case 's':
		cube_draw = false;
		tetra_draw = false;
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
	x_1 += addx_1;
	if (timer_x)
		glutTimerFunc(1, rotate_x, value);
	glutPostRedisplay();
}
void rotate_y(int value) {
	y_1 += addy_1;
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