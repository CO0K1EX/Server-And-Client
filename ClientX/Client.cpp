#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream> 
#include <cstdio> 
#include <cstring> 
#include <winsock2.h> 
#pragma comment(lib, "WS2_32.lib")
using namespace std;

HANDLE hConsole;

enum ColorConsole {
	Blue = 1,
	Green = 2,
	Aqua = 3,
	Red = 4,
	Purple = 5,
	Yellow = 6,
	White = 7,
	Gray = 8
};

DWORD WINAPI clientReceive(LPVOID lpParam) { //Получение данных от сервера
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	char buffer[1024] = { 0 };
	SOCKET server = *(SOCKET*)lpParam;
	while (true) {
		if (recv(server, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {
			SetConsoleTextAttribute(hConsole, Red);
			cout << "recv function failed with error: " << WSAGetLastError() << endl;
			SetConsoleTextAttribute(hConsole, White);
			return -1;
		}
		if (strcmp(buffer, "exit\n") == 0) {
			SetConsoleTextAttribute(hConsole, Red);
			cout << "Server disconnected." << endl;
			SetConsoleTextAttribute(hConsole, White);
			return -1;
		}
		SetConsoleTextAttribute(hConsole, Red);
		cout << "\nServer:" << buffer << endl;
		SetConsoleTextAttribute(hConsole, White);
		memset(buffer, 0, sizeof(buffer));
	}
	return 1;
}

DWORD WINAPI clientSend(LPVOID lpParam) { //Отправка данных на сервер
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	char buffer[1024] = { 0 };
	SOCKET server = *(SOCKET*)lpParam;
	while (true) {
		fgets(buffer, 1024, stdin);
		if (send(server, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {
			SetConsoleTextAttribute(hConsole, Red);
			cout << "send failed with error: " << WSAGetLastError() << endl;
			SetConsoleTextAttribute(hConsole, White);
			return -1;
		}
		if (strcmp(buffer, "exit") == 0) {
			SetConsoleTextAttribute(hConsole, Gray);
			cout << "\nThank you for using the application" << endl;
			break;
		}
	}
	SetConsoleTextAttribute(hConsole, White);
	return 1;
}

int main() {
	system("title ClientX");
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	WSADATA WSAData;
	SOCKET server;
	SOCKADDR_IN addr;
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	if ((server = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		SetConsoleTextAttribute(hConsole, Red);
		cout << "Socket creation failed with error: " << WSAGetLastError() << endl;
		SetConsoleTextAttribute(hConsole, White);
		return -1;
	}

	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //коннект к серверу
	addr.sin_family = AF_INET;
	addr.sin_port = htons(5555); //порт
	if (connect(server, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		SetConsoleTextAttribute(hConsole, Red);
		cout << "Server connection failed with error: " << WSAGetLastError() << endl;
		SetConsoleTextAttribute(hConsole, White);
		return -1;
	}
	SetConsoleTextAttribute(hConsole, Red);
	cout << R"(
  _____ _ _            _  __   __
 / ____| (_)          | | \ \ / /
| |    | |_  ___ _ __ | |_ \ V / 
| |    | | |/ _ \ '_ \| __| > <  
| |____| | |  __/ | | | |_ / . \ 
 \_____|_|_|\___|_| |_|\__/_/ \_\
Version 1.0.0
Creator By CO0K1E
)" << endl << endl;
	SetConsoleTextAttribute(hConsole, Green);
	cout << "\nStatus:Connected to server!" << endl;
	SetConsoleTextAttribute(hConsole, White);
	cout << "--------------------------" << endl << endl;
	SetConsoleTextAttribute(hConsole, Gray);
	cout << "Now you can use our live chat application. " << "Enter \"exit\"to disconnect" << endl;
	SetConsoleTextAttribute(hConsole, Gray);
	cout << "---------------------------------------------------------------------------" << endl << endl;
	SetConsoleTextAttribute(hConsole, White);
	DWORD tid;
	HANDLE t1 = CreateThread(NULL,0,clientReceive,&server,0,&tid);
	SetConsoleTextAttribute(hConsole, Red);
	if (t1 == NULL) cout << "Thread creation error: " << WSAGetLastError();
	HANDLE t2 = CreateThread(NULL, 0, clientSend, &server, 0, &tid);
	SetConsoleTextAttribute(hConsole, Red);
	if (t2 == NULL) cout << "Thread creation error: " << WSAGetLastError();
	SetConsoleTextAttribute(hConsole, White);
	WaitForSingleObject(t1, INFINITE);
	WaitForSingleObject(t2, INFINITE);
	closesocket(server);
	WSAStartup;
}