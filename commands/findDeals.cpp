#include "./commands.h"
using namespace std;

extern char **environ;

int validateArguments(char *arguments[]);

int findDeals(int numArguments, char *arguments[]) {

  if (numArguments != 10) {
    cout << "\nInvalid Arguments, must be of format:\n";
    cout << "-f -dep <departure airport> -dest <destination airport> -ld <YYYY-MM-DD> -rd <YYYY-MM-DD>   Find deals\n\n";
    cout << "For Example:\n";
    cout << "-f -dep SYD -dest SIN -ld 2025-02-12 -rd 2025-03-01\n\n";
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

    cout << "Invalid Options Provided\n";
    return EXIT_FAILURE;
  }

  std::regex airportPattern("[A-Z]{3}");

  if (!regex_match(arguments[3], airportPattern) ||
      !regex_match(arguments[5], airportPattern)) {

    cout << "Invalid Airport Code Given\n";
    return EXIT_FAILURE;
  }

  std::regex datePattern("\\d{4}\\-\\d{2}\\-\\d{2}");

  if (!regex_match(arguments[7], datePattern) ||
      !regex_match(arguments[9], datePattern)) {

    cout << "Invalid Date Given\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void displayFlights(std::string dep, std::string dest, std::string ld, std::string rd) {
  std::ifstream flight_file("./tempFlightData/flightData.json", std::ifstream::binary);
  Json::Value flights;
  flight_file >> flights;

  int count = 1;

  std::cout << "\n";
  std::cout << "ID   |  Price  | Dep | Des | Leave Date |  Ret Date  | Airline \n";
  std::cout << "----------------------------------------------------------------------------------\n";

  for (const auto& flight : flights) {
    std::string airline = flight["airline"].asString();
    std::string price = flight["price"].asString();

    if (price == "Price unavailable") continue;
    if (!airline.find("Operated by")) continue;

    if (count < 10) {
      std::cout << "[" + std::to_string(count) + "]  | " + price + " | " + dep + " | " + dest + " | " + ld + " | " + rd + " | " + airline + "\n";
    } else {
      std::cout << "[" + std::to_string(count) + "] | " + price + " | " + dep + " | " + dest + " | " + ld + " | " + rd + " | " + airline + "\n";
    }

    count++;
  }

  cout << "\n";

  string choice;

  while (choice != "Y" && choice != "N") {
    cout << "Would you like to save this flight path? (Y/N) ";
    cin >> choice;
  }

}