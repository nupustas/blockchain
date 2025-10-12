#include "manolib.h"
#include "user.h"
#include "transaction.h"
#include "functions.h"

int main()
{
srand(static_cast<unsigned int>(time(0)));

std::vector<User> users;

//generate users
for (int i = 0; i < 1000; ++i) {
	string name = generateName();
	double balance = generateAmount();
	string public_key = hashas(name);

	users.push_back(User(name, balance, public_key));
}


for (size_t i = 0; i < users.size() && i < 100; ++i) {
	cout << i+1 << ") Name: " << users[i].getName()
		 << ", Balance: " << users[i].getBalance()
		 << ", Public key: " << users[i].getPublic_key()<<endl;
}

cout << "Total users generated: " << users.size() <<endl;
}