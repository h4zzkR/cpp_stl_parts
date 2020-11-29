#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <assert.h>

//
// Created by h4zzkR on 24.11.2020.
//

#ifndef BIGINT_BIGINTEGER_H
#define BIGINT_BIGINTEGER_H

class BigInteger {
//    static const long long BASE = 1e4;
//    static const int BIT = 4; // на самом деле не бит, ну ладно.
    static const long long BASE = 1e6;
    static const int BIT = 6; // на самом деле не бит, ну ладно.
    static const long long BLOCKS_N = 100;

    int size = 0;
    int blocks_size = 0;
    bool sign = true; // 1 is plus

    bool is_null = false;

    std::vector <long long> bits;

    static int getIntSize(int num) { // количество цИфЕрОк
        return (num == 0) ? 1 : static_cast<int>(trunc(log10(num))) + 1;
    }

    static int getBlocksSize(int number_size) { // длина в блоках по BASE элементов
        return ceil(number_size / (double)BIT);
    }

    void setNull() {
        for (int i = 0; i < blocks_size; ++i)
            bits[i] = 0;
        size = 0;
        sign = true;
        blocks_size = 0;
        is_null = true;
    }

    static bool isEqual(const BigInteger& first, const BigInteger& second, bool strict = true) {
        if (strict) {
            if (first.size == second.size && first.sign == second.sign) {
                for (int i = 0; i < first.blocks_size; ++i) {
                    if (first.bits[i] != second.bits[i])
                        return false;
                }
                return true;
            } else
                return false;
        } else {
            if (first.size == second.size) {
                for (int i = 0; i < first.blocks_size; ++i)
                    if (first.bits[i] != second.bits[i])
                        return false;
                return false;
            } else
                return false;
        }
    }

    void addTo(BigInteger& first, const BigInteger& num);
    void subtractFromBigger(BigInteger& from, const BigInteger& what);
    void subtractFromSmaller(BigInteger& from, const BigInteger& what);

    friend bool operator>(const BigInteger& first, const BigInteger& num);
    friend bool operator<(const BigInteger& first, const BigInteger& num);
    friend bool operator==(const BigInteger& first, const BigInteger& num);
    friend bool operator!=(const BigInteger& first, const BigInteger& num);
    friend bool operator>=(const BigInteger& first, const BigInteger& num);
    friend bool operator<=(const BigInteger& first, const BigInteger& num);
    friend BigInteger operator*(const BigInteger &num1, const BigInteger &num);
    friend BigInteger operator/(const BigInteger &num1, const BigInteger &num);
    friend BigInteger operator%(const BigInteger &num1, const BigInteger &num);

    friend std::ostream& operator<< (std::ostream &out, const BigInteger& num);
    friend std::istream &operator>>(std::istream &is, BigInteger &num);

    static BigInteger divMod(const BigInteger& dividend_, const BigInteger& divider_, bool mod = false) {
        if (!mod && (dividend_ == 0 || dividend_.size < divider_.size))
            return 0;
        if (divider_ == 1 || divider_ == -1 || divider_ == dividend_)
            return (mod) ? 0 : dividend_;
        if (!mod) {
            if (!absGreater(dividend_, divider_)) {
                if (BigInteger::isEqual(dividend_, divider_, false)) return 1;
            }
        }

        BigInteger answ;
        BigInteger dvd = dividend_;
        dvd.sign = 1;

        BigInteger dvr = divider_;
        dvr.sign = 1;

        std::string dvd_s = dvd.toString();
        std::string dvt_s = dvr.toString();

        int index = static_cast<int>(dvt_s.size()) - 1;
        BigInteger term = dvd_s.substr(0, index);

        BigInteger current_quotient;
        std::string quotient;

        while (index < dvd.size) {
            term *= 10;                 // сносим вниз
            BigInteger add = dvd_s[index] - 48;
            term += add; // след. цифру
            ++index;

            current_quotient = 1;
            while (term < dvr) {
                if (!quotient.empty())
                    quotient.push_back('0');
                if (index < dvd.size) {
                    term *= 10;
                    term += dvd_s[index] - 48;
                    ++index;
                } else {
                    break;
//                    --index;
                }
            }

            if (term == dvr) {
                quotient.push_back('1');
                term = 0;
            } else if (term > dvr) {
                BigInteger divr = dvr;
                while (divr < term) {
                    divr += dvr;
                    ++current_quotient;
                }
                if (divr == term) term = 0;
                else if (current_quotient != 1 && divr > term) {
                    --current_quotient;
                    divr -= dvr;
                    // term = term - (divr - dvr)
                    term -= divr;
                }
                quotient.append(current_quotient.toString());
            }
        }
        if (mod) {
            answ = term;
        } else
            answ = quotient;
        return answ;
    }

public:

    bool isNull() const {
        if (size == 0 && blocks_size == 0 && bits[0] == 0)
            return true;
        else
            return false;
    }

    bool isOne() const {
        if (size == 1 && blocks_size == 1 && bits[0] == 1)
            return true;
        else
            return false;
    }

    explicit operator bool() {
        return (size != 0 && bits[0] != 0);
    }

    static bool absGreater(const BigInteger &one, const BigInteger &two) { // one > b
        if (one.size == 0 && one.blocks_size == 0)
            return false;
        else if (two.size == 0 && two.blocks_size == 0)
            return true;
        else if (one.size > two.size)
            return true;
        else if (one.size < two.size)
            return false;
        for (int i = one.blocks_size - 1; i >= 0; --i) {
            if (one.bits[i] > two.bits[i])
                return true;
            if (one.bits[i] < two.bits[i])
                return false;
        }
        return false; // если равны
    }

    BigInteger operator-() const {
        BigInteger copy = *this;
        copy.sign = (sign == 0);
        return copy;
    }

    BigInteger& operator--() { // префиксный
        *this -= 1;
        return *this;
    }

    BigInteger operator--(int) { // постфиксный
        BigInteger copy(*this);
        --*this;
        return copy;
    }

    BigInteger& operator++() { // префиксный
        *this += 1;
        return *this;
    }

    BigInteger operator++(int) { // постфиксный
        BigInteger copy(*this);
        ++*this;
        return copy;
    }

    BigInteger& operator+=(const BigInteger& num);
    BigInteger& operator*=(const BigInteger &num);
    BigInteger& operator/=(const BigInteger &num);
    BigInteger& operator%=(const BigInteger &num);

    static void changeSign(BigInteger& num) {
        num.sign = !num.sign;
    }


    BigInteger& operator-=(const BigInteger& num);

    std::string toString() const;

    BigInteger() {
        bits.resize(BLOCKS_N);
        setNull();
    }

    BigInteger(const int intnum) : BigInteger() { // inverse here
        int integer = intnum;
        int i = 0;
        sign = (integer >= 0);
        if (integer != 0) {
            if (!sign) integer *= (-1);
            size = getIntSize(integer);
            blocks_size = getBlocksSize(size);
            while (integer) {
                bits[i] = integer % BASE;
                integer /= BASE;
                ++i;
            }
        }
    }

    BigInteger(const BigInteger& num) : BigInteger() {
        for (int i = 0; i < num.blocks_size; i++) {
            bits[i] = num.bits[i];
        }
        size = num.size;
        sign = num.sign;
        blocks_size = num.blocks_size;
        is_null = num.is_null;
    }

    BigInteger(std::string number) : BigInteger() { // inverse here
        if (number[0] == '-') {
            sign = 0;
//            lpos = 1;
            number = number.substr(1, number.length());
        }
        else if (number.size() == 1 && number[0] == '0') setNull();
        else {
            size = number.size();
            int block = 0;
            blocks_size = getBlocksSize(size);
            for (int i = static_cast<int>(number.length() - 1); i >= 0; i -= BigInteger::BIT) {
                if (i < BigInteger::BIT)
                    bits[block] = std::atoi(number.substr(0, i + 1).c_str());
                else
                    bits[block] = std::atoi(number.substr(i - BigInteger::BIT + 1, BigInteger::BIT).c_str());
                ++block;
            }
        }
    }

    BigInteger(const char* cstring) : BigInteger(std::string(cstring)) {}

    BigInteger& operator=(const BigInteger& num);

    ~BigInteger() = default;
};

void BigInteger::addTo(BigInteger &first, const BigInteger &num) {
    int term = 0, i = 0;
    int max_size = std::max(first.blocks_size, num.blocks_size);
    first.blocks_size = 0;
    first.size = 0;
    while (i < max_size || term != 0) {
        term = first.bits[i] + num.bits[i] + term;
        first.bits[i] = term % BASE;
        first.size += BIT;
        term /= BASE;
        ++i;
        ++blocks_size;
    }
    first.size -= BIT;
    first.size += getIntSize(first.bits[first.blocks_size-1]);
}

void BigInteger::subtractFromBigger(BigInteger &from, const BigInteger &what) { // left operand is greater than right
    int term = 0;
    int i = 0, max_size = from.blocks_size;
    int is_null = 0;
    from.blocks_size = 0;
    from.size = 0;

    while (i < max_size || term) {
        from.bits[i] -= term + what.bits[i];
        if (from.bits[i] < 0) {
            term = 1;
            from.bits[i] += BASE;
        } else if (from.bits[i] == 0) {
            is_null += 1;
            term = 0;
        } else {
            term = 0;
        }
        ++from.blocks_size;
        ++i;
    }
    if (is_null != from.blocks_size) {
        while (from.bits[from.blocks_size - 1] == 0 && from.blocks_size >= 0) { // в конце (ну т.е. начале) могли остаться нули
            --from.blocks_size;
        }
        from.size = (from.blocks_size - 1) * BIT;
        from.size += getIntSize(from.bits[from.blocks_size-1]);
    } else {
        blocks_size = 0;
        size = 0;
        sign = 1;
    }
}

void BigInteger::subtractFromSmaller(BigInteger &from, const BigInteger &what) { // left operand is greater than right
    int term = 0;
    int i = 0, max_size = what.blocks_size;
    int is_null = 0;
    from.blocks_size = 0;
    from.size = 0;

    while (i < max_size || term) {
        from.bits[i] = what.bits[i] - from.bits[i] - term;
        if (from.bits[i] < 0) {
            term = 1;
            from.bits[i] += BASE;
        } else if (from.bits[i] == 0) {
            is_null += 1;
            term = 0;
        } else {
            term = 0;
        }
        ++i;
        ++from.blocks_size;
    }
//    from.size -= BIT; // remove last bits
//    i = from.blocks_size - 1;
    if (is_null != from.blocks_size) {
        while (from.bits[from.blocks_size - 1] == 0 && from.blocks_size >= 0) { // в конце (ну т.е. начале) могли остаться нули
            --from.blocks_size;
        }
        from.size = (from.blocks_size - 1) * BIT;
        from.size += getIntSize(from.bits[from.blocks_size-1]);
    } else {
        blocks_size = 0;
        size = 0;
        sign = 1;
    }
}

bool operator==(const BigInteger &first, const BigInteger &num) {
    return BigInteger::isEqual(first, num, true);
}

bool operator!=(const BigInteger &first, const BigInteger &num) {
    return !(first == num);
}

bool operator>(const BigInteger& first, const BigInteger& num) {
    if (first.isNull()) {
        if (num.isNull() || num.sign == 1)
            return false;
        if (num.sign == 1)
            return true;
    }

    if (num.isNull()) {
        if (first.isNull() || first.sign == 0)
            return false;
        if (first.sign == 1)
            return true;
    }

    if (first.sign > num.sign) {
        return true;
    } else if (first.sign < num.sign) {
        return false;
    } else {
        bool compare = BigInteger::absGreater(first, num);
        if (first.sign == 0)
            return !compare;
        return compare;
    }
}

bool operator<(const BigInteger& first, const BigInteger& num) {
    return num > first;
}

bool operator>=(const BigInteger& first, const BigInteger& num) {
    return !(first < num);
}

bool operator<=(const BigInteger& first, const BigInteger& num) {
    return !(first > num);
}

BigInteger &BigInteger::operator+=(const BigInteger &num) {
    bool comparation = absGreater(*this, num);
    if (num.isNull())
        return *this;
    else if (isNull()) {
        *this = num;
        return *this;
    } else {
        if (sign == num.sign) {
            addTo(*this, num);
        } else if (sign == 1 && num.sign == 0) {
            if (comparation) {
                subtractFromBigger(*this, num);
            } else {
                subtractFromSmaller(*this, num);
                sign = 0;
            }
        } else if (sign == 0 && num.sign == 1) {
            if (comparation) {
                subtractFromBigger(*this, num);
            } else {
                subtractFromSmaller(*this, num);
                sign = 1;
            }
        }
        if (isNull()) is_null = true;
        return *this;
    }
}

BigInteger &BigInteger::operator-=(const BigInteger &num) {
    bool compare = absGreater(*this, num);
    if (sign == 1 && num.sign == 1) {
        if (compare) { // left is greater 10 - 5
            subtractFromBigger(*this, num);
            sign = 1;
        } else {
            subtractFromSmaller(*this, num); // 5 - 10
            sign = 0;
        }
    } else if (sign == 0 && num.sign == 0) {
        if (compare == 1) { // левый длиннее
            subtractFromBigger(*this, num); // -5 - (-2) = 2 - 5
            sign = (size != 0) ? 0 : 1;
        } else { // -5 - (-10) = 10 - 5 > 0
            subtractFromSmaller(*this, num);
            sign = 1;
        }
    } else if ((sign == 1 && num.sign == 0) || (sign == 0 && num.sign == 1)) {
        addTo(*this, num);
    }
    if (isNull()) is_null = true;
    return *this;
}

BigInteger &BigInteger::operator/=(const BigInteger &num) {
    *this = *this / num;
    return *this;
}
//
BigInteger &BigInteger::operator%=(const BigInteger &num) {
    *this = *this % num;
    return *this;
}

BigInteger operator+(const BigInteger& first, const BigInteger& second) {
    BigInteger copy = first; // RVO
    copy += second;
    return copy;
}


BigInteger operator-(const BigInteger& first, const BigInteger& second) {
    BigInteger copy = first; // RVO
    copy -= second;
    return copy;
}

BigInteger operator/(const BigInteger &dvd, const BigInteger &dvr) {
    BigInteger answ = BigInteger::divMod(dvd, dvr, false);
    if (dvd.sign != dvr.sign)
        answ.sign = 0;
    else
        answ.sign = 1;
    return answ;
}

BigInteger operator%(const BigInteger &dvd, const BigInteger &dvr) {
    if (dvd.isNull())
        return 0;
    if (!BigInteger::absGreater(dvd, dvr)) {
        return dvd;
    }
    BigInteger answ = BigInteger::divMod(dvd, dvr, true);
    if (!answ.isNull()) {
        if (dvd.sign == 0 && dvr.sign == 1)
            BigInteger::changeSign(answ);
        else if (dvd.sign == 0 && dvr.sign == 0)
            BigInteger::changeSign(answ);
        else if (dvd.sign == 1 && dvr.sign == 0) {
            return answ;
//            answ = dvr - answ;
//            BigInteger::changeSign(answ);
        }
    }
    return answ;
}

BigInteger operator*(const BigInteger &num1, const BigInteger &num) {
    BigInteger copy;
    if (num1.isNull() || num.isNull())
        copy = 0;
    else if (num1.isOne())
        copy = num;
    else if (num.isOne())
        copy = num1;
    else {
        int min_size, max_size;
        if (num1.blocks_size >= num.blocks_size) {
            max_size = num1.blocks_size;
            min_size = num.blocks_size;
        } else {
            max_size = num.blocks_size;
            min_size = num1.blocks_size;
        }

        for (int i = 0; i < max_size + 1; i++) {
            for (int j = 0; j < min_size + 1; j++) {
                copy.bits[i + j] += num1.bits[i] * num.bits[j];
            }
        }

        int last_nnul = 0, term = 0;
        for (int i = 0; i < BigInteger::BLOCKS_N; i++) {
            copy.bits[i] += term;
            term = (copy.bits[i] - term > BigInteger::BASE) ? static_cast<int>(copy.bits[i] / BigInteger::BASE) : 0;
            copy.bits[i] %= BigInteger::BASE;
            if (copy.bits[i] != 0) last_nnul = i;
        }

        copy.blocks_size = last_nnul + 1;
        copy.size = BigInteger::BIT * (copy.blocks_size - 1);
        copy.size += BigInteger::getIntSize(copy.bits[copy.blocks_size - 1]);
    }
    if (num1.sign != num.sign) BigInteger::changeSign(copy);
    return copy;
}

BigInteger &BigInteger::operator*=(const BigInteger &num) {
    *this = *this * num;
    return *this;
}

BigInteger &BigInteger::operator=(const BigInteger &num) {
    if (num != *this) {
        setNull();
        for (int i = 0; i < num.blocks_size; i++) {
            bits[i] = num.bits[i];
        }
        size = num.size;
        sign = num.sign;
        blocks_size = num.blocks_size;
    }
    return *this;
}

std::string BigInteger::toString() const {
    if (isNull()) {
        return "0";
    } else {
        std::string number = std::to_string(bits[blocks_size - 1]);
        if (sign == 0) number.insert(0, "-");
        std::string block;
        for (int i = blocks_size - 2; i >= 0; --i) {
            block = std::to_string(bits[i]);
            block.insert(0, std::string(BIT - block.size(), '0'));
            number += block;
        }
        return number;
    }
}

std::ostream &operator<<(std::ostream &out, const BigInteger &num) {
    out << num.toString();
    return out;
}

std::istream &operator>>(std::istream &is, BigInteger &num) {
    char buf = '\0';
    std::string number;
    while (is.get(buf)) {
        if (buf == '\0' || buf == ' ' || buf == '\n' || buf == '\t' || buf == '\r')
            break;
        number.push_back(buf);
    }
    if (buf == '\0')
        num = 0;
    else
        num = number;
    return is;
}

class Rational {
    int p = 0;
    int q = 0;

public:
    Rational() {
        p = 0;
        q = 0;
    }
    Rational(const BigInteger& num) : Rational() {
        num.isNull();
    }
    Rational(int num) : Rational() {
        num++;
    }

    Rational& operator+=(const Rational &num) {
        method(num);
        return *this;
    }

    Rational& operator-=(const Rational &num) {
        method(num);
        return *this;
    }

    Rational& operator*=(const Rational &num) {
        method(num);
        return *this;
    }

    Rational& operator/=(const Rational &num) {
        method(num);
        return *this;
    }

    Rational& operator%=(const Rational &num) {
        method(num);
        return *this;
    }

    bool method(Rational num) const {
        if (num.p)
            return true;
        return false;
    }

    Rational& operator=(const Rational &num) {
        method(num);
        return *this;
    }

    Rational operator-(const Rational &num) {
        method(num);
        return *this;
    }

    Rational operator-() const {
        Rational copy = *this;
        return copy;
    }

    std::string toString() {
        return std::to_string(1999);
    }

    std::string asDecimal(int size, int precision = 0) {
        size += 1;
        precision += 1;
        return std::to_string(1888);
    }

    friend bool operator>(const Rational& first, const Rational& num);
    friend bool operator<(const Rational& first, const Rational& num);
    friend bool operator==(const Rational& first, const Rational& num);
    friend bool operator!=(const Rational& first, const Rational& num);
    friend bool operator>=(const Rational& first, const Rational& num);
    friend bool operator<=(const Rational& first, const Rational& num);
    friend Rational operator*(const Rational& num1, const Rational& num);
    friend Rational operator/(const Rational& num1, const Rational& num);
    friend Rational operator%(const Rational& num1, const Rational& num);
};

bool operator>(const Rational& first, const Rational& num) {
    return first.p > num.p;
}

bool operator<(const Rational& first, const Rational& num) {
    return first.p > num.p;
}

bool operator>=(const Rational& first, const Rational& num) {
    return first.p > num.p;
}

bool operator<=(const Rational& first, const Rational& num) {
    return first.p > num.p;
}

bool operator!=(const Rational& first, const Rational& num) {
    return first.p > num.p;
}

bool operator==(const Rational& first, const Rational& num) {
    return first.p > num.p;
}

Rational operator-(const Rational &num, const Rational &num2) {
    Rational copy = num;
    Rational copy2 = num2;
    copy2.method(num);
    return copy;
}

Rational operator+(const Rational &num, const Rational &num2) {
    Rational copy = num;
    Rational copy2 = num2;
    copy2.method(num);
    return copy;
}

Rational operator*(const Rational &num, const Rational &num2) {
    Rational copy = num;
    Rational copy2 = num2;
    copy2.method(num);
    return copy;
}

Rational operator/(const Rational &num, const Rational &num2) {
    Rational copy = num;
    Rational copy2 = num2;
    copy2.method(num);
    return copy;
}

Rational operator%(const Rational &num, const Rational &num2) {
    Rational copy = num;
    Rational copy2 = num2;
    copy2.method(num);
    return copy;
}

#endif //BIGINT_BIGINTEGER_H

void command_manager() {
    std::string a, c, b;
    std::cin >> a >> c >> b;
    BigInteger aa = a;
    BigInteger bb = b;
    if (c == "+") {
        std::cout << aa + bb;
    } else if (c == "-") {
        std::cout << aa - bb;
    } else if (c == "*") {
        std::cout << aa * bb;
    } else if (c == "/") {
        std::cout << aa / bb;
    } else if (c == ">") {
        std::cout << int((aa > bb));
    } else if (c == "<") {
        std::cout << int((aa < bb));
    } else if (c == ">=") {
        std::cout << int((aa >= bb));
    } else if (c == "<=") {
        std::cout << int((aa <= bb));
    } else if (c == "==") {
        std::cout << int((aa == bb));
    } else if (c == "!=") {
        std::cout << int((aa != bb));
    }
    else if (c == "%") {
        std::cout << aa % bb;
    }
    std::cout << '\n';
}


int main() {
//    int n;
//    std::cin >> n;
//    for (int i = 0; i < n; ++i)
//        command_manager();
    const BigInteger a = 2352545;
    int aa = 2352545;
    int bb = 43434343;
    std::cout << (a > aa) << (a != aa);
    return 0;
}