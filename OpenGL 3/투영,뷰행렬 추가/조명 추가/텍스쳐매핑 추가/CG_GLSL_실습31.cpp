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
#define side_length 0.2
#define board_size 3
#define PI 3.14159265
#define snow_count 100
void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char* file);
void InitBuffer();
GLvoid Keyboard(unsigned char key, int x, int y);
void rotate_y(int);
void snowlain(int value);
void planet_move(int value);
void draw_tetra(glm::mat4 tetra_model, int count);
GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID;
GLuint vao;


std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> under(0.0, 0.7);
std::uniform_real_distribution<> random_color(0.1, 1);
std::uniform_real_distribution<> random_pos(-2.5, 2.5);
std::uniform_real_distribution<> random_speed(0.01, 0.04);

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

class Snow : public Shape {
public:
	glm::vec3 pos;
	float drop_speed;

	void Setting() {
		pos.x = random_pos(gen);
		pos.y = 2.f;
		pos.z = random_pos(gen);
		drop_speed = random_speed(gen);
	}
	void drop_snow() {
		pos.y -= drop_speed;
		if (pos.y < -0.5) {
			pos.y = 2.f;
		}
	}
};

class Planet : public Shape {
public:
	glm::vec3 pos;
	float randian;
};
class Tetra : public Shape {
public:

};

class Rect :public Shape {
public:
	float x = random_pos(gen);
	float z = random_pos(gen);

	void set_alpha() {
		glm::vec4* colordata = new glm::vec4[vertex_count];
		for (int i = 0; i < vertex_count; ++i) {
			colordata[i] = { 1.f,0.5f,0.5f, 0.5f};
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(glm::vec4), colordata, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
		delete[] colordata;
		colordata = nullptr;
	}
};
GLuint vbo_line[2];
GLfloat line[3][6];
GLfloat line_RGB[3][6];

Shape floor_cube;
Snow snow[snow_count];
bool snow_lain;
glm::vec3 light_pos;
glm::vec3 default_light_pos;
glm::vec3 light_color;

int light_count;
float light_rail[200][3];
float light_rotate_y;
bool rotate_light_on;
int light_rotate_LR;

bool rotate_camera;
float rotate_camera_y;

int planet_count[3];
float rail[200][3];

Planet planet[3];
Tetra tetra;
Rect rect[5];
int tetra_mode;
int light_pos_count;
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

	{	//설명		
		std::cout << "y/Y: 중점을 기준으로 조명 y축 회전" << std::endl;
		std::cout << "s/S: 애니메이션 정지" << std::endl;
		std::cout << "q/Q: 프로그램 종료" << std::endl;
	}

	{
		default_light_pos.x = light_pos.x = 1.f;
		default_light_pos.y = light_pos.y = 1.f;
		default_light_pos.z = light_pos.z = 2.f;
		light_color.x = 1.0f;
		light_color.y = 1.0f;
		light_color.z = 1.0f;
		float x = 0;
		float z = 0;
		float light_angle = 0;
		for (int i = 0; i < 200; ++i) {
			light_angle = (i + 30) * 1.8f;
			x = cos(light_angle * PI / 180) * default_light_pos.z;
			z = sin(light_angle * PI / 180) * default_light_pos.z;
			light_rail[i][0] = x;
			light_rail[i][1] = light_pos.y;
			light_rail[i][2] = z;
		}
	}
	{
		float x = 0;
		float z = 0;
		float angle = 0;
		for (int i = 0; i < 200; ++i) {
			angle = i * 1.8f;
			x = cos(angle * PI / 180) * 2.f;
			z = sin(angle * PI / 180) * 2.f;
			rail[i][0] = x;
			rail[i][1] = 1.f;
			rail[i][2] = z;
		}
	}

	{
		tetra_mode = 0;
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
	glm::vec3 cameraPos = glm::vec3(0.f, 2.0f, 4.0f); //--- 카메라 위치
	glm::vec3 cameraDirection = glm::vec3(0.f, 0.f, 0.f); //--- 카메라 바라보는 방향
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	view = glm::rotate(view, glm::radians(rotate_camera_y),glm::vec3(0.f, 1.f, 0.f));
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
	{
		glBindBuffer(GL_ARRAY_BUFFER, floor_cube.vbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, floor_cube.vbo[1]);
		glVertexAttribPointer(ColorLocation, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, floor_cube.vbo[2]);
		glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, floor_cube.vbo[3]);
		glVertexAttribPointer(TextureLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		glm::mat4 ground_cube = base;
		ground_cube = glm::translate(ground_cube, glm::vec3(-7.f, -1.0f, -7.f));
		ground_cube = glm::scale(ground_cube, glm::vec3(15.f, 1.f, 15.f));
		glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(ground_cube));
		glDrawArrays(GL_TRIANGLES, 0, floor_cube.vertex_count);
	}
	
	{
		glBindBuffer(GL_ARRAY_BUFFER, tetra.vbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, tetra.vbo[1]);
		glVertexAttribPointer(ColorLocation, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, tetra.vbo[2]);
		glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, tetra.vbo[3]);
		glVertexAttribPointer(TextureLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		glm::mat4 tetra_model = trans;
		unsigned int tetra_location = glGetUniformLocation(shaderProgramID, "transform");
		glUniformMatrix4fv(tetra_location, 1, GL_FALSE, glm::value_ptr(tetra_model));
		draw_tetra(tetra_model, tetra_mode);
	}
	



	for (int i = 0; i < snow_count; ++i) {

		glBindBuffer(GL_ARRAY_BUFFER, snow[i].vbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, snow[i].vbo[1]);
		glVertexAttribPointer(ColorLocation, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, snow[i].vbo[2]);
		glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, snow[i].vbo[3]);
		glVertexAttribPointer(TextureLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		glm::mat4 snow_model = trans;
		snow_model = glm::translate(snow_model, glm::vec3(snow[i].pos));
		snow_model = glm::scale(snow_model, glm::vec3(0.05, 0.05, 0.05));
		unsigned int snow_location = glGetUniformLocation(shaderProgramID, "transform");
		glUniformMatrix4fv(snow_location, 1, GL_FALSE, glm::value_ptr(snow_model));
		glDrawArrays(GL_TRIANGLES, 0, snow[i].vertex_count);
	}
	for (int i = 0; i < 3; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, planet[i].vbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, planet[i].vbo[1]);
		glVertexAttribPointer(ColorLocation, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, planet[i].vbo[2]);
		glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, planet[i].vbo[3]);
		glVertexAttribPointer(TextureLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		glm::mat4 planet_model = trans;
		planet_model = glm::translate(planet_model, glm::vec3(0.f,1.f,0.f));
		planet_model = glm::rotate(planet_model,glm::radians(planet[i].randian), glm::vec3(1.f, 0.f, 0.f));
		planet_model = glm::translate(planet_model, glm::vec3(planet[i].pos));
		planet_model = glm::scale(planet_model, glm::vec3(0.3, 0.3, 0.3));
		unsigned int planet_location = glGetUniformLocation(shaderProgramID, "transform");
		glUniformMatrix4fv(planet_location, 1, GL_FALSE, glm::value_ptr(planet_model));
		glDrawArrays(GL_TRIANGLES, 0, planet[i].vertex_count);
	}
	glDisable(GL_CULL_FACE);
	for (int i = 0; i < 5; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, rect[i].vbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, rect[i].vbo[1]);
		glVertexAttribPointer(ColorLocation, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, rect[i].vbo[2]);
		glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, rect[i].vbo[3]);
		glVertexAttribPointer(TextureLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		glm::mat4 rect_model = trans;
		rect_model = glm::translate(rect_model, glm::vec3(rect[i].x,0.f,rect[i].z));
		rect_model = glm::scale(rect_model, glm::vec3(0.3, 3.0, 0.3));
		unsigned int rect_location = glGetUniformLocation(shaderProgramID, "transform");
		glUniformMatrix4fv(rect_location, 1, GL_FALSE, glm::value_ptr(rect_model));
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawArrays(GL_TRIANGLES, 0, rect[i].vertex_count);
		glDisable(GL_BLEND);
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

	glGenBuffers(2, vbo_line);
	glBindVertexArray(vao);

	floor_cube.load_Obj("cube_map.obj");
	floor_cube.Set_color(0.5f, 0.5f, 0.5f);
	floor_cube.create_texture("cube.png");

	for (int i = 0; i < snow_count; ++i) {
		snow[i].load_Obj("sphere_white.obj");
		snow[i].Set_color(1.0f, 1.0f, 1.0f);
		snow[i].Setting();
		snow[i].create_texture("white.png");
	}

	for (int i = 0; i < 3; ++i) {
		planet[i].load_Obj("sphere_white.obj");
		planet[i].randian = -45 + (45 * i);
		planet_count[i] = 15 * i;
	}
	planet[0].Set_color(1.f,0.f,0.f);
	planet[1].Set_color(0.f,1.f,0.f);
	planet[2].Set_color(0.f,0.f,1.f);
	for (int i = 0; i < 3; ++i) {
		glutTimerFunc(5*(i+1), planet_move, i);
	}
	
	tetra.load_Obj("tetra_map.obj");
	tetra.Set_color(0.f, 1.f, 1.f);
	for (int i = 0; i < 5; ++i) {
		rect[i].load_Obj("cube_map.obj");
		rect[i].set_alpha();
		rect[i].create_texture("gray.png");
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_RGB), line_RGB, GL_STATIC_DRAW);

}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	float light_x = 0;
	float light_z = 0;
	float light_angle = 0;
	switch (key) {
	case 'y':
	case 'Y':
		if (rotate_camera)
			rotate_camera = false;
		else {
			rotate_camera = true;
			glutTimerFunc(10, rotate_y, 0);
		}
		break;
	case 's':
	case 'S':
		if (!snow_lain) {
			snow_lain = true;
			for (int i = 0; i < snow_count; ++i) {
				glutTimerFunc(10, snowlain, i);
				glutTimerFunc(10, snowlain, i);
			}
		}
		else
			snow_lain = false;
		break;
	case '1':
		tetra_mode = 0;
		break;
	case '2':
		tetra_mode = 1;
		break;
	case '3':
		tetra_mode = 2;
		break;
	case '4':
		tetra_mode = 3;
		break;
	case '5':
		tetra_mode = 4;
		break;
	case 'm':
		if(light_color.x ==1.f || light_color.y == 1.f || light_color.z ==1.f)
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


void snowlain(int value) {
	snow[value].drop_snow();
	if (snow_lain)
		glutTimerFunc(10, snowlain, value);
	glutPostRedisplay();
}
void planet_move(int value) {

	planet[value].pos.x = rail[planet_count[value]][0];
	planet[value].pos.y = rail[planet_count[value]][1];
	planet[value].pos.z = rail[planet_count[value]][2];
	planet_count[value]++;
	
	if (planet_count[value] >= 200)
	{
		planet_count[value] = 0;
	}
	else if (planet_count[value] <= 0) {
		planet_count[value] = 200;
	}
	glutTimerFunc(5 * (value + 1), planet_move, value);
	glutPostRedisplay();
}

void draw_tetra(glm::mat4 tetra_model, int count) {
	if (count == 0) {
		glDrawArrays(GL_TRIANGLES, 0, tetra.vertex_count);
	}
	else {
		for (int i = 0; i < 2; ++i) {
			for (int j = 0; j < 2; ++j) {
				glm::mat4 small_tetra_model = tetra_model;
				small_tetra_model = glm::translate(small_tetra_model, glm::vec3(-0.5f + (1.f * i), 0.f, -0.5f + (1.f * j)));
				small_tetra_model = glm::scale(small_tetra_model, glm::vec3(0.5f, 0.5f, 0.5f));
				unsigned int tetra_location = glGetUniformLocation(shaderProgramID, "transform");
				glUniformMatrix4fv(tetra_location, 1, GL_FALSE, glm::value_ptr(small_tetra_model));
				draw_tetra(small_tetra_model, count - 1);
			}
			glm::mat4 small_tetra_model = tetra_model;
			small_tetra_model = glm::translate(small_tetra_model, glm::vec3(0.f, 1.f, 0.f));
			small_tetra_model = glm::scale(small_tetra_model, glm::vec3(0.5f, 0.5f, 0.5f));
			unsigned int tetra_location = glGetUniformLocation(shaderProgramID, "transform");
			glUniformMatrix4fv(tetra_location, 1, GL_FALSE, glm::value_ptr(small_tetra_model));
			draw_tetra(small_tetra_model, count - 1);
		}
	}
}