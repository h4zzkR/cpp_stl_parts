#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>

//#pragma GCC target ("avx2")
//#pragma GCC optimization ("O3")
//#pragma GCC optimization ("unroll-loops")
//#define IOS std::ios_base::sync_with_stdio(false), std::cin.tie(0), std::cout.tie(0);

typedef std::list<std::pair<std::string, std::string>> bucket;
bool DEBUG = false;

class HashMap {
    std::vector<bucket> buckets;

    int capacity; // количество бакетов
    float load_factor = 0.95;
    int num_var = 0; // количество переменных

    int p = 1'000'000'017, a, b;

    void updateHashFn() {
        a = rand() % p;
        b = rand() % p;
    }

    int getHash(const std::string &s) const {
        int hash = 0;
        for (char c : s)
            hash = 37 * hash + c;

        hash = ((a * hash + b) % p) % capacity;
        if (hash < 0)
            hash *= -1;
        return hash;
    }

    void rehash() {
        int old_capacity = capacity;
        capacity *= 2;
        updateHashFn();
        int hash;
        std::vector<bucket> new_buckets(capacity);
        for (int i = 0; i < old_capacity; ++i) {
            for (auto it = buckets[i].begin(); it != buckets[i].end(); ++it) {
                hash = getHash(it->first);
                new_buckets[hash].push_front(*it);
            }
        }
        buckets = new_buckets;
    }

    std::pair<std::string, std::string>* put_new(std::string key, std::string value) {
        int hash = getHash(key);
        buckets[hash].push_front(std::make_pair(key, value));
        ++num_var;
        return &buckets[hash].front();
    }

    std::pair<std::string,std::string>* find(std::string& key) {
        int hash = getHash(key);
        for (auto it = buckets[hash].begin(); it != buckets[hash].end(); ++it)
            if (it->first == key) return &(*it);
        return nullptr;
    }

    std::pair<std::string,std::string>* find(int hash, std::string& key) {
        for (auto it = buckets[hash].begin(); it != buckets[hash].end(); ++it)
            if (it->first == key) return &(*it);
        return nullptr;
    }

public:
    HashMap(int capacity=10) {
        this->capacity = capacity;
        buckets.resize(capacity);
        updateHashFn();
    }

    void put(std::string& key, std::string value) {
        if (num_var > load_factor * capacity) rehash();

        std::pair<std::string,std::string> *key_ptr = find(key);
        if (key_ptr == nullptr) {
            put_new(key, value);
            return;
        }
        key_ptr->second = value;
    }

    std::string get(std::string& key) {
        std::pair<std::string,std::string> *key_ptr = find(key);
        if (key_ptr != nullptr)
            return key_ptr->second;
        else
            return "none";
    }

    void remove(std::string& key) {
        int hash = getHash(key);
        std::pair<std::string,std::string> *key_ptr = find(hash, key);
        if (key_ptr != nullptr)
            buckets[hash].remove(*key_ptr);
    }
};

int main() {
//    IOS;

    HashMap map;
    std::string key, value, command;

    if (DEBUG) {
        while (std::cin >> command) {
            if (command == "put") {
                std::cin >> key >> value;
                map.put(key, value);
            } else if (command == "get") {
                std::cin >> key;
                std::cout << map.get(key) << '\n';
            } else if (command == "delete") {
                std::cin >> key;
                map.remove(key);
            }
        }
    } else {
        std::ifstream fin("map.in");
        std::ofstream fout("map.out");
        while (fin >> command) {
            if (command == "put") {
                fin >> key >> value;
                map.put(key, value);
            } else if (command == "get") {
                fin >> key;
                std::cerr << map.get(key) << '\n';
                fout << map.get(key) << '\n';
            } else if (command == "delete") {
                fin >> key;
                map.remove(key);
            }
        }
    }
    return 0;
}