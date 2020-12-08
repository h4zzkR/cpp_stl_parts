//
// Created by h4zzkR on 14.11.2020.
//
#include <iostream>
//#define IOS std::ios_base::sync_with_stdio(false), std::cin.tie(0),std::cout.tie(0);

class SplayTree {
    struct Node {
        Node *left = nullptr;
        Node *right = nullptr;
        Node *parent = nullptr;

        long long key;
        long long sum;

        long long max = -1;

        explicit Node(Node *parent, long long key) {
            this->parent = parent;
            left = nullptr;
            right = nullptr;
            this->key = key;
            sum = key;
            max = key;
        }
    };

    Node *root = nullptr;

    bool whoAmI(Node *node) {
        if (node->key > node->parent->key)
            return true;
        return false; // 1 for right, 0 for left
    }

    void connect(Node *node, Node *parent) {
        if (node != nullptr)
            node->parent = parent;
    }

    long long getSum(Node *node) {
        return (node != nullptr) ? node->sum : 0;
    }

    void updateSum(Node *node) {
        if (node != nullptr) {
            node->sum = getSum(node->left) + getSum(node->right) + node->key;
            node->max = (node->right == nullptr || node->key > node->right->max) ? node->key : node->right->max;
        }
    }

    Node *rotateLeft(Node* node, Node* parent_node) {
        Node *left_son = node->left;
        Node *grandparent = parent_node->parent;
        node->left = parent_node;
        parent_node->right = left_son;

        connect(parent_node, node);
        connect(node, grandparent);
        connect(left_son, parent_node);

        updateSum(node->left);
        updateSum(node);

//        if (grandparent != nullptr) {
//            if (node->key < grandparent->key)
//                grandparent->left = node;
//            else
//                grandparent->right = node;
//        }

        return node;
    }

    Node *rotateRight(Node* node, Node* parent_node) {
        /*
         *             C (3)             B (2)
         *            / \               / \
         * ->    (2) B   E (1) - > (1) A   C (2)
         *          / \                   / \
         *     (1) A   D (1)         (1) D   E (1)
         */
        Node *right_son = node->right;
        Node *grandparent = parent_node->parent;
        node->right = parent_node;
        parent_node->left = right_son;

        connect(parent_node, node);
        connect(node, grandparent);
        connect(right_son, parent_node);

        updateSum(node->right);
        updateSum(node);
//        updateSum(right_son);

//        if (grandparent != nullptr) {
//            if (node->key < grandparent->key)
//                grandparent->left = node;
//            else
//                grandparent->right = node;
//        }

        return node;
    }

    Node* splay(Node* nroot, Node* node) {
        while (node->parent != nullptr) { // идем до корня
            if (whoAmI(node) == 0) { // вершина слева
                if (node->parent->parent == nullptr) { // zig
                    node = rotateRight(node, nroot);
                } else if (whoAmI(node->parent) == 0) { // родитель тоже левый, zig-zig
                    Node* parent = rotateRight(node->parent, node->parent->parent);
                    node = rotateRight(node, parent);
                } else { // потомок левый, родитель правый, zig-zag
                    Node* grandparent = node->parent->parent;
                    node = rotateRight(node, node->parent);
                    node = rotateLeft(node, grandparent);
                }
            } else { // вершина справа
                if (node->parent->parent == nullptr) { // zig
                    node = rotateLeft(node, nroot);
                } else if (whoAmI(node->parent) == 1) { // родитель правый (что бы это ни значило), zig-zig
                    Node* parent = rotateLeft(node->parent, node->parent->parent);
                    node = rotateLeft(node, parent);
                } else { // потомок правый, родитель левый, zig-zag
                    Node* grandparent = node->parent->parent;
                    node = rotateLeft(node, node->parent);
                    node = rotateRight(node, grandparent);
                }
            }
        }
        nroot = node; // splay закончен
        updateSum(nroot);
        return nroot;
    }

    std::pair<Node*, Node*> split(Node* nroot, long long key, bool insert = false, bool root_in_left = false) {
        std::pair<bool, Node*> outpair = find(nroot, key, insert);
        nroot = outpair.second;
        bool found = outpair.first;
        if (found == 1 && insert != true || !found) {
            if (nroot->key == key && root_in_left) {
                Node *right_tree = nroot->right;
                connect(right_tree, nullptr);
                nroot->right = nullptr;
                updateSum(nroot);
                updateSum(right_tree);
                return std::make_pair(nroot, right_tree);
            } if (nroot->key >= key) {
                Node *left_tree = nroot->left;
                connect(left_tree, nullptr);
                nroot->left = nullptr;
                updateSum(nroot);
                updateSum(left_tree);
                return std::make_pair(left_tree, nroot);
            } if (nroot->key < key) {
                Node *right_tree = nroot->right;
                connect(right_tree, nullptr);
                nroot->right = nullptr;
                updateSum(nroot);
                updateSum(right_tree);
                return std::make_pair(nroot, right_tree);
            }
        }
        return std::make_pair(nullptr, nullptr);
    }

    Node* merge(Node* left, Node* right) {
        // работает, если слева ключи меньше ключей справа
        if (right == nullptr)
            return left;
        else if (left == nullptr)
            return right;
        else {
            std::pair<bool, Node*> outpair = find(left, right->key, false); // ищем в левом дереве (где все ключи меньше) элемент из правого
            // который точно больше всех слева, тем самым находим ближайший к этому элементу из ключей
            // левого, найденный элемент в корне - максимальный в дереве слева, к нему подсоединяем правое

            left = outpair.second;
            left->right = right;
            connect(right, left);
            updateSum(left);
            return left;
        }
    }

public:

    std::pair<bool, Node*> find(Node* node, long long key, bool insert) {
        // поиск ближайшего (в т.ч. и совпадающего)
        Node* nearest = nullptr;
        long long min = 10e9+1, distance = 0;
        Node* inode = node;
        bool found = false;

        while (inode != nullptr) {
            if (inode->key == key) {
                nearest = inode;
                found = true;
                break;
            }
            distance = abs(inode->key - key);
            if (distance < min) {
                min = distance;
                nearest = inode;
            }

            if (inode->key > key)
                inode = inode->left;
            else if (inode->key < key)
                inode = inode->right;
        }
        if (!found || !insert)
            node = splay(node, nearest);
        return std::make_pair(found, node);
    }


    void test() {
        insert(3);
        insert(5);
        insert(343);
        insert(24);
    }

    void insert(long long key) { // insert only in root
            if (root == nullptr) {
                root = new Node(nullptr, key);
            } else {
                std::pair<Node *, Node *> forest = split(root, key, true); // слева все ключи меньше, справа больше
                if (!(forest.first == nullptr && forest.second == nullptr)) { // если элемента еще не было в куче
                    Node *new_node = new Node(nullptr, key);
                    connect(forest.first, new_node);
                    connect(forest.second, new_node);
                    new_node->left = forest.first;
                    new_node->right = forest.second;
                    updateSum(new_node);
                    root = new_node;
                }
            }
        }

    long long search_sum(long long l, long long r) {
        long long sum;

        std::pair<Node *, Node *> trees = split(root, r, false, true);
        if (trees.first != nullptr && r >= trees.first->max) {
            // Такая проблема, при split не гарантируется (совсем не гарантируется)
            // что root будет исходным деревом. Поэтому тут будет два сплита и два merge, что, конечно,
            // не эфеективно, чем тот случай, когда исходное дерево не меняется и требуется 2 split и 1 merge,
            // но время поджимает, а рефакторингом заниматься долго
            std::pair<Node *, Node *> trees2 = split(trees.first, l, false, false);
            if (trees2.second == nullptr)
                sum = 0;
            else
                sum = trees2.second->sum;
            trees.first = merge(trees2.first, trees2.second);
            root = merge(trees.first, trees.second);
            return sum;
        } else {
            root = merge(trees.first, trees.second);
            return 0;
        }
    }

//    void sum(long long L, long long R);
};

void command_manager(SplayTree &tree, long long& sum, char& prev_command) {
    long long key;
    long long l, r;
    char command;
    std::cin >> command;
    if (command == '+') {
        std::cin >> key;
        if (prev_command == '?')
            tree.insert((key + sum) % 1000000000);
        else
            tree.insert(key);
    } else {
        std::cin >> l >> r;
        sum = tree.search_sum(l, r);
        std::cout << sum << '\n';
    }
//    std::cout << 1;
    prev_command = command;
}

int main() {
//    IOS;
    int n;
    long long sum;
    char prev_commmand = '#';
    std::cin >> n;
    SplayTree tree;
//    tree.test();
    for (int i = 0; i < n; ++i)
        command_manager(tree, sum, prev_commmand);
    return 0;
}