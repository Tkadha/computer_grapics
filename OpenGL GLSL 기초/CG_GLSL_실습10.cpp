#define  _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <stdlib.h>
#include <random>

#define Width 1200
#define Height 800
#define PI 3.1415926

void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char* file);
void Mouse(int, int, int, int);
GLvoid Keyboard(unsigned char key, int x, int y);
void circle_spiral(int);

GLfloat rColor, gColor, bColor;
GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID;
GLuint vao, vbo;
GLfloat spiral[5][2250];
int count;
int shape_mode;
int point_count[5];
GLfloat angle[5];
GLfloat radius[5];
bool draw;
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
	rColor = gColor = bColor = 1.0;
	for (int i = 0; i < 5; ++i)
	{
		angle[i] = 3.6f;
		radius[i] = 0.00005f;
	}
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(spiral), spiral, GL_STATIC_DRAW);
	//--- 세이더 읽어와서 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMainLoop();
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader의 'layout (location = 0)' 부분
	glEnableVertexAttribArray(PosLocation);
	if (draw) {
		for (int i = 0; i < count; ++i)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(i * 2250 * sizeof(GLfloat)));
			glEnableVertexAttribArray(PosLocation);
			if (shape_mode == 0) {
				glPointSize(3);
				glDrawArrays(GL_POINTS, 0, point_count[i]);
			}
			else {
				glLineWidth(3);
				glDrawArrays(GL_LINE_STRIP, 0, point_count[i]);
			}
			glDisableVertexAttribArray(PosLocation); // Disable 필수!
		}
	}

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
	std::uniform_real_distribution<> dist(0, 1);
	GLfloat halfx = Width / 2;
	GLfloat halfy = Height / 2;
	GLclampf mousex = (x - halfx) / halfx;
	GLclampf mousey = 1 - (y / halfy);
	std::uniform_real_distribution<> point(-0.8, 0.8);
	draw = true;
	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {
			for (int i = 0; i < 5; ++i)
			{
				point_count[i] = 0;
				angle[i] = 3.6f;
				radius[i] = 0.00005f;
			}
			rColor = dist(gen);
			gColor = dist(gen);
			bColor = dist(gen);
			spiral[0][0] = mousex;
			spiral[0][1] = mousey;
			glutTimerFunc(1, circle_spiral, 0);
			for (int i = 1; i < count; ++i) {
				spiral[i][0] = point(gen);
				spiral[i][1] = point(gen);
				glutTimerFunc(1, circle_spiral, i);
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
	case '1':
		count = 1;
		break;
	case '2':
		count = 2;
		break;
	case '3':
		count = 3;
		break;
	case '4':
		count = 4;
		break;
	case '5':
		count = 5;
		break;
	}
	glutPostRedisplay();
}
void circle_spiral(int value)
{
	if (angle[value] <= 360 * 3 + 90) {
		//spiral[value][(point_count[value] + 1) * 3] = spiral[value][point_count[value] * 3] + cos(angle[value] * PI / 180) * radius[value];
		//spiral[value][(point_count[value] + 1) * 3 + 1] = spiral[value][point_count[value] * 3 + 1] + sin(angle[value] * PI / 180) * radius[value];
		spiral[value][(point_count[value] + 1) * 3] = spiral[value][point_count[value] * 3] + cos(angle[value] * PI / 180) * 0.005f;
		spiral[value][(point_count[value] + 1) * 3 + 1] = spiral[value][point_count[value] * 3 + 1] + sin(angle[value] * PI / 180) * 0.005f;
		radius[value] += 0.00005f;
	}
	else
	{
		//spiral[value][(point_count[value] + 1) * 3] = spiral[value][point_count[value] * 3] + sin((angle[value]-90) * PI / 180) * radius[value];
		//spiral[value][(point_count[value] + 1) * 3 + 1] = spiral[value][point_count[value] * 3 + 1] + cos((angle[value] - 90) * PI / 180) * radius[value];
		spiral[value][(point_count[value] + 1) * 3] = spiral[value][point_count[value] * 3] + sin((angle[value] - 90) * PI / 180) * 0.005f;
		spiral[value][(point_count[value] + 1) * 3 + 1] = spiral[value][point_count[value] * 3 + 1] + cos((angle[value] - 90) * PI / 180) * 0.005f;
		radius[value] -= 0.00005f;
	}
	++point_count[value];
	angle[value] += 3.6f;
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(spiral), spiral, GL_STATIC_DRAW);
	glutPostRedisplay();
	if (angle[value] <= 360*6+90) {
		glutTimerFunc(1, circle_spiral, value);
	}
}