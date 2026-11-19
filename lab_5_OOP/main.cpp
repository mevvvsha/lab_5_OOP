#include <clocale>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

#include "PaymentEntity.h"
#include "PaymentExceptions.h"
#include "PaymentSystem.h"

using std::cerr;
using std::cin;
using std::cout;
using std::string;
using std::vector;

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    cout << "=========================================================\n";
    cout << "   ЛАБОРАТОРНА РОБОТА №5: ПОЛІМОРФІЗМ ТА ВИНЯТКИ (С++)\n";
    cout << "   Варіант 14: Система Платежі (Рівень 2)\n";
    cout << "=========================================================\n\n";

    // ПУНКТ 9: Виклик статичного методу через ім'я класу (без об'єкта)
    PaymentEntity::showCreatedObjectsCount();

    // ПУНКТ 6: Створення 3 статичних та 3 динамічних об'єктів
    int staticSize = 3;
    BankAccount staticAccounts[3] = {
        BankAccount("UA111", "Іван Петренко", 15000, "UAH", true, "ПриватБанк", "01.09.2026", 0),
        BankAccount("UA222", "Олена Коваль", 500, "UAH", true, "Ощадбанк", "02.09.2026", 0),
        BankAccount("UA333", "Петро Іванов", 8000, "UAH", true, "Монобанк", "03.09.2026", 0) };

    int dynSize = 3;
    BankAccount* dynAccounts = new BankAccount[3]{
        BankAccount("UA444", "Анна Сидоренко", 300, "UAH", true, "ПриватБанк", "04.09.2026", 0),
        BankAccount("UA555", "Тарас Шевченко", 25000, "UAH", true, "Ощадбанк", "05.09.2026", 0),
        BankAccount("UA666", "Марія Лебідь", 4500, "UAH", true, "Монобанк", "06.09.2026", 0) };

    PaymentEntity::showCreatedObjectsCount();

    // ПУНКТ 11: Поліморфні виклики ТРЬОХ віртуальних методів через масив покажчиків Base*
    cout << "\n>>> ПУНКТ 11: Поліморфні виклики 3 методів <<<\n";
    CreditCard card("4444 5555 6666 7777", "Іван Петренко", "12/28", "Credit", 20000, 0, false, "ПриватБанк");
    Client polyClient("UA123", "Іван Петренко", 15000, "UAH", true, "ПриватБанк", "01.09.2026", 0, 1, "ivan@gmail.com", "+38099111", "Хмельницький", card);
    CommissionPolicy polyPolicy(0.02, 0.0, 0.01, "Базовий", "UAH", true, 1.0, 500.0);
    PremiumClient polyPremium("UA999", "VIP Клієнт", 50000, "UAH", card, "Platinum", 0.05);

    PaymentEntity* polyEntities[3] = { &polyClient, &polyPolicy, &polyPremium };

    for (int i = 0; i < 3; ++i) {
        // 1. Віртуальний метод відображення інформації
        polyEntities[i]->display();
        // 2. Віртуальний метод розрахунку комісії
        cout << "Комісія за 1000 грн: " << polyEntities[i]->calculateCommission(1000.0) << " грн\n";
        // 3. Чисто віртуальний метод проведення транзакції
        polyEntities[i]->processTransaction(1000.0);
        cout << "---------------------------------------------------------\n";
    }

    // ПУНКТ 4: Демонстрація відмінності роботи невіртуального та віртуального деструкторів
    cout << "\n>>> ПУНКТ 4: Порівняння деструкторів <<<\n";
    NonVirtualBase* nonVirtPtr = new NonVirtualDerived();
    delete nonVirtPtr;  // Викличеться ЛИШЕ ~NonVirtualBase() (похідний ігнорується)!

    PaymentEntity* virtPtr = new BankAccount("UA777", "Тест Вірт", 1000, "UAH", true, "Банк", "19.11.2026", 0);
    delete virtPtr;     // Викличеться спочатку ~BankAccount(), а потім ~PaymentEntity()!

    // ПУНКТ 6 та 7: Видалення з масивів (Service_Static / Service_Dinamic) та 3 винятки пам'яті
    cout << "\n>>> ПУНКТ 6-7: Винятки динамічної пам'яті та Service_Static / Service_Dinamic <<<\n";
    try {
        // Виняток 1: Некоректний розмір виділення пам'яті
        int invalidSize = -10;
        if (invalidSize <= 0) {
            throw MemoryAllocationException("Спроба виділити пам'ять для від'ємної кількості елементів!");
        }
    }
    catch (const PaymentSystemException& ex) {
        cerr << "[Виняток пам'яті #1] " << ex.what() << "\n";
    }

    try {
        // Видаляємо рахунки з балансом менше 1000 грн
        Service_Static(staticAccounts, staticSize, 1000.0);
        Service_Dinamic(dynAccounts, dynSize, 1000.0);

        // Виняток 2: Спроба видалити елементи, яких не існує за критерієм
        Service_Dinamic(dynAccounts, dynSize, 999999.0);
    }
    catch (const PaymentSystemException& ex) {
        cerr << "[Виняток пам'яті #2] " << ex.what() << "\n";
    }

    try {
        // Виняток 3: Передача порожнього (nullptr) масиву
        BankAccount* nullArr = nullptr;
        int zeroSize = 0;
        Service_Dinamic(nullArr, zeroSize, 500.0);
    }
    catch (const PaymentSystemException& ex) {
        cerr << "[Виняток пам'яті #3] " << ex.what() << "\n";
    }

    // ПУНКТ 8: Функція Service(Read/Write) та 5 виняткових ситуацій роботи з файлами
    cout << "\n>>> ПУНКТ 8: Функція Service та 5 виняткових ситуацій роботи з файлами <<<\n";
    try {
        Service(polyClient, "client_backup.txt");  // Успішна синхронізація з файлом
    }
    catch (const PaymentSystemException& ex) {
        cerr << ex.what() << "\n";
    }

    // Створюємо тестові файли для перевірки винятків читання
    std::ofstream("empty.txt").close();
    std::ofstream badFormat("bad_format.txt");
    badFormat << "UA111 Ivan NOT_A_DOUBLE";
    badFormat.close();

    string badFiles[5] = {
        "",                              // 1. Порожня назва файлу
        "missing_file_12345.txt",        // 2. Файл не існує
        "Z:\\non_existent_dir\\out.txt", // 3. Помилка відкриття для запису
        "empty.txt",                     // 4. Порожній файл
        "bad_format.txt"                 // 5. Невірний формат даних у файлі
    };

    for (int i = 0; i < 5; ++i) {
        try {
            if (i == 2) {
                polyClient.Write(badFiles[i]);
            }
            else {
                polyClient.Read(badFiles[i]);
            }
        }
        catch (const PaymentSystemException& ex) {
            cerr << "[Файловий виняток #" << i + 1 << "] " << ex.what() << "\n";
        }
    }

    // ПУНКТ 10: Інтерактивне меню користувача для створення і знищення об'єктів
    cout << "\n>>> ПУНКТ 10: Інтерактивне меню керування об'єктами <<<\n";
    vector<PaymentEntity*> menuEntities;
    int choice = -1;

    while (choice != 0) {
        cout << "\n--- МЕНЮ СИСТЕМИ ПЛАТЕЖІ ---\n"
            << "1. Створити Банківський рахунок (BankAccount)\n"
            << "2. Створити Клієнта (Client)\n"
            << "3. Створити Преміум-клієнта (PremiumClient)\n"
            << "4. Переглянути створені об'єкти та загальний лічильник класу\n"
            << "5. Знищити об'єкт за індексом\n"
            << "0. Вихід\n"
            << "Оберіть дію: ";
        cin >> choice;

        if (choice == 1) {
            string acc, owner;
            double bal;
            cout << "Введіть номер рахунку, ім'я власника та баланс: ";
            cin >> acc >> owner >> bal;
            menuEntities.push_back(
                new BankAccount(acc, owner, bal, "UAH", true, "ПриватБанк", "19.11.2026", 0));
            cout << "Банківський рахунок створено!\n";
        }
        else if (choice == 2) {
            string acc, owner;
            double bal;
            cout << "Введіть номер рахунку, ім'я клієнта та баланс: ";
            cin >> acc >> owner >> bal;
            menuEntities.push_back(
                new Client(acc, owner, bal, "UAH", true, "ПриватБанк", "19.11.2026", 0, 10,
                    "user@mail.ua", "+38099000", "Хмельницький", card));
            cout << "Клієнта створено!\n";
        }
        else if (choice == 3) {
            string acc, owner;
            double bal;
            cout << "Введіть номер рахунку, ім'я VIP-клієнта та баланс: ";
            cin >> acc >> owner >> bal;
            menuEntities.push_back(
                new PremiumClient(acc, owner, bal, "UAH", card, "Gold", 0.05));
            cout << "Преміум-клієнта створено!\n";
        }
        else if (choice == 4) {
            PaymentEntity::showCreatedObjectsCount();
            for (size_t i = 0; i < menuEntities.size(); ++i) {
                cout << "[" << i << "] ";
                menuEntities[i]->display();
            }
        }
        else if (choice == 5) {
            int idx;
            cout << "Введіть індекс об'єкта для знищення: ";
            cin >> idx;
            if (idx >= 0 && idx < static_cast<int>(menuEntities.size())) {
                delete menuEntities[idx];
                menuEntities.erase(menuEntities.begin() + idx);
                cout << "Об'єкт успішно знищено!\n";
            }
            else {
                cout << "Невірний індекс!\n";
            }
        }
    }

    // Звільнення динамічної пам'яті перед виходом
    for (auto* ptr : menuEntities) {
        delete ptr;
    }
    delete[] dynAccounts;

    cout << "\nРоботу програми успішно завершено.\n";
    return 0;
}