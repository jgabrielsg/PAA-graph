#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <thread>
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

    // Adiciona uma aresta ao grafo
    void adicionar_aresta(const string& origem, const string& destino, double tempo) {
        grafo[origem].push_back({destino, tempo});
        grafo[destino].push_back({origem, tempo});
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

        // Encontrando o tempo para percorrer essa rua
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

    // Adicionando ruas (arestas) ao grafo
    cidade.adicionar_aresta("A", "B", 1);  // Rua de A para B com tempo de 1 segundo
    cidade.adicionar_aresta("B", "C", 1);  // Rua de B para C com tempo de 4 segundos
    cidade.adicionar_aresta("C", "D", 1);  // Rua de C para D com tempo de 4 segundos
    cidade.adicionar_aresta("X", "Y", 4);  // Rua de X para Y com tempo de 1 segundo
    cidade.adicionar_aresta("Y", "Z", 10); // Rua de Y para Z com tempo de 10 segundos

    // Criando pessoas e passando o caminho completo
    Pessoa pessoa1("Fulano", {"A", "B", "C", "D", "C", "B", "A"});  // Fulano começa em "A" e quer ir até "D"
    Pessoa pessoa2("Ciclano", {"X", "Y", "Z"});  // Ciclano começa em "X" e quer ir até "Z"

    // Simulando a movimentação das pessoas
    vector<Pessoa> pessoas = {pessoa1, pessoa2};
    cidade.simular_pessoas(pessoas);

    return 0; 
}
