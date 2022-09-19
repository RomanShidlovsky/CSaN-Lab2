// client.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
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
	
	
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		printf("Invalid socket");
		return 1;
	}

	SOCKADDR_IN sa;

	sa.sin_family = AF_INET;
	sa.sin_port = htons(10000);
	InetPton(AF_INET, _T("192.168.233.1"), &sa.sin_addr.S_un.S_addr);

	if (connect(sock, (SOCKADDR*)&sa, sizeof(sa)) < 0) {
		printf("Connect error");
		return 1;
	}

	DWORD dwThreadId;
	HANDLE hThread;

	hThread = CreateThread(
		NULL,
		0,
		Send,
		(LPVOID*)sock,
		0,
		&dwThreadId
	);

	int iResult;
	do {
		iResult = recv(sock, message, sizeof(message), 0);
		if (iResult > 0) {
			printf("\nUser2: %s", message);
		}
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());
	} while ((iResult > 0) && (c != '/'));

	WaitForMultipleObjects(1, &hThread, TRUE, INFINITE);

	closesocket(sock);
	WSACleanup();
	printf("Memory cleaned!");
	return 0;



	
}