#define  _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/ext.hpp>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext/matrix_transform.hpp>
#include <stdlib.h>
#include <random>
#include <fstream>
#include "stb_image.h"

#define Width 1200
#define Height 800
#define PI 3.14159265

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
void rotate_light(int);

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> random_color(0, 1);

class Shape {
public:
	GLuint vbo[4];
	int vertex_count;

	void load_Obj(const char* path)
	{
		int v_count = 0;
		int n_count = 0;
		int f_count = 0;
		int vt_count = 0;
		std::string lineHeader;
		std::ifstream in(path);
		if (!in) {
			std::cerr << path << "파일 못찾음";
			exit(1);
		}
		while (in >> lineHeader) {
			if (lineHeader == "v")	++v_count;
			else if (lineHeader == "f")	++f_count;
			else if (lineHeader == "vt") ++vt_count;
		}
		in.close();
		in.open(path);

		glm::vec3* vertex = new glm::vec3[v_count];
		glm::vec3* face = new glm::vec3[f_count];
		glm::vec3* face_text = new glm::vec3[f_count];
		glm::vec2* texture = new glm::vec2[vt_count];
		glm::vec3* vertexdata = new glm::vec3[f_count * 3];
		glm::vec3* normaldata = new glm::vec3[f_count * 3];
		glm::vec3* colordata = new glm::vec3[f_count * 3];
		glm::vec2* texdata = new glm::vec2[f_count * 3];
		vertex_count = f_count * 3;
		int v_incount = 0;
		int vt_incount = 0;
		int f_incount = 0;
		int color_count = 0;
		while (in >> lineHeader) {
			if (lineHeader == "v") {
				in >> vertex[v_incount].x >> vertex[v_incount].y >> vertex[v_incount].z;
				++v_incount;
			}
			else if (lineHeader == "vt") {
				in >> texture[vt_incount].x >> texture[vt_incount].y ;
				++vt_incount;
			}
			else if (lineHeader == "f") {
				in >> face[f_incount].x >> face[f_incount].y >> face[f_incount].z >> face_text[f_incount].x >> face_text[f_incount].y >> face_text[f_incount].z;
				vertexdata[f_incount * 3 + 0] = vertex[static_cast<int>(face[f_incount].x - 1)];
				vertexdata[f_incount * 3 + 1] = vertex[static_cast<int>(face[f_incount].y - 1)];
				vertexdata[f_incount * 3 + 2] = vertex[static_cast<int>(face[f_incount].z - 1)];

				texdata[f_incount * 3 + 0] = texture[static_cast<int>(face_text[f_incount].x - 1)];
				texdata[f_incount * 3 + 1] = texture[static_cast<int>(face_text[f_incount].y - 1)];
				texdata[f_incount * 3 + 2] = texture[static_cast<int>(face_text[f_incount].z - 1)];
				++f_incount;
			}
		}
		for (int i = 0; i < vertex_count / 3; i++)
		{
			glm::vec3 normal = glm::cross(vertexdata[i * 3 + 1] - vertexdata[i * 3 + 0], vertexdata[i * 3 + 2] - vertexdata[i * 3 + 0]);
			normaldata[i * 3 + 0] = normal;
			normaldata[i * 3 + 1] = normal;
			normaldata[i * 3 + 2] = normal;

		}
		for (int i = 0; i < vertex_count; ++i) {
			colordata[color_count].x = random_color(gen);
			colordata[color_count].y = random_color(gen);
			colordata[color_count].z = random_color(gen);
			color_count++;
		}

		glGenBuffers(4, vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(glm::vec3), vertexdata, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(glm::vec3), colordata, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(glm::vec3), normaldata, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(glm::vec2), texdata, GL_STATIC_DRAW);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(3);

		delete[] vertex;
		vertex = nullptr;
		delete[] face;
		face = nullptr;
		delete[] face_text;
		face_text = nullptr;
		delete[] vertexdata;
		vertexdata = nullptr;
		delete[] normaldata;
		normaldata = nullptr;
		delete[] colordata;
		colordata = nullptr;
		delete[] texdata;
		texdata = nullptr;
	}	
	unsigned int texturemap;
	int width_image, height_image, number_of_channel;
	unsigned char* data;
	void create_texture(const char* path) {
		glGenTextures(1, &texturemap);
		glBindTexture(GL_TEXTURE_2D, texturemap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		data = stbi_load(path, &width_image, &height_image, &number_of_channel, 0);
		if (data)
		{
			if (number_of_channel == 4) {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_image, height_image, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
			else {
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_image, height_image, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

			}
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}
};

class Cube :public Shape {
public:
	
	void white_color() {
		glm::vec3* colordata = new glm::vec3[vertex_count];

		for (int i = 0; i < vertex_count; ++i) {
			colordata[i].x = 1.f;
			colordata[i].y = 1.f;
			colordata[i].z = 1.f;
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(glm::vec3), colordata, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		delete[] colordata;
		colordata = nullptr;
	}
};

class Tetra : public Shape {
public:
	void white_color() {
		glm::vec3* colordata = new glm::vec3[vertex_count];

		for (int i = 0; i < vertex_count; ++i) {
			colordata[i].x = 1.f;
			colordata[i].y = 1.f;
			colordata[i].z = 1.f;
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(glm::vec3), colordata, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		delete[] colordata;
		colordata = nullptr;
	}
};

GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID;
GLuint vao;


Cube cube;
Tetra tetra;
Cube sky;
float rail[200][3];


bool cube_draw;
bool tetra_draw;
bool timer_x;
bool timer_y;
float x_1;
float y_1;
float light_rotate_y;
float addx_1;
float addy_1;
float movex;
float movey;
bool light_on;

glm::vec3 light_pos;
glm::vec3 light_box_pos;
glm::vec3 light_color;

GLUquadricObj* way;
bool rotate_light_on;

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
	light_box_pos.x = light_pos.x = 0.f;
	light_box_pos.y = light_pos.y = 0.f;
	light_box_pos.z = light_pos.z = 1.f;
	light_color.x = 1.0f;
	light_color.y = 1.0f;
	light_color.z = 1.0f;
	cube_draw = true;
	tetra_draw = false;

	float x;
	float z;
	float angle;
	for (int i = 0; i < 200; ++i) {
		angle = (i + 50) * 1.8f;
		x = cos(angle * PI / 180) * light_pos.z;
		z = sin(angle * PI / 180) * light_pos.z;
		rail[i][0] = x;
		rail[i][1] = 0.0f;
		rail[i][2] = z;

	}
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
	rColor = gColor = bColor = 0.3f;
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	int NormalLocation = glGetAttribLocation(shaderProgramID, "in_Normal"); //	: 2
	int TextureLocation = glGetAttribLocation(shaderProgramID, "vTexCoord"); //	: 2
	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation);
	glEnableVertexAttribArray(NormalLocation);
	glEnableVertexAttribArray(TextureLocation);


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
	//view = glm::rotate(view, glm::radians(30.f), glm::vec3(0.f, 1.f, 0.f));
	glUniformMatrix4fv(view_location, 1, GL_FALSE, &view[0][0]);



	unsigned int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos");
	glUniform3f(lightPosLocation, light_pos.x, light_pos.y, light_pos.z);
	unsigned int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor");
	glUniform3f(lightColorLocation, light_color.x, light_color.y, light_color.z);
	unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos");
	glUniform3f(viewPosLocation, cameraPos.x, cameraPos.y, cameraPos.z);

	glFrontFace(GL_CCW);
	glm::mat4 trans = glm::mat4(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	trans = glm::translate(trans, glm::vec3(0.f, -2.f, -10.f));
	trans = glm::scale(trans, glm::vec3(12.f, 12.f, 12.f));
	trans = glm::translate(trans, glm::vec3(-0.5f, -0.5f, -0.5f));
	unsigned int trans_location = glGetUniformLocation(shaderProgramID, "transform");
	glUniformMatrix4fv(trans_location, 1, GL_FALSE, glm::value_ptr(trans));
	glBindBuffer(GL_ARRAY_BUFFER, sky.vbo[0]);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, sky.vbo[1]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, sky.vbo[2]);
	glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, sky.vbo[3]);
	glVertexAttribPointer(TextureLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
	glBindTexture(GL_TEXTURE_2D, sky.texturemap);
	glDrawArrays(GL_TRIANGLES, 0, sky.vertex_count);

	trans = glm::mat4(1.0f);
	if (tetra_draw) {
		trans = glm::rotate(trans, glm::radians(x_1), glm::vec3(1.0f, 0.0f, 0.0f));
		trans = glm::rotate(trans, glm::radians(y_1), glm::vec3(0.0f, 1.0f, 0.0f));
		trans = glm::translate(trans, glm::vec3(0.0f, -0.5f, 0.0f));
		trans = glm::scale(trans, glm::vec3(0.7, 0.7, 0.7));
		unsigned int trans_location = glGetUniformLocation(shaderProgramID, "transform");
		glUniformMatrix4fv(trans_location, 1, GL_FALSE, glm::value_ptr(trans));
		glBindBuffer(GL_ARRAY_BUFFER, tetra.vbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, tetra.vbo[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, tetra.vbo[2]);
		glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, tetra.vbo[3]);
		glVertexAttribPointer(TextureLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		glBindTexture(GL_TEXTURE_2D, tetra.texturemap);
		glDrawArrays(GL_TRIANGLES, 0, tetra.vertex_count);

	}
	else {
		trans = glm::translate(trans, glm::vec3(movex, movey, 0.0f));
		trans = glm::rotate(trans, glm::radians(x_1), glm::vec3(1.0f, 0.0f, 0.0f));
		trans = glm::rotate(trans, glm::radians(y_1), glm::vec3(0.0f, 1.0f, 0.0f));
		trans = glm::translate(trans, glm::vec3(-0.5f, -0.5f, -0.5f));
		unsigned int trans_location = glGetUniformLocation(shaderProgramID, "transform");
		glUniformMatrix4fv(trans_location, 1, GL_FALSE, glm::value_ptr(trans));
		glBindBuffer(GL_ARRAY_BUFFER, cube.vbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, cube.vbo[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, cube.vbo[2]);
		glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, cube.vbo[3]);
		glVertexAttribPointer(TextureLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		glBindTexture(GL_TEXTURE_2D, cube.texturemap);
		glDrawArrays(GL_TRIANGLES, 0, cube.vertex_count);
	}


	glDisableVertexAttribArray(ColorLocation);
	glDisableVertexAttribArray(PosLocation);
	glDisableVertexAttribArray(NormalLocation);
	glDisableVertexAttribArray(TextureLocation);


	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}
void make_vertexShaders()
{
	vertexSource = filetobuf("vertex_mapping.glsl");
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
	fragmentSource = filetobuf("fragment_mapping.glsl");
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

	glBindVertexArray(vao);
	cube.load_Obj("./cube_map.obj");
	cube.create_texture("cube.png");
	cube.white_color();
	tetra.load_Obj("./tetra_map.obj");
	tetra.create_texture("cube.png");
	tetra.white_color();
	sky.load_Obj("./cube_back.obj");
	sky.create_texture("sky.png");
	sky.white_color();
	way = gluNewQuadric();
	gluQuadricNormals(way, GLU_SMOOTH);
	gluQuadricOrientation(way, GLU_OUTSIDE);
	gluQuadricTexture(way, GL_FALSE);

}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	float light_x;
	float light_z;
	float angle;

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
	case 'm':
		if (light_on)
		{
			light_color.x = 0.f;
			light_color.y = 0.f;
			light_color.z = 0.f;
			light_on = false;
		}
		else
		{
			light_color.x = 1.f;
			light_color.y = 1.f;
			light_color.z = 1.f;
			light_on = true;
		}
		break;
	case 'x':
		addx_1 = 1.f;
		if (!timer_x) {
			timer_x = true;
			glutTimerFunc(50, rotate_x, 0);
		}
		else {
			timer_x = false;
		}
		break;

	case 'X':
		addx_1 = -1.f;
		if (!timer_x) {
			timer_x = true;
			glutTimerFunc(50, rotate_x, 0);
		}
		else {
			timer_x = false;
		}
		break;
	case 'y':
		addy_1 = 1.0f;
		if (!timer_y) {
			timer_y = true;
			glutTimerFunc(50, rotate_y, 0);
		}
		else {
			timer_y = false;
		}
		break;
	case 'Y':
		addy_1 = -1.0f;
		if (!timer_y) {
			timer_y = true;
			glutTimerFunc(50, rotate_y, 0);
		}
		else {
			timer_y = false;
		}
		break;
	case 'r':
		if (!rotate_light_on)
		{
			rotate_light_on = true;
			glutTimerFunc(50, rotate_light, 0);
		}
		else
			rotate_light_on = false;
		break;
	case 'z':
		light_pos.z -= 0.3f;
		light_box_pos.z -= 0.3f;
		for (int i = 0; i < 200; ++i) {
			angle = (i + 50) * 1.8f;
			light_x = cos(angle * PI / 180) * light_box_pos.z;
			light_z = sin(angle * PI / 180) * light_box_pos.z;
			rail[i][0] = light_x;
			rail[i][1] = 0.0f;
			rail[i][2] = light_z;

		}
		break;
	case 'Z':
		light_pos.z += 0.3f;
		light_box_pos.z += 0.3f;
		for (int i = 0; i < 200; ++i) {
			angle = (i + 50) * 1.8f;
			light_x = cos(angle * PI / 180) * light_box_pos.z;
			light_z = sin(angle * PI / 180) * light_box_pos.z;
			rail[i][0] = light_x;
			rail[i][1] = 0.0f;
			rail[i][2] = light_z;
		}
		break;
	case 's':
	case 'S':
		x_1 = 0.0f;
		y_1 = 0.0f;
		light_box_pos.x = light_pos.x = 0.f;
		light_box_pos.y = light_pos.y = 0.f;
		light_box_pos.z = light_pos.z = 1.f;
		light_color.x = 1.0f;
		light_color.y = 1.0f;
		light_color.z = 1.0f;
		cube_draw = true;
		tetra_draw = false;
		timer_x = false;
		timer_y = false;
		rotate_light_on = false;
		break;
	case 'q':
	case 'Q':
		exit(0);
		break;
	}
	glutPostRedisplay();
}
void rotate_x(int value) {
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

int light_count;
void rotate_light(int value)
{
	light_pos.x = rail[light_count][0];
	light_pos.y = rail[light_count][1];
	light_pos.z = rail[light_count][2];
	light_count++;
	light_rotate_y -= 1.8f;
	if (light_count >= 200)
	{
		light_count = 0;
		light_rotate_y = 0;
	}
	if (rotate_light_on)
		glutTimerFunc(1, rotate_light, value);
	glutPostRedisplay();

}