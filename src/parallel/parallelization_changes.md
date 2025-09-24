As seguintes alterações foram feitas para paralelizar o programa.

### 1. Paralelização do treinamento de árvore da floresta

A primeira e mais óbvia medida foi paralelizar o treinamento de cada árvore da floresta. Essa medida é facilitada pelo fato de que o treinamento de cada árvore é totalmente independente dos outros.

**Diretivas utilizadas:**
```c
#pragma omp parallel for schedule(static)
#pragma omp atomic capture
#pragma omp critical
```

*Localização: Função train_random_forest no arquivo random_forest.c*


### 2. (scrapped) Paralelização do algoritmo de ordenação

Primeiramente, o bubble sort presente dentro da função find_best_split foi substituido por um merge sort, mas este não foi paralelizado pois o algoritmo de ordenação é chamado muitas vezes com vetores pequenos, o que não justifica o overhead de paralelização.

### 3. Paralelização da busca pelo melhor split

O laço que itera sobre todos os potenciais splits foi paralelizado usando uma região paralela com distribuição de trabalho estática.
Cada thread trabalha em uma porção dos potenciais splits de forma independente, mantendo variáveis locais para o melhor resultado. Ao final, usa uma seção crítica para atualizar o resultado global.

**Diretivas utilizadas:**
```c
#pragma omp parallel
#pragma omp for nowait schedule(static)
#pragma omp critical
```

*Localização: Função find_best_split no arquivo decision_tree.c*


### 4. Paralelização da predição das árvores em cada floresta

O processo de predição foi paralelizado distribuindo as árvores entre as threads disponíveis.
Cada thread processa um subconjunto das árvores da floresta independentemente, obtendo suas predições em paralelo.

**Diretivas utilizadas:**
```c
#pragma omp parallel for schedule(static)
```

*Localização: Função predict_random_forest no arquivo random_forest.c*


### 5. Paralelização da avaliação da precisão

A avaliação da precisão foi paralelizada processando múltiplas amostras de teste simultaneamente.
Utiliza uma cláusula de redução para somar thread-safely o número de predições corretas de todas as threads.

**Diretivas utilizadas:**
```c
#pragma omp parallel for reduction(+:correct_predictions)
```

*Localização: Função evaluate_accuracy no arquivo random_forest.c*
