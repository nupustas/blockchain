#include "manolib.h"
#include "user.h"
#include "transaction.h"
#include "functions.h"
#include "block.h"


int main()
{
srand(static_cast<unsigned int>(time(0)));

vector<User> users;
vector<Transaction> transactions;
vector<Block> blocks;


	generateUsers(users);

	int numToCreate = 10000; 
	int created = generateTransactions(users, transactions, numToCreate);
	cout << "Total transactions generated: " << created << "\n";

	 // mining setup
    string prev_hash(64, '0'); // Genesis block previous hash (64 hex zeros)
    int blockIndex = 0;
    const size_t txPerBlock = 100;
	cout<<"enter hash difficulty: ";
    int difficulty = 1;   
	cin>>difficulty;

    // mining blocks
    for (size_t offset = 0; offset < transactions.size(); offset += txPerBlock) {
        size_t end = min(offset + txPerBlock, transactions.size());
        vector<Transaction> batch(transactions.begin() + offset, transactions.begin() + end);

        Block blk(blockIndex, prev_hash, batch, batch.size());

        cout << "Mining block " << blockIndex
                  << " with " << batch.size() << " transactions..." <<endl;


        bool solved = blk.mine(difficulty);

    cout << blk;

    blocks.push_back(blk);
    prev_hash = blk.getHash();
    ++blockIndex;
    }


    return 0;
}