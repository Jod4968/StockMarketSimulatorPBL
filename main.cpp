#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>
#include <ctime>

using namespace std;

struct Stock {
    string symbol;
    string name;
    double price;
    double prevPrice;   // price before the last "Next Day"
};

struct Holding {
    string symbol;
    int quantity;
    double buyPrice;    // average buy price
};

vector<Stock> market;
vector<Holding> portfolio;

double balance = 100000.0;
int day = 1;


// ---------- Helpers ----------

// Safely read an integer (no infinite loop on bad input)
int readInt() {
    int x;
    while (!(cin >> x)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid input. Enter a number: ";
    }
    cin.ignore(10000, '\n');
    return x;
}

string readSymbol() {
    string symbol;
    cin >> symbol;
    cin.ignore(10000, '\n');
    for (int i = 0; i < (int)symbol.size(); i++) {
        symbol[i] = toupper(symbol[i]);
    }
    return symbol;
}


// ---------- Market ----------

void setupMarket() {
    market.push_back({"TCS", "Tata Consultancy Services", 3800.0, 3800.0});
    market.push_back({"INFY", "Infosys Ltd", 1550.0, 1550.0});
    market.push_back({"RELIANCE", "Reliance Industries", 2950.0, 2950.0});
    market.push_back({"HDFC", "HDFC Bank Ltd", 1650.0, 1650.0});
    market.push_back({"WIPRO", "Wipro Ltd", 480.0, 480.0});
    market.push_back({"ITC", "ITC Ltd", 430.0, 430.0});
    market.push_back({"SBIN", "State Bank of India", 810.0, 810.0});
}

// Move to next day: prices change randomly by -10% to +10%
void nextDay() {
    for (int i = 0; i < (int)market.size(); i++) {
        int change = rand() % 21 - 10;

        market[i].prevPrice = market[i].price;
        market[i].price += market[i].price * change / 100.0;

        if (market[i].price < 1.0) {
            market[i].price = 1.0;   // price floor
        }
    }
    day++;
    cout << "\nDay " << day << " begins. Prices have been updated.\n";
}

int findStock(const string& symbol) {
    for (int i = 0; i < (int)market.size(); i++) {
        if (market[i].symbol == symbol) {
            return i;
        }
    }
    return -1;
}

int findHolding(const string& symbol) {
    for (int i = 0; i < (int)portfolio.size(); i++) {
        if (portfolio[i].symbol == symbol) {
            return i;
        }
    }
    return -1;
}

double changePercent(const Stock& s) {
    if (s.prevPrice == 0) return 0;
    return (s.price - s.prevPrice) / s.prevPrice * 100.0;
}

void showMarket() {
    cout << "\n-- STOCK MARKET (Day " << day << ") --\n";

    for (int i = 0; i < (int)market.size(); i++) {
        double pct = changePercent(market[i]);

        cout << market[i].symbol
             << " (" << market[i].name
             << ") - Price: Rs " << market[i].price
             << " [" << (pct >= 0 ? "+" : "") << pct << "%]\n";
    }
}

void searchStock() {
    cout << "Enter the stock symbol: ";
    string symbol = readSymbol();

    int index = findStock(symbol);

    if (index != -1) {
        double pct = changePercent(market[index]);

        cout << "Found: " << market[index].name
             << ", Price: Rs " << market[index].price
             << " [" << (pct >= 0 ? "+" : "") << pct << "%]\n";
    }
    else {
        cout << "Stock not found.\n";
    }
}


// ---------- Trading ----------

void buyStock() {
    cout << "Enter stock symbol to buy: ";
    string symbol = readSymbol();

    int index = findStock(symbol);

    if (index == -1) {
        cout << "Invalid stock symbol.\n";
        return;
    }

    cout << "Enter quantity: ";
    int qty = readInt();

    if (qty <= 0) {
        cout << "Quantity must be positive.\n";
        return;
    }

    double price = market[index].price;
    double totalCost = price * qty;

    if (totalCost > balance) {
        cout << "Insufficient balance!\n";
        return;
    }

    balance -= totalCost;

    int holdingIndex = findHolding(symbol);

    if (holdingIndex != -1) {
        // Update average buy price
        double oldCost = portfolio[holdingIndex].buyPrice * portfolio[holdingIndex].quantity;
        portfolio[holdingIndex].quantity += qty;
        portfolio[holdingIndex].buyPrice = (oldCost + totalCost) / portfolio[holdingIndex].quantity;
    }
    else {
        Holding newHolding;
        newHolding.symbol = symbol;
        newHolding.quantity = qty;
        newHolding.buyPrice = price;
        portfolio.push_back(newHolding);
    }

    cout << "Bought " << qty << " " << symbol << " @ Rs " << price << " each.\n";
    cout << "Total: Rs " << totalCost << "\n";
    cout << "Balance left: Rs " << balance << "\n";
}

void sellStock() {
    cout << "Enter stock symbol to sell: ";
    string symbol = readSymbol();

    int holdingIndex = findHolding(symbol);

    if (holdingIndex == -1) {
        cout << "You do not own this stock.\n";
        return;
    }

    cout << "Enter quantity to sell: ";
    int qty = readInt();

    if (qty <= 0) {
        cout << "Quantity must be positive.\n";
        return;
    }

    if (qty > portfolio[holdingIndex].quantity) {
        cout << "You do not have enough shares.\n";
        return;
    }

    int marketIndex = findStock(symbol);
    double price = market[marketIndex].price;
    double totalAmount = price * qty;
    double profit = (price - portfolio[holdingIndex].buyPrice) * qty;

    balance += totalAmount;
    portfolio[holdingIndex].quantity -= qty;

    if (portfolio[holdingIndex].quantity == 0) {
        portfolio.erase(portfolio.begin() + holdingIndex);
    }

    cout << "Sold " << qty << " " << symbol << " @ Rs " << price << " each.\n";
    cout << "Received: Rs " << totalAmount << "\n";
    cout << "Profit/Loss on this sale: Rs " << profit << "\n";
    cout << "Current balance: Rs " << balance << "\n";
}


// ---------- Portfolio ----------

void showPortfolio() {
    if (portfolio.empty()) {
        cout << "\nPortfolio is empty.\n";
        cout << "Net Worth: Rs " << balance << "\n";
        return;
    }

    cout << "\n-- YOUR PORTFOLIO --\n";

    double totalInvested = 0;
    double totalCurrent = 0;

    for (int i = 0; i < (int)portfolio.size(); i++) {
        int marketIndex = findStock(portfolio[i].symbol);

        double currentPrice = market[marketIndex].price;
        double currentValue = currentPrice * portfolio[i].quantity;
        double investedValue = portfolio[i].buyPrice * portfolio[i].quantity;
        double profitLoss = currentValue - investedValue;

        totalInvested += investedValue;
        totalCurrent += currentValue;

        cout << "\nStock: " << portfolio[i].symbol;
        cout << "\nQuantity: " << portfolio[i].quantity;
        cout << "\nAvg Buy Price: Rs " << portfolio[i].buyPrice;
        cout << "\nCurrent Price: Rs " << currentPrice;
        cout << "\nCurrent Value: Rs " << currentValue;
        cout << "\nProfit/Loss: Rs " << profitLoss << "\n";
    }

    cout << "\n---------------------------";
    cout << "\nTotal Invested: Rs " << totalInvested;
    cout << "\nCurrent Holdings Value: Rs " << totalCurrent;
    cout << "\nTotal Profit/Loss: Rs " << (totalCurrent - totalInvested);
    cout << "\nCash Balance: Rs " << balance;
    cout << "\nNet Worth: Rs " << (balance + totalCurrent) << "\n";
}

void showBalance() {
    cout << "\nBalance: Rs " << balance << "\n";
}


// ---------- Menu ----------

void printMenu() {
    cout << "\n===== STOCK MARKET SIMULATOR (Day " << day << ") =====\n";
    cout << "1. View Stock Market\n";
    cout << "2. Search Stock\n";
    cout << "3. Buy Stock\n";
    cout << "4. Sell Stock\n";
    cout << "5. View Portfolio\n";
    cout << "6. View Balance\n";
    cout << "7. Next Day (update prices)\n";
    cout << "8. Exit\n";
    cout << "Enter your choice: ";
}

int main() {
    srand(time(0));
    setupMarket();

    cout << fixed << setprecision(2);
    cout << "Welcome! Starting virtual balance is: Rs " << balance << "\n";

    int choice = 0;

    while (choice != 8) {
        printMenu();
        choice = readInt();

        switch (choice) {
            case 1: showMarket(); break;
            case 2: searchStock(); break;
            case 3: buyStock(); break;
            case 4: sellStock(); break;
            case 5: showPortfolio(); break;
            case 6: showBalance(); break;
            case 7: nextDay(); break;
            case 8: cout << "Goodbye!\n"; break;
            default: cout << "Invalid choice, try again.\n";
        }
    }

    return 0;
}
