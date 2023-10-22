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
#include <math.h>

#define Width 1200
#define Height 800
#define side_length 0.2
#define board_size 3
#define PI 3.14159265

void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char* file);
void InitBuffer();
GLvoid Keyboard(unsigned char key, int x, int y);
void moving_x(int);
void rotating_y(int);
void rotating_gun(int);
void moving_gun(int);
void rotating_arm(int);
void rotating_camera(int);
void rotating_all(int);

GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID;
GLuint vao, cubevbo[4];

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
GLfloat cube_body_RGB[6][12];
GLfloat cube_barrel_RGB[6][12];
GLfloat cube_arm_RGB[6][12];

GLuint vbo_line[2];
GLfloat line[3][6];
GLfloat line_RGB[3][6];

GLuint vbo_ground[2];
GLfloat ground[12];
GLfloat ground_RGB[12];

float camera_z;
float camera_x;
float crain_move_x;
float rotate_y;
float rotate_gun_y;
float move_gun;
float rotate_arm;
int adding_arm;
float angle;
float all_angle;

bool crain_move_x_on;
bool rotate_y_on;
bool rotate_gun_y_on;
bool move_gun_on;
bool rotate_arm_on;
bool rotate_camera_on;
bool rotate_all_on;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> under(0.0, 0.7);
std::uniform_real_distribution<> color(0.1, 1);

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
	for (int i = 0; i < 6; ++i) {
		for (int k = 0; k < 12; ++k)
		{
			cube_body_RGB[i][k] = color(gen);

			cube_barrel_RGB[i][k] = color(gen);

			cube_arm_RGB[i][k] = color(gen);
		}
	}
	line[0][1] = 1;
	line[0][4] = -1;
	line_RGB[0][1] = 1;
	line_RGB[0][4] = 1;//y

	line[1][0] = 1;
	line[1][3] = -1;
	line_RGB[1][0] = 1;
	line_RGB[1][3] = 1;//x

	line[2][2] = 1;
	line[2][5] = -1;
	line_RGB[2][2] = 1;
	line_RGB[2][5] = 1;//z

	{
		ground[0] = board_size;
		ground[2] = board_size;
		ground[3] = board_size;
		ground[5] = -board_size;
		ground[6] = -board_size;
		ground[8] = -board_size;
		ground[9] = -board_size;
		ground[11] = board_size;
		for (int i = 0; i < 12; ++i) {
			ground_RGB[i] = under(gen);
		}
	}

	{	//설명
		std::cout << "b/B: x축 방향으로 (양/음) 방향 이동 / 정지" << std::endl;
		std::cout << "m/M: 중앙 몸체 Y축 (양/음) 방향으로 회전 / 정지" << std::endl;
		std::cout << "f/F: 포신 y축 회전 / 정지" << std::endl;
		std::cout << "e/E: 포신 (합치기 / 분리하기)" << std::endl;
		std::cout << "t/T: 팔 회전 / 멈춤" << std::endl;
		std::cout << "z/Z: 카메라 z축 방향 이동" << std::endl;
		std::cout << "x/X: 카메라 x축 방향 이동" << std::endl;
		std::cout << "y/Y: 카메라 기준 y축 회전" << std::endl;
		std::cout << "r/R: 중점을 기준으로 y축 회전" << std::endl;
		std::cout << "a/A: r명령어 회전 애니메이션 작동 / 정지" << std::endl;
		std::cout << "s/S: 애니메이션 정지" << std::endl;
		std::cout << "c/C: 애니메이션 초기화" << std::endl;
		std::cout << "q/Q: 프로그램 종료" << std::endl;
	}

	{
		camera_x = 0.f;
		camera_z = 0.0f;
		crain_move_x = 0.f;
		rotate_y = 0.f;
		rotate_gun_y = 0.f;
		move_gun = 0.f;
		rotate_arm = 0.f;
		adding_arm = 1;
		angle = 0.f;
		all_angle = 0.f;


		crain_move_x_on = false;
		rotate_y_on = false;
		rotate_gun_y_on = false;
		move_gun_on = false;
		rotate_arm_on = false;
		rotate_camera_on = false;
		rotate_all_on = false;
	}


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
	rColor = gColor = bColor = 0.0f;
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	glEnable(GL_DEPTH_TEST);
	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation);


	unsigned int proj_location = glGetUniformLocation(shaderProgramID, "projection");
	unsigned int view_location = glGetUniformLocation(shaderProgramID, "view");

	//투영행렬
	glm::mat4 proj = glm::mat4(1.0f);
		proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
		proj = glm::translate(proj, glm::vec3(0.f, 0.f, -5.0f));
	glUniformMatrix4fv(proj_location, 1, GL_FALSE, &proj[0][0]);
	
	//뷰 행렬
	glm::vec3 cameraPos = glm::vec3(0.f, 0.5f, -4.0f); //--- 카메라 위치
	glm::vec3 cameraDirection = glm::vec3(0.f, 0.f, -5.f); //--- 카메라 바라보는 방향
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
	glm::mat4 view = glm::mat4(1.0f);

	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	glUniformMatrix4fv(view_location, 1, GL_FALSE, &view[0][0]);


	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, glm::radians(all_angle), glm::vec3(0.0f, 1.0f, 0.0f));
	trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, -5.0f));
	trans = glm::rotate(trans, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	unsigned int shape_location = glGetUniformLocation(shaderProgramID, "transform");
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(trans));
	glm::mat4 base = trans;
	base = glm::rotate(base, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	base = glm::translate(base, glm::vec3(camera_x, 0.0f, camera_z));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(base));
	for (int i = 0; i < 3; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_line[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 6 * sizeof(GLfloat)));
		glBindBuffer(GL_ARRAY_BUFFER, vbo_line[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 6 * sizeof(GLfloat)));
		glLineWidth(1);
		glDrawArrays(GL_LINES, 0, 2);
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo_ground[0]);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_ground[1]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[0]);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[1]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glm::mat4 under_body = trans;
	under_body = glm::rotate(under_body, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	under_body = glm::translate(under_body, glm::vec3(camera_x, 0.0f, camera_z));
	under_body = glm::translate(under_body, glm::vec3(crain_move_x, 0.f, 0.f));
	under_body = glm::translate(under_body, glm::vec3(0.f, 0.1f, 0.f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(under_body));
	for (int i = 0; i < 6; ++i) {
	glDrawArrays(GL_TRIANGLE_FAN, i*4, 4);	//아래 몸통
	}
	glm::mat4 main_body = trans;
	main_body = glm::rotate(main_body, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	main_body = glm::translate(main_body, glm::vec3(camera_x, 0.0f, camera_z));
	main_body = glm::translate(main_body, glm::vec3(crain_move_x, 0.f, 0.f));
	main_body = glm::rotate(main_body, glm::radians(rotate_y), glm::vec3(0.0f, 1.0f, 0.0f));
	main_body = glm::translate(main_body, glm::vec3(0.f, 0.25f, 0.f));
	main_body = glm::translate(main_body, glm::vec3(0.f, 0.1f, 0.f));
	main_body = glm::scale(main_body, glm::vec3(0.6f, 0.7f, 0.6f));
	main_body = glm::rotate(main_body, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(main_body));
	for (int i = 0; i < 6; ++i) {
		glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);	//윗 몸통
	}

	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[2]);	// gun
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glm::mat4 left_gun = trans;
	left_gun = glm::rotate(left_gun, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	left_gun = glm::translate(left_gun, glm::vec3(camera_x, 0.0f, camera_z));
	left_gun = glm::translate(left_gun, glm::vec3(crain_move_x, 0.f, 0.f));
	left_gun = glm::rotate(left_gun, glm::radians(-rotate_gun_y), glm::vec3(0.0f, 1.0f, 0.0f));
	left_gun = glm::translate(left_gun, glm::vec3(-move_gun, 0.f, 0.f));
	left_gun = glm::translate(left_gun, glm::vec3(0.15f, 0.f, 0.3f));
	left_gun = glm::scale(left_gun, glm::vec3(0.15f, 0.5f, 0.7f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(left_gun));
	for (int i = 0; i < 6; ++i) {
		glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);	// 왼쪽 포신
	}
	glm::mat4 right_gun = trans;
	right_gun = glm::rotate(right_gun, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	right_gun = glm::translate(right_gun, glm::vec3(camera_x, 0.0f, camera_z));
	right_gun = glm::translate(right_gun, glm::vec3(crain_move_x, 0.f, 0.f));
	right_gun = glm::rotate(right_gun, glm::radians(rotate_gun_y), glm::vec3(0.0f, 1.0f, 0.0f));
	right_gun = glm::translate(right_gun, glm::vec3(move_gun, 0.f, 0.f));
	right_gun = glm::translate(right_gun, glm::vec3(-0.15f, 0.f, 0.3f));
	right_gun = glm::scale(right_gun, glm::vec3(0.15f, 0.5f, 0.7f));
	right_gun = glm::rotate(right_gun, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(right_gun));
	for (int i = 0; i < 6; ++i) {
		glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);	// 오른쪽 포신
	}

	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[3]);	// arm
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glm::mat4 left_arm = trans;
	left_arm = glm::rotate(left_arm, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	left_arm = glm::translate(left_arm, glm::vec3(camera_x, 0.0f, camera_z));
	left_arm = glm::translate(left_arm, glm::vec3(crain_move_x, 0.f, 0.f));
	left_arm = glm::rotate(left_arm, glm::radians(rotate_y), glm::vec3(0.0f, 1.0f, 0.0f));
	left_arm = glm::translate(left_arm, glm::vec3(0.05f, 0.3f, 0.0f));
	left_arm = glm::rotate(left_arm, glm::radians(-rotate_arm), glm::vec3(1.0f, 0.0f, 0.0f));
	left_arm = glm::translate(left_arm, glm::vec3(0.0f, 0.3f, 0.0f));
	left_arm = glm::scale(left_arm, glm::vec3(0.15f, 0.7f, 0.15f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(left_arm));
	for (int i = 0; i < 6; ++i) {
		glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);	// 왼쪽 팔
	}
	glm::mat4 right_arm = trans;
	right_arm = glm::rotate(right_arm, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	right_arm = glm::translate(right_arm, glm::vec3(camera_x, 0.0f, camera_z));
	right_arm = glm::translate(right_arm, glm::vec3(crain_move_x, 0.f, 0.f));
	right_arm = glm::rotate(right_arm, glm::radians(rotate_y), glm::vec3(0.0f, 1.0f, 0.0f));
	right_arm = glm::translate(right_arm, glm::vec3(-0.05f, 0.3f, 0.0f));
	right_arm = glm::rotate(right_arm, glm::radians(rotate_arm), glm::vec3(1.0f, 0.0f, 0.0f));
	right_arm = glm::translate(right_arm, glm::vec3(0.0f, 0.3f, 0.0f));
	right_arm = glm::scale(right_arm, glm::vec3(0.15f, 0.7f, 0.15f));
	right_arm = glm::rotate(right_arm, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(right_arm));
	for (int i = 0; i < 6; ++i) {
		glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);	// 오른쪽 팔
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
	glGenBuffers(4, cubevbo);
	glGenBuffers(2, vbo_line);
	glGenBuffers(2, vbo_ground);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_body_RGB), cube_body_RGB, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_barrel_RGB), cube_barrel_RGB, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_arm_RGB), cube_arm_RGB, GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_RGB), line_RGB, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_ground[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ground), ground, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_ground[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ground_RGB), ground_RGB, GL_STATIC_DRAW);

}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'b':		
		if (crain_move_x_on)
			crain_move_x_on = false;
		else {
			crain_move_x_on = true;
			glutTimerFunc(10, moving_x, 1);
		}
		break;
	case 'B':		
		if (crain_move_x_on)
			crain_move_x_on = false;
		else {
			crain_move_x_on = true;
			glutTimerFunc(10, moving_x, -1);
		}
		break;

	case 'm':		
		if (rotate_y_on)
			rotate_y_on = false;
		else {
			rotate_y_on = true;
			glutTimerFunc(10, rotating_y, 1);
		}
		break;
	case 'M':	
		if (rotate_y_on)
			rotate_y_on = false;
		else {
			rotate_y_on = true;
			glutTimerFunc(10, rotating_y, -1);
		}
		break;

	case 'f':	
		if (rotate_gun_y_on)
			rotate_gun_y_on = false;
		else {
			rotate_gun_y_on = true;
			glutTimerFunc(10, rotating_gun, 1);
		}
		break;
	case 'F':	
		if (rotate_gun_y_on)
			rotate_gun_y_on = false;
		else {
			rotate_gun_y_on = true;
			glutTimerFunc(10, rotating_gun, -1);
		}
		break;

	case 'e':	
		if (move_gun_on) {
			move_gun_on = false;
		}
		else {
			move_gun_on = true;
			glutTimerFunc(10, moving_gun, 1);
		}
		break;
	case 'E':	
		if (move_gun_on) {
			move_gun_on = false;
		}
		else {
			move_gun_on = true;
			glutTimerFunc(10, moving_gun, -1);
		}
		break;

	case 't':		
		if (rotate_arm_on)
			rotate_arm_on = false;
		else {
			rotate_arm_on = true;
			glutTimerFunc(10, rotating_arm, 1);
		}
		break;
	case 'T':	
		if (rotate_arm_on)
			rotate_arm_on = false;
		else {
			rotate_arm_on = true;
			glutTimerFunc(10, rotating_arm, 1);
		}
		break;

	case 'z':	
		camera_z += 0.1f;
		break;
	case 'Z':		
		camera_z -= 0.1f;
		break;

	case 'x':	
		camera_x += 0.1f;
		break;
	case 'X':	
		camera_x -= 0.1f;
		break;

	case 'a':		
		if (rotate_camera_on)
		{
			rotate_camera_on = false;
		}
		else {
			rotate_camera_on = true;
			glutTimerFunc(10, rotating_camera, 1);
		}
		break;
	case 'A':	
		if (rotate_camera_on) {
			rotate_camera_on = false;
		}
		else {
			rotate_camera_on = true;
			glutTimerFunc(10, rotating_camera, -1);
		}
		break;

	case 'r':	
		angle += 3.6f;
		break;
	case 'R':		
		angle -= 3.6f;
		break;

	case 'y':		
		if (rotate_all_on)
			rotate_all_on = false;
		else {
			rotate_all_on = true;
			glutTimerFunc(10, rotating_all, 1);
		}
		break;
	case 'Y':	
		if (rotate_all_on)
			rotate_all_on = false;
		else {
			rotate_all_on = true;
			glutTimerFunc(10, rotating_all, -1);
		}
		break;

	case 's':		
	case 'S':	
		crain_move_x_on = false;
		rotate_y_on = false;
		rotate_gun_y_on = false;
		move_gun_on = false;
		rotate_arm_on = false;
		rotate_camera_on = false;
		rotate_all_on = false;
		break;

	case 'c':		
	case 'C':	
		camera_x = 0.f;
		camera_z = 0.0f;
		crain_move_x = 0.f;
		rotate_y = 0.f;
		rotate_gun_y = 0.f;
		move_gun = 0.f;
		rotate_arm = 0.f;
		adding_arm = 1;
		angle = 0.f;
		all_angle = 0.f;


		crain_move_x_on = false;
		rotate_y_on = false;
		rotate_gun_y_on = false;
		move_gun_on = false;
		rotate_arm_on = false;
		rotate_camera_on = false;
		rotate_all_on = false;
		break;

	case 'q':		
	case 'Q':	
		exit(0);
		break;
	}
	glutPostRedisplay();
}

void moving_x(int value) {
	crain_move_x += 0.01*value;
	if (crain_move_x_on) {
		glutTimerFunc(10, moving_x, value);
	}
	glutPostRedisplay();
}

void rotating_y(int value) {
	rotate_y += 3*value;
	if (rotate_y_on) {
		glutTimerFunc(10, rotating_y, value);
	}
	glutPostRedisplay();
}
void rotating_gun(int value) {
	rotate_gun_y += 3 * value;
	if (rotate_gun_y_on) {
		glutTimerFunc(10, rotating_gun, value);
	}
	glutPostRedisplay();
}

void moving_gun(int value) {
	if (fmodf(rotate_gun_y, 360) != 0) {
		rotate_gun_y += 3 * value;
	}
	else {
		if (value > 0){
			if (move_gun < 0.15) {
				move_gun += value * 0.01;
			}
		}
		else {
			if (move_gun > 0) {
				move_gun += value * 0.01;
			}
		}	
		if ((move_gun >= 0.15) || (move_gun <= 0)) {
			move_gun_on = false;
		}
	}
	if (move_gun_on) {
		glutTimerFunc(25, moving_gun, value);
	}
	glutPostRedisplay();
}

void rotating_arm(int value) {
	rotate_arm += adding_arm;
	if (rotate_arm >= 45)
		adding_arm *= -1;
	else if (rotate_arm <= -45)
		adding_arm *= -1;
	if (rotate_arm_on)
		glutTimerFunc(10, rotating_arm, value);
	glutPostRedisplay();
}

void rotating_camera(int value) {
	angle += 3.6f*value;
	if (rotate_camera_on) {
		glutTimerFunc(10, rotating_camera, value);
	}
	glutPostRedisplay();
}
void rotating_all(int value) {
	all_angle += 3.6f * value;
	if (rotate_all_on) {
		glutTimerFunc(10, rotating_all, value);
	}
	glutPostRedisplay();
}
