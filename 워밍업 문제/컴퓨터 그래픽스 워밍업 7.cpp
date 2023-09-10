#include <iostream>
#include <cstdlib>
#include <ctime>
#include <Windows.h>
using namespace std;
#define X 10
#define Y 10

class Course {
public:
	int x;
	int y;
};

int board[Y][X];				//길 1 벽 0
bool move_point[Y][X];
Course course[X * Y];
int x;
int y;
int counting;
int count_up;
int count_down;
int count_left;
int count_right;
int count_point;
int onoff;

unsigned int way;
void find_way();
void print_board();
void clear_board();

int main() {

	srand((unsigned int)time(NULL));
	board[0][0] = 1;
	move_point[0][0] = true;
	find_way();
	char command;
	while (1)
	{
		print_board();
		cout << "command: ";
		cin >> command;
		switch (command)
		{
		case 'n':
			clear_board();
			find_way();
			break;
		case '+':
			count_point++;
			break;
		case '-':
			count_point--;
			break;
		case 'r':
			if (onoff == 0)
			{
				onoff = 1;
			}
			else
				onoff = 0;
			break;
		case 'q':
			return 0;
			break;
		default:
			break;
		}
	}
	return 0;
}

void find_way()
{
	srand((unsigned int)time(NULL));
	while (board[Y - 1][X - 1] == 0)
	{
		way = rand() % 4;
		if (count_up == 5)
		{
			if (y > 0) {
				move_point[y - 1][x] = true;
			}
		}
		else if (count_down == 5)
		{
			if (y < Y -1) {
				move_point[y + 1][x] = true;
			}
		}
		else if (count_left == 5)
		{
			if (x > 0) {
				move_point[y][x - 1] = true;
			}
		}
		else if (count_right == 5)
		{
			if (x < X - 1) {
				move_point[y][x + 1] = true;
			}
		}
		if (x == 0)
		{
			if (y == 0)
			{
				if ((move_point[y + 1][x] == true) && (move_point[y][x + 1] == true))	// 움직일 공간이 없다면
				{
					board[y][x] = 0;				// 벽으로 변경
					counting--;
					x = course[counting].x;
					y = course[counting].y;			// 이전 좌표로 이동
				}
			}
			else if (y == Y - 1)
			{
				if ((move_point[y - 1][x] == true) && (move_point[y][x + 1] == true))	// 움직일 공간이 없다면
				{
					board[y][x] = 0;				// 벽으로 변경
					counting--;
					x = course[counting].x;
					y = course[counting].y;			// 이전 좌표로 이동
				}
			}
			else
			{
				if ((move_point[y - 1][x] == true) && (move_point[y + 1][x] == true) && (move_point[y][x + 1] == true))	// 움직일 공간이 없다면
				{
					board[y][x] = 0;				// 벽으로 변경
					counting--;
					x = course[counting].x;
					y = course[counting].y;			// 이전 좌표로 이동
				}
			}
		}
		else if (x == X - 1)
		{
			if (y == 0)
			{
				if ((move_point[y + 1][x] == true) && (move_point[y][x - 1] == true))	// 움직일 공간이 없다면
				{
					board[y][x] = 0;				// 벽으로 변경
					counting--;
					x = course[counting].x;
					y = course[counting].y;			// 이전 좌표로 이동
				}
			}
			else if (y == Y - 1)
			{
				if ((move_point[y - 1][x] == true) && (move_point[y][x - 1] == true))	// 움직일 공간이 없다면
				{
					board[y][x] = 0;				// 벽으로 변경
					counting--;
					x = course[counting].x;
					y = course[counting].y;			// 이전 좌표로 이동
				}
			}
			else
			{
				if ((move_point[y - 1][x] == true) && (move_point[y + 1][x] == true) && (move_point[y][x - 1] == true))	// 움직일 공간이 없다면
				{
					board[y][x] = 0;				// 벽으로 변경
					counting--;
					x = course[counting].x;
					y = course[counting].y;			// 이전 좌표로 이동
				}
			}
		}
		else
		{
			if (y == 0)
			{
				if ((move_point[y + 1][x] == true) && (move_point[y][x - 1] == true) && (move_point[y][x + 1] == true))	// 움직일 공간이 없다면
				{
					board[y][x] = 0;				// 벽으로 변경
					counting--;
					x = course[counting].x;
					y = course[counting].y;			// 이전 좌표로 이동
				}
			}
			else if (y == Y - 1)
			{
				if ((move_point[y - 1][x] == true) && (move_point[y][x - 1] == true) && (move_point[y][x + 1] == true))	// 움직일 공간이 없다면
				{
					board[y][x] = 0;				// 벽으로 변경
					counting--;
					x = course[counting].x;
					y = course[counting].y;			// 이전 좌표로 이동
				}
			}
			else
			{
				if ((move_point[y - 1][x] == true) && (move_point[y + 1][x] == true) && (move_point[y][x - 1] == true) && (move_point[y][x + 1] == true))	// 움직일 공간이 없다면
				{
					board[y][x] = 0;				// 벽으로 변경
					counting--;
					x = course[counting].x;
					y = course[counting].y;			// 이전 좌표로 이동
				}
			}
		}
		switch (way)
		{
		case 0:					// 아래
			if (count_down < 5)
			{
				if (y < Y - 1) {
					if (board[y + 1][x] == 0) {
						y++;
						board[y][x] = 1;
						move_point[y][x] = true;
						counting++;
						course[counting].x = x;
						course[counting].y = y;
						count_up = 0;
						count_down++;
						count_left = 0;
						count_right = 0;
					}
				}
			}
			break;
		case 1:					// 왼
			if (count_left < 5)
			{
				if (x > 0) {
					if (board[y][x - 1] == 0) {
						x--;
						board[y][x] = 1;
						move_point[y][x] = true;
						counting++;
						course[counting].x = x;
						course[counting].y = y;
						count_up = 0;
						count_down = 0;
						count_left++;
						count_right = 0;
					}
				}
			}
			break;
		case 2:					// 오른
			if (count_right < 5)
			{
				if (x < X - 1) {
					if (board[y][x + 1] == 0) {
						x++;
						board[y][x] = 1;
						move_point[y][x] = true;
						counting++;
						course[counting].x = x;
						course[counting].y = y;
						count_up = 0;
						count_down = 0;
						count_left = 0;
						count_right++;
					}
				}
			}
			break;
		case 3:					// 위
			if (count_up < 5)
			{
				if (y > 0) {
					if (board[y - 1][x] == 0) {
						y--;
						board[y][x] = 1;
						move_point[y][x] = true;
						counting++;
						course[counting].x = x;
						course[counting].y = y;
						count_up++;
						count_down = 0;
						count_left = 0;
						count_right = 0;
					}
				}
			}
			break;
		default:
			break;
		}
		//print_board();
		//cout << '\n';
	}
}

void print_board()
{
	if (onoff == 0)
	{
		for (int i = 0; i < Y; i++)
		{
			for (int j = 0; j < X; j++) {
				cout << board[i][j] << ' ';
			}
			cout << '\n';
		}
	}
	else
	{
		for (int i = 0; i < Y; i++)
		{
			for (int j = 0; j < X; j++) {
				if ((course[count_point].x == j) && (course[count_point].y == i))
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
					cout << "*" << ' ';
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				}
				else {
					cout << board[i][j] << ' ';
				}
			}
			cout << '\n';
		}
	}
}
void clear_board()
{
	for (int i = 0; i < Y; i++)
	{
		for (int j = 0; j < X; j++) {
			board[i][j] = 0;
			move_point[i][j] = false;
		}
	}
	for (int i = 0; i < Y*X; i++)
	{
		course[i].x = 0;
		course[i].y = 0;
	}
	x = 0;
	y = 0;
	counting = 0;
	count_up = 0;
	count_down = 0;
	count_left = 0;
	count_right = 0;
	board[0][0] = 1;
	move_point[0][0] = true;
	onoff = 0;
	count_point = 0;
}