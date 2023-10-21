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
void planet_move(int);
void sub_planet_move(int);

void rotate_y(int);
void rotate_z(int);
GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID;
GLuint vao;
GLuint vbo[2];
GLfloat RGB[7][3];


bool perspec_on;
bool solid_line;
float rail[200][3];
float subrail[200][3];
int planet_point[3];
int sub_planet_point[3];


float movex;
float movey;
float movez;
float rotatey;
float rotatez;
bool rotatey_on;
bool rotatez_on;
GLUquadricObj* planet;

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
	//std::cout << sizeof(rail) << std::endl;
	float x;
	float z;
	float angle;
	for (int i = 0; i < 200; ++i) {
		angle = i * 1.8f;
		x = cos(angle * PI / 180) * 1.0f;
		z = sin(angle * PI / 180) * 1.0f;
		rail[i][0] = x;
		rail[i][1] = 0.0f;
		rail[i][2] = z;
		x = cos(angle * PI / 180) * 0.4f;
		z = sin(angle * PI / 180) * 0.4f;
		subrail[i][0] = x;
		subrail[i][1] = 0.0f;
		subrail[i][2] = z;
	}
	for (int i = 0; i < 7; ++i) {
		RGB[i][0] = color(gen);
		RGB[i][1] = color(gen);
		RGB[i][2] = color(gen);
	}
	InitBuffer();
	glutTimerFunc(30, planet_move, 0);
	glutTimerFunc(20, planet_move, 1);
	glutTimerFunc(10, planet_move, 2);
	glutTimerFunc(30, sub_planet_move, 0);
	glutTimerFunc(20, sub_planet_move, 1);
	glutTimerFunc(10, sub_planet_move, 2);
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
	unsigned int proj_location = glGetUniformLocation(shaderProgramID, "projection");
	unsigned int view_location = glGetUniformLocation(shaderProgramID, "view");
	unsigned int shape_location = glGetUniformLocation(shaderProgramID, "transform");
	unsigned int color_location = glGetUniformLocation(shaderProgramID, "setcolor");

	//투영행렬
	glm::mat4 proj = glm::mat4(1.0f);
	if (perspec_on) {
		proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
		proj = glm::translate(proj, glm::vec3(0.f, 0.f, -5.0f));
	}
	else
	{
	proj = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
	}
	glUniformMatrix4fv(proj_location, 1, GL_FALSE, &proj[0][0]);

	//뷰 행렬
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f); //--- 카메라 위치
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
	glm::mat4 view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	glUniformMatrix4fv(view_location, 1, GL_FALSE, &view[0][0]);

	glm::mat4 trans = glm::mat4(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	trans = glm::translate(trans, glm::vec3(movex, movey, movez));
	trans = glm::rotate(trans, glm::radians(rotatez), glm::vec3(0.0f, 0.0f, 1.0f));
	trans = glm::rotate(trans, glm::radians(rotatey), glm::vec3(0.0f, 1.0f, 0.0f));
	trans = glm::rotate(trans, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	trans = glm::rotate(trans, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	gluQuadricDrawStyle(planet, GLU_LINE);
	glUniform3f(color_location, 0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 200; ++i) {
		glm::mat4 rail1 = trans;
		rail1 = glm::translate(rail1, glm::vec3(rail[i][0], rail[i][1], rail[i][2]));
		glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(rail1));
		gluSphere(planet, 0.005, 5, 5);
		glm::mat4 rail2 = trans;
		rail2 = glm::rotate(rail2, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 1.0f));
		rail2 = glm::translate(rail2, glm::vec3(rail[i][0], rail[i][1], rail[i][2]));
		glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(rail2));
		

		gluSphere(planet, 0.005, 5, 5);
		glm::mat4 rail3 = trans;
		rail3 = glm::rotate(rail3, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 1.0f));
		rail3 = glm::translate(rail3, glm::vec3(rail[i][0], rail[i][1], rail[i][2]));
		glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(rail3));
		gluSphere(planet, 0.005, 5, 5);

		glm::mat4 sub_rail1 = trans;
		sub_rail1 = glm::translate(sub_rail1, glm::vec3(subrail[i][0], subrail[i][1], subrail[i][2]));
		sub_rail1 = glm::translate(sub_rail1, glm::vec3(rail[planet_point[0]][0], rail[planet_point[0]][1], rail[planet_point[0]][2]));
		glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(sub_rail1));
		gluSphere(planet, 0.005, 5, 5);

		glm::mat4 sub_rail2 = trans;
		sub_rail2 = glm::translate(sub_rail2, glm::vec3(subrail[i][0], subrail[i][1], subrail[i][2]));
		sub_rail2 = glm::rotate(sub_rail2, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 1.0f));
		sub_rail2 = glm::translate(sub_rail2, glm::vec3(rail[planet_point[1]][0], rail[planet_point[1]][1], rail[planet_point[1]][2]));
		sub_rail2 = glm::rotate(sub_rail2, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(sub_rail2));
		gluSphere(planet, 0.005, 5, 5);

		glm::mat4 sub_rail3 = trans;
		sub_rail3 = glm::translate(sub_rail3, glm::vec3(subrail[i][0], subrail[i][1], subrail[i][2]));
		sub_rail3 = glm::rotate(sub_rail3, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 1.0f));
		sub_rail3 = glm::translate(sub_rail3, glm::vec3(rail[planet_point[2]][0], rail[planet_point[2]][1], rail[planet_point[2]][2]));
		sub_rail3 = glm::rotate(sub_rail3, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(sub_rail3));
		gluSphere(planet, 0.005, 5, 5);
	}


	if (solid_line) {
		gluQuadricDrawStyle(planet, GLU_LINE);
	}
	else
		gluQuadricDrawStyle(planet, GLU_FILL);

	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(trans));
	glUniformMatrix4fv(view_location, 1, GL_FALSE, &view[0][0]);
	glUniform3f(color_location, 0.0f, 1.0f, 0.0f);
	gluSphere(planet, 0.3, 30, 30);
	
	glm::mat4 planet1 = trans;
	planet1 = glm::translate(planet1, glm::vec3(rail[planet_point[0]][0], rail[planet_point[0]][1], rail[planet_point[0]][2]));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(planet1));
	glUniform3f(color_location, 0.0f, 0.0f, 1.0f);
	gluSphere(planet, 0.2, 20, 20);

	glm::mat4 sub_planet1 = planet1;
	sub_planet1 = glm::translate(sub_planet1, glm::vec3(subrail[sub_planet_point[0]][0], subrail[sub_planet_point[0]][1], subrail[sub_planet_point[0]][2]));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(sub_planet1));
	glUniform3f(color_location, 1.0f, 0.0f, 0.0f);
	gluSphere(planet, 0.1, 10, 10);



	glm::mat4 planet2 = trans;
	planet2 = glm::rotate(planet2, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 1.0f));
	planet2 = glm::translate(planet2, glm::vec3(rail[planet_point[1]][0], rail[planet_point[1]][1], rail[planet_point[1]][2]));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(planet2));
	glUniform3f(color_location, 0.3f, 0.3f, 0.3f);
	gluSphere(planet, 0.2, 20, 20);

	glm::mat4 sub_planet2 = trans;
	sub_planet2 = glm::translate(sub_planet2, glm::vec3(subrail[sub_planet_point[1]][0], subrail[sub_planet_point[1]][1], subrail[sub_planet_point[1]][2]));
	sub_planet2 = glm::rotate(sub_planet2, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 1.0f));
	sub_planet2 = glm::translate(sub_planet2, glm::vec3(rail[planet_point[1]][0], rail[planet_point[1]][1], rail[planet_point[1]][2]));
	sub_planet2 = glm::rotate(sub_planet2, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(sub_planet2));
	glUniform3f(color_location, 1.0f, 1.0f, 0.0f);
	gluSphere(planet, 0.1, 10, 10);



	glm::mat4 planet3 = trans;
	planet3 = glm::rotate(planet3, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 1.0f));
	planet3 = glm::translate(planet3, glm::vec3(rail[planet_point[2]][0], rail[planet_point[2]][1], rail[planet_point[2]][2]));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(planet3));
	glUniform3f(color_location, 0.0f, 1.0f, 1.0f);
	gluSphere(planet, 0.2, 20, 20);

	glm::mat4 sub_planet3 = trans;
	sub_planet3 = glm::translate(sub_planet3, glm::vec3(subrail[sub_planet_point[2]][0], subrail[sub_planet_point[2]][1], subrail[sub_planet_point[2]][2]));
	sub_planet3 = glm::rotate(sub_planet3, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 1.0f));
	sub_planet3 = glm::translate(sub_planet3, glm::vec3(rail[planet_point[2]][0], rail[planet_point[2]][1], rail[planet_point[2]][2]));
	sub_planet3 = glm::rotate(sub_planet3, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(sub_planet3));
	glUniform3f(color_location, 5.0f, 0.5f, 0.5f);
	gluSphere(planet, 0.1, 10, 10);


	//glBindBuffer(GL_ARRAY_BUFFER, railvbo);
	//glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	//glDrawArrays(GL_LINE_STRIP, 0, 100);


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
	fragmentSource = filetobuf("fragment_19.glsl");
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
	glGenBuffers(2, vbo);
	glBindVertexArray(vao);

	planet = gluNewQuadric();
	gluQuadricNormals(planet, GLU_SMOOTH);
	gluQuadricOrientation(planet, GLU_OUTSIDE);
	gluQuadricTexture(planet, GL_FALSE);

}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'p':
	case 'P':
		if (perspec_on)
			perspec_on = false;
		else
			perspec_on = true;
		break;
	case 'm':
	case 'M':
		if (solid_line)
			solid_line = false;
		else
			solid_line = true;
		break;

	case 'w':
		movey += 0.1f;
		break;
	case 's':
		movey -= 0.1f;
		break;

	case 'a':
		movex -= 0.1f;
		break;
	case 'd':
		movex += 0.1f;
		break;

	case '+':
		movez += 0.1f;
		break;
	case '-':
		movez -= 0.1f;
		break;

	case 'y':
	case 'Y':
		if (rotatey_on)
			rotatey_on = false;
		else {
			rotatey_on = true;
			glutTimerFunc(10, rotate_y, 0);
		}
		break;

	case 'z':
	case 'Z':
		if (rotatez_on)
			rotatez_on = false;
		else {
			rotatez_on = true;
			glutTimerFunc(10, rotate_z, 0);
		}
		break;
	}
	glutPostRedisplay();
}
void planet_move(int value) {
	++planet_point[value];
	if (planet_point[value] >= 200)
		planet_point[value] = 0;
	if(value==0)
		glutTimerFunc(1, planet_move, value);
	else if (value==1)
		glutTimerFunc(20, planet_move, value);
	else
		glutTimerFunc(10, planet_move, value);

	glutPostRedisplay();
}
void sub_planet_move(int value) {
	++sub_planet_point[value];
	if (sub_planet_point[value] >= 200)
		sub_planet_point[value] = 0;
	if (value == 0)
		glutTimerFunc(10, sub_planet_move, value);
	else if (value == 1)
		glutTimerFunc(1, sub_planet_move, value);
	else
		glutTimerFunc(20, sub_planet_move, value);

	glutPostRedisplay();
}
void rotate_y(int value) {
	++rotatey;
	if (rotatey_on)
		glutTimerFunc(10, rotate_y, 0);
}
void rotate_z(int value) {
	++rotatez;
	if (rotatez_on)
		glutTimerFunc(10, rotate_z, 0);
}