#include <stdio.h>

void main() {
	int a, b;
	a = 0 + 1;
	b = 0 * a;

	switch (a) {
	case 1:
	{
		if (18 - 3) {
			printf("%d\n", ((5 == 5) ? 1 + b : 2 + 1));
		}
		break;
	}
	case 2:
	{
		if (b && 0) {
			printf("%d\n", ((5 != 5) ? 1 + a : 2 + 1));
		}
		break;
	}
	case 3:
	{
		if (0 && b) {
			printf("%d\n", ((5 == 5) ? 1 + a * 0 : 2 + b * 0));
		}
		break;
	}
	case 4:
	{
		if (b) {
			printf("%d\n", ((5 != 5) ? 1 + a : 2 + b / 1));
		}
		break;
	}
	}
	a=2;
	switch (a) {
	case 1:
	{
		if (18 - 3) {
			printf("%d\n", ((5 == 5) ? 1 + b : 2 + 1));
		}
		break;
	}
	case 2:
	{
		if (b && 1) {
			printf("%d\n", ((5 != 5) ? 1 + a : 2 + 1));
		}
		break;
	}
	case 3:
	{
		if (1 && b) {
			printf("%d\n", ((5 == 5) ? 1 + a * 0 : 2 + b * 0));
		}
		break;
	}
	case 4:
	{
		if (b) {
			printf("%d\n", ((5 != 5) ? 1 + a : 2 + b / 1));
		}
		break;
	}
	}
	a=3;
	switch (a) {
	case 1:
	{
		if (18 - 3) {
			printf("%d\n", ((5 == 5) ? 1 + b : 2 + 1));
		}
		break;
	}
	case 2:
	{
		if (b && 1) {
			printf("%d\n", ((5 != 5) ? 1 + a : 2 + 1));
		}
		break;
	}
	case 3:
	{
		if (1 && a) {
			printf("%d\n", ((5 == 5) ? 1 + a * 0 : 2 + b * 0));
		}
		break;
	}
	case 4:
	{
		if (b) {
			printf("%d\n", ((5 != 5) ? 1 + a : 2 + b / 1));
		}
		break;
	}
	}
	a=4;
	switch (a) {
	case 1:
	{
		if (18 - 3) {
			printf("%d\n", ((5 == 5) ? 1 + b : 2 + 1));
		}
		break;
	}
	case 2:
	{
		if (b && 1) {
			printf("%d\n", ((5 != 5) ? 1 + a : 2 + 1));
		}
		break;
	}
	case 3:
	{
		if (1 && a) {
			printf("%d\n", ((5 == 5) ? 1 + a * 0 : 2 + b * 0));
		}
		break;
	}
	case 4:
	{
		if (b+1) {
			printf("%d\n", ((5 != 5) ? 1 + a : 2 + b / 1));
		}
		break;
	}
	}
	return;
}
