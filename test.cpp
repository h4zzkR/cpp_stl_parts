#include <iostream>
#include <functional>
#include <cassert>
//#include "function.h"


// CODE
template <typename UN>
class Function;

template <typename R, typename... Args>
class Function <R(Args...)> {
public:
    using result_type = R;

private:
    template<typename F>
    struct Manager {
        static void construct(void* place, F* object) {
            new (place) F(*object);
        }

        static void rval_construct(void* place, F* object) {
            new (place) F(std::forward<F>(*object));
        }

        static void deconstruct(void* place) {
            std::free(place);
        }

        static void destroy(void* place) {
            reinterpret_cast<F*>(place)->~F();
        }

        static void toggle(void* place = nullptr, F* ptr = nullptr, char OperationType = 0) {
            if (OperationType == 1)
                construct(place, ptr);
            else if (OperationType == 2)
                deconstruct(place);
            else if (OperationType == 3)
                destroy(place);
            else if (OperationType == 4)
                rval_construct(place, ptr);
        }

        static result_type call(F* func, Args&&... args) {
            return (*func)(std::forward<Args>(args)...);
        }
    };

    using call_t = result_type (*)(const void*, Args&&...);
    using toggle_t = void (*)(void*, const void*, char);

    struct Carry {
        size_t allocSize = 0;
        call_t call = nullptr;
        toggle_t toggle = nullptr;

        Carry() = default;
        Carry(const Carry& other) = default;
        Carry(Carry&& other) noexcept = default;

        Carry& operator=(const Carry& other) {
            allocSize = other.allocSize;
            call = other.call;
            toggle = other.toggle;
            return *this;
        }
    };

    std::aligned_storage_t<16> stack{};
    void* allocPtr = nullptr;
    Carry* carry;

    void destroy() {
        if (allocPtr)
            carry->toggle(allocPtr, nullptr, 3);
    }

    void erase() {
        if (allocPtr && carry->allocSize != 0)
            carry->toggle(allocPtr, nullptr, 2);
    }

public:

    Function() = default;

    /* std-like implementation with copy elision in arg on rvalue */
    /* and copy constructor calling in arg on lvalue              */
    template<typename F>//, typename = std::enable_if_t<std::is_invocable_r_v<F, R(Args...), Args...>>> // TODO
    Function(F functor) {
        carry = new Carry();
        carry->call = reinterpret_cast<call_t>(Manager<F>::call);
        carry->toggle = reinterpret_cast<toggle_t>(Manager<F>::toggle);
        if (sizeof(functor) <= sizeof(stack)) {
            allocPtr = &stack;
        } else {
            carry->allocSize = sizeof(std::remove_reference_t<F>);
            allocPtr = std::malloc(carry->allocSize);
        }
        new(allocPtr) F(std::forward<F>(functor));
    }


    Function(const Function& other) {
        if (other.allocPtr != nullptr) {
            carry = new Carry();
            new(carry) Carry(*other.carry);
            if (other.carry->allocSize == 0) { // placed on stack
                allocPtr = &stack;
            } else { // placed in dynamic memory
                allocPtr = std::malloc(carry->allocSize);
            }
            carry->toggle(allocPtr, other.allocPtr, 1);
        }
    }

    Function(Function&& other) noexcept {
        if (other.allocPtr) {
            carry = other.carry;
            other.carry = nullptr;
            if (carry->allocSize == 0) {
                allocPtr = &stack;
                carry->toggle(allocPtr, other.allocPtr, 4);
            } else
                allocPtr = other.allocPtr;
            other.allocPtr = nullptr;
        } else
            allocPtr = nullptr;
    }

    Function& operator=(const Function& other) {
        destroy();
        if (other.allocPtr) {
            size_t allocSize = carry->allocSize;
            if (other.carry->allocSize == 0) {
                erase();
                allocPtr = &stack;
            } else if (allocSize and allocSize < other.carry->allocSize) {
                erase();
                allocPtr = std::malloc(carry->allocSize);
            }
            carry->~Carry();
            new(carry) Carry(*other.carry);
            carry->toggle(allocPtr, other.allocPtr, 1);
        } else {
            erase();
            allocPtr = nullptr;
        }
        return *this;
    }

    Function& operator=(Function&& other)  noexcept {
        destroy();
        if (other.allocPtr) {
            erase();

            carry->~Carry();
            delete carry;
            carry = other.carry;
            other.carry = nullptr;

            if (carry->allocSize == 0) {
                allocPtr = &stack;
                carry->toggle(allocPtr, other.allocPtr, 4);
            } else
                allocPtr = other.allocPtr;
            other.allocPtr = nullptr;
        } else {
            erase();
            allocPtr = nullptr;
        }
        return *this;
    }

    template <typename F, typename = std::enable_if_t<!std::is_lvalue_reference_v<F>>>
    Function& operator=(F&& functor) {
        Function<result_type(Args...)> new_function(std::move(functor));
        *this = std::move(new_function);
        return *this;
    }

//    template <typename F>
//    Function& operator=(std::reference_wrapper<F> functor) noexcept {
//        std::cerr << "call\n";
//
//    }

    result_type operator()(Args&&... args) {
        if (allocPtr == nullptr)
            throw std::bad_function_call();
        return carry->call(allocPtr, std::forward<Args>(args)...);
    }

    ~Function() {
        destroy();
        erase();
        delete carry;
    }
};

//template <class T>
//using Function = std::function<T>;

int foo(double) {
    return 1;
}

int bar(int) {
    return 2;
}

void BasicTest() {
    Function<int(double)> f = &foo;
    assert(sizeof(f) <= 48);

    auto ff = f;
    assert(sizeof(ff) <= 48);
    assert(ff(1) == 1);

    f = bar;
    assert(sizeof(f) <= 48);
    assert(f(2) == 2);

    ff = std::move(f);
    assert(sizeof(ff) <= 48);
    assert(ff(1) == 2);

    f = foo;
    assert(sizeof(f) <= 48);
    assert(f(3) == 1);
}

struct FunctorWithString {
    std::string str = "abcde";
    explicit FunctorWithString(const std::string& str): str(str) {}

    char& operator()(size_t index) {
        return str[index];
    }
};

void TestFunctorWithString() {
    auto func = FunctorWithString("hello world");
    Function<char&(size_t)> f = func;
    assert(sizeof(f) <= 48);
    f(0) = 'H';

    assert(func(0) != 'H');
    assert(f(0) == 'H');

    auto ff = f;
    assert(ff(0) == 'H');

    auto fff = std::move(ff);
    assert(fff(0) == 'H');

    assert(fff(0) == 'H');
}

void incrementor(int& x) {
    x++;
}

void TestVoidFunction() {
    int value = 0;

    Function<void(int&)> f = &incrementor;
    assert(sizeof(f) <= 48);

    f(value);
    assert(value == 1);

    auto f_copy = f;
    assert(sizeof(f_copy) <= 48);

    f_copy(value);
    assert(value == 2);
    f(value);
    assert(value == 3);

    auto f_move = std::move(f_copy);
    assert(sizeof(f_move) <= 48);

    f_move(value);
    assert(value == 4);
    f(value);
    assert(value == 5);
}

void TestClosure() {
    auto simple = [](int a, int b) -> int {
        return a + b;
    };
    Function<int(int, int)> f(simple);
    assert(sizeof(f) <= 48);

    assert(f(10, 20) == 30);

    int value = 3;
    auto medium = [&value](int add) -> int {
        return value + add;
    };
    Function<int(int)> ff(std::move(medium));
    assert(sizeof(ff) <= 48);

    assert(ff(2) == 5);
    assert(ff(10) == 13);

    value = 0;
    auto hard = [&value](int add) {
        value += add;
    };
    Function<void(int)> fff(hard);
    fff(1);
    assert(value == 1);
    fff(2);
    assert(value == 3);
    fff(3);
    assert(value == 6);

    Function<int()> ffff;
    {
        auto scoped = []() {
            return 5;
        };
        ffff = scoped;
    }

    assert(ffff() == 5);
}

struct NonOperatorEq {
    NonOperatorEq() = default;
    NonOperatorEq(const NonOperatorEq&) = default;
    NonOperatorEq(NonOperatorEq&&) = default;
    NonOperatorEq& operator=(const NonOperatorEq&) = delete;
    NonOperatorEq& operator=(NonOperatorEq&&) = delete;

    int operator()() {
        return value++;
    }

    int value = 0;
};

void TestNonCopyMoveAble() {
    Function<int()> f((NonOperatorEq()));
    assert(sizeof(f) <= 48);

    assert(f() == 0);
    assert(f() == 1);

    f = NonOperatorEq();
    assert(f() == 0);

    auto ff = std::move(f);
    assert(ff() == 1);
}

struct A {
    void inc(int add) {
        x += add;
    }

    static int sum(int a, int b) {
        return a + b;
    }

    int x = 0;
};

void TestBind() {
    A obj;
    Function<void(int)> f = std::bind(&A::inc, &obj, std::placeholders::_1);
    assert(sizeof(f) <= 48);

    f(10);
    assert(obj.x == 10);

    Function<int(int)> ff = std::bind(&A::sum, std::placeholders::_1, 5);
    assert(sizeof(f) <= 48);

    assert(ff(10) == 15);
}

int main() {
    BasicTest();
    TestFunctorWithString();
    TestVoidFunction();
    TestClosure();
    TestNonCopyMoveAble();
    TestBind();
    std::cerr << "Tests passed!" << std::endl;

    return 0;
}
