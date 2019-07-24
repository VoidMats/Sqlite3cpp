#include "../sqlite3cpp.h"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("Sqlite3cpp: Function test", "[Function]")
{
    Sqlite db(":memory:", false);

    // Test setQuery()
    SECTION("Test setQuery(), without prepared -> normal")
    {
        REQUIRE_NOTHROW(db.setQuery("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)"));
    }
    SECTION("Test setQuery(), with empty query -> fail")
    {   
        REQUIRE_THROWS_AS(db.setQuery(""), SqliteException);
        REQUIRE_THROWS_WITH(db.setQuery(""), "Can not set sql on prepared query or the query is empty");
    }
    SECTION("Test setQuery(), with already prepared -> fail")
    {
        db.setQuery("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)");
        db.prepare();
        REQUIRE_THROWS_AS(db.setQuery("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)"), SqliteException);
        REQUIRE_THROWS_WITH(db.setQuery("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)"), "Can not set sql on prepared query or the query is empty");
    }
    // Test prepare()
    SECTION("Test prepare(), with empty query -> fail")
    {
        REQUIRE_THROWS_AS(db.prepare(), SqliteException);
        REQUIRE_THROWS_WITH(db.prepare(), "No query set");
    }
    SECTION("Test prepare(), with normal query")
    {
        db.setQuery("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)");
        REQUIRE_NOTHROW(db.prepare());
    }
    SECTION("Test prepare(), with faulty query")
    {
        db.setQuery("CREATE TABLES test(id INTEGER, text TEXT)");
        REQUIRE_THROWS_AS(db.prepare(), SqliteException);
        REQUIRE_THROWS_WITH(db.prepare(), "Could not prepare query: near \"TABLES\": syntax error");
    }
    // Test step()
    SECTION("Test step(), with normal query")
    {
        db.setQuery("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)");
        db.prepare();
        REQUIRE_NOTHROW(db.step());
    }
    SECTION("Test step(), with invalid statement")
    {
        db.setQuery("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)");
        db.prepare();
        db.step();
        //REQUIRE_THROWS_AS(db.step(), SqliteException);
        //REQUIRE_THROWS_WITH(db.step(), "Trying to step an invalid statement.");
    }
    SECTION("Test step(), with normal query, two steps")
    {

    }
    SECTION("Test reset(), normal reset")
    {
        db.setQuery("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)");
        db.prepare();
        db.step();
        REQUIRE_NOTHROW(db.reset());
    }
    SECTION("Test exec(), normal execute")
    {
        REQUIRE_NOTHROW(db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)"));
    }
}

TEST_CASE("Sqlite3cpp: Read and Write TEXT to sqlite3", "[TEXT]")
{
    Sqlite db(":memory:", false);

    SECTION("CREATE TABLE with on column:text")
    {
        REQUIRE_NOTHROW(db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)"));
    }
    SECTION("INSERT text by using exec()")
    {
        REQUIRE_NOTHROW(db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)"));
        REQUIRE_NOTHROW(db.exec("INSERT INTO test(text) VALUES('test')"));
    }
    SECTION("INSERT text two times using exec()")
    {
        REQUIRE_NOTHROW(db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)"));
        REQUIRE_NOTHROW(db.exec("INSERT INTO test(text) VALUES('test')"));
        REQUIRE_NOTHROW(db.exec("INSERT INTO test(text) VALUES('test2')"));
    }
    SECTION("INSERT text by using bind")
    {
        REQUIRE_NOTHROW(db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)"));
        REQUIRE_NOTHROW(db.setQuery("INSERT INTO test(text) VALUES(?)"));
        REQUIRE_NOTHROW(db.prepare());
        REQUIRE_NOTHROW(db.bind(1, "test with binding"));
        REQUIRE_NOTHROW(db.step());
        REQUIRE_NOTHROW(db.reset());
    }
    SECTION("INSERT text by using binding and integer literal")
    {
        REQUIRE_NOTHROW(db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)"));
        REQUIRE_NOTHROW(db.setQuery("INSERT INTO test(text) VALUES(?7)"));
        REQUIRE_NOTHROW(db.prepare());
        REQUIRE_NOTHROW(db.bind(7, "test with literal binding"));
        REQUIRE_NOTHROW(db.step());
        REQUIRE_NOTHROW(db.reset());
    }
    SECTION("SELECT text from database")
    {
        REQUIRE_NOTHROW(db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)"));
        REQUIRE_NOTHROW(db.exec("INSERT INTO test(text) VALUES('test1')"));
        REQUIRE_NOTHROW(db.exec("INSERT INTO test(text) VALUES('test2')"));
        REQUIRE_NOTHROW(db.setQuery("SELECT * FROM test"));
        REQUIRE_NOTHROW(db.prepare());
        REQUIRE_NOTHROW(db.step());
        REQUIRE(db.getInt(0) == 1);
        REQUIRE(db.getText(1) == "test1");
        REQUIRE_NOTHROW(db.step());
        REQUIRE(db.getInt(0) == 2);
        REQUIRE(db.getText(1) == "test2");
    }
}

TEST_CASE("Sqlite3cpp: Read and Write INTEGER to sqlite3", "[INTEGER]")
{
    Sqlite db(":memory:", false);

    SECTION("CREATE TABLE with on column:integer")
    {
        REQUIRE_NOTHROW(db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, int INTEGER)"));
    }
    SECTION("INSERT integer by using exec()")
    {
        REQUIRE_NOTHROW(db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, int INTEGER)"));
        REQUIRE_NOTHROW(db.exec("INSERT INTO test(int) VALUES(23)"));
    }
    SECTION("INSERT integer two times using exec()")
    {
        REQUIRE_NOTHROW(db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, int INTEGER)"));
        REQUIRE_NOTHROW(db.exec("INSERT INTO test(int) VALUES(23)"));
        REQUIRE_NOTHROW(db.exec("INSERT INTO test(int) VALUES(34)"));
    }
    SECTION("INSERT integer by using bind()")
    {
        REQUIRE_NOTHROW(db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, int INTEGER)"));
        REQUIRE_NOTHROW(db.setQuery("INSERT INTO test(int) VALUES(?)"));
        REQUIRE_NOTHROW(db.prepare());
        REQUIRE_NOTHROW(db.bind(1, 45));
        REQUIRE_NOTHROW(db.step());
        REQUIRE_NOTHROW(db.reset());
    }
    SECTION("INSERT text by using binding and integer literal")
    {
        REQUIRE_NOTHROW(db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, int INTEGER)"));
        REQUIRE_NOTHROW(db.setQuery("INSERT INTO test(int) VALUES(?7)"));
        REQUIRE_NOTHROW(db.prepare());
        REQUIRE_NOTHROW(db.bind(7, 56));
        REQUIRE_NOTHROW(db.step());
        REQUIRE_NOTHROW(db.reset());
    }
    SECTION("SELECT text from database")
    {
        REQUIRE_NOTHROW(db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, int INTEGER)"));
        REQUIRE_NOTHROW(db.exec("INSERT INTO test(int) VALUES(23)"));
        REQUIRE_NOTHROW(db.exec("INSERT INTO test(int) VALUES(34)"));
        REQUIRE_NOTHROW(db.setQuery("SELECT * FROM test"));
        REQUIRE_NOTHROW(db.prepare());
        REQUIRE_NOTHROW(db.step());
        REQUIRE(db.getInt(0) == 1);
        REQUIRE(db.getInt(1) == 23);
        REQUIRE_NOTHROW(db.step());
        REQUIRE(db.getInt(0) == 2);
        REQUIRE(db.getInt(1) == 34);
    }
}

TEST_CASE("Sqlite3cpp: Read and Write REAL to sqlite3", "[REAL]")
{
    Sqlite db(":memory:", false);

    SECTION("CREATE TABLE with on column:real")
    {
        REQUIRE_NOTHROW(db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, real REAL)"));
    }
    SECTION("INSERT real by using exec()")
    {
        REQUIRE_NOTHROW(db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, real REAL)"));
        REQUIRE_NOTHROW(db.exec("INSERT INTO test(real) VALUES(2.3)"));
    }
    SECTION("INSERT real two times using exec()")
    {
        REQUIRE_NOTHROW(db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, real REAL)"));
        REQUIRE_NOTHROW(db.exec("INSERT INTO test(real) VALUES(2.3)"));
        REQUIRE_NOTHROW(db.exec("INSERT INTO test(real) VALUES(3.4)"));
    }
    SECTION("INSERT real by using bind()")
    {
        REQUIRE_NOTHROW(db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, real REAL)"));
        REQUIRE_NOTHROW(db.setQuery("INSERT INTO test(real) VALUES(?)"));
        REQUIRE_NOTHROW(db.prepare());
        REQUIRE_NOTHROW(db.bind(1, 4.5));
        REQUIRE_NOTHROW(db.step());
        REQUIRE_NOTHROW(db.reset());
    }
    SECTION("INSERT real by using binding and integer literal")
    {
        REQUIRE_NOTHROW(db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, real REAL)"));
        REQUIRE_NOTHROW(db.setQuery("INSERT INTO test(real) VALUES(?7)"));
        REQUIRE_NOTHROW(db.prepare());
        REQUIRE_NOTHROW(db.bind(7, 5.6));
        REQUIRE_NOTHROW(db.step());
        REQUIRE_NOTHROW(db.reset());
    }
    SECTION("SELECT real from database")
    {
        REQUIRE_NOTHROW(db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, real REAL)"));
        REQUIRE_NOTHROW(db.exec("INSERT INTO test(real) VALUES(2.3)"));
        REQUIRE_NOTHROW(db.exec("INSERT INTO test(real) VALUES(3.4)"));
        REQUIRE_NOTHROW(db.setQuery("SELECT * FROM test"));
        REQUIRE_NOTHROW(db.prepare());
        REQUIRE_NOTHROW(db.step());
        REQUIRE(db.getInt(0) == 1);
        REQUIRE(db.getDouble(1) == 2.3);
        REQUIRE_NOTHROW(db.step());
        REQUIRE(db.getInt(0) == 2);
        REQUIRE(db.getDouble(1) == 3.4);
    }
}
