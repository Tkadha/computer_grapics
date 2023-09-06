#include <iostream>
#include <fstream>
#include <string>
using namespace std;

char str[10][1000];
int command_e;
int str_count;
int len;
void print_str();

void reverse_str();
void insert();
void reverse_space();
void change(char, char);
void same();
void num_PM(int);
int main()
{
	ifstream rf;
	char command;
	int q = 0;
	char a, b;
	rf.open("data.txt");
	while (!rf.eof()) {
		rf.getline(str[str_count++], 1000);	
	}
	print_str();
	while (q == 0) {
		cout << '\n' << "input the command: ";
		cin >> command;
		switch (command)
		{
		case 'd':
			reverse_str();
			break;
		case 'e':
			insert();
			if (command_e == 0) {
				command_e = 1;
			}
			else {
				command_e = 0;
			}
			break;
		case 'f':
			reverse_space();
			break;
		case 'g':		// a값을 b로 변경
			cin >> a;
			cin >> b;
			change(a, b);
			break;
		case 'h':
			same();
			break;
		case '+':
			num_PM(0);
			break;
		case '-':
			num_PM(1);
			break;
		case 'q':
			q++;
			break;
		default:
			break;
		}
	}
	rf.close();
	return 0;
}

void print_str()
{
	for (int i = 0; i < str_count; i++)
	{
		cout << str[i] << '\n';
	}
}
void reverse_str() {
	char temp;
	for (int i = 0; i < str_count; i++) {
		len = strlen(str[i]);
		for (int j = 0; j < len / 2; j++) {
			temp = str[i][j];
			str[i][j] = str[i][len - j - 1];
			str[i][len - j - 1] = temp;
		}
	}
	print_str();
}
void insert() {
	int count = 0;
	if (command_e == 0) {	// **추가
		for (int i = 0; i < str_count; i++) {
			len = strlen(str[i]);
			for (int j = 0; j < len; j++) {
				count++;
				if (count >= 4) {
					for (int k = len; k > j; k--) {
						str[i][k + 1] = str[i][k - 1];
						str[i][k] = '*';
						str[i][k - 1] = '*';
					}
					len += 2;
					count = -1;
				}
			}
			count = 0;
		}
	}
	else {					// **제거
		for (int i = 0; i < str_count; i++) {
			len = strlen(str[i]);
			for (int j = 0; j < len; j++) {
				if (str[i][j] == '*') {
					for (int k = j; k < len; k++) {
						str[i][k] = str[i][k + 1];
					}
					str[i][len - 1] = NULL;
					len--;
					j--;
				}
			}
			count = 0;
		}
	}
	print_str();
}
void reverse_space() {
	int start = 0;
	char temp;
	char wording[30];
	int word_len;
	int count = 0;
	if (command_e == 0) {	// **이 없을 때
		for (int i = 0; i < str_count; i++) {
			start = 0;
			len = strlen(str[i]);
			for (int j = 0; j < len; j++) {
				if (str[i][j] == ' ') {
					for (int k = start; k < j; k++) {
						wording[count++] = str[i][k];
					}
					wording[count] = NULL;
					word_len = strlen(wording);
					for (int m = 0; m < word_len / 2; m++) {
						temp = wording[m];
						wording[m] = wording[word_len - m - 1];
						wording[word_len - m - 1] = temp;
					}
					count = 0;
					for (int k = start; k < j; k++) {
						str[i][k] = wording[count++];
					}
					count = 0;
					start = j + 1;
				}
				else if (str[i][j + 1] == NULL) {		//마지막 문장
					for (int k = start; k <= j; k++) {
						wording[count++] = str[i][k];
					}
					wording[count] = NULL;
					word_len = strlen(wording);
					for (int m = 0; m < word_len / 2; m++) {
						temp = wording[m];
						wording[m] = wording[word_len - m - 1];
						wording[word_len - m - 1] = temp;
					}
					count = 0;
					for (int k = start; k <= j; k++) {
						str[i][k] = wording[count++];
					}
					count = 0;
				}
			}
		}
	}
	else {					// **이 있을 때
		for (int i = 0; i < str_count; i++) {
			start = 0;
			len = strlen(str[i]);
			for (int j = 0; j < len; j++) {
				if (str[i][j] == '*') {
					for (int k = start; k < j; k++) {
						wording[count++] = str[i][k];
					}
					wording[count] = NULL;
					word_len = strlen(wording);
					for (int m = 0; m < word_len / 2; m++) {
						temp = wording[m];
						wording[m] = wording[word_len - m - 1];
						wording[word_len - m - 1] = temp;
					}
					count = 0;
					for (int k = start; k < j; k++) {
						str[i][k] = wording[count++];
					}
					count = 0;
					start = j + 2;
					j++;
				}
				else if (str[i][j + 1] == NULL) {		//마지막 문장
					for (int k = start; k <= j; k++) {
						wording[count++] = str[i][k];
					}
					wording[count] = NULL;
					word_len = strlen(wording);
					for (int m = 0; m < word_len / 2; m++) {
						temp = wording[m];
						wording[m] = wording[word_len - m - 1];
						wording[word_len - m - 1] = temp;
					}
					count = 0;
					for (int k = start; k <= j; k++) {
						str[i][k] = wording[count++];
					}
					count = 0;
				}
			}
		}
	}
	print_str();
}
void change(char a, char b) {
	for (int i = 0; i < str_count; i++) {
		len = strlen(str[i]);
		for (int j = 0; j < len; j++) {
			if (str[i][j] == a) {
				str[i][j] = b;
			}
		}
		print_str();
	}
}
void same() {
	int count[20];
	for (int i = 0; i < 20; i++) {
		count[i] = 0;
	}
	for (int i = 0; i < str_count; i++) {
		len = strlen(str[i]);
		for (int j = 0; j < len / 2; j++) {
			if (str[i][j] == str[i][len - j - 1]) {
				count[i]++;
			}
			else
				break;
		}
	}
	for (int i = 0; i < str_count; i++) {
		cout << str[i] << ": ";
		for (int j = 0; j < count[i]; j++) {
			cout << str[i][j];
		}
		if (count[i] == 0)
			cout << "0";
		cout << '\n';
	}
}

void num_PM(int c)
{
	if (c == 0){		// 숫자 +1
		for (int i = 0; i < str_count; i++)
		{
			len = strlen(str[i]);
			for (int j = 0; j < len; j++) {
				if ((str[i][j] >= '0') && (str[i][j] <= '8')) {
					if ((str[i][j + 1] < '0') || (str[i][j + 1]) > '9') {	//다음이 숫자가 아니면
						str[i][j] += 1;
					}
				}
				else if (str[i][j] == '9') {
					if ((str[i][j - 1] >= '0') && (str[i][j - 1] <= '8')) {	
						str[i][j - 1] += 1;
						str[i][j] = '0';
					}
					else if (str[i][j - 1] == '9') {	
						if ((str[i][j - 2] >= '0') && (str[i][j - 2] <= '8')) {
							str[i][j - 2] += 1;
							str[i][j - 1] = '0';
							str[i][j] = '0';
						}
					}
					else if ((str[i][j + 1] < '0') || (str[i][j + 1]) > '9') {	//다음이 숫자가 아니면
						for (int k = len; k > j+1; k--) {
							str[i][k] = str[i][k - 1];
						}
						str[i][j] = '1';
						str[i][j + 1] = '0';
						len++;
						j++;
					}
				}
			}
		}
	}
	else if (c == 1) {	// 숫자 -1			만드는중
		for (int i = 0; i < str_count; i++)
		{
			len = strlen(str[i]);
			for (int j = 0; j < len; j++) {
				if ((str[i][j] >= '1') && (str[i][j] <= '9')) {
					if ((str[i][j - 1] < '1') || (str[i][j - 1]) > '9') {	//이전이 숫자가 아니면
						str[i][j] -= 1;
					}
				}
				else if (str[i][j] == '0') {
					if ((str[i][j - 1] >= '2') && (str[i][j - 1]) <= '9') {	//2자리수 라면
						str[i][j - 1] -= 1;
						str[i][j] = '9';
					}
					else if (str[i][j - 1] == '1') {
						str[i][j - 1] == '9';
					}
				}
			}
		}
	}
	print_str();
}
