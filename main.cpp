// main.cpp
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
    Graph cityGraph;

    // Load the graph from JSON
    if (!cityGraph.loadFromJson("city_graph.json")) {
        std::cerr << "Failed to load graph from JSON." << std::endl;
        return 1;
    }

    // Optionally, print nodes, edges, and properties for verification
    // cityGraph.printNodes();
    // std::cout << std::endl;
    // cityGraph.printEdges();
    // std::cout << std::endl;
    // cityGraph.printProperties();
    // std::cout << std::endl;

    // Retrieve regions from the graph
    const std::vector<Region>& regions = cityGraph.getRegions();

    // Initialize MetroDesigner with the graph, regions, and cost function
    MetroDesigner designer(cityGraph, regions, getPriceCost);

    // Execute the metro excavation design
    std::vector<Edge> excavationPlan = designer.escavacaoMetro();

    // Output the selected edges for excavation
    std::cout << "\nSelected segments for excavation:" << std::endl;
    for (const auto& edge : excavationPlan) {
        std::cout << "From: " << edge.from << " To: " << edge.to << " Cost: " << edge.price_cost << std::endl;
    }

    // return 0;
}
