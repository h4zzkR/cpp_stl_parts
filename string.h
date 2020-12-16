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

    void createString() {// Разве это не участь конструктора?
        str = new char[capacity];
    }

    bool willExpand() const {// Такую проверку проще явно породить
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
            createString();// Зачем, у тебя ведь есть буфер, который ты можешь забрать, а так у тебя проводится 2 копирования
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
            createString();// Аналогично
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
    size_t length() const;
    
    size_t find(const String& substring) const;
    size_t rfind(const String& substring) const;
    String substr(int start, int count) const;

    // Operators overloading

    friend bool operator==(const String& first, const String& second);// А зачем, почти никто не пишет "abc" == string, всегда пишется наоборот string == "abc"
    friend std::ostream& operator<< (std::ostream &out, const String& str);
    friend std::istream& operator>> (std::istream &inp, const String& str);

    String& operator+=(const String& to_add) { // += strings
        int old_size = size;
        char* buf = bufcpy();
        size += to_add.size;

        deleteString();
        expandCapacity();// Почему бы не проверить на факт надобности расширяться?
        createString();
        memcpy(str, buf, old_size * sizeof(char));
        memcpy(str + old_size, to_add.str, to_add.length());

        delete [] buf; buf = nullptr;
        return *this;
    }
    //Здесь можно убрать const
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
        return *this; // разыменовали указатель this // А зачем этот комментарий?
    }

    // Аналогично, const не нужен, так как всё равно копирование
    char& operator[](const int i) {// long long для работы с памятью, в int максимум 4 Гб влезут. А по хорошему, если без питонячести, то нужно принимать size_t
        if (i < 0) // a[-1] returns last char
            return str[size - i*(-1)]; // some pythonic here
        else
            return str[i];
    }

    char operator[](const int i) const {// Аналогично
        if (i < 0) // a[-1] returns last char
            return str[size - i*(-1)]; // some pythonic here
        else
            return str[i];
    }

    String () {
        size = 0;
        str = new char[capacity];
    } // конструктор по умолчанию.

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
    expandCapacity();// Ну да, на уменьшение.... Г-логика
    createString();
}

size_t String::find(const String& substring) const {
    int pointer = 0;
    int index = size;
    if (substring.size == 0)
        return 0;
    for (int i = 0; i < (int)size; i++) {// long long
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
    for (int i = (int)(size - 1); i >= 0; i--) {// long long
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
    memcpy(copy.str, str + start, count);// Два memcpy...
    return copy;
}

String::String(int n, char c) { // set constructor
    size = (int)n;
    expandCapacity();
    createString();// Зачем, у тебя ведь по умолчанию уже создан какой то массив, который кстати в памяти затеряется
    memset(str, c, n);
}

String::String (const char* cstring) {  // C strings constructor
    size = strlen(cstring);
    expandCapacity();
    createString();// Аналогично
    memcpy(str, cstring, size);
//    str[size] = '0'; // remove \0
}

String::String (const String &copy_str) { // copy constructor
    size = copy_str.size;
    capacity = copy_str.capacity;
    createString();// Аналогично
    memcpy(str, copy_str.str, size);
}

String::String (const char cchar) : String(1, cchar) { } // char constructor

#endif //STRING_STRING_H
