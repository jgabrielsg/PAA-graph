#include <random>
#include "bus.h"
#include "Graph.h"
#include "dataStructures.h"
#include <unordered_map>
#include <climits>
#include <algorithm>
#include <vector>
#include <iostream>

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
    if (e != m_group[e]) {
        m_group[e] = findE(m_group[e]);  // Path compression
    }
    return m_group[e];
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

// Dijkstra Fast Implementation
void cptDijkstraFast(const Graph& graph, int startVertex, std::vector<int>& parent, std::vector<double>& distance) {
    int numVertices = graph.getNumVertices();
    parent.assign(numVertices, -1);
    distance.assign(numVertices, std::numeric_limits<double>::infinity());
    std::vector<bool> checked(numVertices, false);

    // Priority queue: pair<distance, vertex>
    using pii = std::pair<double, int>;
    std::priority_queue<pii, std::vector<pii>, std::greater<pii>> pq;

    distance[startVertex] = 0.0;
    pq.emplace(0.0, startVertex);

    while (!pq.empty()) {
        double dist = pq.top().first;
        int current = pq.top().second;
        pq.pop();

        if (checked[current]) continue;
        checked[current] = true;

        // Iterate through all edges of current vertex
        Edge* edge = graph.getEdgeList(current);
        while (edge) {
            int neighbor = edge->otherVertex(current);
            double cost = edge->price_cost(); // Usando price_cost como peso da aresta

            if (distance[current] + cost < distance[neighbor]) {
                distance[neighbor] = distance[current] + cost;
                parent[neighbor] = current;
                pq.emplace(distance[neighbor], neighbor);
            }

            edge = edge->next();
        }
    }
}

// Compare function for sorting edges by cost
bool compareEdges(const Edge& e1, const Edge& e2) {
    return e1.price_cost() < e2.price_cost();
}

// Kruskal's Minimum Spanning Tree Algorithm (MST)
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
        int v1 = edge.v1();
        int v2 = edge.v2();

        int leaderV1 = uf.findE(v1);
        int leaderV2 = uf.findE(v2);

        if (leaderV1 != leaderV2) {
            uf.unionE(leaderV1, leaderV2);
            mstEdges.push_back(edge);  // Adiciona a aresta à MST
        }
    }
}

// Define a struct for Region
struct Region {
    int number;
    std::vector<vertex> nodes;
};

// Função para encontrar as melhores paradas de ônibus com uma distância mínima
void findBusStopsWithMinDistance(const Graph& graph, std::unordered_map<int, std::vector<std::string>>& result, int minDistance) {
    const std::vector<std::string>& nodeIds = graph.getNodes();

    // Reconstruct regions from m_regionMap
    // Assumindo que Graph possui uma função para iterar sobre todos os nós e obter suas regiões
    // Já que Graph não possui getRegions(), reconstruímos aqui
    std::unordered_map<int, Region> regionsMap;  // Map from region number to Region

    for (int v = 0; v < graph.getNumVertices(); ++v) {
        std::string nodeIdStr = graph.getNodeId(v);
        int region = graph.getRegion(nodeIdStr);
        if (region != -1) {
            regionsMap[region].number = region;
            regionsMap[region].nodes.push_back(v);
        }
    }

    // Convert regionsMap to a vector
    std::vector<Region> regions;
    for (const auto& pair : regionsMap) {
        regions.push_back(pair.second);
    }

    // Função lambda para escolher dois vértices aleatórios da região
    auto getRandomStops = [&](const Region& region) -> std::vector<int> {
        if (region.nodes.size() < 2) {
            // Não há nós suficientes para selecionar duas paradas diferentes
            return {};
        }

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
        std::vector<std::pair<int, double>> nodeDistances;  // Vetor para armazenar (vértice, maxDist)

        // 1. Seleciona o melhor nó (aquele com maior distância de todos os outros)
        for (const auto& vertexId : region.nodes) {
            // Executa o Dijkstra a partir deste nó
            std::vector<int> parent;
            std::vector<double> distance;
            cptDijkstraFast(graph, vertexId, parent, distance);

            // Calcula a maior distância (maxDist) para todos os outros nós na mesma região
            double maxDist = -1.0;
            for (const auto& otherVertexId : region.nodes) {
                if (vertexId != otherVertexId) {
                    maxDist = std::max(maxDist, distance[otherVertexId]);
                }
            }

            // Armazena o vértice e sua maior distância para os outros nós da região
            nodeDistances.emplace_back(vertexId, maxDist);
        }

        // Ordena os nós pela maior distância para os outros nós na região (maior distância primeiro)
        std::sort(nodeDistances.begin(), nodeDistances.end(), [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
            return a.second > b.second;  // Ordena pelo valor da maior distância
        });

        result[region.number].clear();  // Limpa as paradas anteriores (caso haja)

        std::vector<int> selectedStops;
        if (!nodeDistances.empty()) {
            selectedStops.push_back(nodeDistances[0].first);  // Seleciona a primeira parada (melhor)
        }

        // 2. Seleciona dois vértices aleatórios da região (não importa a distância)
        std::vector<int> randomStops = getRandomStops(region);
        if (randomStops.size() == 2) {
            selectedStops.push_back(randomStops[0]);
            selectedStops.push_back(randomStops[1]);
        }

        // Convertendo os vértices selecionados para IDs de nós (strings)
        std::vector<std::string> selectedStopIds;
        for (const auto& stop : selectedStops) {
            selectedStopIds.push_back(graph.getNodeId(stop));
        }

        // Adiciona as paradas selecionadas à região
        result[region.number] = selectedStopIds;
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
        std::vector<int> parent;
        std::vector<double> distance;

        // Calcule as distâncias a partir da parada 'i'
        cptDijkstraFast(graph, busStopVertices[i], parent, distance);

        for (int j = 0; j < numStops; ++j) {
            if (i != j) {
                // Obtém a distância entre a parada 'i' e a parada 'j'
                int stopIndex = busStopVertices[j];
                if (distance[stopIndex] != std::numeric_limits<double>::infinity()) {
                    distanceMatrix[i][j] = static_cast<int>(distance[stopIndex]);

                    // Cria uma aresta entre busStops[i] e busStops[j] para o MST
                    // Utiliza valores padrão para os outros atributos
                    Edge edge(
                        busStopVertices[i],
                        busStopVertices[j],
                        distanceMatrix[i][j],       // cost: use price_cost como o custo (distance)
                        distanceMatrix[i][j],       // distance
                        "unknown",                  // transport_type
                        0.0,                        // max_speed
                        distanceMatrix[i][j],       // price_cost
                        distanceMatrix[i][j] / 12.0, // time_cost (12 metros por segundo)
                        0.0,                        // excavation_cost
                        0,                          // num_residencial
                        0,                          // num_commercial
                        0,                          // num_touristic
                        0,                          // num_industrial
                        0                           // bus_preference
                    );

                    // Adiciona a aresta ao vetor de todas as arestas
                    allEdges.push_back(edge);
                }
            }
        }
    }

    // Agora, vamos aplicar o algoritmo de Kruskal para encontrar a árvore geradora mínima
    std::vector<Edge> mstEdges;
    mstKruskalType2(graph, allEdges, mstEdges);

    // Imprime a árvore geradora mínima
    std::cout << "Árvore Geradora Mínima (MST) entre as paradas de ônibus:\n";
    for (const auto& edge : mstEdges) {
        std::cout << "De: " << graph.getNodeId(edge.v1()) << " Para: " << graph.getNodeId(edge.v2())
                  << ", Custo: " << edge.price_cost() << std::endl;

        // Adiciona as arestas da MST no grafo original
        graph.addEdge(
            edge.v1(),
            edge.v2(),
            edge.price_cost(),
            edge.distance(),
            edge.transport_type(),
            edge.max_speed(),
            edge.price_cost(),
            edge.time_cost(),
            edge.excavation_cost(),
            edge.num_residencial(),
            edge.num_commercial(),
            edge.num_touristic(),
            edge.num_industrial(),
            edge.bus_preference()
        );
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
