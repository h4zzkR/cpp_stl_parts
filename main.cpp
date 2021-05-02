#include <iostream>
#include <vector>

const long long max = 10e15 + 1;

struct Graph {
    int n;
    bool has_negative_cycles = false;
    std::vector<std::vector<std::pair<int,long long>>> graph;
    std::vector<long long> dp;
    std::vector<bool> lower_zero;
    std::vector<int> prev;

    void dfs(int v) {
        lower_zero[v] = true;
        for (auto to : graph[v])
            if (!lower_zero[to.first])
                dfs(to.first);
    }

    void FordBellman(int st) {
        dp[st] = 0;

        for (int k = 0; k < n - 1; ++k)
            for (int v = 0; v < n; ++v) {
                if (dp[v] == max) continue;
                for (auto to : graph[v]) {
                    if (dp[to.first] > dp[v] + to.second) {
                        dp[to.first] = dp[v] + to.second;
                        prev[to.first] = v;
                    }
                }
            }

        for (int v = 0; v < n; ++v) {
            if (dp[v] == max) continue;
            for (auto to : graph[v]) {
                if (!lower_zero[to.first] && dp[to.first] > dp[v] + to.second) {
                    has_negative_cycles = true;
                    dfs(to.first);
                }
            }
        }
    }

    Graph(int n) {
        this->n = n;
        dp.resize(n, max);
        graph.resize(n);
        lower_zero.resize(n);
        prev.resize(n, -1);
    }
};

int main() {
    int s, n, m;
    std::cin >> n >> m >> s;
    int soe, eoe;
    long long w;
    Graph g(n);

    for (int i = 0; i < m; ++i) {
        std::cin >> soe >> eoe >> w;
        --soe; --eoe;
        g.graph[soe].push_back(std::make_pair(eoe, w));
    }

    g.FordBellman(--s);
    for (int i = 0; i < n; ++i) {
        if (g.dp[i] == max)
            std::cout << '*';
        else if (g.lower_zero[i])
            std::cout << '-';
        else
            std::cout << g.dp[i];
        std::cout << '\n';
    }
}
