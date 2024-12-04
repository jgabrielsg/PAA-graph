#include "Graph.h"
#include <fstream>
#include <iostream>
#include "external/json.hpp"

using json = nlohmann::json;

Graph::Graph(int numVertices)
    : m_numVertices(numVertices), m_numEdges(0) {}

Graph::~Graph() {
    for (auto& pair : m_edges) {
        Edge* edge = pair.second;
        while (edge) {
            Edge* next = edge->next();
            delete edge;
            edge = next;
        }
    }
}

void Graph::addEdge(vertex v1, vertex v2, int cost) {
    if (!hasEdge(v1, v2)) {
        m_edges[v1] = new Edge(v1, v2, cost, m_edges[v1]);
        m_numEdges++;
    }
}


void Graph::removeEdge(vertex v1, vertex v2) {
    Edge* edge = m_edges[v1];
    Edge* prevEdge = nullptr;
    while (edge) {
        if (edge->otherVertex(v1) == v2) {
            if (prevEdge) {
                prevEdge->setNext(edge->next());
            } else {
                m_edges[v1] = edge->next();
            }
            delete edge;
            m_numEdges--;
            break;
        }
        prevEdge = edge;
        edge = edge->next();
    }
}

bool Graph::hasEdge(vertex v1, vertex v2) {
    Edge* edge = m_edges[v1];
    while (edge) {
        if (edge->otherVertex(v1) == v2) {
            return true;
        }
        edge = edge->next();
    }
    return false;
}

void Graph::print() const {
    for (const auto& pair : m_edges) {
        vertex v1 = pair.first;
        Edge* edge = pair.second;
        while (edge) {
            std::cout << "(" << v1 << ", " << edge->otherVertex(v1) << ") ";
            edge = edge->next();
        }
        std::cout << std::endl;
    }
}

void Graph::loadFromJSON(const std::string& filename) {
    // Open the JSON file and parse it
    std::ifstream file(filename);
    json graphData;
    file >> graphData;

    // Load nodes and edges
    auto nodes = graphData["nodes"];
    auto edges = graphData["edges"];

    // Map node ids to integer indices
    std::unordered_map<std::string, vertex> nodeMap;
    vertex nodeId = 0;
    for (const auto& node : nodes) {
        nodeMap[node["id"]] = nodeId++;
    }

    for (const auto& edge : edges) {
        vertex v1 = nodeMap[edge["from"]];
        vertex v2 = nodeMap[edge["to"]];
        int cost = edge["cost"];  // Extract the cost from the JSON object
        addEdge(v1, v2, cost);   // Add edge from v1 to v2
        addEdge(v2, v1, cost);   // Add edge from v2 to v1 (since the graph is undirected)
    }

}

