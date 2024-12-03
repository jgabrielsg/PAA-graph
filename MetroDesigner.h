#ifndef METRODESIGNER_H
#define METRODESIGNER_H

#include "Graph.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#include <functional>

// Forward declare Region struct to avoid redefinition
struct Region;

class MetroDesigner {
public:
    // Constructor
    MetroDesigner(const Graph& graph, const std::vector<Region>& regions, double (*costFunc)(const Edge&));

    // Function to perform metro excavation design
    std::vector<Edge> escavacaoMetro();

private:
    Graph graph;
    std::vector<Region> regions;
    double (*getCost)(const Edge&); // Function pointer to retrieve edge cost

    // Helper functions
    void cptDijkstraFast(const std::string& source, std::unordered_map<std::string, double>& distances, std::unordered_map<std::string, std::string>& parent);
    std::vector<Edge> kruskalMST(const std::vector<Edge>& allEdges, const std::set<std::string>& stations);

    // UnionFind class for Kruskal's algorithm
    class UnionFind {
    public:
        UnionFind(int numElements);

        int findE(int e);
        void unionE(int e1, int e2);

    private:
        std::vector<int> parent;
        std::vector<int> rank;
    };
};

#endif // METRODESIGNER_H
