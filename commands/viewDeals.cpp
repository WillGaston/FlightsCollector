#include "./commands.h"
using namespace std;

void viewDeals() {
  
  std::cout << "\n" << "ID  | Dep | Des | Leave Date |  Ret Date  | Lowest Price \n";
  std::cout << "---------------------------------------------------------------\n";

  sqlite3 *db;
  if (sqlite3_open("./database/flightDatabase.db", &db) != SQLITE_OK) {
    std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_close(db);
    return;
  }
  

  std::string readQuery = "SELECT * FROM flights";
  sqlite3_stmt *readStatement;
  sqlite3_prepare(db, readQuery.c_str(), -1, &readStatement, nullptr); 

  while (sqlite3_step(readStatement) == SQLITE_ROW) {
    int numCols = sqlite3_column_count(readStatement);
    const unsigned char* textid = sqlite3_column_text(readStatement, 0);
    std::cout << "[" << (textid ? reinterpret_cast<const char*>(textid) : "NULL") << "]" << " | ";
    for (int i = 1; i < numCols - 1; i++) {
      const unsigned char* text = sqlite3_column_text(readStatement, i);
      std::cout << (text ? reinterpret_cast<const char*>(text) : "NULL") << " | ";
    }
    const unsigned char* textprice = sqlite3_column_text(readStatement, numCols - 1);
    std::cout << (textprice ? reinterpret_cast<const char*>(textprice) : "NULL");
    std::cout << "\n";
  }
  std::cout << "\n";
  
}