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

void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char* file);
void InitBuffer();
GLvoid Keyboard(unsigned char key, int x, int y);

void rotate_y(int);
void up_rect_rotate_x(int);
void LR_rect_up_y(int);
void open_front(int);
void open_back(int);

void open_cone(int);
void open_cone_one(int);
GLchar* vertexSource, * fragmentSource; //--- �ҽ��ڵ� ���� ����
GLuint vertexShader, fragmentShader; //--- ���̴� ��ü
GLuint shaderProgramID;
GLuint vao, cubevbo[2];

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

GLuint conevbo[2];
GLfloat cone[6][9] = {
	{ -side_length, 0, side_length,	//��
	   side_length, 0, side_length,
	   0, side_length * 2,0},
	{ -side_length, 0,-side_length,	//��
	  -side_length, 0, side_length,
	   0,2 * side_length,0},
	 { side_length, 0, -side_length,// ��
	  -side_length, 0, -side_length,
	   0,2 * side_length,0},
	{ side_length, 0,side_length, // ��
	  side_length, 0, -side_length,
	   0,2 * side_length,0},


	   //�ٴ�
	{ -side_length, 0, -side_length,
	   side_length, 0, side_length,
	   -side_length,0,side_length},
	{ -side_length, 0, -side_length,
	   side_length, 0, -side_length,
	   side_length, 0, side_length}
};
GLfloat cone_RGB[6][9];

GLuint vbo_line[2];
GLfloat line[3][6];
GLfloat line_RGB[3][6];

float cube_y;
float up_rect_rotate;
float movey;
float addy;
float open_front_x;
float addx;
float open_back_scale;
float sizex;

bool surface_on;
bool rotate_y_on;
bool up_rect_rotate_on;
bool LR_up_on;
bool open_front_on;
bool open_back_on;
bool perspec_on;

float cone_open_radian[4];
float add_cone_radian_all[4];
float add_cone_radian_one[4];
bool cone_on;
bool cone_open_on;
bool cone_open_one_on;



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
		for (int k = 0; k < 12; ++k)
		{
			cube_RGB[i][k] = color(gen);
			cube_RGB[i][k] = color(gen);
			cube_RGB[i][k] = color(gen);
			cube_RGB[i][k] = color(gen);
			cube_RGB[i][k] = color(gen);
			cube_RGB[i][k] = color(gen);
		}
		for (int k = 0; k < 9; ++k)
		{
			cone_RGB[i][k] = color(gen);
			cone_RGB[i][k] = color(gen);
			cone_RGB[i][k] = color(gen);
			cone_RGB[i][k] = color(gen);
			cone_RGB[i][k] = color(gen);
			cone_RGB[i][k] = color(gen);
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

	cube_y = 30.0f;
	addy = 0.01f;

	addx = 1.0f;

	open_back_scale = 1;
	sizex = -0.01f;

	add_cone_radian_all[0] = 3.0f;
	add_cone_radian_all[1] = 3.0f;
	add_cone_radian_all[2] = 3.0f;
	add_cone_radian_all[3] = 3.0f;

	add_cone_radian_one[0] = 3.0f;
	add_cone_radian_one[1] = 3.0f;
	add_cone_radian_one[2] = 3.0f;
	add_cone_radian_one[3] = 3.0f;

	{	//����
		std::cout << "h: �������� ���� / ����" << std::endl;
		std::cout << "y: y�࿡ ���Ͽ� ���� / ����" << std::endl;
		std::cout << "t: ����ü�� ���� �ִϸ��̼� ���� / ����" << std::endl;
		std::cout << "f: ����ü�� �ո� ���� / �ݱ�" << std::endl;
		std::cout << "s: ����ü�� ���� ���� / �ݱ�" << std::endl;
		std::cout << "b: ����ü�� �޸� ���� / �ݱ�" << std::endl;
		std::cout << "p: �������� / ��������" << std::endl;
		std::cout << "n: ����ü / �簢�� ����" << std::endl;
		std::cout << "o: �簢�� ����/ ����" << std::endl;
		std::cout << "r: �簢�� �Ѹ龿 ���� / ����" << std::endl;

	}
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
	rColor = gColor = bColor = 0.2f;
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

	//�������
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

	//�� ���
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f); //--- ī�޶� ��ġ
	glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- ī�޶� �ٶ󺸴� ����
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����
	glm::mat4 view = glm::mat4(1.0f);

	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	glUniformMatrix4fv(view_location, 1, GL_FALSE, &view[0][0]);



	glm::mat4 lines = glm::mat4(1.0f);
	lines = glm::rotate(lines, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	lines = glm::rotate(lines, glm::radians(-30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	unsigned int lines_location = glGetUniformLocation(shaderProgramID, "transform");
	glUniformMatrix4fv(lines_location, 1, GL_FALSE, glm::value_ptr(lines));

	for (int i = 0; i < 3; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_line[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 6 * sizeof(GLfloat)));
		glBindBuffer(GL_ARRAY_BUFFER, vbo_line[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 6 * sizeof(GLfloat)));
		glLineWidth(1);
		glDrawArrays(GL_LINES, 0, 2);
	}


	glm::mat4 trans = glm::mat4(1.0f);
	if (surface_on) {
		glEnable(GL_DEPTH_TEST);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}
	trans = glm::rotate(trans, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	trans = glm::rotate(trans, glm::radians(cube_y), glm::vec3(0.0f, 1.0f, 0.0f));


	unsigned int shape_location = glGetUniformLocation(shaderProgramID, "transform");
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(trans));
	unsigned int cone_location = glGetUniformLocation(shaderProgramID, "transform");
	unsigned int cube_location = glGetUniformLocation(shaderProgramID, "transform");

	if (cone_on) {
		glBindBuffer(GL_ARRAY_BUFFER, conevbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, conevbo[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glDrawArrays(GL_TRIANGLES, 12, 3);//�ٴ�1
		glDrawArrays(GL_TRIANGLES, 15, 3);//�ٴ�2

		glm::mat4 front_rect = trans;
		front_rect = glm::translate(front_rect, glm::vec3(0.0f, 0.0f, side_length));
		front_rect = glm::rotate(front_rect, glm::radians(cone_open_radian[0]), glm::vec3(1.0f, 0.0f, 0.0f));
		front_rect = glm::translate(front_rect, glm::vec3(0.0f, 0.0f, -side_length));
		glUniformMatrix4fv(cone_location, 1, GL_FALSE, glm::value_ptr(front_rect));
		glDrawArrays(GL_TRIANGLES, 0, 3);//��

		glm::mat4 left_rect = trans;
		left_rect = glm::translate(left_rect, glm::vec3(-side_length, 0.0f, 0.0f));
		left_rect = glm::rotate(left_rect, glm::radians(cone_open_radian[1]), glm::vec3(0.0f, 0.0f, 1.0f));
		left_rect = glm::translate(left_rect, glm::vec3(side_length, 0.0f, 0.0f));
		glUniformMatrix4fv(cone_location, 1, GL_FALSE, glm::value_ptr(left_rect));
		glDrawArrays(GL_TRIANGLES, 3, 3);//��

		glm::mat4 back_rect = trans;
		back_rect = glm::translate(back_rect, glm::vec3(0.0f, 0.0f, -side_length));
		back_rect = glm::rotate(back_rect, glm::radians(-cone_open_radian[2]), glm::vec3(1.0f, 0.0f, 0.0f));
		back_rect = glm::translate(back_rect, glm::vec3(0.0f, 0.0f, side_length));
		glUniformMatrix4fv(cone_location, 1, GL_FALSE, glm::value_ptr(back_rect));
		glDrawArrays(GL_TRIANGLES, 6, 3);//��

		glm::mat4 right_rect = trans;
		right_rect = glm::translate(right_rect, glm::vec3(side_length, 0.0f, 0.0f));
		right_rect = glm::rotate(right_rect, glm::radians(-cone_open_radian[3]), glm::vec3(0.0f, 0.0f, 1.0f));
		right_rect = glm::translate(right_rect, glm::vec3(-side_length, 0.0f, 0.0f));
		glUniformMatrix4fv(cone_location, 1, GL_FALSE, glm::value_ptr(right_rect));
		glDrawArrays(GL_TRIANGLES, 9, 3);//��






	}
	else
	{		
		glBindBuffer(GL_ARRAY_BUFFER, cubevbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, cubevbo[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

		{//�Ʒ�
			glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
		}

		{//��
			glm::mat4 front_rect = trans;
			front_rect = glm::translate(front_rect, glm::vec3(0.0f, -side_length, side_length));
			front_rect = glm::rotate(front_rect, glm::radians(open_front_x), glm::vec3(1.0f, 0.0f, 0.0f));
			front_rect = glm::translate(front_rect, glm::vec3(0.0f, side_length, -side_length));
			glUniformMatrix4fv(cube_location, 1, GL_FALSE, glm::value_ptr(front_rect));
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		{//��
			glm::mat4 back_rect = trans;
			back_rect = glm::translate(back_rect, glm::vec3(0.0f, 0.0f, -side_length));
			back_rect = glm::scale(back_rect, glm::vec3(open_back_scale, open_back_scale, open_back_scale));
			back_rect = glm::translate(back_rect, glm::vec3(0.0f, 0.0f, side_length));
			glUniformMatrix4fv(cube_location, 1, GL_FALSE, glm::value_ptr(back_rect));
			glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
		}


		{//������
			glm::mat4 LR_rect = trans;
			LR_rect = glm::translate(LR_rect, glm::vec3(0.0f, movey, 0.0f));
			glUniformMatrix4fv(cube_location, 1, GL_FALSE, glm::value_ptr(LR_rect));
			glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
		}
		{//����
			glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
		}


		{//��
			glm::mat4 up_rect = trans;
			up_rect = glm::translate(up_rect, glm::vec3(0.0f, side_length, 0.0f));
			up_rect = glm::rotate(up_rect, glm::radians(up_rect_rotate), glm::vec3(1.0f, 0.0f, 0.0f));
			up_rect = glm::translate(up_rect, glm::vec3(0.0f, -side_length, 0.0f));
			glUniformMatrix4fv(cube_location, 1, GL_FALSE, glm::value_ptr(up_rect));
			glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
		}
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
	glGenBuffers(2, vbo_line);
	glGenBuffers(2, conevbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, cubevbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_RGB), cube_RGB, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, conevbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cone), cone, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, conevbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cone_RGB), cone_RGB, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_RGB), line_RGB, GL_STATIC_DRAW);

}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'h':
		if (surface_on)
			surface_on = false;
		else
			surface_on = true;
		break;
	case 'y':
		if (rotate_y_on) {
			rotate_y_on = false;
		}
		else {
			rotate_y_on = true;
			glutTimerFunc(10, rotate_y, 0);
		}
		break;
	case 't':
		if (!cone_on) {
			if (up_rect_rotate_on) {
				up_rect_rotate_on = false;
			}
			else {
				up_rect_rotate_on = true;
				glutTimerFunc(10, up_rect_rotate_x, 0);
			}
		}
		break;
	case 'f':
		if (!cone_on) {

			if (!open_front_on) {
				open_front_on = true;
				glutTimerFunc(10, open_front, 0);
			}
		}
		break;
	case 's':
		if (!cone_on) {
			if (!LR_up_on) {
				LR_up_on = true;
				glutTimerFunc(10, LR_rect_up_y, 0);
			}
		}
		break;
	case 'b':
		if (!cone_on) {
			if (!open_back_on) {
				open_back_on = true;
				glutTimerFunc(10, open_back, 0);
			}
		}
		break;
	case 'p':
		if (perspec_on)
			perspec_on = false;
		else
			perspec_on = true;
		break;
	case 'n':
		if (cone_on)
			cone_on = false;
		else
			cone_on = true;
		break;
	case 'o':
		if (cone_on) {
			if (!cone_open_on) {
				add_cone_radian_one[0] = 3.0f;
				add_cone_radian_one[1] = 3.0f;
				add_cone_radian_one[2] = 3.0f;
				add_cone_radian_one[3] = 3.0f;
				cone_open_on = true;
				glutTimerFunc(10, open_cone, 0);
				glutTimerFunc(10, open_cone, 1);
				glutTimerFunc(10, open_cone, 2);
				glutTimerFunc(10, open_cone, 3);
			}
		}
		break;
	case 'r':
		if (cone_on) {
			if (!cone_open_one_on)
			{
				cone_open_one_on = true;
				glutTimerFunc(1, open_cone_one, 0);
			}
		}
		break;
	}
	glutPostRedisplay();
}
void rotate_y(int value) {
	cube_y += 1.0f;

	if (rotate_y_on) {
		glutTimerFunc(1, rotate_y, 0);
	}
	glutPostRedisplay();
}
void up_rect_rotate_x(int value) {
	up_rect_rotate += 1.0f;

	if (up_rect_rotate_on) {
		glutTimerFunc(1, up_rect_rotate_x, 0);
	}
	glutPostRedisplay();
}
void LR_rect_up_y(int value) {
	movey += addy;
	if ((movey >= 0.4) || (movey <= 0)) {
		LR_up_on = false;
		addy *= -1;
	}
	else if(LR_up_on){
		glutTimerFunc(1, LR_rect_up_y, 0);
	}
	glutPostRedisplay();
}
void open_front(int value) {
	open_front_x += addx;
	if ((open_front_x >= 90) || (open_front_x <= 0)) {
		open_front_on = false;
		addx *= -1;
	}
	else if (open_front_on) {
		glutTimerFunc(1, open_front, 0);
	}
	glutPostRedisplay();
}
void open_back(int value) {
	open_back_scale += sizex;
	if ((open_back_scale >= 1) || (open_back_scale <= 0)) {
		open_back_on = false;
		sizex *= -1;
	}
	else if (open_back_on) {
		glutTimerFunc(1, open_back, 0);
	}
	glutPostRedisplay();
}

void open_cone(int value) {
	cone_open_radian[value] += add_cone_radian_all[value];
	if ((cone_open_radian[value] <= 0) || (cone_open_radian[value] >= 233)) {
		cone_open_on = false;
		add_cone_radian_all[value] *= -1;
	}
	else if (cone_open_on) {
		glutTimerFunc(1, open_cone, value);
	}
	glutPostRedisplay();
}
void open_cone_one(int value) {
	cone_open_radian[value] += add_cone_radian_one[value];
	if ((cone_open_radian[value] <= 0) || (cone_open_radian[value] >= 120)) {
		add_cone_radian_one[value] *= -1;
		if (value < 3)
		{
			glutTimerFunc(1, open_cone_one, value + 1);
		}
		else {
			cone_open_one_on = false;
		}
	}
	else {
		glutTimerFunc(1, open_cone_one, value);
	}
	glutPostRedisplay();
}