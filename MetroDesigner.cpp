#include "MetroDesigner.h"
#include <queue>
#include <limits>
#include <algorithm>
#include <iostream>

// Constructor
MetroDesigner::MetroDesigner(const Graph& graph, const std::vector<Region>& regions, double (*costFunc)(const Edge&))
    : graph(graph), regions(regions), getCost(costFunc) {}

// Dijkstra's algorithm for shortest paths
std::unordered_map<std::string, double> MetroDesigner::dijkstra(const std::string& source, const std::set<std::string>& regionNodes) {
    std::unordered_map<std::string, double> distances;
    for (const auto& node : regionNodes) {
        distances[node] = std::numeric_limits<double>::infinity();
    }
    distances[source] = 0.0;

    using pii = std::pair<double, std::string>;
    std::priority_queue<pii, std::vector<pii>, std::greater<pii>> pq;
    pq.emplace(0.0, source);

    std::unordered_map<std::string, std::vector<std::pair<std::string, double>>> adj;
    for (const auto& edge : graph.getEdges()) {
        adj[edge.from].emplace_back(edge.to, getCost(edge));
        adj[edge.to].emplace_back(edge.from, getCost(edge)); // Assuming undirected graph
    }

    while (!pq.empty()) {
        auto [currentDist, currentNode] = pq.top();
        pq.pop();

        if (currentDist > distances[currentNode]) continue;

        for (const auto& [neighbor, weight] : adj[currentNode]) {
            if (regionNodes.find(neighbor) == regionNodes.end()) continue;

            double newDist = currentDist + weight;
            if (newDist < distances[neighbor]) {
                distances[neighbor] = newDist;
                pq.emplace(newDist, neighbor);
            }
        }
    }

    return distances;
}

// Kruskal's algorithm for Minimum Spanning Tree (MST)
std::vector<Edge> MetroDesigner::kruskalMST(const std::vector<Edge>& edges, const std::set<std::string>& stations) {
    std::unordered_map<std::string, std::string> parent;
    for (const auto& station : stations) {
        parent[station] = station;
    }

    std::function<std::string(const std::string&)> findSet = [&](const std::string& x) -> std::string {
        if (parent[x] != x) parent[x] = findSet(parent[x]);
        return parent[x];
    };

    std::vector<Edge> sortedEdges = edges;
    std::sort(sortedEdges.begin(), sortedEdges.end(), [&](const Edge& a, const Edge& b) {
        return getCost(a) < getCost(b);
    });

    std::vector<Edge> mst;
    for (const auto& edge : sortedEdges) {
        if (stations.find(edge.from) == stations.end() || stations.find(edge.to) == stations.end()) continue;

        std::string root1 = findSet(edge.from);
        std::string root2 = findSet(edge.to);

        if (root1 != root2) {
            mst.push_back(edge);
            parent[root1] = root2;
        }

        if (mst.size() == stations.size() - 1) break;
    }

    return mst;
}

// Main metro excavation design algorithm
std::vector<Edge> MetroDesigner::escavacaoMetro() {
    std::set<std::string> selectedStations;

    // Step 1: Determine optimal station for each region
    for (const auto& region : regions) {
        std::set<std::string> regionNodes(region.nodes.begin(), region.nodes.end());
        std::string bestStation = "";
        double bestMaxDistance = std::numeric_limits<double>::infinity();

        for (const auto& candidate : region.nodes) {
            auto distances = dijkstra(candidate, regionNodes);
            double currentMax = 0.0;

            for (const auto& [node, dist] : distances) {
                if (dist == std::numeric_limits<double>::infinity()) {
                    currentMax = std::numeric_limits<double>::infinity();
                    break;
                }
                currentMax = std::max(currentMax, dist);
            }

            if (currentMax < bestMaxDistance) {
                bestMaxDistance = currentMax;
                bestStation = candidate;
            }
        }

        if (!bestStation.empty()) {
            selectedStations.insert(bestStation);
        }
    }

    // Step 2: Collect edges between selected stations
    std::vector<Edge> candidateEdges;
    for (const auto& edge : graph.getEdges()) {
        if (selectedStations.count(edge.from) && selectedStations.count(edge.to)) {
            candidateEdges.push_back(edge);
        }
    }

    // Step 3: Apply Kruskal's algorithm to find MST
    std::vector<Edge> mst = kruskalMST(candidateEdges, selectedStations);

    // Optionally calculate total cost
    double totalCost = 0.0;
    for (const auto& edge : mst) {
        totalCost += getCost(edge);
    }
    std::cout << "Total excavation cost: " << totalCost << std::endl;

    return mst;
}
