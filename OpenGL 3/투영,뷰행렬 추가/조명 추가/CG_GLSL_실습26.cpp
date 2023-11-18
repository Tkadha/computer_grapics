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
#include <fstream>

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

void rotate_y(int);
void rotate_z(int);
void rotate_light(int);

GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID;
GLuint vao;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> random_color(0, 1);

class Shape {
public:
	GLuint vbo[3];
	int vertex_count;
	void load_Obj(const char* path)
	{
		int v_count = 0;
		int n_count = 0;
		int f_count = 0;
		std::string lineHeader;
		std::ifstream in(path);
		if (!in) {
			std::cerr << path << "파일 못찾음";
			exit(1);
		}
		while (in >> lineHeader) {
			if (lineHeader == "v")	++v_count;
			else if (lineHeader == "f")	++f_count;
		}
		in.close();
		in.open(path);

		glm::vec3* vertex = new glm::vec3[v_count];
		glm::vec3* face = new glm::vec3[f_count];
		glm::vec3* vertexdata = new glm::vec3[f_count * 3];
		glm::vec3* normaldata = new glm::vec3[f_count * 3];
		glm::vec3* colordata = new glm::vec3[f_count * 3];
		vertex_count = f_count * 3;
		int v_incount = 0;
		int f_incount = 0;
		int color_count = 0;
		while (in >> lineHeader) {
			if (lineHeader == "v") {
				in >> vertex[v_incount].x >> vertex[v_incount].y >> vertex[v_incount].z;
				++v_incount;
			}
			else if (lineHeader == "f") {
				in >> face[f_incount].x >> face[f_incount].y >> face[f_incount].z;
				vertexdata[f_incount * 3 + 0] = vertex[static_cast<int>(face[f_incount].x - 1)];
				vertexdata[f_incount * 3 + 1] = vertex[static_cast<int>(face[f_incount].y - 1)];
				vertexdata[f_incount * 3 + 2] = vertex[static_cast<int>(face[f_incount].z - 1)];
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
		colordata[0].x = random_color(gen);
		colordata[0].y = random_color(gen);
		colordata[0].z = random_color(gen);
		for (int i = 1; i < vertex_count; ++i) {
			colordata[i].x = colordata[0].x;
			colordata[i].y = colordata[0].y;
			colordata[i].z = colordata[0].z;
		}

		glGenBuffers(3, vbo);
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

		delete[] vertex;
		vertex = nullptr;
		delete[] face;
		face = nullptr;
		delete[] vertexdata;
		vertexdata = nullptr;
		delete[] normaldata;
		normaldata = nullptr;
		delete[] colordata;
		colordata = nullptr;
	}

};

bool perspec_on;
bool solid_line;
float rail[200][3];
float subrail[200][3];
int planet_point[3];
int sub_planet_point[3];

float light_rail[200][3];

float movex;
float movey;
float movez;
float rotatey;
float rotatez;
bool rotatey_on;
bool rotatez_on;
GLUquadricObj* planet;

Shape cube;
Shape sphere;

int light_count;
glm::vec3 light_pos;
glm::vec3 light_box_pos;
glm::vec3 light_color;
float light_rotate_y;
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
	light_color.x = 1.0f;
	light_color.y = 0.f;
	light_color.z = 0.f;
	light_box_pos.x = light_pos.x = 0.f;
	light_box_pos.y = light_pos.y = 0.f;
	light_box_pos.z = light_pos.z = 1.f;

	for (int i = 0; i < 200; ++i) {
		angle = (i + 50) * 1.8f;
		x = cos(angle * PI / 180) * light_pos.z;
		z = sin(angle * PI / 180) * light_pos.z;
		light_rail[i][0] = x;
		light_rail[i][1] = 0.0f;
		light_rail[i][2] = z;
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
	rColor = gColor = bColor = 0.2f;
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
	unsigned int shape_location = glGetUniformLocation(shaderProgramID, "transform");
	unsigned int color_location = glGetUniformLocation(shaderProgramID, "setcolor");



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



	glm::mat4 trans = glm::mat4(1.0f);
	glEnable(GL_DEPTH_TEST);
	trans = glm::translate(trans, glm::vec3(movex, movey, movez));
	trans = glm::rotate(trans, glm::radians(rotatez), glm::vec3(0.0f, 0.0f, 1.0f));
	trans = glm::rotate(trans, glm::radians(rotatey), glm::vec3(0.0f, 1.0f, 0.0f));
	trans = glm::rotate(trans, glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	trans = glm::rotate(trans, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	gluQuadricDrawStyle(planet, GLU_LINE);

	glBindBuffer(GL_ARRAY_BUFFER, sphere.vbo[0]);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, sphere.vbo[1]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, sphere.vbo[2]);
	glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	for (int i = 0; i < 200; ++i) {
		glm::mat4 rail1 = trans;
		rail1 = glm::translate(rail1, glm::vec3(rail[i][0], rail[i][1], rail[i][2]));
		glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(rail1));
		gluSphere(planet, 0.005, 5, 5);
		glm::mat4 rail2 = trans;

		glm::mat4 sub_rail1 = trans;
		sub_rail1 = glm::translate(sub_rail1, glm::vec3(subrail[i][0], subrail[i][1], subrail[i][2]));
		sub_rail1 = glm::translate(sub_rail1, glm::vec3(rail[planet_point[0]][0], rail[planet_point[0]][1], rail[planet_point[0]][2]));
		glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(sub_rail1));
		gluSphere(planet, 0.005, 5, 5);

		glm::mat4 light_rail1 = trans;
		light_rail1 = glm::translate(light_rail1, glm::vec3(light_rail[i][0], light_rail[i][1], light_rail[i][2]));
		glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(light_rail1));
		gluSphere(planet, 0.005, 5, 5);
	}


	gluQuadricDrawStyle(planet, GLU_FILL);
	trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(trans));
	glDrawArrays(GL_TRIANGLES, 0, sphere.vertex_count);

	glm::mat4 planet1 = trans;
	planet1 = glm::translate(planet1, glm::vec3(rail[planet_point[0]][0], rail[planet_point[0]][1], rail[planet_point[0]][2]));
	planet1 = glm::translate(planet1, glm::vec3(rail[planet_point[0]][0], rail[planet_point[0]][1], rail[planet_point[0]][2]));
	planet1 = glm::scale(planet1, glm::vec3(0.5f, 0.5f, 0.5f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(planet1));
	glDrawArrays(GL_TRIANGLES, 0, sphere.vertex_count);

	glm::mat4 sub_planet1 = planet1;
	sub_planet1 = glm::translate(sub_planet1, glm::vec3(subrail[sub_planet_point[0]][0], subrail[sub_planet_point[0]][1], subrail[sub_planet_point[0]][2]));
	sub_planet1 = glm::translate(sub_planet1, glm::vec3(subrail[sub_planet_point[0]][0], subrail[sub_planet_point[0]][1], subrail[sub_planet_point[0]][2]));
	sub_planet1 = glm::translate(sub_planet1, glm::vec3(subrail[sub_planet_point[0]][0], subrail[sub_planet_point[0]][1], subrail[sub_planet_point[0]][2]));
	sub_planet1 = glm::translate(sub_planet1, glm::vec3(subrail[sub_planet_point[0]][0], subrail[sub_planet_point[0]][1], subrail[sub_planet_point[0]][2]));
	sub_planet1 = glm::scale(sub_planet1, glm::vec3(0.5f, 0.5f, 0.5f));
	glUniformMatrix4fv(shape_location, 1, GL_FALSE, glm::value_ptr(sub_planet1));
	glDrawArrays(GL_TRIANGLES, 0, sphere.vertex_count);




	//glBindBuffer(GL_ARRAY_BUFFER, railvbo);
	//glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	//glDrawArrays(GL_LINE_STRIP, 0, 100);

	glm::mat4 light_box = glm::mat4(1.0f);
	light_box = glm::rotate(light_box, glm::radians(light_rotate_y), glm::vec3(0.0f, 1.0f, 0.0f));
	light_box = glm::translate(light_box, glm::vec3(light_box_pos.x, light_box_pos.y, light_box_pos.z));
	light_box = glm::scale(light_box, glm::vec3(0.3, 0.3, 0.3));
	light_box = glm::translate(light_box, glm::vec3(-0.5f, -0.5f, -0.5f));
	unsigned int cameras_location = glGetUniformLocation(shaderProgramID, "transform");
	glUniformMatrix4fv(cameras_location, 1, GL_FALSE, glm::value_ptr(light_box));
	glBindBuffer(GL_ARRAY_BUFFER, cube.vbo[0]);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, cube.vbo[1]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, cube.vbo[2]);
	glVertexAttribPointer(NormalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glDrawArrays(GL_TRIANGLES, 0, cube.vertex_count);


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
	glBindVertexArray(vao);

	cube.load_Obj("cube.obj");
	sphere.load_Obj("sphere.obj");

	planet = gluNewQuadric();
	gluQuadricNormals(planet, GLU_SMOOTH);
	gluQuadricOrientation(planet, GLU_OUTSIDE);
	gluQuadricTexture(planet, GL_FALSE);
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {

	case 'c':
		light_color = { 0.f,0.f,0.f };
		if (light_count == 0) {
			light_color.x = 1.f;
		}
		else if (light_count == 1) {
			light_color.y = 1.f;
		}
		else if (light_count == 2) {
			light_color.z = 1.f;
		}
		++light_count;
		if (light_count > 2)
			light_count = 0;
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
	case 'r':
		if (!rotate_light_on)
		{
			rotate_light_on = true;
			glutTimerFunc(50, rotate_light, 0);
		}
		else
			rotate_light_on = false;
		break;
	case 'q':
	case 'Q':
		exit(0);
		break;
	}
	glutPostRedisplay();
}
void rotate_y(int value) {
	++rotatey;
	if (rotatey_on)
		glutTimerFunc(10, rotate_y, 0);
	glutPostRedisplay();

}
void rotate_z(int value) {
	++rotatez;
	if (rotatez_on)
		glutTimerFunc(10, rotate_z, 0);
	glutPostRedisplay();

}
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