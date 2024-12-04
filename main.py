import json
import random
import matplotlib.pyplot as plt
import networkx as nx

# Função para gerar um imóvel com probabilidade aleatória para tipo
def generate_property(graph, cep, street_name, node_from, node_to, weights, number, r_count, c_count, i_count, t_count):
    property_types = ['residential', 'commercial', 'industrial', 'touristic']
    # Escolher o tipo de imóvel com base nos pesos
    property_type = random.choices(property_types, weights=weights, k=1)[0]  # k=1 retorna uma lista de um item, então pegamos o primeiro
    
    if property_type == "residential":
        r_count += 1
    if property_type == "commercial":
        c_count += 1
    if property_type == "industrial":
        i_count += 1
    if property_type == "touristic":
        t_count += 1

    graph['properties'].append( {
        "cep": cep,
        "street": street_name,
        "number": number,
        "type": property_type,
        "from": node_from,
        "to": node_to
    })

    return graph, r_count, c_count, i_count, t_count

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
                node_data = {
                    "id": node_id,
                    "location": (i, j),
                    "transport_options": ["taxi", "non_motorized"],  # Táxi e não motorizado
                }
                graph["nodes"].append(node_data)
                nodes_exist[(i, j)] = True  # Marca o nó como existente
            else:
                nodes_exist[(i, j)] = False  # Marca o nó como inexistente


    for i in range(N):
        street_name_vertical = get_unique_street_name()
        for j in range(M):
            street_name = get_unique_street_name()
            if not nodes_exist.get((i, j)):
                continue  # Pula se o nó não existir

            node_id = f"node_{i}_{j}"

            # Determinar a região e o CEP de acordo com a posição (i, j)
            if i < N // 4:
                cep = "51000"  # Região 1
            elif i > 3 * N // 4:
                cep = "54000"  # Região 4
            elif i >= N // 4 and i <= 3 * N // 4:
                if j < M // 2:
                    cep = "52000"  # Região 2
                else:
                    cep = "53000"  # Região 3

            # Arestas verticais
            if j + 1 < M:
                if nodes_exist.get((i, j + 1)):
                    neighbor_id = f"node_{i}_{j+1}"

                    # Gerar arestas bidirecionais
                    mult_rand = random.choice([0.6, 0.8, 1, 1.2, 1.4])
                    # Aresta de caminhada na direção (node_id -> neighbor_id)
                    edge_data_walk_1 = {
                        "from": node_id,
                        "to": neighbor_id,
                        "transport_type": "walk",
                        "max_speed": non_motorized_speed,
                        "distance": street_length,
                        "price_cost": 0,
                        "time_cost": street_length / non_motorized_speed,
                        "excavation_cost": street_length * mult_rand,
                        "num_residencial": 0,
                        "num_commercial": 0,
                        "num_touristic": 0,
                        "num_industrial": 0
                    }

                    # Aresta de caminhada na direção (neighbor_id -> node_id)
                    edge_data_walk_2 = {
                        "from": neighbor_id,
                        "to": node_id,
                        "transport_type": "walk",
                        "max_speed": non_motorized_speed,
                        "distance": street_length,
                        "price_cost": 0,
                        "time_cost": street_length / non_motorized_speed,
                        "excavation_cost": street_length * mult_rand,
                        "num_residencial": 0,
                        "num_commercial": 0,
                        "num_touristic": 0,
                        "num_industrial": 0
                    }

                    # Aresta de táxi na direção (node_id -> neighbor_id)
                    edge_data_taxi_1 = {
                        "from": node_id,
                        "to": neighbor_id,
                        "transport_type": "taxi",
                        "max_speed": max_speed,
                        "distance": street_length,
                        "price_cost": (street_length / 1000) * taxi_rate,
                        "time_cost": street_length / max_speed,
                        "excavation_cost": street_length * mult_rand,
                        "num_residencial": 0,
                        "num_commercial": 0,
                        "num_touristic": 0,
                        "num_industrial": 0
                    }

                    # Aresta de táxi na direção (neighbor_id -> node_id)
                    edge_data_taxi_2 = {
                        "from": neighbor_id,
                        "to": node_id,
                        "transport_type": "taxi",
                        "max_speed": max_speed,
                        "distance": street_length,
                        "price_cost": (street_length / 1000) * taxi_rate,
                        "time_cost": street_length / max_speed,
                        "excavation_cost": street_length * mult_rand,
                        "num_residencial": 0,
                        "num_commercial": 0,
                        "num_touristic": 0,
                        "num_industrial": 0
                    }


                    if cep == "51000":
                        weights = [0.9, 0.05, 0.04, 0.01]   # Região 1 é mais residencial
                    elif cep == "52000":
                        weights = [0.60, 0.35, 0.01, 0.04]  # Região 2 é mais comercial e com atrações
                    elif cep == "53000":
                        weights = [0.25, 0.05, 0.69, 0.01]  # Região 3 mais industrial
                    elif cep == "54000":
                        weights = [0.9, 0.05, 0.04, 0.01]   # Região 4 é mais residencial
                    
                    r_count, c_count, i_count, t_count = 0, 0, 0, 0
                    r_count2, c_count2, i_count2, t_count2 = 0, 0, 0, 0
                    # Gerar 5 propriedades para a aresta
                    for k in range(5):
                        # Propriedades do lado "de"
                        graph, r_count, c_count, i_count, t_count = generate_property(graph, cep, street_name_vertical, node_id, neighbor_id, weights, 10*j + 2*k, r_count, c_count, i_count, t_count)
                        # Propriedades do lado "para"
                        graph, r_count2, c_count2, i_count2, t_count2 = generate_property(graph, cep, street_name_vertical, neighbor_id, node_id, weights, 10*j + 2*k + 1, r_count2, c_count2, i_count2, t_count2)
                    
                    edge_data_walk_1["num_residencial"] = r_count
                    edge_data_walk_1["num_commercial"] = c_count
                    edge_data_walk_1["num_touristic"] = t_count
                    edge_data_walk_1["num_industrial"] = i_count
                    
                    edge_data_walk_2["num_residencial"] = r_count2
                    edge_data_walk_2["num_commercial"] = c_count2
                    edge_data_walk_2["num_touristic"] = t_count2
                    edge_data_walk_2["num_industrial"] = i_count2
                    
                    edge_data_taxi_1["num_residencial"] = r_count
                    edge_data_taxi_1["num_commercial"] = c_count
                    edge_data_taxi_1["num_touristic"] = t_count
                    edge_data_taxi_1["num_industrial"] = i_count
                    
                    edge_data_taxi_2["num_residencial"] = r_count2
                    edge_data_taxi_2["num_commercial"] = c_count2
                    edge_data_taxi_2["num_touristic"] = t_count2
                    edge_data_taxi_2["num_industrial"] = i_count2
                    
                    graph["edges"].append(edge_data_walk_1)
                    graph["edges"].append(edge_data_walk_2)
                    graph["edges"].append(edge_data_taxi_1)
                    graph["edges"].append(edge_data_taxi_2)

                else:
                    # Caso o vértice (i, j+1) tenha sido pulado conectar ao vértice mais próximo para cima
                    k = j + 1
                    while k < M and not nodes_exist.get((i, k)):
                        k += 1
                    if k < M:
                        neighbor_id = f"node_{i}_{k}"
                        
                        # Calcular a distância considerando os vértices pulados
                        skipped_vertices = k - j - 1
                        distance = street_length * (skipped_vertices + 1)

                        # Gerar arestas bidirecionais
                        mult_rand = random.choice([0.6, 0.8, 1, 1.2, 1.4])
                        # Aresta de caminhada na direção (node_id -> neighbor_id)
                        edge_data_walk_1 = {
                            "from": node_id,
                            "to": neighbor_id,
                            "transport_type": "walk",
                            "max_speed": non_motorized_speed,
                            "distance": distance,
                            "price_cost": 0,
                            "time_cost": distance / non_motorized_speed,
                            "excavation_cost": distance * mult_rand,
                            "num_residencial": 0,
                            "num_commercial": 0,
                            "num_touristic": 0,
                            "num_industrial": 0
                        }

                        # Aresta de caminhada na direção (neighbor_id -> node_id)
                        edge_data_walk_2 = {
                            "from": neighbor_id,
                            "to": node_id,
                            "transport_type": "walk",
                            "max_speed": non_motorized_speed,
                            "distance": distance,
                            "price_cost": 0,
                            "time_cost": distance / non_motorized_speed,
                            "excavation_cost": distance * mult_rand,
                            "num_residencial": 0,
                            "num_commercial": 0,
                            "num_touristic": 0,
                            "num_industrial": 0
                        }

                        # Aresta de táxi na direção (node_id -> neighbor_id)
                        edge_data_taxi_1 = {
                            "from": node_id,
                            "to": neighbor_id,
                            "transport_type": "taxi",
                            "max_speed": max_speed,
                            "distance": distance,
                            "price_cost": (distance / 1000) * taxi_rate,
                            "time_cost": distance / max_speed,
                            "excavation_cost": distance * mult_rand,
                            "num_residencial": 0,
                            "num_commercial": 0,
                            "num_touristic": 0,
                            "num_industrial": 0
                        }

                        # Aresta de táxi na direção (neighbor_id -> node_id)
                        edge_data_taxi_2 = {
                            "from": neighbor_id,
                            "to": node_id,
                            "transport_type": "taxi",
                            "max_speed": max_speed,
                            "distance": distance,
                            "price_cost": (distance / 1000) * taxi_rate,
                            "time_cost": distance / max_speed,
                            "excavation_cost": distance * mult_rand,
                            "num_residencial": 0,
                            "num_commercial": 0,
                            "num_touristic": 0,
                            "num_industrial": 0
                        }


                        if cep == "51000":
                            weights = [0.9, 0.05, 0.04, 0.01]   # Região 1 é mais residencial
                        elif cep == "52000":
                            weights = [0.60, 0.35, 0.01, 0.04]  # Região 2 é mais comercial e com atrações
                        elif cep == "53000":
                            weights = [0.25, 0.05, 0.69, 0.01]  # Região 3 mais industrial
                        elif cep == "54000":
                            weights = [0.9, 0.05, 0.04, 0.01]   # Região 4 é mais residencial
                        
                        r_count, c_count, i_count, t_count = 0, 0, 0, 0
                        r_count2, c_count2, i_count2, t_count2 = 0, 0, 0, 0
                        # Gerar 5 propriedades para a aresta
                        for k in range(5):
                            # Propriedades do lado "de"
                            graph, r_count, c_count, i_count, t_count = generate_property(graph, cep, street_name_vertical, node_id, neighbor_id, weights, 10*j + 2*k, r_count, c_count, i_count, t_count)
                            # Propriedades do lado "para"
                            graph, r_count2, c_count2, i_count2, t_count2 = generate_property(graph, cep, street_name_vertical, neighbor_id, node_id, weights, 10*j + 2*k + 1, r_count2, c_count2, i_count2, t_count2)
                        
                        edge_data_walk_1["num_residencial"] = r_count
                        edge_data_walk_1["num_commercial"] = c_count
                        edge_data_walk_1["num_touristic"] = t_count
                        edge_data_walk_1["num_industrial"] = i_count
                        
                        edge_data_walk_2["num_residencial"] = r_count2
                        edge_data_walk_2["num_commercial"] = c_count2
                        edge_data_walk_2["num_touristic"] = t_count2
                        edge_data_walk_2["num_industrial"] = i_count2
                        
                        edge_data_taxi_1["num_residencial"] = r_count
                        edge_data_taxi_1["num_commercial"] = c_count
                        edge_data_taxi_1["num_touristic"] = t_count
                        edge_data_taxi_1["num_industrial"] = i_count
                        
                        edge_data_taxi_2["num_residencial"] = r_count2
                        edge_data_taxi_2["num_commercial"] = c_count2
                        edge_data_taxi_2["num_touristic"] = t_count2
                        edge_data_taxi_2["num_industrial"] = i_count2
                        
                        graph["edges"].append(edge_data_walk_1)
                        graph["edges"].append(edge_data_walk_2)
                        graph["edges"].append(edge_data_taxi_1)
                        graph["edges"].append(edge_data_taxi_2)

            # Arestas horizontais
            if i + 1 < N and nodes_exist.get((i + 1, j)):  # Só cria se tiver uma ao lado
                neighbor_id = f"node_{i+1}_{j}"

                # Gerar arestas bidirecionais
                mult_rand = random.choice([0.6, 0.8, 1, 1.2, 1.4])
                # Aresta de caminhada na direção (node_id -> neighbor_id)
                edge_data_walk_1 = {
                    "from": node_id,
                    "to": neighbor_id,
                    "transport_type": "walk",
                    "max_speed": non_motorized_speed,
                    "distance": street_length,
                    "price_cost": 0,
                    "time_cost": street_length / non_motorized_speed,
                    "excavation_cost": street_length * mult_rand,
                    "num_residencial": 0,
                    "num_commercial": 0,
                    "num_touristic": 0,
                    "num_industrial": 0
                }

                # Aresta de caminhada na direção (neighbor_id -> node_id)
                edge_data_walk_2 = {
                    "from": neighbor_id,
                    "to": node_id,
                    "transport_type": "walk",
                    "max_speed": non_motorized_speed,
                    "distance": street_length,
                    "price_cost": 0,
                    "time_cost": street_length / non_motorized_speed,
                    "excavation_cost": street_length * mult_rand,
                    "num_residencial": 0,
                    "num_commercial": 0,
                    "num_touristic": 0,
                    "num_industrial": 0
                }

                # Aresta de táxi na direção (node_id -> neighbor_id)
                edge_data_taxi_1 = {
                    "from": node_id,
                    "to": neighbor_id,
                    "transport_type": "taxi",
                    "max_speed": max_speed,
                    "distance": street_length,
                    "price_cost": (street_length / 1000) * taxi_rate,
                    "time_cost": street_length / max_speed,
                    "excavation_cost": street_length * mult_rand,
                    "num_residencial": 0,
                    "num_commercial": 0,
                    "num_touristic": 0,
                    "num_industrial": 0
                }

                # Aresta de táxi na direção (neighbor_id -> node_id)
                edge_data_taxi_2 = {
                    "from": neighbor_id,
                    "to": node_id,
                    "transport_type": "taxi",
                    "max_speed": max_speed,
                    "distance": street_length,
                    "price_cost": (street_length / 1000) * taxi_rate,
                    "time_cost": street_length / max_speed,
                    "excavation_cost": street_length * mult_rand,
                    "num_residencial": 0,
                    "num_commercial": 0,
                    "num_touristic": 0,
                    "num_industrial": 0
                }


                if cep == "51000":
                    weights = [0.9, 0.05, 0.04, 0.01]   # Região 1 é mais residencial
                elif cep == "52000":
                    weights = [0.60, 0.35, 0.01, 0.04]  # Região 2 é mais comercial e com atrações
                elif cep == "53000":
                    weights = [0.25, 0.05, 0.69, 0.01]  # Região 3 mais industrial
                elif cep == "54000":
                    weights = [0.9, 0.05, 0.04, 0.01]   # Região 4 é mais residencial
                
                r_count, c_count, i_count, t_count = 0, 0, 0, 0
                r_count2, c_count2, i_count2, t_count2 = 0, 0, 0, 0
                # Gerar 5 propriedades para a aresta
                for k in range(5):
                    # Propriedades do lado "de"
                    graph, r_count, c_count, i_count, t_count = generate_property(graph, cep, street_name_vertical, node_id, neighbor_id, weights, 10*j + 2*k, r_count, c_count, i_count, t_count)
                    # Propriedades do lado "para"
                    graph, r_count2, c_count2, i_count2, t_count2 = generate_property(graph, cep, street_name_vertical, neighbor_id, node_id, weights, 10*j + 2*k + 1, r_count2, c_count2, i_count2, t_count2)
                
                edge_data_walk_1["num_residencial"] = r_count
                edge_data_walk_1["num_commercial"] = c_count
                edge_data_walk_1["num_touristic"] = t_count
                edge_data_walk_1["num_industrial"] = i_count
                
                edge_data_walk_2["num_residencial"] = r_count2
                edge_data_walk_2["num_commercial"] = c_count2
                edge_data_walk_2["num_touristic"] = t_count2
                edge_data_walk_2["num_industrial"] = i_count2
                
                edge_data_taxi_1["num_residencial"] = r_count
                edge_data_taxi_1["num_commercial"] = c_count
                edge_data_taxi_1["num_touristic"] = t_count
                edge_data_taxi_1["num_industrial"] = i_count
                
                edge_data_taxi_2["num_residencial"] = r_count2
                edge_data_taxi_2["num_commercial"] = c_count2
                edge_data_taxi_2["num_touristic"] = t_count2
                edge_data_taxi_2["num_industrial"] = i_count2
                
                graph["edges"].append(edge_data_walk_1)
                graph["edges"].append(edge_data_walk_2)
                graph["edges"].append(edge_data_taxi_1)
                graph["edges"].append(edge_data_taxi_2)

    # Salvar o grafo em JSON
    with open('city_graph.json', 'w', encoding='utf-8') as f:
        json.dump(graph, f, ensure_ascii=False, indent=4)

    return graph

# Função para plotar o grafo
def create_graph(graph):
    G = nx.Graph()

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

    # Gerar o grafo
    graph = create_grid_graph_with_random_vertices(N, M)

    graph, node_colors = create_graph(graph)

    # Plotar o grafo
    plot_graph(graph, node_colors)