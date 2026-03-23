#include <iostream>
#include <vector>    // Инструмент 1: std::vector
#include <fstream>   // Инструмент 2: <fstream>
#include <string>    // Инструмент 3: <string> (find, substr, length)
#include <algorithm>
#include <iomanip>
#include <Windows.h> // Для корректного русского языка

using namespace std;

// === Структуры данных ===
enum Group { WORK, FAMILY, FRIENDS, OTHERS };
struct Date { int day, month, year; };

struct Phone {
    long long countryCode; // Против "мусора"
    long long cityCode;
    long long number;
};

struct Contact {
    string lastName, firstName, patronymic;
    Phone phone;
    Date birthday;
    string email;
    Group category;
};

// === Прототипы функций ===
string formatStr(string s);
void showTable(const vector<Contact>& db);
void search(const vector<Contact>& db);
void removeContact(vector<Contact>& db);
void edit(vector<Contact>& db);
void saveToFile(const vector<Contact>& db);
void loadFromFile(vector<Contact>& db);
void sortDatabase(vector<Contact>& db);

// --- Вспомогательная сортировка (чтобы не дублировать код) ---
void sortDatabase(vector<Contact>& db) {
    sort(db.begin(), db.end(), [](const Contact& a, const Contact& b) {
        return (a.lastName + a.firstName) < (b.lastName + b.firstName);
        });
}

// --- Форматирование (Инструмент 3: substr, length) ---
string formatStr(string s) {
    if (s.length() > 15) return s.substr(0, 12) + "...";
    return s;
}

// --- Таблица (С ОТЧЕСТВОМ) ---
void showTable(const vector<Contact>& db) {
    if (db.empty()) { cout << "База пуста!\n"; return; }
    cout << "\n" << setfill('-') << setw(115) << "-" << setfill(' ') << endl;
    cout << "|" << left << setw(16) << "Фамилия" << "|" << setw(16) << "Имя" << "|"
        << setw(16) << "Отчество" << "|" << setw(22) << "Телефон" << "|" << setw(10) << "Группа" << "|" << endl;
    cout << setfill('-') << setw(115) << "-" << setfill(' ') << endl;

    for (const auto& c : db) {
        string p = "+" + to_string(c.phone.countryCode) + "(" + to_string(c.phone.cityCode) + ")" + to_string(c.phone.number);
        string g = (c.category == 0) ? "WORK" : (c.category == 1) ? "FAMILY" : (c.category == 2) ? "FRIENDS" : "OTHERS";
        cout << "|" << left << setw(16) << formatStr(c.lastName)
            << "|" << setw(16) << formatStr(c.firstName)
            << "|" << setw(16) << formatStr(c.patronymic)
            << "|" << setw(22) << p
            << "|" << setw(10) << g << "|" << endl;
    }
    cout << setfill('-') << setw(115) << "-" << setfill(' ') << endl;
}

// --- Поиск (Инструмент 3: find) ---
void search(const vector<Contact>& db) {
    cout << "Введите поисковый запрос (Фамилия/Имя/Отчество): ";
    string q; cin >> q;
    bool found = false;
    for (const auto& c : db) {
        if (c.lastName.find(q) != string::npos || c.firstName.find(q) != string::npos || c.patronymic.find(q) != string::npos) {
            cout << "Найдено: " << c.lastName << " " << c.firstName << " " << c.patronymic << endl;
            found = true;
        }
    }
    if (!found) cout << "Ничего не найдено.\n";
}

// --- Удаление ---
void removeContact(vector<Contact>& db) {
    cout << "Введите фамилию для удаления: ";
    string ln; cin >> ln;
    auto it = remove_if(db.begin(), db.end(), [&](const Contact& c) { return c.lastName == ln; });
    if (it != db.end()) {
        db.erase(it, db.end());
        cout << "Контакт удален.\n";
    }
    else cout << "Не найдено.\n";
}

// --- Редактирование (С ИЗМЕНЕНИЕМ ФАМИЛИИ) ---
void edit(vector<Contact>& db) {
    cout << "Введите текущую фамилию для поиска: ";
    string ln; cin >> ln;
    bool found = false;
    for (auto& c : db) {
        if (c.lastName == ln) {
            cout << "--- Редактирование (найден " << c.lastName << ") ---\n";
            cout << "Новая Фамилия: "; cin >> c.lastName;
            cout << "Новое Имя: "; cin >> c.firstName;
            cout << "Новое Отчество: "; cin >> c.patronymic;
            cout << "Новый Номер: "; cin >> c.phone.number;
            cout << "Данные обновлены.\n";
            found = true;
            sortDatabase(db); // Авто-сортировка после правки
            break;
        }
    }
    if (!found) cout << "Контакт не найден.\n";
}

// --- Файлы (Инструмент 2: fstream) ---
void saveToFile(const vector<Contact>& db) {
    ofstream out("database.txt");
    for (const auto& c : db) {
        out << c.lastName << " " << c.firstName << " " << c.patronymic << " "
            << c.phone.countryCode << " " << c.phone.cityCode << " " << c.phone.number << " "
            << c.birthday.day << " " << c.birthday.month << " " << c.birthday.year << " "
            << c.email << " " << (int)c.category << endl;
    }
}

void loadFromFile(vector<Contact>& db) {
    ifstream in("database.txt");
    if (!in) return;
    Contact t; int cat;
    while (in >> t.lastName >> t.firstName >> t.patronymic >> t.phone.countryCode
        >> t.phone.cityCode >> t.phone.number >> t.birthday.day
        >> t.birthday.month >> t.birthday.year >> t.email >> cat) {
        t.category = (Group)cat;
        db.push_back(t);
    }
    sortDatabase(db); // Сортируем при загрузке
}

// === Главный цикл (Инструмент 4: do-while + switch) ===
int main() {
    SetConsoleCP(1251); SetConsoleOutputCP(1251); setlocale(LC_ALL, "Russian"); // Кодировка

    vector<Contact> database;
    loadFromFile(database);

    int choice;
    do {
        cout << "\n--- SMART-CONTACTS (Елисеев Даниил) ---\n";
        cout << "1. Добавить\n2. Таблица (Все поля)\n3. Поиск\n4. Удалить\n5. Редактировать\n6. Сортировка (Ручная)\n7. Статистика\n0. Выход\nВыбор: ";
        if (!(cin >> choice)) { cin.clear(); cin.ignore(1000, '\n'); continue; }

        switch (choice) {
        case 1: {
            Contact c;
            cout << "Фамилия: "; cin >> c.lastName; cout << "Имя: "; cin >> c.firstName;
            cout << "Отчество: "; cin >> c.patronymic;
            cout << "КодС КодГ Номер: "; cin >> c.phone.countryCode >> c.phone.cityCode >> c.phone.number;
            cout << "Дата (Д М Г): "; cin >> c.birthday.day >> c.birthday.month >> c.birthday.year;
            cout << "Email: "; cin >> c.email; cout << "Категория (0-3): ";
            int g; cin >> g; c.category = (Group)g;
            database.push_back(c);
            sortDatabase(database); // Авто-сортировка
            break;
        }
        case 2: showTable(database); break;
        case 3: search(database); break;
        case 4: removeContact(database); break;
        case 5: edit(database); break;
        case 6: sortDatabase(database); cout << "Сортировка выполнена!\n"; break;
        case 7: cout << "Всего в базе: " << database.size() << endl; break;
        case 0: saveToFile(database); break;
        }
    } while (choice != 0);

    return 0;
}