#ifndef BUS_H
#define BUS_H

#include "Graph.h"
#include <unordered_map>
#include <vector>
#include <string>

// Função para encontrar as paradas de ônibus com a distância mínima
void findBusStopsWithMinDistance(const Graph& graph, std::unordered_map<int, std::vector<std::string>>& result, int minDistance);

// Função para calcular as distâncias entre as paradas de ônibus
void calculateBusStopsDistances(Graph& graph, const std::vector<std::string>& busStops);

// Função para imprimir a matriz de distâncias entre as paradas de ônibus
void printDistanceMatrix(const std::vector<std::vector<int>>& distanceMatrix);

// Função auxiliar para encontrar dois vértices aleatórios em uma região
std::vector<std::string> getRandomStops(const Region& region);

#endif // BUS_H
