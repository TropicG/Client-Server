#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <tchar.h>

#define MAX_ROWS 32
#define MAX_COLLUMS 32

//this instance will be send between the server and the client
struct Data {
	int matrix[MAX_ROWS][MAX_COLLUMS];
};

using namespace std;


int main() {

	//setting it up
	int port = 55555;
	WSADATA wsaData;

	int wsaerr;
	WORD wVersionRequired = MAKEWORD(2, 2);
	wsaerr = WSAStartup(wVersionRequired, &wsaData);
	if (wsaerr != 0) {
		cout << "The Winsock dll not found!" << endl;
		return 0;
	}


	//create the socket
	SOCKET clientSocket;
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (clientSocket == INVALID_SOCKET) {
		cout << "Error at the socket()" << endl;
		WSACleanup();
		return 0;
	}

	//connect to the server
	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	InetPton(AF_INET, _T("127.0.0.1"), &clientService.sin_addr.s_addr);
	clientService.sin_port = htons(port);

	if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
		cout << "There was error in connect()" << endl;
		WSACleanup();
		return 0;
	}

	//getting the information about the rows and collums for the matrix
	int rows = 0, collums = 0;
	std::cout << "Enter the rows of the matrix: ";
	std::cin >> rows;

	std::cout << "Enter the collums of the matrix: ";
	std::cin >> collums;

	//sending the rows
	int byteCount = send(clientSocket, (char*)&rows, sizeof(rows), 0);
	if (byteCount <= 0) WSACleanup();
	

	byteCount = send(clientSocket, (char*)&collums, sizeof(collums), 0);
	if (byteCount <= 0) WSACleanup();

	//initializing Data variable
	Data data;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < collums; j++) {
			data.matrix[i][j] = 0;
		}
	}


	//sending this data to the server
	byteCount = send(clientSocket, (char*)&data, sizeof(data), 0);
	if (byteCount <= 0) WSACleanup();

	//recieve the struct
	byteCount = recv(clientSocket, (char*)&data, sizeof(data), 0);
	if (byteCount <= 0) WSACleanup();

	//printing the recieved information from the server
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < collums; j++) {
			std::cout << data.matrix[i][j] << " ";
		}
		std::cout << endl;
	}

	closesocket(clientSocket);
	system("pause");
	WSACleanup();
}