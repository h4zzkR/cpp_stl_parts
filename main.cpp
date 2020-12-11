//
// Created by h4zzkR on 14.11.2020.
//
#include <iostream>

class AVLTree {
    struct Node {
        Node* left = nullptr;
        Node* right = nullptr;
        int height = 1;
        long long key;

        Node(long long key) { // а зачем explicit? : Clion ругается
            left = nullptr;
            right = nullptr;
            height = 1;
            this->key = key;
        }

        ~Node() {
            delete this;//Навертел так же, как в Splay). Это рекурсивный вызов, он тут не нужен, так как у тебя всё равно внутри Node мало что происходит
        }
    };

    Node* root = nullptr;

    static int getHeight(Node* node) {
        return (node == nullptr) ? 0 : node->height;
    }

    static int getBalance(Node* node) {
        return getHeight(node->left) - getHeight(node->right);
    }

    static void updateNodeHeight(Node* node) {
        int h1 = getHeight(node->left), h2 = getHeight(node->right);
        node->height = ((h1 > h2) ? h1 : h2) + 1;
    }

    Node* search(Node* node, long long key) {
        if (node == nullptr || node->key == key)
            return node;
        else if (key < node->key)
            return search(node->left, key);
        else
            return search(node->right, key);
    }

    static Node* rotateLeft(Node* node) {
        Node* right_node = node->right; // B
        node->right = right_node->left; // right son of left son to old apex
        right_node->left = node; // C is right for B
        // heights changed only in B and C
        updateNodeHeight(right_node);
        updateNodeHeight(node);
        return right_node; // return new apex
    }

    static Node* rotateRight(Node* node) {
        /*
         * we_here->   C (3)             B (2)
         *            / \               / \
         *       (2) B   E (1) - > (1) A   C (2)
         *          / \                   / \
         *     (1) A   D (1)         (1) D   E (1)
         */
        Node* left_node = node->left; // B
        node->left = left_node->right; // right son of left son to old apex
        left_node->right = node; // C is right for B
        // heights changed only in B and C
        updateNodeHeight(node);
        updateNodeHeight(left_node);
        return left_node; // return new apex
    }

    static Node* updateTree(Node* node) {
        updateNodeHeight(node);
        int balance_a = getBalance(node);

        if (balance_a == 2) {
            if (getBalance(node->left) == -1) { // левое поддерево меньше правого
                // поворачиваем поддеревья - BigRotation
                node->left = rotateLeft(node->left);
            }
            return rotateRight(node); // Small rotation
        } else if (balance_a == -2) { // поворот влево
            if (getBalance(node->right) == 1) { // левое поддерево больше правого
                // поворачиваем поддеревья - BigRotation
                node->right = rotateRight(node->right);
            }
            return rotateLeft(node); // Small rotation
        } // else if balance_a == 1, -1 - дерево удовлетворяет условию AVL
        return node;
    }

    Node* insertNode(Node* node, long long key) {
        /*
         * Рекуривная реализация insert:
         * На каждом шаге рекурсии смотрим, куда нужно класть вершину,
         * обновляем высоты и выполняем балансировку текущей вершины.
         */
        if (node == nullptr) {
            return new Node(key);
        } if (node->key < key) {
            node->right = insertNode(node->right, key);
        } else if (node->key > key)
            node->left = insertNode(node->left, key);
        return updateTree(node);
    }

    Node* findSuccessor(Node* node) { // функция поиска минимума в поддереве дерева поиска
        return (node->left != nullptr) ? findSuccessor(node->left) : node;
    }

    Node* deleteNode(Node* node, long long key) {
        if (node == nullptr) // выход из рекурсии, если key нет в дереве
            return node;

        if (key < node->key) { // поиск нужной вершины
            node->left = deleteNode(node->left, key);
        } else if (key > node->key) {
            node->right = deleteNode(node->right, key);
        } else { // вершина найдена - удаляем
            /*
             * Без потомков или с одним потомком: тривиальный случай
             * Если у такой вершины есть потомок, то он только один - переподвесим его
             */
            if (node->left == nullptr && node->right == nullptr) {
                delete node;
                node = nullptr;
            } else if (node->left == nullptr) {
                Node *temp = node; // А где удаление старой вершины, ссылка на которую пропадёт? : А вот же оно
                node = node->right;
                temp->left = nullptr;
                temp->right = nullptr;
                delete temp;
            } else if (node->right == nullptr) {
                Node *temp = node; // вот
                node = node->left;
                temp->left = nullptr;
                temp->right = nullptr;
                delete temp;
            } else { // есть оба поддерева
                Node* apex = findSuccessor(node->right); // минимум множества элементов, больших node (правое поддерево)
                node->key = apex->key;
                node->right = deleteNode(node->right, apex->key); // рекурсивно запускаем удаление элемента apex
            }
        }

        if (node == nullptr)
            return nullptr; // empty
        node = updateTree(node);
        return node;
    }

public:
    void insert(long long key) {
        root = insertNode(root, key);
    }

    void delete_key(long long key) {
        root = deleteNode(root, key);
    }

    void exists(long long key) {
        Node* answ = search(root, key);
        std::cout << ((answ == nullptr) ? "false" : "true") << '\n';
    }

    void prev(long long x) {
        Node* node = root;
        long long apex = -1000000001;
        while(node != nullptr) {
            if (x > node->key) {
                if (node->key > apex)
                    apex = node->key;
                node = node->right;
            } else if (x < node->key || x == node->key) {
                node = node->left;
            }
        }
        std::cout << ((apex == -1000000001) ? "none" : std::to_string(apex)) << std::endl;
    }

    void next(long long x) {
        Node* node = root;
        long long apex = 1000000001;
        while(node != nullptr) {
            if (x > node->key || x == node->key) {
                node = node->right;
            } else if (x < node->key) {
                if (node->key < apex)
                    apex = node->key;
                node = node->left;
            }
        }
        std::cout << ((apex == 1000000001) ? "none" : std::to_string(apex)) << std::endl;
    }

    ~AVLTree() {// И тут нужно сделать нормально
        delete root->right;
        delete root->left;
        delete root;
    }
};

void command_manager(AVLTree &tree, std::string& command) {
    long long n;
    std::cin >> n;
    if (command == "insert") {
        tree.insert(n);
    } else if (command == "delete") {
        tree.delete_key(n);
    } else if (command == "exists") {
        tree.exists(n);
    } else if (command == "next") {
        tree.next(n);
    } else if (command == "prev") {
        tree.prev(n);
    }
}

int main() {
    AVLTree tree;
    std::string command;
    while (std::cin >> command) {
        command_manager(tree, command);
    }
//    tree.insert(23);
//    tree.insert(10);
//    tree.insert(29);
//    tree.insert(12);
//    tree.insert(25);
//    tree.insert(35);
//    tree.insert(24);
//    tree.insert(27);
//    tree.delete_key(29);
//    tree.search_key(342);
//    tree.next(10);
}
