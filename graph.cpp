#include "Graph.h"
#include <fstream>
#include <iostream>
#include "external/json.hpp"

using json = nlohmann::json;

Graph::Graph(int numVertices)
    : m_numVertices(numVertices), m_numEdges(0), m_nodeIds(numVertices, "") {
        for(vertex v = 0; v < numVertices; ++v){
            m_edges[v] = nullptr;  // Initialize each vertex with no edges
        }
    }

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

// Função addEdge atualizada para incluir novos atributos
void Graph::addEdge(vertex v1, vertex v2, double cost, int distance, const std::string& transport_type, double max_speed, double price_cost, double time_cost, int num_residencial, int num_commercial, int num_touristic, int num_industrial, int bus_preference) {
    if (!hasEdge(v1, v2)) {
        // Cria uma nova aresta com todos os atributos e adiciona à lista encadeada de arestas do vértice v1
        Edge* newEdge = new Edge(v1, v2, cost, distance, transport_type, max_speed, price_cost, time_cost, num_residencial, num_commercial, num_touristic, num_industrial, bus_preference, m_edges[v1]);
        m_edges[v1] = newEdge;
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

    // Adiciona arestas ao grafo
    for (const auto& edge : edges) {
        vertex v1 = nodeMap[edge["from"]];
        vertex v2 = nodeMap[edge["to"]];

        // Lê as propriedades da aresta
        std::string transport_type = edge["transport_type"];
        double max_speed = edge["max_speed"];
        double price_cost = edge["price_cost"];
        double time_cost = edge["time_cost"];
        int num_residencial = edge["num_residencial"];
        int num_commercial = edge["num_commercial"];
        int num_touristic = edge["num_touristic"];
        int num_industrial = edge["num_industrial"];
        int bus_preference = edge["bus_preference"];
        int distance = edge["distance"];
        double cost = edge["price_cost"];  // Usando 'price_cost' como custo para Kruskal

        // Adiciona a aresta ao grafo em ambas as direções (grafo não direcionado)
        addEdge(v1, v2, cost, distance, transport_type, max_speed, price_cost, time_cost, num_residencial, num_commercial, num_touristic, num_industrial, bus_preference);
        addEdge(v2, v1, cost, distance, transport_type, max_speed, price_cost, time_cost, num_residencial, num_commercial, num_touristic, num_industrial, bus_preference);
    }

}

