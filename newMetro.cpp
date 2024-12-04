#include "newMetro.h"
#include "dataStructures.h"
#include <unordered_map>
#include <climits>
#include <algorithm>
#include <vector>

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
    int eLeader = e;
    while (eLeader != m_group[eLeader]) {
        eLeader = m_group[eLeader];
    }
    return eLeader;
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