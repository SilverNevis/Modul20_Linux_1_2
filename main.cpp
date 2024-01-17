#include "Server.h"
#include "Client.h"
#include <iostream>
#include "Chat.h" 

int main() {

    // ������������� Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed: " << iResult << "\n";
        return 1;
    }

    // ����� ������ ������
    char choice;
    std::cout << "Run as server or client? (s/c): ";
    std::cin >> choice;

    if (choice == 's') {
        Server server;
        server.run();
    }
    else if (choice == 'c') {
        Client client;
        client.connectToServer("127.0.0.1", 111); // 

        std::string msg;
        do {
            std::cout << "> ";
            std::getline(std::cin, msg);

            client.sendMessage(msg);

            if (msg != "exit") {
                client.receiveMessage();
            }
        } while (msg != "exit");
    }
    else if (choice == 'r') { // �������� ����� ����� ��� ����
        setlocale(LC_ALL, "RUSSIAN");
        Chat chat;
        std::string current_user;

        while (true) {
            std::cout << "�������� ��������:\n1. �����������\n2. ����\n3. �����\n4. ��������� ���������\n5. ��������� ���������\n6. ��������������\n7. ����� �� ���������\n";
            int choice;
            std::cin >> choice;

            try {
                if (choice == 1) {
                    std::string login, password, name;
                    std::cout << "������� �����: ";
                    std::cin >> login;
                    std::cout << "������� ������: ";
                    std::cin >> password;
                    std::cout << "������� ���: ";
                    std::cin >> name;
                    chat.registerUser(login, password, name);
                }
                else if (choice == 2) {
                    if (current_user.empty()) {
                        std::string login, password;
                        std::cout << "������� �����: ";
                        std::cin >> login;
                        std::cout << "������� ������: ";
                        std::cin >> password;
                        if (chat.loginUser(login, password)) {
                            current_user = login;
                            std::cout << "�� ����� ���: " << current_user << "\n";
                        }
                        else {
                            std::cout << "������ �����. ��������� ����� � ������.\n";
                        }
                    }
                    else {
                        std::cout << "�� ��� ����� ���: " << current_user << "\n";
                    }
                }
                else if (choice == 3) {
                    current_user = "";
                    std::cout << "����� ��������.\n";
                }
                else if (choice == 4) {
                    if (current_user.empty()) {
                        std::cout << "�� �� ����� � ���. ������� ��� �����������������.\n";
                    }
                    else {
                        std::string to, message;
                        std::cout << "������� ����������: ";
                        std::cin >> to;
                        std::cin.ignore();
                        std::cout << "������� ���������: ";
                        std::getline(std::cin, message);
                        chat.sendMessage(current_user, to, message);
                        std::cout << "��������� ����������.\n";
                    }
                }
                else if (choice == 5) {
                    if (current_user.empty()) {
                        std::cout << "�� �� ����� � ���. ������� ��� �����������������.\n";
                    }
                    else {
                        std::cout << chat.readMessages(current_user) << std::endl;
                    }
                }
                else if (choice == 6) {
                    if (current_user.empty()) {
                        std::cout << "�� �� ����� � ���. ������� ��� �����������������.\n";
                    }
                    else {
                        std::string prefix;
                        std::cout << "������� �������: ";
                        std::cin >> prefix;
                        std::vector<std::string> words = chat.autocomplete(prefix);
                        for (const auto& word : words) {
                            std::cout << word << std::endl;
                        }
                    }
                }
                else if (choice == 7) {
                    break;
                }
                else {
                    std::cout << "�������� �����. ���������� ��� ���.\n";
                }
            }
            catch (const std::exception& e) {
                std::cerr << "������: " << e.what() << std::endl;
            }
        }
    }
    else {
        std::cout << "Invalid choice.\n";
    }

    return 0;
}
