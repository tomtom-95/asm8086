#include <stdlib.h>
#include <stdio.h>

void
test_dummy(void)
{
    int a = 0;
    int b = 1;
    int c = a + b;
}

int
main(void)
{
    test_dummy();
}