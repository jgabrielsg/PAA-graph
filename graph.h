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

    // Função addEdge atualizada para incluir novos atributos
    void addEdge(vertex v1, vertex v2, double cost, int distance, const std::string& transport_type, double max_speed, double price_cost, double time_cost, int num_residencial, int num_commercial, int num_touristic, int num_industrial, int bus_preference);

    void removeEdge(vertex v1, vertex v2, const std::string& transport_type);
    bool hasEdge(vertex v1, vertex v2, const std::string& transport_type);
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
    // Construtor sem o ponteiro para a próxima aresta (para iniciar a lista encadeada)
    Edge(vertex v1, vertex v2, double cost, int distance, const std::string& transport_type, double max_speed, double price_cost, double time_cost, int num_residencial, int num_commercial, int num_touristic, int num_industrial, int bus_preference)
        : m_v1(v1), m_v2(v2), m_cost(cost), m_distance(distance),
          m_transport_type(transport_type), m_max_speed(max_speed),
          m_price_cost(price_cost), m_time_cost(time_cost),
          m_num_residencial(num_residencial),
          m_num_commercial(num_commercial), m_num_touristic(num_touristic),
          m_num_industrial(num_industrial), m_bus_preference(bus_preference),
          m_next(nullptr) {}

    // Construtor com o ponteiro para a próxima aresta (para construir a lista encadeada)
    Edge(vertex v1, vertex v2, double cost, int distance, const std::string& transport_type, double max_speed, double price_cost, double time_cost, int num_residencial, int num_commercial, int num_touristic, int num_industrial, int bus_preference, Edge* next)
        : m_v1(v1), m_v2(v2), m_cost(cost), m_distance(distance),
          m_transport_type(transport_type), m_max_speed(max_speed),
          m_price_cost(price_cost), m_time_cost(time_cost),
          m_num_residencial(num_residencial),
          m_num_commercial(num_commercial), m_num_touristic(num_touristic),
          m_num_industrial(num_industrial), m_bus_preference(bus_preference),
          m_next(next) {}

    // Getters para os atributos
    vertex v1() const { return m_v1; }
    vertex v2() const { return m_v2; }
    double cost() const { return m_cost; }
    int distance() const { return m_distance; }
    std::string transport_type() const { return m_transport_type; }
    double max_speed() const { return m_max_speed; }
    double price_cost() const { return m_price_cost; }
    double time_cost() const { return m_time_cost; }
    int num_residencial() const { return m_num_residencial; }
    int num_commercial() const { return m_num_commercial; }
    int num_touristic() const { return m_num_touristic; }
    int num_industrial() const { return m_num_industrial; }
    int bus_preference() const { return m_bus_preference; }


    Edge* next() const { return m_next; }
    void setNext(Edge* next) { m_next = next; }

    // Get the other vertex (v1 or v2)
    vertex otherVertex(vertex v) const {
        return (v == m_v1) ? m_v2 : m_v1;
    }

private:
    vertex m_v1;
    vertex m_v2;
    double m_cost;                  // Custo da aresta (price_cost)
    int m_distance;                 // Distância da aresta
    std::string m_transport_type;   // Tipo de transporte
    double m_max_speed;             // Velocidade máxima
    double m_price_cost;            // Custo do preço
    double m_time_cost;             // Custo do tempo
    int m_num_residencial;          // Número de áreas residenciais
    int m_num_commercial;            // Número de áreas comerciais
    int m_num_touristic;            // Número de áreas turísticas
    int m_num_industrial;           // Número de áreas industriais
    int m_bus_preference;           // Preferência de ônibus
    Edge* m_next;                   // Próxima aresta na lista encadeada
};


#endif  // GRAPH_H