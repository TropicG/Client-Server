#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <tchar.h>

#define MAX_ROWS 32
#define MAX_COLLUMS 32

using namespace std;

//this instance will be send between the server and the client
struct Data {
	int matrix[MAX_ROWS][MAX_COLLUMS];
};

//different threads will fill different rows of the matrix
void fillMatrix(Data& data, const int& row, const int& collumn ,int number) {
	for (int i = 0; i < collumn; i++) {
		data.matrix[row][i] = number;
	}
}

int main() {

	//starting the server
	int port = 55555;
	WSADATA wsaData;

	//launcing the server and making data for it
	int wsaerr;
	WORD wVersionRequsted = MAKEWORD(2, 2);
	wsaerr = WSAStartup(wVersionRequsted, &wsaData);
	if (wsaerr != 0) {
		cout << "The Winsock dll not found!" << endl;
		return 0;
	}

	//creating the sockets
	//socket for the server and the accepting of clients
	SOCKET serverSocket, acceptSocket;

	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET) {

		cout << "Error at socket()" << endl;
		WSACleanup();
		return 0;
	}

	//binding the firstSocket
	sockaddr_in service;

	service.sin_family = AF_INET;
	InetPton(AF_INET, _T("127.0.0.1"), &service.sin_addr.s_addr);
	service.sin_port = htons(port);

	if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
		cout << "Error at bind()" << endl;
		closesocket(serverSocket);
		WSACleanup();
		return 0;
	}
	else cout << "Successfully binded the server" << endl;

	//Listening for the clients
	if (listen(serverSocket, 1) == SOCKET_ERROR) {
		cout << "There is an error on the listen() function" << endl;
	}

	//Accepting the client connection
	acceptSocket = accept(serverSocket, NULL, NULL);
	if (acceptSocket == INVALID_SOCKET) {
		cout << "accept failed" << endl;
		WSACleanup();
		return -1;
	}
	else cout << "Successfully accepted a client" << endl;

	//Accepting the information for the matrix from the client
	int rows = 0;
	int byteCount = recv(acceptSocket, (char*)&rows, sizeof(rows), 0);
	if (byteCount <= 0) WSACleanup();

	int collums = 0;
	byteCount = recv(acceptSocket, (char*)&collums, sizeof(collums), 0);
	if (byteCount <= 0) WSACleanup();

	//accepting the matrix from the client
	Data data;
	byteCount = recv(acceptSocket, (char*)&data, sizeof(data), 0);
	if (byteCount <= 0) WSACleanup();
	else std::cout << "Successfully recieved information from the client" << endl;


	//Creating the little threads that will fill the table
	for (int i = 0; i < rows; i++) {
		thread thread_obj(fillMatrix, ref(data), i, collums, i + 1);
		thread_obj.join();
	}
	

	//Once the data is ready we are sending it to the client
	std::cout << "Sending modified data back to the client" << endl;

	byteCount = send(acceptSocket, (char*)&data, sizeof(data), 0);
	if (byteCount <= 0) WSACleanup();

	closesocket(acceptSocket);
	system("pause");
	WSACleanup();
	return 0;
}