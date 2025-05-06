#pragma once

#include <string>
#include <vector>
#include <list>          
#include <queue>         
#include <unordered_map> 
#include <unordered_set> 
#include <fstream>       
#include <utility>       
#include <cmath>         
#include <iomanip>       
#include <algorithm>     


struct User;

struct User {
    
    double activityRate;
    std::unordered_set<User*> friendPtrs; 
    std::string id;
    std::string name;
    std::string countryCode;
    int age;

    User() : activityRate(0.0), age(0) {} 
};

class SocialNetwork final {
private:
    static const std::string USER_NOT_FOUND_MSG;
    std::unordered_map<std::string, User> users;

public:
    
    SocialNetwork() = default; 
   
    // Methods 
    bool loadUsers(const std::string& filename);
    bool loadFriendships(const std::string& filename);
    bool viewProfile(const std::string& userId, std::ofstream& outFile);
    bool listFriends(const std::string& userId, std::ofstream& outFile);
    bool listMutuals(const std::string& userId1, const std::string& userId2, std::ofstream& outFile);
    bool findSeparation(const std::string& userId1, const std::string& userId2, std::ofstream& outFile);
    bool friendScore(const std::string& userId1, const std::string& userId2, std::ofstream& outFile);
    bool suggestFriends(const std::string& userId, std::ofstream& outFile);
    bool totalUsers(const std::list<std::string>& countryFilter, std::ofstream& outFile); 

};