#include "bus.h"

namespace Bus {

    void BusLine::addBusStop(const BusStop& stop) {
        busStops.push_back(stop);
    }

    double BusLine::getCommercialCoverage() const {
        double totalCommercial = 0.0;
        for (const auto& stop : busStops) {
            totalCommercial += stop.commercialProperties;
        }
        return totalCommercial;
    }

    double BusLine::getResidentialCoverage() const {
        double totalResidential = 0.0;
        for (const auto& stop : busStops) {
            totalResidential += stop.residentialProperties;
        }
        return totalResidential;
    }

    BusLine createOptimalBusLine(const Graph::WeightedGraph& graph) {
        // Implementação do algoritmo de criação da linha de ônibus ótima
        // Considerando a maximização da cobertura de imóveis comerciais/turísticos
        return BusLine();
    }

    double calculateCoverageScore(const std::vector<BusStop>& stops) {
        // Implementação do cálculo do score de cobertura da linha de ônibus
        return 0.0;
    }

}