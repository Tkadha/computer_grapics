#include <iostream>
#define BoardX 10
#define BoardY 10
using namespace std;

int point1_x, point1_y, point2_x, point2_y;
int board[BoardY][BoardX];
int halfx;
int halfy;

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
		case 'x':		// 우측 이동
			movex(0);
			break;
		case 'X':		// 좌측 이동
			movex(1);
			break;
		case 'y':		// 아래 이동
			movey(0);
			break;
		case 'Y':		// 위 이동 
			movey(1);
			break;
		case 's':		// 둘다 확대
			zoomx(0);
			zoomy(0);
			break;
		case 'S':		// 둘다 축소
			zoomx(1);
			zoomy(1);
			break;
		case 'i':		// x 축소
			zoomx(1);
			break;
		case 'j':		// x 확대
			zoomx(0);
			break;
		case 'k':		// y 축소
			zoomy(1);
			break;
		case 'l':		// y 확대
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
	cout << "Board의 크기: " << BoardX << "*" << BoardY << '\n';
	cout << "input coord value:";
	cin >> point1_x;
	cin >> point1_y;
	cin >> point2_x;
	cin >> point2_y;
	if ((point1_x > BoardX) || (point1_x < 0)) {
		cout << "다시 입력하시오." << '\n';
		input_point();
	}
	else if ((point2_x > BoardX) || (point2_x < 0)) {
		cout << "다시 입력하시오." << '\n';
		input_point();
	}
	else if ((point1_y > BoardY) || (point1_y < 0)) {
		cout << "다시 입력하시오." << '\n';
		input_point();
	}
	else if ((point2_y > BoardY) || (point2_y < 0)) {
		cout << "다시 입력하시오." << '\n';
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
	halfx = (point1_x + point2_x) / 2;
	halfy = (point1_y + point2_y) / 2;

	if (a == 0) {		// 우측 x
		if (point1_x > point2_x) {
			if (board[halfy][halfx] != false) {
				if (point1_x < BoardX - 1) {
					for (int i = point1_x + 1; i >= point2_x; i--) {
						if (point1_y < point2_y) {
							for (int j = point1_y; j <= point2_y; j++) {
								board[j][i] = board[j][i - 1];
							}
						}
						else {
							for (int j = point2_y; j <= point1_y; j++) {
								board[j][i] = board[j][i - 1];
							}
						}
					}
					point2_x++;
					point1_x++;
				}
				else if (point1_x == BoardX - 1) {
					if (point1_y < point2_y) {
						for (int j = point1_y; j <= point2_y; j++) {
							board[j][point2_x] = false;
						}
					}
					else {
						for (int j = point2_y; j <= point1_y; j++) {
							board[j][point2_x] = false;
						}
					}
					point2_x++;
					point1_x = 0;
					if (point1_y < point2_y) {
						for (int j = point1_y; j <= point2_y; j++) {
							board[j][point1_x] = true;
						}
					}
					else {
						for (int j = point2_y; j <= point1_y; j++) {
							board[j][point1_x] = true;
						}
					}
				}
			}
			else {
				if (board[halfy][point1_x] != false)
				{
					if (point1_x < BoardX - 1) {
						if (point1_y < point2_y) {
							for (int j = point1_y; j <= point2_y; j++) {
								board[j][point1_x] = false;
								board[j][point2_x + 1] = true;
							}
						}
						else {
							for (int j = point2_y; j <= point1_y; j++) {
								board[j][point1_x] = false;
								board[j][point2_x + 1] = true;
							}
						}
						point1_x++;
						point2_x++;
					}
					else if (point1_x == BoardX - 1) {
						if (point1_y < point2_y) {
							for (int j = point1_y; j <= point2_y; j++) {
								board[j][point1_x] = false;
								board[j][point2_x + 1] = true;
							}
						}
						else {
							for (int j = point2_y; j <= point1_y; j++) {
								board[j][point1_x] = false;
								board[j][point2_x + 1] = true;
							}
						}
						point1_x = 0;
						point2_x++;
					}
				}
				else
				{
					if (point1_x < BoardX - 1) {
						if (point1_y < point2_y) {
							for (int i = 0; i <= point1_y; i++) {
								board[i][point2_x] = false;
								board[i][point1_x + 1] = true;
							}
							for (int i = point2_y; i <= BoardY - 1; i++) {
								board[i][point2_x] = false;
								board[i][point1_x + 1] = true;
							}
						}
						else
						{
							for (int i = 0; i <= point2_y; i++) {
								board[i][point2_x] = false;
								board[i][point1_x + 1] = true;
							}
							for (int i = point1_y; i <= BoardY - 1; i++) {
								board[i][point2_x] = false;
								board[i][point1_x + 1] = true;
							}
						}
						point2_x++;
						point1_x++;
					}
					else if (point1_x == BoardX - 1) {
						point1_x = 0;
						if (point1_y < point2_y) {
							for (int i = 0; i <= point1_y; i++) {
								board[i][point2_x] = false;
								board[i][point1_x] = true;
							}
							for (int i = point2_y; i <= BoardY - 1; i++) {
								board[i][point2_x] = false;
								board[i][point1_x] = true;
							}
						}
						else
						{
							for (int i = 0; i <= point2_y; i++) {
								board[i][point2_x] = false;
								board[i][point1_x] = true;
							}
							for (int i = point1_y; i <= BoardY - 1; i++) {
								board[i][point2_x] = false;
								board[i][point1_x] = true;
							}
						}
						point2_x++;
					}
				}
			}
		}
		else if (point1_x < point2_x) {
			if (board[halfy][halfx] != false) {
				if (point2_x < BoardX - 1) {
					for (int i = point2_x + 1; i >= point1_x; i--) {
						if (point1_y < point2_y) {
							for (int j = point1_y; j <= point2_y; j++) {
								board[j][i] = board[j][i - 1];
							}
						}
						else {
							for (int j = point2_y; j <= point1_y; j++) {
								board[j][i] = board[j][i - 1];
							}
						}
					}
					point2_x++;
					point1_x++;
				}
				else if (point2_x == BoardX - 1) {
					if (point1_y < point2_y) {
						for (int j = point1_y; j <= point2_y; j++) {
							board[j][point1_x] = false;
						}
					}
					else {
						for (int j = point2_y; j <= point1_y; j++) {
							board[j][point1_x] = false;
						}
					}
					point1_x++;
					point2_x = 0;
					if (point1_y < point2_y) {
						for (int j = point1_y; j <= point2_y; j++) {
							board[j][point2_x] = true;
						}
					}
					else {
						for (int j = point2_y; j <= point1_y; j++) {
							board[j][point2_x] = true;
						}
					}
				}
			}
			else {
				if (board[halfy][point1_x] != false)
				{
					if (point2_x < BoardX - 1) {
						if (point1_y < point2_y) {
							for (int j = point1_y; j <= point2_y; j++) {
								board[j][point1_x] = false;
								board[j][point2_x + 1] = true;
							}
						}
						else {
							for (int j = point2_y; j <= point1_y; j++) {
								board[j][point1_x] = false;
								board[j][point2_x + 1] = true;
							}
						}
						point2_x++;
						point1_x++;
					}
					else if (point2_x == BoardX - 1) {
						point2_x = 0;
						if (point1_y < point2_y) {
							for (int j = point1_y; j <= point2_y; j++) {
								board[j][point1_x] = false;
								board[j][point2_x] = true;
							}
						}
						else {
							for (int j = point2_y; j <= point1_y; j++) {
								board[j][point1_x] = false;
								board[j][point2_x] = true;
							}
						}
						point1_x++;
					}
				}
				else
				{
					if (point2_x < BoardX - 1) {
						if (point1_y < point2_y) {
							for (int i = 0; i <= point1_y; i++) {
								board[i][point1_x] = false;
								board[i][point2_x + 1] = true;
							}
							for (int i = point2_y; i <= BoardY - 1; i++) {
								board[i][point1_x] = false;
								board[i][point2_x + 1] = true;
							}
						}
						else
						{
							for (int i = 0; i <= point2_y; i++) {
								board[i][point1_x] = false;
								board[i][point2_x + 1] = true;
							}
							for (int i = point1_y; i <= BoardY - 1; i++) {
								board[i][point1_x] = false;
								board[i][point2_x + 1] = true;
							}
						}
						point2_x++;
						point1_x++;
					}
					else if (point2_x == BoardX - 1) {
						point2_x = 0;
						if (point1_y < point2_y) {
							for (int i = 0; i <= point1_y; i++) {
								board[i][point1_x] = false;
								board[i][point2_x] = true;
							}
							for (int i = point2_y; i <= BoardY - 1; i++) {
								board[i][point1_x] = false;
								board[i][point2_x] = true;
							}
						}
						else
						{
							for (int i = 0; i <= point2_y; i++) {
								board[i][point1_x] = false;
								board[i][point2_x] = true;
							}
							for (int i = point1_y; i <= BoardY - 1; i++) {
								board[i][point1_x] = false;
								board[i][point2_x] = true;
							}
						}
						point1_x++;
					}
				}
			}
		}
		else	//x1==x2
		{
			if (point2_x < BoardX - 1) {
				for (int i = point2_x + 1; i >= point1_x; i--) {
					if (point1_y < point2_y) {
						for (int j = point1_y; j <= point2_y; j++) {
							board[j][i] = board[j][i - 1];
						}
					}
					else {
						for (int j = point2_y; j <= point1_y; j++) {
							board[j][i] = board[j][i - 1];
						}
					}
				}
				point2_x++;
				point1_x++;
			}
			else if (point2_x == BoardX - 1) {
				if (point1_y < point2_y) {
					for (int j = point1_y; j <= point2_y; j++) {
						board[j][point1_x] = false;
					}
				}
				else {
					for (int j = point2_y; j <= point1_y; j++) {
						board[j][point1_x] = false;
					}
				}
				point1_x = 0;
				point2_x = 0;
				if (point1_y < point2_y) {
					for (int j = point1_y; j <= point2_y; j++) {
						board[j][point2_x] = true;
					}
				}
				else {
					for (int j = point2_y; j <= point1_y; j++) {
						board[j][point2_x] = true;
					}
				}
			}
		}
	}
	else {				// 좌측	X
		if (point1_x > point2_x) {
			if (board[halfy][halfx] != false) {
				if (point2_x > 0) {
					for (int i = point2_x; i <= point1_x + 1; i++) {
						if (point1_y < point2_y) {
							for (int j = point1_y; j <= point2_y; j++) {
								board[j][i - 1] = board[j][i];
							}
						}
						else {
							for (int j = point2_y; j <= point1_y; j++) {
								board[j][i - 1] = board[j][i];
							}
						}
					}
					point2_x--;
					point1_x--;
				}
				else if (point2_x == 0) {
					if (point1_y < point2_y) {
						for (int j = point1_y; j <= point2_y; j++) {
							board[j][point1_x] = false;
						}
					}
					else {
						for (int j = point2_y; j <= point1_y; j++) {
							board[j][point1_x] = false;
						}
					}
					point1_x--;
					point2_x = BoardX - 1;
					if (point1_y < point2_y) {
						for (int j = point1_y; j <= point2_y; j++) {
							board[j][point2_x] = true;
						}
					}
					else {
						for (int j = point2_y; j <= point1_y; j++) {
							board[j][point2_x] = true;
						}
					}
				}
			}
			else {
				if (board[halfy][point1_x] != false)
				{
					if (point2_x > 0) {
						if (point1_y < point2_y) {
							for (int i = point1_y; i <= point2_y; i++) {
								board[i][point2_x] = false;
								board[i][point1_x - 1] = true;
							}
						}
						else
						{
							for (int i = point2_y; i <= point1_y; i++) {
								board[i][point2_x] = false;
								board[i][point1_x - 1] = true;
							}
						}
						point2_x--;
						point1_x--;
					}
					else if (point2_x == 0) {
						if (point1_y < point2_y) {
							for (int i = point1_y; i <= point2_y; i++) {
								board[i][point2_x] = false;
								board[i][point1_x - 1] = true;
							}
						}
						else
						{
							for (int i = point2_y; i <= point1_y; i++) {
								board[i][point2_x] = false;
								board[i][point1_x - 1] = true;
							}
						}
						point2_x = BoardX - 1;
						point1_x--;
					}
				}
			}
		}
		else if (point1_x < point2_x) {
			if (board[halfy][halfx] != false) {
				if (point1_x > 0) {
					for (int i = point1_x ; i <= point2_x+1; i++) {
						if (point1_y < point2_y) {
							for (int j = point1_y; j <= point2_y; j++) {
								board[j][i - 1] = board[j][i];
							}
						}
						else {
							for (int j = point2_y; j <= point1_y; j++) {
								board[j][i - 1] = board[j][i];
							}
						}
					}
					point2_x--;
					point1_x--;
				}
				else if (point1_x == 0) {
					if (point1_y < point2_y) {
						for (int j = point1_y; j <= point2_y; j++) {
							board[j][point2_x] = false;
						}
					}
					else {
						for (int j = point2_y; j <= point1_y; j++) {
							board[j][point2_x] = false;
						}
					}
					point2_x--;
					point1_x = BoardX - 1;
					if (point1_y < point2_y) {
						for (int j = point1_y; j <= point2_y; j++) {
							board[j][point1_x] = true;
						}
					}
					else {
						for (int j = point2_y; j <= point1_y; j++) {
							board[j][point1_x] = true;
						}
					}
				}
			}
			else {
				if (board[halfy][point1_x] != false)
				{
					if (point1_x > 0) {
						if (point1_y < point2_y) {
							for (int i = point1_y; i <= point2_y; i++) {
								board[i][point1_x] = false;
								board[i][point2_x - 1] = true;
							}
						}
						else
						{
							for (int i = point2_y; i <= point1_y; i++) {
								board[i][point1_x] = false;
								board[i][point2_x - 1] = true;
							}
						}
						point2_x--;
						point1_x--;
					}
					else if (point1_x == 0) {
						if (point1_y < point2_y) {
							for (int i = point1_y; i <= point2_y; i++) {
								board[i][point2_x] = false;
								board[i][BoardX - 1] = true;
							}
						}
						else
						{
							for (int i = point2_y; i <= point1_y; i++) {
								board[i][point2_x] = false;
								board[i][BoardX - 1] = true;
							}
						}
						point1_x = BoardX - 1;
						point2_x--;
					}
				}
				else
				{
					if (point1_x > 0) {
						if (point1_y < point2_y) {
							for (int i = 0; i <= point1_y; i++) {
								board[i][point2_x] = false;
								board[i][point1_x - 1] = true;
							}
							for (int i = point2_y; i <= BoardY - 1; i++) {
								board[i][point2_x] = false;
								board[i][point1_x - 1] = true;
							}
						}
						else
						{
							for (int i = 0; i <= point2_y; i++) {
								board[i][point2_x] = false;
								board[i][point1_x - 1] = true;
							}
							for (int i = point1_y; i <= BoardY - 1; i++) {
								board[i][point2_x] = false;
								board[i][point1_x - 1] = true;
							}
						}
						point2_x--;
						point1_x--;
					}
					else if (point1_x == 0)
					{
						if (point1_y < point2_y) {
							for (int i = 0; i <= point1_y; i++) {
								board[i][point2_x] = false;
								board[i][BoardX - 1] = true;
							}
							for (int i = point2_y; i <= BoardY - 1; i++) {
								board[i][point2_x] = false;
								board[i][BoardX - 1] = true;
							}
						}
						else
						{
							for (int i = 0; i <= point2_y; i++) {
								board[i][point2_x] = false;
								board[i][BoardX - 1] = true;
							}
							for (int i = point1_y; i <= BoardY - 1; i++) {
								board[i][point2_x] = false;
								board[i][BoardX - 1] = true;
							}
						}
						point1_x = BoardX - 1;
						point2_x--;
					}
				}
			}
		}
		else	//x1==x2
		{
			if (point2_x > 0) {
				for (int i = point1_x; i <= point2_x + 1; i++) {
					if (point1_y < point2_y) {
						for (int j = point1_y; j <= point2_y; j++) {
							board[j][i - 1] = board[j][i];
						}
					}
					else {
						for (int j = point2_y; j <= point1_y; j++) {
							board[j][i - 1] = board[j][i];
						}
					}
				}
				point2_x--;
				point1_x--;
			}
			else if (point2_x == 0) {
				if (point1_y < point2_y) {
					for (int j = point1_y; j <= point2_y; j++) {
						board[j][point1_x] = false;
					}
				}
				else {
					for (int j = point2_y; j <= point1_y; j++) {
						board[j][point1_x] = false;
					}
				}
				point1_x = BoardX-1;
				point2_x = BoardX-1;
				if (point1_y < point2_y) {
					for (int j = point1_y; j <= point2_y; j++) {
						board[j][point2_x] = true;
					}
				}
				else {
					for (int j = point2_y; j <= point1_y; j++) {
						board[j][point2_x] = true;
					}
				}
			}
		}
	}
}
void movey(int a) {
	halfx = (point1_x + point2_x) / 2;
	halfy = (point1_y + point2_y) / 2;
	if (a == 0) {		// 아래 y
		if (point1_y > point2_y) {
			if (board[halfy][halfx] != false) {
				if (point1_y < BoardY - 1) {
					for (int i = point1_y + 1; i >= point2_y; i--) {
						if (point1_x < point2_x) {
							for (int j = point1_x; j <= point2_x; j++) {
								board[i][j] = board[i-1][j];
							}
						}
						else {
							for (int j = point2_x; j <= point1_x; j++) {
								board[i][j] = board[i-1][j];
							}
						}
					}
					if (point2_y == 0) {
						if (point1_x < point2_x) {
							for (int j = point1_x; j <= point2_x; j++) {
								board[point2_y][j] = board[BoardY - 1][j];
							}
						}
						else {
							for (int j = point2_x; j <= point1_x; j++) {
								board[point2_y][j] = board[BoardY - 1][j];
							}
						}
					}
					point2_y++;
					point1_y++;
				}
				else if (point1_y == BoardY - 1) {
					if (point1_x < point2_x) {
						for (int j = point1_x; j <= point2_x; j++) {
							board[point2_y][j] = false;
						}
					}
					else {
						for (int j = point2_x; j <= point1_x; j++) {
							board[point2_y][j] = false;
						}
					}
					point2_y++;
					point1_y = 0;
					if (point1_x < point2_x) {
						for (int j = point1_x; j <= point2_x; j++) {
							board[point1_y][j] = true;
						}
					}
					else {
						for (int j = point2_x; j <= point1_x; j++) {
							board[point1_y][j] = true;
						}
					}
				}
			}
			else
			{
				if (board[point2_y][halfx] != false)
				{
					if (point1_y < BoardY - 1) {
						if (point1_x < point2_x) {
							for (int i = point1_x; i <= point2_x; i++) {
								board[point1_y][i] = false;
								board[point2_y + 1][i] = true;
							}
						}
						else
						{
							for (int i = point2_x; i <= point1_x; i++) {
								board[point1_y][i] = false;
								board[point2_y + 1][i] = true;
							}
						}
						point2_y++;
						point1_y++;
					}
					else if (point1_y == BoardY - 1) {
						if (point1_x < point2_x) {
							for (int i = point1_x; i <= point2_x; i++) {
								board[point2_y + 1][i] = true;
								board[point1_y][i] = false;
							}
						}
						else
						{
							for (int i = point2_x; i <= point1_x; i++) {
								board[point2_y + 1][i] = true;
								board[point1_y][i] = false;
							}
						}
						point1_y = 0;
						point2_y++;
					}
				}
				else
				{
					if (point1_y < BoardY - 1) {
						if (point1_x < point2_x) {
							for (int i = 0; i <= point1_x; i++)
							{
								board[point2_y][i] = false;
								board[point1_y + 1][i] = true;
							}
							for (int i = point2_x; i <= BoardX - 1; i++)
							{
								board[point2_y][i] = false;
								board[point1_y + 1][i] = true;
							}
						}
						else
						{
							for (int i = 0; i <= point2_x; i++)
							{
								board[point2_y][i] = false;
								board[point1_y + 1][i] = true;
							}
							for (int i = point1_x; i <= BoardX - 1; i++)
							{
								board[point2_y][i] = false;
								board[point1_y + 1][i] = true;
							}
						}
						point2_y++;
						point1_y++;
					}
					else if (point1_y == BoardY - 1) {
						if (point1_x < point2_x) {
							for (int i = 0; i <= point1_x; i++)
							{
								board[point2_y][i] = false;
								board[0][i] = true;
							}
							for (int i = point2_x; i <= BoardX - 1; i++)
							{
								board[point2_y][i] = false;
								board[0][i] = true;
							}
						}
						else {
							for (int i = 0; i <= point2_x; i++)
							{
								board[point2_y][i] = false;
								board[0][i] = true;
							}
							for (int i = point1_x; i <= BoardX - 1; i++)
							{
								board[point2_y][i] = false;
								board[0][i] = true;
							}
						}
						point1_y = 0;
						point2_y++;
					}
				}
			}
		}
		else if (point1_y < point2_y) {
			if (board[halfy][halfx] != false) {
				if (point2_y < BoardY - 1) {
					for (int i = point2_y + 1; i >= point1_y; i--) {
						if (point1_x < point2_x) {
							for (int j = point1_x; j <= point2_x; j++) {
								board[i][j] = board[i - 1][j];
							}
						}
						else {
							for (int j = point2_x; j <= point1_x; j++) {
								board[i][j] = board[i - 1][j];
							}
						}
					}
					if (point1_y == 0) {
						if (point1_x < point2_x) {
							for (int j = point1_x; j <= point2_x; j++) {
								board[point1_y][j] = board[BoardY - 1][j];
							}
						}
						else {
							for (int j = point2_x; j <= point1_x; j++) {
								board[point1_y][j] = board[BoardY - 1][j];
							}
						}
					}
					point2_y++;
					point1_y++;
				}
				else if (point2_y == BoardY - 1) {
					if (point1_x < point2_x) {
						for (int j = point1_x; j <= point2_x; j++) {
							board[point1_y][j] = false;
						}
					}
					else {
						for (int j = point2_x; j <= point1_x; j++) {
							board[point1_y][j] = false;
						}
					}
					point1_y++;
					point2_y = 0;
					if (point1_x < point2_x) {
						for (int j = point1_x; j <= point2_x; j++) {
							board[point2_y][j] = true;
						}
					}
					else {
						for (int j = point2_x; j <= point1_x; j++) {
							board[point2_y][j] = true;
						}
					}
				}
			}
			else
			{
				if (board[point1_y][halfx] != false) 
				{
					if (point2_y < BoardY - 1) {
						if (point1_x < point2_x) {
							for (int i = point1_x; i <= point2_x; i++) {
								board[point1_y][i] = false;
								board[point2_y + 1][i] = true;
							}
						}
						else
						{
							for (int i = point2_x; i <= point1_x; i++) {
								board[point1_y][i] = false;
								board[point2_y + 1][i] = true;
							}
						}
						point2_y++;
						point1_y++;
					}
					else if (point2_y == BoardY - 1) {
						if (point1_x < point2_x) {
							for (int i = point1_x; i <= point2_x; i++) {
								board[point1_y][i] = false;
								board[0][i] = true;
							}
						}
						else
						{
							for (int i = point2_x; i <= point1_x; i++) {
								board[point1_y][i] = false;
								board[0][i] = true;
							}
						}
						point2_y = 0;
						point1_y++;
					}
				}
				else
				{
					if (point2_y < BoardY - 1) {
						if (point1_x < point2_x) {
							for (int i = 0; i <= point1_x; i++)
							{
								board[point1_y][i] = false;
								board[point2_y + 1][i] = true;
							}
							for (int i = point2_x; i <= BoardX - 1; i++)
							{
								board[point1_y][i] = false;
								board[point2_y + 1][i] = true;
							}
						}
						else
						{
							for (int i = 0; i <= point2_x; i++)
							{
								board[point1_y][i] = false;
								board[point2_y + 1][i] = true;
							}
							for (int i = point1_x; i <= BoardX - 1; i++)
							{
								board[point1_y][i] = false;
								board[point2_y + 1][i] = true;
							}
						}
						point2_y++;
						point1_y++;
					}
					else if (point2_y == BoardY - 1) {
						if (point1_x < point2_x) {
							for (int i = 0; i <= point1_x; i++)
							{
								board[point1_y][i] = false;
								board[0][i] = true;
							}
							for (int i = point2_x; i <= BoardX - 1; i++)
							{
								board[point1_y][i] = false;
								board[0][i] = true;
							}
						}
						else {
							for (int i = 0; i <= point2_x; i++)
							{
								board[point1_y][i] = false;
								board[0][i] = true;
							}
							for (int i = point1_x; i <= BoardX - 1; i++)
							{
								board[point1_y][i] = false;
								board[0][i] = true;
							}
						}
						point2_y = 0;
						point1_y++;
					}
				}
			}
		}
		else	//y1==y2
		{
			if (point2_y < BoardY - 1) {
				for (int i = point2_y + 1; i >= point1_y; i--) {
					if (point1_x < point2_x) {
						for (int j = point1_x; j <= point2_x; j++) {
							board[i][j] = board[i-1][j];
						}
					}
					else {
						for (int j = point2_x; j <= point1_x; j++) {
							board[i][j] = board[i - 1][j];
						}
					}
				}
				if (point1_y == 0) {
					if (point1_x < point2_x) {
						for (int j = point1_x; j <= point2_x; j++) {
							board[point1_y][j] = board[BoardY - 1][j];
						}
					}
					else {
						for (int j = point2_x; j <= point1_x; j++) {
							board[point1_y][j] = board[BoardY - 1][j];
						}
					}
				}
				point2_y++;
				point1_y++;
			}
			else if (point2_y == BoardY - 1) {
				if (point1_x < point2_x) {
					for (int j = point1_x; j <= point2_x; j++) {
						board[point1_y][j] = false;
					}
				}
				else {
					for (int j = point2_x; j <= point1_x; j++) {
						board[point1_y][j] = false;
					}
				}
				point1_y = 0;
				point2_y = 0;
				if (point1_x < point2_x) {
					for (int j = point1_x; j <= point2_x; j++) {
						board[point2_y][j] = true;
					}
				}
				else {
					for (int j = point2_x; j <= point1_x; j++) {
						board[point2_y][j] = true;
					}
				}
			}
		}
	}
	else {				// 위 Y
		if (point1_y > point2_y) {
			if (board[halfy][halfx] != false) {
				if (point2_y > 0) {
					for (int i = point2_y; i <= point1_y + 1; i++) {
						if (point1_x < point2_x) {
							for (int j = point1_x; j <= point2_x; j++) {
								board[i - 1][j] = board[i][j];
							}
						}
						else {
							for (int j = point2_x; j <= point1_x; j++) {
								board[i - 1][j] = board[i][j];
							}
						}
					}
					point2_y--;
					point1_y--;
					if (point2_y != 0) {
						if (point1_x < point2_x) {
							for (int j = point1_x; j <= point2_x; j++) {
								board[BoardY - 1][j] = board[0][j];
							}
						}
						else
						{
							for (int j = point2_x; j <= point1_x; j++) {
								board[BoardY - 1][j] = board[0][j];
							}
						}
					}
				}
				else if (point2_y == 0) {
					if (point1_x < point2_x) {
						for (int j = point1_x; j <= point2_x; j++) {
							board[point1_y][j] = false;
						}
					}
					else {
						for (int j = point2_x; j <= point1_x; j++) {
							board[point1_y][j] = false;
						}
					}
					point1_y--;
					point2_y = BoardY - 1;
					if (point1_x < point2_x) {
						for (int j = point1_x; j <= point2_x; j++) {
							board[point2_y][j] = true;
						}
					}
					else {
						for (int j = point2_x; j <= point1_x; j++) {
							board[point2_y][j] = true;
						}
					}
				}
			}
			else {
				if (board[point1_y][halfx] != false)
				{
					if (point2_y > 0) {
						if (point1_x < point2_x) {
							for (int i = point1_x; i <= point2_x; i++) {
								board[point2_y][i] = false;
								board[point1_y - 1][i] = true;
							}
						}
						else
						{
							for (int i = point2_x; i <= point1_x; i++) {
								board[point2_y][i] = false;
								board[point1_y - 1][i] = true;
							}
						}
						point2_y--;
						point1_y--;
					}
					else if (point2_y == 0) {
						if (point1_x < point2_x) {
							for (int i = point1_x; i <= point2_x; i++) {
								board[point2_y][i] = false;
								board[point1_y - 1][i] = true;
							}
						}
						else
						{
							for (int i = point2_x; i <= point1_x; i++) {
								board[point2_y][i] = false;
								board[point1_y - 1][i] = true;
							}
						}
						point2_y = BoardY - 1;
						point1_y--;
					}
				}
				else
				{
					if (point2_y > 0) {
						if (point1_x < point2_x) {
							for (int i = 0; i <= point1_x; i++)
							{
								board[point1_y][i] = false;
								board[point2_y - 1][i] = true;
							}
							for (int i = point2_x; i <= BoardX - 1; i++)
							{
								board[point1_y][i] = false;
								board[point2_y - 1][i] = true;
							}
						}
						else
						{
							for (int i = 0; i <= point2_x; i++)
							{
								board[point1_y][i] = false;
								board[point2_y - 1][i] = true;
							}
							for (int i = point1_x; i <= BoardX - 1; i++)
							{
								board[point1_y][i] = false;
								board[point2_y - 1][i] = true;
							}
						}
						point1_y--;
						point2_y--;
					}
					else if (point2_y == 0) {
						if (point1_x < point2_x) {
							for (int i = 0; i <= point1_x; i++)
							{
								board[point1_y][i] = false;
								board[BoardY - 1][i] = true;
							}
							for (int i = point2_x; i <= BoardX - 1; i++)
							{
								board[point1_y][i] = false;
								board[BoardY - 1][i] = true;
							}
						}
						else
						{
							for (int i = 0; i <= point2_x; i++)
							{
								board[point1_y][i] = false;
								board[BoardY - 1][i] = true;
							}
							for (int i = point1_x; i <= BoardX - 1; i++)
							{
								board[point1_y][i] = false;
								board[BoardY - 1][i] = true;
							}
						}
						point2_y = BoardY - 1;
						point1_y--;
					}
				}
			}
		}
		else if (point1_y < point2_y) {				
			if (board[halfy][halfx] != false) {
				if (point1_y > 0) {
					for (int i = point1_y; i <= point2_y + 1; i++) {
						if (point1_x < point2_x) {
							for (int j = point1_x; j <= point2_x; j++) {
								board[i - 1][j] = board[i][j];
							}
						}
						else {
							for (int j = point2_x; j <= point1_x; j++) {
								board[i - 1][j] = board[i][j];
							}
						}
					}
					point2_y--;
					point1_y--;
					if (point1_y != 0) {
						if (point1_x < point2_x) {
							for (int j = point1_x; j <= point2_x; j++) {
								board[BoardY-1][j] = board[0][j];
							}
						}
						else
						{
							for (int j = point2_x; j <= point1_x; j++) {
								board[BoardY-1][j] = board[0][j];
							}
						}
					}
				}
				else if (point1_y == 0) {
					if (point1_x < point2_x) {
						for (int j = point1_x; j <= point2_x; j++) {
							board[point2_y][j] = false;
						}
					}
					else {
						for (int j = point2_x; j <= point1_x; j++) {
							board[point2_y][j] = false;
						}
					}
					point2_y--;
					point1_y = BoardY - 1;
					if (point1_x < point2_x) {
						for (int j = point1_x; j <= point2_x; j++) {
							board[point1_y][j] = true;
						}
					}
					else {
						for (int j = point2_x; j <= point1_x; j++) {
							board[point1_y][j] = true;
						}
					}
				}
			}
			else {
				if (board[point1_y][halfx] != false)
				{
					if (point1_y > 0) {
						if (point1_x < point2_x) {
							for (int i = point1_x; i <= point2_x; i++) {
								board[point1_y][i] = false;
								board[point2_y - 1][i] = true;
							}
						}
						else
						{
							for (int i = point2_x; i <= point1_x; i++) {
								board[point1_y][i] = false;
								board[point2_y - 1][i] = true;
							}
						}
						point2_y--;
						point1_y--;
					}
					else if (point1_y == 0) {
						if (point1_x < point2_x) {
							for (int i = point1_x; i <= point2_x; i++) {
								board[point1_y][i] = false;
								board[point2_y - 1][i] = true;
							}
						}
						else
						{
							for (int i = point2_x; i <= point1_x; i++) {
								board[point1_y][i] = false;
								board[point2_y - 1][i] = true;
							}
						}
						point1_y = BoardY - 1;
						point2_y--;
					}
				}
				else
				{
					if (point1_y > 0) {
						if (point1_x < point2_x) {
							for (int i = 0; i <= point1_x; i++)
							{
								board[point2_y][i] = false;
								board[point1_y - 1][i] = true;
							}
							for (int i = point2_x; i <= BoardX - 1; i++)
							{
								board[point2_y][i] = false;
								board[point1_y - 1][i] = true;
							}
						}
						else
						{
							for (int i = 0; i <= point2_x; i++)
							{
								board[point2_y][i] = false;
								board[point1_y - 1][i] = true;
							}
							for (int i = point1_x; i <= BoardX - 1; i++)
							{
								board[point2_y][i] = false;
								board[point1_y - 1][i] = true;
							}
						}
						point2_y--;
						point1_y--;
					}
					else if (point1_y == 0) {
						if (point1_x < point2_x) {
							for (int i = 0; i <= point1_x; i++)
							{
								board[point2_y][i] = false;
								board[BoardY - 1][i] = true;
							}
							for (int i = point2_x; i <= BoardX - 1; i++)
							{
								board[point2_y][i] = false;
								board[BoardY - 1][i] = true;
							}
						}
						else
						{
							for (int i = 0; i <= point2_x; i++)
							{
								board[point2_y][i] = false;
								board[BoardY - 1][i] = true;
							}
							for (int i = point1_x; i <= BoardX - 1; i++)
							{
								board[point2_y][i] = false;
								board[BoardY - 1][i] = true;
							}
						}
						point1_y = BoardY - 1;
						point2_y--;
					}
				}
			}
		}
		else	//y1==y2
		{
			if (point2_y > 0) {
				for (int i = point1_y; i <= point2_y + 1; i++) {
					if (point1_x < point2_x) {
						for (int j = point1_x; j <= point2_x; j++) {
							board[i - 1][j] = board[i][j];
						}
					}
					else {
						for (int j = point2_x; j <= point1_x; j++) {
							board[i - 1][j] = board[i][j];
						}
					}
				}
				if (point1_y == BoardY - 1) {
					if (point1_x < point2_x) {
						for (int j = point1_x; j <= point2_x; j++) {
							board[BoardY - 1][j] = board[0][j];
						}
					}
					else {
						for (int j = point2_x; j <= point1_x; j++) {
							board[BoardY - 1][j] = board[0][j];
						}
					}
				}
				point2_y--;
				point1_y--;
			}
			else if (point2_y == 0) {
				if (point1_x < point2_x) {
					for (int j = point1_x; j <= point2_x; j++) {
						board[point1_y][j] = false;
					}
				}
				else {
					for (int j = point2_x; j <= point1_x; j++) {
						board[point1_y][j] = false;
					}
				}
				point1_y = BoardY - 1;
				point2_y = BoardY - 1;
				if (point1_x < point2_x) {
					for (int j = point1_x; j <= point2_x; j++) {
						board[point2_y][j] = true;
					}
				}
				else {
					for (int j = point2_x; j <= point1_x; j++) {
						board[point2_y][j] = true;
					}
				}
			}
		}
	}
}
void zoomx(int a) {
	halfx = (point1_x + point2_x) / 2;
	halfy = (point1_y + point2_y) / 2;
	if (board[halfy][halfx] == true) {
		if (a == 0) {			//확대 j
			if (point1_x > point2_x) {
				if (point1_x < BoardX - 1)
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
			{
				if (point2_x < BoardX - 1)
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
		}
		else {					//축소 i
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
}
void zoomy(int a) {
	halfx = (point1_x + point2_x) / 2;
	halfy = (point1_y + point2_y) / 2;
	if (board[halfy][halfx] == true) {
		if (a == 0) {			//확대 l
			if (point1_y > point2_y) {
				if (point1_y < BoardY - 1)
				{
					if (point1_x > point2_x) {
						for (int i = point2_x; i <= point1_x; i++) {
							board[point1_y + 1][i] = 1;
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
				if (point2_y < BoardY - 1) {
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
		else {					//축소 k
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
}