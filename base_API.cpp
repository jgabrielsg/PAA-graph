#include "Graph.h"  // Supondo que a classe Graph esteja aqui
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <thread>
#include <random>
#include <future>  // Para usar std::future

using namespace std;

// Função de hash personalizada para std::pair<string, string>
struct hash_pair {
    template <typename T1, typename T2>
    size_t operator() (const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;  // Combina os hashes dos dois elementos da pair
    }
};

// Declaração antecipada da classe Pessoa
class Cidade;

class Pessoa {
public:
    string nome;
    string cruzamento_atual;
    string destino;
    vector<string> caminho;
    double tempo_total;

    // Construtor
    Pessoa(string nome, const vector<string>& caminho)
        : nome(nome), caminho(caminho), tempo_total(0) {
        cruzamento_atual = caminho[0]; // A pessoa começa no primeiro cruzamento
    }

    void mover(Cidade& cidade);  // Declaração antecipada da função mover

    void exibir_status() {
        cout << "Pessoa: " << nome << endl;
        cout << "Cruzamento Atual: " << cruzamento_atual << endl;
        cout << "Tempo Total: " << tempo_total << " segundos" << endl;
        cout << "Caminho percorrido: ";
        for (auto& p : caminho) {
            cout << p << " ";
        }
        cout << endl; 
    }
};

// Classe Cidade
class Cidade {
public:
    unordered_map<string, vector<pair<string, double>>> grafo;  // Mapeia cada cruzamento para os vizinhos e tempo

    // Carrega o grafo a partir de um arquivo JSON
    bool carregar_grafo_de_json(const string& arquivo_json) {
        Graph graph;
        if (!graph.loadFromJson(arquivo_json)) {
            return false; // Se não conseguir carregar o grafo, retorna false
        }

        // Preenche o grafo da cidade com os dados do grafo carregado
        for (const auto& node : graph.getNodes()) {
            // Adiciona as arestas de cada nó
            for (const auto& edge : graph.getEdges()) {
                if (edge.from == node.id) {
                    // Adiciona a aresta com o tempo de deslocamento (time_cost)
                    grafo[edge.from].push_back({edge.to, edge.time_cost/10});
                }
            }
        }

        return true;
    }

    // Função auxiliar para criar um caminho aleatório
    vector<string> gerar_caminho_aleatorio(int n) {
        vector<string> caminho;
        vector<string> nodes;
        // Preenche o vetor com todos os nós do grafo
        for (const auto& pair : grafo) {
            nodes.push_back(pair.first);
        }

        // Gerando um caminho aleatório
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, nodes.size() - 1);

        string node_atual = nodes[dis(gen)]; // Pega um nó aleatório para começar
        caminho.push_back(node_atual);

        for (int i = 0; i < n - 1; ++i) {
            // Seleciona um vizinho aleatório para o próximo nó
            auto& vizinhos = grafo[node_atual];
            uniform_int_distribution<> dis_vizinho(0, vizinhos.size() - 1);
            int vizinho_index = dis_vizinho(gen);
            node_atual = vizinhos[vizinho_index].first;  // O próximo cruzamento
            caminho.push_back(node_atual);
        }

        return caminho;
    }

    // Função para exibir o caminho aleatório gerado
    void exibir_caminho_aleatorio(const Pessoa& pessoa) {
        cout << "Caminho aleatorio gerado para " << pessoa.nome << ": ";
        for (const auto& cruzamento : pessoa.caminho) {
            cout << cruzamento << " ";
        }
        cout << endl;
    }

    // Atualiza a posição de uma pessoa e imprime o evento
    void atualizar_posicao_pessoa(const string& nome_pessoa, const string& cruzamento, double tempo_decorrido) {
        cout << nome_pessoa << " - " << cruzamento << " - " << tempo_decorrido << endl;
    }

    // Simula o movimento de todas as pessoas
    void simular_pessoas(vector<Pessoa>& pessoas) {
        vector<future<void>> futures;

        // Cria uma thread para cada pessoa
        for (int i = 0; i < pessoas.size(); ++i) {
            futures.push_back(std::async(launch::async, &Pessoa::mover, &pessoas[i], ref(*this)));
        }

        // Espera todas as threads terminarem
        for (auto& fut : futures) {
            fut.get();  // Espera todas as threads terminarem
        }
    }
};

// Definindo a função mover
void Pessoa::mover(Cidade& cidade) {
    // A pessoa se move seguindo o caminho que ela tem
    for (size_t i = 0; i < caminho.size() - 1; ++i) {
        string cruzamento_atual = caminho[i];
        string proximo_cruzamento = caminho[i + 1];

        // Encontrando o tempo para percorrer essa rua usando time_cost
        double tempo_rua = 0;
        for (auto& vizinho : cidade.grafo[cruzamento_atual]) {
            if (vizinho.first == proximo_cruzamento) {
                tempo_rua = vizinho.second;
                break;
            }
        }

        // Simulando o tempo de deslocamento
        std::this_thread::sleep_for(std::chrono::seconds((int)tempo_rua));  // Pausa apenas essa thread
        
        // Atualizando o tempo total e o cruzamento atual após o tempo de movimento
        tempo_total += tempo_rua;

        // Notificando a cidade sobre a chegada da pessoa no novo cruzamento
        cidade.atualizar_posicao_pessoa(nome, proximo_cruzamento, tempo_total);
    }

    // Após percorrer todo o caminho, a pessoa chega ao destino
    exibir_status();
}

int main() {
    Cidade cidade;

    // Carrega o grafo do arquivo JSON
    if (!cidade.carregar_grafo_de_json("city_graph.json")) {
        cerr << "Erro ao carregar o grafo do arquivo JSON!" << endl;
        return 1;
    }

    // Criando pessoas e atribuindo caminhos aleatórios
    Pessoa pessoa1("Fulano", cidade.gerar_caminho_aleatorio(5));  // Caminho aleatório de tamanho 5
    Pessoa pessoa2("Ciclano", cidade.gerar_caminho_aleatorio(5));  // Caminho aleatório de tamanho 5

    // Exibindo os caminhos aleatórios
    cidade.exibir_caminho_aleatorio(pessoa1);
    cidade.exibir_caminho_aleatorio(pessoa2);

    // Simulando a movimentação das pessoas
    vector<Pessoa> pessoas = {pessoa1, pessoa2};
    cidade.simular_pessoas(pessoas);

    return 0; 
}