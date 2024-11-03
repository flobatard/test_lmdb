#ifndef LMDB_WRAPPER_H
#define LMDB_WRAPPER_H

#include <string>
#include "lmdb.h"

class LMDBWrapper
{
    public:
        LMDBWrapper(const std::string& dirPath);
        ~LMDBWrapper();
        std::string get(const std::string& key);
        int put(const std::string& key, const std::string& value);
        int remove(const std::string& key);
        int begin_transaction(bool read_only = false);
        int end_transaction();

        MDB_dbi mdb_dbi;
        MDB_env* mdb_env;
        MDB_cursor* mdb_cursor;
        MDB_txn* mdb_transaction;

};

#endif //LMDB_WRAPPER_H