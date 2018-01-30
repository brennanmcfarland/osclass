// Chris Fietkiewicz (cxf47)
// Stays busy with nested for loops
#include <stdio.h>

int main() {
	int integer = 356;
	const int constant = 356;
	printf("integer: %p constant: %p\n", &integer, &constant);
	sleep(100);
	return 0;
}

