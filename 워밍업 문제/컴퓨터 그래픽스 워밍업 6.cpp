#include <iostream>
#define BoardX 10
#define BoardY 10
using namespace std;

int point1_x, point1_y, point2_x, point2_y;
int board[BoardY][BoardX];

void print();
void clear_board();
void input_point();
void movex(int);
void movey(int);
void zoomx(int);
void zoomy(int);
int main()
{
	char command;
	clear_board();
	input_point();

	while (1) {
		print();
		cout << "input order:";
		cin >> command;
		switch (command) {
		case 'x':		// ���� �̵�
			movex(0);
			break;
		case 'X':		// ���� �̵�
			movex(1);
			break;
		case 'y':		// �� �̵�
			movey(0);
			break;
		case 'Y':		// �Ʒ� �̵� 
			movey(1);
			break;
		case 's':		// �Ѵ� Ȯ��
			zoomx(1);
			zoomy(1);
			break;
		case 'S':		// �Ѵ� ���
			zoomx(1);
			zoomy(1);
			break;
		case 'i':		// x ���
			zoomx(1);
			break;
		case 'j':		// x Ȯ��
			zoomx(0);
			break;
		case 'k':		// y ���
			zoomy(1);
			break;
		case 'l':		// y Ȯ��
			zoomy(0);
			break;
		case 'R':
			clear_board();
			input_point();
			break;
		case 'q':
			return 0;
			break;
		}
	}

	return 0;
}


void print() {
	for (int i = 0; i < BoardY; i++) {
		for (int j = 0; j < BoardX; j++) {
			if (board[i][j] == 0) {
				cout << "." << '\t';
			}
			else
				cout << "0" << '\t';
		}
		cout << '\n';
	}
}
void clear_board() {
	for (int i = 0; i < BoardY; i++) {
		for (int j = 0; j < BoardX; j++) {
			board[i][j] = 0;
		}
	}
}
void input_point()
{
	cout << "Board�� ũ��: " << BoardX << "*" << BoardY << '\n';
	cout << "input coord value:";
	cin >> point1_x;
	cin >> point1_y;
	cin >> point2_x;
	cin >> point2_y;
	if ((point1_x > BoardX) || (point1_x < 0)) {
		cout << "�ٽ� �Է��Ͻÿ�." << '\n';
		input_point();
	}
	else if ((point2_x > BoardX) || (point2_x < 0)) {
		cout << "�ٽ� �Է��Ͻÿ�." << '\n';
		input_point();
	}
	else if ((point1_y > BoardY) || (point1_y < 0)) {
		cout << "�ٽ� �Է��Ͻÿ�." << '\n';
		input_point();
	}
	else if ((point2_y > BoardY) || (point2_y < 0)) {
		cout << "�ٽ� �Է��Ͻÿ�." << '\n';
		input_point();
	}
	if (point1_x > point2_x) {
		if (point1_y > point2_y) {
			for (int i = point2_x; i <= point1_x; i++) {
				for (int j = point2_y; j <= point1_y; j++) {
					board[j][i] = 1;
				}
			}
		}
		else if (point1_y < point2_y) {
			for (int i = point2_x; i <= point1_x; i++) {
				for (int j = point1_y; j <= point2_y; j++) {
					board[j][i] = 1;
				}
			}
		}
		else {
			for (int i = point2_x; i <= point1_x; i++) {
				board[point1_y][i] = 1;
			}
		}
	}
	else if (point1_x < point2_x) {
		if (point1_y > point2_y) {
			for (int i = point1_x; i <= point2_x; i++) {
				for (int j = point2_y; j <= point1_y; j++) {
					board[j][i] = 1;
				}
			}
		}
		else if (point1_y < point2_y) {
			for (int i = point1_x; i <= point2_x; i++) {
				for (int j = point1_y; j <= point2_y; j++) {
					board[j][i] = 1;
				}
			}
		}
		else {
			for (int i = point1_x; i <= point2_x; i++) {
				board[point1_y][i] = 1;
			}
		}
	}
	else {
		if (point1_y > point2_y) {
			for (int j = point2_y; j <= point1_y; j++) {
				board[j][point1_x] = 1;

			}
		}
		else if (point1_y < point2_y) {

			for (int j = point1_y; j <= point2_y; j++) {
				board[j][point1_x] = 1;
			}
		}
		else {
			board[point1_y][point1_x] = 1;
		}
	}
}
void movex(int a) {
	if (a == 0) {		// ���� x
		if (point1_x > point2_x) {

		}
		else if (point1_x < point2_x) {
			for (int i = point1_x; i <= point2_x; i++) {

			}
		}
		else
		{

		}
	}
	else {				// ����	X
		
	}
}
void movey(int a) {
	if (a == 0) {		// �� y
		
	}
	else {				// �Ʒ� Y
		
	}
}
void zoomx(int a) {
	if (a == 0) {			//Ȯ�� j
		if (point1_x > point2_x) {
			if (point1_x < BoardX)
			{
				if (point1_y > point2_y) {
					for (int i = point2_y; i <= point1_y; i++) {
						board[i][point1_x + 1] = 1;
					}
				}
				else if (point1_y < point2_y) {
					for (int i = point1_y; i <= point2_y; i++) {
						board[i][point1_x + 1] = 1;
					}
				}
				else {
					board[point1_y][point1_x + 1] = 1;
				}
				point1_x++;
			}
		}
		else
			if (point2_x < BoardX)
			{
				if (point1_y > point2_y) {
					for (int i = point2_y; i <= point1_y; i++) {
						board[i][point2_x + 1] = 1;
					}
				}
				else if (point1_y < point2_y) {
					for (int i = point1_y; i <= point2_y; i++) {
						board[i][point2_x + 1] = 1;
					}
				}
				else {
					board[point1_y][point2_x + 1] = 1;
				}
				point2_x++;
			}
	}
	else {					//��� i
		if (point1_x < point2_x) {
			if (point1_y > point2_y) {
				for (int i = point2_y; i <= point1_y; i++) {
					board[i][point2_x] = 0;
				}
			}
			else if (point1_y < point2_y) {
				for (int i = point1_y; i <= point2_y; i++) {
					board[i][point2_x] = 0;
				}
			}
			else {
				board[point1_y][point2_x] = 0;
			}
			point2_x--;
		}
		else if (point2_x < point1_x)
		{
			if (point1_y > point2_y) {
				for (int i = point2_y; i <= point1_y; i++) {
					board[point1_y][i] = 0;
				}
			}
			else if (point1_y < point2_y) {
				for (int i = point1_y; i <= point2_y; i++) {
					board[point1_y][i] = 0;
				}
			}
			else {
				board[point1_y][point1_x] = 0;
			}
			point1_x--;
		}
	}
}
void zoomy(int a) {
	if (a == 0) {			//Ȯ�� l
		if (point1_y > point2_y) {
			if (point1_y < BoardY)
			{
				if (point1_x > point2_x) {
					for (int i = point2_x; i <= point1_x; i++) {
						board[point1_y+1][i] = 1;
					}
				}
				else if (point1_x < point2_x) {
					for (int i = point1_x; i <= point2_x; i++) {
						board[point1_y + 1][i] = 1;
					}
				}
				else {
					board[point1_y + 1][point1_x] = 1;
				}
				point1_y++;
			}
		}
		else
			if (point2_y < BoardY) {
				if (point1_x > point2_x) {
					for (int i = point2_x; i <= point1_x; i++) {
						board[point2_y + 1][i] = 1;
					}
				}
				else if (point1_x < point2_x) {
					for (int i = point1_x; i <= point2_x; i++) {
						board[point2_y + 1][i] = 1;
					}
				}
				else {
					board[point2_y + 1][point1_x] = 1;
				}
				point2_y++;
			}		
	}
	else {					//��� k
		if (point1_y < point2_y) {
			if (point1_x > point2_x) {
				for (int i = point2_x; i <= point1_x; i++) {
					board[point2_y][i] = 0;
				}
			}
			else if (point1_x < point2_x) {
				for (int i = point1_x; i <= point2_x; i++) {
					board[point2_y][i] = 0;
				}
			}
			else {
				board[point2_y][point1_x] = 0;
			}
			point2_y--;
		}
		else if (point2_y < point1_y)
		{
			if (point1_x > point2_x) {
				for (int i = point2_x; i <= point1_x; i++) {
					board[point1_y][i] = 0;
				}
			}
			else if (point1_x < point2_x) {
				for (int i = point1_x; i <= point2_x; i++) {
					board[point1_y][i] = 0;
				}
			}
			else {
				board[point1_y][point1_x] = 0;
			}
			point1_y--;
		}
	}
}