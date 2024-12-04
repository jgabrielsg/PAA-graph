#include "Graph.h"
#include "MetroDesigner.h"
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
    std::cout << "Número de regiões: " << regions.size() << std::endl;

    // Initialize MetroDesigner with the graph, regions, and cost function
    MetroDesigner designer(cityGraph, regions, getPriceCost);
    std::cout << "MetroDesigner inicializado." << std::endl;

    // Execute the metro excavation design
    std::vector<Edge> excavationPlan = designer.escavacaoMetro();

    std::cout << "\nSegmentos selecionados para escavação:" << std::endl;
    std::cout << "[";
    // Output the selected edges for excavation
    for (const auto& edge : excavationPlan) {
        std::cout << "("<< edge.from << "," << edge.to << "),";
    }
    std::cout << "]" << std::endl;

    std::cout << "Programa finalizado com sucesso." << std::endl;
    return 0;
}
