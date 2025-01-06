#include "./commands/commands.h"
using namespace std;

void help(bool flag);
void findFunction(int numArguments, char *arguments[]);

int main(int argc, char *argv[]) {

  if (argc == 1) {
    help(true);
  } else {
    findFunction(argc, argv);
  }

  return 0;

}

// Potential Functions
// -f find deal
// -a add deal
// -v view deals
// -r remove deal
// -h || undefined help

void findFunction(int numArguments, char *arguments[]) {

  char *funcOption = arguments[1];

  if(strcmp(funcOption, "-f") == 0) {
    findDeals(numArguments, arguments);
  } else if (strcmp(funcOption, "-v") == 0) {
    viewDeals();
  } else if (strcmp(funcOption, "-r") == 0) {
    removeDeal(numArguments, arguments);
  } else if (strcmp(funcOption, "-s") == 0) {
    findDealLoop();
  } else {
    help(false);
  }
}

//flag tells how help was accessed
// -true = bad input
// -false = used command
void help(bool flag) {
  if (flag == true) {
    cout << "Invalid Input!";
  };

  std::cout << R"(
  ______ _ _       _     _      _____      _ _           _             
 |  ____| (_)     | |   | |    / ____|    | | |         | |            
 | |__  | |_  __ _| |__ | |_  | |     ___ | | | ___  ___| |_ ___  _ __ 
 |  __| | | |/ _` | '_ \| __| | |    / _ \| | |/ _ \/ __| __/ _ \| '__|
 | |    | | | (_| | | | | |_  | |___| (_) | | |  __/ (__| || (_) | |   
 |_|    |_|_|\__, |_| |_|\__|  \_____\___/|_|_|\___|\___|\__\___/|_|   
              __/ |                                                    
             |___/   

  --------------------------------------------------------------------
  |                                                                   |
  |                    Find the best flight deals                     |
  |                                                                   |
  --------------------------------------------------------------------

  Options:
   -f -dep <departure> -dest <destination> -ld <YYYY-MM-DD> -rd <YYYY-MM-DD>    Find deals
   -v                                                                           View saved deals
   -s                                                                           Search saved deals
   -r -fn <flight number> || -all                                               Remove saved flight deals
   -h                                                                           Help page                                                                     
  )" << '\n';
}
