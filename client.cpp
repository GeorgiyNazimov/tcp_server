#include <iostream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

using namespace std;


int main(void)
{
	
	//данные сервера
	
	const char SERVER_IP[] = "127.0.0.1";	
	const short SERVER_PORT_NUM = 9876;
	const short BUFF_SIZE = 1024;

	
	int erStat;
	in_addr ip_to_num;
	erStat=inet_pton(AF_INET, SERVER_IP, &ip_to_num);

	WSADATA wsData;
	erStat = WSAStartup(MAKEWORD(2, 1), &wsData);

	if (erStat != 0) {
		cout << "Error WinSock version initialization";
		WSACleanup();
		return 0;
	}
	else
		cout << "WinSock initialization is OK" << endl;
	
	//создаём сокет клиента
	
	SOCKET ClientSock = socket(AF_INET, SOCK_STREAM, 0);

	if (ClientSock == INVALID_SOCKET) {
		cout << "Error initialization socket" << endl;
		closesocket(ClientSock);
		WSACleanup();
		return 0;
	}
	else
		cout << "Client socket initialization is OK" << endl;

	sockaddr_in servInfo;
	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(SERVER_PORT_NUM);

	//подключаем клиентский сокет к серверу
	
	erStat = connect(ClientSock, (sockaddr*)&servInfo, sizeof(servInfo));

	if (erStat != 0) {
		cout << "Connection to Server is FAILED" << endl;
		closesocket(ClientSock);
		WSACleanup();
		return 0;
	}
	else
		cout << "Connection established SUCCESSFULLY. Ready to send a message to Server" << endl;

	string clientBuff;
	short packet_size = 0;

	while (true) {

		cout << "Your (Client) message to Server: ";
		
		//вводим сообщение для сервера
		
		getline(cin, clientBuff);
		
		//отправляем сообщение на сервер

		packet_size = send(ClientSock, clientBuff.data(), clientBuff.size(), 0);

		if (packet_size == SOCKET_ERROR) {
			cout << "Can't send message to Server" << endl;
			closesocket(ClientSock);
			WSACleanup();
			return 0;
		}
		
		//закрываем клиентский сокет если мы ввели exit
		
		if (clientBuff=="exit") {
			cout << "GoodBye!" << endl;
			closesocket(ClientSock);
			WSACleanup();
			return 0;
		}


	}

	closesocket(ClientSock);
	WSACleanup();

	return 0;
}