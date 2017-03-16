#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iphlpapi.h>
#include <stdio.h>
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 1024
#pragma comment(lib, "Ws2_32.lib")


int main(int argc, char **argv)
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	char sendbuf[DEFAULT_BUFLEN];
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;


	// Validate the parameters
	if (argc != 2) {
		printf("usage: %s server-name\n%d", argv[0],argc);
		while (1);
		return 1;
	}


	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		while (1);
		return 1;
	}


	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;


	// Resolve the server address and port
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		while (1);
		return 1;
	}
	printf("argv[0] is :%s, argv[1] is: %s\n", argv[0], argv[1]);
	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {


		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			while (1);
			return 1;
		}


		// Connect to server.


		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			printf("socket failed with error\n");
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		printf("i'm going to break\n");
		break;
	}


	freeaddrinfo(result);


	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		while (1);
		return 1;
	}


	// Send an initial buffer
	scanf_s(" %s", sendbuf, DEFAULT_BUFLEN);
	printf("strlen(sendbuf) is %d, sendbuf is %s\n", strlen(sendbuf), sendbuf);
	/ndbuf[strlen(sendbuf)] = '\0';
	while (strcmp("!", sendbuf) != 0) {

		iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf) + 1, 0);
		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			while (1);
			return 1;
		}


		printf("client Sent: %s\n", sendbuf);


		// shutdown the connection since no more data will be sent
		/*
		iResult = shutdown(ConnectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		while (1);
		return 1;
		}
		*/
		// Receive until the peer closes the connection
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			printf("Client received: %s\n", recvbuf);
		scanf_s("%s", sendbuf, DEFAULT_BUFLEN);
		/ndbuf[strlen(sendbuf)] = '\0';
	}
	// cleanup


	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		while (1);
		return 1;
	}


	closesocket(ConnectSocket);
	WSACleanup();
	while (1);
	return 0;
}
