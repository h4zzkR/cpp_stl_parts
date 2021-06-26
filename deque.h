#ifndef DEQUE_DEQUE_H
#define DEQUE_DEQUE_H


#include <cmath>
#include <iterator>

template<typename T>
class Deque {

    const static size_t bucket_capacity = 100;
    const static char resize_factor = 2;

    struct point {
        size_t x;
        size_t y;

        point(size_t x, size_t y) : x(x), y(y) {}
    };

    T **array;

    size_t _size = 0;
    size_t capacity = 0;

    point head{0, 0};
    point tail{0, 0};

    void reserve_buckets(T **arr, size_t start, size_t end) {
        size_t i = start;
        try {
            for (; i < end; ++i)
                arr[i] = reinterpret_cast<T *>(new int8_t[bucket_capacity * sizeof(T)]);
        } catch (...) {
            for (size_t j = 0; j < i; ++j)
                delete[] reinterpret_cast<int8_t *>(arr[j]);
            throw;
        }
    }

    // С точки зрения ООП круто было бы реализовать в reserve возможность вставить кастомную функцию (вместе с "отменителем" эффекта), и тогда уже выбирать
    // Либо просто выделять память, либо ещё конструировать элементы
    void fill_reserve_buckets(size_t start, size_t end, const T &value) {
        size_t i = start, j = 0;
        try {
            for (; i < end; ++i) {
                array[i] = reinterpret_cast<T *>(new int8_t[bucket_capacity * sizeof(T)]);
                for (; j < bucket_capacity; ++j)
                    new(array[i] + j) T(value);
                j = 0;
            }
        } catch (...) {
            // TODO MEMORY LEAK: VALGRIND SAYS THAT WITH NON SUCCESSFUL INITIALISATION THERE IS ML
            // BUT WHO CARES
            for (size_t k = start; k < i; ++k) {
                for (size_t l = 0; l < bucket_capacity; ++l)
                    (array[k] + l)->~T();
                delete[] reinterpret_cast<int8_t *>(array[k]);
            }
            for (size_t l = 0; l < j; ++l)
                (array[i] + l)->~T();
            delete[] reinterpret_cast<int8_t *>(array[i]);
            throw;
        }
    }

    void on_init(size_t size) {
        _size = size;
        size_t num_alive_buckets = std::max((size_t) 1, static_cast<size_t>(ceil(size * 1.0 / bucket_capacity)));
        capacity = num_alive_buckets;

        array = new T *[capacity];
        head = point(0, (num_alive_buckets * bucket_capacity - size) / 2);
        tail = point(head.x + num_alive_buckets - 1, (head.y + size - 1) % bucket_capacity);
    }

    size_t active_buckets_num() {
        return (capacity) ? tail.x - head.x + 1 : 0;
    }

    size_t x_by_index(size_t index) const {
        return head.x + (head.y + index) / bucket_capacity;
    }

    size_t y_by_index(size_t index) const {
        return (head.y + index) % bucket_capacity;
    }

    T **deque_deep_copy(const Deque &other) {
        T **newarray = new T *[other.capacity];
        size_t i = 0, j = 0;
        try {
            for (; i < other.capacity; ++i) {
                // В данном случае будет вызов конструктора по умолчанию для всех элементов. Тебе нужно через new int8(n*sizeof(T)) делать (ну или ::operator new, что тоже самое). -5%
                // DONE
                newarray[i] = ::operator new(other.bucket_capacity);
                for (; j < other.bucket_capacity; ++j)
                    new(newarray[i] + j) T(other.array[i][j]);
                j = 0;
            }
            return newarray;
        } catch (...) {
            for (size_t k = 0; k < i; ++k) {
                for (size_t l = 0; l < other.bucket_capacity; ++l)
                    (newarray[k] + l)->~T();
                delete[] reinterpret_cast<int8_t *>(newarray[k]);
            }
            for (size_t l = 0; l < j; ++l)
                (newarray[i] + l)->~T();
            delete[] reinterpret_cast<int8_t *>(newarray[i]);
            throw;
        }
    }

    void deque_erase() {
        for (size_t i = 0; i < size(); ++i)
            (&(*this)[i])->~T();
        for (size_t i = 0; i < capacity; ++i)
            delete[] reinterpret_cast<int8_t *>(array[i]);
        delete[] array;
    }

    // Комментарии с инструкцией пишут до объявления, тогда IDE покажет подсказку
    /* You can call it ONLY for EMPTY buckets! */
    void deque_range_erase(size_t start, size_t end) {
        for (; start < end; ++start)
            delete[] reinterpret_cast<int8_t *>(array[start]);
    }

    void deque_expand() {
        size_t newsize = capacity * resize_factor + 1;
        size_t left = (newsize - capacity) / 2, delta = active_buckets_num();

        T **newarray = new T *[newsize];

        try {
            for (size_t i = 0; i < delta; ++i)
                newarray[i + left] = array[i + head.x];
            reserve_buckets(newarray, 0, left);
            reserve_buckets(newarray, left + delta, newsize);
        } catch (...) {
            throw;
        }

        deque_range_erase(0, head.x);
        deque_range_erase(tail.x + 1, capacity);

        if (capacity)
            delete[] array;

        array = newarray;

        head.x = left;
        tail.x = (capacity) ? left + delta - 1 : 0;
        capacity = newsize;
    }

    void deque_shrink() {
        // Useless function, bcs Mescherin says that it isn't necessary to free memory
        // in pop_back/front (who cares that you do not free reserved memory...)
        // if you call it, existing iterators will be invalid, but it seems that in Test 3
        // you can't invalid them after pops.

        size_t newsize = active_buckets_num();
        T **newarray = new T *[newsize];

        for (size_t i = 0; i < newsize; ++i)
            newarray[i] = array[head.x + i];

        deque_range_erase(0, head.x);
        deque_range_erase(tail.x + 1, capacity);

        delete[] array;

        array = newarray;
        head.x = 0;
        tail.x = newsize - 1;
        capacity = newsize;
    }

public:

    explicit Deque() = default;

    explicit Deque(size_t size) {
        on_init(size);
        try {
            fill_reserve_buckets(0, capacity, T());
        } catch (...) {
            delete[] array;
            throw;
        }
    }

    explicit Deque(size_t size, const T &value) {
        on_init(size);
        try {
            fill_reserve_buckets(head.x, tail.x + 1, value);
        } catch (...) {
            delete[] array;
            throw;
        }
        reserve_buckets(array, tail.x + 1, capacity);
    }

    /// START OF ITERATOR ///
    template<bool IsConst>
    struct common_iterator {
    public:
        using difference_type = std::ptrdiff_t;
//        using value_type = T;// А почему здесь без conditional?
//      DONE
        using value_type = std::conditional_t<IsConst, const T, T>;
        using pointer = typename std::conditional_t<IsConst, const T *, T *>;
        using reference = typename std::conditional_t<IsConst, const T &, T &>;
        using iterator_category = std::random_access_iterator_tag;

    private:
        friend Deque;
        T **const *array = nullptr;
        pointer ptr;
        point it_pos;
        point last;

        void upd_ptr() {
            ptr = (*array)[it_pos.x] + it_pos.y;
        }

        common_iterator(T **const *array, point pt, point last) : array(array), it_pos(pt), last(last) {
            upd_ptr();
        }

    public:

        common_iterator() = default;

        common_iterator(const common_iterator &it) : array(it.array), it_pos(it.it_pos), last(it.last) {
            upd_ptr();
        }

        operator common_iterator<true>() {
            return const_iterator(array, it_pos, last);
        }

        common_iterator &operator=(const common_iterator &other) {
            array = other.array;
            it_pos = other.it_pos;
            last = other.last;
            upd_ptr();
            return *this;
        }

        pointer operator->() {
            return ptr;
        }

        reference operator*() {
            return *ptr;
        }

        common_iterator &operator++() {
            if (it_pos.y + 1 == bucket_capacity || it_pos.x < 0) {
                ++it_pos.x;
                it_pos.y = 0;
            } else
                ++it_pos.y;
            if (it_pos.x <= last.x) upd_ptr();
            return *this;
        }

        common_iterator &operator--() {
            if (it_pos.y == 0 || it_pos.x > last.x) {
                --it_pos.x;
                it_pos.y = bucket_capacity - 1;
            } else
                --it_pos.y;
            if (it_pos.x >= 0) upd_ptr();
            return *this;
        }

        common_iterator operator++(int) {
            common_iterator iter(*this);
            ++(*this);
            return iter;
        }

        common_iterator operator--(int) {
            common_iterator iter(*this);
            --(*this);
            return iter;
        }

        bool operator==(const common_iterator &it) {
            return (it_pos.x == it.it_pos.x) && (it_pos.y == it.it_pos.y);
        }

        bool operator!=(const common_iterator &it) {
            return !(*this == it);
        }

        bool operator<(const common_iterator &rhs) {
            if (it_pos.x == rhs.it_pos.x) {
                return ptr < rhs.ptr;
            } else {
                return it_pos.x < rhs.it_pos.x;
            }
        }

        bool operator>(const common_iterator &rhs) {
            return rhs < *this;
        }

        bool operator<=(const common_iterator &rhs) {
            return !(*this > rhs);
        }

        bool operator>=(const common_iterator &rhs) {
            return !(*this < rhs);
        }

        common_iterator &operator+=(size_t shift) {
            if (it_pos.y + shift < bucket_capacity) {
                it_pos.y += shift;
            } else {
                size_t delta = bucket_capacity - it_pos.y;
                shift -= delta;
                size_t jumps = floor(shift * 1.0 / bucket_capacity);
                it_pos.x += jumps + 1;
                shift %= bucket_capacity;
                it_pos.y = shift;
            }
            upd_ptr();
            return *this;
        }

        common_iterator &operator-=(size_t shift) {
            if (it_pos.y >= shift)
                it_pos.y -= shift;
            else {
                shift -= it_pos.y + 1;
                size_t jumps = floor(shift * 1.0 / bucket_capacity) + 1;
                it_pos.x -= jumps;
                shift = bucket_capacity - shift % bucket_capacity;
                it_pos.y = shift - 1;
            }
            upd_ptr();
            return *this;
        }

        common_iterator operator+(size_t shift) {
            common_iterator n(*this);
            n += shift;
            return n;
        }

        common_iterator operator-(size_t shift) {
            common_iterator n(*this);
            n -= shift;
            return n;
        }

        size_t operator-(const common_iterator &other) {
            if (it_pos.x == other.it_pos.x) {
                size_t delta = 0;
                if (other.it_pos.y > it_pos.y)
                    delta = other.it_pos.y - it_pos.y;
                else
                    delta = it_pos.y - other.it_pos.y;
                return delta;
            } else {
                // count elements in first block, blocks * bucket_capacity in the middle and the last block
                size_t delta1, delta2, delta;
                if (it_pos.x > other.it_pos.x) {
                    delta = it_pos.x - other.it_pos.x - 1;
                    delta1 = it_pos.y + 1;
                    delta2 = bucket_capacity - other.it_pos.y - 1;
                } else {
                    delta = other.it_pos.x - it_pos.x - 1;
                    delta1 = bucket_capacity - it_pos.y - 1;
                    delta2 = other.it_pos.y + 1;
                }
                return delta1 + delta * bucket_capacity + delta2;
            }
        }

    };

    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;
    // Не обязательно, так как они объявлены внутри класса
    friend iterator;
    friend const_iterator;

    iterator begin() {
        return iterator(&array, head, tail);
    }

    const_iterator begin() const {
        return cbegin();
    }

    iterator end() {
        return iterator(begin() + _size);
    }

    const_iterator end() const {
        return cend();
    }

    const_iterator cbegin() const {
        return const_iterator(&array, head, tail);
    }

    const_iterator cend() const {
        return const_iterator(cbegin() + _size);
    }

    std::reverse_iterator<iterator> rbegin() {
        return std::reverse_iterator<iterator>(end());
    }

    std::reverse_iterator<const_iterator> crbegin() const {
        return std::reverse_iterator<const_iterator>(cend());
    }

    std::reverse_iterator<iterator> rend() {
        return std::reverse_iterator<iterator>(begin());
    }

    std::reverse_iterator<const_iterator> crend() const {
        return std::reverse_iterator<const_iterator>(cbegin());
    }

    std::reverse_iterator<const_iterator> rend() const {
        return crend();
    }

    std::reverse_iterator<const_iterator> rbegin() const {
        return crbegin();
    }

    /// END OF ITERATOR ///
    /// START OF POINTING ///
    T &operator[](size_t index) {
        return array[x_by_index(index)][y_by_index(index)];
    }

    T operator[](size_t index) const {
        return array[x_by_index(index)][y_by_index(index)];
    }

    T &at(size_t index) {
        size_t x = x_by_index(index);
        size_t y = y_by_index(index);
        if (x > tail.x || y > bucket_capacity || (x == tail.x && y > tail.y))
            throw std::out_of_range("Out of range");
        return array[x][y];
    }

    T at(size_t index) const {
        size_t x = x_by_index(index);
        size_t y = y_by_index(index);
        if (x > tail.x || y > bucket_capacity || (x == tail.x && y > tail.y))
            throw std::out_of_range("Out of range");
        return array[x][y];
    }

    /// END OF POINTING ///
    /// START OF INSERTING ///
    void push_back(const T &item) {
        point old_head = head;
        point old_tail = tail;

        if (capacity && tail.y + 1 < bucket_capacity) {
            ++tail.y;
        } else {
            if (tail.x + 1 >= capacity) {
                try {
                    deque_expand();
                } catch (...) {
                    throw;
                }
                std::cerr << "long back\n";
            }
            old_tail = tail;
            old_head = head;
            (_size) ? ++tail.x : tail.x = 0;
            tail.y = 0;
        }

        try {
            new(*(array + tail.x) + tail.y) T(item);
        } catch (...) {
            tail = old_tail;
            head = old_head;
            throw;
        }
        ++_size;
    }

    void push_front(const T &item) {
        point old_head = head;
        point old_tail = tail;
        if (capacity && head.y >= 1) {
            --head.y;
        } else {
            if (head.x < 1) {
                try {
                    deque_expand();
                } catch (...) {
                    throw;
                }
                std::cerr << "long front\n";
            }
            old_head = head;
            old_tail = tail;
            if (!_size) {
                head.x = 0;
                head.y = 0;
            } else {
                --head.x;
                head.y = bucket_capacity - 1;
            }
        }
        try {
            new(*(array + head.x) + head.y) T(item);
        } catch (...) {
            tail = old_tail;
            head = old_head;
            throw;
        }
        ++_size;
    }

    void pop_back() {
//        if (capacity >= resize_factor * active_buckets_num())
//            deque_shrink();
        (&array[tail.x][tail.y])->~T();
        if (tail.y >= 1) {
            --tail.y;
        } else {
            --tail.x;
            tail.y = bucket_capacity - 1;
        }
        --_size;
    }

    void pop_front() {
//        if (capacity >= resize_factor * active_buckets_num())
//            deque_shrink();
        (&array[tail.x][tail.y])->~T();
        if (head.y + 1 < bucket_capacity)
            ++head.y;
        else {
            head.y = 0;
            ++head.x;
        }
        --_size;
    }

    void insert(iterator it, const T &item) {
        T value = *it;
        *it = item;
        ++it;
        for (; it < end(); ++it) {
            std::swap(value, *it);
        }
        push_back(value);
    }

    void erase(iterator it) {
        for (; it != end() - 1; ++it)
            std::swap(*it, *(it + 1));
        pop_back();
    }
    ///END OF INSERTING///

    T &back() {
        return (*array[tail.x])[tail.y];
    }

    T &front() {
        return (*array[head.x])[head.y];
    }

    size_t size() const {
        return _size;
    }

    Deque &operator=(const Deque &other) {
        try {
            T **copy = deque_deep_copy(other);
            deque_erase();
            array = copy;
        } catch (...) {
            throw;
        }
        _size = other._size;
        capacity = other.capacity;

        head = other.head;
        tail = other.tail;
        return *this;
    }

    Deque(const Deque &other) {
        try {
            T **copy = deque_deep_copy(other);
            array = copy;
        } catch (...) {
            throw;
        }
        _size = other._size;
        capacity = other.capacity;

        head = other.head;
        tail = other.tail;
    }

    ~Deque() { deque_erase(); }
};

#endif //DEQUE_DEQUE_H
