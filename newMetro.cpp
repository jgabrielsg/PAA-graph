#include "newMetro.h"
#include "dataStructures.h"
#include "graph.h"
#include <unordered_map>
#include <climits>
#include <algorithm>
#include <vector>
#include <string>

// Dijkstra Fast Implementation
void cptDijkstraFast(const Graph& graph, const std::string& v0_id, std::vector<std::string>& parent, std::vector<int>& distance) {
    // Get nodes and edges from the Graph instance
    const std::vector<Node>& nodes = graph.getNodes();
    const std::vector<Edge>& edges = graph.getEdges();

    // Create a mapping of node IDs to indices for faster lookup
    std::unordered_map<std::string, int> nodeIndex;
    for (size_t i = 0; i < nodes.size(); ++i) {
        nodeIndex[nodes[i].id] = i;
    }

    int numVertices = nodes.size();
    std::vector<bool> checked(numVertices, false);
    Heap heap; // Create the heap

    // Initialize parent and distance arrays
    parent.assign(numVertices, "-1");
    distance.assign(numVertices, INT_MAX);

    // Get the starting vertex index
    int v0 = nodeIndex[v0_id];
    parent[v0] = v0_id;
    distance[v0] = 0;
    heap.insert_or_update(distance[v0], v0);

    while (!heap.empty()) {
        int v1 = heap.top().second; // Min vertex
        heap.pop(); // Remove from heap

        if (distance[v1] == INT_MAX) { break; }

        // Loop through the edges of v1
        for (const auto& edge : edges) {
            // Check if the edge is connected to the current node (v1)
            if (edge.from == nodes[v1].id || edge.to == nodes[v1].id) {
                int v2 = (edge.from == nodes[v1].id) ? nodeIndex[edge.to] : nodeIndex[edge.from];
                if (!checked[v2]) {
                    int cost = edge.price_cost;
                    if (distance[v1] + cost < distance[v2]) {
                        parent[v2] = nodes[v1].id;
                        distance[v2] = distance[v1] + cost;
                        heap.insert_or_update(distance[v2], v2);
                    }
                }
            }
        }
        checked[v1] = true;
    }
}

// UnionFind Class for Kruskal's Algorithm
UnionFind::UnionFind(int numElements)
    : m_numElements(numElements), m_group(nullptr), m_groupSize(nullptr) {
    m_group = new int[numElements];
    m_groupSize = new int[numElements];
    for (int e = 0; e < numElements; e++) {
        m_group[e] = e;
        m_groupSize[e] = 1;
    }
}

UnionFind::~UnionFind() {
    delete[] m_group;
    delete[] m_groupSize;
}

int UnionFind::findE(int e) {
    if (e != m_group[e]) {
        m_group[e] = findE(m_group[e]);  // Path compression
    }
    return m_group[e];
}


void UnionFind::unionE(int e1, int e2) {
    int leader1 = findE(e1);
    int leader2 = findE(e2);

    if (leader1 != leader2) {
        if (m_groupSize[leader1] < m_groupSize[leader2]) {
            m_group[leader1] = leader2;
            m_groupSize[leader2] += m_groupSize[leader1];
        } else {
            m_group[leader2] = leader1;
            m_groupSize[leader1] += m_groupSize[leader2];
        }
    }
}

// Compare function for sorting edges by cost
bool compareEdges(const Edge& e1, const Edge& e2) {
    return e1.price_cost < e2.price_cost;
}

// Kruskal's Minimum Spanning Tree Algorithm (MST)
void mstKruskalFast(const Graph& graph, std::vector<Edge>& mstEdges) {
    const std::vector<Edge>& edges = graph.getEdges();
    int numVertices = graph.getNodes().size();

    // Prepare edges for sorting
    std::vector<Edge> sortedEdges;
    for (const auto& edge : edges) {
        sortedEdges.push_back(edge);
    }

    // Sort edges by their weight (price_cost)
    std::sort(sortedEdges.begin(), sortedEdges.end(), compareEdges);

    UnionFind uf(numVertices);
    mstEdges.clear();

    // Iterate through sorted edges and build MST using Union-Find
    for (const auto& edge : sortedEdges) {
        int v1 = std::find_if(graph.getNodes().begin(), graph.getNodes().end(), 
                              [&edge](const Node& n) { return n.id == edge.from; }) - graph.getNodes().begin();
        int v2 = std::find_if(graph.getNodes().begin(), graph.getNodes().end(), 
                              [&edge](const Node& n) { return n.id == edge.to; }) - graph.getNodes().begin();

        int leaderV1 = uf.findE(v1);
        int leaderV2 = uf.findE(v2);

        if (leaderV1 != leaderV2) {
            uf.unionE(leaderV1, leaderV2);
            mstEdges.push_back(edge);
        }
    }
}

void findStations(const Graph& graph, std::unordered_map<int, std::string>& result) {
    const std::vector<Node>& nodes = graph.getNodes();
    const std::vector<Region>& regions = graph.getRegions();
    
    // For each region, find the node with the minimum max distance to other nodes in the same region
    for (const auto& region : regions) {
        int minMaxDist = INT_MAX;
        std::string bestNode;

        // Iterate over each node in the region
        for (const auto& nodeId : region.nodes) {
            // Run Dijkstra from this node
            std::vector<std::string> parent;
            std::vector<int> distance;
            cptDijkstraFast(graph, nodeId, parent, distance);

            // Find the maximum distance to nodes in the same region
            int maxDist = -1;
            for (const auto& otherNodeId : region.nodes) {
                if (nodeId != otherNodeId) {
                    int otherNodeIndex = std::find_if(nodes.begin(), nodes.end(),
                        [&otherNodeId](const Node& n) { return n.id == otherNodeId; }) - nodes.begin();
                    maxDist = std::max(maxDist, distance[otherNodeIndex]);
                }
            }

            // Update the best node for this region
            if (maxDist < minMaxDist) {
                minMaxDist = maxDist;
                bestNode = nodeId;
            }
        }

        // Store the node with the minimum of maximum distances for the region
        result[region.number] = bestNode;
    }
}

std::vector<std::pair<std::string, std::string>> Graph::getPathFromParents(
    const std::vector<std::string>& parent, const std::string& start, const std::string& end) const {

    std::vector<std::pair<std::string, std::string>> path;
    std::string current = end;

    while (current != start) {
        std::string prev = parent[getNodeIndex(current)];
        path.push_back({prev, current});
        current = prev;
    }

    std::reverse(path.begin(), path.end());
    return path;
}

void findBestStations(const Graph& graph, std::unordered_map<int, std::string>& bestStations) {
    const std::vector<Region>& regions = graph.getRegions();
    const std::vector<Node>& nodes = graph.getNodes();

    for (const auto& region : regions) {
        int minMaxDist = INT_MAX;
        std::string bestNode;

        // Iterate over each node in the region
        for (const auto& nodeId : region.nodes) {
            // Run Dijkstra from this node
            std::vector<std::string> parent;
            std::vector<int> distance;
            cptDijkstraFast(graph, nodeId, parent, distance);

            // Find the maximum distance to nodes in the same region
            int maxDist = -1;
            for (const auto& otherNodeId : region.nodes) {
                if (nodeId != otherNodeId) {
                    int otherNodeIndex = graph.getNodeIndex(otherNodeId);
                    maxDist = std::max(maxDist, distance[otherNodeIndex]);
                }
            }

            // Update the best node for this region
            if (maxDist < minMaxDist) {
                minMaxDist = maxDist;
                bestNode = nodeId;
            }
        }

        // Store the best station for this region
        bestStations[region.number] = bestNode;
    }
}

std::vector<std::pair<std::string, std::string>> getPathEdgesFromParents(
    const Graph& graph, 
    const std::vector<std::string>& parent, 
    const std::string& startNodeId) {
    
    std::vector<std::pair<std::string, std::string>> pathEdges;
    std::string currentNodeId = startNodeId;

    // Traverse from the start node to the root using parent pointers
    while (parent[getNodeIndex(currentNodeId)] != currentNodeId) {
        std::string parentNodeId = parent[getNodeIndex(currentNodeId)];
        
        // Add the edge (parentNodeId -> currentNodeId)
        pathEdges.push_back({parentNodeId, currentNodeId});
        
        // Move to the parent node
        currentNodeId = parentNodeId;
    }

    // The pathEdges vector will contain the edges from the root to the start node.
    std::reverse(pathEdges.begin(), pathEdges.end()); // Reverse to make it from root to start node

    return pathEdges;
}

void createGraphFromBestStations(
    const std::unordered_map<int, std::string>& bestStations,
    Graph& newGraph, Graph& oldGraph) {

    // Iterate through each best station (key: region number, value: node ID of the best station)
    for (const auto& region : bestStations) {
        std::string bestStationId = region.second;

        // Run Dijkstra from the best station
        std::vector<std::string> parent;
        std::vector<int> distance;
        cptDijkstraFast(oldGraph, bestStationId, parent, distance);

        // For each of the other best stations, find the path from bestStationId to it
        for (const auto& otherRegion : bestStations) {
            std::string otherBestStationId = otherRegion.second;

            // Skip if it's the same station
            if (bestStationId == otherBestStationId) {
                continue;
            }

            // Get the path from bestStationId to otherBestStationId using parent info
            std::vector<std::pair<std::string, std::string>> pathEdges = getPathEdgesFromParents(oldGraph, parent, otherBestStationId);

            // Add the nodes and edges to the new graph
            for (const auto& edge : pathEdges) {
                const std::string& fromNodeId = edge.first;
                const std::string& toNodeId = edge.second;

                // Add the 'from' node to the new graph if not already present
                if (!newGraph.containsNode(fromNodeId)) {
                    newGraph.addNode(fromNodeId);
                }

                // Add the 'to' node to the new graph if not already present
                if (!newGraph.containsNode(toNodeId)) {
                    newGraph.addNode(toNodeId);
                }

                // Add the edge between 'from' and 'to' to the new graph if not already present
                if (!newGraph.containsEdge(fromNodeId, toNodeId)) {
                    newGraph.addEdge(fromNodeId, toNodeId, oldGraph.getEdgeCost(fromNodeId, toNodeId));
                }
            }
        }
    }
}
