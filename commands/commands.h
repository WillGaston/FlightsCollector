#ifndef COMMANDS_H
#define COMMANDS_H

#include <iostream>
#include <string.h>
#include <regex>
#include <spawn.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>
#include <jsoncpp/json/json.h>
#include <sqlite3.h>
#include <bits/stdc++.h>

int findDeals(int numArguments, char *arguments[]);
void viewDeals();
void saveDeal();
void removeDeal();
void displayFlights(std::string dep, std::string dest, std::string ld, std::string rd);
void saveFlight(std::string dep, std::string dest, std::string ld, std::string rd, std::string lowestPrice);

#endif