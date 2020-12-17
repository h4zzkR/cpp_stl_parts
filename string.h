#ifndef STRING_STRING_H
#define STRING_STRING_H

#include <iostream>
#include <cstring>
#include <algorithm>

class String {
private:
    size_t size = 0;
    size_t capacity = 1;
    const int capacity_factor = 2; // множитель для размеров строк
    char* str;

    bool willExpand() const {// Такую проверку проще явно породить // как?
        return size == capacity;
    }

    bool willShrink() const {
        if ((int)size < int(capacity / capacity_factor)) {
            return true;
        } else {
            return false;
        }
    }

    void deleteString() { // TODO
        delete[] str;
        str = nullptr;
    }

    // Тебе бы хватило только этой функции, назвал бы resizeCapacity и применял и когда нужно уменьшить
    void resizeCapacity(bool mode) {
        if (mode)
            capacity = (size == 0) ? 1 : size * capacity_factor;
        else
            capacity = (size == 0) ? 1 : static_cast<int>(capacity / 2);
    }

    char* bufcpy() {
        char* buf;
        buf = new char[capacity];
        std::copy(str, str + size, buf);
        return buf;
    } // aa

public:
    void push_back(const char c) { // TODO
        if (willExpand()) {
            resizeCapacity(1);
            char* buf = bufcpy();
            delete [] str;
            str = buf;
        }
        str[size++] = c;
    }

    void pop_back() { // TODO
        if (willShrink()) {
            size--;
            resizeCapacity(0);
            char* buf = bufcpy();
            delete [] str;
            str = buf;
        } else { size--; }
    }

    char& front();
    char& back();
    char front() const;
    char back() const;
    bool empty() const;

    void clear();
    size_t length() const;

    size_t find(const String& substring) const;
    size_t rfind(const String& substring) const;
    String substr(int start, int count) const;

    // Operators overloading

    bool operator==(const String& second) const {
        if (size != second.size)
            return false;
        for (int i = 0; i < static_cast<int>(size); i++) {
            if (str[i] != second[i])
                return false;
        }
        return true;
    }

    friend std::ostream& operator<< (std::ostream &out, const String& str);
    friend std::istream& operator>> (std::istream &inp, const String& str);

    String& operator+=(const String& to_add) { // += strings
        int old_size = size;
        char* buf = bufcpy();
        size += to_add.size;

        deleteString();
        if (willExpand())
            resizeCapacity(1);// Почему бы не проверить на факт надобности расширяться?
        str = new char[capacity];
        memcpy(str, buf, old_size * sizeof(char));
        memcpy(str + old_size, to_add.str, to_add.length());
        delete [] buf; buf = nullptr;
        return *this;
    }

    String& operator+=(char to_add) { // += chars // delete for cast
        push_back(to_add);
        return *this;
    }

    String& operator=(const String& new_string) {
        if (!(new_string == *this)) {
            deleteString();
            capacity = new_string.capacity;
            size = new_string.size;
            str = new char[capacity];
            memcpy(str, new_string.str, new_string.size);
        }
        return *this;
    }

    // Аналогично, const не нужен, так как всё равно копирование
    char& operator[](size_t i) {// long long для работы с памятью, в int максимум 4 Гб влезут. А по хорошему, если без питонячести, то нужно принимать size_t
        return str[i];
    }

    char operator[](size_t i) const {// Аналогично
        return str[i];
    }

    String () {
        size = 0;
        str = new char[capacity];
    }

    String(int n, char c) { // set constructor
        size = (size_t)n;
        resizeCapacity(1);
        str = new char[capacity];
//        createString();// Зачем, у тебя ведь по умолчанию уже создан какой то массив, который кстати в памяти затеряется
        memset(str, c, n);
    }

    String (const char* cstring) {  // C strings constructor
        size = strlen(cstring);
        resizeCapacity(1);
        str = new char[capacity];
        memcpy(str, cstring, size);
//    str[size] = '0'; // remove \0
    }

    String (const String &copy_str) { // copy constructor
        size = copy_str.size;
        capacity = copy_str.capacity;
        str = new char[capacity];
        memcpy(str, copy_str.str, size);
    }

    String (const char cchar) : String(1, cchar) { } // char constructor

    ~String() {
        deleteString();
    }
};

String operator+(const String& first, const String& second) {
    String copy = first; // RVO
    copy += second;
    return copy;
}

//bool operator==(const String& first, const String& second) {
//    if (first.size != second.size)
//        return false;
//    for (int i = 0; i < (int)first.size; i++) {
//        if (first[i] != second[i])
//            return false;
//    }
//    return true;
//}

std::ostream& operator<< (std::ostream &out, const String& str) {
    for (int i = 0; i < (int)str.size; i++) {
        out << str[i];
    }
    return out;
}

std::istream& operator>>(std::istream& is, String& s) {
    s.clear();
    char buf = '0';
    while (is.get(buf)) {
        if (buf == '\0' || buf == ' ' || buf == '\n' || buf == '\t' || buf == '\r')
            break;
        s.push_back(buf);
    }
    return is;
}

char& String::front() {
    return str[0];
}

char& String::back() {
    return str[size-1];
}

char String::front() const {
    return str[0];
}

char String::back() const {
    return str[size-1];
}

bool String::empty() const {
    return (size == 0);
}

size_t String::length() const {
    return size;
}

void String::clear() {
    deleteString();
    size = 0;
    resizeCapacity(0); // Ну да, на уменьшение.... Г-логика
    str = new char[capacity];
//    createString();
}

size_t String::find(const String& substring) const {
    int pointer = 0;
    int index = size;
    if (substring.size == 0)
        return 0;
    for (size_t i = 0; i < size; i++) { // long long
        if (pointer == (int)substring.size)
            break;
        if (str[i] == substring[pointer]) {
            pointer += 1;
            index = i;
        } else {
            pointer = 0;
            index = size;
        }
    }
    return (index != (int)size) ? index - (int)substring.size + 1 : index;
}

size_t String::rfind(const String& substring) const {
    int var = (int)substring.size - 1;
    int pointer = var;
    int index = size;
    if (substring.size == 0)
        return 0;
    for(size_t i = size - 1; i != 0; --i) { // long long
        if (pointer < 0)
            break;
        if (str[i] == substring[pointer]) {
            pointer -= 1;
            index = i;
        } else {
            pointer = var;
            index = size;
        }
    }
    return index;
//    return (index != (int)size) ? index : index;
}

String String::substr(int start, int count) const { // TODO
    String copy;
    for (size_t i = 0; i < static_cast<size_t>(count); ++i) {
        copy.push_back(str[start + i]);
    }
//    std::copy(str + start, str + count, copy.str);
//    String copy(count, '\0');
//    memcpy(copy.str, str + start, count);// Два memcpy...
    return copy;
}

#endif //STRING_STRING_H
