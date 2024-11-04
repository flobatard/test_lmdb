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

int LMDBWrapper::begin_transaction(bool read_only)
{
    dbi_mtx.lock();
    if(const int rc = mdb_txn_begin(mdb_env, NULL, (read_only ?  MDB_RDONLY : 0), &mdb_transaction)){
        throw runtime_error("Couldn't start LMDB txn mdb_txn_begin() returned " + string(mdb_strerror(rc)));
    }

    if(const int rc = mdb_dbi_open(mdb_transaction, 0, MDB_CREATE, &mdb_dbi))
    {
        throw runtime_error("Couldn't open LMDB zone database  mdb_dbi_open() returned " + string(mdb_strerror(rc)));
    }

    return 0;
}

int LMDBWrapper::commit()
{
    mdb_txn_commit(mdb_transaction);
    dbi_mtx.unlock();
    return 0;
}

int LMDBWrapper::abort()
{
    mdb_txn_abort(mdb_transaction);
    dbi_mtx.unlock();
    return 0;
}

int LMDBWrapper::end_transaction()
{
    return commit();
}

// Méthode pour insérer une paire clé-valeur
int LMDBWrapper::put(const std::string& key, const std::string& value) {
    begin_transaction();
    MDB_val mdb_key, mdb_value;
    mdb_key.mv_size = key.size();
    mdb_key.mv_data = (void*)key.c_str();
    mdb_value.mv_size = value.size();
    mdb_value.mv_data = (void*)value.c_str();

    if (const int rc = mdb_put(mdb_transaction, mdb_dbi, &mdb_key, &mdb_value, 0))
    {
        std::cerr << "Error putting data: " << mdb_strerror(rc) << std::endl;
        abort();
        throw runtime_error("Couldn't put LMDB zone database  mdb_put() returned " + string(mdb_strerror(rc)));
    }
    end_transaction();
    return 0;
}

// Méthode pour récupérer la valeur associée à une clé
std::string LMDBWrapper::get(const std::string& key) {
    begin_transaction();
    MDB_val mdb_key, mdb_value;
    mdb_key.mv_size = key.size();
    mdb_key.mv_data = (void*)key.c_str();

    int rc = mdb_get(mdb_transaction, mdb_dbi, &mdb_key, &mdb_value);
    if (rc == MDB_NOTFOUND) {
        abort();
        return "";  // Clé non trouvée
    } else if (rc != 0) {
        abort();
        throw runtime_error("Couldn't GET LMDB zone database  mdb_get() returned " + string(mdb_strerror(rc)));
    }

    end_transaction();
    std::string value(static_cast<char*>(mdb_value.mv_data), mdb_value.mv_size);
    return value;
}

// Méthode pour supprimer une paire clé-valeur
int LMDBWrapper::remove(const std::string& key) {
    begin_transaction();
    MDB_val mdb_key;
    mdb_key.mv_size = key.size();
    mdb_key.mv_data = (void*)key.c_str();


    int rc = mdb_del(mdb_transaction, mdb_dbi, &mdb_key, NULL);
    if (rc == MDB_NOTFOUND) {
        abort();
        return rc;
    } else if (rc != 0) {
        abort();
        throw runtime_error("Couldn't DELETE LMDB zone database  mdb_del() returned " + string(mdb_strerror(rc)));
        return rc;
    }

    end_transaction();
    return 0;
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
    std::cout << "Setting up Database env" << endl;
    if (const int rc = mdb_env_create(&mdb_env))
    {
        std::cerr << "Error creating env" << mdb_strerror(rc) << endl;
    }
    std::cout << "MDB ENV Created" << endl;
    if (const int rc = mdb_env_set_maxdbs(mdb_env, 50))
    {
        std::cerr << "Error Max DBS: " << mdb_strerror(rc) << endl;
    }
    if (const int rc = mdb_env_set_mapsize(mdb_env, (size_t)1048576 * (size_t)100000))// 1MB * 100000
    {
        std::cerr << "Error Mapsize: " <<  mdb_strerror(rc) << endl;
    }; 
    if (const int rc = mdb_env_open(mdb_env, (baseDir + sep + dirPath).c_str(), 0, 0664))
    {
        std::cerr << "Error creating env: " << mdb_strerror(rc) << endl;
    }
    std::cout << "Database launched " << dirPath << endl;
}

LMDBWrapper::~LMDBWrapper()
{
    mdb_dbi_close(mdb_env, mdb_dbi);
    mdb_txn_commit(mdb_transaction);
    mdb_env_close(mdb_env);
    cout << "Destroyed" <<endl;
}