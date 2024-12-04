#ifndef METRODESIGNER_H
#define METRODESIGNER_H

#include "graph.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>

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
    void dijkstra(const Graph& graph, const std::string& source, 
                  std::unordered_map<std::string, double>& distances, 
                  std::unordered_map<std::string, std::string>& parent);
    std::vector<Edge> kruskal(const Graph& graph, const std::vector<Edge>& allEdges, const std::unordered_set<std::string>& stations);

    // Função para extrair as arestas conectando as estações selecionadas
    std::vector<Edge> extractConnectingEdges(const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& selectedStationsParents);

    // Função auxiliar para criar um subgrafo com apenas as estações selecionadas e suas conexões
    Graph createSubgraph(const Graph& graph, const std::unordered_set<std::string>& selectedStations, const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& selectedStationsParents);

    // UnionFind class for Kruskal's algorithm
    class UnionFind {
    public:
        UnionFind(int numElements);

        int find(int e);
        void unionSets(int e1, int e2);

    private:
        std::vector<int> parent;
        std::vector<int> rank;
    };
};

#endif // METRODESIGNER_H
