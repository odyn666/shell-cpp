#include "handler/command/commandHandler.h"
#include <iostream>
#include <ostream>
#include <string>
/*#include "commandHandler.h"*/

int main() {
  // Flush after every std::cout / std::cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::vector<std::string> knownCommands{"exit", "echo", "type"};
  std::vector<std::string> args{};

  while (true) {
    std::cout << "$ ";

    std::string input;
    std::getline(std::cin, input);

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
    } else {
      std::string pathToExecutable =
          extractPathToExecutable(splitedCommands[0])[0];
#ifdef _WIN32
      executeProgram(
          "C:\\Windows\\System32\\notepad.exe"); // Example for Windows

#else

      handleFileExecution(pathToExecutable, args); // Example for Linux
#endif
                                                 // std::cout << input << ":
                                                 // command not found" <<
                                                 // std::endl;
      args.clear();
    }
  }
}
