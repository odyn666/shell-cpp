#include <iostream>
#include <string>
#include <vector>
#include <sstream>   // For std::istringstream
#include <algorithm> // For std::find

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

int main() {
	// Flush after every std::cout / std::cerr
	std::cout << std::unitbuf;
	std::cerr << std::unitbuf;

	std::vector<std::string> knownCommands{ "exit","echo" };
	std::vector<std::string> args{};

	while (true) {
		std::cout << "$ ";

		std::string input{ "" };
		std::getline(std::cin, input);

		if (input.empty()) continue;

		std::vector<std::string> splitedCommands = splitString(input);

		//set arguments
		args = splitedCommands;
		args.erase(args.begin());// removing first elemenct since its command not argument

		// finds command in vector and Ensures we don't access splitedCommands[0] if the input is empty
		bool isKnownCommand = !splitedCommands.empty() &&
			std::find(knownCommands.begin(), knownCommands.end(), splitedCommands[0]) != knownCommands.end();


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
		}
		else
		{
			std::cout << input << ": command not found" << std::endl;
			args.clear();
		}
	}
}
