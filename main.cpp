#include <windows.h>
#include <cstdlib> // rand, srand
#include <ctime> // time
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm> // copy_n
#include <sstream> // stringstream

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

void displayDataToScreen(vector<string> const &data, const string &msg) {
    std::cout << msg << ": " << std::endl;
    for (int i = 0; i < data.size(); ++i) {
        std::cout << i << ": " << data[i] << ((i % 5) == 4 ? "\n" : " ");
    }
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    std::srand(std::time(nullptr)); // NOLINT(cert-msc51-cpp)

    const char* pathName = R"(../fish_list.txt)";
    vector<string> fishList;

    bool isReadFileSuccessfully = readFileToVector(pathName, fishList);

    if (!isReadFileSuccessfully) {
        printf("%s не обнаружен.\n"
               "Он должен находиться в директории с исполняемым файлом!\n", pathName);
        return 1;
    }

    displayDataToScreen(fishList, "Выберите, какую рыбу будете ловить из данных видов");

    string selectedFish = fishList[getUserNumeric("Введите цифру", 0, (int)(fishList.size() - 1))];

    vector<string> listOfFishInRiver = getShuffledRange(fishList, getRandomIntFromRange(5, (int)(fishList.size() - 1)));

    displayDataToScreen(listOfFishInRiver, "В реке были");

    int foundIndex = findIndexInVector(listOfFishInRiver, selectedFish);

    std::cout << "Результат: " << foundIndex << std::endl;

    if (foundIndex == -1) {
        std::cout << "В реке не оказалось выбранной рыбы" << std::endl;
    } else {
        std::cout << "Поймана одна рыба: " << selectedFish << std::endl;
    }
}
