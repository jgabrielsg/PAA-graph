#pragma once

#include "graph.h"
#include "transport.h"

namespace Routing {

    // Definição de estruturas e classes relacionadas ao cálculo de rota
    struct RouteRequest {
        Vertex origin;
        Vertex destination;
        double maxCost;
        // Outros parâmetros da requisição de rota
    };

    struct RouteResponse {
        std::vector<Vertex> path;
        double totalTime;
        double totalCost;
    };

    class RoutingService {
    public:
        RouteResponse findOptimalRoute(const RouteRequest& request, const std::unordered_map<TransportMode, TransportProvider*>& transportProviders);
        // Outras operações relacionadas ao cálculo de rota
    private:
        double estimateTravelTime(const Graph::Edge& edge, TransportMode mode, const TransportProvider* provider) const;
        double estimateTravelCost(const Graph::Edge& edge, TransportMode mode, const TransportProvider* provider) const;
    };

    // Declaração de funções relacionadas ao cálculo de rota
    RouteResponse calculateShortestPath(const Graph::WeightedGraph& graph, const RouteRequest& request);
    // Outras funções de otimização de rota
}