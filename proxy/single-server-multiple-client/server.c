#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iphlpapi.h>
#include <stdio.h>
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 1024
#pragma comment(lib, "Ws2_32.lib")
SOCKET GLOBAL[100];
DWORD WINAPI handle_a_connect(LPVOID aa)
{
	SOCKET ClientSocket = *(SOCKET*)aa;
	SOCKADDR_IN sockAddr;
	int iLen = sizeof(sockAddr);
	//获取local ip and port
	ZeroMemory(&sockAddr, sizeof(sockAddr));
	getpeername(ClientSocket, (struct sockaddr *)&sockAddr, &iLen);//得到远程IP地址和端口号
	char *nSourceIP = inet_ntoa(sockAddr.sin_addr);
	int nSourcePort = sockAddr.sin_port;
	//获取remote ip and port
	ZeroMemory(&sockAddr, sizeof(sockAddr));
	getsockname(ClientSocket, (struct sockaddr *)&sockAddr, &iLen);//得到本地的IP地址和端口号
	char *nDestIP = inet_ntoa(sockAddr.sin_addr);//IP
	int nDestPort = sockAddr.sin_port;
	//端口号
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	do {
		iResult = recv(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);
		if (iResult > 0) {
			printf("Server received: %s from %s:%d(i am %s:%d)\n", recvbuf, nSourceIP, nSourcePort, nDestIP, nDestPort);

			// Echo the buffer back to the sender
			int iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				while (1);
				return 1;
			}
			printf("Server send: %s from %s:%d(i am %s:%d)\n", recvbuf, nSourceIP, nSourcePort, nDestIP, nDestPort);
		}
		else if (iResult == 0) {
			printf("Connection closing...\n");
			iResult = shutdown(ClientSocket, SD_SEND);
			if (iResult == SOCKET_ERROR) {
				printf("shutdown failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				while (1);
				return 1;
			}
			closesocket(ClientSocket);
		}
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			while (1);
			return 1;
		}
	} while (iResult > 0);
	return 0;
}
int main(void)
{
	WSADATA wsaData;
	int iResult;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL;
	struct addrinfo hints;
	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	//int recvbuflen = DEFAULT_BUFLEN;
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		while (1);
		return 1;
	}
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		while (1);
		return 1;
	}
	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		while (1);
		return 1;
	}
	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		while (1);
		return 1;
	}
	freeaddrinfo(result);
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		while (1);
		return 1;
	}
	// Accept a client socket
	int i = 0;
	while (1) {
		ClientSocket = accept(ListenSocket, NULL, NULL);
		i = (i + 1) % 100;
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			while (1);
			return 1;
		}
		GLOBAL[i] = ClientSocket;
		HANDLE hand = CreateThread(NULL, 0, handle_a_connect, &GLOBAL[i], 0, NULL);
	}
	// No longer need server socket
	closesocket(ListenSocket);
	WSACleanup();
	while (1);
	return 0;
}
