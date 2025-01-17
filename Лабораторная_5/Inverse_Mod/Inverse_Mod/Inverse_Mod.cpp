// Inverse_Mod.cpp: определяет точку входа для консольного приложения.
//



#include "stdafx.h"
using namespace std;

int main() {
	setlocale(LC_ALL, "Russian");
	int inverse(int, int);
	int a, b;
	cout << "Введите число\n";
	cin >> a;
	cout << "Введите модуль\n";
	cin >> b;
	cout << "Обратное значение: " << inverse(a, b) << endl;
}

	int ExtendedEuclidAlgGCD(int a, int b, int & s, int & t) {
		int s1 = 0, s2 = 1, t1 = 1, t2 = 0, q, r;
		while (a % b != 0) {
			q = a / b;
			r = a % b;
			s = s2 - q * s1;
			t = t2 - q * t1;

			s2 = s1;
			s1 = s;
			t2 = t1;
			t1 = t;
			a = b;
			b = r;
		}

		return r;
	}

	/* n>1, a is nonnegative
	a <= n
	a and n are relative prime to each other
	return s such that a*s mod n is 1 */
	int inverse(int a, int n) {
		int s, t;
		int d = ExtendedEuclidAlgGCD(n, a, s, t);
		int mod(int, int);

		if (d == 1) {
			return (mod(t, n)); // t might be negative, use mod() to reduce to 
								// an uinteger between 0 and n-1 
		}
		else {
			cout << "a,n are not relative prime!\n";
			return 0;
		}
	}




int mod(int a, int b) {
	int div, mod;

	if (b <= 1) {
		cout << "Модуль должен быть больше  1." << endl;
		return 0;
	}
	// Recall that -1 mod 3 = 2 as -1= (-1)*3+2.
	// This if-else is necessary for an int implementation, note this version uses int
	//if (a >= 0) {
	div = a / b;
	mod = a - (b * div);
	return mod;
}