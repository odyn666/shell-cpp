#include "algorithm"
#include "filesystem"
#include "sstream"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <ostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h> // Windows-specific headers
#else
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> // Linux-specific headers
#endif

#define Log(x) std::cout << x << std::endl;
// common imports
using std::string;
using std::vector;

namespace fs = std::filesystem;

const char pathSeparator =
#ifdef _WIN32
    ';';
#else
    ':';
#endif

void handleNotValidCommand(std::string input) {
  std::cout << input << ": not found" << std::endl;
  return;
}

std::string extractNameFromPath(std::string path) {

#ifdef _WIN32
  return path.substr(path.find_last_of("/\\"));

#else

#endif
  return path.substr(path.find_last_of("/") + 1);
}

#ifdef _WIN32

void handleFileExecution(std::string &path, std::vector<std::string> args) {

  // Construct the command line from the arguments
  std::string commandLine = path;
  for (const auto &arg : args) {
    commandLine += " " + arg; // Add each argument with a space
  }

  // Additional information
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  // Set the size of the structures
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  // Start the program
  if (CreateProcess(
          NULL, // Application name is null, we pass the command line directly
          &commandLine[0], // Command line (must be a pointer to a char array)
          NULL,            // Process handle not inheritable
          NULL,            // Thread handle not inheritable
          FALSE,           // Set handle inheritance to FALSE
          0,               // No creation flags
          NULL,            // Use parent's environment block
          NULL,            // Use parent's starting directory
          &si,             // Pointer to STARTUPINFO structure
          &pi              // Pointer to PROCESS_INFORMATION structure
          ) == 0) {
    // If CreateProcess fails, output the error message
    std::cerr << "CreateProcess failed with error: " << GetLastError()
              << std::endl;
    return;
  }

  // Wait for the process to finish
  WaitForSingleObject(pi.hProcess, INFINITE);

  // Close process and thread handles
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
}

#else

void handleFileExecution(std::string &path, std::vector<std::string> args) {

  pid_t pid = fork();
  if (pid == -1) {
    std::cerr << "Fork failed\n";
  } else if (pid == 0) {

    std::vector<char *> argv;
    char *filename = const_cast<char *>(extractNameFromPath(path).c_str());
    argv.push_back(filename);
    for (auto &arg : args) {
      argv.push_back(const_cast<char *>(arg.c_str())); // Arguments
    }
    argv.push_back(nullptr); // execvp expects the last element to be NULL

    execvp(argv[0], argv.data());
    perror("execvp failed"); // If execvp fails
    exit(1);
  } else {
    int status;
    waitpid(pid, &status, 0);
  }
}

#endif

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

std::string getEnvPath() {
#ifdef _WIN32
  // Allocate a buffer to hold the environment variable value
  char *envPath = nullptr;
  size_t len = 0;

  // Retrieve the PATH environment variable securely using _dupenv_s
  if (_dupenv_s(&envPath, &len, "PATH") != 0 || envPath == nullptr) {
    std::cerr << "Failed to get PATH environment variable." << std::endl;
    return "";
  }

  // Convert the result to std::string
  std::string result(envPath);

  // Free the allocated memory
  free(envPath);

  return result;
#else
  const char *envPath = getenv("PATH");

  return envPath;

#endif
}

std::vector<std::string> extractPathToExecutable(std::string command) {

  std::string envPath = getEnvPath(); // Get system PATH variable
  std::vector<std::string> paths =
      splitString(envPath, pathSeparator); // Split by ':'
  //
  std::vector<std::string> result{};

  for (const std::string &path : paths) {
    if (!fs::exists(path))
      continue; // Skip non-existing directories

    for (const auto &entry : fs::directory_iterator(path)) {
      if (entry.path().filename() == command) { // Compare only filename
        result.push_back(entry.path().string());

        return result; // Stop after first match
      }
    }
  }

  handleNotValidCommand(command);
  return result;
}
void handleTypeCommand(std::string command, bool isKnownCommand) {
  if (isKnownCommand) {
    std::cout << command << " is a shell builtin" << std::endl;
    return;
  }
  std::vector<std::string> paths = extractPathToExecutable(command);
  for (std::string path : paths) {

    std::cout << command << " is " << path << std::endl;
  }
  // TODO: double check if its necessary if not delete
  if (paths.empty()) {
    /*std::cout << command << ": not found" << std::endl;*/
  }
}

void handlePwdCommand() {

  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) != 0) {

    Log(cwd);
  } else {
    perror("getcwd() error");
  }
}
