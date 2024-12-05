import json
import random
import matplotlib.pyplot as plt
import networkx as nx
import ast

# Função para criar o grafo NetworkX
def create_graph(graph, N, M):
    # Usar MultiGraph para permitir múltiplas arestas entre os mesmos nós
    G = nx.MultiGraph()

    # Adicionar nós com atributo de região (CEP)
    color_map = {"51000": "blue", "52000": "green", "53000": "orange", "54000": "purple"}
    node_colors = []

    for node in graph['nodes']:
        # Determinar a região e a cor com base no CEP
        i, j = node['location']
        if i < N // 4:
            cep = "51000"  # Região 1
        elif i > 3 * N // 4:
            cep = "54000"  # Região 4
        elif i >= N // 4 and i <= 3 * N // 4:
            cep = "52000" if j < M // 2 else "53000"  # Região 2 ou 3

        G.add_node(node['id'], pos=node['location'], region=cep)
        node_colors.append(color_map[cep])  # Adiciona a cor correspondente

    # Adicionar arestas
    for edge in graph['edges']:
        G.add_edge(edge['from'], edge['to'], weight=edge['distance'], transport_type=edge['transport_type'])

    return G, node_colors

# Função para plotar apenas as arestas destacadas
def plot_highlighted_edges(G, node_colors, highlighted_edges):
    """
    Plota apenas as arestas destacadas no grafo.

    :param G: Grafo NetworkX.
    :param node_colors: Lista de cores para os nós.
    :param highlighted_edges: Lista de tuplas representando as arestas a serem destacadas.
    """
    # Converter a lista de arestas destacadas para um conjunto de tuplas ordenadas
    highlighted_edges_set = set(tuple(sorted(edge)) for edge in highlighted_edges)

    # Criar um subgrafo contendo apenas as arestas destacadas
    H = nx.MultiGraph()
    for u, v, data in G.edges(data=True):
        if tuple(sorted((u, v))) in highlighted_edges_set:
            H.add_edge(u, v, **data)

    # Extrair posições para plotagem
    pos = nx.get_node_attributes(G, 'pos')

    # Extrair regiões para os nós do subgrafo H
    regions = nx.get_node_attributes(G, 'region')
    color_map = {"51000": "blue", "52000": "green", "53000": "orange", "54000": "purple"}
    H_node_colors = [color_map[regions[node]] for node in H.nodes()]

    # Desenhar as arestas destacadas
    plt.figure(figsize=(12, 12))
    nx.draw_networkx_edges(H, pos, edge_color='red', width=2)
    
    # Desenhar os nós do subgrafo
    nx.draw_networkx_nodes(H, pos, node_color=H_node_colors, node_size=100)
    
    # Desenhar os rótulos dos nós
    nx.draw_networkx_labels(H, pos, font_size=6, font_weight='bold')

    # Criar legendas manualmente
    from matplotlib.lines import Line2D
    legend_elements = [
        Line2D([0], [0], color='red', lw=2, label='Arestas Destacadas'),
        Line2D([0], [0], marker='o', color='w', label='Região 1', markerfacecolor='blue', markersize=10),
        Line2D([0], [0], marker='o', color='w', label='Região 2', markerfacecolor='green', markersize=10),
        Line2D([0], [0], marker='o', color='w', label='Região 3', markerfacecolor='orange', markersize=10),
        Line2D([0], [0], marker='o', color='w', label='Região 4', markerfacecolor='purple', markersize=10),
    ]
    plt.legend(handles=legend_elements, loc='upper right')

    # Configurar o título e mostrar o gráfico
    plt.title("Arestas Destacadas no City Graph")
    plt.axis('off')  # Desligar os eixos para melhor visualização
    plt.show()

def plot_graph(G, node_colors):
    # Extrair posições para plotagem
    pos = nx.get_node_attributes(G, 'pos')

    # Desenhar o grafo com as cores dos nós
    plt.figure(figsize=(10, 10))
    nx.draw(
        G, pos, with_labels=True, node_size=100, node_color=node_colors,
        font_size=6, font_weight='bold'
    )
    plt.title("City Graph - Nós Coloridos por Região")
    plt.show()

if __name__ == '__main__':
    # Parâmetros do grafo
    N, M = 24, 16  # Dimensões do grid
    
    # Carregar o grafo gerado
    with open('city_graph.json', 'r', encoding='utf-8') as f:
        graph = json.load(f)

    with open('station_edges.txt', 'r', encoding='utf-8') as f:
        file_content = f.read()
        highlighted_edges = [('node_0_0','node_0_1'),('node_0_1','node_1_1'),('node_1_1','node_1_2'),('node_1_2','node_1_3'),('node_1_3','node_2_3'),('node_2_3','node_2_4'),('node_2_4','node_3_4'),('node_3_4','node_3_2'),('node_3_2','node_3_1'),('node_3_1','node_4_1'),('node_4_1','node_4_0'),('node_4_0','node_5_0'),('node_5_0','node_5_3'),('node_5_3','node_6_3'),('node_6_3','node_6_5'),('node_6_5','node_6_6'),('node_6_6','node_6_7'),('node_6_7','node_6_8'),('node_6_8','node_6_9'),('node_6_9','node_7_9'),('node_7_9','node_8_9'),('node_8_9','node_8_10'),('node_8_10','node_7_10'),('node_7_10','node_7_11'),('node_7_11','node_7_12'),('node_7_12','node_7_13'),('node_7_13','node_8_13'),('node_8_13','node_8_11'),('node_8_11','node_9_11'),('node_9_11','node_10_11'),('node_10_11','node_10_9'),('node_10_9','node_10_8'),('node_10_8','node_10_6'),('node_10_6','node_11_6'),('node_11_6','node_11_5'),('node_11_5','node_11_3'),('node_11_3','node_11_1'),('node_11_3','node_11_1'),('node_11_5','node_11_3'),('node_11_6','node_11_5'),('node_10_6','node_11_6'),('node_10_4','node_10_6'),('node_9_4','node_10_4'),('node_9_3','node_9_4'),('node_8_3','node_9_3'),('node_7_3','node_8_3'),('node_7_5','node_7_3'),('node_6_5','node_7_5'),('node_5_5','node_6_5'),('node_4_5','node_5_5'),('node_3_5','node_4_5'),('node_2_5','node_3_5'),('node_2_4','node_2_5'),('node_2_3','node_2_4'),('node_1_3','node_2_3'),('node_1_2','node_1_3'),('node_1_1','node_1_2'),('node_0_1','node_1_1'),('node_0_0','node_0_1'),]
        
    # Criar o grafo NetworkX
    G, node_colors = create_graph(graph, N, M)
    
    # Definir as arestas a serem destacadas
    highlighted_edges_set = set(tuple(sorted(edge)) for edge in highlighted_edges)

    # Verificar quais arestas destacadas realmente existem no grafo
    existing_highlighted_edges = set()
    for edge in highlighted_edges_set:
        if G.has_edge(*edge):
            existing_highlighted_edges.add(edge)
        else:
            print(f"Aresta {edge} não existe no grafo e não pode ser destacada.")

    if not existing_highlighted_edges:
        print("Nenhuma das arestas destacadas existe no grafo.")
    else:
        plot_graph(G, node_colors)
        # Plotar o grafo com apenas as arestas destacadas
        plot_highlighted_edges(G, node_colors, existing_highlighted_edges)
