// Copyright 2026 Student
#ifndef PAYMENT_SYSTEM_H_
#define PAYMENT_SYSTEM_H_

#include <iostream>
#include <string>
#include <vector>

#include "PaymentEntity.h"

// Клас транзакції (інформація про окремий платіж)
class Transaction {
private:
    int id;
    std::string date;
    double amount;
    std::string type;
    std::string currency;
    double commission;
    std::string status;
    std::string description;

public:
    Transaction();
    Transaction(int id, std::string date, double amount, std::string type,
        std::string currency, double commission, std::string status,
        std::string description);
    Transaction(const Transaction& other);
    ~Transaction();

    void display() const;
    double getAmount() const;
    std::string getType() const;
};

// Клас кредитної картки (агрегується всередині клієнта)
class CreditCard {
private:
    std::string cardNumber;
    std::string holder;
    std::string expiryDate;
    std::string cardType;
    double creditLimit;
    double usedCredit;
    bool blocked;
    std::string bankName;

public:
    CreditCard();
    CreditCard(std::string number, std::string holder, std::string expiry,
        std::string type, double limit, double used, bool blocked,
        std::string bank);
    CreditCard(const CreditCard& other);
    ~CreditCard();

    void display() const;
    void block();
    void unblock();
    bool isBlocked() const;
    double getCreditLimit() const;
    double getUsedCredit() const;
    std::string getCardNumber() const;
    void setCreditLimit(double limit);
};

// Похідний клас 1: Банківський рахунок.
// Використано virtual public, щоб уникнути дублювання PaymentEntity при множинному наслідуванні.
class BankAccount : virtual public PaymentEntity {
protected:
    std::string accountNumber;
    std::string owner;
    double balance;
    bool active;
    std::string bankName;
    std::string openedDate;
    int transactionCount;

public:
    BankAccount();
    BankAccount(std::string number, std::string owner, double balance,
        std::string currency, bool active, std::string bank,
        std::string date, int count);
    BankAccount(const BankAccount& other);
    ~BankAccount() override;

    // Перевизначення віртуальних методів базового класу
    bool processTransaction(double amount) override;
    void display() const override;
    double calculateCommission(double amount) const override;

    void displayAccount() const;
    double getBalance() const;
    std::string getAccountNumber() const;
    bool isActive() const;
    void deposit(double amount);
    void withdraw(double amount);
    void cancelAccount();
};

// Похідний клас 2: Історія платежів (також віртуальне наслідування)
class PaymentHistory : virtual public PaymentEntity {
protected:
    std::vector<Transaction> transactions;
    std::string historyOwner;
    int historyId;
    int totalTransactions;
    double totalAmount;
    std::string creationDate;
    std::string lastOperation;
    bool historyActive;

public:
    PaymentHistory();
    PaymentHistory(std::string owner, int id, std::string date);
    PaymentHistory(const PaymentHistory& other);
    ~PaymentHistory() override;

    // Перевизначення віртуальних методів базового класу
    bool processTransaction(double amount) override;
    void display() const override;
    double calculateCommission(double amount) const override;

    void addTransaction(const Transaction& transaction);
    void showHistory() const;
    int getTransactionCount() const;
    double getTotalAmount() const;
    void clearHistory();
};

// Похідний клас 3: Політика комісій (віртуальне наслідування)
class CommissionPolicy : virtual public PaymentEntity {
protected:
    double transferCommission;
    double paymentCommission;
    double withdrawalCommission;
    std::string policyName;
    bool policyActive;
    double minimumCommission;
    double maximumCommission;

public:
    CommissionPolicy();
    CommissionPolicy(double transfer, double payment, double withdrawal,
        std::string name, std::string currency, bool active,
        double minimum, double maximum);
    CommissionPolicy(const CommissionPolicy& other);
    ~CommissionPolicy() override;

    // Перевизначення віртуальних методів базового класу
    bool processTransaction(double amount) override;
    void display() const override;
    double calculateCommission(double amount) const override;

    double calculateCommissionByType(double amount, std::string type) const;
    void displayPolicy() const;
    void setTransferCommission(double value);
    double getTransferCommission() const;
    bool isPolicyActive() const;
};

// Клас Клієнт: відкрито наслідує BankAccount, містить методи роботи з файлами
class Client : public BankAccount {
private:
    int clientId;
    std::string email;
    std::string phone;
    std::string address;
    CreditCard card;
    std::vector<Transaction> history;
    double requestedLimit;
    std::string registrationDate;

public:
    using BankAccount::getAccountNumber;
    using BankAccount::getBalance;

    Client();
    Client(std::string number, std::string owner, double balance,
        std::string currency, bool active, std::string bank, std::string date,
        int count, int id, std::string email, std::string phone,
        std::string address, CreditCard card);
    Client(const Client& other);
    ~Client() override;

    bool processTransaction(double amount) override;
    void display() const override;
    double calculateCommission(double amount) const override;

    void payOrder(double amount);
    void transfer(double amount, std::string targetAccount);
    void blockCard();
    void cancelClientAccount();
    void requestCreditLimit(double newLimit);
    void showClientInfo() const;
    void showClientHistory() const;
    void addTransaction(const Transaction& transaction);

    CreditCard& getCard();
    double getRequestedLimit() const;
    void approveCreditLimit(double limit);
    void rejectCreditLimit();

    // Методи запису/читання з файлу із генерацією винятків
    void Write(const std::string& filename) const;
    void Read(const std::string& filename);
};

// Преміум-клієнт: множинне наслідування (має єдиний підоб'єкт PaymentEntity)
class PremiumClient : public Client,
    public CommissionPolicy,
    public PaymentHistory {
private:
    int premiumId;
    std::string membershipLevel;
    double bonusBalance;
    int bonusPoints;
    std::string membershipDate;
    bool premiumActive;
    double cashbackRate;
    std::string privileges;

public:
    PremiumClient();
    PremiumClient(std::string number, std::string owner, double balance,
        std::string currency, CreditCard card, std::string level,
        double cashback);
    PremiumClient(const PremiumClient& other);
    ~PremiumClient() override;

    bool processTransaction(double amount) override;
    void display() const override;
    double calculateCommission(double amount) const override;

    void displayPremiumInfo() const;
    void makePremiumTransfer(double amount, std::string target);
    void showPremiumHistory() const;
};

// Адміністратор системи
class Administrator {
private:
    int administratorId;
    std::string name;
    std::string email;
    std::string login;
    std::string department;
    std::string position;
    bool active;
    int operationsCount;

public:
    Administrator();
    Administrator(int id, std::string name, std::string email, std::string login,
        std::string department, std::string position, bool active,
        int operations);
    Administrator(const Administrator& other);
    ~Administrator();

    void viewHistory(const Client& client) const;
    void blockCardForOverrun(Client& client) const;
    void approveCreditLimit(Client& client, double limit) const;
    void rejectCreditLimit(Client& client) const;
    void displayInfo() const;
};

// План виплат (розстрочка замовлення на кілька платежів)
class PaymentPlan {
private:
    double totalAmount;
    int numberOfPayments;
    std::vector<double> installments;
    std::vector<std::string> dueDates;
    std::string clientName;
    std::string orderNumber;
    double paidAmount;
    bool planActive;

public:
    PaymentPlan();
    PaymentPlan(double total, int count, std::string client, std::string order,
        std::string date);
    PaymentPlan(const PaymentPlan& other);
    ~PaymentPlan();

    void createPlan();
    void displayPlan() const;
    void makePayment(int installmentNumber);
    double getRemainingAmount() const;
    bool isCompleted() const;
};

// Класи для демонстрації захищеного (protected) та закритого (private) наслідування
class ProtectedAccount : protected BankAccount {
private:
    std::string accessCode;
    int securityLevel;
    bool verification;
    std::string department;
    std::string manager;
    int accountId;
    std::string status;
    double limit;

public:
    ProtectedAccount();
    ProtectedAccount(std::string number, std::string owner, double balance,
        std::string currency, bool active, std::string bank,
        std::string date, int count);
    ~ProtectedAccount() override;

    void showProtectedAccount() const;
    void changeBalance(double amount);
};

class PrivateClient : private Client {
private:
    std::string privateCode;
    int accessLevel;
    bool confidential;
    std::string category;
    std::string registrationNumber;
    int operations;
    std::string manager;
    double personalLimit;

public:
    PrivateClient();
    PrivateClient(std::string number, std::string owner, double balance,
        std::string currency, bool active, std::string bank,
        std::string date, int count, int id, std::string email,
        std::string phone, std::string address, CreditCard card);
    ~PrivateClient() override;

    void showPrivateClient() const;
    void makePrivatePayment(double amount);
};

// Зовнішні сервісні функції для роботи з файлами та фільтрації масивів
void Service(Client& client, const std::string& filename);
void Service_Static(BankAccount arr[], int& size, double minBalance);
void Service_Dinamic(BankAccount*& arr, int& size, double minBalance);

#endif  // PAYMENT_SYSTEM_H_