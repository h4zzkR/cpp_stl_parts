#include <iostream>
#include <list>
#include <chrono>
#include <iterator>

template <typename T>
class shared_ptr {
private:
    T* ptr = nullptr;
    size_t* uses = nullptr;

    void erase() {
        --*uses;
        if (!*uses) {
            delete ptr;
            delete uses;
        }
    }

public:

    shared_ptr() {
        ptr = new T();
        uses = new size_t(1);
    }

    explicit shared_ptr(T* ptr) {
        this->ptr = ptr;
        uses = new size_t(1);
    }

    shared_ptr(const shared_ptr &obj) : ptr(obj.ptr), uses(obj.uses) {
        if (obj.ptr != nullptr) ++uses;
    }

    shared_ptr& operator=(const shared_ptr &obj) {
        erase();
        ptr = obj.ptr;
        uses = obj.uses;
        if (obj.ptr != nullptr)
            ++*uses;
        return *this;
    }

    T *operator->() const {
        return this->ptr;
    }

    T& operator*() {
        return *ptr;
    }

    bool operator==(const shared_ptr& oth) const {
        return ptr == oth.ptr;
    }

    bool operator!=(const shared_ptr& oth) const {
        return ptr != oth.ptr;
    }

    ~shared_ptr() {
        erase();
    }
};

struct Block {
    uint8_t* block = nullptr;
    Block* prev = nullptr;

    Block() = default;

    explicit Block(Block* prev, size_t& blockSize, size_t bucketSize) {
        block = new uint8_t[bucketSize * blockSize];
        this->prev = prev;
    }

    ~Block() {
        delete[] block;
    }
};

template<size_t bucketSize>
class FixedAllocator {
    const size_t reallocFactor = 2;

    struct Bucket {// Бритва Оккама?.. 
        Bucket* prev;
        Bucket(Bucket* prev) : prev(prev) {}
        ~Bucket() {
            prev = nullptr;
        }
    };

    struct State {
        Block* top = nullptr;
        Bucket* chain = nullptr;

        void chain_step_back() {
            chain = chain->prev;
        }

        void top_update(size_t size) {
            top = new Block(top, size, bucketSize);
        }

        State() = default;

        ~State() {
            while (top != nullptr) {
                Block* prev = top->prev;
                delete top;
                top = prev;
            }
        }
    };

    size_t bucket_size = std::max(bucketSize, sizeof(Bucket));
    size_t curBlockSize = 1, last = 0, curBlockByteSize = bucket_size;

    shared_ptr<State> state;

    template<typename U>
    friend class FastAllocator;

    template<size_t U>
    friend class FixedAllocator;

public:

    FixedAllocator() = default;

    template<size_t U>
    FixedAllocator(const FixedAllocator<U>& other) noexcept {
        state->top = other.state->top;
    }

    FixedAllocator& operator=(const FixedAllocator& oth) {
        state->~State();
        bucket_size = oth.bucket_size;
        curBlockSize = oth.curBlockSize;
        curBlockByteSize = oth.curBlockByteSize;
        last = oth.last;
        state = oth.state;
        return *this;
    }

    bool operator==(const FixedAllocator& oth) const {
        return state == oth.state;
    }

    bool operator!=(const FixedAllocator& oth) const {
        return (oth == *this);
    }

    uint8_t* allocate() {
        if (state->chain != nullptr) {
            Bucket* bucket = state->chain;
            bucket->~Bucket();
            state->chain_step_back();
            return reinterpret_cast<uint8_t*>(bucket);
        }

//        if (last + 1 == curBlockSize) {
        if (last + bucket_size >= curBlockByteSize) {
            curBlockSize *= reallocFactor;
            curBlockByteSize *= reallocFactor;
            state->top_update(curBlockSize);
            last = 0;
        }
        last += bucket_size;
        return &(state->top->block[last]);
    }

    void deallocate(uint8_t* ptr) {
        Bucket* b = reinterpret_cast<Bucket*>(ptr);
        new(b) Bucket(state->chain);
        state->chain = b;
    }

};

template <typename T>
class FastAllocator {
public:
    using pointer = T*;
    using value_type = T;
    using reference = T&;
    using const_pointer = const T*;
    using const_reference = const T&;
    using difference_type = std::ptrdiff_t;
    using is_always_equal = std::true_type;
    using propagate_on_container_copy_assignment = std::false_type;

    template<typename U> struct rebind { typedef FastAllocator<U> other; };

    template<class U>
    friend class FastAllocator;

    static const size_t bucketSize = sizeof(T);
    FixedAllocator<bucketSize> fixedAlloc;

public:

    FastAllocator() = default;

    template<typename U>
    FastAllocator(const FastAllocator<U>& other) noexcept {
        fixedAlloc = other.fixedAlloc;
    }

    FastAllocator(const FastAllocator<T>& other) {// Тоже самое же?
        this->fixedAlloc = other.fixedAlloc;
    }

    pointer allocate(size_t n) {
        if (n == 1 && bucketSize ) {
            return reinterpret_cast<pointer>(fixedAlloc.allocate());
        } else {
            pointer ptr = (pointer)malloc(bucketSize * n);
            return ptr;
        }
    }

    void deallocate(pointer start, size_t n) {
        if (n == 1) {
            fixedAlloc.deallocate(reinterpret_cast<uint8_t *>(start));
        } else {
            free(start);
        }
    }

    bool operator==(const FastAllocator& oth) const {
        return (bucketSize == oth.bucketSize && oth.fixedAlloc == fixedAlloc);
    }

    bool operator!=(const FastAllocator& oth) const {
        return !(*this == oth);
    }
};

template<typename T, typename Allocator=std::allocator<T>>
class List {
    size_t _size = 0;

    struct Node {
        Node* prev;
        Node* next;
        T value;

        template<typename U>
        Node(Node* prev, Node* next, U&& value) : prev(prev), next(next), value(std::forward<U>(value)) {} // 2 одинаковых конструктора?

        template<typename U>
        Node(U&& value, Node* prev = nullptr, Node* next = nullptr) : prev(prev), next(next), value(std::forward<U>(value)) {}

        Node(T&& value, Node* prev = nullptr, Node* next = nullptr) : prev(prev), next(next), value(std::move(value)) {}

        template<typename... Args>
        Node(Node* prev, Node* next, Args&&... args) : prev(prev), next(next), value(std::forward<Args>(args)...) {}

        Node() {
            prev = nullptr;
            next = nullptr;
        }
    };

    using nodeAllocType     = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using alloc_traits      = std::allocator_traits<nodeAllocType>;

    nodeAllocType allocator;
    Allocator useless;

    Node* afterback = nullptr;
    Node* afterfront = nullptr;

    void loop_list() {
        afterback = alloc_traits::allocate(allocator, 1);
        afterfront = alloc_traits::allocate(allocator, 1);
        afterback->prev = afterfront;
        afterback->next = nullptr;
        afterfront->next = afterback;
        afterfront->prev = nullptr;
    }

    void list_copy(const List& other) {
        Node* tmp = other.afterfront->next;
        int cnt = 0;

        while (tmp->next) {
            ++cnt;
            push_back(tmp->value);
            tmp = tmp->next;
        }
    }

    void list_erase() {
        Node* tmp = afterfront;

        if (afterfront->next == afterback) {
            // Вырожденный случай: пустой лист
            alloc_traits::deallocate(allocator, afterfront, 1);
            alloc_traits::deallocate(allocator, afterback, 1);
        } else {
            afterfront = afterfront->next;
            while (afterfront->next != nullptr) {
                Node *next = afterfront->next;
                alloc_traits::destroy(allocator, afterfront);
                alloc_traits::deallocate(allocator, afterfront, 1);
                afterfront = next;
            }
            alloc_traits::deallocate(allocator, afterfront, 1); // now it is afterback
            alloc_traits::deallocate(allocator, tmp, 1);
        }

        afterback = nullptr;
        afterfront = nullptr;

        _size = 0;
    }

public:

    size_t size() const { return _size; }
    Allocator get_allocator() { return useless; }

    explicit List(const Allocator &alloc = Allocator()) : allocator(alloc) {
        useless = alloc_traits::select_on_container_copy_construction(alloc);// Он всегда должен копироваться, тут ведь не копирование
        loop_list();
    }

    List(size_t count, const T& value, const Allocator& alloc = Allocator()) : allocator(alloc) {
        loop_list();
        for (size_t i = 0; i < count; ++i)
            push_back(value);
    }

    List(size_t count, const Allocator& alloc = Allocator()) : allocator(alloc) {
        loop_list();

        Node* tmp = alloc_traits::allocate(allocator, 1);
        alloc_traits::construct(allocator, tmp);
        tmp->prev = afterfront;
        afterfront->next = tmp;

        for (size_t i = 1; i < count; ++i) {
            Node* node = alloc_traits::allocate(allocator, 1);
            alloc_traits::construct(allocator, node);
            tmp->next = node;
            node->prev = tmp;
            tmp = node;
        }

        _size = count;
        tmp->next = afterback;
        afterback->prev = tmp;
    }

    List(const List& oth) {
        useless = alloc_traits::select_on_container_copy_construction(oth.useless);
        allocator = alloc_traits::select_on_container_copy_construction(oth.allocator);
        loop_list();
        list_copy(oth);
    }

    List(List&& oth) noexcept {
        if constexpr (alloc_traits::propagate_on_container_move_assignment::value) {
            if (useless == oth.useless)
                useless = std::move(oth.useless);
            if (allocator == oth.allocator)
                allocator = std::move(oth.allocator);
        } else {
            useless = alloc_traits::select_on_container_copy_construction(oth.useless);
            allocator = alloc_traits::select_on_container_copy_construction(oth.allocator);
        }
        afterfront = std::move(oth.afterfront);
        afterback = std::move(oth.afterback);
        _size = oth._size;
        oth._size = 0;
        oth.loop_list();
    }

    List& operator=(const List& oth) {
        list_erase();

        if (alloc_traits::propagate_on_container_copy_assignment::value == true && allocator != oth.allocator) {
            if (allocator != oth.allocator)
                allocator = oth.allocator;
            if (useless != oth.useless)
                useless = oth.useless;
        }

        loop_list();
        list_copy(oth);

        return *this;
    }

    List& operator=(List&& oth) noexcept {
        list_erase();
        if (alloc_traits::propagate_on_container_move_assignment::value == true && allocator != oth.allocator) {
            if (allocator != oth.allocator)
                allocator = std::move(oth.allocator);
            if (useless != oth.useless)
                useless = std::move(oth.useless);
        }
        afterfront = std::move(oth.afterfront);
        afterback = std::move(oth.afterback);

        _size = oth._size;
        oth._size = 0;

        oth.loop_list();
        return *this;
    }

    ~List() { list_erase(); }

    template<typename U>
    void push_back(U&& value) {// Это скорее emplace, для push_back отдельно определяются копирование и move перегрузки
        Node* node = alloc_traits::allocate(allocator,1);
        alloc_traits::construct(allocator, node, afterback->prev, afterback, std::forward<U>(value));
        node->prev->next = node;
        node->next->prev = node;
        ++_size;
    }

    template<typename U>
    void push_front(U&& value) {
        Node* node = alloc_traits::allocate(allocator,1);
        alloc_traits::construct(allocator, node, afterfront, afterfront->next, std::forward<U>(value));
        node->prev->next = node;
        node->next->prev = node;
        ++_size;
    }

    void pop_back() {
        Node* node = afterback->prev;
        if (node != afterfront) {
            Node *step_back = node->prev;
            if (step_back)
                step_back->next = afterback;
            alloc_traits::destroy(allocator, node);
            alloc_traits::deallocate(allocator, node, 1);
            afterback->prev = step_back;
            --_size;
        }
    }

    void pop_front() {
        Node* node = afterfront->next;
        if (node != afterback) {
            Node* step_forward = node->next;
            if (step_forward)
                step_forward->prev = afterfront;
            alloc_traits::destroy(allocator, node);
            alloc_traits::deallocate(allocator, node, 1);
            afterfront->next = step_forward;
            --_size;
        }
    }

    /// START OF ITERATOR ///
    template<bool IsConst>
    struct common_iterator {
    public:
        using difference_type       = std::ptrdiff_t;
        using value_type            = T;
        using pointer               = typename std::conditional_t<IsConst, const T *, T *>;
        using reference             = typename std::conditional_t<IsConst, const T &, T &>;
        using iterator_category     = std::bidirectional_iterator_tag;

    private:
        friend List;
        Node* ptr;
        common_iterator(Node* ptr) : ptr(ptr) {}

    public:
        common_iterator() = default;
        common_iterator(const common_iterator &it) : ptr(it.ptr) {}
        operator common_iterator<true>() { return const_iterator(ptr); }
        common_iterator &operator=(const common_iterator &other) {
            ptr = other.ptr;
            return *this;
        }
        pointer operator->() { return &ptr->value; }
        reference operator*() { return ptr->value; }
        common_iterator &operator++() {
            ptr = ptr->next;
            return *this;
        }
        common_iterator &operator--() {
            ptr = ptr->prev;
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
        common_iterator &operator+=(size_t shift) {
            for (size_t i = 0; i < shift; ++i)
                ptr = ptr->next;
            return *this;
        }
        common_iterator &operator-=(size_t shift) {
            for (size_t i = 0; i < shift; ++i)
                ptr = ptr->prev;
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
        bool operator==(const common_iterator &it) const { return (ptr == it.ptr); }
        bool operator!=(const common_iterator &it) const { return !(*this == it); }
        bool operator<(const common_iterator &rhs) const { return ptr < rhs.ptr; }
        bool operator>(const common_iterator &rhs) const { return rhs < *this; }
        bool operator<=(const common_iterator &rhs) const { return !(*this > rhs); }
        bool operator>=(const common_iterator &rhs) const { return !(*this < rhs); }
    };

    using iterator                  = common_iterator<false>;
    using const_iterator            = common_iterator<true>;
    using const_reverse_iterator    = std::reverse_iterator<const_iterator>;
    using reverse_iterator          = std::reverse_iterator<iterator>;

    friend iterator;
    friend const_iterator;

    iterator begin() { return iterator(afterfront->next); }
    const_iterator begin() const { return cbegin(); }
    iterator end() { return iterator(afterback); }
    const_iterator end() const { return cend(); }
    const_iterator cbegin() const { return const_iterator(afterfront->next); }
    const_iterator cend() const { return const_iterator(afterback); }
    std::reverse_iterator<iterator> rbegin() {; return std::reverse_iterator<iterator>(afterback); }
    std::reverse_iterator<const_iterator> crbegin() const { return std::reverse_iterator<const_iterator>(afterback); }
    std::reverse_iterator<iterator> rend() { return std::reverse_iterator<iterator>(afterfront->next); }
    std::reverse_iterator<const_iterator> crend() const { return std::reverse_iterator<const_iterator>(afterfront->next); }
    std::reverse_iterator<const_iterator> rend() const { return crend(); }
    std::reverse_iterator<const_iterator> rbegin() const { return crbegin(); }
    /// END OF ITERATOR ///

    void insert(const_iterator it, const T &item) {
        Node* newnode = alloc_traits::allocate(allocator, 1);
        alloc_traits ::construct(allocator, newnode, it.ptr->prev, it.ptr, item);
        if (it.ptr->prev)
            it.ptr->prev->next = newnode;
        if (it.ptr)
            it.ptr->prev = newnode;
        ++_size;
    }

    template<typename... Args>
    iterator emplace(const_iterator pos, Args&&... args) {
        Node* newnode = alloc_traits::allocate(allocator, 1);
        std::allocator_traits<Allocator>::construct(useless, &newnode->value, std::forward<Args>(args)...);

        newnode->prev = pos.ptr->prev;
        newnode->next = pos.ptr;

        if (newnode->prev)
            newnode->prev->next = newnode;
        if (newnode->next)
            newnode->next->prev = newnode;
        ++_size;
        return iterator(newnode);
    }

    void erase(const_iterator it) {
        Node* node = it.ptr;
        node->prev->next = node->next;
        node->next->prev = node->prev;
        alloc_traits::destroy(allocator, node);
        alloc_traits::deallocate(allocator, node, 1);
        --_size;
    }
};
