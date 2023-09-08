#include <iostream>
#include <cstdlib>
#include <ctime>
#include <Windows.h>
#define MAX 20
using namespace std;

void input_card();
void print_board();
void choice();
void compare(int, int, int, int);


bool board[4][4];
char card[4][4];
int color[4][4];
char card_x1;
char card_y1;
char card_x2;
char card_y2;

int main()
{
	srand((unsigned int)time(NULL));
	int count = 0;
	int point = 0;
	input_card();
	while (count < MAX) {
		cout << "남은 횟수: " << MAX - count << '\n';
		print_board();
		choice();
		cout << '\n';
		if ((card_x1 != 'r') && (card_x2 != 'r'))
		{
			print_board();
		}
		compare(card_x1 - 97, card_y1 - 49, card_x2 - 97, card_y2 - 49);
		Sleep(2000);
		system("cls");
		count++;
		if ((card_x1 == 'r')||(card_x2=='r'))
		{
			count = 0;
		}
	}
	point = (MAX - count) * 10;
	cout << "획득 점수(남은 횟수 * 10): " << point << '\n';
	return 0;
}

void input_card() {
	char alp[16] = { 'A','A','B','B','C','C','D','D','E','E','F','F','G','G','H','H' };
	int a;
	int b;
	int num = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			card[i][j] = ' ';
		}
	}

	a = rand() % 4;
	b = rand() % 4;
	while (1) {
		if (card[a][b] == ' ')
		{
			card[a][b] = alp[num++];
			if (num >= 16)
				break;
		}
		a = rand() % 4;
		b = rand() % 4;
	}
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			switch (card[i][j])
			{
			case 'A':
				color[i][j] = 3;
				break;
			case 'B':
				color[i][j] = 4;
				break;
			case 'C':
				color[i][j] = 5;
				break;
			case 'D':
				color[i][j] = 6;
				break;
			case 'E':
				color[i][j] = 7;
				break;
			case 'F':
				color[i][j] = 8;
				break;
			case 'G':
				color[i][j] = 9;
				break;
			case 'H':
				color[i][j] = 10;
				break;
			}
		}
	}
}
void print_board()
{
	cout << "  a b c d" << '\n';
	for (int i = 0; i < 4; i++) {
		cout << i + 1 << ' ';
		for (int j = 0; j < 4; j++) {
			if (board[i][j] == true)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color[i][j]);
				cout << card[i][j] << ' ';
			}
			else
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				cout << "*" << ' ';
			}
		}
		cout << '\n';
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}
void choice() {
	cout << "input card 1:";
	cin >> card_x1;
	if (card_x1 == 'r')
	{
		cout << "게임을 리셋합니다." << '\n';
		input_card();
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				board[i][j] = false;
		return;
	}
	cin >> card_y1;
	cout << "input card 2:";
	cin >> card_x2;
	if (card_x2 == 'r')
	{
		cout << "게임을 리셋합니다." << '\n';
		input_card();
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				board[i][j] = false;
		return;
	}
	cin >> card_y2;
	if ((board[card_y1 - 49][card_x1 - 97] == true) || (board[card_y2 - 49][card_x2 - 97] == true)) {
		cout << "다시 선택하십시오." << '\n';
		choice();
	}
	board[card_y1 - 49][card_x1 - 97] = true;
	board[card_y2 - 49][card_x2 - 97] = true;
}
void compare(int x1, int y1 , int x2 , int y2 )
{
	if (card[y1][x1] != card[y2][x2]) {
		board[y1][x1] = false;
		board[y2][x2] = false;
	}
}