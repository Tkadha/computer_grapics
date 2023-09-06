#include <iostream>
#include <cstdlib>
using namespace std;

int a[3][3];
int b[3][3];
int a4[4][4];
int b4[4][4];

void mul_matrix();
void add_matrix();
void remove_matrix();
void det_matrix(int);
void trans_matrix();
void by4_matrix();
void reset_matrix();
void print_matrix();

int main()
{
	reset_matrix();
	char command;
	int q = 0;
	print_matrix();
	while (q == 0) {
		cout << "command: ";
		cin >> command;
		switch (command) {
		case 'm':
			mul_matrix();
			print_matrix();
			break;
		case 'a':
			add_matrix();
			print_matrix();
			break;
		case 'd':
			remove_matrix();
			print_matrix();
			break;
		case 'r':
			det_matrix(0);
			break;
		case 't':
			trans_matrix();
			break;
		case 'h':
			by4_matrix();
			break;
		case 's':
			reset_matrix();
			print_matrix();
			break;
		case 'q':
			q++;
			break;
		default:
			if ((command >= '1') && (command <= '9')) {
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						a[i][j] *= command-48;
						b[i][j] *= command-48;
					}
				}
				print_matrix();
			}
			break;
		}
	}
	return 0;
}
void mul_matrix() 
{
	int c[3][3];
	for (int i = 0; i < 3; i++) {
		for (int j = 0 ; j < 3; j++) {
			c[i][j] = a[i][0] * b[0][j] + a[i][1] * b[1][j] + a[i][2] * b[2][j];
		}
	}
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			cout << c[i][j] << ' ';
		}
		cout << '\n';
	}
	cout << '\n';

}
void add_matrix()
{
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			cout << a[i][j]+b[i][j] << ' ';
		}
		cout << '\n';
	}
	cout << '\n';
}
void remove_matrix()
{
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			cout << a[i][j] - b[i][j] << ' ';
		}
		cout << '\n';
	}
	cout << '\n';
}
void det_matrix(int k) 
{
	int num = 0;
	if (k == 0) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				cout << a[i][j] << ' ';
			}
			cout << '\n';
		}
		num = a[0][0] * (a[1][1] * a[2][2] - a[1][2] * a[2][1])
			- a[0][1] * (a[1][0] * a[2][2] - a[1][2] * a[2][0])
			+ a[0][2] * (a[1][0] * a[2][1] - a[1][1] * a[2][0]);
		cout << "a = " << num << '\n';
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				cout << b[i][j] << ' ';
			}
			cout << '\n';
		}
		num = b[0][0] * (b[1][1] * b[2][2] - b[1][2] * b[2][1])
			- b[0][1] * (b[1][0] * b[2][2] - b[1][2] * b[2][0])
			+ b[0][2] * (b[1][0] * b[2][1] - b[1][1] * b[2][0]);
		cout << "b = " << num << '\n';
	}
	else if (k==1){
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				cout << a4[i][j] << ' ';
			}
			cout << '\n';
		}
		num = a[0][0] * (a[1][1] * a[2][2] - a[1][2] * a[2][1])
			- a[0][1] * (a[1][0] * a[2][2] - a[1][2] * a[2][0])
			+ a[0][2] * (a[1][0] * a[2][1] - a[1][1] * a[2][0]);
		cout << "4x4 a = " << num << '\n';
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				cout << b4[i][j] << ' ';
			}
			cout << '\n';
		}
		num = b[0][0] * (b[1][1] * b[2][2] - b[1][2] * b[2][1])
			- b[0][1] * (b[1][0] * b[2][2] - b[1][2] * b[2][0])
			+ b[0][2] * (b[1][0] * b[2][1] - b[1][1] * b[2][0]);
		cout << "4x4 b = " << num << '\n';
	}
}
void trans_matrix()
{
	int temp;
	for (int i = 1; i < 3; i++) {
		for (int j = 0; j < 2; j++) {

			temp = a[i][j];
			a[i][j] = a[j][i];
			a[j][i] = temp;
			temp = b[i][j];
			b[i][j] = b[j][i];
			b[j][i] = temp;
		}
	}
	det_matrix(0);
}
void by4_matrix() {
	a4[3][3] = 1;
	b4[3][3] = 1;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			a4[i][j] = a[i][j];
			b4[i][j] = b[i][j];
		}
	}
	det_matrix(1);
}
void reset_matrix() {
	srand(time(NULL));
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			a[i][j] = rand() % 3;
			b[i][j] = rand() % 3;
		}
	}
}
void print_matrix() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			cout << a[i][j] << ' ';
		}
		cout << '\n';
	}
	cout << '\n';
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			cout << b[i][j] << ' ';
		}
		cout << '\n';
	}
}