// main.cpp
#include "Graph.h"
#include <iostream>

int main() {
    Graph cityGraph;
    
    // Load the graph from JSON
    if (!cityGraph.loadFromJson("city_graph.json")) {
        std::cerr << "Failed to load graph from JSON." << std::endl;
        return 1;
    }
    
    // Print nodes, edges, and properties
    cityGraph.printNodes();
    std::cout << std::endl;
    cityGraph.printEdges();
    std::cout << std::endl;
    cityGraph.printProperties();
    
    return 0;
}
