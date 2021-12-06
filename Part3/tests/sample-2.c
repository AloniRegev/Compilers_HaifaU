#include <stdio.h>

void main() {
	int a;
	a = 3;

	switch (1) {
	case 1:
	{
		printf("%d\n", 13 + 12 - 5);
		switch (a) {
		case 3:
		{
			printf("%d\n", 13 + 12 - 5);
			switch (1) {
			case 1:
			{
				printf("%d\n", 13 + 12 - 5);
				switch (a) {
				case 3:
				{
					printf("%d\n", 13 + 12 - 5);

					break;
				}
				}
				break;
			}
			}
			
			break;
		}
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
