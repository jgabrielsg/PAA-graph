import json
import random
import matplotlib.pyplot as plt
import networkx as nx

# Função para gerar um imóvel com probabilidade aleatória para tipo
def generate_property(graph, cep, street_name, node_from, node_to):
    property_counter = 0
    # Gerar imóveis para a rua
    for side in ["left", "right"]:
        for k in range(1, 6):
            if side == "left":
                number = property_counter
            else:
                number = property_counter + 5
            property_types = ['residential', 'commercial', 'industrial', 'touristic']
            property_type = random.choice(property_types)  # Escolhe aleatoriamente o tipo do imóvel

            graph['properties'].append({
                "cep": cep,
                "street": street_name,
                "number": number,
                "type": property_type,
                "from": node_from,
                "to": node_to
            })
    return graph

# Função principal para criar o grafo com propriedades e regiões, incluindo aleatoriedade nos vértices
def create_grid_graph_with_random_vertices(N, M, street_length=200, max_speed=15, taxi_rate=4, non_motorized_speed=1.5):
    """
    Cria um grafo em formato de grid N x M com ruas, cruzamentos, arestas e imóveis,
    divide o grid em 4 regiões com CEPs diferentes e adiciona aleatoriedade aos vértices.
    
    :param N: Número de ruas horizontais.
    :param M: Número de ruas verticais.
    :param street_length: Comprimento padrão de cada rua (em metros).
    :param max_speed: Velocidade máxima das ruas (em metros por segundo).
    :param taxi_rate: Custo por quilômetro de táxi (em reais).
    :param non_motorized_speed: Velocidade do transporte não motorizado (em metros por segundo).
    :return: Um dicionário com a representação do grafo.
    """
    # Carregar os nomes das ruas do arquivo JSON
    with open('ruas.json', 'r', encoding='utf-8') as file:
        ruas_data = json.load(file)
    
    rua_names = ruas_data['ruas']
    used_ruas = set()  # Para evitar repetição de nomes

    # Função para obter nome único de rua
    def get_unique_street_name():
        available_names = [name for name in rua_names if name not in used_ruas]
        if available_names:
            street_name = random.choice(available_names)
            used_ruas.add(street_name)
            return street_name
        else:
            raise Exception("Não há nomes de ruas suficientes disponíveis!")

    graph = {
        "nodes": [],
        "edges": [],
        "properties": []  # Para armazenar os imóveis
    }

    nodes_exist = {}  # Dicionário para verificar se o nó existe

    # Gerando os nós (cruzamentos) com aleatoriedade
    for i in range(N):
        for j in range(M):
            if random.random() >= 0.3:
                node_id = f"node_{i}_{j}"
                
                # Determine the region based on (i, j)
                if i < N // 4:
                    cep = "51000"  # Região 1
                    region = "Region1"
                elif i > 3 * N // 4:
                    cep = "54000"  # Região 4
                    region = "Region4"
                elif i >= N // 4 and i <= 3 * N // 4:
                    if j < M // 2:
                        cep = "52000"  # Região 2
                        region = "Region2"
                    else:
                        cep = "53000"  # Região 3
                        region = "Region3"
                
                node_data = {
                    "id": node_id,
                    "location": (i, j),
                    "transport_options": ["taxi", "non_motorized"],
                    "region": region  # Adiciona informação da região
                }
                graph["nodes"].append(node_data)
                nodes_exist[(i, j)] = True
            else:
                nodes_exist[(i, j)] = False

    # Gerando as arestas e imóveis somente entre nós existentes
    property_counter = 1  # Contador de imóveis para manter a numeração contínua

    for i in range(N):
        for j in range(M):
            if not nodes_exist.get((i, j)):
                continue  # Pula se o nó não existir

            node_id = f"node_{i}_{j}"

            # Arestas horizontais
            if j + 1 < M:
                if nodes_exist.get((i, j + 1)):
                    neighbor_id = f"node_{i}_{j+1}"
                    street_name = get_unique_street_name()

                    # Gerar arestas bidirecionais
                    for direction in [("node_id, neighbor_id"), (neighbor_id, node_id)]:
                        # Caminhada
                        edge_data_walk = {
                            "from": direction[0],
                            "to": direction[1],
                            "transport_type": "walk",
                            "max_speed": non_motorized_speed,
                            "distance": street_length,
                            "price_cost": 0,
                            "time_cost": street_length / non_motorized_speed
                        }
                        graph["edges"].append(edge_data_walk)
                        
                        # Táxi
                        edge_data_taxi = {
                            "from": direction[0],
                            "to": direction[1],
                            "transport_type": "taxi",
                            "max_speed": max_speed,
                            "distance": street_length,
                            "price_cost": (street_length / 1000) * taxi_rate,
                            "time_cost": street_length / max_speed
                        }
                        graph["edges"].append(edge_data_taxi)
                else:
                    # Caso o vértice (i, j+1) tenha sido pulado
                    # Conectar ao vértice mais próximo à direita
                    k = j + 1
                    while k < M and not nodes_exist.get((i, k)):
                        k += 1
                    if k < M:
                        neighbor_id = f"node_{i}_{k}"
                        street_name = get_unique_street_name()

                        # Calcular a distância considerando os vértices pulados
                        skipped_vertices = k - j - 1
                        distance = street_length * (skipped_vertices + 1)

                        # Gerar aresta entre o vértice atual e o mais próximo à direita
                        edge_data_walk = {
                            "from": node_id,
                            "to": neighbor_id,
                            "transport_type": "walk",
                            "max_speed": non_motorized_speed,
                            "distance": distance,
                            "price_cost": 0,
                            "time_cost": distance / non_motorized_speed
                        }
                        graph["edges"].append(edge_data_walk)
                        
                        # Táxi
                        edge_data_taxi = {
                            "from": node_id,
                            "to": neighbor_id,
                            "transport_type": "taxi",
                            "max_speed": max_speed,
                            "distance": distance,
                            "price_cost": (distance / 1000) * taxi_rate,
                            "time_cost": distance / max_speed
                        }
                        graph["edges"].append(edge_data_taxi)

            # Arestas verticais
            if i + 1 < N and nodes_exist.get((i + 1, j)):
                neighbor_id = f"node_{i+1}_{j}"
                street_name = get_unique_street_name()

                # Gerar arestas bidirecionais
                for direction in [("node_id, neighbor_id"), (neighbor_id, node_id)]:
                    # Caminhada
                    edge_data_walk = {
                        "from": direction[0],
                        "to": direction[1],
                        "transport_type": "walk",
                        "max_speed": non_motorized_speed,
                        "distance": street_length,
                        "price_cost": 0,
                        "time_cost": street_length / non_motorized_speed
                    }
                    graph["edges"].append(edge_data_walk)
                    
                    # Táxi
                    edge_data_taxi = {
                        "from": direction[0],
                        "to": direction[1],
                        "transport_type": "taxi",
                        "max_speed": max_speed,
                        "distance": street_length,
                        "price_cost": (street_length / 1000) * taxi_rate,
                        "time_cost": street_length / max_speed
                    }
                    graph["edges"].append(edge_data_taxi)

                    # Gerar propriedades (imóveis) para essa rua
                    graph = generate_property(graph, cep, street_name, direction[1], direction[0])

    # Salvar o grafo em JSON
    with open('city_graph.json', 'w', encoding='utf-8') as f:
        json.dump(graph, f, ensure_ascii=False, indent=4)

    return graph

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

if __name__ == '__main__':
    # Parâmetros do grafo
    N, M = 24, 16  # Dimensões do grid
    
    # Carregar o grafo gerado
    with open('city_graph.json', 'r', encoding='utf-8') as f:
        graph = json.load(f)

    # Criar o grafo NetworkX
    G, node_colors = create_graph(graph, N, M)

    # Definir as arestas a serem destacadas
    highlighted_edges = [('node_11_9','node_11_11'),('node_12_4','node_12_3'),('node_1_7','node_1_6'),('node_20_7','node_20_6'),]    
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
        # Plotar o grafo com apenas as arestas destacadas
        plot_highlighted_edges(G, node_colors, existing_highlighted_edges)
