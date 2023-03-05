#include <iostream>

#include "alloc.h"

int main()
{
    malloc_alloc al;
    void * p = al.allocate(10);
    *(int*)p = 5;
    std::cout << *(int*)p << std::endl;
    return 0;
}