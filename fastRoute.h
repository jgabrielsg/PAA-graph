#ifndef NEWMETRO_H
#define NEWMETRO_H

#include <vector>
#include <string>
#include <utility>
#include "graph.h"

// Funções de cálculo
double calcularTempo(const Edge& edge, const std::string& transport_type);
double calcularCusto(const Edge& edge, const std::string& transport_type);

// Estrutura do Estado
struct Estado {
    double tempoGasto;         
    double dinheiroGasto;      
    vertex atual;
    std::string modoAtual;
    std::vector<vertex> caminho;

    bool operator<(const Estado& other) const {
        // Para a fila de prioridade funcionar como min-heap, invertemos a comparação
        return tempoGasto > other.tempoGasto;
    }
};

// Função principal para obter o melhor trajeto
std::pair<std::vector<vertex>, double> obter_melhor_trajeto(
    Graph& grafo,
    vertex v_inicial,
    vertex v_final,
    double K
);

#endif // NEWMETRO_H