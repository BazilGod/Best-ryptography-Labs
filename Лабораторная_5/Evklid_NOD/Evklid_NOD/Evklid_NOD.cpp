// Evklid_NOD.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include  <iostream>

using namespace std;

 int nod (int a, int b) {

	 cout << "Введите первое число\n";
	 cin >> a;
	 cout << "Введите второе число\n";
	 cin >> b;
	while (a != b) {
		if (a > b) {
			a = a - b;
		}
		else {
			b = b - a;
		}
	}
	return a;
}

int main(int a,int b)
{
	setlocale(LC_ALL, "Russian");
	cout << nod(a, b) << endl;
	return 0;
}