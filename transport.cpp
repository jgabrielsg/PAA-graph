#include "transport.h"

namespace Transport {

    double MetroProvider::getSpeed(const Graph::Edge& edge) const {
        // Implementação da velocidade do metrô no segmento de rua
        return 70.0; // Exemplo de velocidade fixa do metrô
    }

    double MetroProvider::getCost(const Graph::Edge& edge) const {
        // Implementação do cálculo do custo da viagem de metrô no segmento de rua
        return 2.50; // Exemplo de custo fixo do metrô
    }

    TransportMode MetroProvider::getMode() const {
        return TransportMode::Metro;
    }

    // Implementações similares para os outros provedores de transporte
}