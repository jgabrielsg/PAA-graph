#include "MetroDesigner.h"
#include <queue>
#include <limits>
#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>
#include <unordered_set>

// Union-Find structure for Kruskal's algorithm
MetroDesigner::UnionFind::UnionFind(int numElements)
    : parent(numElements), rank(numElements, 0) {
    for (int i = 0; i < numElements; ++i) {
        parent[i] = i;
    }
}

int MetroDesigner::UnionFind::findE(int e) {
    if (parent[e] != e) {
        parent[e] = findE(parent[e]); // Path compression
    }
    return parent[e];
}

void MetroDesigner::UnionFind::unionE(int e1, int e2) {
    int root1 = findE(e1);
    int root2 = findE(e2);

    if (root1 != root2) {
        // Union by rank
        if (rank[root1] < rank[root2]) {
            parent[root1] = root2;
        } else if (rank[root1] > rank[root2]) {
            parent[root2] = root1;
        } else {
            parent[root2] = root1;
            rank[root1]++;
        }
    }
}

// Constructor
MetroDesigner::MetroDesigner(const Graph& graph, const std::vector<Region>& regions, double (*costFunc)(const Edge&))
    : graph(graph), regions(regions), getCost(costFunc) {}

// Optimized Dijkstra's algorithm with heap (sem alterações)
void MetroDesigner::cptDijkstraFast(const std::string& source, std::unordered_map<std::string, double>& distances, std::unordered_map<std::string, std::string>& parent) {
    std::unordered_map<std::string, bool> checked;
    using pii = std::pair<double, std::string>;
    std::priority_queue<pii, std::vector<pii>, std::greater<pii>> heap;

    // Reset distances and parents
    for (const auto& node : graph.getNodes()) {
        distances[node.id] = std::numeric_limits<double>::infinity();
        parent[node.id] = "";
        checked[node.id] = false;
    }

    distances[source] = 0.0;
    heap.emplace(0.0, source);

    while (!heap.empty()) {
        auto [currentDist, currentNode] = heap.top();
        heap.pop();

        if (checked[currentNode]) continue;
        checked[currentNode] = true;

        for (const auto& [neighbor, weight] : graph.getNeighbors(currentNode)) {
            if (checked[neighbor]) continue;

            double newDist = currentDist + weight;
            if (newDist < distances[neighbor]) {
                distances[neighbor] = newDist;
                parent[neighbor] = currentNode;
                heap.emplace(newDist, neighbor);
            }
        }
    }
}

// Kruskal's MST algorithm modificado
std::vector<Edge> MetroDesigner::kruskalMST(const std::vector<Edge>& allEdges, const std::set<std::string>& stations) {
    std::cout << "Entrando em kruskalMST com " << allEdges.size() << " arestas e " << stations.size() << " estações." << std::endl;

    // Mapeamento de IDs de nós para índices
    std::unordered_map<std::string, int> nodeIdToIndex;
    int index = 0;
    for (const auto& node : graph.getNodes()) {
        nodeIdToIndex[node.id] = index++;
    }

    UnionFind uf(graph.getNodes().size());

    // Ordenar todas as arestas por custo
    std::vector<Edge> sortedEdges = allEdges;
    std::sort(sortedEdges.begin(), sortedEdges.end(), [&](const Edge& a, const Edge& b) {
        return getCost(a) < getCost(b);
    });

    std::vector<Edge> mst;
    // Para rastrear quais estações já estão conectadas
    // Inicialmente, cada estação está em seu próprio conjunto
    std::unordered_set<int> stationIndices;
    for (const auto& station : stations) {
        stationIndices.insert(nodeIdToIndex[station]);
    }

    // Número de estações que já estão conectadas
    int connectedStations = 1;

    for (const auto& edge : sortedEdges) {
        int idxFrom = nodeIdToIndex[edge.from];
        int idxTo = nodeIdToIndex[edge.to];

        int leader1 = uf.findE(idxFrom);
        int leader2 = uf.findE(idxTo);

        if (leader1 != leader2) {
            uf.unionE(leader1, leader2);
            mst.push_back(edge);
            std::cout << "Aresta adicionada ao MST: " << edge.from << " - " << edge.to << " Custo: " << getCost(edge) << std::endl;

            // Verificar se as estações estão todas conectadas
            bool allConnected = true;
            int firstLeader = -1;
            for (const auto& station : stations) {
                int stationLeader = uf.findE(nodeIdToIndex[station]);
                if (firstLeader == -1) {
                    firstLeader = stationLeader;
                } else if (stationLeader != firstLeader) {
                    allConnected = false;
                    break;
                }
            }

            if (allConnected) {
                std::cout << "Todas as estações selecionadas estão agora conectadas." << std::endl;
                break;
            }
        }
    }

    return mst;
}

// Main metro excavation design algorithm
std::vector<Edge> MetroDesigner::escavacaoMetro() {
    std::cout << "Iniciando escavacaoMetro..." << std::endl;
    std::set<std::string> selectedStations;

    // Step 1: Determine optimal station for each region
    for (const auto& region : regions) {
        std::string bestStation = "";
        double bestMaxDistance = std::numeric_limits<double>::infinity();

        std::unordered_map<std::string, double> distances;
        std::unordered_map<std::string, std::string> parent;

        for (const auto& candidate : region.nodes) {
            cptDijkstraFast(candidate, distances, parent);

            double currentMax = 0.0;
            bool valid = true;
            for (const auto& nodeInRegion : region.nodes) {
                double dist = distances[nodeInRegion];
                if (dist == std::numeric_limits<double>::infinity()) {
                    valid = false;
                    break;
                }
                currentMax = std::max(currentMax, dist);
            }

            if (valid && currentMax < bestMaxDistance) {
                bestMaxDistance = currentMax;
                bestStation = candidate;
            }
        }

        if (!bestStation.empty()) {
            selectedStations.insert(bestStation);
            std::cout << "Estação selecionada: " << bestStation << " para a região: " << region.name << std::endl;
        } else {
            std::cout << "Nenhuma estação selecionada para a região: " << region.name << std::endl;
        }
    }

    // Imprimir todas as estações selecionadas
    std::cout << "\nEstações selecionadas:" << std::endl;
    for (const auto& station : selectedStations) {
        std::cout << " - " << station << std::endl;
    }

    // Step 2: Coletar todas as arestas do grafo
    std::vector<Edge> allEdges = graph.getEdges();
    std::cout << "\nTotal de arestas no grafo: " << allEdges.size() << std::endl;

    // Passar todas as arestas para Kruskal
    std::vector<Edge> mst = kruskalMST(allEdges, selectedStations);

    // Calcular o custo total
    double totalCost = 0.0;
    for (const auto& edge : mst) {
        totalCost += getCost(edge);
    }
    std::cout << "Custo total de escavação: " << totalCost << std::endl;

    return mst;
}
