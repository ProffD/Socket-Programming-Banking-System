#include "ATM.h"
#include<iostream>
#include<chrono>
#include<iomanip>
#include<sstream>

#pragma warning(disable : 4996)

ATM::ATM():amount(0.0f),balance(500.0f)
{
}

float ATM::getbalance() const
{
	return balance;
}

bool ATM::withdraw(float amount)
{
	if (balance >= amount) {
		balance -= amount;
		return true;
	}
		
	return false;
		
}

std::string ATM::insuffientFund() const
{
	return std::string("Insufficient fund\nYou have: R" + std::to_string(getbalance()) + " left");
}

void ATM::deposite(float amount)
{
	balance += amount;
}

std::string ATM::print() const
{
	std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
	time_t tt = std::chrono::system_clock::to_time_t(today);
	std::tm tm = *std::localtime(&tt);
	std::stringstream dtbuffer;
	dtbuffer << "Transaction Date: " << std::put_time(&tm, "%A %d %b %Y");
	dtbuffer << "\nTransaction Time: " << std::put_time(&tm, "%H:%M:%S");
	std::string dateTime = dtbuffer.str();

	return std::string(dateTime + "\nBalance: R" + std::to_string(getbalance()));
}

void ATM::writeToFile()
{
}


ATM::~ATM()
{
}
