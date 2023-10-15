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
#define side_length 0.2
#define PI 3.14159265

void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char* file);
void InitBuffer();
GLvoid Keyboard(unsigned char key, int x, int y);
void rotate_center(int);
void scale_shape(int);
void scale_point_shape(int);
void move_rollback(int);
void move_otherpoint(int);
void move_otherupdown(int);
void setting_spiral();
void move_spiral(int);
GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID;
GLuint vao, cubevbo[2], cube_ebo;
GLuint spiralvbo;

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

GLuint vbo_line[2];
GLfloat line[3][6];
GLfloat line_RGB[3][6];

bool cube_draw;
bool sphere_draw;
bool timer_center;
bool scale_xyz;
bool scale_point;

float x_1;
float y_1;
float x_2;
float y_2;
float center;
float addx_1;
float addy_1;
float addx_2;
float addy_2;
float add_center;

float scale_1;
float scale_2;
float add_scale_1;
float add_scale_2;

bool rotate_xy_1;
bool rotate_xy_2;

bool pos1;
bool pos2;
float pos1_x;
float pos1_y;
float pos1_z;
float pos2_x;
float pos2_y;
float pos2_z;

bool rollback;
bool other_point;
bool ud_other;
float movex_1;
float movey_1;
float movez_1;
float movex_2;
float movey_2;
float movez_2;

GLfloat spiral[3003];
bool see_spiral;

GLUquadricObj* cylinderobj;
GLUquadricObj* otherobj;
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
	setting_spiral();
	x_1 = -35.0f;
	y_1 = -10.0f;
	x_2 = -35.0f;
	y_2 = -10.0f;
	cube_draw = true;
	sphere_draw = true;
	scale_1 = 1.0f;
	scale_2 = 1.0f;
	add_scale_1 = 0.05f;
	add_scale_2 = 0.05f;
	add_center = 1.0f;

	pos1_x = -0.5f;
	pos1_y = 0.3f;
	pos1_z = 0.0f;
	pos2_x = 0.5f;
	pos2_y = 0.3f;
	pos2_z = 0.5f;

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
	line_RGB[0][1] = 1;
	line_RGB[0][4] = 1;
	line[1][0] = 1;
	line[1][3] = -1;
	line_RGB[1][0] = 1;
	line_RGB[1][3] = 1;
	line[2][2] = 20;
	line[2][5] = -20;
	line_RGB[2][2] = 1;
	line_RGB[2][5] = 1;
	InitBuffer();
	//--- 세이더 읽어와서 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
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

	if (see_spiral) {
		glm::mat4 spirals = glm::mat4(1.0f);
		spirals = glm::rotate(spirals, glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		spirals = glm::rotate(spirals, glm::radians(-10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		unsigned int spirals_location = glGetUniformLocation(shaderProgramID, "transform");
		glUniformMatrix4fv(spirals_location, 1, GL_FALSE, glm::value_ptr(spirals));
		glBindBuffer(GL_ARRAY_BUFFER, spiralvbo);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(PosLocation);
		glLineWidth(1);
		glDrawArrays(GL_LINE_STRIP, 0, 999);
	}



	glm::mat4 lines = glm::mat4(1.0f);
	lines = glm::rotate(lines, glm::radians(-35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	lines = glm::rotate(lines, glm::radians(-10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	unsigned int lines_location = glGetUniformLocation(shaderProgramID, "transform");
	glUniformMatrix4fv(lines_location, 1, GL_FALSE, glm::value_ptr(lines));

	for (int i = 0; i < 3; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_line[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(PosLocation);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_line[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(ColorLocation);
		glLineWidth(1);
		glDrawArrays(GL_LINES, 0, 2);
	}


	glm::mat4 trans_1 = glm::mat4(1.0f);
	glm::mat4 trans_2 = glm::mat4(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	trans_1 = glm::scale(trans_1, glm::vec3(scale_2, scale_2, scale_2));
	trans_1 = glm::rotate(trans_1, glm::radians(center), glm::vec3(0.0f, 1.0f, 0.0f));
	trans_1 = glm::translate(trans_1, glm::vec3(pos1_x, pos1_y, 0));
	trans_1 = glm::rotate(trans_1, glm::radians(-center), glm::vec3(0.0f, 1.0f, 0.0f));
	trans_1 = glm::rotate(trans_1, glm::radians(x_1), glm::vec3(1.0f, 0.0f, 0.0f));
	trans_1 = glm::rotate(trans_1, glm::radians(y_1), glm::vec3(0.0f, 1.0f, 0.0f));
	trans_1 = glm::translate(trans_1, glm::vec3(0, 0, pos1_z));
	trans_1 = glm::scale(trans_1, glm::vec3(scale_1, scale_1, scale_1));
	unsigned int trans_location = glGetUniformLocation(shaderProgramID, "transform");
	glUniformMatrix4fv(trans_location, 1, GL_FALSE, glm::value_ptr(trans_1));
	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[0]);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[1]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	if (cube_draw)
	{
		glLineWidth(3);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glLineWidth(1);
		cylinderobj = gluNewQuadric();
		gluQuadricDrawStyle(cylinderobj, GLU_LINE);
		gluQuadricNormals(cylinderobj, GLU_SMOOTH);
		gluQuadricOrientation(cylinderobj, GLU_OUTSIDE);
		gluCylinder(cylinderobj, 0.2, 0.2, 0.5, 20, 10);
	}
	trans_2 = glm::scale(trans_2, glm::vec3(scale_2, scale_2, scale_2));
	trans_2 = glm::rotate(trans_2, glm::radians(center), glm::vec3(0.0f, 1.0f, 0.0f));
	trans_2 = glm::translate(trans_2, glm::vec3(pos2_x, pos2_y, 0.0f));
	trans_2 = glm::rotate(trans_2, glm::radians(-center), glm::vec3(0.0f, 1.0f, 0.0f));
	trans_2 = glm::rotate(trans_2, glm::radians(x_2), glm::vec3(1.0f, 0.0f, 0.0f));
	trans_2 = glm::rotate(trans_2, glm::radians(y_2), glm::vec3(0.0f, 1.0f, 0.0f));
	trans_2 = glm::translate(trans_2, glm::vec3(0, 0, pos2_z));
	trans_2 = glm::scale(trans_2, glm::vec3(scale_1, scale_1, scale_1));
	unsigned int trans_location_2 = glGetUniformLocation(shaderProgramID, "transform");
	glUniformMatrix4fv(trans_location_2, 1, GL_FALSE, glm::value_ptr(trans_2));
	if (sphere_draw)
	{
		glLineWidth(1);
		otherobj = gluNewQuadric();
		gluQuadricDrawStyle(otherobj, GLU_LINE);
		gluQuadricNormals(otherobj, GLU_SMOOTH);
		gluQuadricOrientation(otherobj, GLU_OUTSIDE);
		gluSphere(otherobj, 0.2, 20, 20);
	}
	else
	{
		glLineWidth(1);
		otherobj = gluNewQuadric();
		gluQuadricDrawStyle(otherobj, GLU_LINE);
		gluQuadricNormals(otherobj, GLU_SMOOTH);
		gluQuadricOrientation(otherobj, GLU_OUTSIDE);
		gluCylinder(otherobj, 0.2, 0.0, 0.5, 20, 10);
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
	glGenBuffers(2, vbo_line);
	glGenBuffers(1, &cube_ebo);
	glGenBuffers(1, &spiralvbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_index), cube_index, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_RGB), cube_RGB, GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_RGB), line_RGB, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, spiralvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(spiral), spiral, GL_STATIC_DRAW);
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 't':	//원점 도달후 제자리로
		if (!rollback) {
			rollback = true;
			movex_1 = pos1_x / 50;
			movey_1 = pos1_y / 50;
			movez_1 = pos1_z / 50;
			movex_2 = pos2_x / 50;
			movey_2 = pos2_y / 50;
			movez_2 = pos2_z / 50;
			glutTimerFunc(10, move_rollback, 0);
		}
		break;
	case '1':	// 중심점 통과해서 상대방 자리로
		if (!other_point) {
			other_point = true;
			movex_1 = pos1_x / 50;
			movey_1 = pos1_y / 50;
			movez_1 = pos1_z / 50;
			movex_2 = pos2_x / 50;
			movey_2 = pos2_y / 50;
			movez_2 = pos2_z / 50;
			glutTimerFunc(10, move_otherpoint, 0);
		}
		break;
	case '2':	// 공전하면서 상대방 자리로 이동
		if (!timer_center) {
			timer_center = true;
			glutTimerFunc(1, rotate_center, 0);
		}
		break;
	case '3':	// 위아래로 이동후 상대방 자리로
		if (!ud_other) {
			ud_other = true;
			movex_1 = pos1_x / 50;
			movey_1 = pos1_y / 50;
			movez_1 = pos1_z / 50;
			movex_2 = pos2_x / 50;
			movey_2 = pos2_y / 50;
			movez_2 = pos2_z / 50;
			glutTimerFunc(10, move_otherupdown, 0);
		}
		break;
	case 'c':
		if (cube_draw)
			cube_draw = false;
		else
			cube_draw = true;
		if (sphere_draw)
			sphere_draw = false;
		else
			sphere_draw = true;
		break;
	case 'r':
		if (!see_spiral) {
			see_spiral = true;
			glutTimerFunc(10, move_spiral, 0);
		}

		break;
	case 's':
		if (!scale_xyz) {
			scale_xyz = true;
			glutTimerFunc(50, scale_shape, 0);
		}
		break;
	case 'S':
		if (!scale_point) {
			scale_point = true;
			glutTimerFunc(50, scale_point_shape, 0);
		}
		break;
	case '8':
		pos1 = true;
		pos2 = false;
		break;
	case '9':
		pos1 = false;
		pos2 = true;
		break;
	case '0':
		pos1 = true;
		pos2 = true;
		break;
	case 'o':	//x축이동
		if (pos1)
			pos1_x -= 0.05f;
		if (pos2)
			pos2_x -= 0.05f;
		break;
	case 'p':
		if (pos1)
			pos1_x += 0.05f;
		if (pos2)
			pos2_x += 0.05f;
		break;
	case 'k':	//y축이동
		if (pos1)
			pos1_y += 0.05f;
		if (pos2)
			pos2_y += 0.05f;
		break;
	case 'l':
		if (pos1)
			pos1_y -= 0.05f;
		if (pos2)
			pos2_y -= 0.05f;
		break;
	case 'n':	//z축이동
		if (pos1)
			pos1_z += 0.05f;
		if (pos2)
			pos2_z += 0.05f;
		
		break;
	case 'm':
		if (pos1)
			pos1_z -= 0.05f;
		if (pos2)
			pos2_z -= 0.05f;
		break;
	case 'Q':
	case 'q':	//reset
		cube_draw = true;
		sphere_draw = true;
		timer_center = false;
		scale_xyz = false;
		scale_point = false;
		pos1 = false;
		pos2 = false;
		rollback = false;
		other_point = false;
		see_spiral = false;
		x_1 = -35.0f;
		y_1 = -10.0f;
		x_2 = -35.0f;
		y_2 = -10.0f;
		center = 0;
		scale_1 = 1.0f;
		scale_2 = 1.0f;
		pos1_x = -0.5f;
		pos1_y = 0.3f;
		pos1_z = 0.0f;
		pos2_x = 0.5f;
		pos2_y = 0.3f;
		pos2_z = 0.5f;
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
	glutPostRedisplay();
}
int count_center;
void rotate_center(int value) {
	center += add_center;
	++count_center;
	if (timer_center)
		if (count_center < 180)
			glutTimerFunc(1, rotate_center, value);
		else
		{
			timer_center = false;
			count_center = 0;
		}
	glutPostRedisplay();
}
int scale_count;
void scale_shape(int value) {
	scale_1 += add_scale_1;
	if (scale_1 > 2.0f)
		add_scale_1 *= -1;
	else if(scale_1 <0.0f)
		add_scale_1 *= -1;
	if (scale_xyz)
		glutTimerFunc(10, scale_shape, value);
	glutPostRedisplay();
}
void scale_point_shape(int value) {
	scale_2 += add_scale_2;
	if (scale_2 > 1.0f)
		add_scale_2 *= -1;
	else if (scale_2 < 0.0f)
		add_scale_2 *= -1;
	if (scale_point)
		glutTimerFunc(10, scale_point_shape, value);
	glutPostRedisplay();
}

int rollback_count;
void move_rollback(int value) {
	if (rollback_count < 50) {
		pos1_x -= movex_1;
		pos1_y -= movey_1;
		pos1_z -= movez_1;
		pos2_x -= movex_2;
		pos2_y -= movey_2;
		pos2_z -= movez_2;
	}
	else {
		pos1_x += movex_1;
		pos1_y += movey_1;
		pos1_z += movez_1;
		pos2_x += movex_2;
		pos2_y += movey_2;
		pos2_z += movez_2;
	}
	++rollback_count;

	if (rollback)
		if(rollback_count<100)
			glutTimerFunc(10, move_rollback, value);
		else {
			rollback = false;
			rollback_count = 0;
		}
	glutPostRedisplay();
}
int other_count;
void move_otherpoint(int value) {
	if (other_count < 50) {
		pos1_x -= movex_1;
		pos1_y -= movey_1;
		pos1_z -= movez_1;
		pos2_x -= movex_2;
		pos2_y -= movey_2;
		pos2_z -= movez_2;
	}
	else {
		pos1_x += movex_2;
		pos1_y += movey_2;
		pos1_z += movez_2;
		pos2_x += movex_1;
		pos2_y += movey_1;
		pos2_z += movez_1;
	}
	++other_count;

	if (other_point)
		if (other_count < 100)
			glutTimerFunc(10, move_otherpoint, value);
		else {
			other_point = false;
			other_count = 0;
		}
	glutPostRedisplay();
}
int ud_count;
void move_otherupdown(int value) {
	if (ud_count < 50) {
		pos1_x -= movex_1;
		pos1_y -= movey_1;
		pos1_y += 0.02f;
		pos1_z -= movez_1;
		pos2_x -= movex_2;
		pos2_y -= movey_2;
		pos2_y -= 0.005f;
		pos2_z -= movez_2;
	}
	else {
		pos1_x += movex_2;
		pos1_y += movey_2;
		pos1_y -= 0.02f;
		pos1_z += movez_2;
		pos2_x += movex_1;
		pos2_y += movey_1;
		pos2_y += 0.005f;
		pos2_z += movez_1;
	}
	++ud_count;

	if (ud_other)
		if (ud_count < 100)
			glutTimerFunc(10, move_otherupdown, value);
		else {
			ud_other = false;
			ud_count = 0;
		}
	glutPostRedisplay();

}
void setting_spiral()
{
	spiral[0] = 0.0f;	//x
	spiral[2] = 0.0f;	//z
	float angle;
	float radius;
	angle = 3.6f;
	radius = 0.00015f;
	for (int i = 0; i < 3000-3; i+=3) {
		spiral[i + 3] = spiral[i] + cos(angle * PI / 180) * radius;
		spiral[i + 5] = spiral[i + 2] + sin(angle * PI / 180) * radius;
		angle += 3.6f;
		radius += 0.00007f;
	}
}
int spiralcount;
void move_spiral(int value) {

	pos1_x = spiral[spiralcount];
	pos1_y = spiral[spiralcount+1];
	pos1_z = spiral[spiralcount+2];
	pos2_x = spiral[spiralcount+450];
	pos2_y = spiral[spiralcount+450+1];
	pos2_z = spiral[spiralcount+450+2];
	spiralcount+=3;
	if (see_spiral) {
		if (spiralcount < 2000) {
			glutTimerFunc(25, move_spiral, value);
		}
		else {
			see_spiral = false;
			spiralcount = 0;
			pos1_x = -0.5f;
			pos1_y = 0.3f;
			pos1_z = 0.0f;
			pos2_x = 0.5f;
			pos2_y = 0.3f;
			pos2_z = 0.5f;
		}
	}
	glutPostRedisplay();
}
