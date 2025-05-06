#include <sstream>
#include "Solution.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <queue>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include "SocialNetwork.h"
#include <chrono>

using namespace std;
const std::string SocialNetwork::USER_NOT_FOUND_MSG = "Error: User not found.";

bool SocialNetwork::loadUsers(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open user file: " << filename << endl;
        return false;
    }
    users.clear(); 

    string line;
    int lineNumber = 0;
    while (getline(file, line)) {
        lineNumber++;
        stringstream ss(line);
        string segment;
        vector<string> data;
        while (getline(ss, segment, ',')) {
            data.push_back(segment);
        }
        if (data.size() == 5) {
            User newUser; // Creates a temporary User object
            newUser.id = data[0];
            newUser.name = data[1];
            try {
                newUser.age = stoi(data[2]);
                newUser.countryCode = data[3];
                newUser.activityRate = stod(data[4]);

                const auto result = users.emplace(newUser.id, std::move(newUser));
                if (!result.second) {
                    cerr << "Warning: Duplicate user ID found and ignored: " << newUser.id << " at line " << lineNumber << endl;
                }

            }
            catch (const exception& e) {
                cerr << "Error parsing user data: " << e.what() << " at line " << lineNumber << ": " << line << endl;
               
            }
        }
        else {
            cerr << "Error: Invalid user data format at line " << lineNumber << ": " << line << endl;
           
        }
    }
    file.close();
    cout << "Loaded " << users.size() << " users." << endl; // Debug 
    return true;
}

bool SocialNetwork::loadFriendships(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open friendships file: " << filename << endl;
        return false;
    }
    string line;
    int lineNumber = 0;
    int friendshipsAdded = 0;
    while (getline(file, line)) {
        lineNumber++;
        stringstream ss(line);
        string id1, id2;
        if (getline(ss, id1, ',') && getline(ss, id2, ',')) {
            auto it1 = users.find(id1); 
            auto it2 = users.find(id2); 

            if (it1 != users.end() && it2 != users.end() && id1 != id2) {
                User* const user1 = &it1->second; // pointer to User in map
                User* const user2 = &it2->second; 

                // pointers to each other's friend 
                user1->friendPtrs.insert(user2);
                user2->friendPtrs.insert(user1);
                friendshipsAdded++;
            }
        }
        else {
            cerr << "Error: Invalid friendship data format at line " << lineNumber << ": " << line << endl;
        }
    }
    file.close();
    cout << "Processed " << friendshipsAdded << " friendships." << endl; // Debug 
    return true;
}

bool SocialNetwork::viewProfile(const string& userId, ofstream& outFile) {
    const auto it = users.find(userId); 
    if (it != users.end()) {
        const User& user = it->second; 

        outFile << "Name: " << user.name << endl;
        outFile << "Age: " << user.age << endl;
        string countryName = "Unknown";
        if (user.countryCode == "DEU") countryName = "Germany";
        else if (user.countryCode == "DNK") countryName = "Denmark";
        else if (user.countryCode == "FIN") countryName = "Finland";
        else if (user.countryCode == "FRA") countryName = "France";
        else if (user.countryCode == "GBR") countryName = "United Kingdom";
        else if (user.countryCode == "NLD") countryName = "Netherlands";
        else if (user.countryCode == "NOR") countryName = "Norway";
        else if (user.countryCode == "SWE") countryName = "Sweden";
        outFile << "Country: " << countryName << endl;
        outFile << "Activity Rate: " << round(user.activityRate * 100) << "%" << endl;
        outFile << "Friends: " << user.friendPtrs.size() << endl;
        return true;
    }
    else {
        outFile << USER_NOT_FOUND_MSG << endl;
        return false;
    }
}

bool SocialNetwork::listFriends(const string& userId, ofstream& outFile) {
    const auto it = users.find(userId); 
    if (it != users.end()) {
        const User& user = it->second;
      
        outFile << user.friendPtrs.size() << " friend(s) found." << endl;
     
        for (const User* const friendUser : user.friendPtrs) {
            outFile << friendUser->name << " [ID:" << friendUser->id << "]" << endl;
        }
        
        return true;
    }
    else {
        outFile << USER_NOT_FOUND_MSG << endl;
        return false;
    }
}

bool SocialNetwork::listMutuals(const string& userId1, const string& userId2, ofstream& outFile) {
    const auto it1 = users.find(userId1); 
    const auto it2 = users.find(userId2); 

    if (it1 != users.end() && it2 != users.end()) {
        const User& user1 = it1->second;
        const User& user2 = it2->second;
        vector<User*> mutualFriendsList; 

    
        const auto& set1 = user1.friendPtrs;
        const auto& set2 = user2.friendPtrs;

        if (set1.size() < set2.size()) {
            for (User* const friend1 : set1) {
                if (set2.count(friend1)) { 
                    mutualFriendsList.push_back(friend1);
                }
            }
        }
        else {
            for (User* const friend2 : set2) {
                if (set1.count(friend2)) { 
                    mutualFriendsList.push_back(friend2);
                }
            }
        }

        outFile << mutualFriendsList.size() << " mutual friend(s) found." << endl;
        for (const User* const friendUser : mutualFriendsList) {
            outFile << friendUser->name << " [ID:" << friendUser->id << "]" << endl;
        }
        return true;
    }
    else {
        outFile << USER_NOT_FOUND_MSG << endl; 
        return false;
    }
}

bool SocialNetwork::findSeparation(const string& userId1, const string& userId2, ofstream& outFile) {
    auto it1 = users.find(userId1);
    auto it2 = users.find(userId2);

    if (it1 == users.end() || it2 == users.end()) {
        outFile << USER_NOT_FOUND_MSG << endl;
        return false;
    }

    User* startUser = &it1->second;
    User* endUser = &it2->second;

    if (startUser == endUser) {
        outFile << "0 degree(s)" << endl;
        return true;
    }

    queue<pair<User*, int>> q_fwd;
    queue<pair<User*, int>> q_bwd;

    std::map<User*, pair<int, int>> visited;

    q_fwd.push({ startUser, 0 });
    visited[startUser] = { 0, 1 };

    q_bwd.push({ endUser, 0 });
    if (visited.count(endUser)) {
    }
    else {
        visited[endUser] = { 0, 2 };
    }


    int current_dist_fwd = 0;
    int current_dist_bwd = 0;
    int final_distance = -1;

    while (!q_fwd.empty() && !q_bwd.empty() && final_distance == -1) {

        const int nodes_at_level_fwd = q_fwd.size();
        for (int i = 0; i < nodes_at_level_fwd && final_distance == -1; ++i) {
            User* const currentUser = q_fwd.front().first;
            int dist = q_fwd.front().second;
            q_fwd.pop();

            if (dist >= 3 && current_dist_bwd >= 3) continue; 


            for (User* neighbor : currentUser->friendPtrs) {
                const auto visited_it = visited.find(neighbor);
                if (visited_it == visited.end()) { 
                    visited[neighbor] = { dist + 1, 1 }; 
                    q_fwd.push({ neighbor, dist + 1 });
                }
                else if (visited_it->second.second == 2) { 
                    final_distance = dist + 1 + visited_it->second.first; 
                    break; 
                }
            
            }
        }
        current_dist_fwd++;


        if (final_distance != -1) break; 

        const int nodes_at_level_bwd = q_bwd.size();
        for (int i = 0; i < nodes_at_level_bwd && final_distance == -1; ++i) {
            User* const currentUser = q_bwd.front().first;
            int dist = q_bwd.front().second;
            q_bwd.pop();

            if (dist >= 3 && current_dist_fwd >= 4) continue; 


            for (User* neighbor : currentUser->friendPtrs) {
                const auto visited_it = visited.find(neighbor);
                if (visited_it == visited.end()) { 
                    visited[neighbor] = { dist + 1, 2 }; 
                    q_bwd.push({ neighbor, dist + 1 });
                }
                else if (visited_it->second.second == 1) { 
                    final_distance = dist + 1 + visited_it->second.first; 
                    break; 
                }
            }
        }
        current_dist_bwd++;

    } 


    if (final_distance != -1 && final_distance <= 6) {
        outFile << final_distance << " degree(s)" << endl;
    }
    else {
        outFile << "6 degree(s)" << endl; 
    }
    return true; 
}


bool SocialNetwork::friendScore(const string& userId1, const string& userId2, ofstream& outFile) {
    auto it1 = users.find(userId1); 
    auto it2 = users.find(userId2); 

    if (it1 == users.end() || it2 == users.end()) {
        outFile << USER_NOT_FOUND_MSG << endl; return false;
    }

    User* const user1 = &it1->second;
    User* const user2 = &it2->second;

    if (user1 == user2) {
        outFile << "Error: Cannot calculate friend score for the same user." << endl; return false;
    }

    if (user1->friendPtrs.count(user2)) {
        outFile << "FriendScore " << userId1 << " " << userId2 << endl;
        outFile << fixed << setprecision(2) << 0.00 << endl; return true;
    }

    int mutualFriendsCount = 0;
    const auto& set1 = user1->friendPtrs;
    const auto& set2 = user2->friendPtrs;

    if (set1.size() < set2.size()) {
        for (User* const friend1 : set1) {
            if (set2.count(friend1)) { mutualFriendsCount++; }
        }
    }
    else {
        for (User* const friend2 : set2) {
            if (set1.count(friend2)) { mutualFriendsCount++; }
        }
    }

    const double score = static_cast<double>(mutualFriendsCount) * sqrt(user1->activityRate * user2->activityRate);
    outFile << fixed << setprecision(2) << score << endl; return true;
}


bool SocialNetwork::suggestFriends(const string& userId, ofstream& outFile) {
    auto it = users.find(userId); 
    if (it == users.end()) {
        outFile << USER_NOT_FOUND_MSG << endl;
        return false;
    }
    User* const currentUser = &it->second;

    unordered_set<User*> excludeSet = currentUser->friendPtrs;
    excludeSet.insert(currentUser);

    unordered_map<User*, int> candidates;

    for (User* const friend1 : currentUser->friendPtrs) {
        for (User* const friend2 : friend1->friendPtrs) {
            if (excludeSet.count(friend2) == 0) {
                candidates[friend2]++; 
            }
        }
    }

    vector<pair<double, User*>> potentialFriendsScores;
    potentialFriendsScores.reserve(candidates.size()); 

    for (const auto& pair : candidates) {
        User* candidate = pair.first;
        const int mutualFriendsCount = pair.second;

        const double score = static_cast<double>(mutualFriendsCount) * sqrt(currentUser->activityRate * candidate->activityRate);
        potentialFriendsScores.push_back({ score, candidate });
        
    }

    const int num_suggestions = min(static_cast<int>(potentialFriendsScores.size()), 5);


    if (num_suggestions > 0) {
        std::partial_sort(
            potentialFriendsScores.begin(),                   
            potentialFriendsScores.begin() + num_suggestions, 
            potentialFriendsScores.end(),                     
            [](const pair<double, User*>& a, const pair<double, User*>& b) { 
                if (a.first != b.first) {
                    return a.first > b.first; 
                }
                return a.second->id < b.second->id; 
            }
        );
    }
    for (int i = 0; i < num_suggestions; ++i) {
        const auto& scorePair = potentialFriendsScores[i];
        User* const suggestedUser = scorePair.second;
        const int mutualCountOutput = candidates[suggestedUser];

        outFile << suggestedUser->name << " [ID:" << suggestedUser->id << "] - " << mutualCountOutput << " mutual friend(s)" << endl;
    }
    return true;
}

bool SocialNetwork::totalUsers(const list<string>& countryFilter, ofstream& outFile) {

    int count = 0;
    if (countryFilter.empty()) {
        count = users.size(); 
    }
    else {
        const unordered_set<string> filterSet(countryFilter.begin(), countryFilter.end());
        for (const auto& pair : users) {
            if (filterSet.count(pair.second.countryCode)) { 
                count++;
            }
        }
    }
    outFile << count << endl;
    return true;
}


Solution::Solution() : _outFile("Output.txt") {
	// Add your code here
}



bool Solution::buildNetwork(const std::string& fileNameUsers, const std::string& fileNameFriendships) {

    cout << "Building network..." << endl; // Debug
    if (!network.loadUsers(fileNameUsers)) {
        cerr << "Failed to load users." << endl;
        return false;
    }
    if (!network.loadFriendships(fileNameFriendships)) {
        cerr << "Failed to load friendships." << endl;
        return false;
    }
    cout << "Network build successful." << endl; // Debug

    return true;
}

bool Solution::processCommand(const std::string& commandString) {
    std::istringstream inString(commandString);
    std::string command;
    inString >> command;

    // Outputs the original command first
    _outFile << commandString << std::endl;
    bool success = false;

    try { 
        if (command == "ViewProfile") {
            std::string userId;
            if (inString >> userId) { success = network.viewProfile(userId, _outFile); }
            else { _outFile << "Error: Invalid ViewProfile command format." << std::endl; }
        }
        else if (command == "ListFriends") {
            std::string userId;
            if (inString >> userId) { success = network.listFriends(userId, _outFile); }
            else { _outFile << "Error: Invalid ListFriends command format." << std::endl; }
        }
        else if (command == "ListMutuals") {
            std::string userId1, userId2;
            if (inString >> userId1 >> userId2) { success = network.listMutuals(userId1, userId2, _outFile); }
            else { _outFile << "Error: Invalid ListMutuals command format." << std::endl; }
        }
        else if (command == "FindSeparation") {
            std::string userId1, userId2;
            if (inString >> userId1 >> userId2) { success = network.findSeparation(userId1, userId2, _outFile); }
            else { _outFile << "Error: Invalid FindSeparation command format." << std::endl; }
        }
        else if (command == "FriendScore") {
            std::string userId1, userId2;
            if (inString >> userId1 >> userId2) { success = network.friendScore(userId1, userId2, _outFile); }
            else { _outFile << "Error: Invalid FriendScore command format." << std::endl; }
        }
        else if (command == "SuggestFriends") {
            std::string userId;
            if (inString >> userId) { success = network.suggestFriends(userId, _outFile); }
            else { _outFile << "Error: Invalid SuggestFriends command format." << std::endl; }
        }
        else if (command == "TotalUsers") {
            std::string countryCode;
            std::list<std::string> countries;
            while (inString >> countryCode) { countries.push_back(countryCode); }
            success = network.totalUsers(countries, _outFile);
        }
        else {
            _outFile << "Error: Unrecognized command." << std::endl;
            success = false;
        }
    }
    catch (const std::exception& e) {
        _outFile << "Error: Exception during command processing: " << e.what() << std::endl;
        success = false;
    }
    catch (...) {
        _outFile << "Error: Unknown exception during command processing." << std::endl;
        success = false;
    }

    // Add the blank line after output or error messages
    _outFile << std::endl;

    return success;
}

