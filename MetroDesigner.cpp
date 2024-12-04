#include "MetroDesigner.h"
#include <queue>
#include <limits>
#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>
#include <unordered_set>

// Union-Find class removed as it's no longer needed

// Constructor
MetroDesigner::MetroDesigner(const Graph& graph, const std::vector<Region>& regions, double (*costFunc)(const Edge&))
    : graph(graph), regions(regions), getCost(costFunc) {}

// Optimized Dijkstra's algorithm with heap
void MetroDesigner::cptDijkstraFast(const std::string& source, std::unordered_map<std::string, double>& distances, std::unordered_map<std::string, std::string>& parent) {
    std::unordered_map<std::string, bool> checked;
    using pii = std::pair<double, std::string>;
    std::priority_queue<pii, std::vector<pii>, std::greater<pii>> heap;

    // Reset distances and parents
    for (const auto& node : graph.getNodes()) {
        distances[node.id] = std::numeric_limits<double>::infinity();
        parent[node.id] = "";
        checked[node.id] = false;
    }

    distances[source] = 0.0;
    heap.emplace(0.0, source);

    while (!heap.empty()) {
        auto [currentDist, currentNode] = heap.top();
        heap.pop();

        if (checked[currentNode]) continue;
        checked[currentNode] = true;

        for (const auto& [neighbor, weight] : graph.getNeighbors(currentNode)) {
            if (checked[neighbor]) continue;

            double newDist = currentDist + weight;
            if (newDist < distances[neighbor]) {
                distances[neighbor] = newDist;
                parent[neighbor] = currentNode;
                heap.emplace(newDist, neighbor);
            }
        }
    }
}

// Function to extract connecting edges based on parent maps
std::vector<Edge> MetroDesigner::extractConnectingEdges(
    const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& selectedStationsParents) {
    
    std::vector<Edge> pathEdges;

    // Convert edges to a map for quick lookup
    std::unordered_map<std::string, std::unordered_map<std::string, Edge>> edgeMap;
    for (const auto& edge : graph.getEdges()) {
        edgeMap[edge.from][edge.to] = edge;
        edgeMap[edge.to][edge.from] = edge; // Assuming undirected graph
    }

    // Iterate through each station's parent map
    for (const auto& [station, parentMap] : selectedStationsParents) {
        // Traverse from each station to every "end station"
        for (const auto& [endStation, parentNode] : parentMap) {
            if (!parentNode.empty()) {
                // Start from endStation and go backwards to the current station (station)
                std::vector<Edge> pathToCurrentStation;
                std::string currentStation = endStation;

                // Traverse in reverse from endStation to the current station
                while (currentStation != station) {
                    std::cout << "Traversing from " << currentStation << " to " << parentNode << std::endl;
                    if (edgeMap.find(currentStation) != edgeMap.end()) {
                        const auto& childrenMap = edgeMap.at(currentStation);
                        if (childrenMap.find(parentNode) != childrenMap.end()) {
                            // Add edge
                            pathToCurrentStation.push_back(childrenMap.at(parentNode));
                            currentStation = parentNode;
                        } else {
                            std::cerr << "Edge not found between " << currentStation << " and " << parentNode << std::endl;
                            break;
                        }
                    } else {
                        std::cerr << "No edge map entry found for station " << currentStation << std::endl;
                        break;
                    }
                }


                // If we reached the original station, add the path
                if (currentStation == station) {
                    std::reverse(pathToCurrentStation.begin(), pathToCurrentStation.end());
                    pathEdges.insert(pathEdges.end(), pathToCurrentStation.begin(), pathToCurrentStation.end());
                }
            }
        }
    }



    // Remove duplicate edges
    std::sort(pathEdges.begin(), pathEdges.end(), [&](const Edge& a, const Edge& b) {
        if (a.from < b.from) return true;
        if (a.from > b.from) return false;
        return a.to < b.to;
    });
    pathEdges.erase(std::unique(pathEdges.begin(), pathEdges.end(), [&](const Edge& a, const Edge& b) {
        return (a.from == b.from && a.to == b.to) || (a.from == b.to && a.to == b.from);
    }), pathEdges.end());

    return pathEdges;
}

// Function to create a subgraph with selected stations and their connecting edges
Graph MetroDesigner::createSubgraph(
    const std::set<std::string>& selectedStations, 
    const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& selectedStationsParents) {
    
    Graph subgraph;

    // To track all nodes involved in the paths
    std::unordered_set<std::string> involvedNodes;

    // Add all selected stations to the subgraph and mark them as involved
    for (const auto& station : selectedStations) {
        // Find the node in the original graph
        const auto& nodes = graph.getNodes();
        auto it = std::find_if(nodes.begin(), nodes.end(), [&](const Node& node) { return node.id == station; });
        if (it != nodes.end()) {
            subgraph.addNode(*it); // Use the public addNode method
            involvedNodes.insert(station);
        }
    }

    // Add all edges that connect the selected stations based on the parent maps
    std::vector<Edge> connectingEdges = extractConnectingEdges(selectedStationsParents);

    // Add edges and mark involved nodes
    for (const auto& edge : connectingEdges) {
        subgraph.addEdge(edge); // Use the public addEdge method
        involvedNodes.insert(edge.from);
        involvedNodes.insert(edge.to);
    }

    // Add all involved nodes to the subgraph
    for (const auto& node : graph.getNodes()) {
        if (involvedNodes.find(node.id) != involvedNodes.end() && 
            std::find_if(subgraph.getNodes().begin(), subgraph.getNodes().end(),
                         [&](const Node& n) { return n.id == node.id; }) == subgraph.getNodes().end()) {
            subgraph.addNode(node); // Use the public addNode method
        }
    }

    // Copy other properties if needed
    for (const auto& prop : graph.getProperties()) {
        subgraph.addProperty(prop); // Use the public addProperty method
    }

    return subgraph;
}

// Main metro excavation design algorithm
std::vector<Edge> MetroDesigner::escavacaoMetro() {
    std::cout << "Iniciando escavacaoMetro..." << std::endl;
    std::set<std::string> selectedStations;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> selectedStationsParents;

    // Step 1: Determine optimal station for each region
    for (const auto& region : regions) {
        std::string bestStation = "";
        double bestMaxDistance = std::numeric_limits<double>::infinity();

        std::unordered_map<std::string, double> distances;
        std::unordered_map<std::string, std::string> parent;

        for (const auto& candidate : region.nodes) {
            cptDijkstraFast(candidate, distances, parent);

            double currentMax = 0.0;
            bool valid = true;
            for (const auto& nodeInRegion : region.nodes) {
                double dist = distances[nodeInRegion];
                if (dist == std::numeric_limits<double>::infinity()) {
                    valid = false;
                    break;
                }
                currentMax = std::max(currentMax, dist);
            }

            if (valid && currentMax < bestMaxDistance) {
                bestMaxDistance = currentMax;
                bestStation = candidate;
            }
        }

        if (!bestStation.empty()) {
            selectedStations.insert(bestStation);
            selectedStationsParents[bestStation] = parent; // Store the parent map
            std::cout << "Estação selecionada: " << bestStation << " para a região: " << region.name << std::endl;
        } else {
            std::cout << "Nenhuma estação selecionada para a região: " << region.name << std::endl;
        }
    }

    // Print all selected stations
    std::cout << "\nEstações selecionadas:" << std::endl;
    for (const auto& station : selectedStations) {
        std::cout << " - " << station << std::endl;
    }

    // Step 2: Extract shortest path edges between selected stations
    std::vector<Edge> shortestPathEdges = extractConnectingEdges(selectedStationsParents);
    std::cout << "\nTotal de arestas nos caminhos mais curtos entre estações: " << shortestPathEdges.size() << std::endl;

    // Step 3: Create a subgraph with these edges
    Graph subgraph = createSubgraph(selectedStations, selectedStationsParents);
    std::cout << "\nTotal de arestas no subgrafo: " << subgraph.getEdges().size() << std::endl;

    // Step 4: Calculate the total cost based on shortest path edges
    double totalCost = 0.0;
    for (const auto& edge : shortestPathEdges) {
        totalCost += getCost(edge);
    }
    std::cout << "Custo total de escavação nos caminhos selecionados: " << totalCost << std::endl;

    return shortestPathEdges;
}
