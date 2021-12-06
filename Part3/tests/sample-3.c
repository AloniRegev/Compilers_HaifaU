#include <stdio.h>

void main() {
	int a;
	a = 3;

	switch (1) {
	case 1:
	{
		printf("%d\n", 13 + 12 - 5);
		for (a =  3; a < (2 * 3 * 5 * 2); a++) {
			printf("%d\n", a);
			switch (a)
			{
			case 3:
			{
				printf("%d\n", 2);
				for (a =  3; a < (2 * 3 * 5 * 2); a++) {
					printf("%d\n", a);
					switch (2)
					{
					case 2:
					{
						printf("%d\n", 2);
						break;
					}
					}
					break;
				}
				break;
			}
				break;
			}
			printf("%d\n", 999);

			break;
		}
		break;
	}
	case 2:
	{
		printf("%d\n", 2);
		break;
	}
	}
	
	printf("%d\n", 1);




	return;
}
