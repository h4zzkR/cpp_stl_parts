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
    static const long long BASE = 1e9;
    static const int BIT = 9; // на самом деле не бит, ну ладно.
    static const long long BLOCKS_N = 200;

    int size = 0;
    int blocks_size = 0;
    bool sign = true; // 1 is plus

    std::vector <long long> blocks;

    static int getIntSize(int num) { // количество цИфЕрОк
        return (num == 0) ? 1 : static_cast<int>(trunc(log10(num))) + 1;
    }

    static int getBlocksSize(int number_size) { // длина в блоках по BASE элементов
        return ceil(number_size / (double)BIT);
    }

    void setNull() {
        for (int i = 0; i < blocks_size; ++i)
            blocks[i] = 0;
        size = 0;
        sign = true;
        blocks_size = 0;
    }

    static bool isEqual(const BigInteger& first, const BigInteger& second, bool strict = true) {
        if (first.size != second.size)
            return false;
        if (strict && (first.sign != second.sign)) // с учетом знака
            return false;
        else { // без учета знака
            for (int i = 0; i < first.blocks_size; ++i)
                if (first.blocks[i] != second.blocks[i])
                    return false;
        }
        return true;
    }

    static bool absGreater(const BigInteger &one, const BigInteger &two) { // one > b
        if (!one.isNull() && two.isNull()) return true;
        if (one.size > two.size) return true;
        if (one.size == two.size) {
            for (int i = one.blocks_size - 1; i >= 0; --i) {
                if (one.blocks[i] > two.blocks[i])
                    return true;
                if (one.blocks[i] < two.blocks[i])
                    return false;
            }
        }
        return false;
    }

    static void changeSign(BigInteger& num) {
        num.sign = !num.sign;
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
                } else
                    break;
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
        return (mod) ? term : quotient;
    }

public:

    bool isNull() const {
        return (size == 0 && blocks_size == 0 && blocks[0] == 0);
    }

    bool isOne() const {
        return (size == 1 && blocks_size == 1 && blocks[0] == 1);
    }

    explicit operator bool() {
        return (size != 0 && blocks[0] != 0);
    }

    BigInteger operator-() const {
        BigInteger copy(*this);
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

    BigInteger& operator-=(const BigInteger& num);
    std::string toString() const;

    BigInteger() {
        blocks.resize(BLOCKS_N);
        setNull();
    }

    BigInteger(const int integer) : BigInteger() {
        int integer_ = integer;
        int i = 0;
        if (integer != 0) {
            sign = (integer >= 0);
            if (sign == 0) integer_ *= (-1);
            size = getIntSize(integer_);
            blocks_size = getBlocksSize(size);
            while (integer_) {
                blocks[i] = integer_ % BASE;
                integer_ /= BASE;
                ++i;
            }
        }
    }

    explicit BigInteger(const char integer) : BigInteger(integer - 48) {}

    BigInteger(const BigInteger& num) : BigInteger() {
        size = num.size;
        sign = num.sign;
        blocks_size = num.blocks_size;
        for (int i = 0; i < blocks_size; i++) {
            blocks[i] = num.blocks[i];
        }
    }

    BigInteger(const std::string& num) : BigInteger() { // check
        std::string number = num;
        if (number.size() == 1 && number[0] == '0') setNull();
        else {
            if (number[0] == '-') {
                sign = 0;
                number = number.substr(1, number.length());
            }
            size = number.size();
            int block = 0, last_pos = size;
            blocks_size = getBlocksSize(size);
            for (int i = static_cast<int>(number.size() - 1); i >= 0; i -= BigInteger::BIT) {
                if (i < BigInteger::BIT) {
                    blocks[block] = std::atoi(number.substr(0, last_pos).c_str());
                } else {
                    last_pos = i - BigInteger::BIT + 1;
                    blocks[block] = std::atoi(number.substr(last_pos, BigInteger::BIT).c_str());
                }
                ++block;
            }
        }
    }

    BigInteger(const char* cstring) : BigInteger(std::string(cstring)) {}
    BigInteger& operator=(const BigInteger& num);
    ~BigInteger() = default;
};

void BigInteger::addTo(BigInteger &first, const BigInteger &num) {
    int i = 0;
    long long term = 0;
    int max_size = std::max(first.blocks_size, num.blocks_size);
    first.blocks_size = 0;
    first.size = 0;
    while (i < max_size || term != 0) {
        term = first.blocks[i] + num.blocks[i] + term;
        first.blocks[i] = term % BASE;
        first.size += BIT;
        term /= BASE;
        ++i;
        ++blocks_size;
    }
    first.size -= BIT;
    first.size += getIntSize(first.blocks[first.blocks_size-1]);
}

void BigInteger::subtractFromBigger(BigInteger &from, const BigInteger &what) { // left operand is greater than right
    long long term = 0;
    int i = 0, max_size = from.blocks_size;
    int is_null = 0;
    from.blocks_size = 0;
    from.size = 0;

    while (i < max_size || term) {
        from.blocks[i] -= term + what.blocks[i];
        if (from.blocks[i] < 0) {
            term = 1;
            from.blocks[i] += BASE;
        } else if (from.blocks[i] == 0) {
            is_null += 1;
            term = 0;
        } else {
            term = 0;
        }
        ++from.blocks_size;
        ++i;
    }
    if (is_null != from.blocks_size) {
        while (from.blocks[from.blocks_size - 1] == 0 && from.blocks_size >= 0) { // в конце (ну т.е. начале) могли остаться нули
            --from.blocks_size;
        }
        from.size = (from.blocks_size - 1) * BIT;
        from.size += getIntSize(from.blocks[from.blocks_size-1]);
    } else {
        blocks_size = 0;
        size = 0;
        sign = 1;
    }
}

void BigInteger::subtractFromSmaller(BigInteger &from, const BigInteger &what) { // left operand is greater than right
    long long term = 0;
    int i = 0, max_size = what.blocks_size;
    int is_null = 0;
    from.blocks_size = 0;
    from.size = 0;

    while (i < max_size || term) {
        from.blocks[i] = what.blocks[i] - from.blocks[i] - term;
        if (from.blocks[i] < 0) {
            term = 1;
            from.blocks[i] += BASE;
        } else if (from.blocks[i] == 0) {
            is_null += 1;
            term = 0;
        } else {
            term = 0;
        }
        ++i;
        ++from.blocks_size;
    }
//    from.size -= BIT; // remove last blocks
//    i = from.blocks_size - 1;
    if (is_null != from.blocks_size) {
        while (from.blocks[from.blocks_size - 1] == 0 && from.blocks_size >= 0) { // в конце (ну т.е. начале) могли остаться нули
            --from.blocks_size;
        }
        from.size = (from.blocks_size - 1) * BIT;
        from.size += getIntSize(from.blocks[from.blocks_size-1]);
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
    } else { // знаки равны
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
    bool compare = absGreater(*this, num);
    if (num.isNull())
        return *this;
    else if (isNull()) {
        *this = num;
        return *this;
    } else {
        if (sign == num.sign) {
            addTo(*this, num);
        } else if (sign == 1 && num.sign == 0) {
            if (compare) {
                subtractFromBigger(*this, num);
            } else {
                subtractFromSmaller(*this, num);
                sign = 0;
            }
        } else if (sign == 0 && num.sign == 1) {
            if (compare) {
                subtractFromBigger(*this, num);
            } else {
                subtractFromSmaller(*this, num);
                sign = 1;
            }
        }
        return *this;
    }
}

BigInteger &BigInteger::operator-=(const BigInteger &num) {
    bool compare = absGreater(*this, num);
    if (sign == 1 && num.sign == 1) {
        if (compare) { // left is greater 10 - 5
            subtractFromBigger(*this, num);
        } else {
            subtractFromSmaller(*this, num); // 5 - 10
            sign = false;
        }
    } else if (sign == 0 && num.sign == 0) {
        if (compare) { // левый длиннее
            subtractFromBigger(*this, num); // -55 - (-2) = 2 - 55
        } else { // -5 - (-10) = 10 - 5 > 0
            subtractFromSmaller(*this, num);
            sign = 1;
        }
    } else if ((sign == 1 && num.sign == 0) || (sign == 0 && num.sign == 1)) {
        addTo(*this, num);
    }
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
    BigInteger answ;
    if (dvd == dvr) answ = 1;
    else answ = BigInteger::divMod(dvd, dvr, false);
    (dvd.sign != dvr.sign) ? answ.sign = 0 : answ.sign = 1;
    return answ;
}

BigInteger operator%(const BigInteger &dvd, const BigInteger &dvr) { // check
    if (dvd == dvr)
        return 0;
    if (dvd.isNull())
        return 0;
    if (!BigInteger::absGreater(dvd, dvr)) {
        return dvd;
    }
    BigInteger answ = BigInteger::divMod(dvd, dvr, true);
    if (!answ.isNull() && dvd.sign == 0) {
        BigInteger::changeSign(answ);
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
//        int min_size, max_size;
//        if (num1.blocks_size >= num.blocks_size) {
//            max_size = num1.blocks_size;
//            min_size = num.blocks_size;
//        } else {
//            max_size = num.blocks_size;
//            min_size = num1.blocks_size;
//        }

        for (int i = 0; i < BigInteger::BLOCKS_N; ++i) {
            for (int j = 0; j < BigInteger::BLOCKS_N; ++j) {
                copy.blocks[i + j] += num1.blocks[i] * num.blocks[j];
            }
        }

        int last_nnul = 0;
        long long term = 0;
//        int term = 0;
        for (int i = 0; i < BigInteger::BLOCKS_N; ++i) {
            copy.blocks[i] += term;
            term = static_cast<long long>(copy.blocks[i] / BigInteger::BASE);
//            term = (copy.blocks[i] - term > BigInteger::BASE) ? static_cast<int>(copy.blocks[i] / BigInteger::BASE) : 0;
            copy.blocks[i] %= BigInteger::BASE;
            if (copy.blocks[i] != 0) last_nnul = i;
        }

        copy.blocks_size = last_nnul + 1;
        copy.size = BigInteger::BIT * (copy.blocks_size - 1);
        copy.size += BigInteger::getIntSize(copy.blocks[copy.blocks_size - 1]);
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
            blocks[i] = num.blocks[i];
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
        std::string number = std::to_string(blocks[blocks_size - 1]);
        if (sign == 0) number.insert(0, "-");
        std::string block;
        for (int i = blocks_size - 2; i >= 0; --i) {
            block = std::to_string(blocks[i]);
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
    const BigInteger aa = a;
    const BigInteger bb = b;
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
////
////
//int main() {
//    int n;
//    std::cin >> n;
//    for (int i = 0; i < n; ++i)
//        command_manager();
//    BigInteger a = "944056378772093043145649633345434714550203648413095474431004559525090854869900666631154812102775844134500068981994894178449978962689230344482444439571178810443341548350539078808180609004555557784805711763278472476309085027062603898480658152053593442154598441830932550187182339416837745747498678693213503986720";
//    BigInteger b = "5027857645568181866223649830586550074143050932531550509565256049254533946495828098551922386089288479210594411086496583340918131373527244346676841029974953710589302110879411706534757663736360206032921020931658578524066012509047250235117269177328873510040352936917808839345830040609417448200314460798594927210";
//    std::cout << a * b;
//    BigInteger a = -10;
//    BigInteger b = -5;
//    a += b;
//    std::cout << a;
//    return 0;
//}
