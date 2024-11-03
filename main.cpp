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
    LMDBWrapper lmdb = LMDBWrapper("db1");
    for (int i = 0; i < 10000; i++)
    {
        lmdb.put("toto" + std::to_string(i), "tata" + std::to_string(i));
        if (i%1000 == 0)
        {
            std::cout << lmdb.get("toto" + std::to_string(i)) << std::endl;
        }
    }
    std::cout << lmdb.get("toto1 REMOVE") << std::endl;
    for (int i = 0; i < 10000; i++)
    {
        if (i%1000 == 0)
        {
            std::cout << lmdb.get("toto" + std::to_string(i)) << std::endl;
        }
        lmdb.remove("toto" + std::to_string(i));
    }
    std::cout << lmdb.get("toto1 END") << std::endl;
    return 0;
}