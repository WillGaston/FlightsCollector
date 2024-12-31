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
  setenv("DestENV", arguments[5], 1);
  setenv("LeaveENV", arguments[7], 1);
  setenv("ReturnENV", arguments[9], 1);

  const char *args[3];
  args[0] = "node";
  args[1] = "./dealsScript.js";
  args[2] = NULL;
  pid_t pid;
  int status;

  if (posix_spawn(&pid, "/usr/bin/node", NULL, NULL, const_cast<char *const *>(args), environ) != 0) {
    cout << "deals script failed\n";
    unsetenv("DepENV");
    unsetenv("DestENV");
    unsetenv("LeaveENV");
    unsetenv("ReturnENV");
    return EXIT_FAILURE;
  }

  if (waitpid(pid, &status, 0) == -1) {
		printf("Process Wait Failed");
    unsetenv("DepENV");
    unsetenv("DestENV");
    unsetenv("LeaveENV");
    unsetenv("ReturnENV");
		return EXIT_FAILURE;
	}

  unsetenv("DepENV");
  unsetenv("DestENV");
  unsetenv("LeaveENV");
  unsetenv("ReturnENV");

	return WIFEXITED(status);
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