#ifndef BUS3_H
#define BUS3_H

#include "Graph.h"
#include <vector>

// Função para encontrar os melhores vértices
std::vector<std::vector<vertex>> encontrarMelhoresVerticesParaTodasRegioes(Graph &g);

// Função para calcular a matriz de distâncias e coletar os vértices selecionados
std::vector<std::vector<int>> calcularMatrizDeDistancias(Graph& graph, std::vector<vertex>& todosVertices);

// Função para adicionar arestas ao grafo com base na matriz de distâncias
void adicionarArestasAoGrafo(Graph& graph, const std::vector<std::vector<int>>& matrizDistancias);

// Função para calcular a MST e adicionar as arestas ao grafo original
void calcularMST(Graph& graph, Graph& graphDijkstra, const std::vector<vertex>& todosVertices);

#endif // BUS3_H
