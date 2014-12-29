#ifndef LITE_DB_HEADER_DEFINED
#define LITE_DB_HEADER_DEFINED

#include <fstream>
#include <map>
#include <sstream>
#include <list>
#include <thread>

#include <sqlite3.h>

class db_key_value {
private:
    sqlite3* db_;
    std::string file_name_;
    std::string table_name_;
public:
    db_key_value() = delete;
    db_key_value(const std::string& file_name, const std::string& table_name);
    virtual ~db_key_value();
public:
    void open(const std::string& file_name);
    void create_table();
    void clear();
    std::string find(const std::string& key);
    void remove(const std::string& key);
    void update(const std::string& key, const std::string& value);
    void insert(const std::string& key, const std::string& value);
    size_t size();
    // debug
    void list(std::map<std::string, std::string>& mm);
    void list_value(std::list<std::string>& l);
};

#endif // LITE_DB_HEADER_DEFINED
