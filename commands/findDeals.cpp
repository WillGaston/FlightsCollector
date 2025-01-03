#include "./commands.h"
using namespace std;

extern char **environ;

int validateArguments(char *arguments[]);
int parsePrice(std::string price);

int findDeals(int numArguments, char *arguments[]) {

  if (numArguments != 10) {
    std::cout << "\nInvalid Arguments, must be of format:\n";
    std::cout << "-f -dep <departure airport> -dest <destination airport> -ld <YYYY-MM-DD> -rd <YYYY-MM-DD>   Find deals\n\n";
    std::cout << "For Example:\n";
    std::cout << "-f -dep SYD -dest SIN -ld 2025-02-12 -rd 2025-03-01\n\n";
    return EXIT_FAILURE;
  }

  if (validateArguments(arguments) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  setenv("DepENV", arguments[3], 0);
  setenv("DestENV", arguments[5], 0);
  setenv("LeaveENV", arguments[7], 0);
  setenv("ReturnENV", arguments[9], 0);

  const char* js_file = "./commands/dealsScript.js";

  const char* command = "/home/gaston/.nvm/versions/node/v20.17.0/bin/node";
  char* const args[] = { (char*)command, (char*)js_file, nullptr };

  pid_t pid;
  int status;
    
  if (posix_spawn(&pid, command, nullptr, nullptr, args, environ) != 0) {
    std::cerr << "Error spawning process: " << strerror(errno) << std::endl;
    return 1;
  }

  if (waitpid(pid, &status, 0) == -1) {
    std::cerr << "Error waiting for child process: " << strerror(errno) << std::endl;
    return 1;
  }

  unsetenv("DepENV");
  unsetenv("DestENV");
  unsetenv("LeaveENV");
  unsetenv("ReturnENV");

  displayFlights(arguments[3], arguments[5], arguments[7], arguments[9]);

  return EXIT_SUCCESS;

}

int validateArguments(char *arguments[]) {

  if (strcmp(arguments[2], "-dep") != 0 ||
      strcmp(arguments[4], "-dest") != 0 ||
      strcmp(arguments[6], "-ld") != 0 ||
      strcmp(arguments[8], "-rd") != 0) {

    std::cout << "Invalid Options Provided\n";
    return EXIT_FAILURE;
  }

  std::regex airportPattern("[A-Z]{3}");

  if (!regex_match(arguments[3], airportPattern) ||
      !regex_match(arguments[5], airportPattern)) {

    std::cout << "Invalid Airport Code Given\n";
    return EXIT_FAILURE;
  }

  std::regex datePattern("\\d{4}\\-\\d{2}\\-\\d{2}");

  if (!regex_match(arguments[7], datePattern) ||
      !regex_match(arguments[9], datePattern)) {

    std::cout << "Invalid Date Given\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void displayFlights(std::string dep, std::string dest, std::string ld, std::string rd) {
  std::ifstream flight_file("./tempFlightData/flightData.json", std::ifstream::binary);
  Json::Value flights;
  flight_file >> flights;

  int count = 1;

  std::string lowestPrice = flights[0]["price"].asString();

  for (const auto& flight : flights) {
    if (flight["price"] == "Price unavailable") continue;
    if (parsePrice(lowestPrice) > parsePrice(flight["price"].asString())) {
      lowestPrice = flight["price"].asString();
    }
  }

  std::cout << "\n";
  std::cout << "ID   |  Price  | Dep | Des | Leave Date |  Ret Date  | Directness | Airline \n";
  std::cout << "----------------------------------------------------------------------------------\n";

  for (const auto& flight : flights) {
    std::string airline = flight["airline"].asString();
    std::string price = flight["price"].asString();
    std::string directness = flight["directness"].asString();

    if (price == "Price unavailable") continue;
    if (!airline.find("Operated by")) continue;
    if (airline == "") airline = "Unavailable";

    if (parsePrice(price) < 1000) {
      std::cout << "[" + std::to_string(count) + "]  | " + price + "   | " + dep + " | " + dest + " | " + ld + " | " + rd + " | " + directness + " | " + airline + "\n";
    } else if (parsePrice(price) >= 10000 && count < 10) {
      std::cout << "[" + std::to_string(count) + "]  | " + "> A$10k" + " | " + dep + " | " + dest + " | " + ld + " | " + rd + " | " + directness + " | " + airline + "\n";
    } else if (parsePrice(price) >= 10000) {
      std::cout << "[" + std::to_string(count) + "] | " + "> A$10k" + " | " + dep + " | " + dest + " | " + ld + " | " + rd + " | " + directness + " | " + airline + "\n";
    } else if (count < 10) {
      std::cout << "[" + std::to_string(count) + "]  | " + price + " | " + dep + " | " + dest + " | " + ld + " | " + rd + " | " + directness + " | " + airline + "\n";
    } else {
      std::cout << "[" + std::to_string(count) + "] | " + price + " | " + dep + " | " + dest + " | " + ld + " | " + rd + " | " + directness + " | " + airline + "\n";
    }

    count++;
  }

  std::cout << "\n";

  string choice;

  while (choice != "Y" && choice != "N") {
    std::cout << "Would you like to save this flight path? (Y/N) ";
    std::cin >> choice;
  }

  if (choice == "Y") {
    saveFlight(dep, dest, ld, rd, lowestPrice);
    std::cout << "\nSaved\n";
  }

}

int parsePrice(std::string price) {
  std::string parsedPrice;
  for (const char character : price) {
    if (isdigit(character)) {
      parsedPrice += character;
    }
  }

  return std::stoi(parsedPrice);
}

void saveFlight(std::string dep, std::string dest, std::string ld, std::string rd, std::string lowestPrice) {

  sqlite3 *db;
  if (sqlite3_open("./database/flightDatabase.db", &db) != SQLITE_OK) {
    std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_close(db);
    return;
  }
  
  std::string createQuery = "CREATE TABLE IF NOT EXISTS flights (id INTEGER PRIMARY KEY AUTOINCREMENT, departure TEXT, destination TEXT, leave_date TEXT, return_date TEXT, lowest_price INT)";
  sqlite3_stmt *createStatement;
  sqlite3_prepare(db, createQuery.c_str(), -1, &createStatement, nullptr); 

  if (sqlite3_step(createStatement) != SQLITE_DONE) {
    std::cerr << "Failed to create table: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_close(db);
    return;
  }

  std::string insertQuery = "INSERT INTO flights (departure, destination, leave_date, return_date, lowest_price) VALUES (?, ?, ?, ?, ?)";
  sqlite3_stmt *insertStatement;
  sqlite3_prepare(db, insertQuery.c_str(), -1, &insertStatement, nullptr);
  sqlite3_bind_text(insertStatement, 1, dep.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(insertStatement, 2, dest.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(insertStatement, 3, ld.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(insertStatement, 4, rd.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(insertStatement, 5, lowestPrice.c_str(), -1, SQLITE_STATIC);
  if (sqlite3_step(insertStatement) != SQLITE_DONE) {
    std::cerr << "Error inserting data: " << sqlite3_errmsg(db) << std::endl;
  }

  sqlite3_finalize(insertStatement);
  sqlite3_close(db);

}