#include "commandHandler.hpp"
#include <cstddef>
#include <iostream>
#include <ostream>
#include <regex>
#include <string>
#define Log(x) std::cout << x << std::endl;

int main() {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::vector<std::string> knownCommands{"exit", "echo", "type", "pwd", "cd"};
  std::vector<std::string> args{};

  while (true) {
    std::cout << "$ ";

    std::string input;
    std::getline(std::cin, input);
    std::string qtText{""};
    if (input.empty())
      continue;

    std::vector<std::string> splitedCommands = splitString(input);

    // set arguments
    args = splitedCommands;
    args.erase(
        args.begin()); // removing first element since its command not argument

    // finds command in vector and Ensures we don't access splitedCommands[0] if
    // the input is empty
    bool isKnownCommand = isCommandValid(knownCommands, splitedCommands[0]);
    std::regex quotationRgx(R"(['"]([^'"]*)['"])");
    std::smatch match;
    if (std::regex_search(input, match, quotationRgx)) {
      args.clear();
      while (std::regex_search(input, match, quotationRgx)) {
        std::string extracted = match[1]; // Get quoted content

        // If previous match was adjacent (no space between), merge with last
        // argument
        if (!args.empty() && match.prefix().str().empty()) {
          args.back() += extracted;
        } else {
          args.push_back(extracted);
        }
        input = match.suffix().str();
      }
    }

    if (isKnownCommand) {

      if (splitedCommands[0] == "exit") {
        int exitCode = handleExitCommand(args[0]);
        if (exitCode != -1)
          return exitCode; // returning with provided exit code
      }
      if (splitedCommands[0] == "echo") {
        handleEchoCommand(args);
      }

      if (splitedCommands[0] == "type") {
        handleTypeCommand(args[0],
                          isCommandValid(knownCommands, splitedCommands[1]));
      }

      if (splitedCommands[0] == "pwd") {
        Log(handlePwdCommand());
      }

      if (splitedCommands[0] == "cd") {
        handleCdCommand(args[0]);
      }

      // checking for quotation marks
    } else {
      std::string pathToExecutable{};
      if (extractPathToExecutable(splitedCommands[0]).empty()) {
        pathToExecutable = "";
      } else {
        pathToExecutable = extractPathToExecutable(splitedCommands[0])[0];
      }
#ifdef _WIN32
      handleFileExecution(pathToExecutable, args); // Example for Windows

#else
      if (!pathToExecutable.empty()) {

        handleFileExecution(pathToExecutable, args); // Example for Linux
      } // }
#endif
                                                   // std::cout << input << ":
                                                   // command not found" <<
                                                   // std::endl;
      args.clear();
    }
  }
}
