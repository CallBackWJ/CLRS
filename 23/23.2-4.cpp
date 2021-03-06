#include <cassert>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <list>

template <std::size_t n>
class Graph {
    struct UnionInfo {
        std::size_t index;
        std::size_t parent;
        std::size_t rank = 0;
        UnionInfo(std::size_t index) : index {index}, parent {index} {}
    };

    std::vector<std::list<std::pair<std::size_t, std::size_t>>> adj;
    std::unordered_map<std::size_t, std::list<std::pair<std::size_t, std::size_t>>::iterator> edges;

    void Link(std::size_t x, std::size_t y) {
        auto& ux = uinfos[x];
        auto& uy = uinfos[y];
        if (ux.rank < uy.rank) {
            uy.parent = x;
        } else {
            ux.parent = y;
            if (ux.rank == uy.rank) {
                uy.rank++;
            }
        }
    }

    std::size_t FindSet(std::size_t x) {
        auto& ux = uinfos[x];
        if (ux.index != ux.parent) {
            ux.parent = FindSet(ux.parent);
        }
        return ux.parent;
    }

    void Union(std::size_t x, std::size_t y) {
        Link(FindSet(x), FindSet(y));
    }

public:
    Graph() : adj(n) {
        for (std::size_t i = 0; i < n; i++) {
            uinfos.emplace_back(i);
        }
    }

    std::vector<UnionInfo> uinfos;

    void addEdge(std::size_t src, std::size_t dst, std::size_t weight) {
        assert(src < n && dst < n);
        if (!edges.contains(src * n + dst)) {
            adj[src].emplace_front(dst, weight);
            edges[src * n + dst] = adj[src].begin();
        }
        if (!edges.contains(dst * n + src)) {
            adj[dst].emplace_front(src, weight);
            edges[dst * n + src] = adj[dst].begin();
        }
    }

    void removeEdge(std::size_t src, std::size_t dst) {
        assert(src < n && dst < n);
        adj[src].erase(edges[src * n + dst]);
        edges.erase(src * n + dst);
        adj[dst].erase(edges[dst * n + src]);
        edges.erase(dst * n + src);
    }

    using Edge = std::tuple<std::size_t, std::size_t, std::size_t>;

    std::vector<Edge> countingSort(const std::vector<Edge>& A, size_t k) {
        std::vector<Edge> B (A.size());
        std::vector<size_t> C (k + 1);
        for (const auto& [src, dst, w] : A) {
            C[w]++;
        }
        for (size_t j = 1; j <= k; j++) {
            C[j] += C[j - 1];
        }
        for (size_t j = A.size() - 1; j < A.size(); j--) {
            std::size_t v = std::get<2>(A[j]);
            B[C[v] - 1] = A[j];
            C[v]--;
        }
        return B;
    }

    std::vector<Edge> MSTKruskal() {
        std::vector<Edge> A;
        std::vector<Edge> unsorted_edges;
        for (const auto& [idx, it] : edges) {
            std::size_t src = idx / n;
            std::size_t dst = idx % n;
            if (src > dst) {
                std::swap(src, dst); // ensure src < dst, no duplicate in set.
            }
            std::size_t w = it->second;
            unsorted_edges.emplace_back(src, dst, w);
        }
        auto sorted_edges = countingSort(unsorted_edges, 14);

        for (const auto& [src, dst, w] : sorted_edges) {
            if (FindSet(src) != FindSet(dst)) {
                A.emplace_back(src, dst, w);
                Union(src, dst);
            }
        }
        return A;
    }
};

int main() {
    Graph<9> g;
    g.addEdge(0, 1, 4);
    g.addEdge(0, 7, 8);
    g.addEdge(1, 2, 8);
    g.addEdge(1, 7, 11);
    g.addEdge(2, 3, 7);
    g.addEdge(2, 5, 4);
    g.addEdge(2, 8, 2);
    g.addEdge(3, 4, 9);
    g.addEdge(3, 5, 14);
    g.addEdge(4, 5, 10);
    g.addEdge(5, 6, 2);
    g.addEdge(6, 7, 1);
    g.addEdge(6, 8, 6);
    g.addEdge(7, 8, 7);
    auto gMST = g.MSTKruskal();
    for (const auto& [src, dst, w] : gMST) {
        std::cout << "(" << src << "-" << dst << ") : " << w << '\n';
    }
}
