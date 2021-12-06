#include <stdio.h>

void main() {
	double a;
	int b, * c;

	a = 5.3;
	b = a < 6.0;
	c = &(b);
	if (*c) {
		printf("%d\n", 6);
	}
	return;
}
