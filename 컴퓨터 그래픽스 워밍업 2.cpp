#include <iostream>
#include <fstream>
#include <string>
using namespace std;

char str[10][1000];

void print_str();
void count_word();
int main()
{
	ifstream rf;
	int q = 0;
	rf.open("data.txt");
	for (int i = 0; i < 10; i++)
		rf.getline(str[i], 1000);
	print_str();
	count_word();
	return 0;
}

void print_str()
{
	for (int i = 0; i < 10; i++)
	{
		cout << str[i] << '\n';
	}
}

void count_word()
{
	char copystr[10][1000];
	int word_count = 0;
	int capital_count = 0;
	int number_count = 0;
	int len;
	for (int i = 0; i < 10; i++) {
		strcpy(copystr[i], str[i]);
	}
	for (int i = 0; i < 10; i++) {
		char* ptr = strtok(copystr[i], " ");
		while (ptr != NULL)              
		{
			len = strlen(ptr);
			for (int j = 0; j < len; j++) {
				if ((ptr[j] >= 'A') && (ptr[j] <= 'Z')) {
					capital_count++;
				}			
			}
			for (int j = 0; j < len; j++) {
				if ((ptr[j] >= 'A') && (ptr[j] <= 'Z')) {
					break;
				}
				else if ((ptr[j] >= 'a') && (ptr[j] <= 'z')) {
					break;
				}
				else if (j == len - 1) {
					number_count++;
				}
			}
			word_count++;
			ptr = strtok(NULL, " ");     
		}
	}
	cout << "word count: " << word_count - number_count << '\n';
	cout << "number count: " << number_count << '\n';
	cout << "Capital count: " << capital_count << '\n';


}