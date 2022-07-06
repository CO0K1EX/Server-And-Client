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

DWORD WINAPI serverReceive(LPVOID lpParam) { //Получение данных от клиента
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	char buffer[1024] = { 0 }; //Буфер для данных
	SOCKET client = *(SOCKET*)lpParam; //Сокет для клиента
	while (true) { //Цикл работы сервера
		if (recv(client, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {
			//Если не удалось получить данные буфера, сообщить об ошибке и выйти
			SetConsoleTextAttribute(hConsole, Red);
			cout << "recv function failed with error " << WSAGetLastError() << endl;
			SetConsoleTextAttribute(hConsole, White);
			return -1;
		}
		if (strcmp(buffer, "exit\n") == 0) { //Если клиент отсоединился
			SetConsoleTextAttribute(hConsole, Yellow);
			cout << "Client Disconnected." << endl;
			break;
			SetConsoleTextAttribute(hConsole, White);
		}
		SetConsoleTextAttribute(hConsole, Blue);
		cout << "\nClient: " << buffer << endl; //Иначе вывести сообщение от клиента из буфера
		SetConsoleTextAttribute(hConsole, White);
		memset(buffer, 0, sizeof(buffer)); //Очистить буфер
	}
	return 1;
}

DWORD WINAPI serverSend(LPVOID lpParam) { //Отправка данных клиенту
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	char buffer[1024] = { 0 };
	SOCKET client = *(SOCKET*)lpParam;
	while (true) {
		fgets(buffer, 1024, stdin);
		if (send(client, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {
			SetConsoleTextAttribute(hConsole, Red);
			cout << "send failed with error " << WSAGetLastError() << endl;
			SetConsoleTextAttribute(hConsole, White);
			return -1;
		}
		if (strcmp(buffer, "exit\n") == 0) {
			SetConsoleTextAttribute(hConsole, Gray);
			cout << "\nThank you for using the application" << endl;
			SetConsoleTextAttribute(hConsole, White);
			break;
		}
	}
	return 1;
}

int main() {
	system("title ServerX");
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	WSADATA WSAData; //Данные 
	SOCKET server, client; //Сокеты сервера и клиента
	SOCKADDR_IN serverAddr, clientAddr; //Адреса сокетов
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	server = socket(AF_INET, SOCK_STREAM, 0); //Создали сервер
	if (server == INVALID_SOCKET) {
		SetConsoleTextAttribute(hConsole, Red);
		cout << "Socket creation failed with error:" << WSAGetLastError() << endl;
		SetConsoleTextAttribute(hConsole, White);
		return -1;
	}
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(5555);
	if (bind(server, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		SetConsoleTextAttribute(hConsole, Red);
		cout << "Bind function failed with error: " << WSAGetLastError() << endl;
		SetConsoleTextAttribute(hConsole, White);
		return -1;
	}

	if (listen(server, 0) == SOCKET_ERROR) { //Если не удалось получить запрос
		SetConsoleTextAttribute(hConsole, Red);
		cout << "Listen function failed with error:" << WSAGetLastError() << endl;
		SetConsoleTextAttribute(hConsole, White);
		return -1;
	}
	SetConsoleTextAttribute(hConsole, Red);
	cout << R"(
  _____                       __   __
 / ____|                      \ \ / /
| (___   ___ _ ____   _____ _ _\ V / 
 \___ \ / _ \ '__\ \ / / _ \ '__> <  
 ____) |  __/ |   \ V /  __/ | / . \ 
|_____/ \___|_|    \_/ \___|_|/_/ \_\
Version 1.0.0
Creator By CO0K1E
)" << endl << endl;
	SetConsoleTextAttribute(hConsole, Gray);
	cout << "Listening for incoming connections...." << endl << endl;

	char buffer[1024]; //Создать буфер для данных
	int clientAddrSize = sizeof(clientAddr); //Инициализировать адерс клиента
	if ((client = accept(server, (SOCKADDR*)&clientAddr, &clientAddrSize)) != INVALID_SOCKET) {
		//Если соединение установлено
		SetConsoleTextAttribute(hConsole, Green);
		cout << "\nStatus:Client connected!" << endl;
		SetConsoleTextAttribute(hConsole, White);
		cout << "--------------------------" << endl << endl;
		SetConsoleTextAttribute(hConsole, Gray);
		cout << "Now you can use our live chat application. " << "Enter \"exit\" to disconnect" << endl;
		cout << "---------------------------------------------------------------------------" << endl << endl;
		SetConsoleTextAttribute(hConsole, White);
		DWORD tid; //Идентификатор
		HANDLE t1 = CreateThread(NULL, 0, serverReceive, &client, 0, &tid); //Создание потока для получения данных
		if (t1 == NULL) { //Ошибка создания потока
			SetConsoleTextAttribute(hConsole, Red);
			cout << "Thread Creation Error: " << WSAGetLastError() << endl;
		}
		HANDLE t2 = CreateThread(NULL, 0, serverSend, &client, 0, &tid); //Создание потока для отправки данных
		if (t2 == NULL) {
			SetConsoleTextAttribute(hConsole, Red);
			cout << "Thread Creation Error: " << WSAGetLastError() << endl;
		}

		WaitForSingleObject(t1, INFINITE);
		WaitForSingleObject(t2, INFINITE);

		closesocket(client); //Закрыть сокет
		if (closesocket(server) == SOCKET_ERROR) { //Ошибка закрытия сокета
			SetConsoleTextAttribute(hConsole, Red);
			cout << "Close socket failed with error: " << WSAGetLastError() << endl;
			SetConsoleTextAttribute(hConsole, White);
			return -1;
		}
		WSACleanup();
	}
}