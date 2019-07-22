#include <iostream>

#include "sqlite3cpp.h"

int main()
{
    try
    {
        // !!! Don't run Valgrind on in-memory database it leak memory
        Sqlite db(":memory:", false);  // Open an in-memory database
        std::cout << "== Create table 'test' which has text ==" << std::endl;
        db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)"); //exec calls prepare and step at once.
    
        std::cout << "== Inserted into test using exec() ==" << std::endl;
        db.exec("INSERT INTO test(text) VALUES('test')");

        std::cout << "== Second inserted into test using exec() ==" << std::endl;
        db.exec("INSERT INTO test(text) VALUES('test2')");

        std::string special_text = "text with ap'ostrophes and sla/shes in both \\ \
            directions are handled perfectly with bind!";

        std::cout << "== Inserted into test with binding values. ==" << std::endl;
        db.setQuery("INSERT INTO test(text) VALUES(?)");
        db.prepare();
        db.bind(1, special_text);
        db.step();
        db.reset();

        std::cout << "== Inserted into test with binding values to integer literal. ==" << std::endl;
        db.setQuery("INSERT into test(text) VALUES(?5)");
        db.prepare();
        db.bind(5, "Some other text ");
        db.step();
        db.reset();

        std::cout << "== Inserted into test with reuse of sql. ==" << std::endl;
        db.prepare();
        db.bind(5, "other test, reuse query");
        db.step();
        db.reset();

        std::cout << "== Selext query ==" << std::endl;
        db.setQuery("SELECT id, text FROM test");
        db.prepare();
        std::cout << "Retrieve values:" << std::endl;
        while(db.step())
        {
            std::cout << db.getInt(0) << " " << db.getText(1) << std::endl;
        }
    }
    catch(SqliteException e)
    {
        std::cerr << "Exception was cought: " << e.what() << std::endl;
    }
    return 0;
}