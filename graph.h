#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include "external/json.hpp"

using json = nlohmann::json;

typedef int vertex;  // Define vertex type as int

// Forward declaration of the EdgeNode class
class EdgeNode;

class GraphAdjList {
public:
    GraphAdjList(int numVertices);
    ~GraphAdjList();

    void addEdge(vertex v1, vertex v2);
    void removeEdge(vertex v1, vertex v2);
    bool hasEdge(vertex v1, vertex v2);
    void print() const;
    void loadFromJSON(const std::string& filename);  // Function to load graph from a JSON file

private:
    int m_numVertices;
    int m_numEdges;
    std::unordered_map<vertex, EdgeNode*> m_edges;
};

// EdgeNode definition
class EdgeNode {
public:
    EdgeNode(vertex otherVertex, EdgeNode* next = nullptr)
        : m_otherVertex(otherVertex), m_next(next) {}

    vertex otherVertex() const { return m_otherVertex; }
    EdgeNode* next() const { return m_next; }
    void setNext(EdgeNode* next) { m_next = next; }

private:
    vertex m_otherVertex;
    EdgeNode* m_next;
};

#endif  // GRAPH_H