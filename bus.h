#pragma once

#include "graph.h"

namespace Bus {

    // Definição de estruturas e classes relacionadas à linha de ônibus
    struct BusStop {
        Vertex stop;
        int commercialProperties;
        int residentialProperties;
    };

    class BusLine {
    public:
        void addBusStop(const BusStop& stop);
        double getCommercialCoverage() const;
        double getResidentialCoverage() const;
        // Outras operações relacionadas à linha de ônibus
    private:
        std::vector<BusStop> busStops;
    };

    // Declaração de funções relacionadas à criação da linha de ônibus
    BusLine createOptimalBusLine(const Graph::WeightedGraph& graph);
    double calculateCoverageScore(const std::vector<BusStop>& stops);
    // Outras funções de otimização da linha de ônibus
}