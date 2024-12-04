#include "graph.h"
#include "newMetro.h"
#include "dataStructures.h"
#include <iostream>
#include <vector>
#include <string>

double getPriceCost(const Edge& edge) {
    return edge.price_cost;
}

int main() {
    std::cout << "Iniciando o programa principal..." << std::endl;
    Graph cityGraph;

    // Load the graph from JSON
    if (!cityGraph.loadFromJson("city_graph.json")) {
        std::cerr << "Falha ao carregar o grafo do JSON." << std::endl;
        return 1;
    }
    std::cout << "Grafo carregado com sucesso." << std::endl;

    // Retrieve regions from the graph
    const std::vector<Region>& regions = cityGraph.getRegions();

    std::unordered_map<int, std::string> result;
    findBestStations(cityGraph, result);

    // Print results
    for (const auto& entry : result) {
        std::cout << "Region: " << entry.first << ", Best Node: " << entry.second << std::endl;
    }

    Graph newGraph;
    createGraphFromBestStations(result, newGraph, cityGraph);

    std::cout << "Novo grafo com as melhores estações criado." << std::endl;
    std::cout << "Número de nós no novo grafo: " << newGraph.getNodes().size() << std::endl;
    std::cout << "Número de arestas no novo grafo: " << newGraph.getEdges().size() << std::endl;

    std::vector<Edge> mstEdges;
    mstKruskalFast(newGraph, mstEdges);

    mstEdges = newGraph.getEdges();

    // Print MST edges
    if (!mstEdges.empty()) {
        std::cout << "[";
        for (size_t i = 0; i < mstEdges.size(); ++i) {
            const auto& entry = mstEdges[i];
            std::cout << "('" << entry.from << "','" << entry.to << "')";
            if (i < mstEdges.size() - 1) {
                std::cout << ",";  // Add a comma if it's not the last element
            }
        }
        std::cout << "]" << std::endl;
    } else {
        std::cout << "No edges in MST." << std::endl;
    }

    return 0;
}
