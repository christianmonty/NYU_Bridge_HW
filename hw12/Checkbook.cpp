//checkbook balancing program
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cctype>
#include <string>

using namespace std;

class Money {

public:

    friend Money add(Money amount1, Money amount2);
    friend Money subtract(Money amount1, Money amount2);
    friend bool equal(Money amount1, Money amount2);

    Money(double dollars, int cents);
    Money(double dollars);
    Money();

    double get_value() const { return (all_cents * 0.01); }
    bool input(istream& ins);
    void output(ostream& outs);

private:
    double all_cents;
};


class Check {
public:
    Check(int Num, Money total, bool cash);
    Check();

    Check input(int Num, Money total, bool cash);
    void output();

    void set(int newCheckNum, Money newAmount, bool newCashed);
    int get_number() const { return checkNum; }
    double get_amount() const { return amount.get_value(); }
    bool is_cashed() const { return cashed; }

private:
    int checkNum;
    Money amount;
    bool cashed;
};


int digit_to_int(char c);

int main() {
    vector<Check> vectorCheck;
    int vectorCount = 0, arrCount = 0, checkNum = 0, checkMax = 0;
    bool flag = false;
    Money oldBalance, newBalance, calcBalance, balanceDif, checksCashedTotal;

    cout << "Welcome to the checkbook program. Please enter details of uncashed checks, and additional deposit information in order to calculate bank account information. For each check, you will first be asked the check number and then afterwards be asked for the Amount, and whether it has been cashed" << endl;

    Money userAmount;
    int checkID = 0;
    bool userCashed;

    while (checkID >= 0) {
        cout << "Please enter a check number. Enter -1 when finished entering uncashed checks. " << endl;
        cin >> checkID;

        if (checkID < 0) {
            break;
        }
        cout << "Please enter Amount for check number in the format $##.##" << endl;
        userAmount.input(cin);
        cout << "Has the check been cashed? Enter 0 for NO, and 1 for YES: " << endl;
        cin >> userCashed;
        cout << endl;

        vectorCheck.push_back(Check(checkID, userAmount, userCashed));

        if (vectorCheck[vectorCount].get_number() > checkMax) {
            checkMax = vectorCheck[vectorCount].get_number();
        }
        vectorCount++;
    }


    int arrSize = checkMax + 1;
    Check* arr = new Check[arrSize];

    for (int i = 0; i < vectorCount; i++) {
        arr[vectorCheck[i].get_number()] = vectorCheck[i];
    }


    for (int i = 0; i < arrSize; i++) {
        if (arr[i].is_cashed()) {
            checksCashedTotal = add(checksCashedTotal, arr[i].get_amount());
        }
    }

    Money deposit, depositTotal;
    vector<Money> depositList;
    cout << endl << "Please enter all the deposits to your account in the format $##.##. When you are done entering deposits please enter $0.00" << endl;

    while (true) {
        if (!deposit.input(cin)) {
            break;
        }
        depositList.push_back(deposit.get_value());
        depositTotal = add(depositTotal, deposit);
    }

    cout << endl << "What is the old account balance? Please enter in the format $##.##" << endl;
    oldBalance.input(cin);
    cout << endl << "What is the new account balance? Please enter in the format $##.##" << endl;
    newBalance.input(cin);

    cout << endl << "Checks cashed total is: ";
    checksCashedTotal.output(cout);
    cout << endl;
    cout << "Deposit total is: ";
    depositTotal.output(cout);

    calcBalance = add(oldBalance, depositTotal);
    calcBalance = subtract(calcBalance, checksCashedTotal);
    string difference;

    if (calcBalance.get_value() > newBalance.get_value()) {
        balanceDif = subtract(calcBalance, newBalance);
        difference = " greater than";
    }
    else if (calcBalance.get_value() < newBalance.get_value()) {
        balanceDif = subtract(newBalance, calcBalance);
        difference = " less than";
    }
    else if (calcBalance.get_value() == newBalance.get_value()) {
        balanceDif = subtract(calcBalance, newBalance);
        difference = " equal to";
    }


    cout << endl << "New balance should be: ";
    calcBalance.output(cout);
    cout << " which is ";
    balanceDif.output(cout);
    cout << difference << " bank balance of: ";
    newBalance.output(cout);
    cout << endl;


    cout << endl << "List of checks cashed since last time balanced checkbook: " << endl;
    for (int i = 0; i < arrSize; i++) {
        if (arr[i].is_cashed()) {
            arr[i].output();
        }
    }
    cout << endl << "List of checks still not cashed since last time balanced checkbook: " << endl;
    for (int i = 0; i < arrSize; i++) {
        if (!arr[i].is_cashed() && arr[i].get_amount() > 0) {
            arr[i].output();
        }
    }

    delete[] arr;
    arr = nullptr;

    return 0;
}

Money add(Money amount1, Money amount2) {
    Money temp;

    temp.all_cents = amount1.all_cents + amount2.all_cents;
    return temp;
}

Money subtract(Money amount1, Money amount2) {
    Money temp;
    temp.all_cents = amount1.all_cents - amount2.all_cents;
    return temp;
}

bool equal(Money amount1, Money amount2) {
    return (amount1.all_cents == amount2.all_cents);
}

Money::Money(double dollars, int cents)
{
    if (dollars * cents < 0) {
        cout << "Illegal values for dollars and cents." << endl;
        exit(1);
    }
    all_cents = dollars * 100 + cents;
}

Money::Money(double dollars) : all_cents(dollars * 100)
{
}

Money::Money() : all_cents(0)
{
}

bool Money::input(istream& ins)
{
    char one_char, decimal_point, digit1, digit2;
    long dollars;
    int cents;
    bool negative;

    ins >> one_char;
    if (one_char == '-')
    {
        negative = true;
        ins >> one_char;
    }
    else {
        negative = false;
    }
    ins >> dollars >> decimal_point >> digit1 >> digit2;

    if (one_char != '$' || decimal_point != '.' || !isdigit(digit1) || !isdigit(digit2)) {
        cout << "Error illegal form for money input ";
        exit(1);
    }
    cents = digit_to_int(digit1) * 10 + digit_to_int(digit2);
    all_cents = dollars * 100 + cents;
    if (negative) {
        all_cents = -all_cents;
    }

    if (all_cents == 0) {
        return false;
    }
    return true;
}

void Money::output(ostream& outs) {
    long positive_cents, dollars, cents;
    if (all_cents < 0) {
        positive_cents = all_cents * -1;
    }
    else {
        positive_cents = all_cents;
    }

    dollars = positive_cents / 100;
    cents = positive_cents % 100;

    if (all_cents < 0) {
        outs << "-$" << dollars << '.';
    }
    else {
        outs << "$" << dollars << '.';
    }
    if (cents < 10) {
        outs << '0';
    }
    outs << cents;
}

int digit_to_int(char c) {
    return (static_cast<int>(c) - static_cast<int>('0'));
}


Check::Check(int Num, Money total, bool cash) {
    checkNum = Num;
    amount = total;
    cashed = cash;
}

Check::Check() : checkNum(0), amount(), cashed(false)
{
}

Check Check::input(int Num, Money total, bool cash) {
    return Check(Num, total, cash);
}

void Check::output() {
    cout << "Check number " << checkNum << " of amount ";
    amount.output(cout);

    if (cashed) {
        cout << " is cashed " << endl;
    }
    else if (!cashed) {
        cout << " is not cashed " << endl;
    }
}

void Check::set(int newCheckNum, Money newAmount, bool newCashed) {
    checkNum = newCheckNum;
    amount = newAmount;
    cashed = newCashed;
}
