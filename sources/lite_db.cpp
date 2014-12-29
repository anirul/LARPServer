#include "lite_db.hpp"

db_key_value::db_key_value(
    const std::string& file_name,
    const std::string& table_name) :
    db_(NULL),
    file_name_(""),
    table_name_(table_name)
{
    open(file_name);
}

db_key_value::~db_key_value() {
    try {
        sqlite3_close(db_);
        db_ = NULL;
    } catch (std::exception& ex) {
        throw ex;
    }
}

void db_key_value::open(const std::string& file_name) {
    file_name_ = std::string(file_name);
    int rc = 0;
    rc = sqlite3_open_v2(
    file_name_.c_str(),
    &db_,
    SQLITE_OPEN_READWRITE |
    SQLITE_OPEN_CREATE |
    SQLITE_OPEN_FULLMUTEX |
    SQLITE_OPEN_SHAREDCACHE,
    NULL);
    if (rc) {
        std::stringstream error("");
        error << "Can't open database (full mutex, err = " << rc << " : ";
        error << sqlite3_errmsg(db_);
        sqlite3_close(db_);
        db_ = NULL;
        throw std::runtime_error(error.str());
    }
    create_table();
}

void db_key_value::create_table() {
    int rc = 0;
    char* szErrMsg = 0;
    std::stringstream ss;
    ss << "CREATE TABLE IF NOT EXISTS ";
    ss << table_name_;
    ss << "(key TEXT PRIMARY KEY, value TEXT)";
    rc = sqlite3_exec(
        db_,
        ss.str().c_str(),
        NULL,
        0,
        &szErrMsg);
    if (rc != SQLITE_OK) {
        std::stringstream ss("");
        ss << "SQL error in CREATE table : ";
        ss << szErrMsg;
        sqlite3_free(szErrMsg);
        throw std::runtime_error(ss.str());
    }
}

void db_key_value::clear() {
    int rc = 0;
    char* szErrMsg = 0;
    {
        std::stringstream ss;
        ss << "DROP TABLE IF EXISTS ";
        ss << table_name_;
        rc = sqlite3_exec(
            db_,
            ss.str().c_str(),
            NULL,
            0,
            &szErrMsg);
        if (rc != SQLITE_OK) {
            std::stringstream ss("");
            ss << "SQL error in DROP table : ";
            ss << szErrMsg;
            sqlite3_free(szErrMsg);
            throw std::runtime_error(ss.str());
        }
    }
    create_table();
}

std::string db_key_value::find(const std::string& key) {
    std::stringstream ss("");
    ss << "SELECT value FROM ";
    ss << table_name_;
    ss << " WHERE key = '";
    ss << key << "'";
    int rc = 0;
    char** result;
    int nrow, ncol;
    char* szMsg;
    rc = sqlite3_get_table(
        db_,
        ss.str().c_str(),
        &result,
        &nrow,
        &ncol,
        &szMsg);
    if (rc != SQLITE_OK) {
        std::stringstream ss("");
        ss << "SQL error in SELECT value : ";
        ss << szMsg;
        sqlite3_free(szMsg);
        sqlite3_free_table(result);
        throw std::runtime_error(ss.str());
    }
    std::string ret = "";
    if (ncol != 1) return ret;
    if (nrow == 1) ret = result[1];
    sqlite3_free_table(result);
    return ret;
}

void db_key_value::update(const std::string &key, const std::string &value) {
    // FIXME do it the correct way
    remove(key);
    insert(key, value);
}

void db_key_value::remove(const std::string& key) {
    std::stringstream ss("");
    ss << "DELETE FROM ";
    ss << table_name_;
    ss << " WHERE key = '";
    ss << key << "'";
    int rc = 0;
    char* szMsg;
    rc = sqlite3_exec(
        db_,
        ss.str().c_str(),
        NULL,
        0,
        &szMsg);
    if (rc != SQLITE_OK) {
        std::stringstream ss("");
        ss << "SQL error in DELETE : ";
        ss << szMsg;
        sqlite3_free(szMsg);
        throw std::runtime_error(ss.str());
    }
}

void db_key_value::insert(
const std::string& key,
const std::string& value)
{
    std::stringstream ss("");
    ss << "INSERT INTO ";
    ss << table_name_;
    ss << " VALUES('";
    ss << key << "', '";
    ss << value << "')";
    int rc = 0;
    char* szMsg;
    rc = sqlite3_exec(
        db_,
        ss.str().c_str(),
        NULL,
        0,
        &szMsg);
    if (rc != SQLITE_OK) {
        std::stringstream ss("");
        ss << "SQL error in INSERT table : ";
        ss << szMsg;
        sqlite3_free(szMsg);
        throw std::runtime_error(ss.str());
    }
}

size_t db_key_value::size() {
    int rc = 0;
    char** result;
    int nrow, ncol;
    char* szMsg;
    std::stringstream ss;
    ss << "SELECT Count(*) FROM ";
    ss << table_name_;
    rc = sqlite3_get_table(
        db_,
        ss.str().c_str(),
        &result,
        &nrow,
        &ncol,
        &szMsg);
    if (rc != SQLITE_OK) {
        std::stringstream ss("");
        ss << "SQL error in SELECT Count(*) : ";
        ss << szMsg;
        sqlite3_free(szMsg);
        sqlite3_free_table(result);
        throw std::runtime_error(ss.str());
    }
    if (ncol != 1) return 0;
    if (nrow != 1) return 0;
    return (size_t)atoi(result[1]);
}

void db_key_value::list(std::map<std::string, std::string>& mm) {
    int rc = 0;
    char** result;
    int nrow, ncol;
    char* szMsg;
    std::stringstream ss;
    ss << "SELECT * FROM ";
    ss << table_name_;
    rc = sqlite3_get_table(
        db_,
        ss.str().c_str(),
        &result,
        &nrow,
        &ncol,
        &szMsg);
    if (rc != SQLITE_OK) {
        std::stringstream ss("");
        ss << "SQL error in SELECT * : ";
        ss << szMsg;
        sqlite3_free(szMsg);
        sqlite3_free_table(result);
        throw std::runtime_error(ss.str());
    }
    if (ncol != 2) return;
    for (int i = 2; i < (ncol * (nrow + 1)); i+= 2) {
        mm.insert(
        std::make_pair<std::string, std::string>(
        result[i],
        result[i + 1]));
    }
    sqlite3_free_table(result);
}

void db_key_value::list_value(std::list<std::string>& l) {
    int rc = 0;
    char** result;
    int nrow, ncol;
    char* szMsg;
    std::stringstream ss;
    ss << "SELECT value FROM ";
    ss << table_name_;
    rc = sqlite3_get_table(
        db_,
        ss.str().c_str(),
        &result,
        &nrow,
        &ncol,
        &szMsg);
    if (rc != SQLITE_OK) {
        std::stringstream ss("");
        ss << "SQL error in SELECT value : ";
        ss << szMsg;
        sqlite3_free(szMsg);
        sqlite3_free_table(result);
        throw std::runtime_error(ss.str());
    }
    if (ncol != 1) return;
    for (int i = 1; i < (nrow + 1); ++i)
        l.push_back(result[i]);
    sqlite3_free_table(result);
}
