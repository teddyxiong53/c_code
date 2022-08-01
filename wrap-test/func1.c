#include <stdlib.h>
#include <stdio.h>

void func1() {
    char *p = malloc(100);
    printf("p:%p\n", p);
    free(p);
}