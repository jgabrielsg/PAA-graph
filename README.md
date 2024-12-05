# Simulação da Cidade e Otimização do Transporte

Na nossa simulação de cidade, os vértices representam cruzamentos e as arestas, as ruas. A cidade é modelada por um grid 2D com dimensões MxN, onde cada rua tem 5 edifícios em cada lado. Para cada aresta, armazenamos informações como o tipo de transporte (ex: táxi), a velocidade máxima, a distância, o custo e o tempo de viagem, o custo de escavação, a quantidade de edifícios residenciais, comerciais, industriais e turísticos, além da preferência de ônibus.

Esses dados são exportados em **JSON** para serem lidos em **C++**, usando a biblioteca **JSON for Modern C++** para construir o grafo da cidade.

O objetivo do código é **minimizar a maior distância entre os vértices** de cada região, encontrando os pontos centrais. Usamos o algoritmo de **Dijkstra** para calcular a distância entre os vértices e escolher o que tem a menor "máxima distância". Em seguida, aplicamos **Kruskal** para criar a rede de metrô, conectando as estações da forma mais eficiente possível.

### **Design da Rota de Ônibus:**
A rota de ônibus é otimizada com o conceito de **Ciclo Hamiltoniano**, que conecta todos os pontos da cidade minimizando a distância percorrida. Para calcular a melhor rota, o peso das arestas é ajustado com base no tipo de edifício ao longo das ruas, priorizando áreas comerciais e turísticas.

O algoritmo de **Dijkstra** é usado para otimizar a distância entre os vértices e melhorar a conectividade entre áreas-chave, focando na **eficiência** do transporte e redução do tempo de viagem, ao mesmo tempo em que considera a sustentabilidade ao minimizar o impacto ambiental e o congestionamento.

### **Otimização de Trajetos e Custos:**
Além disso, o código também otimiza trajetos com base no tempo e no custo. Utilizando uma fila de prioridade, o algoritmo calcula o melhor caminho entre dois vértices, levando em conta não apenas o tempo de viagem, mas também o custo, respeitando um limite máximo de dinheiro \( K \). Isso é feito com base no tipo de transporte (metrô, ônibus, táxi, caminhada), garantindo que os recursos sejam usados de forma otimizada.

---

### **Instruções para Rodar o Código:**

1. **Baixe um compilador de C++:** 
   Para compilar o código, é necessário ter um compilador C++ instalado. Recomendamos o **MSYS2**, que é uma plataforma de desenvolvimento que fornece ferramentas de compilação, como o `g++`.

2. **Instale o MSYS2:**
   - Baixe o MSYS2 em: [https://www.msys2.org](https://www.msys2.org).
   - Siga as instruções de instalação para o seu sistema operacional.

3. **Compile os Arquivos:**
   Após instalar o MSYS2, abra o terminal do MSYS2 e navegue até o diretório onde os arquivos do projeto estão localizados. Execute o seguinte comando para compilar todos os arquivos e gerar o executável:

   ```bash
   g++ -std=c++17 main.cpp Graph.cpp dataStructures.cpp newMetro.cpp bus.cpp bus3.cpp fastestRouteQ3.cpp -o main
