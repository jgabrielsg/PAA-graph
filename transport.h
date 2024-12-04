#pragma once

#include "graph.h"

namespace Transport {

    // Definição de enumeração para os modos de transporte
    enum class TransportMode { Metro, Bus, Taxi, Pedestrian };

    // Definição da interface abstrata do provedor de transporte
    class TransportProvider {
    public:
        virtual double getSpeed(const Graph::Edge& edge) const = 0;
        virtual double getCost(const Graph::Edge& edge) const = 0;
        virtual TransportMode getMode() const = 0;
    };

    // Declaração de classes concretas para cada modo de transporte
    class MetroProvider : public TransportProvider {
        // Implementação dos métodos virtuais
    };

    class BusProvider : public TransportProvider {
        // Implementação dos métodos virtuais
    };

    class TaxiProvider : public TransportProvider {
        // Implementação dos métodos virtuais
    };

    class PedestrianProvider : public TransportProvider {
        // Implementação dos métodos virtuais
    };
}