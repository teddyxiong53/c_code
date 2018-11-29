#include <stdio.h>
#include "f1.h"
#include "f2.h"

int main()
{
	printf("hello cmake\n");
	#if DEBUG
	printf("hello define\n");
	#endif
	f1();
	f2();
	return 0;
}