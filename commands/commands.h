#ifndef COMMANDS_H
#define COMMANDS_H

#include <iostream>
#include <string.h>
#include <regex>
#include <spawn.h>
#include <sys/types.h>
#include <sys/wait.h>

int findDeals(int numArguments, char *arguments[]);
void viewDeals();
void saveDeal();
void removeDeal();

#endif