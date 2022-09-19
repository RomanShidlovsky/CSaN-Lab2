// server.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <tchar.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")
char c;
DWORD WINAPI Send(LPVOID lpParam)
{
	SOCKET s = (SOCKET)lpParam;

	while ((c = getchar()) != '/') {
		if (c == 10)
		{
			printf("Enter message: ");

			char message[1024] = "";

			fgets(message, 1023, stdin);
			//		printf("%d", strlen(message));
			fflush(stdin);
			if (send(s, message, strlen(message) + 1, 0) == SOCKET_ERROR) {
				printf("Sending failed");
				return 1;
			}
		}
	}
	closesocket(s);

	return 0;
}
int main() {
	WORD  wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		printf("WSAStartup failed with error: %d\n", err);
		return 1;
	}

	char message[1024];
	char nickname[20];


	SOCKET lSock = socket(AF_INET, SOCK_STREAM, 0);
	if (lSock == INVALID_SOCKET) {
		printf("Invalid socket");
		return 1;
	}

	SOCKADDR_IN sa;

	sa.sin_family = AF_INET;
	sa.sin_port = htons(10000);
	InetPton(AF_INET, _T("192.168.233.1"), &sa.sin_addr.S_un.S_addr);

	if (bind(lSock, (SOCKADDR*)&sa, sizeof(sa)) == SOCKET_ERROR) {
		printf("Bind error");
		WSACleanup();
		return 1;

	}

	if (listen(lSock, 0) == SOCKET_ERROR) {
		printf("Listen failed");
		closesocket(lSock);
		WSACleanup();
		return 1;
	}

	SOCKET cSock = accept(lSock, NULL, NULL);
	if (cSock == INVALID_SOCKET)
	{
		printf("Accept failed");
		closesocket(lSock);
		WSACleanup();
		return 1;
	}
	else
	{
		printf("\nAccepted\n");
	}

	DWORD dwThreadId;
	HANDLE hThread;



	hThread = CreateThread(
		NULL,
		0,
		Send,
		(LPVOID*)cSock,
		0,
		&dwThreadId
	);




	int iResult;
	char buff[1024];
	do
	{
		iResult = recv(cSock, buff, sizeof(buff), 0);
		if (iResult > 0) {
			printf("\nUser1: %s", buff);
		}
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());
	} while (c != '/');
	WaitForMultipleObjects(1, &hThread, TRUE, INFINITE);

	closesocket(cSock);
	closesocket(lSock);
	WSACleanup();
	printf("Memory cleaned!");
	return 0;
}
