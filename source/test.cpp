#include <iostream>

#include "alloc.h"
#include "iterator.h"
#include "uninitialized.h"
#include "construct.h"

#include "vector.h"

void print(mystl::__true_type)
{
    std::cout << "YES" << std::endl;
}

void print(mystl::vector<int>& iv)
{
    for(auto x :iv) std::cout << x << std::endl;
    std::cout << std::endl;    
}


int main()
{


    mystl::vector<int>iv(2, 1);

    print(iv);

    iv.push_back(2);

    print(iv);

    iv.push_back(3);

    print(iv);

    iv.pop_back();



    int* pt = new int[5];

    for(int i = 0; i <5; i ++)
        pt[i] = i;
    
    for(int i = 0; i <5; i ++)
        std::cout << i << " ";
    std::cout << std::endl;

    mystl::destory(pt, pt + 5);



    return 0;
}