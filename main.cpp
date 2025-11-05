#include "manolib.h"
#include "user.h"
#include "transaction.h"
#include "functions.h"
#include "block.h"
#include <omp.h>


int main()
{
int Ucount = 1000; //user count
int Tcount = 10000; //transaction count

srand(static_cast<unsigned int>(time(0)));

vector<User> users;
vector<Transaction> transactions;
vector<Block> blocks; //blockchain


generateUsers(users, Ucount);
cout <<"Users generated: "<< users.size()<< "\n";
generateTransactions(users, transactions, Tcount);
cout << "Total transactions generated: " << transactions.size() << "\n";


//genesis block
Block genesis;
blocks.push_back(genesis);
cout << "Genesis block created:\n" << genesis << "\n";
string prev_hash = genesis.getHash();

// mining setup
int difficulty = 4;
cout<<"Iveskite hasho sunkuma: ";
cin>>difficulty;   
int blockIndex = 1;
const size_t txPerBlock = 100;
const int printLimit = 10;
    // mining
while (!transactions.empty()) {
    size_t take = min(txPerBlock, transactions.size());
    vector<Transaction> batch(transactions.begin(), transactions.begin() + take);

    Block blk(blockIndex, prev_hash, batch, batch.size());

    //cout << "Mining block " << blockIndex<<" with "<< batch.size() << " transactions..." << endl;

    blk.mine(difficulty);
    // outputs only first 10 mined blocks
    if (blockIndex < printLimit) {
        cout << "Mined block " << blockIndex << " with " << batch.size() << " transactions:";
        cout << blk;
    }

    // apply block transactions
    vector<string> applied = blk.applyTransactions(users);
    //cout << "Applied transactions to balances: " << applied.size() << "\n";

    // remove the batch we just included from the pending list (erase front)
    transactions.erase(transactions.begin(), transactions.begin() + take);

    blocks.push_back(blk);
    prev_hash = blk.getHash();
    ++blockIndex;
};

    // interactive menu loop
    while (true) {
        cout << "\nMenu:\n";
        cout << " 0 - Exit\n";
        cout << " 1 - View a block\n";
        cout << " 2 - View a user\n";
        cout << "Choice: ";

        int choice = -1;
    if (!(cin >> choice)) {
        // invalid input, clear and retry
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Try again.\n";
        continue;
    }

    if (choice == 0) break;

    if (choice == 1) {

    cout << "Which block index do you want to view? (0 - " << (blocks.empty() ? 0 : blocks.size()-1) << ")\n";
    int blockIdx; 
if (!(cin >> blockIdx)) {
    cin.clear(); 
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
    cout << "Bad index\n"; 
    continue; 
}
if (blockIdx < 0 || static_cast<size_t>(blockIdx) >= blocks.size()) {
    cout << "Block index out of range\n"; 
    continue; 
}

    Block &block = blocks[blockIdx];
    cout << block; // show the block info

    // see a specific transaction?
if (!block.getTransactions().empty()) {
    cout << "Do you want to view a transaction in this block? (y/n): ";
    char txChoice;
    cin >> txChoice;
    if (txChoice == 'y' || txChoice == 'Y') {
        cout << "Which transaction index? (0 - " 
                    << block.getTransactions().size()-1 << "): ";
        int txIdx;
        if (!(cin >> txIdx)) {
            cin.clear(); 
            cin.ignore(numeric_limits<std::streamsize>::max(), '\n'); 
            cout << "Bad index\n"; 
            continue;
        }
        if (txIdx < 0 || static_cast<size_t>(txIdx) >= block.getTransactions().size()) {
            cout << "Transaction index out of range\n"; 
            continue;
        }

        block.printTransaction(txIdx);
    }
}

    continue;
}

if (choice == 2) {
    cout << "Which user index? (0 - " << (users.empty() ? 0 : users.size()-1) << ")\n";
    int idx; if (!(cin >> idx)) { cin.clear(); cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); cout<<"Bad index\n"; continue; }
    if (idx < 0 || static_cast<size_t>(idx) >= users.size()) { cout << "User index out of range\n"; continue; }
    cout << "Name: " << users[idx].getName() << "\n";
    cout << "Balance: " << users[idx].getBalance() << "\n";
    cout << "PublicKey: " << users[idx].getPublic_key() << "\n";
    cout << "=================================================\n\n";
    continue;
}

    cout << "Unknown choice. Try again.\n";
}              
    return 0;
}