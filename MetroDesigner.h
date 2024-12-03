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
    std::unordered_map<std::string, double> dijkstra(const std::string& source, const std::set<std::string>& regionNodes);
    std::vector<Edge> kruskalMST(const std::vector<Edge>& edges, const std::set<std::string>& stations);
};

#endif // METRODESIGNER_H
