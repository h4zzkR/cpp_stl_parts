#ifndef STRING_STRING_H
#define STRING_STRING_H

#include <iostream>
#include <cstring>

class String {
private:
    size_t size = 0;
    size_t capacity = 1;
    const int capacity_factor = 2; // множитель для размеров строк
    char* str = new char[capacity];

    void createString() {
        str = new char[capacity];
    }

    bool willExpand() const {
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

    void expandCapacity() { // for "long" push_back, get more memory
        capacity = (size == 0) ? 1 : size * capacity_factor;
    }

    void shrinkCapacity() { // for "long" pop_back, cut memory
        capacity = (size == 0) ? 1 : int(capacity / 2);
    }

    char* bufcpy() {
        char* buf;
        buf = new char[size];
        memcpy(buf, str, size);
        return buf;
    }

public:
    void push_back(const char c) { // fix end
        if (willExpand()) {
            char* buf = bufcpy();
//            char* temp = str;
            deleteString();
            expandCapacity();
            createString();
            memcpy(str, buf, size);

            delete [] buf; buf = nullptr;
        }
        str[size++] = c;
//        str[size] = '\0';
    }

    void pop_back() {
        if (willShrink()) {
            size--;
            char* buf = bufcpy();
            deleteString();
            shrinkCapacity();
            createString();
            memcpy(str, buf, size);

            delete [] buf; buf = nullptr;
        } else { size--; }
//        str[size] = '\0';
    }

    char& front();
    char& back();
    char front() const;
    char back() const;
    bool empty() const;
    void clear();
    size_t find(const String& substring) const;
    size_t rfind(const String& substring) const;
    String substr(int start, int count) const;
    size_t length() const;

    // Operators overloading

    friend bool operator==(const String& first, const String& second);
    friend std::ostream& operator<< (std::ostream &out, const String& str);
    friend std::istream& operator>> (std::istream &inp, const String& str);

    String& operator+=(const String& to_add) { // += strings
        int old_size = size;
        char* buf = bufcpy();
        size += to_add.size;

        deleteString();
        expandCapacity();
        createString();
        memcpy(str, buf, old_size * sizeof(char));
        memcpy(str + old_size, to_add.str, to_add.length());

        delete [] buf; buf = nullptr;
        return *this;
    }

    String& operator+=(const char to_add) { // += chars // delete for cast
        push_back(to_add);
        return *this;
    }

    String& operator=(const String& new_string) {
        if (!(new_string == *this)) {
            deleteString();
            capacity = new_string.capacity;
            size = new_string.size;
            createString();
            memcpy(str, new_string.str, new_string.size);
        }
        return *this; // разыменовали указатель this
    }


    char& operator[](const int i) {
        if (i < 0) // a[-1] returns last char
            return str[size - i*(-1)]; // some pythonic here
        else
            return str[i];
    }

    char operator[](const int i) const {
        if (i < 0) // a[-1] returns last char
            return str[size - i*(-1)]; // some pythonic here
        else
            return str[i];
    }

    String () {
        size = 0;
        str = new char[capacity];
    }; // конструктор по умолчанию.

    String (int n, char c);
    String (const char* cstring);
    String (const String &copy_str); // конструктор копирования
    String (char cchar);

    ~String() {
        deleteString();
    }
};

String operator+(const String& first, const String& second) {
    String copy = first; // RVO
    copy += second;
    return copy;
}

bool operator==(const String& first, const String& second) {
    if (first.size != second.size)
        return false;
    for (int i = 0; i < (int)first.size; i++) {
        if (first[i] != second[i])
            return false;
    }
    return true;
}

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
    expandCapacity();
    createString();
}

size_t String::find(const String& substring) const {
    int pointer = 0;
    int index = size;
    if (substring.size == 0)
        return 0;
    for (int i = 0; i < (int)size; i++) {
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
    for (int i = (int)(size - 1); i >= 0; i--) {
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
    return (index != (int)size) ? index : index;
}

String String::substr(int start, int count) const {
    String copy(count, '\0');
    memcpy(copy.str, str + start, count);
    return copy;
}

String::String(int n, char c) { // set constructor
    size = (int)n;
    expandCapacity();
    createString();
    memset(str, c, n);
}

String::String (const char* cstring) {  // C strings constructor
    size = strlen(cstring);
    expandCapacity();
    createString();
    memcpy(str, cstring, size);
//    str[size] = '0'; // remove \0
}

String::String (const String &copy_str) { // copy constructor
    size = copy_str.size;
    capacity = copy_str.capacity;
    createString();
    memcpy(str, copy_str.str, size);
}

String::String (const char cchar) : String(1, cchar) { } // char constructor

#endif //STRING_STRING_H
