#include"LMDB_wrapper.h"
#include<iostream>

#include<lmdb.h>
#include <string>
#include <filesystem>


std::string sep = 
    #ifdef _WIN32
        "\\";
    #else
        "/";
    #endif

std::string baseDir = "data";

using namespace std;

string LMDBWrapper::get(const string& key)
{
    return "";
}

int LMDBWrapper::put(const string& key, const string& value)
{
    return 1;
}

int LMDBWrapper::remove(const string& key)
{
    return 1;
}

LMDBWrapper::LMDBWrapper(const string& dirPath)
{
    
    try{
        
        if(std::filesystem::create_directory(baseDir + sep + dirPath))
        {
            std::cout << "Created a directory " << dirPath << endl;
        }
    }catch(const std::exception& e){
        std::cerr << "Error on directory creation: " << dirPath << " " << e.what() << endl;
    }
    if (const int rc = mdb_env_create(&mdb_env))
    if (mdb_env_set_maxdbs(mdb_env, 50))
    {
        std::cerr << "Error Mapsize: " << endl;   
    }
    if (mdb_env_set_mapsize(mdb_env, (size_t)1048576 * (size_t)100000))// 1MB * 100000
    {
        std::cerr << "Error Mapsize: " << endl;
    }; 
    if (const int rc = mdb_env_open(mdb_env, (baseDir + sep + dirPath).c_str(), 0, 0664))
    {
        std::cerr << "Error creating env: " << mdb_strerror(rc) << endl;
    }

    

    MDB_txn *parentTransaction = NULL;
    if (const int rc = mdb_txn_begin(mdb_env, parentTransaction, 0, &mdb_transaction)) {
        std::cerr << "Error creating transaction" << mdb_strerror(rc) << endl;
    }
    
    if (const int rc = mdb_dbi_open(mdb_transaction, "files", MDB_CREATE, mdb_dbi))
    {
        std::cerr << "Error opening database: " << mdb_strerror(rc) << endl;
    }

    std::cout << "Database launched " << dirPath << endl;
}

LMDBWrapper::~LMDBWrapper()
{
    cout << "Destroyed" <<endl;
}