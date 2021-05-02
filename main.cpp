#include <iostream>
#include <vector>
#include <algorithm>
#include <stack>

struct Graph {
    size_t vertex_count = 0;
    std::vector<std::vector<int>> igraph;
    std::vector<int> ans;

    Graph(size_t vertex_count) {
        this->vertex_count = vertex_count;
        igraph.resize(vertex_count, std::vector<int>(vertex_count, 0));
    }

    void euler(int v) {
        std::stack<int> euler_stack;
        euler_stack.push(v);
        while (!euler_stack.empty()) {
            int w = euler_stack.top();
            bool found = false;
            for (int i = 0; i < vertex_count; ++i) {
                if (igraph[w][i] == 1) {
                    euler_stack.push(i);
                    igraph[w][i] = 0;
                    found = true;
                    break;
                }
            }

            if (!found) {
                euler_stack.pop();
                ans.push_back(w);
            }
        }
    }
};

int main() {
    int vertex_cnt, start, edge;
    std::cin >> vertex_cnt >> start;
    --start;

    Graph g(vertex_cnt);
    for (int i = 0; i < vertex_cnt; ++i) {
        for (int j = 0; j < vertex_cnt; ++j) {
            std::cin >> edge;
            if (i != j && edge == 0)
                g.igraph[i][j] = 1;
        }
    }
    g.euler(start);

    for (int i = g.ans.size() - 1; i >= 1; --i) {
        std::cout << g.ans[i] + 1 << ' ' << g.ans[i - 1] + 1 << '\n';
    }
}
