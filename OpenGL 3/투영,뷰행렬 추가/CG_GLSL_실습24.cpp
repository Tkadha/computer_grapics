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

void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char* file);
void InitBuffer();
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid UpKeyboard(unsigned char key, int x, int y);
void drawbox();
void swingarm(int);
void swingleg(int);
void robot_move(int);
void settingcolor();
void rotating_camera(int);
void robot_jump(int);
void robot_down(int);
void obstacle_down(int);
void setting_floor_pos();
void robot_draw();
void small_robot_draw(int);
void small_robot_jump(int value);
void small_robot_down(int value);

GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID;
GLuint vao, cubevbo, colorvbo[8];

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
GLfloat body_RGB[6][12];
GLfloat obstacle_RGB[6][12];
GLfloat cube_RGB[6][12];
GLfloat leftarm_RGB[6][12];
GLfloat rightarm_RGB[6][12];
GLfloat leftleg_RGB[6][12];
GLfloat rightleg_RGB[6][12];
GLfloat nose_RGB[6][12];
float obstacle_pos[3][2];
float obstacle_box_y[3];
float floor_pos[9][9][2];

float n_shape_obstacle[3][2];


float swing_arm_angle_add;
float swing_arm_angle;

float swing_leg_angle_add;
float swing_leg_angle;
int speed;
bool forward;
bool back;
bool left;
bool right;

float robot_x;
float robot_y;
float robot_z;
bool key_down;
float camera_x;
float camera_y;
float camera_z;
float camera_angle;
float camera_angle_add;
bool camera_rotate_on;
bool jump;
bool small_jump;
float small_robot_y;
int small_robot_count;

GLuint vbo_line[2];
GLfloat line[3][6];
GLfloat line_RGB[3][6];


GLUquadricObj* qobj;


std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> color(0.1, 1);
std::uniform_int_distribution<> pos(3, 7);
std::uniform_int_distribution<> n_pos_x(2, 4);
std::uniform_int_distribution<> n_pos_z(0, 2);
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
	setting_floor_pos();
	settingcolor();
	{	//설명
		std::cout << "w/a/s/d: 로봇 이동" << std::endl;
		std::cout << "+/-: 로봇 이동속도 증가/감소" << std::endl;
		std::cout << "j: 로봇 점프" << std::endl;
		std::cout << "i: 리셋" << std::endl;
		std::cout << "z/Z: 카메라 앞/뒤 이동" << std::endl;
		std::cout << "x/X: 카메라 좌/우 이동" << std::endl;
		std::cout << "y/Y: 카메라 화면 중심 기준 공전" << std::endl;
		std::cout << "q/Q: 프로그램 종료" << std::endl;
	}
	{
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
	}
	{
		swing_arm_angle_add = 1.0f;
		swing_leg_angle_add = 1.0f;
		speed = 3;
		back = true;
		camera_x = 0.f;
		camera_y = 0.4f;
		camera_z = 1.f;
		small_robot_y = 0.f;
	}

	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_FILL);
	InitBuffer();
	glutTimerFunc(10, swingleg, 0);
	glutTimerFunc(10, swingarm, 0);
	//--- 세이더 읽어와서 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(UpKeyboard);
	glutMainLoop();
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	GLfloat rColor, gColor, bColor;
	rColor = gColor = bColor = 0.4f;
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
	glm::vec3 cameraPos = glm::vec3(camera_x, camera_y, camera_z); //--- 카메라 위치
	glm::vec3 cameraDirection = glm::vec3(camera_x, camera_y - 0.4f, camera_z - 1.f); //--- 카메라 바라보는 방향
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	view = glm::rotate(view, glm::radians(camera_angle), glm::vec3(0.f, 1.f, 0.f));
	glUniformMatrix4fv(view_location, 1, GL_FALSE, &view[0][0]);


	glm::mat4 trans = glm::mat4(1.0f);
	unsigned int shape_location = glGetUniformLocation(shaderProgramID, "transform");
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(trans));
	for (int i = 0; i < 3; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_line[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 6 * sizeof(GLfloat)));
		glBindBuffer(GL_ARRAY_BUFFER, vbo_line[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 6 * sizeof(GLfloat)));
		glLineWidth(1);
		glDrawArrays(GL_LINES, 0, 2);
	}



	glBindBuffer(GL_ARRAY_BUFFER, cubevbo);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[0]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glFrontFace(GL_CW);
	glm::mat4 boxs = trans;
	for (int i = 0; i < 9; ++i) {
		for (int j = 0; j < 9; ++j) {
			glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)((i + j) % 2 * 12 * sizeof(GLfloat)));
			boxs = trans;
			boxs = glm::translate(boxs, glm::vec3(floor_pos[i][j][0], -side_length * 5, floor_pos[i][j][1]));
			glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(boxs));
			glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
		}
	}

	robot_draw();

	for (int i = 0; i < small_robot_count; ++i) {
		small_robot_draw(i);
	}

	glFrontFace(GL_CCW);
	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[1]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);	//장애물
	glm::mat4 obstacle = trans;

	for (int i = 0; i < 3; ++i) {
		obstacle = trans;
		obstacle = glm::translate(obstacle, glm::vec3(obstacle_pos[i][0], -side_length * 5 + obstacle_box_y[i], obstacle_pos[i][1]));
		glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(obstacle));
		drawbox();
	}

	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[0]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	for (int i = 0; i < 2; ++i) {
		obstacle = trans;
		obstacle = glm::translate(obstacle, glm::vec3(n_shape_obstacle[i][0], -side_length * 5, n_shape_obstacle[i][1]));
		obstacle = glm::scale(obstacle, glm::vec3(1.f, 5.f, 1.f));
		glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(obstacle));
		drawbox();
	}
	obstacle = trans;
	obstacle = glm::translate(obstacle, glm::vec3(n_shape_obstacle[2][0]+ side_length*3, 0.f, n_shape_obstacle[2][1]));
	obstacle = glm::scale(obstacle, glm::vec3(4.f, 1.f, 1.f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(obstacle));
	drawbox();



	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 2; ++j) {
			obstacle = trans;
			obstacle = glm::translate(obstacle, glm::vec3(-side_length * 7.5f + side_length * i * 15.f, -side_length * 5, -side_length * 6 + side_length * j * 15.f));
			obstacle = glm::rotate(obstacle, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
			glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(obstacle));
			gluCylinder(qobj, side_length, side_length, side_length * 7, 20, 8);
		}
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
	glGenBuffers(1, &cubevbo);
	glGenBuffers(8, colorvbo);
	glGenBuffers(2, vbo_line);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, cubevbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_DYNAMIC_DRAW);



	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_RGB), cube_RGB, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(obstacle_RGB), obstacle_RGB, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(body_RGB), body_RGB, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(leftarm_RGB), leftarm_RGB, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rightarm_RGB), rightarm_RGB, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[5]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(leftleg_RGB), leftleg_RGB, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[6]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rightleg_RGB), rightleg_RGB, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[7]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(nose_RGB), nose_RGB, GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_RGB), line_RGB, GL_STATIC_DRAW);
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case '+':
		if (speed < 3) {
			++speed;
		}
		break;
	case '-':
		if (speed > 1) {
			--speed;
		}
		break;
	case 'w':
		if (!key_down) {
			back = false;
			left = false;
			right = false;
			forward = true;
			key_down = true;
			glutTimerFunc(10, robot_move, 0);
		}
		break;

	case 's':
		if (!key_down) {
			forward = false;
			left = false;
			right = false;
			back = true;
			key_down = true;
			glutTimerFunc(10, robot_move, 0);
		}
		break;

	case 'a':
		if (!key_down) {
			forward = false;
			back = false;
			right = false;
			left = true;
			key_down = true;
			glutTimerFunc(10, robot_move, 0);
		}
		break;

	case 'd':
		if (!key_down) {
			forward = false;
			back = false;
			left = false;
			right = true;
			key_down = true;
			glutTimerFunc(10, robot_move, 0);
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

	case 'y':
		camera_angle_add = 1.f;
		if (camera_rotate_on)
			camera_rotate_on = false;
		else {
			camera_rotate_on = true;
			glutTimerFunc(10, rotating_camera, 0);
		}
		break;
	case 'Y':
		camera_angle_add = -1.f;
		if (camera_rotate_on)
			camera_rotate_on = false;
		else {
			camera_rotate_on = true;
			glutTimerFunc(10, rotating_camera, 0);
		}
		break;
	case 'i':
		swing_arm_angle_add = 1.0f;
		swing_leg_angle_add = 1.0f;
		speed = 3;
		forward = false;
		left = false;
		right = false;
		back = true;
		robot_z = 0.f;
		robot_x = 0.f;
		robot_y = 0.f;
		camera_x = 0.f;
		camera_y = 0.4f;
		camera_z = 1.f;
		camera_rotate_on = false;
		jump = false;
		break;
	case 'j':
		if (!jump) {
			jump = true;
			glutTimerFunc(10, robot_jump, 0);
		}
		break;
	case 't':
		++small_robot_count;
		if (small_robot_count > 3)
			small_robot_count = 0;
		break;
	case 'f':
		if (!small_jump) {
			small_jump = true;
			glutTimerFunc(10, small_robot_jump, 0);
		}
		break;
	case 'q':
	case 'Q':
		exit(0);
		break;
	}
	glutPostRedisplay();
}
GLvoid UpKeyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
	case 's':
	case 'a':
	case 'd':
		key_down = false;
		break;
	}
	glutPostRedisplay();
}
void drawbox() {
	for (int i = 0; i < 6; ++i) {
		glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
	}
}
void swingarm(int value) {
	swing_arm_angle += 2 * swing_arm_angle_add;
	if ((swing_arm_angle > 30 * speed) || (swing_arm_angle < -30 * speed)) {
		swing_arm_angle_add *= -1;
	}
		glutTimerFunc(10, swingarm, value);
	glutPostRedisplay();
}
void swingleg(int value) {
	swing_leg_angle += 2 * swing_leg_angle_add;
	if ((swing_leg_angle > 30 * speed) || (swing_leg_angle < -30 * speed)) {
		swing_leg_angle_add *= -1;
	}
		glutTimerFunc(10, swingleg, value);
	glutPostRedisplay();
}
void robot_move(int value) {
	if (forward) {
		robot_z -= 0.01f * speed;
		if ((robot_x < -1.2f) || (robot_x > 1.2f)) {
			if (robot_z < -0.9f)
				robot_z += 0.01f * speed;
		}
		if (robot_y <= 0.01f) {
			for (int i = 0; i < 3; ++i) {
				if (obstacle_box_y[i] > -side_length) {
					if ((obstacle_pos[i][0] - side_length <= robot_x) && (robot_x <= obstacle_pos[i][0] + side_length)) {
						if ((robot_z > obstacle_pos[i][1] - side_length) && (robot_z < obstacle_pos[i][1] + side_length))
							robot_z += 0.01f * speed;
					}
				}
			}
		}
		for (int i = 0; i < 2; ++i) {
			if ((n_shape_obstacle[i][0] - side_length <= robot_x) && (robot_x <= n_shape_obstacle[i][0] + side_length)) {
				if ((robot_z > n_shape_obstacle[i][1] - side_length) && (robot_z < n_shape_obstacle[i][1] + side_length))
					robot_z += 0.01f * speed;
			}
		}
		if (robot_y >= 0.1f) {
			if ((n_shape_obstacle[0][0] - side_length <= robot_x) && (robot_x <= n_shape_obstacle[1][0] + side_length)) {
				if ((robot_z > n_shape_obstacle[0][1] - side_length) && (robot_z < n_shape_obstacle[0][1] + side_length))
					robot_z += 0.01f * speed;
			}
		}
		if (robot_z < -1.5f) {
			forward = false;
			back = true;
		}
	}
	else if (back) {
		robot_z += 0.01f * speed;
		if ((robot_x < -1.2f) || (robot_x > 1.2f)) {
			if (robot_z > 1.3f)
				robot_z -= 0.01f * speed;
		}
		if (robot_y <= 0.01f) {
			for (int i = 0; i < 3; ++i) {
				if (obstacle_box_y[i] > -side_length) {
					if ((obstacle_pos[i][0] - side_length <= robot_x) && (robot_x <= obstacle_pos[i][0] + side_length)) {
						if ((robot_z > obstacle_pos[i][1] - side_length) && (robot_z < obstacle_pos[i][1] + side_length))
							robot_z -= 0.01f * speed;
					}
				}
			}
		}
		for (int i = 0; i < 2; ++i) {
			if ((n_shape_obstacle[i][0] - side_length <= robot_x) && (robot_x <= n_shape_obstacle[i][0] + side_length)) {
				if ((robot_z > n_shape_obstacle[i][1] - side_length) && (robot_z < n_shape_obstacle[i][1] + side_length))
					robot_z -= 0.01f * speed;
			}
		}
		if (robot_y >= 0.1f) {
			if ((n_shape_obstacle[0][0] - side_length <= robot_x) && (robot_x <= n_shape_obstacle[1][0] + side_length)) {
				if ((robot_z > n_shape_obstacle[0][1] - side_length) && (robot_z < n_shape_obstacle[0][1] + side_length))
					robot_z -= 0.01f * speed;
			}
		}

		if (robot_z > 1.7f) {
			forward = true;
			back = false;
		}
	}
	else if (left) {
		robot_x -= 0.01f * speed;
		if ((robot_z < -1.2f) || (robot_z > 1.5f)) {
			if (robot_x < -1.2f)
				robot_x += 0.01f * speed;
		}
		if (robot_y <= 0.01f) {
			for (int i = 0; i < 3; ++i) {
				if (obstacle_box_y[i] > -side_length) {
					if ((obstacle_pos[i][1] - side_length <= robot_z) && (robot_z <= obstacle_pos[i][1] + side_length)) {
						if ((robot_x > obstacle_pos[i][0] - side_length) && (robot_x < obstacle_pos[i][0] + side_length))
							robot_x += 0.01f * speed;
					}
				}
			}
		}
		for (int i = 0; i < 2; ++i) {
			if ((n_shape_obstacle[i][1] - side_length <= robot_z) && (robot_z <= n_shape_obstacle[i][1] + side_length)) {
				if ((robot_x > n_shape_obstacle[i][0] - side_length) && (robot_x < n_shape_obstacle[i][0] + side_length))
					robot_x += 0.01f * speed;
			}
		}
		if (robot_y >= 0.1f) {
			if ((n_shape_obstacle[0][1] - side_length <= robot_z) && (robot_z <= n_shape_obstacle[1][1] + side_length)) {
				if ((robot_x > n_shape_obstacle[0][0] - side_length) && (robot_x < n_shape_obstacle[0][0] + side_length))
					robot_x += 0.01f * speed;
			}
		}

		if (robot_x < -1.5f) {
			left = false;
			right = true;
		}
	}
	else if (right) {
		robot_x += 0.01f * speed;
		if ((robot_z < -1.2f) || (robot_z > 1.5f)) {
			if (robot_x > 1.2f)
				robot_x -= 0.01f * speed;
		}
		if (robot_y <= 0.01f) {
			for (int i = 0; i < 3; ++i) {
				if (obstacle_box_y[i] > - side_length) {
					if ((obstacle_pos[i][1] - side_length <= robot_z) && (robot_z <= obstacle_pos[i][1] + side_length)) {
						if ((robot_x > obstacle_pos[i][0] - side_length) && (robot_x < obstacle_pos[i][0] + side_length))
							robot_x -= 0.01f * speed;
					}
				}
			}
		}
		for (int i = 0; i < 2; ++i) {
			if ((n_shape_obstacle[i][1] - side_length <= robot_z) && (robot_z <= n_shape_obstacle[i][1] + side_length)) {
				if ((robot_x > n_shape_obstacle[i][0] - side_length) && (robot_x < n_shape_obstacle[i][0] + side_length))
					robot_x -= 0.01f * speed;
			}
		}
		if (robot_y >= 0.1f) {
			if ((n_shape_obstacle[0][1] - side_length <= robot_z) && (robot_z <= n_shape_obstacle[1][1] + side_length)) {
				if ((robot_x > n_shape_obstacle[0][0] - side_length) && (robot_x < n_shape_obstacle[0][0] + side_length))
					robot_x -= 0.01f * speed;
			}
		}

		if (robot_x > 1.5f) {
			left = true;
			right = false;
		}
	}
	if (key_down)
		glutTimerFunc(10, robot_move, value);
	glutPostRedisplay();
}
void settingcolor() {
	for (int i = 0; i < 6; ++i)
	{
		cube_RGB[i][0] = color(gen);
		cube_RGB[i][1] = color(gen);
		cube_RGB[i][2] = color(gen);


		for (int k = 1; k < 4; ++k) {
			cube_RGB[i][k * 3] = cube_RGB[i][0];
			cube_RGB[i][k * 3 + 1] = cube_RGB[i][1];
			cube_RGB[i][k * 3 + 2] = cube_RGB[i][2];

			body_RGB[i][k * 3] = body_RGB[i][0];
			body_RGB[i][k * 3 + 1] = body_RGB[i][1];
			body_RGB[i][k * 3 + 2] = body_RGB[i][2];
		}
		for (int k = 0; k < 12; ++k) {
			obstacle_RGB[i][k] = 0.6f;
			nose_RGB[i][k] = 0.f;
		}
	}
	for (int i = 0; i < 3; ++i) {
		int x_floor = pos(gen);
		int z_floor = pos(gen);
		obstacle_pos[i][0] = floor_pos[z_floor][x_floor][0];
		obstacle_pos[i][1] = floor_pos[z_floor][x_floor][1];
	}
	int x_floor = n_pos_x(gen);
	int z_floor = n_pos_z(gen);
	n_shape_obstacle[2][0] = n_shape_obstacle[0][0] = floor_pos[z_floor][x_floor][0];
	n_shape_obstacle[2][1] = n_shape_obstacle[0][1] = floor_pos[z_floor][x_floor][1];
	n_shape_obstacle[1][0] = floor_pos[z_floor][x_floor+3][0];
	n_shape_obstacle[1][1] = floor_pos[z_floor][x_floor+3][1];


	body_RGB[0][0] = color(gen);
	body_RGB[0][1] = color(gen);
	body_RGB[0][2] = color(gen);
	for (int i = 0; i < 6; ++i)
	{
		for (int k = 1; k < 4; ++k) {
			body_RGB[i][k * 3] = body_RGB[0][0];
			body_RGB[i][k * 3 + 1] = body_RGB[0][1];
			body_RGB[i][k * 3 + 2] = body_RGB[0][2];
		}
	}
	leftarm_RGB[0][0] = color(gen);
	leftarm_RGB[0][1] = color(gen);
	leftarm_RGB[0][2] = color(gen);
	leftleg_RGB[0][0] = color(gen);
	leftleg_RGB[0][1] = color(gen);
	leftleg_RGB[0][2] = color(gen);
	for (int i = 0; i < 6; ++i)
	{
		for (int k = 1; k < 4; ++k) {
			leftarm_RGB[i][k * 3] = leftarm_RGB[0][0];
			leftarm_RGB[i][k * 3 + 1] = leftarm_RGB[0][1];
			leftarm_RGB[i][k * 3 + 2] = leftarm_RGB[0][2];
			leftleg_RGB[i][k * 3] = leftleg_RGB[0][0];
			leftleg_RGB[i][k * 3 + 1] = leftleg_RGB[0][1];
			leftleg_RGB[i][k * 3 + 2] = leftleg_RGB[0][2];
		}
	}
	rightarm_RGB[0][0] = 1 - leftarm_RGB[0][0];
	rightarm_RGB[0][1] = 1 - leftarm_RGB[0][1];
	rightarm_RGB[0][2] = 1 - leftarm_RGB[0][2];
	rightleg_RGB[0][0] = 1 - leftleg_RGB[0][0];
	rightleg_RGB[0][1] = 1 - leftleg_RGB[0][1];
	rightleg_RGB[0][2] = 1 - leftleg_RGB[0][2];
	for (int i = 0; i < 6; ++i)
	{
		for (int k = 1; k < 4; ++k) {
			rightarm_RGB[i][k * 3] = rightarm_RGB[0][0];
			rightarm_RGB[i][k * 3 + 1] = rightarm_RGB[0][1];
			rightarm_RGB[i][k * 3 + 2] = rightarm_RGB[0][2];
			rightleg_RGB[i][k * 3] = rightleg_RGB[0][0];
			rightleg_RGB[i][k * 3 + 1] = rightleg_RGB[0][1];
			rightleg_RGB[i][k * 3 + 2] = rightleg_RGB[0][2];
		}
	}
}
void rotating_camera(int value) {
	camera_angle += camera_angle_add;
	if (camera_rotate_on) {
		glutTimerFunc(10, rotating_camera, value);
	}
	glutPostRedisplay();
}
void robot_jump(int value) {
	if (value < 30)
	{
		robot_y += 0.03f;
		if ((n_shape_obstacle[0][0] - side_length <= robot_x) && (robot_x <= n_shape_obstacle[1][0] + side_length)) {
			if ((robot_z > n_shape_obstacle[0][1] - side_length) && (robot_z < n_shape_obstacle[0][1] + side_length))
			{
				glutTimerFunc(10, robot_down, 0);
			}
			else {
				glutTimerFunc(10, robot_jump, value + 1);
			}
		}
		else {
			glutTimerFunc(10, robot_jump, value + 1);
		}
	}
	else if (jump) {
		glutTimerFunc(10, robot_down, 0);
	}
	glutPostRedisplay();
}
void robot_down(int value) {
	robot_y -= 0.03f;
	if (robot_y <= 0) {
		jump = false;
	}
	else {
		for (int i = 0; i < 3; ++i) {
			if ((obstacle_pos[i][0] - side_length <= robot_x) && (robot_x <= obstacle_pos[i][0] + side_length)) {
				if ((obstacle_pos[i][1] - side_length <= robot_z) && (robot_z <= obstacle_pos[i][1] + side_length)) {
					if (-0.6f + robot_y <= -0.3f + obstacle_box_y[i]) {
						glutTimerFunc(10, obstacle_down, i);
						break;
					}
				}
			}
		}
	}
	if (jump) glutTimerFunc(10, robot_down, value);

	glutPostRedisplay();
}
void obstacle_down(int value) {
	obstacle_box_y[value] -= 0.01f;
	if(obstacle_box_y[value] > -side_length-0.1f) glutTimerFunc(10, obstacle_down, value);
	glutPostRedisplay();
}

void setting_floor_pos() {
	float x = -side_length * 8;
	float z = -side_length * 8;
	for (int i = 0; i < 9; ++i) {
		for (int j = 0; j < 9; ++j) {
			floor_pos[i][j][0] = x;
			floor_pos[i][j][1] = z;
			x += side_length * 2;
		}
		x = -side_length * 8;
		z += side_length * 2;
	}
}

void robot_draw() {
	glm::mat4 trans = glm::mat4(1.0f);
	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation);
	unsigned int shape_location = glGetUniformLocation(shaderProgramID, "transform");

	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[2]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);	// 몸 & 머리
	glm::mat4 body = trans;
	body = glm::translate(body, glm::vec3(robot_x, -0.6f + robot_y, robot_z));
	if (forward) {
		body = glm::rotate(body, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
	}
	else if (left) {
		body = glm::rotate(body, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	else if (right) {
		body = glm::rotate(body, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	body = glm::scale(body, glm::vec3(0.4f, 1.f, 0.4f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(body));
	drawbox();

	glm::mat4 head = trans;
	head = glm::translate(head, glm::vec3(robot_x, -0.6f + robot_y, robot_z));
	head = glm::translate(head, glm::vec3(0.f, 0.25f, 0.f));
	if (forward) {
		head = glm::rotate(head, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
	}
	else if (left) {
		head = glm::rotate(head, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	else if (right) {
		head = glm::rotate(head, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	head = glm::scale(head, glm::vec3(0.25f, 0.25f, 0.25f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(head));
	drawbox();

	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[7]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);	// 코
	glm::mat4 nose = head;
	nose = glm::translate(nose, glm::vec3(0.f, -0.15f, 0.25f));
	if (forward) {
		nose = glm::rotate(nose, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
	}
	else if (left) {
		nose = glm::rotate(nose, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	else if (right) {
		nose = glm::rotate(nose, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	nose = glm::scale(nose, glm::vec3(0.25f, 0.5f, 0.75f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(nose));
	drawbox();


	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[3]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);	// 왼팔
	glm::mat4 arm = trans;
	arm = glm::translate(arm, glm::vec3(robot_x, -0.6f + robot_y, robot_z));
	if (back) {
		arm = glm::translate(arm, glm::vec3(0.1f, 0.f, 0.f));
	}
	else if (left) {
		arm = glm::translate(arm, glm::vec3(0.f, 0.f, 0.1f));
	}
	else if (right) {
		arm = glm::translate(arm, glm::vec3(0.f, 0.f, -0.1f));
	}
	else {
		arm = glm::translate(arm, glm::vec3(-0.1f, 0.f, 0.f));
	}
	if (left) {
		arm = glm::rotate(arm, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	else if (right) {
		arm = glm::rotate(arm, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	arm = glm::translate(arm, glm::vec3(0.f, 0.1f, 0.f));
	arm = glm::rotate(arm, glm::radians(-swing_arm_angle), glm::vec3(1.f, 0.f, 0.f));
	arm = glm::translate(arm, glm::vec3(0.f, -0.1f, 0.f));
	arm = glm::scale(arm, glm::vec3(0.1f, 0.75f, 0.1f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(arm));
	drawbox();

	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[4]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);	// 오른팔
	arm = trans;
	arm = glm::translate(arm, glm::vec3(robot_x, -0.6f + robot_y, robot_z));
	if (back) {
		arm = glm::translate(arm, glm::vec3(-0.1f, 0.f, 0.f));
	}
	else if (left) {
		arm = glm::translate(arm, glm::vec3(0.f, 0.f, -0.1f));
	}
	else if (right) {
		arm = glm::translate(arm, glm::vec3(0.f, 0.f, 0.1f));
	}
	else {
		arm = glm::translate(arm, glm::vec3(0.1f, 0.f, 0.f));
	}
	if (left) {
		arm = glm::rotate(arm, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	else if (right) {
		arm = glm::rotate(arm, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	arm = glm::translate(arm, glm::vec3(0.f, 0.1f, 0.f));
	arm = glm::rotate(arm, glm::radians(swing_arm_angle), glm::vec3(1.f, 0.f, 0.f));
	arm = glm::translate(arm, glm::vec3(0.f, -0.1f, 0.f));
	arm = glm::scale(arm, glm::vec3(0.1f, 0.75f, 0.1f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(arm));
	drawbox();

	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[5]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);	// 왼다리

	glm::mat4 leg = trans;
	leg = glm::translate(leg, glm::vec3(robot_x, -0.6f + robot_y, robot_z));
	if (back) {
		leg = glm::translate(leg, glm::vec3(0.025f, -0.3f, 0.f));
	}
	else if (left) {
		leg = glm::translate(leg, glm::vec3(0.f, -0.3f, -0.025f));
	}
	else if (right) {
		leg = glm::translate(leg, glm::vec3(0.f, -0.3f, 0.025f));
	}
	else {
		leg = glm::translate(leg, glm::vec3(-0.025f, -0.3f, 0.f));
	}
	if (left) {
		leg = glm::rotate(leg, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	else if (right) {
		leg = glm::rotate(leg, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	leg = glm::translate(leg, glm::vec3(0.f, 0.1f, 0.f));
	leg = glm::rotate(leg, glm::radians(swing_leg_angle), glm::vec3(1.f, 0.f, 0.f));
	leg = glm::translate(leg, glm::vec3(0.f, -0.1f, 0.f));
	leg = glm::scale(leg, glm::vec3(0.1f, 0.75f, 0.1f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(leg));
	drawbox();

	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[6]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);	// 왼다리
	leg = trans;
	leg = glm::translate(leg, glm::vec3(robot_x, -0.6f + robot_y, robot_z));
	if (back) {
		leg = glm::translate(leg, glm::vec3(-0.025f, -0.3f, 0.f));
	}
	else if (left) {
		leg = glm::translate(leg, glm::vec3(0.f, -0.3f, 0.025f));
	}
	else if (right) {
		leg = glm::translate(leg, glm::vec3(0.f, -0.3f, -0.025f));
	}
	else {
		leg = glm::translate(leg, glm::vec3(0.025f, -0.3f, 0.f));
	}
	if (left) {
		leg = glm::rotate(leg, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	else if (right) {
		leg = glm::rotate(leg, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	leg = glm::translate(leg, glm::vec3(0.f, 0.1f, 0.f));
	leg = glm::rotate(leg, glm::radians(-swing_leg_angle), glm::vec3(1.f, 0.f, 0.f));
	leg = glm::translate(leg, glm::vec3(0.f, -0.1f, 0.f));
	leg = glm::scale(leg, glm::vec3(0.1f, 0.75f, 0.1f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(leg));
	drawbox();

}

void small_robot_draw(int count) {
	glm::mat4 trans = glm::mat4(1.0f);
	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation);
	unsigned int shape_location = glGetUniformLocation(shaderProgramID, "transform");

	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[2]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);	// 몸 & 머리
	glm::mat4 body = trans;
	if (forward) body = glm::translate(body, glm::vec3(0.f, 0.f, (count + 1) * 0.2f));
	else if (back) body = glm::translate(body, glm::vec3(0.f, 0.f, (count + 1) * -0.2f));
	else if (left) body = glm::translate(body, glm::vec3((count + 1) * 0.2f, 0.f, 0.f));
	else if (right) body = glm::translate(body, glm::vec3((count + 1) * -0.2f, 0.f, 0.f));

	body = glm::translate(body, glm::vec3(robot_x, -0.6f + robot_y-side_length+ small_robot_y, robot_z));
	if (forward) {
		body = glm::rotate(body, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
	}
	else if (left) {
		body = glm::rotate(body, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	else if (right) {
		body = glm::rotate(body, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
		body = glm::scale(body, glm::vec3(0.4f, 1.f, 0.4f));
	}
	body = glm::scale(body, glm::vec3(0.5f, 0.5f, 0.5f));
	body = glm::scale(body, glm::vec3(0.4f, 1.f, 0.4f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(body));
	drawbox();

	glm::mat4 head = trans;
	if (forward) head = glm::translate(head, glm::vec3(0.f, 0.f, (count + 1) * 0.2f));
	else if (back) head = glm::translate(head, glm::vec3(0.f, 0.f, (count + 1) * -0.2f));
	else if (left) head = glm::translate(head, glm::vec3((count + 1) * 0.2f, 0.f, 0.f));
	else if (right) head = glm::translate(head, glm::vec3((count + 1) * -0.2f, 0.f, 0.f));

	head = glm::translate(head, glm::vec3(robot_x, -0.6f + robot_y - side_length*1.5f+ small_robot_y, robot_z));
	head = glm::translate(head, glm::vec3(0.f, 0.25f, 0.f));
	if (forward) {
		head = glm::rotate(head, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
	}
	else if (left) {
		head = glm::rotate(head, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	else if (right) {
		head = glm::rotate(head, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	head = glm::scale(head, glm::vec3(0.5f, 0.5f, 0.5f));
	head = glm::scale(head, glm::vec3(0.25f, 0.25f, 0.25f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(head));
	drawbox();

	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[7]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);	// 코
	glm::mat4 nose = head;

	if (forward) nose = glm::translate(nose, glm::vec3(0.f, 0.f, (count + 1) * 0.2f));
	else if (back) nose = glm::translate(nose, glm::vec3(0.f, 0.f, (count + 1) * -0.2f));
	else if (left) nose = glm::translate(nose, glm::vec3((count + 1) * 0.2f, 0.f, 0.f));
	else if (right) nose = glm::translate(nose, glm::vec3((count + 1) * -0.2f, 0.f, 0.f));

	nose = glm::translate(nose, glm::vec3(0.f, -0.15f, 0.25f));
	if (forward) {
		nose = glm::rotate(nose, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
	}
	else if (left) {
		nose = glm::rotate(nose, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	else if (right) {
		nose = glm::rotate(nose, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	nose = glm::scale(nose, glm::vec3(0.5f, 0.5f, 0.5f));
	nose = glm::scale(nose, glm::vec3(0.25f, 0.5f, 0.75f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(nose));
	drawbox();


	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[3]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);	// 왼팔
	glm::mat4 arm = trans;

	if (forward) arm = glm::translate(arm, glm::vec3(0.f, 0.f, (count + 1) * 0.2f));
	else if (back) arm = glm::translate(arm, glm::vec3(0.f, 0.f, (count + 1) * -0.2f));
	else if (left) arm = glm::translate(arm, glm::vec3((count + 1) * 0.2f, 0.f, 0.f));
	else if (right) arm = glm::translate(arm, glm::vec3((count + 1) * -0.2f, 0.f, 0.f));

	arm = glm::translate(arm, glm::vec3(robot_x, -0.6f + robot_y - side_length + small_robot_y, robot_z));
	if (back) {
		arm = glm::translate(arm, glm::vec3(0.1f, 0.f, 0.f));
	}
	else if (left) {
		arm = glm::translate(arm, glm::vec3(0.f, 0.f, 0.1f));
	}
	else if (right) {
		arm = glm::translate(arm, glm::vec3(0.f, 0.f, -0.1f));
	}
	else {
		arm = glm::translate(arm, glm::vec3(-0.1f, 0.f, 0.f));
	}
	if (left) {
		arm = glm::rotate(arm, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	else if (right) {
		arm = glm::rotate(arm, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	arm = glm::translate(arm, glm::vec3(0.f, 0.1f, 0.f));
	arm = glm::rotate(arm, glm::radians(-swing_arm_angle), glm::vec3(1.f, 0.f, 0.f));
	arm = glm::translate(arm, glm::vec3(0.f, -0.1f, 0.f));
	arm = glm::scale(arm, glm::vec3(0.5f, 0.5f, 0.5f));
	arm = glm::scale(arm, glm::vec3(0.1f, 0.75f, 0.1f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(arm));
	drawbox();

	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[4]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);	// 오른팔
	arm = trans;
	if (forward) arm = glm::translate(arm, glm::vec3(0.f, 0.f, (count + 1) * 0.2f));
	else if (back) arm = glm::translate(arm, glm::vec3(0.f, 0.f, (count + 1) * -0.2f));
	else if (left) arm = glm::translate(arm, glm::vec3((count + 1) * 0.2f, 0.f, 0.f));
	else if (right) arm = glm::translate(arm, glm::vec3((count + 1) * -0.2f, 0.f, 0.f));
	arm = glm::translate(arm, glm::vec3(robot_x, -0.6f + robot_y - side_length + small_robot_y, robot_z));
	if (back) {
		arm = glm::translate(arm, glm::vec3(-0.1f, 0.f, 0.f));
	}
	else if (left) {
		arm = glm::translate(arm, glm::vec3(0.f, 0.f, -0.1f));
	}
	else if (right) {
		arm = glm::translate(arm, glm::vec3(0.f, 0.f, 0.1f));
	}
	else {
		arm = glm::translate(arm, glm::vec3(0.1f, 0.f, 0.f));
	}
	if (left) {
		arm = glm::rotate(arm, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	else if (right) {
		arm = glm::rotate(arm, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	arm = glm::translate(arm, glm::vec3(0.f, 0.1f, 0.f));
	arm = glm::rotate(arm, glm::radians(swing_arm_angle), glm::vec3(1.f, 0.f, 0.f));
	arm = glm::translate(arm, glm::vec3(0.f, -0.1f, 0.f));
	arm = glm::scale(arm, glm::vec3(0.5f, 0.5f, 0.5f));
	arm = glm::scale(arm, glm::vec3(0.1f, 0.75f, 0.1f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(arm));
	drawbox();

	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[5]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);	// 왼다리

	glm::mat4 leg = trans;
	if (forward) leg = glm::translate(leg, glm::vec3(0.f, 0.f, (count + 1) * 0.2f));
	else if (back) leg = glm::translate(leg, glm::vec3(0.f, 0.f, (count + 1) * -0.2f));
	else if (left) leg = glm::translate(leg, glm::vec3((count + 1) * 0.2f, 0.f, 0.f));
	else if (right) leg = glm::translate(leg, glm::vec3((count + 1) * -0.2f, 0.f, 0.f));
	leg = glm::translate(leg, glm::vec3(robot_x, -0.6f + robot_y - side_length * 0.5f + small_robot_y, robot_z));
	if (back) {
		leg = glm::translate(leg, glm::vec3(0.025f, -0.3f, 0.f));
	}
	else if (left) {
		leg = glm::translate(leg, glm::vec3(0.f, -0.3f, -0.025f));
	}
	else if (right) {
		leg = glm::translate(leg, glm::vec3(0.f, -0.3f, 0.025f));
	}
	else {
		leg = glm::translate(leg, glm::vec3(-0.025f, -0.3f, 0.f));
	}
	if (left) {
		leg = glm::rotate(leg, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	else if (right) {
		leg = glm::rotate(leg, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	leg = glm::translate(leg, glm::vec3(0.f, 0.1f, 0.f));
	leg = glm::rotate(leg, glm::radians(swing_leg_angle), glm::vec3(1.f, 0.f, 0.f));
	leg = glm::translate(leg, glm::vec3(0.f, -0.1f, 0.f));
	leg = glm::scale(leg, glm::vec3(0.5f, 0.5f, 0.5f));
	leg = glm::scale(leg, glm::vec3(0.1f, 0.75f, 0.1f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(leg));
	drawbox();

	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[6]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);	// 왼다리
	leg = trans;
	if (forward) leg = glm::translate(leg, glm::vec3(0.f, 0.f, (count + 1) * 0.2f));
	else if (back) leg = glm::translate(leg, glm::vec3(0.f, 0.f, (count + 1) * -0.2f));
	else if (left) leg = glm::translate(leg, glm::vec3((count + 1) * 0.2f, 0.f, 0.f));
	else if (right) leg = glm::translate(leg, glm::vec3((count + 1) * -0.2f, 0.f, 0.f));
	leg = glm::translate(leg, glm::vec3(robot_x, -0.6f + robot_y - side_length * 0.5f + small_robot_y, robot_z));
	if (back) {
		leg = glm::translate(leg, glm::vec3(-0.025f, -0.3f, 0.f));
	}
	else if (left) {
		leg = glm::translate(leg, glm::vec3(0.f, -0.3f, 0.025f));
	}
	else if (right) {
		leg = glm::translate(leg, glm::vec3(0.f, -0.3f, -0.025f));
	}
	else {
		leg = glm::translate(leg, glm::vec3(0.025f, -0.3f, 0.f));
	}
	if (left) {
		leg = glm::rotate(leg, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	else if (right) {
		leg = glm::rotate(leg, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
	}
	leg = glm::translate(leg, glm::vec3(0.f, 0.1f, 0.f));
	leg = glm::rotate(leg, glm::radians(-swing_leg_angle), glm::vec3(1.f, 0.f, 0.f));
	leg = glm::translate(leg, glm::vec3(0.f, -0.1f, 0.f));
	leg = glm::scale(leg, glm::vec3(0.5f, 0.5f, 0.5f));
	leg = glm::scale(leg, glm::vec3(0.1f, 0.75f, 0.1f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(leg));
	drawbox();
}

void small_robot_jump(int value) {
	if (value < 30)
	{
		small_robot_y += 0.01f;	
		glutTimerFunc(10, small_robot_jump, value + 1);
	}
	else if (small_jump) {
		glutTimerFunc(10, small_robot_down, 0);
	}
	glutPostRedisplay();
}
void small_robot_down(int value) {
	small_robot_y -= 0.01f;
	if (small_robot_y <= 0) {
		small_jump = false;
	}
	
	if (small_jump) glutTimerFunc(10, small_robot_down, value);

	glutPostRedisplay();
}