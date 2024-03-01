#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib,"ws2_32.lib")
#include<stdio.h>
#include<Winsock2.h>
#include<stdlib.h>
#include<Windows.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define PORT 9734

const char szHost[] = "www.google.com";

#define PORT "9734"
#define SERVER "192.168.1.73" // and localhost 

void client() {
    //	printf("Init socket...\n");
    //
    //	// Init socket
    //	WSAData wsaData;
    //	WORD DllVersion = MAKEWORD(2, 1);
    //	if (WSAStartup(DllVersion, &wsaData) != 0) {
    //		ExitProcess(EXIT_FAILURE);
    //	}
    //
    //	printf("Make socket...\n");
    //
    //	// Make socket
    //	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    //	if (sock < 0) {
    //		ExitProcess(EXIT_FAILURE);
    //	}
    //
    //	// Get server info
    ////	HOSTENT* host = gethostbyname(szHost);
    //
    //	printf("inet_addr(\"192.168.1.74\")...\n");
    //
    //	struct in_addr addr = { 0 };
    //	addr.s_addr = inet_addr("192.168.1.74");
    //	//addr.s_addr = inet_addr("192.168.1.67");
    //	//addr.s_addr = inet_addr("127.0.0.1");
    //	if (addr.s_addr == INADDR_NONE) {
    //		printf("The IPv4 address entered must be a legal address\n");
    //		return 1;
    //	}
    //
    //	printf("gethostbyaddr()...\n");
    //
    //	HOSTENT* host = gethostbyaddr((char*)&addr, 4, AF_INET);
    //	//pHostInfo = gethostbyaddr((LPCTSTR)& iaHost, sizeof(struct in_addr), AF_INET);
    //	//HOSTENT* host = gethostbyaddr((char*)&addr, sizeof(struct in_addr), AF_INET);
    //	if (host == nullptr) {
    //		printf("failed gethostbyaddr()...\n");
    //		DWORD dwError = WSAGetLastError();
    //
    //		if (dwError != 0) {
    //			if (dwError == WSAHOST_NOT_FOUND) {
    //				printf("Host not found!\n");
    //				return 1;
    //			}
    //			else if (dwError == WSANO_DATA) {
    //				printf("No data record found!\n");
    //				return 1;
    //			}
    //			else {
    //				printf("Function failed with error code %ld\n", dwError);
    //				return 1;
    //			}
    //		}
    //		ExitProcess(EXIT_FAILURE);
    //	}
    //
    //	printf("after gethostbyaddr()...\n");
    //
    //	// Define server info
    //	SOCKADDR_IN sin;
    //	ZeroMemory(&sin, sizeof(sin));
    //	sin.sin_port = htons(PORT);
    //	sin.sin_family = AF_INET;
    //	memcpy(&sin.sin_addr.S_un.S_addr, host->h_addr_list[0], sizeof(sin.sin_addr.S_un.S_addr));
    //
    WSADATA wsadata;
    if ((WSAStartup(MAKEWORD(2, 0), &wsadata)) != 0) {
        printf("-WSAStartup Not Initialized.\n");
        exit(1);
    }

    struct addrinfo hints, * res;
    int sockfd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(SERVER, PORT, &hints, &res) != 0) {
        printf("-getaddrinfo unsuccessful.\n");
        exit(1);
    }

    if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
        printf("-Unable to create socket.\n");
        exit(1);
    }
    if ((connect(sockfd, res->ai_addr, res->ai_addrlen)) == -1) {
        printf("-Connection Not Established..\n");
        exit(1);
    }
    //// Connect to server
    //printf("Connecting...\n");
    //if (connect(sockfd, (const sockaddr*)& sin, sizeof(sin)) != 0) {
    //	ExitProcess(EXIT_FAILURE);
    //}
    printf("Connected...\n");
    char szBuffer[4096];
    char szTemp[4096];
    //while (recv(sock, szTemp, 4096, 0)) {
    //	strcat(szBuffer, szTemp);
    //}
    //printf("%s\n", szBuffer);
    char ch[2];
    ch[1] = '\0';
    ch[0] = 'A';
    printf("Sending A...\n", ch[0]);
    send(sockfd, &ch[0], 1, 0);
    printf("Receiving B...\n", ch[0]);
    recv(sockfd, &ch[0], 1, 0);
    printf("Server: |%c|\n", ch[0]);
    //ExitProcess(EXIT_SUCCESS);
}
