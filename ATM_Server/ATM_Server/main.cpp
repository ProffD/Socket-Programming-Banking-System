#include<iostream>
#include<string>
#include <WS2tcpip.h>
#include<chrono>
#include<thread>
#include<iomanip>
#include<sstream>
#include "ATM.h"

#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable : 4996)

void wait(); // Wait 5 seconds before closing Server Console

void main() {
	ATM atm;
    

	//Initialize WinSock
	WSAData winSockData;
	WORD version = MAKEWORD(2, 2);
	int winSockOk = WSAStartup(version, &winSockData); //Will return 0 if successful
	if (winSockOk != 0) {
		std::cerr << "ERROR: Could not Initialize WinSocket" << '\n';
		return;
	}

	//Create a Socket
	SOCKET listeningSock = socket(AF_INET, SOCK_STREAM, 0); //TCP IPV4
	if (listeningSock == INVALID_SOCKET) {
		std::cerr << "ERROR: Could not create a Socket" << '\n';
		return;
	}

	//Bind the IP address and Port to the Socket 
	sockaddr_in address;                         //create an instance of a structure then fill it in.
	address.sin_family = AF_INET;               //AF_INET - Addr family using INET version 4
	address.sin_port = htons(58000);
	address.sin_addr.S_un.S_addr = INADDR_ANY;  //OR you can specify your ip i.e address.sin_addr.s_addr = inet_addr("127.0.0.1")

	bind(listeningSock, (sockaddr*)&address, sizeof(address));
	listen(listeningSock, SOMAXCONN);           //Let winSock the sockect is for listening,SOMAXCONN- listen with maximum connection

	std::cout << "Server is up and running!\nPlease Connect..." << '\n';

												//Wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);
	SOCKET clientSocket = accept(listeningSock, (sockaddr*)&client, &clientSize);

	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "ERROR: Could not create a Client Socket" << '\n';
		return;
	}
	
	char host[NI_MAXHOST];         //Node buffer
	char service[NI_MAXSERV];
	ZeroMemory(host, NI_MAXHOST);  //Fills a block of memory with zeros.
	ZeroMemory(host, NI_MAXSERV);
	if (getnameinfo((sockaddr*)&client, clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) { // DNS lookup
		std::cout << "Host Name Connected: " << host << "\nHost Port: " << service << '\n';
		
		
	}
	else {//Otherwise lets show Host IP
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << "Host IP Connected: " << host << "\nHost Port: " << ntohs(client.sin_port) << '\n';

	}

	//Date Time
	std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(today);
	std::tm tm = *std::localtime(&tt);
	std::stringstream dtbuffer;
	dtbuffer << "Date: " << std::put_time(&tm, "%A %d %b %Y");
	dtbuffer << "\nTime: " << std::put_time(&tm, "%H:%M:%S");
	std::string dateTime = dtbuffer.str();

	std::string welcome_menu = "==========WELCOME TO IBANK!!===============\n"+dateTime+"\nTo make a Transaction,Please Select One of the following Options\nB)alance\nD)eposit\nW)ithdraw\nP)rint Slip\nE)xit or press Enter to Exit Program\n";
	send(clientSocket, welcome_menu.c_str(), welcome_menu.size() + 1, 0);

	char buf[256];
	while (true) {
		ZeroMemory(buf, 256);
		//Wait for client to send data
		int receivedBytes = recv(clientSocket, buf, 256, 0);
		if (receivedBytes == SOCKET_ERROR) {
			std::cout << "ERROR: Could not recieve data!" << '\n';
			break;
		}
		//If the connection has been gracefully closed
		if (receivedBytes == 0 || std::string(buf, 0, receivedBytes) == "E" || std::string(buf, 0, receivedBytes) == "e") {
			std::cout << "Host disconnected!" << '\n';
			std::string disconnected = "Server disconnected!\nPlease press Enter to close  the terminal";
			send(clientSocket,disconnected.c_str() , disconnected.size() + 1, 0);
			break;
		}
		
		if (receivedBytes > 0) {

			if (std::string(buf, 0, receivedBytes) == "B" || std::string(buf, 0, receivedBytes) == "b") {
				std::string bal = "Balance: R" + std::to_string(atm.getbalance());	
				send(clientSocket, bal.c_str(), bal.size() + 1, 0);
			}
			else if (std::string(buf, 0, receivedBytes) == "D" || std::string(buf, 0, receivedBytes) == "d") {
				//std::string amount = "How much do want to deposite?";
				//send(clientSocket, amount.c_str(), amount.size() + 1, 0);
				//auto rec = recv(clientSocket, buf, 256, 0);				
				//atm.deposite(float(rec));	

				float amount = 600.0f; // needs to come from client
				atm.deposite(amount);
				std::string deposite = "Deposited: R" + std::to_string(amount) + "\nBalance: R" + std::to_string(atm.getbalance());

			
				send(clientSocket, deposite.c_str(), deposite.size() + 1, 0);

			}
			else if (std::string(buf, 0, receivedBytes) == "W" || std::string(buf, 0, receivedBytes) == "w") {
				//std::string amount = "How much do want to withdraw?";
				float amount = 200.0f; // needs to come from client
				if (atm.withdraw(amount)) {
					std::string withdraw = "Requested Amount: R" + std::to_string(amount) + "\nBalance: R" + std::to_string(atm.getbalance());

					send(clientSocket, withdraw.c_str(), withdraw.size() + 1, 0);
				}
				else
				{
					std::string insuf = atm.insuffientFund();
					send(clientSocket, insuf.c_str(), insuf.size() + 1, 0);
				}
				
			}
			else if (std::string(buf, 0, receivedBytes) == "P" || std::string(buf, 0, receivedBytes) == "p") {
				std::string display = atm.print();

				send(clientSocket, display.c_str(), display.size() + 1, 0);
			}
			else {
				
				std::string invalid = "Invalid Selection!!\nPlease Select from the menu given above!";
				send(clientSocket, invalid.c_str(), invalid.size() + 1, 0);
				
			}



		}
		
	}

	// Close a socket
	closesocket(listeningSock);
	closesocket(clientSocket);

	//do clean Up
	WSACleanup();
	wait();	  // Wait 5 seconds before closing Server Console

	//std::cin.get();
}


void wait() {
	using namespace std::literals::chrono_literals;
	std::this_thread::sleep_for(5s);
}