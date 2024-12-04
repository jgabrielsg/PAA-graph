#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include "external/json.hpp"

using json = nlohmann::json;

typedef int vertex;  // Define vertex type as int

// Forward declaration of the Edge class
class Edge;

class Graph {
public:
    Graph(int numVertices);
    ~Graph();

    void addEdge(vertex v1, vertex v2, int cost, int distance);
    void removeEdge(vertex v1, vertex v2);
    bool hasEdge(vertex v1, vertex v2);
    void print() const;
    void loadFromJSON(const std::string& filename);  // Function to load graph from a JSON file

    // New functions for region management
    void setRegion(const std::string& nodeId, int region) {
        m_regionMap[nodeId] = region; // Store region for a node
    }

    int getRegion(const std::string& nodeId) const {
        auto it = m_regionMap.find(nodeId);
        if (it != m_regionMap.end()) {
            return it->second;
        }
        return -1;
    }

    // Function to set node ID at a given vertex index
    void setNodeId(vertex v, const std::string& nodeId) {
        if (v >= 0 && v < m_nodeIds.size()) {
            m_nodeIds[v] = nodeId;
        }
    }

    std::string getNodeId(vertex v) const {
        if (v >= 0 && v < m_nodeIds.size()) {
            return m_nodeIds[v];
        }
        return "";  // Return empty string if out of bounds
    }

    int getNumVertices() const { return m_numVertices; }
    Edge* getEdges(vertex v) const { return m_edges.at(v); }

private:
    std::unordered_map<std::string, int> m_regionMap;  // Map node IDs to region IDs
    std::vector<std::string> m_nodeIds;  // Vector to store node IDs corresponding to vertices
    int m_numVertices;
    int m_numEdges;
    std::unordered_map<vertex, Edge*> m_edges;
};



class Edge {
public:
    Edge(vertex v1, vertex v2, int cost, int distance)
        : m_v1(v1), m_v2(v2), m_cost(cost), m_distance(distance), m_next(nullptr) {}

    Edge(vertex v1, vertex v2, int cost, int distance, Edge* next)
        : m_v1(v1), m_v2(v2), m_cost(cost), m_distance(distance), m_next(next) {}

    vertex v1() const { return m_v1; }
    vertex v2() const { return m_v2; }
    int cost() const { return m_cost; }
    int distance() const { return m_distance; }
    Edge* next() const { return m_next; }
    void setNext(Edge* next) { m_next = next; }

    // Get the other vertex (v1 or v2)
    vertex otherVertex(vertex v) const {
        return (v == m_v1) ? m_v2 : m_v1;
    }

private:
    vertex m_v1;
    vertex m_v2;
    int m_cost;
    int m_distance;
    Edge* m_next;
};


#endif  // GRAPH_H