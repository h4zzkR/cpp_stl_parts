#include <iostream>
#include <iterator>
#include <vector>
#include <cmath>

template<typename T, typename Allocator=std::allocator<T>>
class List {
    size_t _size = 0;

    struct Node {
        Node* prev;
        Node* next;
        T value;

        template<typename U>
        Node(Node* prev, Node* next, U&& value) : prev(prev), next(next), value(std::forward<U>(value)) {}

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
        useless = alloc_traits::select_on_container_copy_construction(alloc);
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
    void push_back(U&& value) {;
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



template<typename Key,
        typename Value,
        typename Hash=std::hash<Key>,
        typename Equal=std::equal_to<const Key>,
        typename Alloc=std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {
public:
    using NodeType             = std::pair<const Key, Value>;
    using hasher               = Hash;
    using key_equal            = Equal;
    using pointer              = typename std::allocator_traits<Alloc>::pointer;
    using const_pointer        = typename std::allocator_traits<Alloc>::const_pointer;
    using reference            = Value&;
    using const_reference      = const Value&;

    using Iterator             = typename List<NodeType, Alloc>::iterator;
    using ConstIterator        = typename List<NodeType, Alloc>::const_iterator;

    using iteratorAlloc        = typename std::allocator_traits<Alloc>::template rebind_alloc<Iterator>;
    using bucketAlloc          = typename std::allocator_traits<Alloc>::template rebind_alloc<List<Iterator, iteratorAlloc>>;

private:

    float rehash_factor = 1;
    size_t _size, capacity; // размер, количество бакетов

    List<NodeType, Alloc> iterate_list;
    std::vector<List<Iterator, iteratorAlloc>, bucketAlloc> buckets;

    hasher hash_fn;
    key_equal equal_fn;

    Alloc allocator;

    size_t hashify(const Key& key) const { return hash_fn(key) % capacity; }

    Iterator _find_in_bucket(const Key& key, size_t hash) noexcept {
        for (auto it = buckets[hash].begin(); it != buckets[hash].end(); ++it) {
            auto real_iter = *it;
            if (equal_fn(key, real_iter->first))
                return real_iter;
        }
        return iterate_list.end();
    }

    ConstIterator _find_in_bucket(const Key& key, size_t &hash) const noexcept {
        for (auto it : buckets[hash]) {
            auto real_iter = *it;
            if (equal_fn(key, real_iter->first))
                return real_iter;
        }
        return iterate_list.cend();
    }

    void _move_container(UnorderedMap&& oth) noexcept {
        _size = oth._size; capacity = oth.capacity;
        buckets = std::move(oth.buckets);
        iterate_list = std::move(oth.iterate_list);
        hash_fn = oth.hash_fn; equal_fn = oth.equal_fn;
        oth._size = 0; oth.capacity = 1;
        oth.buckets.resize(1);
        oth.buckets.shrink_to_fit();
    }

public:
    size_t size() const { return _size; }
    void max_load_factor(float load_factor) {
        rehash_factor = load_factor;
        rehash(bucket_count());
    }
    size_t bucket_count() const { return capacity; }
    Iterator end() { return iterate_list.end(); }
    Iterator begin() { return iterate_list.begin(); }
    ConstIterator cend() const { return iterate_list.cend(); }
    ConstIterator cbegin() const { return iterate_list.cbegin(); }
    float max_load_factor() const { return rehash_factor; }
    float load_factor() const { return size() / bucket_count(); }
    size_t max_size() const noexcept { return std::numeric_limits<size_t>::max(); }
    void reserve(size_t count) {
        size_t cap = std::ceil(max_load_factor() * bucket_count());
        if (count > cap)
            rehash(cap);
    }

    reference operator[](const Key& key) {
        auto found = insert(std::make_pair(key, Value()));
        return found.first->second;
    }

    reference at(const Key& key) {
        auto found = find(key);
        if (found == end())
            throw std::out_of_range("empty");
        return found->second;
    }
    const_reference at(const Key& key) const {
        auto found = find(key);
        if (found == cend())
            throw std::out_of_range("empty");
        return found->second;
    }

    Iterator find(const Key& key) noexcept {
        size_t hash = hashify(key);
        return _find_in_bucket(key, hash);
    }

    ConstIterator find(const Key& key) const noexcept {
        size_t hash = hashify(key);
        return _find_in_bucket(key, hash);
    }

    void rehash(size_t count) {
        if (count > bucket_count()) {
            capacity = count;
            std::vector<List<Iterator, iteratorAlloc>, bucketAlloc> new_buckets(capacity);
            size_t hash;
            for (auto it = begin(); it != end(); ++it) {
                hash = hashify((*it).first);
                new_buckets[hash].push_back(it);
            }
            buckets = std::move(new_buckets);
            buckets.shrink_to_fit();
            new_buckets.clear();
        }
    }

    explicit UnorderedMap(size_t bucket_size, const Hash& hash = Hash(), const key_equal& equal = key_equal(), const Alloc& alloc = Alloc()) :
            capacity(bucket_size), hash_fn(hash), equal_fn(equal) {
        buckets = decltype(buckets)(bucket_size, alloc);
        iterate_list = decltype(iterate_list)(alloc);
    }

    UnorderedMap() : _size(0), capacity(1) {
        buckets.resize(1);
        iterate_list = List<NodeType, Alloc>(allocator);
    }
    UnorderedMap(const UnorderedMap& oth) = default;
    UnorderedMap& operator=(const UnorderedMap& oth) = default;
    UnorderedMap(UnorderedMap&& oth) noexcept {
        _move_container(std::forward<UnorderedMap>(oth));
    }
    UnorderedMap& operator=(UnorderedMap&& oth) noexcept {
        _move_container(std::forward<UnorderedMap>(oth));
        return *this;
    }

    ~UnorderedMap() = default;
public:
    std::pair<Iterator, bool> insert(NodeType&& node) {
        if (size() > rehash_factor * bucket_count()) rehash(std::ceil(capacity * 2));
        size_t hash = hashify(node.first);
        auto found = _find_in_bucket(node.first, hash);
        bool inserted = (found == end());
        if (inserted) {
            iterate_list.push_front(std::forward<NodeType>(node));
            buckets[hash].push_back(iterate_list.begin());
            found = iterate_list.begin();
            ++_size;
        }
        return std::make_pair(found, inserted);
    }

    std::pair<Iterator, bool> insert(const NodeType& node) {
        if (size() > rehash_factor * bucket_count()) rehash(std::ceil(capacity * 2));
        size_t hash = hashify(node.first);
        auto found = _find_in_bucket(node.first, hash);
        bool inserted = (found == end());
        if (inserted) {
            iterate_list.push_front(node);
            buckets[hash].push_back(iterate_list.begin());
            found = iterate_list.begin();
            ++_size;
        }
        return std::make_pair(found, inserted);
    }

    template<typename P>
    std::pair<Iterator,bool> insert(P&& value) {
        return emplace(std::forward<P>(value));
    }

    void insert(std::initializer_list<NodeType> ilist) {
        for (auto &it : ilist) {
            insert(std::forward<NodeType>(it));
        }
    }

    template<typename InputIt>
    void insert(InputIt first, InputIt last) {
        while (first != last) {
            insert(std::forward<decltype(*first)>(*first));
            ++first;
        }
    }

    template<typename... Args>
    std::pair<Iterator,bool> emplace(Args&&... args) {
        if (size() > rehash_factor * capacity) rehash(std::ceil(capacity * 2));
        auto emplaced = iterate_list.emplace(iterate_list.begin(), std::forward<Args>(args)...);

        size_t hash = hashify(emplaced->first);
        auto found_in = _find_in_bucket(emplaced->first, hash);
        bool found = (found_in != iterate_list.end());

        if (!found) {
            buckets[hash].push_front(emplaced);
            ++_size;
        } else {
            iterate_list.erase(emplaced);
            emplaced = found_in;
        }
        return std::make_pair(emplaced, !found);
    }

    Iterator erase(ConstIterator pos) {
        size_t hash = hashify(pos->first);

        auto it = buckets[hash].begin();
        for (; it != buckets[hash].end();) {
            if (equal_fn(pos->first,(*it)->first)) {
                break;
            }
            ++it;
        }
        Iterator next = *it;
        std::advance(next, 1);
        buckets[hash].erase(it);
        iterate_list.erase(pos);
        --_size;
        return next;
    }

    Iterator erase(ConstIterator first, ConstIterator last) {
        auto it = iterate_list.begin();
        for (; first != last; ) {
            auto current = first;
            std::advance(first, 1);
            it = erase(current);
        }
        return it;
    }
};

