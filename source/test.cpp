#include <iostream>

#include "alloc.h"
#include "iterator.h"

void print(mystl::__true_type)
{
    std::cout << "YES" << std::endl;
}

int main()
{
    print(mystl::__type_traits<int>::is_POD_type());

    return 0;
}