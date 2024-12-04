#include "Graph.h"
#include <fstream>
#include <iostream>
#include "external/json.hpp"

using json = nlohmann::json;

GraphAdjList::GraphAdjList(int numVertices)
    : m_numVertices(numVertices), m_numEdges(0) {}

GraphAdjList::~GraphAdjList() {
    for (auto& pair : m_edges) {
        EdgeNode* edge = pair.second;
        while (edge) {
            EdgeNode* next = edge->next();
            delete edge;
            edge = next;
        }
    }
}

void GraphAdjList::addEdge(vertex v1, vertex v2) {
    if (!hasEdge(v1, v2)) {
        m_edges[v1] = new EdgeNode(v2, m_edges[v1]);
        m_numEdges++;
    }
}

void GraphAdjList::removeEdge(vertex v1, vertex v2) {
    EdgeNode* edge = m_edges[v1];
    EdgeNode* prevEdge = nullptr;
    while (edge) {
        if (edge->otherVertex() == v2) {
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

bool GraphAdjList::hasEdge(vertex v1, vertex v2) {
    EdgeNode* edge = m_edges[v1];
    while (edge) {
        if (edge->otherVertex() == v2) {
            return true;
        }
        edge = edge->next();
    }
    return false;
}

void GraphAdjList::print() const {
    for (const auto& pair : m_edges) {
        vertex v1 = pair.first;
        EdgeNode* edge = pair.second;
        while (edge) {
            std::cout << "(" << v1 << ", " << edge->otherVertex() << ") ";
            edge = edge->next();
        }
        std::cout << std::endl;
    }
}

void GraphAdjList::loadFromJSON(const std::string& filename) {
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

    // Load edges into the graph
    for (const auto& edge : edges) {
        vertex v1 = nodeMap[edge["from"]];
        vertex v2 = nodeMap[edge["to"]];
        addEdge(v1, v2);  // Add edge from v1 to v2
        addEdge(v2, v1);  // Add edge from v2 to v1 (since the graph is undirected)
    }
}
