#include<iostream>
#include"LMDB_wrapper.h"
#ifdef _WIN32
    #define SEP "\\"
#else
    #define SEP "/"
#endif

int main()
{
    std::cout << "__cplusplus: " << __cplusplus;
    std::cout << std::endl;
    LMDBWrapper a = LMDBWrapper("db1");
    std::cout << "Hello world" << std::endl;
    
    return 0;
}