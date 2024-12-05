#include "newMetro.h"
#include <queue>
#include <unordered_map>
#include <limits>
#include <stdexcept>

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

// Função para calcular o tempo baseado no tipo de transporte
double calcularTempo(const Edge& edge, const std::string& transport_type) {
    if (transport_type == "metro") {
        return (edge.distance() / 70.0) * 60.0; // Tempo em minutos
    } else if (transport_type == "onibus") {
        return (edge.distance() / 50.0) * 60.0;
    } else if (transport_type == "taxi") {
        return (edge.distance() / 50.0) * 60.0;
    } else if (transport_type == "walk") {
        return (edge.distance() / 5.0) * 60.0;
    }
    return std::numeric_limits<double>::max();
}

// Função para calcular o custo baseado no tipo de transporte
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

// Função para obter o melhor trajeto
std::pair<std::vector<vertex>, double> obter_melhor_trajeto(
    Graph& grafo,
    vertex v_inicial,
    vertex v_final,
    double K
) {
    // Fila de prioridade: min-heap baseado no tempo gasto
    std::priority_queue<Estado> fila;

    // Estado inicial: tempo = 0, dinheiro = 0, modo = "walk"
    fila.push(Estado{0.0, 0.0, v_inicial, "walk", {v_inicial}});

    // Mapa para armazenar os estados visitados: (vértice, modo) -> dinheiro mínimo gasto
    std::unordered_map<std::string, double> visitados;

    while (!fila.empty()) {
        Estado atualEstado = fila.top();
        fila.pop();

        vertex verticeAtual = atualEstado.atual;
        double tempoAtual = atualEstado.tempoGasto;
        double dinheiroAtual = atualEstado.dinheiroGasto;
        std::string modoAtual = atualEstado.modoAtual;
        std::vector<vertex> caminhoAtual = atualEstado.caminho;

        // Verifica se chegou ao destino
        if (verticeAtual == v_final) {
            return {caminhoAtual, tempoAtual};
        }

        // Chave para o mapa de visitados
        std::string chave = std::to_string(verticeAtual) + "_" + modoAtual;

        // Verifica se já visitou este estado com menos ou igual dinheiro
        if (visitados.find(chave) != visitados.end() && visitados[chave] <= dinheiroAtual) {
            continue;
        }

        // Marca como visitado
        visitados[chave] = dinheiroAtual;

        // Itera sobre as arestas do vértice atual
        Edge* edge = grafo.getEdges(verticeAtual);
        while (edge) {
            vertex vizinho = edge->otherVertex(verticeAtual);
            std::string tipoTransporte = edge->transport_type();
            double tempoAresta = 0.0;
            double custoAresta = 0.0;
            std::string novoModo = modoAtual;
            double novoDinheiro = dinheiroAtual;
            double novoTempo = tempoAtual;
            std::vector<vertex> novoCaminho = caminhoAtual;
            novoCaminho.push_back(vizinho);

            // Se a aresta for uma linha de metrô
            if (tipoTransporte == "metro") {
                if (modoAtual != "metro") { // Se for entrar no metrô agora
                    custoAresta = calcularCusto(*edge, "metro");  // Cobre o ticket de metrô
                    tempoAresta = calcularTempo(*edge, "metro");
                }
                // Se já estiver no metrô, apenas adiciona o tempo
                else {
                    tempoAresta = calcularTempo(*edge, "metro");
                }
                novoModo = "metro";
            }
            // Mesma lógica para o ônibus
            else if (tipoTransporte == "onibus") {
                if (modoAtual != "onibus") {
                    custoAresta = calcularCusto(*edge, "onibus"); // Custo do ticket de ônibus
                    tempoAresta = calcularTempo(*edge, "onibus");
                }
                // Se já estiver no ônibus, apenas adiciona o tempo
                else {
                    tempoAresta = calcularTempo(*edge, "onibus");
                }
                novoModo = "onibus";
            }
            // Se a aresta for uma linha de taxi 
            else if (tipoTransporte == "taxi") {
                // Se a pessoa já estiver no taxi, ela continua nele
                if (modoAtual == "taxi") {
                    custoAresta = calcularCusto(*edge, "taxi");
                    tempoAresta = calcularTempo(*edge, "taxi");
                    novoModo = "taxi";
                }
                // Se não estiver no taxi, pode optar por pegar um taxi ou caminhar
                else {
                    // Opção 1: Pegar um taxi
                    double custoTaxi = calcularCusto(*edge, "taxi");
                    double tempoTaxi = calcularTempo(*edge, "taxi");
                    if (dinheiroAtual + custoTaxi <= K) {
                        std::vector<vertex> caminhoTaxi = novoCaminho;
                        fila.push(Estado{
                            tempoAtual + tempoTaxi,
                            dinheiroAtual + custoTaxi,
                            vizinho,
                            "taxi",
                            caminhoTaxi
                        });
                    }

                    // Opção 2: Caminhar
                    double tempoWalk = calcularTempo(*edge, "walk");
                    novoModo = "walk";
                    tempoAresta = tempoWalk;
                    // Nenhum custo adicional para caminhar
                }
            }
            // Se a aresta for caminhar
            else if (tipoTransporte == "walk") {
                // Se estiver caminhando, continua caminhando
                if (modoAtual == "walk") {
                    tempoAresta = calcularTempo(*edge, "walk");
                }
                // Se estiver em outro modo, pode optar por caminhar
                else {
                    tempoAresta = calcularTempo(*edge, "walk");
                }
                novoModo = "walk";
            }

            // Atualiza o tempo e o dinheiro
            novoTempo += tempoAresta;
            novoDinheiro += custoAresta;

            // Verifica se o custo está dentro do limite
            if (novoDinheiro > K) {
                // Não considera este caminho
                edge = edge->next();
                continue;
            }

            // Adiciona o novo estado à fila
            fila.push(Estado{
                novoTempo,
                novoDinheiro,
                vizinho,
                novoModo,
                novoCaminho
            });

            // Próxima aresta
            edge = edge->next();
        }
    }

    // Caso não encontre um caminho válido
    return {std::vector<vertex>(), std::numeric_limits<double>::max()};
}
