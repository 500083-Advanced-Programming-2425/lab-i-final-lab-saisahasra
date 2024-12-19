#pragma once

#include <fstream>
#include <string>

class Solution final
{
	std::ofstream _outFile;

	// Add your code here

public:
	Solution();

	bool buildNetwork(const std::string& fileNameUsers, const std::string& fileNameFriendships);
	bool processCommand(const std::string& commandString);

	// Add your code here
};