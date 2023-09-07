#include <iostream>
#include <cmath>
#include <algorithm>
using namespace std;

typedef struct Point {
	int x;
	int y;
	int z;
};
typedef struct Sort_point {
	float length;
	int x;
	int y;
	int z;
};
bool upSort(Sort_point& a, Sort_point& b)
{
	return a.length > b.length;
}
bool downSort(Sort_point& a, Sort_point& b)
{
	return a.length < b.length;
}
void found_top_bottom();
void print_list();
void input(int, int, int, int);
void output(int);
void found_point(int);
void sort_list(int);

int list_in[10];			//  ����Ʈ�� 1: ����, 0: ����
Point point[10];
int save_in[10];
Point save[10];
int count_list;
int top;
int bottom;
int return_sort;
int main()
{
	char command;
	int q = 0;
	int x, y, z;
	while (q == 0) {
		found_top_bottom();
		print_list();
		cout << "command: ";
		cin >> command;
		switch (command)
		{
		case '+':							// ����Ʈ �� �� �Է� (9������ ���� 0���� ��������� 0�� ä���)
			if (count_list < 10) {
				cin >> x;
				cin >> y;
				cin >> z;
				input(x, y, z, 0);
			}
			break;
		case '-':							// ����Ʈ �� �� ����
			output(0);
			break;
		case 'e':							// ����Ʈ �� �Ʒ� �Է�	(0������ ���� 9���� ��������� 9�� ä���?)
			if (count_list < 10) {
				cin >> x;
				cin >> y;
				cin >> z;
				input(x, y, z, 1);
			}
			break;
		case 'd':							// ����Ʈ �� �Ʒ� ����
			output(1);
			break;
		case 'l':							// ����Ʈ ���� ���� ���� ���
			cout << "����Ʈ ����: " << count_list << '\n';
			break;
		case 'c':							// ����Ʈ �ʱ�ȭ
			for (int i = 0; i < 10; i++) {
				point[i].x = 0;
				point[i].y = 0;
				point[i].z = 0;
				list_in[i] = 0;
			}
			count_list = 0;
			break;
		case 'm':							// ������ ���� �� �Ÿ��� �ִ� ���� ��ǥ ���
			found_point(1);
			break;
		case 'n':							// ������ ���� ����� �Ÿ��� �ִ� ���� ��ǥ ���
			found_point(0);
			break;
		case 'a':							// ������ �Ÿ� �����Ͽ� �������� ���� �� ���, 0�� ���� ä���
			sort_list(1);
			break;
		case 's':							// ������ �Ÿ� �����Ͽ� �������� ���� �� ���, 0�� ���� ä���
			sort_list(0);
			break;
		case 'q':							// ���α׷� ����
			q++;
			break;
		default:
			break;
		}

	}
	return 0;
}
void found_top_bottom() {
	for (int i = 0; i < 10; i++) {
		if (list_in[i] != 0) {
			bottom = i;
			break;
		}
		else if (i == 9) {
			bottom = 0;
		}
	}
	for (int i = 9; i >= 0; i--) {
		if (list_in[i] != 0) {
			top = i + 1;
			break;
		}
		else if (i == 0) {
			top = 0;
		}
	}
}
void print_list() {
	for (int i = 9; i >= 0; i--) {
		cout << i << ' ';
		if (list_in[i] == 1) {
			cout << point[i].x << ' ' << point[i].y << ' ' << point[i].z;
		}
		cout << '\n';
	}
}
void input(int x, int y, int z, int updown) {
	if (updown == 0) {							// command: +
		if (top < 10) {
			point[top].x = x;
			point[top].y = y;
			point[top].z = z;
			list_in[top] = 1;
			count_list++;
		}
		else {
			point[bottom].x = x;
			point[bottom].y = y;
			point[bottom].z = z;
			list_in[bottom] = 1;
			count_list++;
		}
	}
	else {										// command: e
		if (list_in[0] == 0) {
			point[0].x = x;
			point[0].y = y;
			point[0].z = z;
			list_in[0] = 1;
			count_list++;
		}
		else {
			for (int i = top; i > 0; i--) {
				point[i] = point[i - 1];
				list_in[i] = list_in[i - 1];
			}
			point[0].x = x;
			point[0].y = y;
			point[0].z = z;
			list_in[0] = 1;
			count_list++;
		}
	}
}
void output(int updown) {
	if (updown == 0) {						// command: - 
		if (count_list > 0) {
			point[top - 1].x = 0;
			point[top - 1].y = 0;
			point[top - 1].z = 0;
			list_in[top - 1] = 0;
			count_list--;
		}
	}
	else {									// command: d
		if (count_list > 0) {
			point[bottom].x = 0;
			point[bottom].y = 0;
			point[bottom].z = 0;
			list_in[bottom] = 0;
			count_list--;
		}
	}
}
void found_point(int far) {
	float num = 0;
	float max = 0;
	float min = 1000000;
	Point numpoint;
	if (far == 1) {			// ���� �� �Ÿ� �� ��ǥ
		for (int i = 0; i < 10; i++) {
			if (list_in[i] != 0) {
				num = sqrt(point[i].x * point[i].x + point[i].y * point[i].y + point[i].z * point[i].z);
				if (max < num)
				{
					max = num;
					numpoint = point[i];
				}
			}
		}
		cout << '(' << numpoint.x << ',' << numpoint.y << ',' << numpoint.z << ')' << '\n';
	}
	else {					// ���� ����� �Ÿ� �� ��ǥ
		for (int i = 0; i < 10; i++) {
			if (list_in[i] != 0) {
				num = sqrt(point[i].x * point[i].x + point[i].y * point[i].y + point[i].z * point[i].z);
				if (min > num)
				{
					min = num;
					numpoint = point[i];
				}
			}
		}
		cout << '(' << numpoint.x << ',' << numpoint.y << ',' << numpoint.z << ')' << '\n';
	}
}
void sort_list(int k) {
	Sort_point sort_point[10];
	int numcount = 0;
	if (return_sort == 0) {
		for (int i = 0; i < 10; i++) {
			save[i] = point[i];
			save_in[i] = list_in[i];
		}
		for (int i = 0; i < 10; i++) {
			if (list_in[i] != 0) {
				sort_point[numcount].length = sqrt(point[i].x * point[i].x + point[i].y * point[i].y + point[i].z * point[i].z);
				sort_point[numcount].x = point[i].x;
				sort_point[numcount].y = point[i].y;
				sort_point[numcount].z = point[i].z;
				numcount++;
			}
		}
		if (k == 1)
			sort(sort_point, sort_point + numcount, upSort);
		else
			sort(sort_point, sort_point + numcount, downSort);
		for (int i = 0; i < 10; i++) {
			point[i].x = 0;
			point[i].y = 0;
			point[i].z = 0;
			list_in[i] = 0;
			count_list = 0;
		}
		for (int i = 0; i < numcount; i++) {
			point[i].x = sort_point[i].x;
			point[i].y = sort_point[i].y;
			point[i].z = sort_point[i].z;
			list_in[i] = 1;
			count_list++;
		}
		return_sort++;
	}
	else {
		for (int i = 0; i < 10; i++) {
			point[i] = save[i];
			list_in[i] = save_in[i];
		}
		return_sort--;
	}
}