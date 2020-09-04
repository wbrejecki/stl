#include <algorithm>
#include <cmath>
#include <functional>
#include <map>
#include <string>

#include "advancedCalculator.hpp"

const std::string operations = "+-*/%$^!";
const std::string digits = "1234567890.";

const std::map<std::string, std::function<double(double, double)>> command = {{"+", std::plus<double>()},
                                                                              {"-", std::minus<double>()},
                                                                              {"*", std::multiplies<double>()},
                                                                              {"/", std::divides<double>()},
                                                                              {"%", std::modulus<int>()},
                                                                              {"^", [](double base, double exp) { return pow(base, exp); }},
                                                                              {"$", [](double base, double root) { return pow(base, 1.0 / root); }},
                                                                              {"!", [](double base, double ignored) { return tgamma(base); }}};

ErrorCode process(std::string input, double* out) {
    double firstNum, secondNum;
    std::string operation{};

    if (!checkCharacters(input)) {
        return ErrorCode::BadCharacter;
    }
    removeSpaces(input);
    size_t distance = findOperation(input);
    auto pairOfNumbers = separateNums(input, distance);

    if (input[distance] == '!') {
        if (!pairOfNumbers.second.empty()) {
            return ErrorCode::BadFormat;
        } else {
            if (!checkNumber(pairOfNumbers.first)) {
                return ErrorCode::BadFormat;
            }
            firstNum = std::stod(pairOfNumbers.first, nullptr);
            secondNum = 0;
            operation += input[distance];
            if (firstNum <= 0) {
                *out = 1;
                return ErrorCode::OK;
            }
            *out = command.at(operation)(firstNum + 1, secondNum);
            return ErrorCode::OK;
        }
    }

    if (!checkNumber(pairOfNumbers.first) || !checkNumber(pairOfNumbers.second)) {
        return ErrorCode::BadFormat;
    }

    firstNum = std::stod(pairOfNumbers.first, nullptr);
    secondNum = std::stod(pairOfNumbers.second, nullptr);

    if (checkIfDividedByZero(input, distance, secondNum)) {
        return ErrorCode::DivideBy0;
    }
    if (checkSqrtOfNegativeNumber(input, distance, firstNum)) {
        return ErrorCode::SqrtOfNegativeNumber;
    }
    if (checkIfModuloOfNonIntegerValue(input, distance, pairOfNumbers)) {
        return ErrorCode::ModuleOfNonIntegerValue;
    }
    operation += input[distance];
    *out = command.at(operation)(firstNum, secondNum);

    return ErrorCode::OK;
}

bool checkCharacters(const std::string& input) {
    std::string allowedChars{"0123456789+-/*!$^%., "};
    return std::all_of(input.begin(), input.end(),
                       [&](const auto& character) { return std::any_of(allowedChars.begin(), allowedChars.end(),
                                                                       [character](const auto& goodChar) { return character == goodChar; }); });
}

void removeSpaces(std::string& input) {
    input.erase(std::remove(input.begin(), input.end(), ' '), input.end());
}

size_t findOperation(const std::string& input) {
    size_t n = 0;
    if (input[0] == '-') {
        n = 1;
    }
    auto operation = std::find_if(input.begin() + n, input.end(), [&](auto ch) { return std::any_of(operations.begin(), operations.end(), [&](auto op) { return ch == op; }); });

    return std::distance(input.begin(), operation);
}

std::pair<std::string, std::string> separateNums(std::string input, const size_t& distance) {
    size_t trim = input.length() - distance;
    std::string buffer = input;
    std::string num1 = input.erase(distance, trim);
    std::string num2 = buffer.erase(0, distance + 1);

    return std::make_pair(num1, num2);
}

bool checkFirstChar(const std::string& input) {
    if ((isdigit(input[0])) || (input[0] == '-')) {
        return true;
    }
    return false;
}

bool checkNumber(const std::string& input) {
    size_t dotsCounter = 0;
    if (input.empty())
        return false;

    if (!checkFirstChar(input)) {
        return false;
    }
    if ((input[0] == '-') && (input[1] == '.' || !isdigit(input[1]))) {
        return false;
    }

    for (size_t i = 1; i < input.length(); i++) {
        if (input[i] == '.') {
            dotsCounter++;
            if (dotsCounter > 1) {
                return false;
            } else {
                continue;
            }
        }
        if (!isdigit(input[i])) {
            return false;
        }
    }
    if (input.back() == '.') {
        return false;
    }

    return true;
}

bool checkIfDividedByZero(const std::string& input, const size_t& distance, const double& secondNum) {
    if (input[distance] == '/' && secondNum == 0) {
        return true;
    }
    return false;
}

bool checkSqrtOfNegativeNumber(const std::string& input, const size_t& distance, const double& firstNum) {
    if (input[distance] == '$' && firstNum < 0) {
        return true;
    }
    return false;
}

bool checkIfModuloOfNonIntegerValue(const std::string& input, const size_t& distance, const std::pair<std::string, std::string>& pair) {
    if (input[distance] == '%') {
        if (std::stoi(pair.first) != std::stod(pair.first, nullptr) ||
            std::stoi(pair.second) != std::stod(pair.second, nullptr)) {
            return true;
        }
    }
    return false;
}
