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
void diagonal(int);
void zigzag(int);
void rec_spiral(int);
void circle_spiral(int);


GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID;
GLuint vao, vbo[2];
GLuint vbo_line[2];
GLfloat triangle[4][9];
GLfloat RGB[4][9];
GLfloat line[2][6];
GLfloat line_RGB[2][6];
GLfloat diag[4][2];
GLfloat zig[4][2];
int zigzag_count;
GLfloat r[4];
GLfloat h[4];

int mode_timer;
int mode;


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
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 2; ++j)
		{
			diag[i][j] = 0.005f;
			zig[i][j] = 0.005f;
		}
	line[0][1] = 1;
	line[0][4] = -1;
	line[1][0] = 1;
	line[1][3] = -1;
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, vbo);
	glGenBuffers(2, vbo_line);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), triangle, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), RGB, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[0]);
	glBufferData(GL_ARRAY_BUFFER, 2 * 6 * sizeof(GLfloat), line, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_line[1]);
	glBufferData(GL_ARRAY_BUFFER, 2 * 6 * sizeof(GLfloat), line_RGB, GL_STATIC_DRAW);
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
	rColor = gColor = bColor = 1.0;
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader의 'layout (location = 0)' 부분
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation);
	for (int i = 0; i < 2; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_line[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(PosLocation);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_line[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(ColorLocation);
		glDrawArrays(GL_LINES, 0, 2);
	}
	for (int i = 0; i < 4; ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 9 * sizeof(GLfloat)));
		glEnableVertexAttribArray(PosLocation);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 9 * sizeof(GLfloat)));
		glEnableVertexAttribArray(ColorLocation);
		if (mode == 1) {
			glPolygonMode(GL_FRONT, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT, GL_FILL);
		}
		glDrawArrays(GL_TRIANGLES, 0, 3);
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
void Mouse(int button, int state, int x, int y)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0.05, 0.3);
	GLfloat halfx = Width / 2;
	GLfloat halfy = Height / 2;
	GLclampf mousex = (x - halfx) / halfx;
	GLclampf mousey = 1 - (y / halfy);
	GLfloat tri_size = dist(gen);
	std::uniform_real_distribution<> color(0, 1);
	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {
			if (mousex < 0) {
				if (mousey < 0) {	//3사분면
					triangle[2][0] = mousex - tri_size / 2;
					triangle[2][1] = mousey - tri_size;
					triangle[2][2] = 0.0f;
					triangle[2][3] = mousex + tri_size / 2;
					triangle[2][4] = mousey - tri_size;
					triangle[2][5] = 0.0f;
					triangle[2][6] = mousex;
					triangle[2][7] = mousey + tri_size;
					triangle[2][8] = 0.0f;
					r[2] = tri_size;
					h[2] = tri_size * 2;
					for (int i = 0; i < 3; ++i) {
						RGB[2][i + 6] = RGB[2][i + 3] = RGB[2][i] = color(gen);
					}
				}
				else if (mousey >= 0) {	//2사분면
					triangle[1][0] = mousex - tri_size / 2;
					triangle[1][1] = mousey - tri_size;
					triangle[1][2] = 0.0f;
					triangle[1][3] = mousex + tri_size / 2;
					triangle[1][4] = mousey - tri_size;
					triangle[1][5] = 0.0f;
					triangle[1][6] = mousex;
					triangle[1][7] = mousey + tri_size;
					triangle[1][8] = 0.0f;
					r[1] = tri_size;
					h[1] = tri_size * 2;
					for (int i = 0; i < 3; ++i) {
						RGB[1][i + 6] = RGB[1][i + 3] = RGB[1][i] = color(gen);
					}
				}
			}
			else if (mousex >= 0) {
				if (mousey < 0) {		//4사분면
					triangle[3][0] = mousex - tri_size / 2;
					triangle[3][1] = mousey - tri_size;
					triangle[3][2] = 0.0f;
					triangle[3][3] = mousex + tri_size / 2;
					triangle[3][4] = mousey - tri_size;
					triangle[3][5] = 0.0f;
					triangle[3][6] = mousex;
					triangle[3][7] = mousey + tri_size;
					triangle[3][8] = 0.0f;
					r[3] = tri_size;
					h[3] = tri_size * 2;
					for (int i = 0; i < 3; ++i) {
						RGB[3][i + 6] = RGB[3][i + 3] = RGB[3][i] = color(gen);
					}
				}
				else if (mousey >= 0) {	//1사분면
					triangle[0][0] = mousex - tri_size / 2;
					triangle[0][1] = mousey - tri_size;
					triangle[0][2] = 0.0f;
					triangle[0][3] = mousex + tri_size / 2;
					triangle[0][4] = mousey - tri_size;
					triangle[0][5] = 0.0f;
					triangle[0][6] = mousex;
					triangle[0][7] = mousey + tri_size;
					triangle[0][8] = 0.0f;
					r[0] = tri_size;
					h[0] = tri_size * 2;
					for (int i = 0; i < 3; ++i) {
						RGB[0][i + 6] = RGB[0][i + 3] = RGB[0][i] = color(gen);
					}
				}
			}
		}
	}
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), triangle, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), RGB, GL_STATIC_DRAW);
	glutPostRedisplay();
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'a':
		mode = 1;
		break;
	case 'b':
		mode = 0;
		break;
	case '1':
		mode_timer = 1;
		for (int i = 0; i < 4; ++i)
			glutTimerFunc(10, diagonal, i);
		break;
	case '2':
		mode_timer = 2;
		for (int i = 0; i < 4; ++i)
			glutTimerFunc(10, zigzag, i);
		break;
	case '3':
		mode_timer = 3;
		for (int i = 0; i < 4; ++i)
			glutTimerFunc(10, rec_spiral, i);
		break;
	case '4':
		mode_timer = 4;
		for (int i = 0; i < 4; ++i)
			glutTimerFunc(10, circle_spiral, i);
		break;
	}
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), triangle, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), RGB, GL_STATIC_DRAW);
	glutPostRedisplay();
}
void diagonal(int value)
{
	triangle[value][0] += diag[value][0];
	triangle[value][3] += diag[value][0];
	triangle[value][6] += diag[value][0];
	triangle[value][1] += diag[value][1];
	triangle[value][4] += diag[value][1];
	triangle[value][7] += diag[value][1];
	if (diag[value][1] > 0) {
		if (triangle[value][1] >= 1) {
			triangle[value][0] = triangle[value][3];
			triangle[value][1] = triangle[value][4] - r[value];
			triangle[value][6] = triangle[value][3] - h[value] / 2;
			triangle[value][7] = triangle[value][4] - r[value] / 2;
			diag[value][1] *= -1;
		}
		else if (triangle[value][4] >= 1)
		{
			triangle[value][0] = triangle[value][3] + r[value];
			triangle[value][1] = triangle[value][4];
			triangle[value][6] = triangle[value][3] + r[value] / 2;
			triangle[value][7] = triangle[value][4] - h[value];
			diag[value][1] *= -1;
		}
		else if (triangle[value][7] >= 1) {
			triangle[value][0] = triangle[value][3];
			triangle[value][1] = triangle[value][4] - r[value];
			triangle[value][6] = triangle[value][3] - h[value] / 2;
			triangle[value][7] = triangle[value][4] - r[value] / 2;
			diag[value][1] *= -1;
		}
	}
	if (diag[value][1] < 0) {
		if ((triangle[value][1] <= -1)||(triangle[value][4] <= -1)) {
			triangle[value][3] = triangle[value][0];
			triangle[value][4] = triangle[value][1] + r[value];
			triangle[value][6] = triangle[value][0] - h[value] / 2;
			triangle[value][7] = triangle[value][1] + r[value] / 2;
			diag[value][1] *= -1;
		}
		else if (triangle[value][7] <= -1)
		{
			triangle[value][3] = triangle[value][0];
			triangle[value][4] = triangle[value][1] + r[value];
			triangle[value][6] = triangle[value][0] - h[value] / 2;
			triangle[value][7] = triangle[value][1] + r[value] / 2;
			diag[value][1] *= -1;
		}
	}
	if (diag[value][0] > 0) {
		if ((triangle[value][0] >= 1) || (triangle[value][3] >= 1) || (triangle[value][6] >= 1)) {
			triangle[value][0] = triangle[value][3];
			triangle[value][1] = triangle[value][4];
			triangle[value][3] = triangle[value][0] - r[value];
			triangle[value][4] = triangle[value][1];
			triangle[value][6] = triangle[value][0] - r[value] / 2;
			triangle[value][7] = triangle[value][1] - h[value];
			diag[value][0] *= -1;
		}
	}
	if (diag[value][0] < 0) {
		if ((triangle[value][0] <= -1) || (triangle[value][3] <= -1) || (triangle[value][6] <= -1)) {
			triangle[value][0] = triangle[value][3];
			triangle[value][1] = triangle[value][4];
			triangle[value][3] = triangle[value][0] + r[value];
			triangle[value][4] = triangle[value][1];
			triangle[value][6] = triangle[value][0] + r[value] / 2;
			triangle[value][7] = triangle[value][1] + h[value];
			diag[value][0] *= -1;
		}
	}
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), triangle, GL_DYNAMIC_DRAW);
	glutPostRedisplay();
	if (mode_timer == 1) {
		glutTimerFunc(10, diagonal, value);
	}
}
void zigzag(int value) 
{
	if (zigzag_count == 0) {
		triangle[value][0] += zig[value][0];
		triangle[value][3] += zig[value][0];
		triangle[value][6] += zig[value][0];
		if (zig[value][1] < 0) {
			if (triangle[value][3] < -1) {
				++zigzag_count;
				triangle[value][3] = triangle[value][0];
				triangle[value][4] = triangle[value][1] - r[value];
				triangle[value][6] = triangle[value][0] + h[value];
				triangle[value][7] = triangle[value][1] - r[value] / 2;
				if (triangle[value][4] < -1) {
					zig[value][1] *= -1;
				}
			}
			else if (triangle[value][3] > 1) {
				++zigzag_count;
				triangle[value][0] = triangle[value][3];
				triangle[value][1] = triangle[value][4] + r[value];
				triangle[value][6] = triangle[value][3] - h[value];
				triangle[value][7] = triangle[value][4] + r[value] / 2;
				if (triangle[value][1] < -1) {
					zig[value][1] *= -1;
				}
			}
			else if (triangle[value][0] < -1) {
				++zigzag_count;
				triangle[value][0] = triangle[value][3];
				triangle[value][1] = triangle[value][4] + r[value];
				triangle[value][6] = triangle[value][3] + h[value];
				triangle[value][7] = triangle[value][4] + r[value] / 2;
				if (triangle[value][1] < -1) {
					zig[value][1] *= -1;
				}
			}
			else if (triangle[value][0] > 1) {
				++zigzag_count;
				triangle[value][3] = triangle[value][0];
				triangle[value][4] = triangle[value][1] + r[value];
				triangle[value][6] = triangle[value][0] - h[value];
				triangle[value][7] = triangle[value][1] + r[value] / 2;
				if (triangle[value][4] < -1) {
					zig[value][1] *= -1;
				}
			}
		}
		else {
			if (triangle[value][3] < -1) {
				++zigzag_count;
				triangle[value][3] = triangle[value][0];
				triangle[value][4] = triangle[value][1] - r[value];
				triangle[value][6] = triangle[value][0] + h[value];
				triangle[value][7] = triangle[value][0] - r[value] / 2;
				if (triangle[value][4] > 1) {
					zig[value][1] *= -1;
				}
			}
			else if (triangle[value][3] > 1) {
				++zigzag_count;
				triangle[value][0] = triangle[value][3];
				triangle[value][1] = triangle[value][4] + r[value];
				triangle[value][6] = triangle[value][3] - h[value];
				triangle[value][7] = triangle[value][4] + r[value] / 2;
				if (triangle[value][1] > 1) {
					zig[value][1] *= -1;
				}
			}
		}
	}
	else {

	}

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), triangle, GL_DYNAMIC_DRAW);
	glutPostRedisplay();
	if (mode_timer == 2) {
		glutTimerFunc(10, zigzag, value);
	}
}
void rec_spiral(int value) 
{



	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), triangle, GL_DYNAMIC_DRAW);
	glutPostRedisplay();
	if (mode_timer == 3) {
		glutTimerFunc(10, rec_spiral, value);
	}
}
void circle_spiral(int value)
{



	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 9 * sizeof(GLfloat), triangle, GL_DYNAMIC_DRAW);
	glutPostRedisplay();
	if (mode_timer == 4) {
		glutTimerFunc(10, circle_spiral, value);
	}
}