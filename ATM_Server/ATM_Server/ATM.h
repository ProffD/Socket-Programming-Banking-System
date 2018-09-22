#pragma once
#include<string>


class ATM
{
public:
	ATM();
	float getbalance()const;
	bool withdraw(float amount);
	std::string insuffientFund()const;
	void deposite(float amount);
	std::string print()const;
	void writeToFile();
	~ATM();

private:
	float amount;
    float balance;
	

};

