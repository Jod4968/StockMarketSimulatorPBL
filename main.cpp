
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>
#include <ctime>

using namespace std;

// ---------- Stock Class ----------

class Stock {
private:
    string symbol;
    string name;
    double price;
    double prevPrice;

public:
    Stock(string s, string n, double p)
        : symbol(s), name(n), price(p), prevPrice(p) {}

    string getSymbol() const {
        return symbol;
    }

    string getName() const {
        return name;
    }

    double getPrice() const {
        return price;
    }

    double getPrevPrice() const {
        return prevPrice;
    }

    void updatePrice(int change) {
        prevPrice = price;
        price += price * change / 100.0;

        if (price < 1.0) {
            price = 1.0;
        }
    }

    double changePercent() const {
        if (prevPrice == 0) return 0;
        return (price - prevPrice) / prevPrice * 100.0;
    }
};


// ---------- Holding Class ----------

class Holding {
private:
    string symbol;
    int quantity;
    double buyPrice;

public:
    Holding(string s, int q, double p)
        : symbol(s), quantity(q), buyPrice(p) {}

    string getSymbol() const {
        return symbol;
    }

    int getQuantity() const {
        return quantity;
    }

    double getBuyPrice() const {
        return buyPrice;
    }

    void addShares(int qty, double price) {
        double oldCost = buyPrice * quantity;
        quantity += qty;
        buyPrice = (oldCost + price * qty) / quantity;
    }

    void removeShares(int qty) {
        quantity -= qty;
    }
};


// ---------- Global Data ----------

vector<Stock> market;
vector<Holding> portfolio;

double balance = 100000.0;
int day = 1;


// ---------- Helpers ----------

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
        symbol[i] = toupper((unsigned char)symbol[i]);
    }

    return symbol;
}


// ---------- Market ----------

void setupMarket() {
    market.emplace_back("TCS", "Tata Consultancy Services", 3800.0);
    market.emplace_back("INFY", "Infosys Ltd", 1550.0);
    market.emplace_back("RELIANCE", "Reliance Industries", 2950.0);
    market.emplace_back("HDFC", "HDFC Bank Ltd", 1650.0);
    market.emplace_back("WIPRO", "Wipro Ltd", 480.0);
    market.emplace_back("ITC", "ITC Ltd", 430.0);
    market.emplace_back("SBIN", "State Bank of India", 810.0);
}

void nextDay() {
    for (int i = 0; i < (int)market.size(); i++) {
        int change = rand() % 21 - 10;
        market[i].updatePrice(change);
    }

    day++;
    cout << "\nDay " << day
         << " begins. Prices have been updated.\n";
}

int findStock(const string& symbol) {
    for (int i = 0; i < (int)market.size(); i++) {
        if (market[i].getSymbol() == symbol) {
            return i;
        }
    }

    return -1;
}

int findHolding(const string& symbol) {
    for (int i = 0; i < (int)portfolio.size(); i++) {
        if (portfolio[i].getSymbol() == symbol) {
            return i;
        }
    }

    return -1;
}

void showMarket() {
    cout << "\n-- STOCK MARKET (Day " << day << ") --\n";

    for (int i = 0; i < (int)market.size(); i++) {
        double pct = market[i].changePercent();

        cout << market[i].getSymbol()
             << " (" << market[i].getName()
             << ") - Price: Rs " << market[i].getPrice()
             << " [" << (pct >= 0 ? "+" : "") << pct << "%]\n";
    }
}

void searchStock() {
    cout << "Enter the stock symbol: ";
    string symbol = readSymbol();

    int index = findStock(symbol);

    if (index != -1) {
        Stock& s = market[index];
        double pct = s.changePercent();

        cout << "Found: " << s.getName()
             << ", Price: Rs " << s.getPrice()
             << " [" << (pct >= 0 ? "+" : "") << pct << "%]\n";
    } else {
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

    double price = market[index].getPrice();
    double totalCost = price * qty;

    if (totalCost > balance) {
        cout << "Insufficient balance!\n";
        return;
    }

    int holdingIndex = findHolding(symbol);

    // Update an existing holding or create a new one.
    if (holdingIndex != -1) {
        portfolio[holdingIndex].addShares(qty, price);
    } else {
        portfolio.emplace_back(symbol, qty, price);
    }

    balance -= totalCost;

    cout << "Bought " << qty << " " << symbol
         << " @ Rs " << price << " each.\n";
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

    if (qty > portfolio[holdingIndex].getQuantity()) {
        cout << "You do not have enough shares.\n";
        return;
    }

    int marketIndex = findStock(symbol);
    double price = market[marketIndex].getPrice();

    double totalAmount = price * qty;
    double profit =
        (price - portfolio[holdingIndex].getBuyPrice()) * qty;

    balance += totalAmount;
    portfolio[holdingIndex].removeShares(qty);

    if (portfolio[holdingIndex].getQuantity() == 0) {
        portfolio.erase(portfolio.begin() + holdingIndex);
    }

    cout << "Sold " << qty << " " << symbol
         << " @ Rs " << price << " each.\n";
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
        int marketIndex = findStock(portfolio[i].getSymbol());

        double currentPrice = market[marketIndex].getPrice();
        double currentValue =
            currentPrice * portfolio[i].getQuantity();

        double investedValue =
            portfolio[i].getBuyPrice() * portfolio[i].getQuantity();

        double profitLoss = currentValue - investedValue;

        totalInvested += investedValue;
        totalCurrent += currentValue;

        cout << "\nStock: " << portfolio[i].getSymbol();
        cout << "\nQuantity: " << portfolio[i].getQuantity();
        cout << "\nAvg Buy Price: Rs " << portfolio[i].getBuyPrice();
        cout << "\nCurrent Price: Rs " << currentPrice;
        cout << "\nCurrent Value: Rs " << currentValue;
        cout << "\nProfit/Loss: Rs " << profitLoss << "\n";
    }

    cout << "\n---------------------------";
    cout << "\nTotal Invested: Rs " << totalInvested;
    cout << "\nCurrent Holdings Value: Rs " << totalCurrent;
    cout << "\nTotal Profit/Loss: Rs "
         << totalCurrent - totalInvested;
    cout << "\nCash Balance: Rs " << balance;
    cout << "\nNet Worth: Rs " << balance + totalCurrent << "\n";
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
    srand((unsigned int)time(0));
    setupMarket();

    cout << fixed << setprecision(2);
    cout << "Welcome! Starting virtual balance is: Rs "
         << balance << "\n";

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
