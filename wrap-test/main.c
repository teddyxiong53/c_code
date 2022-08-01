#include <stdlib.h>
#include <stdio.h>

void* __real_malloc(size_t size); // 只声明不定义
 
void* __wrap_malloc(size_t size) 
{
    printf("__wrap_malloc called\n");
 
    return __real_malloc(size); //调用真正的malloc
}

int main(int argc, char const *argv[])
{
    extern void func1();
    func1();
    char *p1 = malloc(10);
    printf("p1:%p\n", p1);
    free(p1);
    return 0;
}
