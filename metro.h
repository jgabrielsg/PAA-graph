#pragma once

#include "graph.h"

namespace Metro {

    // Definição de estruturas e classes relacionadas às linhas de metrô
    struct MetroStation {
        Vertex station;
        double excavationCost;
    };

    class MetroNetwork {
    public:
        void addMetroLine(const std::vector<MetroStation>& stations);
        double getTotalExcavationCost() const;
        // Outras operações relacionadas à rede de metrô
    private:
        std::vector<std::vector<MetroStation>> metroLines;
    };

    // Declaração de funções relacionadas ao planejamento das linhas de metrô
    std::vector<MetroStation> findOptimalMetroLine(const Graph::WeightedGraph& graph);
    double calculateExcavationCost(const Graph::WeightedGraph& graph, const std::vector<MetroStation>& stations);
    // Outras funções de otimização da rede de metrô
}