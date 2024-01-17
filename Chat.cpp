#include "Chat.h"
#include <iostream>
#include <stdexcept>
#include <ws2tcpip.h>

void Chat::registerUser(const std::string& login, const std::string& password, const std::string& name) {
    if (user_logins.find(login) != user_logins.end()) {
        throw std::runtime_error("Логин уже существует.");
    }
    users.push_back(User(login, password, name));
    user_logins.insert(login);
}

bool Chat::loginUser(const std::string& login, const std::string& password) {
    for (const auto& user : users) {
        if (user.getLogin() == login && user.getPassword() == password) {
            return true;
        }
    }
    return false;
}

void Chat::sendMessage(const std::string& from, const std::string& to, const std::string& message) {
    if (loginUser(from, "")) {
        throw std::runtime_error("Отправитель не вошел в чат.");
    }
    if (loginUser(to, "")) {
        throw std::runtime_error("Получатель не вошел в чат.");
    }

    messages.insert({ from + " -> " + to, message });
    trie.insert(message); // Добавляем сообщение в Trie
}

std::string Chat::readMessages(const std::string& user) {
    std::string result = "Сообщения для " + user + ":\n";
    for (const auto& message : messages) {
        if (message.first.find(user) != std::string::npos) {
            result += message.first + ": " + message.second + "\n";
        }
    }
    return result;
}

std::vector<std::string> Chat::autocomplete(const std::string& prefix) {
    return trie.autocomplete(prefix);
}

void Chat::startNetworkService() {
    WSADATA wsaData;
    int iResult;

    // Инициализация Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed: " << iResult << "\n";
        return;
    }

    struct addrinfo* result = NULL, * ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Разрешение адреса сервера и порта
    int serverPort = 111;
    std::string portStr = std::to_string(serverPort);
    iResult = getaddrinfo(NULL, portStr.c_str(), &hints, &result);
    if (iResult != 0) {
        std::cout << "getaddrinfo failed: " << iResult << "\n";
        WSACleanup();
        return;
    }

    // Создание сокета для подключения к серверу
    serverSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Error at socket(): " << WSAGetLastError() << "\n";
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    // Настройка TCP-сокета
    int tcpNoDelay = 1;
    setsockopt(serverSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&tcpNoDelay, sizeof(int));

    // Привязка сокета
    iResult = bind(serverSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cout << "bind failed with error: " << WSAGetLastError() << "\n";
        freeaddrinfo(result);
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    // После того как адрес привязан, объект addrinfo больше не нужен
    freeaddrinfo(result);

    // Прослушивание входящих соединений
    iResult = listen(serverSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::cout << "listen failed with error: " << WSAGetLastError() << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return;
    }
}