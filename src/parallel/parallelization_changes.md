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

*Localização: Função merge_sort no arquivo utils.c* *Não implementado*

### 3. (scrapped) Paralelização da construção recursiva da árvore

Teoricamente, é possível paralelizar a construção recursiva da árvore usando Tasks, mas isso implicaria em problemas de sincronização de estruturas de dados compartilhadas (a árvore). Além disso, o trabalho computacional diminui exponencialmente com a profundidade da árvore, fazendo que o overhead de sincronização seja maior que o ganho de performance.

*Localização: Função build_tree_recursive no arquivo decision_tree.c* *Não implementado*

### 4. (scrapped) Paralelização do cálculo da impureza de Gini

Determinamos que o overhead de paralelização não seria compensado pelo ganho de performance, por vários motivos: Durante a construção de cada árvore, o cálculo da impureza de Gini é chamado com conjuntos cada vez menores ao passo que a profundidade da árvore aumenta (O tamanho do conjunto de dados diminui em um fator de 2 a cada nível da árvore). Além disso, as operações envolvidas no cálculo da impureza de Gini são bastante simples (acesso a vetor, contagem e aritmética básica) e já são otimizadas a nível de hardware. Por fim, o cálculo da impureza de Gini se encontra dentro de um laço que já é paralelizado.

*Localização: Função calculate_gini_impurity no arquivo decision_tree.c* *Não implementado*


### 5. Paralelização da busca pelo melhor split

O laço que itera sobre todos os potenciais splits foi paralelizado usando uma região paralela com distribuição de trabalho estática.
Cada thread trabalha em uma porção dos potenciais splits de forma independente, mantendo variáveis locais para o melhor resultado. Ao final, usa uma seção crítica para atualizar o resultado global.

**Diretivas utilizadas:**
```c
#pragma omp parallel
#pragma omp for nowait schedule(static)
#pragma omp critical
```

*Localização: Função find_best_split no arquivo decision_tree.c*


### 6. Paralelização da predição das árvores em cada floresta

O processo de predição foi paralelizado distribuindo as árvores entre as threads disponíveis.
Cada thread processa um subconjunto das árvores da floresta independentemente, obtendo suas predições em paralelo.

**Diretivas utilizadas:**
```c
#pragma omp parallel for schedule(static)
```

*Localização: Função predict_random_forest no arquivo random_forest.c*


### 7. Paralelização da avaliação da precisão

A avaliação da precisão foi paralelizada processando múltiplas amostras de teste simultaneamente.
Utiliza uma cláusula de redução para somar thread-safely o número de predições corretas de todas as threads.

**Diretivas utilizadas:**
```c
#pragma omp parallel for reduction(+:correct_predictions)
```

*Localização: Função evaluate_accuracy no arquivo random_forest.c*
