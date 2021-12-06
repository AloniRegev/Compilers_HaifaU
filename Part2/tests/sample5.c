#include <stdio.h>

void main() {
	int a[1], * b, ** c, *** d;
	b = &(a[0]);
	c = &(b);
	d = &(c);

	a[0] = 8;
	printf("%d\n", (***d));
	return;
}
