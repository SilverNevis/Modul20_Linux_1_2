#pragma once
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "User.h"
#include "Trie.h"
#include <Winsock2.h>

class Chat {
private:
    std::vector<User> users;
    std::unordered_set<std::string> user_logins;
    std::unordered_map<std::string, std::string> messages;
    Trie trie;
    SOCKET serverSocket;
    SOCKET clientSocket;

public:
    void registerUser(const std::string& login, const std::string& password, const std::string& name);
    bool loginUser(const std::string& login, const std::string& password);
    void sendMessage(const std::string& from, const std::string& to, const std::string& message);
    std::string readMessages(const std::string& user);
    std::vector<std::string> autocomplete(const std::string& prefix);
    void startNetworkService();
};
