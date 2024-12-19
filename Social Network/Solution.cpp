#include <sstream>
#include "Solution.h"

Solution::Solution() : _outFile("Output.txt") {
	// Add your code here
}

bool Solution::processCommand(const std::string& commandString) {
	std::istringstream inString(commandString);
	std::string command;
	inString >> command;

	// Add your code here

	return false;
}

bool Solution::buildNetwork(const std::string& fileNameUsers, const std::string& fileNameFriendships) {
	std::ifstream finUsers(fileNameUsers);
	std::ifstream finFriendships(fileNameFriendships);
	if (finUsers.fail() || finFriendships.fail()) {
		return false;
	}

	// ****** Add your code here *******
	
	return true;
}

// Add your code here
