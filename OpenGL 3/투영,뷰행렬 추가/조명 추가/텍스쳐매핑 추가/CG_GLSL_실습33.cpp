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
#include <math.h>
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
void rotate_y(int);
GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID;
GLuint vao;


std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> random_color(0.1, 1);
std::uniform_int_distribution<> random_pos(0, 6);

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
		glm::vec4* colordata = new glm::vec4[f_count * 3];
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
				in >> texture[vt_incount].x >> texture[vt_incount].y;
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
			colordata[color_count].a = 1.0f;
			color_count++;
		}

		glGenBuffers(4, vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(glm::vec3), vertexdata, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(glm::vec4), colordata, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
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
	void Set_color(float r, float g, float b) {
		glm::vec4* colordata = new glm::vec4[vertex_count];
		for (int i = 0; i < vertex_count; ++i) {
			colordata[i] = { r,g,b,1.0f };
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(glm::vec4), colordata, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
		delete[] colordata;
		colordata = nullptr;
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
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
	}
};

class Floor :public Shape {
public:
	float x;
	float z;
};
class Obstacle :public Shape {
public:
	glm::vec3 pos = { 0.f,1.f,0.f };
	glm::vec3 scale = { 1.f,4.f,1.f };
	void set_alpha() {
		glm::vec4* colordata = new glm::vec4[vertex_count];
		for (int i = 0; i < vertex_count; ++i) {
			colordata[i] = { 1.f,1.f,1.f, 0.5f };
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(glm::vec4), colordata, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
		delete[] colordata;
		colordata = nullptr;
	}
};
class Body :public Shape {
public:
	glm::vec3 pos = { 0.f,2.0f,0.f };
	glm::vec3 scale = { 0.5f,1.f,0.5f };
	float rad;
};
class Head :public Body {
public:

};
class Leg :public Body {
public:
	glm::vec3 lr_pos = { 0.f,2.0f,0.f };

};

Floor floors[9][9];
Obstacle obs[3];
Obstacle n_obs[3];

Body body;
Head head;
Leg leg[2];



glm::vec3 light_pos;
glm::vec3 default_light_pos;
glm::vec3 light_color;

bool lr;
bool rotate_camera;
float rotate_camera_y;

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


	{	//설명		
		std::cout << "q/Q: 프로그램 종료" << std::endl;
	}

	default_light_pos.x = light_pos.x = 1.f;
	default_light_pos.y = light_pos.y = 1.f;
	default_light_pos.z = light_pos.z = 2.f;
	light_color.x = 1.0f;
	light_color.y = 1.0f;
	light_color.z = 1.0f;
	{
		for (int i = 0; i < 9; ++i) {
			for (int j = 0; j < 9; ++j) {
				floors[i][j].x = -4.0f + 1.0f * i;
				floors[i][j].z = -4.0f + 1.0f * j;
			}
		}
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
	rColor = gColor = bColor = 0.3f;
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	int NormalLocation = glGetAttribLocation(shaderProgramID, "in_Normal"); //	: 2
	int TextureLocation = glGetAttribLocation(shaderProgramID, "vTexCoord"); //	: 3
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
	glm::vec3 cameraPos = glm::vec3(0.f, 10.f, 10.0f); //--- 카메라 위치
	glm::vec3 cameraDirection = glm::vec3(0.f, 0.f, 0.f); //--- 카메라 바라보는 방향
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	view = glm::rotate(view, glm::radians(rotate_camera_y), glm::vec3(0.f, 1.f, 0.f));
	glUniformMatrix4fv(view_location, 1, GL_FALSE, &view[0][0]);


	unsigned int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos");
	glUniform3f(lightPosLocation, light_pos.x, light_pos.y, light_pos.z);
	unsigned int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor");
	glUniform3f(lightColorLocation, light_color.x, light_color.y, light_color.z);
	unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos");
	glUniform3f(viewPosLocation, cameraPos.x, cameraPos.y, cameraPos.z);



	glm::mat4 trans = glm::mat4(1.0f);
	unsigned int shape_location = glGetUniformLocation(shaderProgramID, "transform");
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(trans));
	glm::mat4 base = trans;
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(base));
	for (int i = 0; i < 9; ++i) {
		for (int j = 0; j < 9; ++j) {
			glBindBuffer(GL_ARRAY_BUFFER, floors[i][j].vbo[0]);
			glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
			glBindBuffer(GL_ARRAY_BUFFER, floors[i][j].vbo[1]);
			glVertexAttribPointer(ColorLocation, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
			glBindBuffer(GL_ARRAY_BUFFER, floors[i][j].vbo[2]);
			glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
			glBindBuffer(GL_ARRAY_BUFFER, floors[i][j].vbo[3]);
			glVertexAttribPointer(TextureLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
			glm::mat4 ground_cube = base;
			ground_cube = glm::translate(ground_cube, glm::vec3(floors[i][j].x, 0.f, floors[i][j].z));
			glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(ground_cube));
			glBindTexture(GL_TEXTURE_2D, floors[i][j].texturemap);
			glDrawArrays(GL_TRIANGLES, 0, floors[i][j].vertex_count);
		}
	}
	if (lr) {
		glBindBuffer(GL_ARRAY_BUFFER, body.vbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, body.vbo[1]);
		glVertexAttribPointer(ColorLocation, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, body.vbo[2]);
		glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, body.vbo[3]);
		glVertexAttribPointer(TextureLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		glm::mat4 ground_cube = base;
		ground_cube = glm::translate(ground_cube, glm::vec3(body.pos));
		ground_cube = glm::scale(ground_cube, glm::vec3(body.scale));
		ground_cube = glm::rotate(ground_cube, glm::radians(body.rad), glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(ground_cube));
		glBindTexture(GL_TEXTURE_2D, body.texturemap);
		glDrawArrays(GL_TRIANGLES, 0, body.vertex_count);

		glBindBuffer(GL_ARRAY_BUFFER, head.vbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, head.vbo[1]);
		glVertexAttribPointer(ColorLocation, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, head.vbo[2]);
		glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, head.vbo[3]);
		glVertexAttribPointer(TextureLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		ground_cube = base;
		ground_cube = glm::translate(ground_cube, glm::vec3(head.pos));
		ground_cube = glm::scale(ground_cube, glm::vec3(head.scale));
		ground_cube = glm::rotate(ground_cube, glm::radians(head.rad), glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(ground_cube));
		glBindTexture(GL_TEXTURE_2D, head.texturemap);
		glDrawArrays(GL_TRIANGLES, 0, head.vertex_count);

		for (int i = 0; i < 2; ++i) {
			glBindBuffer(GL_ARRAY_BUFFER, leg[i].vbo[0]);
			glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
			glBindBuffer(GL_ARRAY_BUFFER, leg[i].vbo[1]);
			glVertexAttribPointer(ColorLocation, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
			glBindBuffer(GL_ARRAY_BUFFER, leg[i].vbo[2]);
			glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
			glBindBuffer(GL_ARRAY_BUFFER, leg[i].vbo[3]);
			glVertexAttribPointer(TextureLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
			ground_cube = base;
			ground_cube = glm::translate(ground_cube, glm::vec3(leg[i].lr_pos));
			ground_cube = glm::scale(ground_cube, glm::vec3(leg[i].scale));
			ground_cube = glm::rotate(ground_cube, glm::radians(leg[i].rad), glm::vec3(0.f, 1.f, 0.f));
			glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(ground_cube));
			glBindTexture(GL_TEXTURE_2D, leg[i].texturemap);
			glDrawArrays(GL_TRIANGLES, 0, leg[i].vertex_count);
		}
	}
	else {
		glBindBuffer(GL_ARRAY_BUFFER, body.vbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, body.vbo[1]);
		glVertexAttribPointer(ColorLocation, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, body.vbo[2]);
		glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, body.vbo[3]);
		glVertexAttribPointer(TextureLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		glm::mat4 ground_cube = base;
		ground_cube = glm::translate(ground_cube, glm::vec3(body.pos));
		ground_cube = glm::scale(ground_cube, glm::vec3(body.scale));
		ground_cube = glm::rotate(ground_cube, glm::radians(body.rad), glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(ground_cube));
		glBindTexture(GL_TEXTURE_2D, body.texturemap);
		glDrawArrays(GL_TRIANGLES, 0, body.vertex_count);

		glBindBuffer(GL_ARRAY_BUFFER, head.vbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, head.vbo[1]);
		glVertexAttribPointer(ColorLocation, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, head.vbo[2]);
		glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, head.vbo[3]);
		glVertexAttribPointer(TextureLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		ground_cube = base;
		ground_cube = glm::translate(ground_cube, glm::vec3(head.pos));
		ground_cube = glm::scale(ground_cube, glm::vec3(head.scale));
		ground_cube = glm::rotate(ground_cube, glm::radians(head.rad), glm::vec3(0.f, 1.f, 0.f));
		glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(ground_cube));
		glBindTexture(GL_TEXTURE_2D, head.texturemap);
		glDrawArrays(GL_TRIANGLES, 0, head.vertex_count);

		for (int i = 0; i < 2; ++i) {
			glBindBuffer(GL_ARRAY_BUFFER, leg[i].vbo[0]);
			glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
			glBindBuffer(GL_ARRAY_BUFFER, leg[i].vbo[1]);
			glVertexAttribPointer(ColorLocation, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
			glBindBuffer(GL_ARRAY_BUFFER, leg[i].vbo[2]);
			glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
			glBindBuffer(GL_ARRAY_BUFFER, leg[i].vbo[3]);
			glVertexAttribPointer(TextureLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
			ground_cube = base;
			ground_cube = glm::translate(ground_cube, glm::vec3(leg[i].pos));
			ground_cube = glm::scale(ground_cube, glm::vec3(leg[i].scale));
			ground_cube = glm::rotate(ground_cube, glm::radians(leg[i].rad), glm::vec3(0.f, 1.f, 0.f));
			glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(ground_cube));
			glBindTexture(GL_TEXTURE_2D, leg[i].texturemap);
			glDrawArrays(GL_TRIANGLES, 0, leg[i].vertex_count);
		}
	}
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (int i = 0; i < 3; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, obs[i].vbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, obs[i].vbo[1]);
		glVertexAttribPointer(ColorLocation, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, obs[i].vbo[2]);
		glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, obs[i].vbo[3]);
		glVertexAttribPointer(TextureLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		glm::mat4 ground_cube = base;
		ground_cube = glm::translate(ground_cube, glm::vec3(obs[i].pos));
		glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(ground_cube));
		glBindTexture(GL_TEXTURE_2D, obs[i].texturemap);
		glDrawArrays(GL_TRIANGLES, 0, obs[i].vertex_count);
	}
	for (int i = 0; i < 3; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, n_obs[i].vbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, n_obs[i].vbo[1]);
		glVertexAttribPointer(ColorLocation, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, n_obs[i].vbo[2]);
		glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, n_obs[i].vbo[3]);
		glVertexAttribPointer(TextureLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		glm::mat4 ground_cube = base;
		ground_cube = glm::translate(ground_cube, glm::vec3(n_obs[i].pos));
		ground_cube = glm::scale(ground_cube, glm::vec3(n_obs[i].scale));
		glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(ground_cube));
		glBindTexture(GL_TEXTURE_2D, n_obs[i].texturemap);
		glDrawArrays(GL_TRIANGLES, 0, n_obs[i].vertex_count);
	}
	glDisable(GL_BLEND);

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
	vertexSource = filetobuf("vertex_31.glsl");
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
	fragmentSource = filetobuf("fragment_31.glsl");
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

	for (int i = 0; i < 9; ++i) {
		for (int j = 0; j < 9; ++j) {
			floors[i][j].load_Obj("./cube_33.obj");
			floors[i][j].Set_color(1.f, 1.f, 1.f);
			if ((i + j)% 2 == 0) {
				floors[i][j].create_texture("./floor_1.png");
			}
			else{
				floors[i][j].create_texture("./floor_2.png");
			}
		}
	}
	for (int i = 0; i < 3; ++i) {
		obs[i].load_Obj("./cube_33.obj");
		obs[i].set_alpha();
		obs[i].create_texture("./sky.png");
		obs[i].pos.x = floors[random_pos(gen)][random_pos(gen)].x;
		obs[i].pos.z = floors[random_pos(gen)][random_pos(gen)].z;
		n_obs[i].load_Obj("./cube_33.obj");
		n_obs[i].set_alpha();
		n_obs[i].create_texture("./cube.png");
	}
	n_obs[0].pos.x = floors[random_pos(gen)][random_pos(gen)].x;
	n_obs[0].pos.z = floors[random_pos(gen)][random_pos(gen)].z;
	n_obs[1].pos.x = n_obs[0].pos.x + 3.0f;
	n_obs[1].pos.z = n_obs[0].pos.z;
	n_obs[2].pos = { n_obs[0].pos.x+1.5f ,3.f ,n_obs[0].pos.z };
	n_obs[2].scale = { 4.f,1.f,1.f };

	body.load_Obj("./cube_33.obj");
	body.Set_color(1.f, 1.f, 1.f);
	body.create_texture("./body.png");

	head.load_Obj("./cube_33.obj");
	head.Set_color(1.f, 1.f, 1.f);
	head.create_texture("./body.png");
	head.scale = { 0.3f,0.5f,0.3f };
	head.pos = { 0.f,2.8f,0.f };

	for (int i = 0; i < 2; ++i) {
		leg[i].load_Obj("./cube_33.obj");
		leg[i].Set_color(1.f, 1.f, 1.f);
		leg[i].create_texture("./body.png");
		leg[i].scale = { 0.2f,1.f,0.2f };
	}
	leg[0].pos = { -0.15f,1.3f,0.f };
	leg[1].pos = { 0.15f,1.3f,0.f };
	leg[0].lr_pos = { 0.f,1.3f,-0.15f };
	leg[1].lr_pos = { 0.f,1.3f,0.15f };
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	float temp;
	switch (key) {
	case 'w':
	case 'W':
		lr = false;
		head.rad = 0.f;
		body.rad = 0.f;
		leg[0].rad = 0.f;
		leg[1].rad = 0.f;
		head.pos.z -= 0.3f;
		body.pos.z -= 0.3f;
		leg[0].pos.z -= 0.3f;
		leg[1].pos.z -= 0.3f;
		leg[0].lr_pos.z -= 0.3f;
		leg[1].lr_pos.z -= 0.3f;
		break;
	case 's':
	case 'S':
		lr = false;
		head.rad = 0.f;
		body.rad = 0.f;
		leg[0].rad = 0.f;
		leg[1].rad = 0.f;
		head.pos.z += 0.3f;
		body.pos.z += 0.3f;
		leg[0].pos.z += 0.3f;
		leg[1].pos.z += 0.3f;
		leg[0].lr_pos.z += 0.3f;
		leg[1].lr_pos.z += 0.3f;
		break;
	case 'a':
	case 'A':
		lr = true;
		head.rad = 90.f;
		body.rad = 90.f;
		leg[0].rad = 90.f;
		leg[1].rad = 90.f;
		head.pos.x -= 0.3f;
		body.pos.x -= 0.3f;
		leg[0].pos.x -= 0.3f;
		leg[1].pos.x -= 0.3f;
		leg[0].lr_pos.x -= 0.3f;
		leg[1].lr_pos.x -= 0.3f;
		break;
	case 'd':
	case 'D':
		lr = true;
		head.rad = 90.f;
		body.rad = 90.f;
		leg[0].rad = 90.f;
		leg[1].rad = 90.f;
		head.pos.x += 0.3f;
		body.pos.x += 0.3f;
		leg[0].pos.x += 0.3f;
		leg[1].pos.x += 0.3f;
		leg[0].lr_pos.x += 0.3f;
		leg[1].lr_pos.x += 0.3f;
		break;
	case 'y':
	case 'Y':
		if (rotate_camera)
			rotate_camera = false;
		else {
			rotate_camera = true;
			glutTimerFunc(10, rotate_y, 0);
		}
		break;
	case 'm':
		if (light_color.x == 1.f || light_color.y == 1.f || light_color.z == 1.f)
			light_color = { 0.2f,0.2f,0.2f };
		else {
			light_color = { 1.f,1.f,1.f };
		}
		break;
	case 'q':
	case 'Q':
		exit(0);
		break;
	}
	glutPostRedisplay();
}
void rotate_y(int value) {
	rotate_camera_y += 1.0f;
	if (rotate_camera)
		glutTimerFunc(10, rotate_y, value);
	glutPostRedisplay();
}
