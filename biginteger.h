#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <assert.h>

#include <algorithm>

//
// Created by h4zzkR on 24.11.2020.
//

#ifndef BIGINT_BIGINTEGER_H
#define BIGINT_BIGINTEGER_H

class BigInteger {
    static const long long BASE = 1e9;
    static const int BIT = 9;
    static const long long BLOCKS_N = 100;

    int size = 0;
    int blocks_size = 0;
    bool sign = true; // 1 is plus

    std::vector <long long> blocks;

    static int getIntSize(int num) { // get length of integer
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
        if (divider_ == dividend_)
            return (mod) ? 0 : 1;
        if (divider_ == 1 || divider_ == -1)
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
            term += dvd_s[index] - 48; // след. цифру
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

    void changeSign() {
        sign = (sign == 0);
    }

    void setSign(bool sign) {
        this->sign = sign;
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

    bool isEven() const {
        return !(blocks[0] % 2);
    }

    bool isNull() const {
        return (blocks[0] == 0 && size == 0 && blocks_size == 0);
    }

    bool isOne() const {
        return (blocks[0] == 1 && sign == 1 && size == 1 && blocks_size == 1);
    }

    explicit operator bool() {
        return (size != 0 && blocks[0] != 0);
    }

    int getSize() const {
        return size;
    }

    bool getSign() const {
        return sign;
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
        for (int i = 0; i < blocks_size; i++)
            blocks[i] = num.blocks[i];
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
                if (i < BigInteger::BIT)
                    blocks[block] = std::atoi(number.substr(0, last_pos).c_str());
                else {
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

    static bool getSign(const BigInteger& num) {
        return num.sign;
    }
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
    if (!answ.isNull() && dvd.sign == 0)
        answ.changeSign();
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
        for (int i = 0; i < BigInteger::BLOCKS_N; ++i) {
            for (int j = 0; j < BigInteger::BLOCKS_N; ++j) {
                if (i + j > BigInteger::BLOCKS_N) break;
                copy.blocks[i + j] += num1.blocks[i] * num.blocks[j];
            }
        }
        int last_nnul = 0;
        long long term = 0;
        // нормализация блоков и перенос остатка в следующий разряд как в обычном умножении в столбик
        for (int i = 0; i < BigInteger::BLOCKS_N; ++i) {
            copy.blocks[i] += term;
            term = static_cast<long long>(copy.blocks[i] / BigInteger::BASE);
            copy.blocks[i] %= BigInteger::BASE;
            if (copy.blocks[i] != 0) last_nnul = i;
        }

        // высчитывание длины нового числа
        copy.blocks_size = last_nnul + 1;
        copy.size = BigInteger::BIT * (copy.blocks_size - 1); // последний блок (начало) может быть неполным
        copy.size += BigInteger::getIntSize(copy.blocks[copy.blocks_size - 1]);
    }
    if (num1.sign == num.sign)
        copy.sign = 1;
    else copy.sign = 0;
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

// END OF BIGINTEGER

class Rational {
public:
    BigInteger p;
    BigInteger q;


    void toIrreducible() {
        if (BigInteger::isEqual(p, q, false)) {
            bool sign = p.getSign();
            p = 1;
            q = 1;
            p.setSign(sign);
            return;
        } else if (q == 1 || p == 0)
            return;
        BigInteger gcd = GCD(p, q);
        p /= gcd;
        q /= gcd;
    }

    std::string buildDecimal(std::string main, std::string mantissa, size_t precision) const {
        std::string number;
        if (main.empty()) number.insert(0, "0.");
        else number.insert(0, main + ".");
        if (p.getSign() == 0) number.insert(0, "-");
//        if (mantissa.empty()) {
//            number.insert()
        number += mantissa + std::string(precision - mantissa.size(), '0');
        return number;
    }

    static BigInteger GCD(BigInteger a, BigInteger b) {
        a.setSign(1);
        b.setSign(1);
        if (a == 0) return b;
        if (b == 0) return a;
        if (a == b) return a;
        if (a == 1 || b == 1) return 1;

        BigInteger k = 1;
        while (!a.isNull() && !b.isNull()) {
            while (a.isEven() && b.isEven()) {
                a /= 2;
                b /= 2;
                k *= 2;
            }
            while (a.isEven()) a /= 2;
            while (b.isEven()) b /= 2;
            if (a >= b) a -= b;
            else b -= a;
        }
        return b * k;
    }

    static BigInteger LCS(const BigInteger& a, const BigInteger& b, const BigInteger& gcd) {
        // least common multiple
        BigInteger lcs = (a * b) / gcd;
        lcs.setSign(1);
        return lcs;
    }

//    BigInteger div2(BigInteger&a) {
//    }

public:

    explicit operator double() {
//        std::cerr << "double" << '\n';
        std::string num = asDecimal(15);
        return std::stod(num);
    }

    Rational() {
        p = 1;
        q = 1;
    }
    Rational(const BigInteger& number) : Rational() {
//        std::cerr << number;
        p = number;
        q = 1;
    }
    Rational(int number) : Rational(BigInteger(number)) {}
    Rational(const Rational& number) {
        p = number.p;
        q = number.q;
    }

    Rational& operator=(const Rational &num) {
        std::cerr << "=" << '\n';
        p = num.p;
        q = num.q;
        return *this;
    }

    Rational operator-() const {
        std::cerr << "-a" << '\n';
        Rational copy(p);
        copy.q = q;
        copy.p.changeSign();
        return copy;
    }

    std::string toString() const {
//        std::cerr << "toString_const" << '\n';
        Rational copy = *this;
        copy.toIrreducible();
        std::string pstr = copy.p.toString();
        if (!copy.q.isOne() && !p.isNull())
            return pstr + "/" + copy.q.toString();
        return pstr;
    }

    std::string toString() {
//        std::cerr << "toString" << '\n';
        toIrreducible();
        std::string pstr = p.toString();
        if (!q.isOne() && !p.isNull())
            return pstr + "/" + q.toString();
        return pstr;
    }

    Rational& operator+=(const Rational &num) {
//        std::cerr << "+=" << toString() << ' ' << num.toString() << ' ';
        if (!num.p.isNull()) {
            if (q == num.q)
                p += num.p;
            else {
                BigInteger gcd = GCD(q, num.q);
                BigInteger lcs = LCS(q, num.q, gcd);
                p = p * (lcs / q) + num.p * (lcs / num.q);
                q = lcs;
            }
            toIrreducible();
        }
//        std::cerr << toString() << '\n';
        return *this;
    }

    Rational& operator-=(const Rational &num) {
        std::cerr << "-=" << '\n';
        *this += (-num);
        return *this;
    }

    Rational& operator*=(const Rational &num) {
//        std::string a = toString(), b = num.toString();
        if (num.p == -1) {
            p.changeSign();
        } else if (!num.p.isOne() && !num.p.isNull()) {
            p *= num.p;
            q *= num.q;
            if (q.getSign() == 0) {
                q.changeSign();
                p.changeSign();
            }
            toIrreducible();
        }
//        std::cerr << toString() << '\n';
        return *this;
    }

    Rational& operator/=(const Rational &num) {
//        std::cerr << "/= ";
//        std::string a = toString(), b = num.toString();
//        if (a.size() + b.size() > 140)
//            std::cerr << "XxX" << '\n';
//        else {
//            std::cerr << a << ' ' << b << '\n';
//        }
        if (num.p == -1) {
            p.changeSign();
        } else if (!num.p.isOne() && !num.p.isNull()) {
            p *= num.q;
            q *= num.p;
            if (q.getSign() == 0) {
                q.changeSign();
                p.changeSign();
            }
            toIrreducible();
        }
        return *this;
    }

    std::string asDecimal(size_t precision = 0) const {
//        std::cerr << "asDec" << '\n';
        if (p == 0)
            return buildDecimal("0", "0", precision);
        if (p == q)
            return buildDecimal("1", "0", precision);
        if (q == 1 || q == -1)
            return buildDecimal(p.toString(), "0", precision);
        precision += p.getSize();

        BigInteger dvd = p;
        BigInteger dvr = q;
        if (BigInteger::getSign(dvd) == 0) dvd.changeSign();
        if (BigInteger::getSign(dvr) == 0) dvr.changeSign();

        std::string dvd_s = dvd.toString();
        std::string dvt_s = dvr.toString();

        int index;
        BigInteger term;
        BigInteger current_quotient;
        std::string quotient;
        std::string mantissa;
        bool noint = !BigInteger::absGreater(p, q);
        if (noint) {
            index = static_cast<int>(dvd_s.size());
            term = dvd_s.substr(0, dvd_s.size());
        } else {
            index = static_cast<int>(dvt_s.size()) - 1;
            term = dvd_s.substr(0, index);
        }

        while (index < static_cast<int>(precision)) {
            if (term == 0 && noint) break;
            term *= 10;
            if (!noint)
                term += dvd_s[index] - 48;
            ++index;

            current_quotient = 1;
            while (term < dvr) {
                if (!quotient.empty())
                    quotient.push_back('0');
                if (index < dvd.getSize()) {
                    term *= 10;
                    term += dvd_s[index] - 48;
                    ++index;
                } else { // нецелочисленное деление
                    mantissa.push_back('0');
                    term *= 10;
                    ++index;
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
                if (noint) mantissa.append(current_quotient.toString());
                else quotient.append(current_quotient.toString());
            }

            if (index >= static_cast<int>(dvd_s.size())) noint = true;
        }
        if (mantissa.empty()) mantissa = "0";
        return buildDecimal(quotient, mantissa, precision - p.getSize());
    }

    friend bool operator>(const Rational& first, const Rational& second);
    friend bool operator<(const Rational& first, const Rational& second);
    friend bool operator==(const Rational& first, const Rational& second);
    friend bool operator!=(const Rational& first, const Rational& second);
    friend bool operator>=(const Rational& first, const Rational& second);
    friend bool operator<=(const Rational& first, const Rational& second);
    friend Rational operator/(const Rational &first, const Rational &second);
};

bool operator>(const Rational& first, const Rational& second) {
    if (BigInteger::getSign(first.p) == 0 && BigInteger::getSign(second.p) == 1)
        return false;
    else if ((BigInteger::getSign(first.p) == 1 && BigInteger::getSign(second.p) == 0))
        return true;
    if (first.q == second.q)
        return first.p > second.p;
    else
        return first.p * second.q > second.p * first.q;
}

bool operator<(const Rational& first, const Rational& second) {
    return second > first;
}

bool operator==(const Rational& first, const Rational& second) {
    std::cerr << "==" << '\n';
    if (first.p.getSign() != second.p.getSign())
        return false;
    else if (first.p.isNull() && second.p.isNull())
        return true;
    else if (first.q == 1 && second.q == 1)
        return first.p == second.p;
    else
        return first.p * second.q == second.p * first.q;
}

bool operator!=(const Rational& first, const Rational& second) {
    return !(first == second);
}

bool operator>=(const Rational& first, const Rational& second) {
    return !(first < second);
}
bool operator<=(const Rational& first, const Rational& second) {
    return !(first > second);
}

Rational operator-(const Rational &first, const Rational &second) {
    std::cerr << "-" << '\n';
    Rational copy = first;
    copy -= second;
    return copy;
}

Rational operator+(const Rational &first, const Rational &second) {
    std::cerr << "+" << '\n';
    Rational copy = first;
    copy += second;
    return copy;
}

Rational operator*(const Rational &first, const Rational &second) {
    std::cerr << "*" << '\n';
    Rational copy = first;
    copy *= second;
    return copy;
}

Rational operator/(const Rational &first, const Rational &second) {
    std::cerr << "/" << '\n';
    Rational copy = first;
    copy /= second;
    return copy;
}
//
#endif //BIGINT_BIGINTEGER_H
//int main() {
//    Rational a = BigInteger("-12345113134324324324234324324");
//    Rational b = BigInteger("-123451312312332425345355656563454223423423424");
//    std::cout << (a / b).toString();
//    return 0;
//}
