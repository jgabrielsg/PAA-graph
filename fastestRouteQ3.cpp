#include <queue>
#include <unordered_map>
#include <vector>
#include <string>
#include <limits>
#include <tuple>
#include <functional>
#include <algorithm>
#include "newMetro.h"
#include "graph.h"

// unção para calcular o tempo baseado no tipo de transporte
double calcularTempo(const Edge& edge, const std::string& transport_type) {
    if (transport_type == "metro") {
        return (edge.distance() / 70.0) * 60; // Tempo em minutos
    } else if (transport_type == "onibus") {
        return (edge.distance() / 50.0) * 60;
    } else if (transport_type == "taxi") {
        return (edge.distance() / 50.0) * 60;
    } else if (transport_type == "walk") {
        return (edge.distance() / 5.0) * 60;
    }
    return std::numeric_limits<double>::max();
}

// função para calcular o custo baseado no tipo de transporte
double calcularCusto(const Edge& edge, const std::string& transport_type) {
    if (transport_type == "metro") {
        return 4.40;
    } else if (transport_type == "onibus") {
        return 3.50;
    } else if (transport_type == "taxi") {
        return std::max(10.0, 2.50 * edge.distance());
    }
    return std::numeric_limits<double>::max();
}

// definição de um estado para a fila de prioridade
struct Estado {
    int tempoGasto;
    int dinheiroGasto;
    vertex atual;
    std::string modoAtual;
    std::vector<vertex> caminho;

    bool operator<(const Estado& other) const {
        // para a fila de prioridade funcionar como min-heap, invertemos a comparação
        return tempoGasto > other.tempoGasto;
    }
};

// função para obter o melhor trajeto
std::pair<std::vector<vertex>, int> obter_melhor_trajeto(
    Graph& grafo,
    vertex v_inicial,
    vertex v_final,
    int K
) {
    // fila de prioridade: min-heap baseado no tempo gasto
    std::priority_queue<Estado> fila;

    // estado inicial: tempo = 0, dinheiro = 0, modo = "caminhar"
    fila.push(Estado{0, 0, v_inicial, "caminhar", {v_inicial}});

    // mapa para armazenar os estados visitados: (vértice, modo) -> dinheiro mínimo gasto
    std::unordered_map<std::string, int> visitados;

    while (!fila.empty()) {
        Estado atualEstado = fila.top();
        fila.pop();

        vertex verticeAtual = atualEstado.atual;
        int tempoAtual = atualEstado.tempoGasto;
        int dinheiroAtual = atualEstado.dinheiroGasto;
        std::string modoAtual = atualEstado.modoAtual;
        std::vector<vertex> caminhoAtual = atualEstado.caminho;

        // verifica se chegou ao destino
        if (verticeAtual == v_final) {
            return {caminhoAtual, tempoAtual};
        }

        // chave para o mapa de visitados
        std::string chave = std::to_string(verticeAtual) + "_" + modoAtual;

        // verifica se já visitou este estado com menos ou igual dinheiro
        if (visitados.find(chave) != visitados.end() && visitados[chave] <= dinheiroAtual) {
            continue;
        }

        // marca como visitado
        visitados[chave] = dinheiroAtual;

        // itera sobre as arestas do vértice atual
        Edge* edge = grafo.getEdges(verticeAtual);
        while (edge) {
            vertex vizinho = edge->otherVertex(verticeAtual);
            std::string tipoTransporte = edge->transport_type();
            int tempoAresta = 0;
            int custoAresta = 0;
            std::string novoModo = modoAtual;
            int novoDinheiro = dinheiroAtual;
            int novoTempo = tempoAtual + tempoAresta;
            std::vector<vertex> novoCaminho = caminhoAtual;
            novoCaminho.push_back(vizinho);

            // se a aresta for uma linha de metrô
            if (tipoTransporte == "metro") {
                if (modoAtual != "metro") { // se for entrar no metrô agora
                    custoAresta = calcularCusto(*edge, "metro");  // cobra o ticket de metrô
                    tempoAresta = calcularTempo(*edge, "metro");
                }
                novoModo = "metro";
            }
            // mesma lógica para o ônibus
            else if (tipoTransporte == "onibus") {
                if (modoAtual != "onibus") {
                    custoAresta = calcularCusto(*edge, "onibus"); // Custo do ticket de ônibus
                    tempoAresta = calcularTempo(*edge, "onibus");
                }
                novoModo = "onibus";
            }
            // se a aresta for uma linha de taxi 
            else if (tipoTransporte == "taxi") {
                // se a pessoa já estiver no taxi, ela vai continuar nele se tiver dinheiro
                if (modoAtual == "taxi" && dinheiroAtual + calcularCusto(*edge, "taxi") - 100 < K) 
                {
                    custoAresta = calcularCusto(*edge, "taxi") - 100;
                    tempoAresta = calcularTempo(*edge, "taxi");
                    novoModo = "taxi";
                }
                // se não tiver dinheiro, vai caminhar
                tempoAresta = calcularTempo(*edge, "walk");
                novoModo = "walk";
                }
            else if (tipoTransporte == "walk") {
                // se tiver andando, vai entrar no taxi se tiver dinheiro
                if (dinheiroAtual + calcularCusto(*edge, "taxi") < K) {
                    custoAresta = calcularCusto(*edge, "taxi") - 100;
                    tempoAresta = calcularTempo(*edge, "taxi");
                    novoModo = "taxi";
                }
                // se não tiver dinheiro, continua caminhando
                tempoAresta = calcularTempo(*edge, "walk");
                novoModo = "walk";
            }

            // atualiza o gasto
            novoDinheiro += custoAresta;

            // verifica se o custo está dentro do limite
            if (novoDinheiro > K) {
                // não considera este caminho
                edge = edge->next();
                continue;
            }

            // adiciona o novo estado à fila
            fila.push(Estado{
                novoTempo,
                novoDinheiro,
                vizinho,
                novoModo,
                novoCaminho
            });

            edge = edge->next();
        }
    }

    // caso não encontre um caminho válido
    return {std::vector<vertex>(), std::numeric_limits<int>::max()};
}
