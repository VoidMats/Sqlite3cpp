#ifndef SQLITE3CPP_H
#define SQLITE3CPP_H
// C++ includes
#include <iostream>
#include <memory>
#include <string>
// Library includes
#include <sqlite3.h>


class SqliteException : public std::exception
{
public:
    SqliteException(int no, const std::string& msg) 
        :number{no}, msg{msg} {}

    virtual const char* what() const noexcept {
        return this->msg.c_str();
    }

    int getNumber() {
        return this->number;
    }
private:
    int number;
    std::string msg;
};


class Sqlite
{
public:
    // Constructor, Destructor, Copy & Move
    Sqlite(std::string file, bool debug) 
        :file{file}, db{}, debug{debug}, prepared{false}, valid{true}, 
        rows_left{false}, query{""}, tail{""}, stmt{NULL}
    { 
        if(debug) std::cout << "Open database: " << file.c_str() << std::endl;
        int rc = sqlite3_open(file.c_str(), &this->db);
        if(rc != SQLITE_OK) { 
            std::string error_msg = "Can't open '" + file + "' : "
                + std::string(sqlite3_errmsg(this->db));
            SqliteException e(rc, error_msg);
            throw e;
        }
    }
    ~Sqlite() {
        sqlite3_finalize(this->stmt);
        sqlite3_close(this->db);
    }
    // TODO This does not work due to pointers
    Sqlite(Sqlite const& copy) = default;
    Sqlite &operator = (const Sqlite &copy) = default;
    Sqlite(Sqlite &&move) = default;
    Sqlite &operator = (Sqlite &&move) = default;

    void exec(std::string q) {
        // alt sqlite3_exec(this->db, q, 0, 0, 0);
        setQuery(q);
        prepare();
        step();
        reset();
    }

    void setQuery(std::string const& q) {
        if(debug) std::cout << "Set query: " << q << std::endl;
        if(this->prepared || q == "") {
            SqliteException e(-1, "Can not set sql on prepared query or the query is empty");
            throw e;
        } else {
            this->query = q;
        }
    }

    void prepare() {
        if(this->query != "") {
            if(debug) std::cout << "Prepare query" << std::endl;
            const char* tail;
            int rc = sqlite3_prepare_v2(
                this->db, 
                this->query.c_str(), 
                this->query.length(), 
                &this->stmt, 
                &tail);
            if(rc != SQLITE_OK) {
                SqliteException e(rc, "Could not prepare query: " + std::string(sqlite3_errmsg(this->db)));
                throw e;
            }
            this->prepared = true;
            this->tail = std::string(tail);
        } else {
            SqliteException e(-1, "No query set" );
            throw e;
        }
        
    }

    bool step() {
        if(debug) std::cout << "Step query" << std::endl;
        if(!this->valid) {
            SqliteException e(-1, "Trying to step an invalid statement.");
        }
        int rc = sqlite3_step(this->stmt);
        bool return_value = NULL;
        switch(rc){ 
            case SQLITE_DONE: {
                this->valid = false;
                return_value = false;
                break;
            }
            case SQLITE_ROW: {
                this->rows_left = true;
                return_value = true;
                break;
            }
            default:
                SqliteException e(rc, "Sqlite had an error: " + std::string(sqlite3_errmsg(this->db)));
        }
        return return_value;
    }

    void reset() {
        if(debug) std::cout << "Reset query" << std::endl;
        int rc = sqlite3_reset(this->stmt);
        if(rc != SQLITE_OK) {
            SqliteException e(rc, "Could not reset the query: "  + std::string(sqlite3_errmsg(this->db)));
            throw e;
        }
        this->valid = true;
        this->rows_left = false;
        this->prepared = false;
    }

    double getDouble(int fieldnumber)
    {
        return sqlite3_column_double(this->stmt, fieldnumber);
    }

    int getInt(int fieldnumber)
    {
        return sqlite3_column_int(this->stmt, fieldnumber);
    }

    std::string getText(int fieldnumber)
    {
        return std::string(reinterpret_cast<const char*>(sqlite3_column_text(this->stmt, fieldnumber)));
    }

    std::string getBlob(int fieldnumber)
    {
        return std::string(reinterpret_cast<const char*>(sqlite3_column_blob(this->stmt, fieldnumber), 
                sqlite3_column_bytes(this->stmt, fieldnumber)));
    }

    // Bind functions 
    void bind(int column, std::string const& text)
    {
        int rc = sqlite3_bind_text(
            this->stmt, 
            column, 
            text.c_str(), 
            text.length(), 
            SQLITE_TRANSIENT); //SQLITE_STATIC
        if(rc != SQLITE_OK) {
            SqliteException e(rc, "Could not bind text: " + std::string(sqlite3_errmsg(this->db)));
            throw e;
        }
    }

    void bind(int column, double const& d)
    {
        int rc = sqlite3_bind_double(this->stmt, column, d);
        if(rc != SQLITE_OK) {
            SqliteException e(rc, "Could not bind double: " + std::string(sqlite3_errmsg(this->db)));
            throw e;
        }
    }

    void bind(int column, int i)
    {
        int rc = sqlite3_bind_int(this->stmt, column, i);
        if(rc != SQLITE_OK)
        {
            SqliteException e(rc, "Could not bind int: " + std::string(sqlite3_errmsg(this->db)));
            throw e;
        }
    }

    void bind_null(int column) {
        int rc = sqlite3_bind_null(this->stmt, column);
        if(rc != SQLITE_OK) {
            SqliteException e(rc, "Could not bind to NULL: " + std::string(sqlite3_errmsg(this->db)));
            throw e;
        }
    }

    int64_t lastInsertId() {
        return sqlite3_last_insert_rowid(this->db);
    }

private:
    
    std::string file;
    sqlite3* db = NULL;
    bool debug;
    // Statement variables
    bool prepared, valid, rows_left;
    std::string query;
    std::string tail;
    sqlite3_stmt* stmt = NULL;
};

typedef std::shared_ptr<Sqlite> sqlite_ptr;

#endif //SQLITE3CPP_H
