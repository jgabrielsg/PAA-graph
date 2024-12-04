#include "graph.h"
#include "newMetro.h"
#include "dataStructures.h"
#include <iostream>
#include <vector>
#include <string>

// Define the cost function (e.g., using 'price_cost' as excavation cost)
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

    // Optionally, print nodes, edges, and properties for verification
    // cityGraph.printNodes();
    // std::cout << std::endl;
    // cityGraph.printEdges();
    // std::cout << std::endl;
    // cityGraph.printProperties();
    // std::cout << std::endl;

    // Retrieve regions from the graph
    const std::vector<Region>& regions = cityGraph.getRegions();

    std::unordered_map<int, std::string> result;

    findStations(cityGraph, result);

    // Print results
    for (const auto& entry : result) {
        std::cout << "Region: " << entry.first << ", Best Node: " << entry.second << std::endl;
    }
    return 0;
}
