// server.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <tchar.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

typedef struct MyData {
	SOCKET s;
	SOCKADDR_IN sa;
} MYDATA, * PMYDATA;
char c;
DWORD WINAPI Send(LPVOID lpParam)
{
	PMYDATA pData = (PMYDATA)lpParam;
	SOCKET s = pData->s;
	SOCKADDR_IN addr = pData->sa;
	
	while ((c = getchar()) != '/') {
		if (c == 10)
		{
			addr = pData->sa;
			printf("Enter message: ");

			char message[1024] = "";

			fgets(message, 1023, stdin);
			//		printf("%d", strlen(message));
			fflush(stdin);
			if (sendto(s, message, strlen(message) + 1, 0, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
				printf("Sending failed");
				return 1;
			}
		}
	}
	closesocket(s);
	return 0;
}
int main()
{
	WORD wVersioRequested;
	WSADATA wsaData;
	int err;

	wVersioRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersioRequested, &wsaData);
	if (err != 0) {
		printf("WSAStartup failed with error: %d\n", err);
		return 1;
	}

	SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s == INVALID_SOCKET) {
		printf("Invalid socket");
		return 1;
	}

	//setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
	SOCKADDR_IN sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(2000);
	InetPton(AF_INET, _T("192.168.233.1"), &sa.sin_addr.S_un.S_addr);

	if (bind(s, (SOCKADDR*)&sa, sizeof(sa)) == SOCKET_ERROR)
	{
		printf("Bind error");
		return 1;
	}

	SOCKADDR_IN nsa;
	int sizeof_nsa = sizeof(nsa);

	char buffer[1024];
	int buffer_size = sizeof(buffer);
	
	if (recvfrom(s, buffer, buffer_size, 0, (SOCKADDR*)&nsa, &sizeof_nsa) == SOCKET_ERROR) {
		printf("SOCKET_ERROR");
		return 1;
	}
	else printf("%s", buffer);

	PMYDATA pData;
	DWORD dwThreadId;
	HANDLE hThread;

	pData = (PMYDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
		sizeof(MYDATA));
	if (pData != NULL) {
		pData->s = s;
		pData->sa = nsa;
	}


	hThread = CreateThread(
		NULL,
		0,
		Send,
		pData,
		0,
		&dwThreadId
	);
	

	int iResult;
	do {
		iResult = recvfrom(s, buffer, buffer_size, 0, (SOCKADDR*)&nsa, &sizeof_nsa);
		if (iResult > 0)
		{
			printf("\nUser1: %s", buffer);
			pData->sa = nsa;
			//			printf("Enter message: ");
		}
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recvfrom failed with error: %d\n ", WSAGetLastError());

	} while (c != '/');


	WaitForMultipleObjects(1, &hThread, TRUE, INFINITE);
	if (hThread != NULL) CloseHandle(hThread);
	HeapFree(GetProcessHeap(), 0, pData);

	closesocket(s);
	
	WSACleanup();
	printf("Memory cleaned!");
	return 0;
}


