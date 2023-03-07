#include <iostream>

#include "alloc.h"

int main()
{
    mystl::alloc al;
    int * p = static_cast<int*>(al.allocate(10 * 4));
    for(int i = 0; i < 10; i ++)
        *(p + i) = i;
    for(int i = 0; i < 10; i ++)
        std::cout << *(p +i) << " ";
    std::cout << std::endl;
    return 0;
}