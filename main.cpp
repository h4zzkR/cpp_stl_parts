#include <iostream>
#include <vector>
#include <algorithm>

const int inf = 100'001;

struct DSU {
    int _size;
    std::vector<int> parent;
    std::vector<int> size;

    DSU(int _size) : _size(_size) {
        parent.resize(_size, -1);
        size.resize(_size, 1);
    }

    DSU() = default;

    int get(int v) {
        if (parent[v] == -1)
            return v;
        return parent[v] = get(parent[v]);
    }

    bool unite(int u, int v) {
        int root_u = get(u), root_v = get(v);
        if (root_u == root_v)
            return false;
        if (size[root_u] < size[root_v]) {
            parent[root_u] = root_v;
            size[root_v] += size[root_u];
            size[root_u] = -1;
        } else {
            parent[root_v] = root_u;
            size[root_u] += size[root_v];
            size[root_u] = -1;
        }
        return true;
    }
};

struct Graph {
    int n;

    struct Edge {
        int from = 0, to = 0;
        long long w = 0;
        Edge(int from, int to, int w) : from(from), to(to), w(w) {}
        Edge() = default;
    };

    std::vector<Edge> graph;
    DSU dsu;

    Graph(int n) {
        this->n = n;
        graph.resize(n);
        dsu = DSU(n);
    }

    long long kruskalMST() {
        std::sort(graph.begin(), graph.end(), [](const Edge& one, const Edge& two) { return one.w < two.w; });
        long long weight = 0;
        for (auto edge : graph) {
            bool united = dsu.unite(edge.from, edge.to);
            if (united)
                weight += edge.w;
        }
        return weight;
    }
};

int main() {
    int n, m;
    std::cin >> n >> m;
    int soe, eoe, w;
    Graph g(n);

    for (int i = 0; i < m; ++i) {
        std::cin >> soe >> eoe >> w;
        --soe; --eoe;
        g.graph.emplace_back(soe, eoe, w);
    }
    std::cout << g.kruskalMST();
}
