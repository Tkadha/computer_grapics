#define  _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <stdlib.h>
#include <random>
//-----------------------------------
// 마우스 클릭한 좌표를 기점으로 마우스의 x값 큰 x값을 가진 점들과 연결되있는 점을 찾는다.
// 비율로 계산하여 각각 점의 x값에서 마우스의 x값을 뺀 절댓값이 a : b 라면 점 하나에서 a: b 비율로 y좌표를 넣고
// 마우스의 y값과 같으면 count 를 1 증가 시킨다.
// count가 짝수면 사각형 외부, 홀수 면 사각형 내부이다
//
//
#define Width 800
#define Height 600
void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char* file);
void Mouse(int, int, int, int);
void move(int, int);
GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID;
GLuint vao, vbo[2];
GLfloat point[5][3] = {
	{ -0.5f,  0.5f, 0.0f },
	{  0.5f,  0.5f, 0.0f },
	{  0.5f, -0.5f, 0.0f },
	{ -0.5f, -0.5f, 0.0f },
	{ -0.5f,  0.5f, 0.0f },
};
GLfloat RGB[4][3];
bool mousepoint[4];
bool rectmove;
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
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(RGB), RGB, GL_STATIC_DRAW);
	//--- 세이더 읽어와서 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(move);
	glutMainLoop();
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	GLfloat rColor, gColor, bColor;
	rColor = gColor = bColor = 1.0f;
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);
	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader의 'layout (location = 0)' 부분
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1

	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(PosLocation);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(ColorLocation);
	glLineWidth(3);
	glDrawArrays(GL_LINE_STRIP, 0, 5);

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
GLfloat px[5], py[5];
void Mouse(int button, int state, int x, int y)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0, 1);
	GLfloat halfx = Width / 2;
	GLfloat halfy = Height / 2;
	GLclampf mousex = (x - halfx) / halfx;
	GLclampf mousey = 1 - (y / halfy);
	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {
			if ((point[0][0] <= mousex) && (point[1][0] >= mousex)) {
				if ((point[0][1] >= mousey) && (point[2][1] <= mousey)) {
					rectmove = true;
					for (int i = 0; i < 5; ++i) {
						px[i] = abs(mousex - point[i][0]);
						py[i] = abs(mousey - point[i][1]);
					}
				}
			}
			for (int i = 0; i < 4; ++i) {
				if ((point[i][0] - 0.05f <= mousex) && (point[i][0] + 0.05f >= mousex)) {
					if ((point[i][1] - 0.05f <= mousey) && (point[i][1] + 0.05f >= mousey)) {
						mousepoint[i] = true;
						rectmove = false;
						break;
					}
				}
			}
		}
	}
	else if (state == GLUT_UP) {
		if (button == GLUT_LEFT_BUTTON) {
			rectmove = false;
			for (int i = 0; i < 4; ++i) {
				if ((point[i][0] - 0.01f <= mousex) && (point[i][0] + 0.01f >= mousex)) {
					if ((point[i][1] - 0.01f <= mousey) && (point[i][1] + 0.01f >= mousey)) {
						mousepoint[i] = false;
					}
				}
			}
		}
	}
	glutPostRedisplay();
}
void move(int x, int y) {
	GLfloat halfx = Width / 2;
	GLfloat halfy = Height / 2;
	GLclampf mousex = (x - halfx) / halfx;
	GLclampf mousey = 1 - (y / halfy);
	if (rectmove) {
		point[0][0] = mousex - px[0];
		point[0][1] = mousey + py[0];

		point[4][0] = mousex - px[4];
		point[4][1] = mousey + py[4];

		point[1][0] = mousex + px[1];
		point[1][1] = mousey + py[1];

		point[2][0] = mousex + px[2];
		point[2][1] = mousey - py[2];

		point[3][0] = mousex - px[3];
		point[3][1] = mousey - py[3];
	}
	else {
		for (int i = 0; i < 4; ++i) {
			if (mousepoint[i]) {
				point[i][0] = mousex;
				point[i][1] = mousey;
				if (i == 0) {
					point[4][0] = mousex;
					point[4][1] = mousey;
				}
			}
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_DYNAMIC_DRAW);
	glutPostRedisplay();
}