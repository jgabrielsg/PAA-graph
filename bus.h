#ifndef BUS_H
#define BUS_H

#include "graph.h"
#include <unordered_map>
#include <vector>
#include <string>

double calculateEdgeWeight(const Edge&);
std::vector<vertex> findHamiltonianCycle(Graph&);
void designBusRoute(Graph&);

// Define uma estrutura para Região
struct Region {
    int number;
    std::vector<vertex> nodes;
};

// Função para encontrar as paradas de ônibus com a distância mínima
void findBusStopsWithMinDistance(const Graph& graph, std::unordered_map<int, std::vector<std::string>>& result, int minDistance);

// Função para calcular as distâncias entre as paradas de ônibus
void calculateBusStopsDistances(Graph& graph, const std::vector<std::string>& busStops);

// Função para imprimir a matriz de distâncias entre as paradas de ônibus
void printDistanceMatrix(const std::vector<std::vector<int>>& distanceMatrix, const std::vector<std::string>& busStops, const Graph& graph);

// Função auxiliar para encontrar dois vértices aleatórios em uma região
std::vector<int> getRandomStops(const Region& region);

#endif // BUS_H
