#include <iostream>
#include "string"

struct element {
    int n;
    element *next; // указатель на нижний элемент
    element(){
        n = 0;
        next = nullptr;
    }
};

struct stack {
    /*
     * Здесь я буду использовать void,
     * потому что не знаю, что возвращать в
     * int-овых функциях в том случае,
     * когда надо выводить "error".
     * Соответственно все реализовано на cout,
     * но при желании можно отбросить ненужный функционал
     * вывода ошибки и сделать все как надо,
     * с возвращаемыми значениями и исключениями в случае
     * каких-либо ошибок.
     */
    element *top_el;
    stack(){
        top_el = nullptr; // указатель на текущий элемент (сверху стека)
    }

    void push(int a){
        element *new_element;
        new_element = new element();
        new_element->n = a;
        new_element->next = top_el;
        top_el = new_element;
    }

    void back(){
        if (top_el == nullptr) {
            std::cout << "error";
        }
        else
            std::cout << top_el->n;
    }

    void pop(std::string modifier=""){
        if (top_el == nullptr) {
            if (modifier != "quiet")
                std::cout << "error";
        }
        else {
            int temp = top_el->n;
            element *temp_pointer = top_el->next;
            delete top_el; // удаление последнего элемента (вручную из-за new)
            top_el = temp_pointer; // указатель на новый предыдущий (тот, который был предыдущим для удаленного)
            if (modifier != "quiet")
                std::cout << temp;
        }
    }

    void size(){
        int cnt = 0;
        element *temp_pointer = top_el;
        while (temp_pointer != nullptr){
            cnt++;
            temp_pointer = temp_pointer->next;
        }
        std::cout << cnt;
    }

    void clear(){
        while (top_el != nullptr)
            pop("quiet");
        std::cout << "ok";
    }

};

std::string command_manager(stack *my_stack){
    std::string command; int n;
    std::getline(std::cin, command);
    if (command.find("push") != std::string::npos){
        n = stoi(command.substr(command.find(' ')+1));
        my_stack->push(n);
        std::cout << "ok";
    }
    else if (command == "pop"){
        my_stack->pop();
    }
    else if (command == "back"){
        my_stack->back();
    }
    else if (command == "size"){
        my_stack->size();
    }
    else if (command == "clear"){
        my_stack->clear();
    }
    return command;
}

int main() {
    stack my_stack;
    std::string command;
    while (command != "exit"){
        command = command_manager(&my_stack);
        std::cout << std::endl;
    }
    std::cout << "bye";
    return 0;
}
