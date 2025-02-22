#pragma once

#include <string>
#include <vector>

void handleNotValidCommand(std::string input);

std::vector<std::string> splitString(const std::string &input);

std::vector<std::string> splitString(std::string str, char delimiter);

bool isCommandValid(std::vector<std::string> knownCommands,
                    std::string splitedCommands);

int handleExitCommand(std::string &arg);

void handleEchoCommand(std::vector<std::string> args);

std::vector<std::string> extractPathToExecutable(std::string command);

void handleTypeCommand(std::string command, bool isKnownCommand);

void handleFileExecution(std::string &path, std::vector<std::string> args);

std::string handlePwdCommand();

void handleCdCommand(std::string path);
