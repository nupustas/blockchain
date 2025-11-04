#include "manolib.h"
#include "user.h"
#include "transaction.h"
#include "functions.h"
#include "block.h"
#include <algorithm>
#include <limits>


int main()
{
int Ucount = 1000;
int Tcount = 10000; 

srand(static_cast<unsigned int>(time(0)));

vector<User> users;
vector<Transaction> transactions;
vector<Block> blocks;


generateUsers(users, Ucount);

	
int created = generateTransactions(users, transactions, Tcount);
cout << "Total transactions generated: " << created << "\n";

    // mining setup
    string prev_hash(64, '0'); // Genesis block previous hash (64 hex zeros)
    int blockIndex = 0;
    const size_t txPerBlock = 100;
    int difficulty = 4;   


    // mining blocks
while (!transactions.empty()) {
        size_t take = std::min(txPerBlock, transactions.size());
        vector<Transaction> batch(transactions.begin(), transactions.begin() + take);

        Block blk(blockIndex, prev_hash, batch, batch.size());

        cout << "Mining block " << blockIndex
                << " with " << batch.size() << " transactions..." << endl;

        bool solved = blk.mine(difficulty);

        cout << blk;

        // apply block transactions (vector-based lookups)
        std::vector<std::string> applied_ids = blk.applyTransactions(users);
        cout << "Applied transactions to balances: " << applied_ids.size() << "\n";

        // remove the batch we just included from the pending list (erase front)
        transactions.erase(transactions.begin(), transactions.begin() + take);

        blocks.push_back(blk);
        prev_hash = blk.getHash();
        ++blockIndex;
    }

    // interactive menu loop
    while (true) {
        cout << "\nMenu:\n";
        cout << " 0 - Exit\n";
        cout << " 1 - View a block\n";
        cout << " 2 - View a pending transaction\n";
        cout << " 3 - View a user\n";
        cout << "Choice: ";

        int choice = -1;
        if (!(cin >> choice)) {
            // invalid input, clear and retry
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Invalid input. Try again.\n";
            continue;
        }

        if (choice == 0) break;

        if (choice == 1) {
            cout << "Which block index do you want to view? (0 - " << (blocks.empty() ? 0 : blocks.size()-1) << ")\n";
            int idx; if (!(cin >> idx)) { cin.clear(); cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); cout<<"Bad index\n"; continue; }
            if (idx < 0 || static_cast<size_t>(idx) >= blocks.size()) { cout << "Block index out of range\n"; continue; }
            cout << blocks[idx];
            continue;
        }

        if (choice == 2) {
            cout << "Which pending transaction index? (0 - " << (transactions.empty() ? 0 : transactions.size()-1) << ")\n";
            int idx; if (!(cin >> idx)) { cin.clear(); cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); cout<<"Bad index\n"; continue; }
            if (idx < 0 || static_cast<size_t>(idx) >= transactions.size()) { cout << "Transaction index out of range\n"; continue; }
            const Transaction &t = transactions[idx];
            cout << transactions[idx];
            continue;
        }

        if (choice == 3) {
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