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
void Mouse(int, int, int, int);
void Motion(int, int);
void drawbox();
void settingcolor();
void moving_box(int);
void settingsphere();
void moving_ball(int);
void rotating_y(int);
void open_floor(int);
GLchar* vertexSource, * fragmentSource; //--- �ҽ��ڵ� ���� ����
GLuint vertexShader, fragmentShader; //--- ���̴� ��ü
GLuint shaderProgramID;
GLuint vao, cubevbo, colorvbo[2];

GLfloat cube[6][12] = {
	{ -side_length, -side_length, side_length,
	  -side_length,  side_length, side_length,	//��
	   side_length,  side_length, side_length,
	   side_length, -side_length, side_length},

	{ -side_length, -side_length,  -side_length,
	  -side_length,  side_length,  -side_length,	//������
	  -side_length,  side_length, side_length,
	  -side_length, -side_length, side_length },

	{  side_length, -side_length, side_length,
	   side_length,  side_length, side_length,	//����
	   side_length,  side_length,  -side_length,
	   side_length, -side_length,  -side_length },

	{  side_length, -side_length,  -side_length,
	   side_length,  side_length,  -side_length,	//��
	  -side_length,  side_length,  -side_length,
	  -side_length, -side_length,  -side_length },

	{ -side_length,  side_length,  side_length,
	  -side_length,  side_length,  -side_length,	//��
	   side_length,  side_length,  -side_length,
	   side_length,  side_length,  side_length},

	{ -side_length, -side_length,  -side_length,
	  -side_length, -side_length, side_length,	//�Ʒ�
	   side_length, -side_length, side_length,
	   side_length, -side_length,  -side_length },
};
GLfloat cube_RGB[6][12];
GLfloat box_RGB[6][12];
float box_pos_x[3];
float box_pos_y[3];
float box_drop[3];
bool key_down;
float camera_x;
float camera_y;
float camera_z;
float camera_angle;
float camera_angle_add;
bool camera_rotate_on;
float rotate_z;
float mouse_pos[2];
GLuint vbo_line[2];
GLfloat line[3][6];
GLfloat line_RGB[3][6];

GLUquadricObj* qobj[5];
float balls_pos[5][3];
float balls_add[5][3];
int balls;

float open_floor_z;


std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> color(0.1, 1);
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
	settingsphere();
	settingcolor();
	{	//����
		std::cout << "w/a/s/d: �κ� �̵�" << std::endl;
		std::cout << "+/-: �κ� �̵��ӵ� ����/����" << std::endl;
		std::cout << "j: �κ� ����" << std::endl;
		std::cout << "i: ����" << std::endl;
		std::cout << "o/O: �ո� ����/�ݱ�" << std::endl;
		std::cout << "z/Z: ī�޶� ��/�� �̵�" << std::endl;
		std::cout << "x/X: ī�޶� ��/�� �̵�" << std::endl;
		std::cout << "y/Y: ī�޶� ȭ�� �߽� ���� ����" << std::endl;
		std::cout << "q/Q: ���α׷� ����" << std::endl;
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
		camera_x = 0.f;
		camera_y = 0.25f;
		camera_z = 1.f;
	}
	{
		balls_add[0][0] = 0.05f;
		balls_add[0][1] = 0.05f;
		balls_add[0][2] = -0.05f;

		balls_add[1][0] = 0.05f;
		balls_add[1][1] = 0.0f;
		balls_add[1][2] = 0.05f;

		balls_add[2][0] = 0.0f;
		balls_add[2][1] = 0.05f;
		balls_add[2][2] = -0.05f;

		balls_add[3][0] = -0.05f;
		balls_add[3][1] = 0.05f;
		balls_add[3][2] = 0.0f;

		balls_add[4][0] = 0.f;
		balls_add[4][1] = 0.05f;
		balls_add[4][2] = 0.f;
	}
	InitBuffer();
	glutTimerFunc(10, moving_box, 0);
	//--- ���̴� �о�ͼ� ���̴� ���α׷� �����
	glutDisplayFunc(drawScene); //--- ��� �ݹ� �Լ�
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutMainLoop();
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	GLfloat rColor, gColor, bColor;
	rColor = gColor = bColor = 0.0f;
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation);


	unsigned int proj_location = glGetUniformLocation(shaderProgramID, "projection");
	unsigned int view_location = glGetUniformLocation(shaderProgramID, "view");

	//�������
	glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
	proj = glm::translate(proj, glm::vec3(0.f, 0.f, -5.0f));
	glUniformMatrix4fv(proj_location, 1, GL_FALSE, &proj[0][0]);

	//�� ���
	glm::vec3 cameraPos = glm::vec3(camera_x, camera_y, camera_z); //--- ī�޶� ��ġ
	glm::vec3 cameraDirection = glm::vec3(camera_x, camera_y - 0.25f, camera_z - 1.f); //--- ī�޶� �ٶ󺸴� ����
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����
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
	glFrontFace(GL_CCW);
	glm::mat4 cubes = trans;
	cubes = glm::rotate(cubes, glm::radians(rotate_z), glm::vec3(0.f, 0.f, 1.f));
	cubes = glm::scale(cubes, glm::vec3(6.f, 6.f, 6.f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(cubes));
	for (int i = 1; i < 5; ++i) {
		glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
	}
	cubes = glm::translate(cubes, glm::vec3(0.f, 0.f, open_floor_z));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(cubes));
	if (open_floor_z > -6 * side_length) {
		glDrawArrays(GL_TRIANGLE_FAN, 5 * 4, 4);
	}
	cubes = trans;
	cubes = glm::rotate(cubes, glm::radians(rotate_z), glm::vec3(0.f, 0.f, 1.f));
	cubes = glm::scale(cubes, glm::vec3(6.f, 6.f, 6.f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(cubes));
	glFrontFace(GL_CW);
	glDrawArrays(GL_TRIANGLE_FAN, 4 * 4, 4);
	glm::mat4 boxs = trans;
	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[1]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	for (int i = 0; i < 3; ++i) {
		boxs = trans;
		boxs = glm::rotate(boxs, glm::radians(rotate_z), glm::vec3(0.f, 0.f, 1.f));
		boxs = glm::translate(boxs, glm::vec3(box_pos_x[i], box_drop[i]+ box_pos_y[i], 0.f));
		boxs = glm::translate(boxs, glm::vec3(0.f, -5 * side_length - (i + 1.f) * 0.04f, 0.5f * i));
		boxs = glm::scale(boxs, glm::vec3(0.8f - 0.2f * i, 0.8f - 0.2f * i, 0.8f - 0.2f * i));
		glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(boxs));
		drawbox();
	}
	glm::mat4 ball = trans;
	for (int i = 0; i < balls; ++i) {
		ball = glm::translate(ball, glm::vec3(balls_pos[i][0], balls_pos[i][1], balls_pos[i][2]));
		ball = glm::rotate(ball, glm::radians(rotate_z), glm::vec3(0.f, 0.f, 1.f));
		glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(ball));
		gluSphere(qobj[i], 0.2, 50, 50);
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
	glGenBuffers(1, &cubevbo);
	glGenBuffers(2, colorvbo);
	glGenBuffers(2, vbo_line);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, cubevbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_DYNAMIC_DRAW);



	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_RGB), cube_RGB, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, colorvbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(box_RGB), box_RGB, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_RGB), line_RGB, GL_STATIC_DRAW);
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'z':
		camera_z -= 0.5f;
		break;
	case 'Z':
		camera_z += 0.5f;
		break;
	case 'y':
		camera_angle_add = 1.0f;
		if (camera_rotate_on)
			camera_rotate_on = false;
		else {
			camera_rotate_on = true;
			glutTimerFunc(10, rotating_y, 0);
		}
		break;
	case 'Y':
		camera_angle_add = -1.0f;
		if (camera_rotate_on)
			camera_rotate_on = false;
		else {
			camera_rotate_on = true;
			glutTimerFunc(10, rotating_y, 0);
		}
		break;
	case 'b':
	case 'B':
		if (balls < 5) {
			glutTimerFunc(10, moving_ball, balls);
			++balls;
		}
		break;
	case 'o':
		glutTimerFunc(10, open_floor, 0);
		break;
	case 'q':
	case 'Q':
		exit(0);
		break;
	}
	glutPostRedisplay();
}
void drawbox() {
	for (int i = 0; i < 6; ++i) {
		glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
	}
}
void settingcolor() {
	for (int i = 0; i < 6; ++i)
	{
		cube_RGB[i][9] = cube_RGB[i][6] = cube_RGB[i][3] = cube_RGB[i][0] = color(gen);
		cube_RGB[i][10] = cube_RGB[i][7] = cube_RGB[i][4] = cube_RGB[i][1] = color(gen);
		cube_RGB[i][11] = cube_RGB[i][8] = cube_RGB[i][5] = cube_RGB[i][2] = color(gen);
		box_RGB[i][9] = box_RGB[i][6] = box_RGB[i][3] = box_RGB[i][0] = color(gen);
		box_RGB[i][10] = box_RGB[i][7] = box_RGB[i][4] = box_RGB[i][1] = color(gen);
		box_RGB[i][11] = box_RGB[i][8] = box_RGB[i][5] = box_RGB[i][2] = color(gen);
	}

}
void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON || button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			key_down = true;
			mouse_pos[0] = x;
			mouse_pos[1] = y;
		}
		else if (state == GLUT_UP) {
			key_down = false;
		}
	}
}
void Motion(int x, int y) {
	if (key_down) {
		if (mouse_pos[0] < x) {
			rotate_z -= 1.0f;
		}
		else if (mouse_pos[0] > x) {
			rotate_z += 1.0f;
		}
		mouse_pos[0] = x;
		mouse_pos[1] = y;
	}
	glutPostRedisplay();

}
void moving_box(int value) {
	if (rotate_z > -60.f && rotate_z < 0.f) {
		for (int i = 0; i < 3; ++i) {
			if(box_pos_x[i] < 5 * side_length + (i + 1.f) * 0.04f)
			box_pos_x[i] += 0.04f;
		}
	}
	else if (rotate_z > 0.f && rotate_z < 60.f) {
		for (int i = 0; i < 3; ++i) {
			if (box_pos_x[i] > -5 * side_length - (i + 1.f) * 0.04f)
				box_pos_x[i] -= 0.04f;
		}
	}

	for (int i = 0; i < 3; ++i) {
		if (open_floor_z < -side_length * i) {
			box_drop[2-i] -= 0.05f;
		}
	}
	glutTimerFunc(10, moving_box, value);
	glutPostRedisplay();

}
void settingsphere() {
	for (int i = 0; i < 5; ++i) {
		qobj[i] = gluNewQuadric();
		gluQuadricDrawStyle(qobj[i], GLU_FILL);
	}
}
void moving_ball(int value) {
	for (int i = 0; i < 3; ++i) {
		balls_pos[value][i] += balls_add[value][i];
		if ((balls_pos[value][i] > 5 * side_length)||(balls_pos[value][i] < -5 * side_length)) {
			balls_add[value][i] *= -1;
		}
	}
	glutTimerFunc(10, moving_ball, value);
	glutPostRedisplay();
}

void rotating_y(int value) {
	camera_angle += camera_angle_add;

	if (camera_rotate_on) {
		glutTimerFunc(10, rotating_y, value);
	}
	glutPostRedisplay();

}
void open_floor(int value) {
	open_floor_z -= 0.01f;
	if (open_floor_z > -side_length * 6) {
		glutTimerFunc(10, open_floor, value);
	}
	glutPostRedisplay();
}