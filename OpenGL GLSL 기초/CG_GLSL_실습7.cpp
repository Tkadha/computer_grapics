#define  _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <stdlib.h>
#include <random>

#define Width 1000
#define Height 600
void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char* file);
void Mouse(int, int, int, int);
GLvoid Keyboard(unsigned char key, int x, int y);

GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID; 
GLuint vao, vbo[2];
GLfloat point[10][12];
GLfloat RGB[10][12];
int shape_mode;
int shape_count;
int shape_type[10];

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{

	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(Width, Height);
	glutCreateWindow("Example1");
	glewExperimental = GL_TRUE;
	glewInit();
	make_shaderProgram();

	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbo);
	glBindVertexArray(vao);
	//--- 세이더 읽어와서 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMainLoop();
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	GLfloat rColor, gColor, bColor;
	rColor = gColor = bColor = 0.3;
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader의 'layout (location = 0)' 부분
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	glEnableVertexAttribArray(PosLocation); 
	glEnableVertexAttribArray(ColorLocation);
	for (int i = 0; i < shape_count; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 12 * sizeof(GLfloat)));
		glEnableVertexAttribArray(PosLocation);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 12 * sizeof(GLfloat)));
		glEnableVertexAttribArray(ColorLocation);
		switch (shape_type[i]) {
		case 0:	// 점
			glPointSize(10.0);
			glDrawArrays(GL_POINTS, 0, 1); //--- 렌더링하기: 0번 인덱스에서 1개의 버텍스를 사용하여 점 그리기
			break;
		case 1:	// 선
			glDrawArrays(GL_LINES, 0, 2);
			break;
		case 2:	// 삼각형
			glDrawArrays(GL_TRIANGLES, 0, 3);
			break;
		case 3:	// 사각형 (삼각형2개)
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			break;
		}
	}
	glDisableVertexAttribArray(PosLocation); // Disable 필수!
	glDisableVertexAttribArray(ColorLocation);

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
	if(!result)
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
void Mouse(int button, int state, int x, int y)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0, 1);
	GLfloat halfx = Width / 2;
	GLfloat halfy = Height / 2;
	GLclampf mousex = (x - halfx) / halfx;
	GLclampf mousey = 1 - (y / halfy);
	if (shape_count < 10) {
		if (state == GLUT_DOWN) {
			if (button == GLUT_LEFT_BUTTON) {
				shape_type[shape_count] = shape_mode;
				if (shape_mode == 0) {
					point[shape_count][0] = mousex;
					point[shape_count][1] = mousey;
					point[shape_count][2] = 0.0f;
					for (int i = 0; i < 3; ++i) {
						RGB[shape_count][i] = dist(gen);
					}
					++shape_count;
				}
				else if (shape_mode == 1) {
					point[shape_count][0] = mousex - 0.05f;
					point[shape_count][1] = mousey - 0.05f;
					point[shape_count][2] = 0.0f;
					point[shape_count][3] = mousex + 0.05f;
					point[shape_count][4] = mousey + 0.05f;
					point[shape_count][5] = 0.0f;
					for (int i = 0; i < 3; ++i) {
						RGB[shape_count][i] = dist(gen);
					}
					for (int i = 0; i < 3; ++i) {
						RGB[shape_count][i+3] = RGB[shape_count][i];
					}
					++shape_count;
				}
				else if (shape_mode == 2) {
					point[shape_count][0] = mousex-0.05f;
					point[shape_count][1] = mousey-0.05f;
					point[shape_count][2] = 0.0f;
					point[shape_count][3] = mousex + 0.05f;
					point[shape_count][4] = mousey - 0.05f;
					point[shape_count][5] = 0.0f;
					point[shape_count][6] = mousex;
					point[shape_count][7] = mousey + 0.05f;
					point[shape_count][8] = 0.0f;
					for (int i = 0; i < 3; ++i) {
						RGB[shape_count][i] = dist(gen);
					}
					for (int i = 0; i < 3; ++i) {
						RGB[shape_count][i + 6] = RGB[shape_count][i + 3] = RGB[shape_count][i];
					}
					++shape_count;
				}
				else if (shape_mode == 3) {

					point[shape_count][0] = mousex - 0.05f;
					point[shape_count][1] = mousey - 0.05f;
					point[shape_count][2] = 0.0f;
					point[shape_count][3] = mousex - 0.05f;
					point[shape_count][4] = mousey + 0.05f;
					point[shape_count][5] = 0.0f;
					point[shape_count][6] = mousex + 0.05f;
					point[shape_count][7] = mousey - 0.05f;
					point[shape_count][8] = 0.0f;
					point[shape_count][9] = mousex + 0.05f;
					point[shape_count][10] = mousey + 0.05f;
					point[shape_count][11] = 0.0f;
					for (int i = 0; i < 3; ++i) {
						RGB[shape_count][i] = dist(gen);
					}
					for (int i = 0; i < 3; ++i) {
						RGB[shape_count][i + 9] = RGB[shape_count][i + 6] = RGB[shape_count][i + 3] = RGB[shape_count][i];
					}
					++shape_count;
				}
				glBindVertexArray(vao);
				glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
				glBufferData(GL_ARRAY_BUFFER, shape_count * 12 * sizeof(GLfloat), point, GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
				glBufferData(GL_ARRAY_BUFFER, shape_count * 12 * sizeof(GLfloat), RGB, GL_STATIC_DRAW);
			}
		}
	}
	glutPostRedisplay();
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'p':
		shape_mode = 0;
		break;
	case 'l':
		shape_mode = 1;
		break;
	case 't':
		shape_mode = 2;
		break;
	case 'r':
		shape_mode = 3;
		break;
	case 'w':
		for (int i = 0; i < 10; ++i) {
			for (int j = 1; j < 12; j+=3) {
				point[i][j] += 0.05f;
			}
		}
		break;
	case 's':
		for (int i = 0; i < 10; ++i) {
			for (int j = 1; j < 12; j += 3) {
				point[i][j] -= 0.05f;
			}
		}
		break;
	case 'a':
		for (int i = 0; i < 10; ++i) {
			for (int j = 0; j < 12; j += 3) {
				point[i][j] -= 0.05f;
			}
		}
		break;
	case 'd':
		for (int i = 0; i < 10; ++i) {
			for (int j = 0; j < 12; j += 3) {
				point[i][j] += 0.05f;
			}
		}
		break;
	case 'c':
		shape_count = 0;
		break;
	}
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, shape_count * 12 * sizeof(GLfloat), point, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, shape_count * 12 * sizeof(GLfloat), RGB, GL_STATIC_DRAW);
	glutPostRedisplay();
}