#include "./commands.h"
using namespace std;

void removeSingle(int numArguments, char *flightNumber);
void removeAll(int numArguments);

void removeDeal(int numArguments, char *arguments[]) {
  
  if (strcmp(arguments[2], "-fn") == 0) {
    removeSingle(numArguments, arguments[3]);
  } else if (strcmp(arguments[2], "-all") == 0) {
    removeAll(numArguments);
  } else {
    std::cout << "Invalid Arguments\n";
  }

}

void removeSingle(int numArguments, char *flightNumber) {
  if (numArguments > 4) {
    std::cout << "Invalid Arguments\n";
  }

  std::string choice;
  std::cout << "Are you sure you would like to delete flight with id: " << flightNumber << " (Yes/No)\n";
  while (1) {
    cin >> choice;
    if (choice == "Yes" || choice == "No") break;
      std::cout << "Are you sure you would like to delete flight with id: " << flightNumber << " (Yes/No)\n";
  } 

  if (choice == "Yes") {
    sqlite3 *db;
    if (sqlite3_open("./database/flightDatabase.db", &db) != SQLITE_OK) {
      std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
      sqlite3_close(db);
      return;
    }
    
    std::string deleteQuery = "DELETE FROM flights WHERE flights.id = ?";
    sqlite3_stmt *deleteStatement;
    sqlite3_prepare(db, deleteQuery.c_str(), -1, &deleteStatement, nullptr); 

    sqlite3_bind_int(deleteStatement, 1, std::stoi(flightNumber));

    if (sqlite3_step(deleteStatement) != SQLITE_DONE) {
      std::cerr << "Failed to create table: " << sqlite3_errmsg(db) << std::endl;
      sqlite3_close(db);
      return;
    }

    std::cout << "Flights Deleted\n";
  }

}

void removeAll(int numArguments) {
  if (numArguments > 3) {
    std::cout << "Invalid Arguments\n";
  }

  std::string choice;
  std::cout << "Are you sure you would like to delete all saved flights? (Yes/No)\n";
  while (1) {
    cin >> choice;
    if (choice == "Yes" || choice == "No") break;
    std::cout << "Are you sure you would like to delete all saved flights? (Yes/No)\n";
  } 

  if (choice == "Yes") {
    sqlite3 *db;
    if (sqlite3_open("./database/flightDatabase.db", &db) != SQLITE_OK) {
      std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
      sqlite3_close(db);
      return;
    }
    
    std::string deleteAllQuery = "DELETE FROM flights";
    sqlite3_stmt *deleteAllStatement;
    sqlite3_prepare(db, deleteAllQuery.c_str(), -1, &deleteAllStatement, nullptr); 

    if (sqlite3_step(deleteAllStatement) != SQLITE_DONE) {
      std::cerr << "Failed to create table: " << sqlite3_errmsg(db) << std::endl;
      sqlite3_close(db);
      return;
    }

    std::cout << "Flights Deleted\n";
  }

}