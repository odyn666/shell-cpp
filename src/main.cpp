#include <iostream>
#include <string>
#include <vector>
#include <sstream>   // For std::istringstream
#include <algorithm> // For std::find

#ifdef _WIN32
#include <io.h>    // Windows: _access()
#define ACCESS _access
#define PATH_SEPARATOR ';'  // Windows uses ';' in PATH
#define EXECUTABLE_EXT ".exe"
#else
#include <unistd.h> // Unix: access()
#define ACCESS access
#define PATH_SEPARATOR ':'  // Unix uses ':'
#define EXECUTABLE_EXT ""   // No extension needed on Linux/macOS
#endif

void handleNotValidCommand(std::string input) {
	std::cout << input << ": not found" << std::endl;

}
// Function to split a string into words
std::vector<std::string> splitString(const std::string& input) {
	std::vector<std::string> splited;
	std::istringstream iss(input);
	std::string word;

	while (iss >> word) {
		splited.push_back(word);
	}

	return splited;
}

bool isCommandValid(std::vector<std::string> knownCommands, std::string splitedCommands) {
	return !splitedCommands.empty() &&
		std::find(knownCommands.begin(), knownCommands.end(), splitedCommands) != knownCommands.end();
}

int handleExitCommand(std::string& arg) {
	try
	{
		return std::stoi(arg);
	}
	catch (const std::exception&)
	{
		std::cout << " wrong argument" << std::endl;
		return -1;
	}
}

void handleEchoCommand(std::vector<std::string> args) {

	for (size_t i = 0; i < args.size(); i++)
	{
		std::cout << args[i] << " ";
	}
	std::cout << std::endl;
}

std::vector<std::string> split(const std::string& s, char& delimiter) {
	std::vector<std::string> tokens;
	std::string token;
	std::stringstream ss(s);

	while (std::getline(ss, token)) {
		tokens.push_back(token);
	}

	return tokens;
}
std::string getExecutablePath(std::string& command) {

	const char* envPath = getenv("PATH");
	char delimiter{ ';' };
	if (!envPath) {
		return "";
	}
	std::vector<std::string>directories{ split(envPath,delimiter) };

	for (const std::string& dir : directories) {
		const char* pathEnv = getenv("PATH");
		if (!pathEnv) {
			return "";
		}

		std::vector<std::string> directories = split(pathEnv, delimiter);  // Windows uses ';' as a separator

		for (const std::string& dir : directories) {
			std::string fullPath = dir + "\\" + command + ".exe";  // Windows executables usually have .exe
			if (_access(fullPath.c_str(), X_OK) == 0) {
				return fullPath;
			}
		}

		return "";
	}
}


void handleTypeCommand(std::string command, bool isknownCommand) {
	if (!isknownCommand)
	{
		handleNotValidCommand(command);
	}
	else
	{


		std::string builtInEnvPath = getExecutablePath(command);

		std::cout << command << " " << "is a shell builtin" << std::endl;
	}
}




int main() {
	// Flush after every std::cout / std::cerr
	std::cout << std::unitbuf;
	std::cerr << std::unitbuf;

	std::vector<std::string> knownCommands{ "exit","echo","type" };
	std::vector<std::string> args{};

	while (true) {
		std::cout << "$ ";

		std::string input;
		std::getline(std::cin, input);

		if (input.empty()) continue;

		std::vector<std::string> splitedCommands = splitString(input);

		//set arguments
		args = splitedCommands;
		args.erase(args.begin());// removing first element since its command not argument

		// finds command in vector and Ensures we don't access splitedCommands[0] if the input is empty
		bool isKnownCommand = isCommandValid(knownCommands, splitedCommands[0]);


		if (isKnownCommand)
		{

			if (splitedCommands[0] == "exit")
			{
				int exitCode = handleExitCommand(args[0]);
				if (exitCode != -1) return exitCode; // returning with provided exit code
			}
			if (splitedCommands[0] == "echo")
			{
				handleEchoCommand(args);
			}

			if (splitedCommands[0] == "type") {
				handleTypeCommand(args[0], isCommandValid(knownCommands, splitedCommands[1]));
			}
		}
		else
		{
			std::cout << input << ": command not found" << std::endl;
			args.clear();
		}
	}
}
