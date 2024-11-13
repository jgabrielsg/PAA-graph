#include "metro.h"

namespace Metro {

    void MetroNetwork::addMetroLine(const std::vector<MetroStation>& stations) {
        metroLines.push_back(stations);
    }

    double MetroNetwork::getTotalExcavationCost() const {
        double totalCost = 0.0;
        for (const auto& line : metroLines) {
            totalCost += calculateExcavationCost(line);
        }
        return totalCost;
    }

    double calculateExcavationCost(const Graph::WeightedGraph& graph, const std::vector<MetroStation>& stations) {
        double totalCost = 0.0;
        // Implementação do cálculo do custo de escavação para a linha de metrô
        return totalCost;
    }

    std::vector<MetroStation> findOptimalMetroLine(const Graph::WeightedGraph& graph) {
        // Implementação do algoritmo de encontrar a linha de metrô ótima
        // Considerando a minimização do custo de escavação
        return std::vector<MetroStation>();
    }

}