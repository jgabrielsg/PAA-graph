#include "bus3.h"
#include "Graph.h"
#include "newMetro.h"
#include <limits.h>
#include <vector>
#include <iostream>
#include <algorithm>

// Função para calcular a distância entre dois vértices usando Dijkstra
int calcularDistancia(Graph& graph, vertex origem, vertex destino) {
    std::vector<int> distancias(graph.getNumVertices(), INT_MAX);
    std::vector<vertex> pais(graph.getNumVertices(), -1);
    Dijkstra::cptDijkstraFast(origem, pais.data(), distancias.data(), graph);
    return distancias[destino];
}

// Função para encontrar os 3 melhores vértices com distância mínima de 800 entre eles
std::vector<vertex> encontrarMelhoresVerticesParaRegiao(const std::vector<vertex>& regioes, Graph& graph) {
    std::vector<vertex> melhoresVertices;

    // Vetor para armazenar os 3 melhores vértices da região
    std::vector<vertex> candidatos = regioes;

    // Encontrar o melhor vértice (já implementado anteriormente)
    vertex melhorVertice = -1;
    int menorDistanciaMaxima = INT_MAX;

    for (vertex v : candidatos) {
        std::vector<int> distancias(graph.getNumVertices(), INT_MAX);
        std::vector<vertex> pais(graph.getNumVertices(), -1);

        // Executa Dijkstra a partir do vértice v
        Dijkstra::cptDijkstraFast(v, pais.data(), distancias.data(), graph);

        // Encontrar a maior distância para o vértice v
        int distanciaMaxima = *std::max_element(distancias.begin(), distancias.end());

        // Se a distância máxima for a menor que a anterior, atualiza o melhor vértice
        if (distanciaMaxima < menorDistanciaMaxima) {
            menorDistanciaMaxima = distanciaMaxima;
            melhorVertice = v;
        }
    }

    // Adiciona o melhor vértice à lista de melhores vértices
    melhoresVertices.push_back(melhorVertice);

    // Agora, tentamos adicionar o segundo melhor vértice, garantindo a distância mínima de 800 entre eles
    vertex segundoMelhorVertice = -1;
    for (vertex v : candidatos) {
        if (v != melhorVertice) {
            int distancia = calcularDistancia(graph, melhorVertice, v);
            if (distancia >= 800) {
                segundoMelhorVertice = v;
                break;
            }
        }
    }

    if (segundoMelhorVertice != -1) {
        melhoresVertices.push_back(segundoMelhorVertice);

        // Tentamos adicionar o terceiro melhor vértice, garantindo que a distância entre ele e os anteriores seja >= 800
        vertex terceiroMelhorVertice = -1;
        for (vertex v : candidatos) {
            if (v != melhorVertice && v != segundoMelhorVertice) {
                int distanciaParaMelhor = calcularDistancia(graph, melhorVertice, v);
                int distanciaParaSegundo = calcularDistancia(graph, segundoMelhorVertice, v);

                if (distanciaParaMelhor >= 800 && distanciaParaSegundo >= 800) {
                    terceiroMelhorVertice = v;
                    break;
                }
            }
        }

        if (terceiroMelhorVertice != -1) {
            melhoresVertices.push_back(terceiroMelhorVertice);
        }
    }

    return melhoresVertices;
}

// Função principal para iterar sobre as regiões e encontrar os 3 melhores vértices para cada região
std::vector<std::vector<vertex>> encontrarMelhoresVerticesParaTodasRegioes(Graph& graph) {
    // Obtemos as regiões do grafo
    std::vector<std::vector<vertex>> regioes = criarRegioes(graph);

    std::vector<std::vector<vertex>> melhoresVerticesPorRegiao;
    
    // Para cada região, encontramos os 3 melhores vértices com a condição de distância >= 800 entre eles
    for (const auto& regioesDeVertice : regioes) {
        std::vector<vertex> melhoresVertices = encontrarMelhoresVerticesParaRegiao(regioesDeVertice, graph);
        melhoresVerticesPorRegiao.push_back(melhoresVertices);
    }

    return melhoresVerticesPorRegiao;
}

// Função para criar a matriz de distâncias entre os vértices de parada de ônibus e coletar os vértices selecionados
std::vector<std::vector<int>> calcularMatrizDeDistancias(Graph& graph, std::vector<vertex>& todosVertices) {
    std::vector<std::vector<vertex>> melhoresVerticesPorRegiao = encontrarMelhoresVerticesParaTodasRegioes(graph);

    // Vetor para armazenar todos os 12 vértices
    for (const auto& regioesDeVertice : melhoresVerticesPorRegiao) {
        todosVertices.insert(todosVertices.end(), regioesDeVertice.begin(), regioesDeVertice.end());
    }

    int numVertices = todosVertices.size();  // Isso deve ser 12

    // Criando a matriz de distâncias 12x12
    std::vector<std::vector<int>> matrizDeDistancias(numVertices, std::vector<int>(numVertices, -1));

    // Calculando as distâncias entre cada par de vértices
    for (int i = 0; i < numVertices; ++i) {
        for (int j = 0; j < numVertices; ++j) {
            if (i != j) {
                matrizDeDistancias[i][j] = calcularDistancia(graph, todosVertices[i], todosVertices[j]);
            } else {
                matrizDeDistancias[i][j] = 0;  // A distância de um vértice para ele mesmo é 0
            }
        }
    }

    return matrizDeDistancias;
}

// Função para adicionar arestas ao grafo com base na matriz de distâncias
void adicionarArestasAoGrafo(Graph& graph, const std::vector<std::vector<int>>& matrizDistancias) {
    int numVertices = matrizDistancias.size();

    // Adicionando arestas ao grafo, usando as distâncias da matriz
    for (int i = 0; i < numVertices; ++i) {
        for (int j = 0; j < numVertices; ++j) {
            if (i != j && matrizDistancias[i][j] > 0) {
                graph.addEdge(
                    i, 
                    j, 
                    0, 
                    matrizDistancias[i][j], 
                    "bus", 
                    12, 
                    0, 
                    matrizDistancias[i][j]/12, 
                    0, 
                    0, 
                    0, 
                    0, 
                    0
                );
            }
        }
    }
}

// Função para calcular a MST (Árvore Geradora Mínima) usando Kruskal e adicionar as arestas ao grafo original
void calcularMST(Graph& graph, Graph& graphDijkstra, const std::vector<vertex>& todosVertices) {
    std::vector<Edge*> mstEdges;

    // Calcula a MST usando Kruskal
    Kruskal::mstKruskalFast(mstEdges, graphDijkstra);

    // Adiciona as arestas da MST no grafo original, utilizando os IDs originais dos vértices
    for (Edge* edge : mstEdges) {
        vertex originalV1 = todosVertices[edge->v1()];
        vertex originalV2 = todosVertices[edge->v2()];
        graph.addEdge(
            originalV1, 
            originalV2, 
            edge->cost(), 
            edge->distance(), 
            "bus", 
            12, 
            0, 
            edge->distance() / 12, 
            0, 
            0, 
            0, 
            0, 
            0
        );
        std::cout << "Aresta adicionada: " << originalV1 << " para " << originalV2 << std::endl;
    }
}
