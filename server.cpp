#include <iostream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

void handle_client(int client_socket) {

    while (true) {

        char buffer[1024] = "";
        
        //принимаем сообщение клиента

        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        
        //выводим информацию об отключении клиента, если пришло сообщение exit

        if (buffer[0] == 'e' && buffer[1] == 'x' && buffer[2] == 'i' && buffer[3] == 't')
        {
            cout << "!!!!!ONE DISCONNECT!!!!!" << endl;
            break;
        }

        if (bytes_received > 0) {
            std::cout <<"From "<<client_socket<< " Received: " << buffer << std::endl;
        }
    }
    closesocket(client_socket);
}

int main(void)
{

    //данные сервера
    
    const char IP_SERV[] = "127.0.0.1";
    const int PORT_NUM = 9876;
    const short BUFF_SIZE = 1024;

    int erStat;
    in_addr ip_to_num;
    erStat = inet_pton(AF_INET, IP_SERV, &ip_to_num);

    WSADATA wsData;

    erStat = WSAStartup(MAKEWORD(2, 1), &wsData);

    if (erStat != 0) {
        cout << "WinSock version initialization is FAILED";
        WSACleanup();
        return 0;
    }
    else
        cout << "WinSock version initialization is OK" << endl;

    //создаём сокет сервера

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == INVALID_SOCKET) {
        cout << "Server socket initialization is FAILED" << endl;
        closesocket(server_socket);
        WSACleanup();
        return 0;
    }
    else
        cout << "Server socket initialization is OK" << endl;

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr = ip_to_num;
    server_address.sin_port = htons(PORT_NUM);

    //связываем сокет сервера с ip и портом сервера

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Server socket binding is FAILED" << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return 0;
    }
    else
        cout << "Server socket binding is OK" << endl;

    //переводим сокет сервера в режим ожидания новых подключений

    if (listen(server_socket, SOMAXCONN) == -1) {
        std::cerr << "Server socket listening if FAILED" << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return 0;
    }
    else
        cout << "Server socket listening is OK" << endl;

    //создаём массив потоков для обработки подключившихся клиентов

    vector<thread> client_threads;

    while (true) {

        sockaddr_in client_address;
        socklen_t client_address_size = sizeof(client_address);

        //принимаем запрос на подключение клиента к серверу и создаём отдельный поток для его обработки

        SOCKET client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_size);
        if (client_socket == -1) {
            std::cerr << "Error accepting client connection" << std::endl;
            continue;
        }
        else
        {
            cout << "!!!!!NEW CONNECT!!!!!" << endl;
            client_threads.emplace_back([client_socket]() {
                handle_client(client_socket);
            });
        }

    }

    return 0;
}