#define  _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <stdlib.h>
#include <random>
#include <vector>

#define Width 1000
#define Height 700

struct Point {
public:
	float x, y, z;
};
struct RGB {
public:
	float r, g, b;
};

class Shape {
public:
	int way;
	float add_x, add_y;
	bool cut;

	void addvertex(Point point) {
		vertice.push_back(point);
	}
	void addcolor(RGB rgb) {
		color.push_back(rgb);
	}

	void add_xy() {
		for (int i = 0; i < vertice.size(); ++i) {
			vertice[i].x += add_x;
			vertice[i].y += add_y;
		}
	}

	const std::vector<Point>& getvertices() const {
		return vertice;
	}
	const std::vector<RGB>& getcolor() const {
		return color;
	}

private:
	std::vector<Point> vertice;
	std::vector<RGB> color;
};


void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
char* filetobuf(const char* file);
void initbuffer();
void Mouse(int, int, int, int);
void MouseMotion(int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
void move_box(int value);
void add_shape(int);
void move_shape(int);
void check_slice();

int ccw(Point a, Point b, Point p);
bool check_line_on(Point a, Point q, Point b);
bool checking_cross(Point p1, Point q1, Point p2, Point q2);



GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID;
GLuint vao, boxvbo[2];
GLfloat box[4][3] = {
	{-0.2f,-0.8f,0.f},
	{-0.2f,-0.9f,0.f},
	{0.2f,-0.9f,0.f},
	{0.2f,-0.8f,0.f}
};
GLfloat box_RGB[4][3];
float box_move_way;

GLuint linevbo[2];
float cutting_line[2][3];
float cutting_line_rgb[2][3];
bool mouse_down;

GLuint polygonvbo[2];

float adding_x, adding_y;
int add_speed;

std::vector<Shape> polygons;

bool fill_line;

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
	for (int i = 0; i < 4; ++i) {
		box_RGB[i][0] = 0.0f;
		box_RGB[i][1] = 0.0f;
		box_RGB[i][2] = 1.0f;
	}
	{
		box_move_way = 0.01f;
	}


	{
		adding_x = 0.01f;
		adding_y = 0.00125f;
		add_speed = 1000;
	}



	{
		std::cout << "m/M: 그리기 모드 변경 LINE/FILL" << std::endl;
		std::cout << "-/+: 도형 속도 감소/증가" << std::endl;
		std::cout << "q/Q: 프로그램 종료" << std::endl;


	}
	initbuffer();
	//--- 세이더 읽어와서 세이더 프로그램 만들기
	glutDisplayFunc(drawScene); //--- 출력 콜백 함수
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMotionFunc(MouseMotion);
	glutMouseFunc(Mouse);
	glutTimerFunc(10, move_box, 0);
	glutTimerFunc(add_speed, add_shape, 0);
	glutMainLoop();
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	GLfloat rColor, gColor, bColor;
	rColor = gColor = 0.5;
	bColor = 1.0;
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glBindVertexArray(vao);

	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position"); //	: 0  Shader의 'layout (location = 0)' 부분
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color"); //	: 1
	glEnableVertexAttribArray(PosLocation);
	glEnableVertexAttribArray(ColorLocation);
	glBindBuffer(GL_ARRAY_BUFFER, boxvbo[0]);
	glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, boxvbo[1]);
	glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);


	for (auto& poly : polygons) {
		std::vector<float> vertex_pos;
		std::vector<float> rgb_color;
		const std::vector<Point>& vertice = poly.getvertices();
		const std::vector<RGB>& color = poly.getcolor();
		for (auto& vertex : vertice) {
			vertex_pos.push_back(vertex.x);
			vertex_pos.push_back(vertex.y);
			vertex_pos.push_back(vertex.z);
		}
		for (auto& rgb : color) {
			rgb_color.push_back(rgb.r);
			rgb_color.push_back(rgb.g);
			rgb_color.push_back(rgb.b);
		}
		glBindBuffer(GL_ARRAY_BUFFER, polygonvbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_pos.size(), vertex_pos.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, polygonvbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * rgb_color.size(), rgb_color.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		if (fill_line)
		{
			glLineWidth(2);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLE_FAN, 0, vertex_pos.size() / 3);
	}
	if (mouse_down) {
		glBindBuffer(GL_ARRAY_BUFFER, linevbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cutting_line), cutting_line, GL_STATIC_DRAW);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, linevbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cutting_line_rgb), cutting_line_rgb, GL_STATIC_DRAW);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glLineWidth(2);
		glDrawArrays(GL_LINES, 0, 2);
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
	vertexSource = filetobuf("vertex_project.glsl");
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
	fragmentSource = filetobuf("fragment_project.glsl");
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
void initbuffer() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(2, boxvbo);
	glGenBuffers(2, polygonvbo);
	glGenBuffers(2, linevbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, boxvbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, boxvbo[1]);
	glBufferData(GL_ARRAY_BUFFER,sizeof(box_RGB), box_RGB, GL_STATIC_DRAW);

}
void Mouse(int button, int state, int x, int y)
{
	GLfloat halfx = Width / 2;
	GLfloat halfy = Height / 2;
	GLclampf mousex = (x - halfx) / halfx;
	GLclampf mousey = 1 - (y / halfy);
	if (state == GLUT_DOWN)
	{
		if (button == GLUT_LEFT_BUTTON || button == GLUT_RIGHT_BUTTON) {
			mouse_down = true;
			cutting_line[0][0] = mousex;
			cutting_line[0][1] = mousey;
			cutting_line[0][2] = 0.f;
			cutting_line[1][0] = mousex;
			cutting_line[1][1] = mousey;
			cutting_line[1][2] = 0.f;
		}
	}
	else if (state == GLUT_UP) {
		if (button == GLUT_LEFT_BUTTON || button == GLUT_RIGHT_BUTTON) {
			mouse_down = false;

			// 여기서 도형 잘리는지 체크
			std::cout << "cutting start" << std::endl;
			check_slice();
			//
			cutting_line[0][0] = mousex;
			cutting_line[0][1] = mousey;
			cutting_line[0][2] = 0.f;
			cutting_line[1][0] = mousex;
			cutting_line[1][1] = mousey;
			cutting_line[1][2] = 0.f;
		}
	}
	glutPostRedisplay();
}
void MouseMotion(int x, int y) {
	GLfloat halfx = Width / 2;
	GLfloat halfy = Height / 2;
	GLclampf mousex = (x - halfx) / halfx;
	GLclampf mousey = 1 - (y / halfy);
	if (mouse_down) {
		cutting_line[1][0] = mousex;
		cutting_line[1][1] = mousey;
		cutting_line[1][2] = 0.f;
	}
	glutPostRedisplay();
}
GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'm':
	case 'M':
		if (fill_line)
			fill_line = false;
		else
			fill_line = true;
		break;
	case '-':
		if (adding_x > 0.005f) {
			adding_x -= 0.005f;
		}
		if (adding_y > 0.0005f)
		{
			adding_y -= 0.0005f;
		}		
		break;
	case '+':
		adding_x += 0.005f;
		adding_y += 0.0005f;
		for (auto& polygon : polygons) {
			if(polygon.add_x>0)
				polygon.add_x += 0.005f;
			else {
				polygon.add_x -= 0.005f;
			}
		}
		break;
	case 'q':
	case 'Q':
		exit(0);
		break;
	}
	glutPostRedisplay();
}
void move_box(int value) {
	for (int i = 0; i < 4; ++i) {
		box[i][0] += box_move_way;
	}
	if ((box[0][0] <= -1.f)||(box[3][0] >= 1.f)) {
		box_move_way *= -1;
	}
	glBindBuffer(GL_ARRAY_BUFFER, boxvbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW);
	glutTimerFunc(10, move_box, value);
	glutPostRedisplay();

}

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> color(0, 1);
std::uniform_real_distribution<float> y_pos(-0.3, 0.8);
std::uniform_int_distribution<int> vertex_count(3, 4);
std::uniform_int_distribution<int> way_cos(0, 1);
void add_shape(int value) {
	Shape poly;

	Point point;
	RGB rgb;
	int count= vertex_count(gen);
	poly.way = way_cos(gen);
	poly.cut = false;
	if (count == 3) {
		if (poly.way == 0) {		// left -> right
			point = { -1.1f,y_pos(gen),0.f };
			poly.addvertex(point);
			point = { point.x + 0.2f,point.y,0.f };
			poly.addvertex(point);
			point = { point.x - 0.2f,point.y + 0.2f,0.f };
			poly.addvertex(point);
			if (point.y - 0.2f < 0.1f) {
				poly.add_x = adding_x;
				poly.add_y = adding_y;
			}
			else {
				poly.add_x = adding_x;
				poly.add_y = -adding_y;
			}
		}
		else {						// right -> left
			point = { 1.1f,y_pos(gen),0.f };
			poly.addvertex(point);
			point = { point.x + 0.2f,point.y,0.f };
			poly.addvertex(point);
			point = { point.x - 0.2f,point.y + 0.2f,0.f };
			poly.addvertex(point);
			if (point.y - 0.2f < 0.1f) {
				poly.add_x = -adding_x;
				poly.add_y = adding_y;
			}
			else {
				poly.add_x = -adding_x;
				poly.add_y = -adding_y;
			}
		}
		for (int i = 0; i < 3; ++i)
		{
			rgb.r = color(gen);
			rgb.g = color(gen);
			rgb.b = color(gen);
			poly.addcolor(rgb);
		}
	}
	else if (count == 4) {
		if (poly.way == 0) {		// left -> right
			point = { -1.1f,y_pos(gen),0.f };
			poly.addvertex(point);
			point = { point.x + 0.2f,point.y,0.f };
			poly.addvertex(point);
			point = { point.x,point.y + 0.2f,0.f };
			poly.addvertex(point);
			point = { point.x - 0.2f,point.y,0.f };
			poly.addvertex(point);
			if (point.y - 0.2f < 0.1f) {
				poly.add_x = adding_x;
				poly.add_y = adding_y;
			}
			else {
				poly.add_x = adding_x;
				poly.add_y = -adding_y;
			}
		}
		else {						// right -> left
			point = { 1.1f,y_pos(gen),0.f };
			poly.addvertex(point);
			point = { point.x + 0.2f,point.y,0.f };
			poly.addvertex(point);
			point = { point.x,point.y + 0.2f,0.f };
			poly.addvertex(point);
			point = { point.x - 0.2f,point.y,0.f };
			poly.addvertex(point);
			if (point.y - 0.2f < 0.1f) {
				poly.add_x = -adding_x;
				poly.add_y = adding_y;
			}
			else {
				poly.add_x = -adding_x;
				poly.add_y = -adding_y;
			}
		}
		for (int i = 0; i < 4; ++i)
		{
			rgb.r = color(gen);
			rgb.g = color(gen);
			rgb.b = color(gen);
			poly.addcolor(rgb);
		}
	}
	polygons.push_back(poly);
	glutTimerFunc(10, move_shape, polygons.size() - 1);
	glutTimerFunc(add_speed, add_shape, value);
	glutPostRedisplay();
}
void move_shape(int value) {
	if (value >= polygons.size())
		return;
	Shape& poly = polygons[value];
	poly.add_xy();

	const std::vector<Point>& vertice = poly.getvertices();

	if (vertice[2].y && vertice[1].y && vertice[0].y < -1.0f) {
		polygons.erase(polygons.begin() + value);
	}
	if (poly.way == 0) {
		if (vertice[0].x > 1.0f) {
			polygons.erase(polygons.begin() + value);
			//std::cout << polygons.size() << std::endl;
		}
	}
	else if (poly.way == 1) {
		if (vertice[1].x < -1.0f) {
			polygons.erase(polygons.begin() + value);
			//std::cout << polygons.size() << std::endl;
		}
	}
	if (poly.cut){
		polygons.erase(polygons.begin() + value);
	}
	glutTimerFunc(10, move_shape, value);
	glutPostRedisplay();
}


std::uniform_real_distribution<float> slice_x_speed(0.001, 0.003);
std::uniform_real_distribution<float> slice_y_speed(0.004, 0.008);
void check_slice() {
	Point mousepoint[2];
	Point point;
	RGB rgb;
	mousepoint[0] = { cutting_line[0][0],cutting_line[0][1],0.f };
	mousepoint[1] = { cutting_line[1][0],cutting_line[1][1],0.f };
	for (auto& polygon : polygons) {
		int crosscount = 0;
		const std::vector<Point>& vertice = polygon.getvertices();
		// 선분 교차 체크
		for (int i = 0; i < vertice.size() - 1; ++i) {
			if (checking_cross(mousepoint[0], mousepoint[1], vertice[i], vertice[i + 1])) {
				++crosscount;
			}
		}

		if (checking_cross(mousepoint[0], mousepoint[1], vertice[vertice.size() - 1], vertice[0])) {
			++crosscount;
		}
		// 교차가 2개면 가른상태
		if (crosscount == 2) {
			std::cout << vertice.size() << " cut" << std::endl;
			polygon.cut = true;
		}
		if (crosscount > 0) std::cout << crosscount << std::endl;
	}



	for (auto& polygon : polygons) {
		if (polygon.cut)
		{
			const std::vector<Point>& vertice = polygon.getvertices();
			const std::vector<RGB>& color = polygon.getcolor();
			Shape poly;
			Shape poly2;
			Point point;
			if (vertice.size() == 4) {
				point = vertice[3];
				point.x -= 0.05f;
				point.y += 0.05f;
				poly.addvertex(point);
				point = vertice[0];
				point.x -= 0.05f;
				point.y += 0.05f;
				poly.addvertex(point);
				point = vertice[2];
				point.x -= 0.05f;
				point.y += 0.05f;
				poly.addvertex(point);
				poly.add_x = -slice_x_speed(gen);
				poly.add_y = -slice_y_speed(gen);
				poly.way = polygon.way;
				poly.cut = false;
				rgb = color[3];
				poly.addcolor(rgb);
				rgb = color[0];
				poly.addcolor(rgb);
				rgb = color[2];
				poly.addcolor(rgb);

				point = vertice[1];
				point.x += 0.05f;
				poly2.addvertex(point);
				point = vertice[2];
				point.x += 0.05f;
				poly2.addvertex(point);
				point = vertice[0];
				point.x += 0.05f;
				poly2.addvertex(point);
				poly2.add_x = slice_x_speed(gen);
				poly2.add_y = -slice_y_speed(gen);
				poly2.way = polygon.way;
				poly2.cut = false;
				rgb = color[1];
				poly2.addcolor(rgb);
				rgb = color[2];
				poly2.addcolor(rgb);
				rgb = color[0];
				poly2.addcolor(rgb);
				polygons.push_back(poly);
				polygons.push_back(poly2);
				glutTimerFunc(10, move_shape, polygons.size() - 2);
				glutTimerFunc(10, move_shape, polygons.size() - 1);
			}
			else if (vertice.size() == 3) {

				point = vertice[2];
				point.x += vertice[1].x;
				point.y += vertice[1].y;
				point.z += vertice[1].z;
				point.x /= 2;
				point.y /= 2;
				point.z /= 2;
				point.x -= 0.05f;
				point.y += 0.05f;
				poly.addvertex(point);
				point = vertice[2];
				point.x -= 0.05f;
				point.y += 0.05f;
				poly.addvertex(point);
				point = vertice[0];
				point.x -= 0.05f;
				point.y += 0.05f;
				poly.addvertex(point);
				poly.add_x = -slice_x_speed(gen);
				poly.add_y = -slice_y_speed(gen);
				poly.way = polygon.way;
				poly.cut = false;
				rgb.r = (color[2].r + color[1].r) / 2;
				rgb.g = (color[2].g + color[1].g) / 2;
				rgb.b = (color[2].b + color[1].b) / 2;
				poly.addcolor(rgb);
				rgb = color[2];
				poly.addcolor(rgb);
				rgb = color[0];
				poly.addcolor(rgb);

				point = vertice[2];
				point.x += vertice[1].x;
				point.y += vertice[1].y;
				point.z += vertice[1].z;
				point.x /= 2;
				point.y /= 2;
				point.z /= 2;
				point.x += 0.05f;
				poly2.addvertex(point);
				point = vertice[0];
				point.x += 0.05f;
				poly2.addvertex(point);
				point = vertice[1];
				point.x += 0.05f;
				poly2.addvertex(point);
				poly2.add_x = slice_x_speed(gen);
				poly2.add_y = -slice_y_speed(gen);
				poly2.way = polygon.way;
				poly2.cut = false;
				rgb.r = (color[2].r + color[1].r) / 2;
				rgb.g = (color[2].g + color[1].g) / 2;
				rgb.b = (color[2].b + color[1].b) / 2;
				poly2.addcolor(rgb);
				rgb = color[0];
				poly2.addcolor(rgb);
				rgb = color[1];
				poly2.addcolor(rgb);


				polygons.push_back(poly);
				polygons.push_back(poly2);
				glutTimerFunc(10, move_shape, polygons.size() - 2);
				glutTimerFunc(10, move_shape, polygons.size() - 1);
			}
		}
	}

}

int ccw(Point a, Point b, Point p) {
	double value = (b.y - a.y) * (p.x - b.x) - (b.x - a.x) * (p.y - b.y);
	if (value == 0)return 0;
	else if (value > 0) return 1;
	else if (value < 0) return -1;
}

bool check_line_on(Point a, Point q, Point b) {
	return (q.x <= std::max(a.x, b.x) && q.x >= std::min(a.x, b.x) &&
		    q.y <= std::max(a.y, b.y) && q.y >= std::min(a.y, b.y));
}


bool checking_cross(Point p1, Point q1, Point p2, Point q2) {

	int o1 = ccw(p1, q1, p2);
	int o2 = ccw(p1, q1, q2);
	int o3 = ccw(p2, q2, p1);
	int o4 = ccw(p2, q2, q1);

	if (o1 != o2 && o3 != o4)
	{
		return true;
	}
	if (o1 == 0 && check_line_on(p1, p2, q1)) return true;
	if (o2 == 0 && check_line_on(p1, q2, q1)) return true;
	if (o3 == 0 && check_line_on(p2, p1, q2)) return true;
	if (o4 == 0 && check_line_on(p2, q1, q2)) return true;

	return false;
}