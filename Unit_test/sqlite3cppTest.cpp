#include "../sqlite3cpp.h"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("Sqlite3cpp: Function test", "[Function]")
{
    Sqlite db(":memory", false);

    SECTION("Test setQuery(), without prepared -> normal")
    {
        REQUIRE_NOTHROW(db.setQuery("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)"));
        //CHECK_NOTHROW("Can not set sql on prepared query or the query is empty");
    }
    SECTION("Test setQuery(), with empty query -> fail")
    {   
        REQUIRE_THROWS_AS(db.setQuery(""), SqliteException);
        REQUIRE_THROWS_WITH(db.setQuery(""), "Can not set sql on prepared query or the query is empty");

    }
    SECTION("Test set Query(), with already prepared -> fail")
    {
        db.setQuery("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)");
        db.prepare();
        //db.setQuery("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)");
        REQUIRE_THROWS_AS(db.setQuery("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)"), SqliteException);
        REQUIRE_THROWS_WITH(db.setQuery("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)"), "Can not set sql on prepared query or the query is empty");
    }
}

/* 
TEST_CASE("Sqlite3cpp: Read and Write TEXT to sqlite3", "[TEXT]")
{
    Sqlite db(":memory:", false);

    SECTION("CREATE TABLE with on column:text")
    {
        db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)");

        CHECK_NOTHROW()

        SECTION("INSERT text by using exec()")
        {

        }

    }
}
*/