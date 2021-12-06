#include <stdio.h>

struct B {
	int c;
	struct B* d;
	float* e;
};

void main() {
	float a[1];
	struct B b;
	struct B* f;
	b.c = 6;
	f = &(b);
	(*f).d = &(b);
	(*((*f).d)).e = &(a[0]);
	a[0] = 2.5;
	printf("%f\n", (*((*((*f).d)).e)) * 2.0);
	return;
}
