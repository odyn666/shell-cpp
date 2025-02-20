#include <algorithm> // For std::find
#include <filesystem>
#include <iostream>
#include <ostream>
#include <regex>
#include <sstream> // For std::istringstream
#include <string>
#include <vector>

namespace fs = std::filesystem;

const char pathSeparator =
#ifdef _WIN32
    ';';
#else
    ':';
#endif

void handleNotValidCommand(std::string input) {
  std::cout << input << ": not found" << std::endl;
}
// Function to split a string into words
std::vector<std::string> splitString(const std::string &input) {
  std::vector<std::string> splited;
  std::istringstream iss(input);
  std::string word;

  while (iss >> word) {
    splited.push_back(word);
  }

  return splited;
}

std::vector<std::string> splitString(std::string str, char delimiter) {
  // Using str in a string stream
  std::stringstream ss(str);
  std::vector<std::string> res;
  std::string token;
  while (std::getline(ss, token, delimiter)) {
    res.push_back(token);
  }

  return res;
}

bool isCommandValid(std::vector<std::string> knownCommands,
                    std::string splitedCommands) {
  return !splitedCommands.empty() &&
         std::find(knownCommands.begin(), knownCommands.end(),
                   splitedCommands) != knownCommands.end();
}

int handleExitCommand(std::string &arg) {
  try {
    return std::stoi(arg);
  } catch (const std::exception &) {
    std::cout << " wrong argument" << std::endl;
    return -1;
  }
}

void handleEchoCommand(std::vector<std::string> args) {

  for (size_t i = 0; i < args.size(); i++) {
    std::cout << args[i] << " ";
  }
  std::cout << std::endl;
}
std::string getExecutablePath(std::string &command) {

  const char *envPath = getenv("PATH");

  return envPath;
}

void handleTypeCommand(std::string command, bool isknownCommand) {
  /*if (!isknownCommand) {*/
  /*handleNotValidCommand(command);*/
  /*} else {*/

  std::string envPath = getExecutablePath(command);
  std::vector<std::string> paths = splitString(envPath, pathSeparator);

  for (std::string path : paths) {
    /*std::cout << path << std::endl;*/
    for (const auto &entry : fs::directory_iterator(path)) {

      if (entry.path().string() == path + "/" + command) {
        std::cout << command << " is " << entry.path().string() << " "
                  << std::endl;
      }
    }
  }
  /*}*/
}

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
      std::cout << input << ": command not found" << std::endl;
      args.clear();
    }
  }
}
