#include "./commands.h"
using namespace std;

extern char **environ;

void displayFlightsForEmail(std::string dep, std::string dest, std::string ld, std::string rd, std::vector<std::string>& output);
int findDealsForEmail(string dep, string dest, string ld, string rd, std::vector<std::string>& output);

void findDealLoop() {

  std::vector<std::string> output;

  sqlite3 *db;
  if (sqlite3_open("./database/flightDatabase.db", &db) != SQLITE_OK) {
      std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
      sqlite3_close(db);
      return;
  }

  std::string readQuery = "SELECT * FROM flights";
  sqlite3_stmt *readStatement;

  if (sqlite3_prepare_v2(db, readQuery.c_str(), -1, &readStatement, nullptr) != SQLITE_OK) {
      std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
      sqlite3_close(db);
      return;
  }

  while (sqlite3_step(readStatement) == SQLITE_ROW) {
    const unsigned char* dep = sqlite3_column_text(readStatement, 1);
    const unsigned char* dest = sqlite3_column_text(readStatement, 2);
    const unsigned char* ld = sqlite3_column_text(readStatement, 3);
    const unsigned char* rd = sqlite3_column_text(readStatement, 4);

    std::string dep2 = dep ? reinterpret_cast<const char*>(dep) : "NULL";
    std::string dest2 = dest ? reinterpret_cast<const char*>(dest) : "NULL";
    std::string ld2 = ld ? reinterpret_cast<const char*>(ld) : "NULL";
    std::string rd2 = rd ? reinterpret_cast<const char*>(rd) : "NULL";

    findDealsForEmail(dep2, dest2, ld2, rd2, output);

  }
  for (const auto& line : output) {
    std::cout << line;
  }

}

int findDealsForEmail(string dep, string dest, string ld, string rd, std::vector<std::string>& output) {

  setenv("DepENV", dep.c_str(), 0);
  setenv("DestENV", dest.c_str(), 0);
  setenv("LeaveENV", ld.c_str(), 0);
  setenv("ReturnENV", rd.c_str(), 0);

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

  displayFlightsForEmail(dep, dest, ld, rd, output);

  return EXIT_SUCCESS;

}

void displayFlightsForEmail(std::string dep, std::string dest, std::string ld, std::string rd, std::vector<std::string>& output) {
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

  output.push_back("\n");
  output.push_back(dep + " --> " + dest + "\n");

  output.push_back("\nLowest Price: " + lowestPrice + "\n");

  output.push_back("\n");
  output.push_back("ID   |  Price  | Dep | Des | Leave Date |  Ret Date  | Directness | Airline \n");
  output.push_back("----------------------------------------------------------------------------------\n");

  for (const auto& flight : flights) {
    std::string airline = flight["airline"].asString();
    std::string price = flight["price"].asString();
    std::string directness = flight["directness"].asString();

    if (price == "Price unavailable") continue;
    if (!airline.find("Operated by")) continue;
    if (airline == "") airline = "Unavailable";

    if (parsePrice(price) < 1000) {
      output.push_back("[" + std::to_string(count) + "]  | " + price + "   | " + dep + " | " + dest + " | " + ld + " | " + rd + " | " + directness + " | " + airline + "\n");
    } else if (parsePrice(price) >= 10000 && count < 10) {
      output.push_back("[" + std::to_string(count) + "]  | " + "> A$10k" + " | " + dep + " | " + dest + " | " + ld + " | " + rd + " | " + directness + " | " + airline + "\n");
    } else if (parsePrice(price) >= 10000) {
      output.push_back("[" + std::to_string(count) + "] | " + "> A$10k" + " | " + dep + " | " + dest + " | " + ld + " | " + rd + " | " + directness + " | " + airline + "\n");
    } else if (count < 10) {
      output.push_back("[" + std::to_string(count) + "]  | " + price + " | " + dep + " | " + dest + " | " + ld + " | " + rd + " | " + directness + " | " + airline + "\n");
    } else {
      output.push_back("[" + std::to_string(count) + "] | " + price + " | " + dep + " | " + dest + " | " + ld + " | " + rd + " | " + directness + " | " + airline + "\n");
    }

    count++;
  }

  output.push_back("\n");

}
