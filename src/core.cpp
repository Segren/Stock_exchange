#include "core.h"

std::string Core::RegisterNewUser(const std::string& user_name)
{
    std::lock_guard<std::mutex> lock(mMutex);

    if(userNames.find(user_name) != userNames.end()){
        return "Error! User already exists.";
    }

    size_t newUserId = mUsers.size();
    mUsers[newUserId] = user_name;
    userNames.insert(user_name);
    std::cout << "Registered user: " << user_name << " with ID: " << newUserId << std::endl;

    for(const auto& pair : mUsers){
        std::cout << "PAIR FOR UMAP " << pair.first << " " << pair.second << std::endl;
    }

    return std::to_string(newUserId);
}

std::string Core::GetUserName(const std::string& user_id)
{
    std::lock_guard<std::mutex> lock(mMutex);
    for(const auto& pair : mUsers){
        std::cout << "PAIR FOR UMAP " << pair.first << " " << pair.second << std::endl;
    }

    const auto userIt = mUsers.find(std::stoi(user_id));
    if (userIt == mUsers.cend())
    {
        std::cout << "User ID: " << user_id << " not found." << std::endl;
        return "Error! Unknown User";
    }
    else
    {
        std::cout << "Found user ID: " << user_id << " with name: " << userIt->second << std::endl;
        return userIt->second;
    }
}

Core& Core::GetCore()
{
    static Core core;
    return core;
}