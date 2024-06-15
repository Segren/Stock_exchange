#ifndef CORE_H
#define CORE_H

#include <string>
#include <unordered_map>
#include <iostream>
#include <unordered_map>
#include <unordered_set>


class Core
{
public:
    static Core& GetCore();

    // "Регистрирует" нового пользователя и возвращает его ID.
    std::string RegisterNewUser(const std::string& user_name);
    // Запрос имени клиента по ID
    std::string GetUserName(const std::string& user_id);

    Core() = default;
private:
    // <UserId, UserName>
    std::unordered_map<size_t, std::string> mUsers;
    std::unordered_set<std::string> userNames; 
    std::mutex mMutex;
};


#endif //CORE_H