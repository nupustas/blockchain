#pragma once
#include "manolib.h"

class User{
private:
    string name;
    double balance;
    string public_key;
public:
  
    User(string n, double b, string k): name(n), balance(b), public_key(k) {}
    ~User()=default;

    string getName() const{return name;}
    double getBalance()const{return balance;}
    string getPublic_key()const{return public_key;}

    void setName(const string n){name=n;}
    void setBalance(const double b){balance=b;}
    void setPublic_key(const string k){public_key=k;}
};
