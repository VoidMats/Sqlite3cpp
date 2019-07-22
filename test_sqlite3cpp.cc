#include <iostream>

#include "sqlite3cpp.h"

int main()
{
    try
    {
        // Don't run Valgrind in-memory database leak memory
        Sqlite db(":memory:", false);  // Open an in-memory database
                                        // Will auto close on destruction
        //sqlite::statement_ptr s = db.get_statement();
        //s->set_sql("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)");
        std::cout << "== Create table 'test' which has text ==" << std::endl;
        db.exec("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)"); //exec calls prepare and step at once.
        //db.setQuery("CREATE TABLE test(id INTEGER PRIMARY KEY, text TEXT)");
        //db.prepare();
        //db.step();
        //db.reset();

        // When a shared_ptr like statement_pt is assigned another value, it deletes 
        // its contents. So s = db.get_statement(); Would do almost the same, except
        // it would clear its bindings, which are NOT done now! As we don't have bindings,
        // we don't care right now.
        //s->reset();

        // INSERT example without bind.
        //db.setQuery("INSERT INTO test(text) VALUES('test')");
        std::cout << "== Inserted into test using exec() ==" << std::endl;
        db.exec("INSERT INTO test(text) VALUES('test')");

        std::cout << "== Second inserted into test using exec() ==" << std::endl;
        db.exec("INSERT INTO test(text) VALUES('test2')");
        // Reset the statement again.
        //s->reset();

        std::string special_text = "text with ap'ostrophes and sla/shes in both \\ \
            directions are handled perfectly with bind!";

        std::cout << "== Inserted into test with binding values. ==" << std::endl;
        db.setQuery("INSERT INTO test(text) VALUES(?)");
        db.prepare();
        db.bind(1, special_text); // sqlite starts counting at 0
        db.step();
        
        //Another reset. Bear in mind that bindings are NOT cleared.
        db.reset();

        std::cout << "== Inserted into test with binding values to integer literal. ==" << std::endl;
        db.setQuery("INSERT into test(text) VALUES(?5)");
        db.prepare();
        db.bind(5, "Some other text ");   // If we specify an integer literal after the
                                    // question mark, we can use that number, which is
                                    // very useful in longer queries.
        db.step();
        db.reset();

        std::cout << "== Inserted into test with reuse of sql. ==" << std::endl;
        db.prepare(); // You can reuse your sql of the previous query.
        db.bind(5, "other test, reuse text");
        db.step();
        db.reset(); // We know what this does...

        // SELECT example.
        std::cout << "== Selext query ==" << std::endl;
        db.setQuery("SELECT id, text FROM test");
        db.prepare();
        std::cout << "Retrieve values:" << std::endl;
        while(db.step())
        {
            std::cout << db.getInt(0) << " " << db.getText(1) << std::endl;
            //std::cout << db.getInt(0) << " " << db.getBlob(1) << std::endl;
        }
    }
    catch(SqliteException e)
    {
        std::cerr << "Exception was cought: " << e.what() << std::endl;
    }
    return 0;
}