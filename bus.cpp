#include <random>
#include "bus.h"
#include "Graph.h"
#include "dataStructures.h"
#include <unordered_map>
#include <climits>
#include <algorithm>
#include <vector>
#include <iostream>

// Função para encontrar as melhores paradas de ônibus com uma distância mínima
void findBusStopsWithMinDistance(const Graph& graph, std::unordered_map<int, std::vector<std::string>>& result, int minDistance) {
    const std::vector<Node>& nodes = graph.getNodes();
    const std::vector<Region>& regions = graph.getRegions();

    // Função lambda para escolher dois vértices aleatórios da região
    auto getRandomStops = [&](const Region& region) -> std::vector<int> {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, region.nodes.size() - 1);

        // Seleciona dois vértices aleatórios
        int firstRandomStop = region.nodes[dis(gen)];
        int secondRandomStop = region.nodes[dis(gen)];

        // Para garantir que não sejam o mesmo vértice, fazemos uma nova escolha caso isso aconteça
        while (firstRandomStop == secondRandomStop) {
            secondRandomStop = region.nodes[dis(gen)];
        }

        return {firstRandomStop, secondRandomStop};
    };

    // Para cada região, encontrar as melhores paradas de ônibus
    for (const auto& region : regions) {
        std::vector<std::pair<int, int>> nodeDistances;  // Vetor para armazenar (vértice, maxDist)

        // 1. Seleciona o melhor nó (aquele com maior distância de todos os outros)
        for (const auto& nodeId : region.nodes) {
            // Executa o Dijkstra a partir deste nó
            std::vector<std::string> parent;
            std::vector<int> distance;
            cptDijkstraFast(graph, graph.getNodeId(nodeId), parent, distance);

            // Calcula a maior distância (maxDist) para todos os outros nós na mesma região
            int maxDist = -1;
            for (const auto& otherNodeId : region.nodes) {
                if (nodeId != otherNodeId) {
                    int otherNodeIndex = otherNodeId;
                    maxDist = std::max(maxDist, distance[otherNodeIndex]);
                }
            }

            // Armazena o vértice e sua maior distância para os outros nós da região
            nodeDistances.emplace_back(nodeId, maxDist);
        }

        // Ordena os nós pela maior distância para os outros nós na região (maior distância primeiro)
        std::sort(nodeDistances.begin(), nodeDistances.end(), [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
            return a.second > b.second;  // Ordena pelo valor da maior distância
        });

        result[region.number].clear();  // Limpa as paradas anteriores (caso haja)

        std::vector<int> selectedStops;
        selectedStops.push_back(nodeDistances[0].first);  // Seleciona a primeira parada (melhor)

        // 2. Seleciona dois vértices aleatórios da região (não importa a distância)
        std::vector<int> randomStops = getRandomStops(region);
        selectedStops.push_back(randomStops[0]);
        selectedStops.push_back(randomStops[1]);

        // Adiciona as paradas selecionadas à região
        std::vector<std::string> selectedStopIds;
        for (const auto& stop : selectedStops) {
            selectedStopIds.push_back(graph.getNodeId(stop));
        }
        result[region.number] = selectedStopIds;
    }
}

// Função para calcular a Árvore Geradora Mínima (MST) utilizando o algoritmo de Kruskal
void mstKruskalType2(const Graph& graph, const std::vector<Edge>& allEdges, std::vector<Edge>& mstEdges) {
    int numVertices = graph.getNumVertices();

    // Copia todas as arestas para um vetor para ordenação
    std::vector<Edge> sortedEdges = allEdges;

    // Ordena as arestas pelo custo (price_cost)
    std::sort(sortedEdges.begin(), sortedEdges.end(), compareEdges);

    UnionFind uf(numVertices);
    mstEdges.clear();

    // Itera através das arestas ordenadas e constrói a MST usando Union-Find
    for (const auto& edge : sortedEdges) {
        int v1 = edge.from;
        int v2 = edge.to;

        int leaderV1 = uf.findE(v1);
        int leaderV2 = uf.findE(v2);

        if (leaderV1 != leaderV2) {
            uf.unionE(leaderV1, leaderV2);
            mstEdges.push_back(edge);  // Adiciona a aresta à MST
        }
    }
}

// Função para calcular as distâncias entre as paradas de ônibus e construir a MST
void calculateBusStopsDistances(Graph& graph, const std::vector<std::string>& busStops) {
    const int numStops = busStops.size();
    std::vector<std::vector<int>> distanceMatrix(numStops, std::vector<int>(numStops, std::numeric_limits<int>::max()));
    std::vector<Edge> allEdges;

    // Mapeia os IDs das paradas para seus respectivos vértices
    std::vector<int> busStopVertices;
    busStopVertices.reserve(numStops);
    for (const auto& busStopId : busStops) {
        int vertex = graph.getNodeIndex(busStopId);
        if (vertex != -1) {
            busStopVertices.push_back(vertex);
        } else {
            std::cerr << "Bus stop ID not found: " << busStopId << std::endl;
            return;
        }
    }

    // Para cada par de paradas de ônibus, calcule a distância entre elas
    for (int i = 0; i < numStops; ++i) {
        std::vector<std::string> parent;
        std::vector<int> distance;

        // Calcule as distâncias a partir da parada 'i'
        cptDijkstraFast(graph, busStops[i], parent, distance);

        for (int j = 0; j < numStops; ++j) {
            if (i != j) {
                // Obtém a distância entre a parada 'i' e a parada 'j'
                int stopIndex = busStopVertices[j];
                distanceMatrix[i][j] = distance[stopIndex];

                // Cria uma aresta entre busStops[i] e busStops[j] para o MST
                Edge edge;
                edge.from = busStopVertices[i];
                edge.to = busStopVertices[j];
                edge.distance = distanceMatrix[i][j]; // Distância entre os vértices
                edge.price_cost = distanceMatrix[i][j];  // Aqui, usamos a distância como o custo do preço
                edge.time_cost = distanceMatrix[i][j] / 12;   // 12 metros por segundo
                edge.excavation_cost = 0;
                edge.num_residencial = 0;
                edge.num_comercial = 0;
                edge.num_touristic = 0;
                edge.num_industrial = 0;
                edge.bus_preference = 0;

                // Adiciona a aresta ao vetor de todas as arestas
                allEdges.push_back(edge);
            }
        }
    }

    // Agora, vamos aplicar o algoritmo de Kruskal para encontrar a árvore geradora mínima
    std::vector<Edge> mstEdges;
    mstKruskalType2(graph, allEdges, mstEdges);

    // Imprime a árvore geradora mínima
    std::cout << "Árvore Geradora Mínima (MST) entre as paradas de ônibus:\n";
    for (const auto& edge : mstEdges) {
        std::cout << "De: " << graph.getNodeId(edge.from) << " Para: " << graph.getNodeId(edge.to)
                  << ", Custo: " << edge.price_cost << std::endl;

        // Adiciona as arestas da MST no grafo original
        graph.addEdge(edge.from, edge.to, edge.price_cost, edge.distance);
    }

    // Opcional: Imprimir a matriz de distâncias
    std::cout << "Matriz de distâncias entre as paradas de ônibus:\n";
    for (int i = 0; i < numStops; ++i) {
        for (int j = 0; j < numStops; ++j) {
            if (distanceMatrix[i][j] == std::numeric_limits<int>::max()) {
                std::cout << "INF\t";
            } else {
                std::cout << distanceMatrix[i][j] << "\t";
            }
        }
        std::cout << "\n";
    }
}

// Função para imprimir a matriz de distâncias
void printDistanceMatrix(const std::vector<std::vector<int>>& distanceMatrix, const std::vector<std::string>& busStops, const Graph& graph) {
    int numStops = busStops.size();
    std::cout << "Matriz de distâncias entre as paradas de ônibus:\n\t";
    for (const auto& busStop : busStops) {
        std::cout << busStop << "\t";
    }
    std::cout << "\n";

    for (int i = 0; i < numStops; ++i) {
        std::cout << busStops[i] << "\t";
        for (int j = 0; j < numStops; ++j) {
            if (distanceMatrix[i][j] == std::numeric_limits<int>::max()) {
                std::cout << "INF\t";
            } else {
                std::cout << distanceMatrix[i][j] << "\t";
            }
        }
        std::cout << "\n";
    }
}
