#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <queue>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include "SocialNetwork.h"
using namespace std;



class Solution final
{
private:
	std::ofstream _outFile;
    SocialNetwork network;

public:
	Solution();

	bool buildNetwork(const std::string& fileNameUsers, const std::string& fileNameFriendships);
	bool processCommand(const std::string& commandString);

};