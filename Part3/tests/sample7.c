#include <stdio.h>

void main() {
	int a, b, c;
	a = 5 * 2 + 5 * 2;
	b = 40 - 10;
	c = 4 * 4 * 4 * 4;
	if (5 + 5) {
		if (5 - 5) {
			printf("%d\n", ((5 == 5) ? 1+1 : 2 + 1));
		}
		if (5 + 5) {
			printf("%d\n", ((5 == 5) ? 3 + 1 : 4 + 1));
		}
		if (5 + 5) {
			printf("%d\n", ((5 != 5) ? 5 + 1 : 6 + 1));
		}
	}
	else {
		if (5 - 5) {
			printf("%d\n", ((5 == 5) ? 7 + 1 : 8 + 1));
		}
		if (5 + 5) {
			printf("%d\n", ((5 == 5) ? 9 + 1 : 10 + 1));
		}
		if (5 + 5) {
			printf("%d\n", ((5 != 5) ? 11 + 1 : 12 + 1));
		}
	}

	if (5 - 5) {
		if (5 - 5) {
			printf("%d\n", ((5 == 5) ? 1 + 1 : 2 + 1));
		}
		if (5 + 5) {
			printf("%d\n", ((5 == 5) ? 3 + 1 : 4 + 1));
		}
		if (5 + 5) {
			printf("%d\n", ((5 != 5) ? 5 + 1 : 6 + 1));
		}
	}
	else {
		if (a) {
			printf("%d\n", ((5 == 5) ? 7 + a : 8 + 1));
		}
		if (5 + 5) {
			printf("%d\n", ((5 == a) ? 9 + b : 10 + 1));
		}
		if (5 + 5) {
			printf("%d\n", ((5 != 5) ? 11 + 1 : 12 + c));
		}
	}
	return;
}