#include <windows.h>
#include <cstdlib> // rand, srand
#include <ctime> // time
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm> // copy_n
#include <sstream> // stringstream
#include <iterator>

using std::string;
using std::vector;

string getTrimmedString(std::string str, std::string const &whiteSpaces = " \r\n\t\v\f") {
    auto start = str.find_first_not_of(whiteSpaces);
    str.erase(0, start);
    auto end = str.find_last_not_of(whiteSpaces);
    str.erase(end + 1);

    return str;
}

// Разбивает строку на подстроки и возвращает вектор
vector<string> getSplitStringOnRecords(string const &str, const char delim = ',', bool isEmptyDenied = true) {
    vector<string> records;
    std::stringstream ss(str);

    string rawRecord;
    // Делим строки на токены по delim
    while (std::getline(ss, rawRecord, delim)) {
        string record = getTrimmedString(rawRecord);
        // Не позволяет добавлять пустой элемент
        if (record.empty() && isEmptyDenied) continue;

        records.push_back(record);
    }

    return records;
}

std::string getUserString(const string &propose) {
    while (true) {
        string userInput;
        printf("%s: ", propose.c_str());
        std::getline(std::cin, userInput);

        userInput = getTrimmedString(userInput);
        if (userInput.empty()) {
            std::cout << "Строка не может быть пустой. Попробуйте снова!" << std::endl;
            continue;
        }

        return getSplitStringOnRecords(userInput, ' ')[0];
    }
}

bool isNumeric(std::string const &str) {
    auto it = std::find_if(
            str.begin(),
            str.end(),
            [](char const &c) { return !std::isdigit(c); });

    return !str.empty() && it == str.end();
}

// Если from != to, тогда ввести цифры возможно лишь в диапазоне от from до to
int getUserNumeric(const string &msg = "Введите цифры", int from = 0, int to = 0) {
    string warning = "Попробуйте снова. Это должно быть целое число";
    bool isRange = (from != to);

    while (true) {
        string userInput = getUserString(msg);

        if (!isNumeric(userInput)) {
            printf("%s\n", warning.c_str());
            continue;
        }

        int num = std::stoi(userInput);

        if (isRange && (num < from || num > to)) {
            printf("%s в диапазоне (%i - %i)\n", warning.c_str(), from, to);
            continue;
        }

        return num;
    }
}

int getRandomIntFromRange(int from, int to) {
    return (from + std::rand() % (to - from + 1)); // NOLINT(cert-msc50-cpp)
}

template<typename T>
vector<T> getShuffleVector(vector<T> const &data) {
    vector<T> out = data;
    for (int i = 0; i < out.size() - 1; ++i) {
        int j = getRandomIntFromRange(i, out.size() - 1);
        std::swap(out[i], out[j]);
    }

    return out;
}

template<typename T>
vector<T> getCopyVector(vector<T> const &data, int amount = 0, int firstPos = 0) {
    vector<T> out;

    int sizeOfCopy = (amount == 0 )|| ((amount + firstPos) > data.size()) ? (data.size() - firstPos) : amount;

    std::copy_n(data.begin() + firstPos, sizeOfCopy, std::back_inserter(out));

    return out;
}

template<typename T>
vector<T> getShuffledRange(vector<T> const &data, int amount = 0, int firstPos = 0) {
    vector<T> shuffledVector = getShuffleVector(data);

    return getCopyVector(shuffledVector, amount, firstPos);
}

bool readFileToVector(const char* pathName, vector<string> &data) {
    bool isReadSuccessfully = false;
    std::ifstream in(pathName);

    if (in.is_open() && !in.bad()) {
        string textLine;

        while (std::getline(in, textLine)) data.push_back(textLine);

        isReadSuccessfully = true;
    }

    in.close();

    return isReadSuccessfully;
}

template<typename T>
int findIndexInVector(vector<T> const &source, const T &key) {
    const int NOT_FOUND = -1;
    auto it = std::find_if(source.cbegin(), source.cend(), [key](const T &i){ return i == key; });

    if (it != source.cend()) {
        return (int)std::distance(source.cbegin(), it);
    }

    return NOT_FOUND;
}

void outputData(std::ostream &out, vector<string> const &data, const string &format = " ", bool isNumbering = false) {
    for (int i = 0; i < data.size(); ++i) {
        string outputStr = isNumbering ? std::to_string(i) + ": " + data[i] : data[i];
        out << outputStr << (i != data.size() - 1 ? format : "");
    }
    out << std::endl;
}

void displayDataToScreen(vector<string> const &data, const string &msg, const string &format = ", ", bool isNumbering = false) {
    std::cout << msg << ": " << std::endl;
    outputData(std::cout, data, format, isNumbering);
}

void displayFileToScreen(const char* pathName, string const &msg) {
    vector<string> data;
    bool isReadFileSuccessfully = readFileToVector(pathName, data);

    if (!isReadFileSuccessfully) {
        printf("%s не обнаружен. Он должен находиться в директории с исполняемым файлом!\n", pathName);
        return;
    }

    system("cls");
    if (!data.empty()) {
        displayDataToScreen(data, msg, ", ");
    } else {
        std::cout << "Данных пока нет" << std::endl;
    }
}

void writeDataToFile(const char* path, vector<string> const &data, bool isAppendMode = false, const string &format = ",") {
    std::ofstream file(path, (isAppendMode ? std::ios::app : std::ios::out));
    outputData(file, data, format);
    file.close();
}

// ======================================================================================================

int getIndexOfSelectedFish(vector<string> const &fishList) {
    string msg = "Выберите, какую рыбу будете ловить из данных видов";
    displayDataToScreen(fishList, msg, ", ", true);
    return getUserNumeric("Введите цифру", 0, (int)(fishList.size() - 1));
}

vector<string> getListOfFishInRiver(vector<string> const &fishList) {
    int numberOfRandomFish = getRandomIntFromRange(5, (int)(fishList.size() - 1));
    return getShuffledRange(fishList, numberOfRandomFish);
}

bool hasFishCaught(vector<string> const &fishList, vector<string> const &listOfFishInRiver, const int indexOfSelectedFish) {
    displayDataToScreen(listOfFishInRiver, "В реке были");
    int foundIndex = findIndexInVector(listOfFishInRiver, fishList[indexOfSelectedFish]);

    if (foundIndex == -1) {
        std::cout << "В реке не оказалось выбранной рыбы" << std::endl;
        return false;
    } else {
        std::cout << "Поймана одна рыба: " << fishList[indexOfSelectedFish] << std::endl;
        return true;
    }
}

int getIndexOfMenu() {
    const char* menu[] = {
            "Продолжить ловить рыбу",
            "Посмотреть результат и продолжить ловить",
            "Завершить ловлю и посмотреть результат"
    };

    int sizeOfMenu = std::end(menu) - std::begin(menu);

    for (int i = 0; i < sizeOfMenu; ++i) {
        std::cout << i << ": " << menu[i] << std::endl;
    }

    return getUserNumeric("Введите цифру", 0, (sizeOfMenu - 1));
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    std::srand(std::time(nullptr)); // NOLINT(cert-msc51-cpp)

    const char* pathNameOfFishList = R"(../fish_list.txt)";
    const char* pathNameOfBasket = R"(../basket.txt)";
    vector<string> fishList;

    // Читаем данные из файла. В данном случае - перечень рыб
    bool isReadFileSuccessfully = readFileToVector(pathNameOfFishList, fishList);

    if (!isReadFileSuccessfully) {
        printf("%s не обнаружен. Он должен находиться в директории с исполняемым файлом!\n", pathNameOfFishList);
        return 1;
    }

    // Основной цикл выполнения
    while(true) {
        // Получаем индекс рыбы, которую пользователь собирается ловить
        int indexOfSelectedFish = getIndexOfSelectedFish(fishList);

        // Получаем список случайных рыб, которые прямо сейчас находятся в реке
        vector<string> listOfFishInRiver = getListOfFishInRiver(fishList);

        // Поймана ли рыба
        bool isFishCaught = hasFishCaught(fishList, listOfFishInRiver, indexOfSelectedFish);

        if (isFishCaught) {
            writeDataToFile(pathNameOfBasket, { fishList[indexOfSelectedFish] }, true);
        }

        // Выбираем из меню дальнейшее действие
        int indexOfMenu = getIndexOfMenu();

        switch (indexOfMenu) {
            case 0:
                system("cls");
                continue;
            case 1:
                displayFileToScreen(pathNameOfBasket, "Сейчас выловлено");
                continue;
            default:
                displayFileToScreen(pathNameOfBasket, "Сейчас выловлено");
        }

        break;
    }

    std::cout << "Игра окончена!" << std::endl;
}
