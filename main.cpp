//#include <iostream>
//#include <functional>
//#include <vector>
//#include <type_traits>
//
//template <typename UN>
//class Function;
//
//template <typename R, typename... Args>
//class Function <R(Args...)> {
//public:
//    using result_type = R;
//
//private:
//    template<typename F>
//    struct Manager {
//        static void construct(void* place, F* object) {
//            new (place) F(*object);
//        }
//
//        static void rval_construct(void* place, F* object) {
//            new (place) F(std::forward<F>(*object));
//        }
//
//        static void deconstruct(void* place) {
//            std::free(place);
//        }
//
//        static void destroy(void* place) {
//            reinterpret_cast<F*>(place)->~F();
//        }
//
//        static void toggle(void* place = nullptr, F* ptr = nullptr, char OperationType = 0) {
//            if (OperationType == 1)
//                construct(place, ptr);
//            else if (OperationType == 2)
//                deconstruct(place);
//            else if (OperationType == 3)
//                destroy(place);
//            else if (OperationType == 4)
//                rval_construct(place, ptr);
//        }
//
//        static result_type call(F* func, Args&&... args) {
//            return (*func)(std::forward<Args>(args)...);
//        }
//    };
//
//    using call_t = result_type (*)(const void*, Args&&...);
//    using toggle_t = void (*)(void*, const void*, char);
//
//    struct Carry {
//        size_t allocSize = 0;
//        call_t call = nullptr;
//        toggle_t toggle = nullptr;
//
//        Carry() = default;
//        Carry(const Carry& other) = default;
//        Carry(Carry&& other) noexcept = default;
//
//        Carry& operator=(const Carry& other) {
//            allocSize = other.allocSize;
//            call = other.call;
//            toggle = other.toggle;
//            return *this;
//        }
//    };
//
//    std::aligned_storage_t<16> stack{};
//    void* allocPtr = nullptr;
//    Carry* carry;
//
//    void destroy() {
//        if (allocPtr)
//            carry->toggle(allocPtr, nullptr, 3);
//    }
//
//    void erase() {
//        if (allocPtr && carry->allocSize != 0)
//            carry->toggle(allocPtr, nullptr, 2);
//    }
//
//public:
//
//    Function() = default;
//
//    /* std-like implementation with copy elision in arg on rvalue */
//    /* and copy constructor calling in arg on lvalue              */
//    template<typename F>//, typename = std::enable_if_t<std::is_invocable_r_v<F, R(Args...), Args...>>> // TODO
//    Function(F functor) {
//        carry = new Carry();
//        carry->call = reinterpret_cast<call_t>(Manager<F>::call);
//        carry->toggle = reinterpret_cast<toggle_t>(Manager<F>::toggle);
//        if (sizeof(functor) <= sizeof(stack)) {
//            allocPtr = &stack;
//        } else {
//            carry->allocSize = sizeof(std::remove_reference_t<F>);
//            allocPtr = std::malloc(carry->allocSize);
//        }
//        new(allocPtr) F(std::forward<F>(functor));
//    }
//
//
//    Function(const Function& other) {
//        if (other.allocPtr != nullptr) {
//            carry = new Carry();
//            new(carry) Carry(other.carry);
//            if (other.carry->allocSize == 0) { // placed on stack
//                allocPtr = &stack;
//            } else { // placed in dynamic memory
//                allocPtr = std::malloc(carry->allocSize);
//            }
//            carry->toggle(allocPtr, other.allocPtr, 1);
//        }
//    }
//
//    Function(Function&& other) noexcept {
//        if (other.allocPtr) {
//            carry = other.carry;
//            other.carry = nullptr;
//            if (carry->allocSize == 0) {
//                allocPtr = &stack;
//                carry->toggle(allocPtr, other.allocPtr, 4);
//            } else
//                allocPtr = other.allocPtr;
//            other.allocPtr = nullptr;
//        } else
//            allocPtr = nullptr;
//    }
//
//    Function& operator=(const Function& other) {
//        destroy();
//        if (other.allocPtr) {
//            size_t allocSize = carry->allocSize;
//            if (other.carry->allocSize == 0) {
//                erase();
//                allocPtr = &stack;
//            } else if (allocSize and allocSize < other.carry->allocSize) {
//                erase();
//                allocPtr = std::malloc(carry->allocSize);
//            }
//            carry->~Carry();
//            new(carry) Carry(other.carry);
//            carry->toggle(allocPtr, other.allocPtr, 1);
//        } else {
//            erase();
//            allocPtr = nullptr;
//        }
//        return *this;
//    }
//
//    Function& operator=(Function&& other)  noexcept {
//        destroy();
//        if (other.allocPtr) {
//            erase();
//
//            carry->~Carry();
//            delete carry;
//            carry = other.carry;
//            other.carry = nullptr;
//
//            if (carry->allocSize == 0) {
//                allocPtr = &stack;
//                carry->toggle(allocPtr, other.allocPtr, 4);
//            } else
//                allocPtr = other.allocPtr;
//            other.allocPtr = nullptr;
//        } else {
//            erase();
//            allocPtr = nullptr;
//        }
//        return *this;
//    }
//
//    template <typename F, typename = std::enable_if_t<!std::is_lvalue_reference_v<F>>>
//    Function& operator=(F&& functor) {
//        Function<result_type(Args...)> new_function(std::move(functor));
//        *this = std::move(new_function);
//        return *this;
//    }
//
////    template <typename F>
////    Function& operator=(std::reference_wrapper<F> functor) noexcept {
////        std::cerr << "call\n";
////
////    }
//
//    result_type operator()(Args&&... args) {
//        if (allocPtr == nullptr)
//            throw std::bad_function_call();
//        return carry->call(allocPtr, std::forward<Args>(args)...);
//    }
//
//    ~Function() {
//        destroy();
//        erase();
//        delete carry;
//    }
//};
//
//struct S {
//    int x = 0;
//    std::vector<int> abba;
//    int operator()() const {
//        return x;
//    }
//
//    S(int y) {
//        abba.resize(y);
//        x = y;
//    }
//
//    S(S&& oth) {
//        x = oth.x;
//        std::cerr << "MOVE\n";
//    }
//
//    S(const S& oth) {
//        x = oth.x;
//        std::cerr << "COPY\n";
//    }
//
//    S& operator=(S&& oth) {
//        std::cerr << "move\n";
//        return *this;
//    }
//
//    S& operator=(const S& oth) {
//        std::cerr << "copy\n";
//        return *this;
//    }
//};
//
//    int add() {
//        return 1;
//    }
//
//    int add2() {
//        return 2;
//    }
//
//int main() {
////    S s(5);
////    Function<int()> g = add;
////    g = add2;
//////    g = []() { return 3; };
//////    g = s;
////    std::cout << g();
//    S s(5), t(6);
////    Function<int()> f([](){ return "string"; });
//    Function<int()> f(s);
//    f = std::move(s);
//    f = add;
//    f = t;
//    f = [](){return 5;};
//    std::cout << f();
//}
