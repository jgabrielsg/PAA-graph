#include <queue>
#include <unordered_map>
#include <limits>
#include <vector>
#include <tuple>
#include "Graph.h"

double calcularTempo(const Edge& edge) {
    if (edge.transport_type == "metro") {
        return (edge.distance / 70.0) * 60; 
    } else if (edge.transport_type == "onibus") {
        return (edge.distance / 50.0) * 60; 
    } else if (edge.transport_type == "taxi") {
        return (edge.distance / 50.0) * 60; 
    } else if (edge.transport_type == "deslocamento") {
        return (edge.distance / 5.0) * 60; 
    }
    return std::numeric_limits<double>::max(); 
}

double calcularCusto(const Edge& edge) {
    if (edge.transport_type == "metro") {
        return 4.40; 
    } else if (edge.transport_type == "onibus") {
        return 3.50; 
    } else if (edge.transport_type == "taxi") {
        return std::max(10.0, 2.50 * edge.distance);
    } else if (edge.transport_type == "deslocamento") {
        return 0.0; 
    }
    return std::numeric_limits<double>::max(); 
}

std::vector<std::string> encontrarRotaMaisRapida(Graph& graph, const std::string& origem, const std::string& destino, double orcamentoMax) {
    using Estado = std::tuple<double, std::string, double>; 
    std::priority_queue<Estado, std::vector<Estado>, std::greater<>> heap;

    std::unordered_map<std::string, double> distancia;
    std::unordered_map<std::string, double> custo;
    std::unordered_map<std::string, std::string> anterior;

    for (const auto& node : graph.getNodes()) {
        distancia[node.id] = std::numeric_limits<double>::max();
        custo[node.id] = std::numeric_limits<double>::max();
    }
    distancia[origem] = 0;
    custo[origem] = 0;

    heap.emplace(0.0, origem, 0.0); 

    while (!heap.empty()) {
        auto [tempoAtual, noAtual, custoAtual] = heap.top();
        heap.pop();

        if (noAtual == destino) {
            std::vector<std::string> caminho;
            for (std::string at = destino; !at.empty(); at = anterior[at]) {
                caminho.push_back(at);
            }
            std::reverse(caminho.begin(), caminho.end());
            return caminho;
        }

        for (const auto& edge : graph.getEdges()) {
            if (edge.from == noAtual || edge.to == noAtual) {
                std::string vizinho = (edge.from == noAtual) ? edge.to : edge.from;

                double novoTempo = tempoAtual + calcularTempo(edge);
                double novoCusto = custoAtual + calcularCusto(edge);

                if (novoCusto <= orcamentoMax && novoTempo < distancia[vizinho]) {
                    distancia[vizinho] = novoTempo;
                    custo[vizinho] = novoCusto;
                    anterior[vizinho] = noAtual;
                    heap.emplace(novoTempo, vizinho, novoCusto);
                }
            }
        }
    }

    return {}; 
}
