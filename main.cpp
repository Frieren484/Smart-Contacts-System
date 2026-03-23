#include <algorithm>
#include <cctype>
#include <ctime>
#include <fstream>
#include <iomanip>
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <clocale>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>

using namespace std;

// Перечисление Group
enum Group { WORK, FAMILY, FRIENDS, OTHERS };

string groupToString(Group g) {
  switch (g) {
  case WORK:
    return "WORK";
  case FAMILY:
    return "FAMILY";
  case FRIENDS:
    return "FRIENDS";
  default:
    return "OTHERS";
  }
}

// Структура Date
struct Date {
  int day, month, year;
};

// Структура Phone
struct Phone {
  int countryCode;
  int cityCode;
  long long number;
};

// Структура Contact
struct Contact {
  string lastName, firstName, middleName;
  Phone phone;
  Date birthday;
  string email;
  Group category;
};

// --- Валидация и вспомогательные функции ---

// Безопасный ввод чисел (с обработкой ошибок)
int getSafeInt(int minVal, int maxVal) {
  int val;
  string input;
  while (true) {
    cin >> input;
    try {
      size_t pos;
      val = stoi(input, &pos);
      // Убедимся, что вся строка - это число
      if (pos == input.length() && val >= minVal && val <= maxVal) {
        return val;
      }
    } catch (...) {
      // Игнорируем исключения
    }
    cout << "Ошибка! Введите целое число в диапазоне (" << minVal << "-"
         << maxVal << "): ";
    cin.clear();
    cin.ignore(10000, '\n');
  }
}

// Ввод телефона (только цифры)
long long getSafePhone() {
  string input;
  while (true) {
    cin >> input;
    bool valid = true;
    for (char c : input) {
      if (!isdigit(c)) {
        valid = false;
        break;
      }
    }
    if (valid && !input.empty()) {
      try {
        return stoll(input);
      } catch (...) {
      }
    }
    cout << "Ошибка! Номер не должен содержать буквы. Повторите ввод: ";
    cin.clear();
    cin.ignore(10000, '\n');
  }
}

// Валидация Email (собачка и точка после нее)
bool isValidEmail(const string &email) {
  size_t atPos = email.find('@');
  if (atPos == string::npos)
    return false;
  size_t dotPos = email.find('.', atPos);
  return dotPos != string::npos && dotPos > atPos + 1 &&
         dotPos < email.length() - 1;
}

// Обрезка строки для таблицы
string formatStr(string name, int maxLen = 15) {
  if (name.length() > (size_t)maxLen) {
    return name.substr(0, maxLen) + "...";
  }
  return name;
}

// Перевод строки в нижний регистр
string toLower(string s) {
  string res = s;
  transform(res.begin(), res.end(), res.begin(),
            [](unsigned char c) { return tolower(c); });
  return res;
}

// --- Основные функции программы ---

int getConsoleWidth() {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
  }
  return 80; // По умолчанию
}

void printHeader() {
  int w = getConsoleWidth();
  int tableW = (w > 140) ? 140 : (w < 80 ? 80 : w - 1);

  cout << setfill('-') << setw(tableW) << "-" << setfill(' ') << endl;
  cout << "| " << left << setw(15) << "Фамилия" << "| " << setw(15) << "Имя"
       << "| " << setw(15) << "Отчество" << "| " << setw(15) << "Телефон"
       << "| " << setw(12) << "Дата рожд." << "| " << setw(20) << "Email"
       << "| " << setw(12) << "Категория |" << endl;
  cout << setfill('-') << setw(tableW) << "-" << setfill(' ') << endl;
}

void printContactRow(const Contact &c) {
  int w = getConsoleWidth();
  int tableW = (w > 140) ? 140 : (w < 80 ? 80 : w - 1);

  string phoneStr = "+" + to_string(c.phone.countryCode) + "(" +
                    to_string(c.phone.cityCode) + ")" +
                    to_string(c.phone.number);
  string dateStr = to_string(c.birthday.day) + "." +
                   to_string(c.birthday.month) + "." +
                   to_string(c.birthday.year);

  // Динамическое сокращение в зависимости от ширины экрана
  int nameW = (w < 100) ? 10 : 15;
  int emailW = (w < 120) ? 15 : 20;

  cout << "| " << left << setw(15) << formatStr(c.lastName, nameW) << "| "
       << setw(15) << formatStr(c.firstName, nameW) << "| " << setw(15)
       << formatStr(c.middleName, nameW) << "| " << setw(15)
       << formatStr(phoneStr, 14) << "| " << setw(12) << dateStr << "| "
       << setw(20) << formatStr(c.email, emailW) << "| " << setw(12)
       << formatStr(groupToString(c.category), 10) << " |" << endl;
}

Contact createContact() {
  Contact c;
  cout << "Фамилия: ";
  cin >> c.lastName;
  cout << "Имя: ";
  cin >> c.firstName;
  cout << "Отчество: ";
  cin >> c.middleName;

  cout << "Код страны (1-999): ";
  c.phone.countryCode = getSafeInt(1, 999);
  cout << "Код города (0-9999): ";
  c.phone.cityCode = getSafeInt(0, 9999);
  cout << "Номер (без букв): ";
  c.phone.number = getSafePhone();

  cout << "День рождения (1-31): ";
  c.birthday.day = getSafeInt(1, 31);
  cout << "Месяц рождения (1-12): ";
  c.birthday.month = getSafeInt(1, 12);
  cout << "Год рождения (1900-2026): ";
  c.birthday.year = getSafeInt(1900, 2026);

  do {
    cout << "Email: ";
    cin >> c.email;
    if (!isValidEmail(c.email))
      cout << "Некорректный формат почты!" << endl;
  } while (!isValidEmail(c.email));

  cout << "Категория (0-WORK, 1-FAMILY, 2-FRIENDS, 3-OTHERS): ";
  c.category = (Group)getSafeInt(0, 3);

  return c;
}

void addContact(vector<Contact> &db) {
  db.push_back(createContact());
  cout << "Контакт успешно добавлен!" << endl;
}

void showAll(const vector<Contact> &db) {
  if (db.empty()) {
    cout << "База пуста." << endl;
    return;
  }
  printHeader();
  for (const auto &c : db) {
    printContactRow(c);
  }
}

// 3. Поиск по маске (несколько ключевых слов, Фамилия или Имя)
void searchContacts(const vector<Contact> &db) {
  if (db.empty()) {
    cout << "База пуста." << endl;
    return;
  }
  cout << "Введите ключевые слова для поиска (через пробел, 0 для отмены): ";
  string line;
  cin.ignore(10000, '\n');
  getline(cin, line);
  if (line == "0")
    return;

  stringstream ss(line);
  string word;
  vector<string> keywords;
  while (ss >> word) {
    keywords.push_back(toLower(word));
  }

  bool foundAny = false;
  printHeader();

  for (const auto &c : db) {
    string searchTarget = toLower(c.lastName + " " + c.firstName);
    bool matchAll = true;
    for (const string &kw : keywords) {
      if (searchTarget.find(kw) == string::npos) {
        matchAll = false;
        break;
      }
    }
    if (matchAll && !keywords.empty()) {
      printContactRow(c);
      foundAny = true;
    }
  }

  if (!foundAny) {
    cout << "Контакты не найдены." << endl;
  }
}

// 4. Удаление по фамилии
void deleteContact(vector<Contact> &db) {
  if (db.empty()) {
    cout << "База пуста." << endl;
    return;
  }
  cout << "Введите Фамилию для удаления: ";
  string target;
  cin >> target;
  string targetLower = toLower(target);

  auto it = remove_if(db.begin(), db.end(), [&](const Contact &c) {
    return toLower(c.lastName) == targetLower;
  });

  if (it != db.end()) {
    db.erase(it, db.end());
    cout << "Контакт(ы) удален(ы)." << endl;
  } else {
    cout << "Контакт с такой фамилией не найден." << endl;
  }
}

// 5. Редактирование существующего контакта
void editContact(vector<Contact> &db) {
  if (db.empty()) {
    cout << "База пуста." << endl;
    return;
  }
  cout << "Введите Фамилию для редактирования: ";
  string target;
  cin >> target;
  string targetLower = toLower(target);

  bool found = false;
  for (auto &c : db) {
    if (toLower(c.lastName) == targetLower) {
      cout << "Редактирование контакта: " << c.lastName << " " << c.firstName
           << endl;
      c = createContact();
      found = true;
      cout << "Обновлено успешно!" << endl;
      break;
    }
  }

  if (!found) {
    cout << "Контакт не найден." << endl;
  }
}

// 6. Сортировка по алфавиту
void sortAlphabetical(vector<Contact> &db) {
  sort(db.begin(), db.end(), [](const Contact &a, const Contact &b) {
    if (a.lastName == b.lastName)
      return a.firstName < b.firstName;
    return a.lastName < b.lastName;
  });
  cout << "Отсортировано по алфавиту." << endl;
}

// Получить "вес" даты для сортировки по ближайшему ДР (от сегодня)
int getDaysToBirthday(const Date &bday) {
  time_t t = time(0);
  tm *now = localtime(&t);
  int curMonth = now->tm_mon + 1;
  int curDay = now->tm_mday;

  int bDaySort = bday.month * 100 + bday.day;
  int curSort = curMonth * 100 + curDay;

  if (bDaySort >= curSort) {
    return bDaySort - curSort; // В этом году
  } else {
    return bDaySort - curSort + 1300; // В следующем году
  }
}

// 6. Сортировка по дате рождения (от ближайших)
void sortBirthday(vector<Contact> &db) {
  sort(db.begin(), db.end(), [](const Contact &a, const Contact &b) {
    return getDaysToBirthday(a.birthday) < getDaysToBirthday(b.birthday);
  });
  cout << "Отсортировано по дате рождения." << endl;
}

// ДОП ФУНКЦИЯ 1: Экспорт в CSV
void exportToCSV(const vector<Contact> &db) {
  ofstream out("contacts_export.csv");
  if (!out) {
    cout << "Ошибка при создании CSV файла." << endl;
    return;
  }
  out << "LastName,FirstName,MiddleName,Phone,Birthday,Email,Category\n";
  for (const auto &c : db) {
    out << c.lastName << "," << c.firstName << "," << c.middleName << ",+"
        << c.phone.countryCode << "(" << c.phone.cityCode << ")"
        << c.phone.number << "," << c.birthday.day << "." << c.birthday.month
        << "." << c.birthday.year << "," << c.email << ","
        << groupToString(c.category) << "\n";
  }
  cout << "Успешно экспортировано в contacts_export.csv" << endl;
}

// ДОП ФУНКЦИЯ 2: Фильтрация по категории
void filterByCategory(const vector<Contact> &db) {
  cout << "Выберите категорию (0-WORK, 1-FAMILY, 2-FRIENDS, 3-OTHERS): ";
  int cat = getSafeInt(0, 3);
  Group g = (Group)cat;

  bool found = false;
  printHeader();
  for (const auto &c : db) {
    if (c.category == g) {
      printContactRow(c);
      found = true;
    }
  }
  if (!found) {
    cout << "В данной категории контактов нет." << endl;
  }
}

// Чтение и запись в файл
void saveToFile(const vector<Contact> &db) {
  ofstream out("database.txt");
  for (const auto &c : db) {
    out << c.lastName << " " << c.firstName << " " << c.middleName << " "
        << c.phone.countryCode << " " << c.phone.cityCode << " "
        << c.phone.number << " " << c.birthday.day << " " << c.birthday.month
        << " " << c.birthday.year << " " << c.email << " " << (int)c.category
        << endl;
  }
}

void loadFromFile(vector<Contact> &db) {
  ifstream in("database.txt");
  if (!in)
    return; // Файл может не существовать при первом запуске
  Contact c;
  int cat;
  while (in >> c.lastName >> c.firstName >> c.middleName >>
         c.phone.countryCode >> c.phone.cityCode >> c.phone.number >>
         c.birthday.day >> c.birthday.month >> c.birthday.year >> c.email >>
         cat) {
    c.category = (Group)cat;
    db.push_back(c);
  }
}

int main() {
  SetConsoleCP(65001);
  SetConsoleOutputCP(65001);
  setlocale(LC_ALL, ".65001");

  vector<Contact> database;
  loadFromFile(database); // Загрузка базы при запуске
  int choice;

  do {
    cout << "\n--- Информационная система Smart-Contacts ---\n";
    cout << "1. Добавить контакт\n";
    cout << "2. Просмотреть всех\n";
    cout << "3. Поиск по маске (Фамилия Имя)\n";
    cout << "4. Удалить контакт (по Фамилии)\n";
    cout << "5. Редактировать контакт (по Фамилии)\n";
    cout << "6. Сортировка по алфавиту (А-Я)\n";
    cout << "7. Сортировка по дате рождения (Ближайшие)\n";
    cout << "8. [Доп] Экспорт в CSV\n";
    cout << "9. [Доп] Фильтр по категории\n";
    cout << "10. Сохранить и выйти\n";
    cout << "Выбор: ";

    choice = getSafeInt(1, 10);

    switch (choice) {
    case 1:
      addContact(database);
      break;
    case 2:
      showAll(database);
      break;
    case 3:
      searchContacts(database);
      break;
    case 4:
      deleteContact(database);
      break;
    case 5:
      editContact(database);
      break;
    case 6:
      sortAlphabetical(database);
      break;
    case 7:
      sortBirthday(database);
      break;
    case 8:
      exportToCSV(database);
      break;
    case 9:
      filterByCategory(database);
      break;
    }
  } while (choice != 10);

  saveToFile(database); // Сохранение при выходе
  return 0;
}