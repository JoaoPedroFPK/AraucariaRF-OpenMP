# Trabalho Prático – Programação Distribuída e Paralela

**Instituto de Informática - UFRGS**
**Entrega Final: 29 de setembro de 2025**

## Regras para a execução do trabalho

1. O trabalho será entregue até o dia **29 de setembro de 2025 (10:20)** e apresentado nos dias **29 de setembro e 01 de outubro de 2025**, em aula.
   → Trabalho atrasado será descontado em **10% da nota máxima a cada dia de atraso**.

2. O trabalho pode ser realizado em **duplas ou trios**. Todos os alunos devem apresentar sua parte do trabalho e tirar dúvidas sobre a implementação realizada e os experimentos realizados.

3. Um aluno, em nome da dupla ou trio, até o prazo de entrega, deverá fazer o envio via **Moodle** de um arquivo contendo o que se pede.

## Sobre a avaliação do trabalho

* A nota será composta pela **qualidade técnica da solução desenvolvida**, do **conhecimento da implementação** realizada e da **apresentação**.
* A nota é condicionada à apresentação do trabalho no dia definido.

## Definição do Trabalho

O trabalho consiste da **implementação paralela em OpenMP** de um algoritmo sequencial, com o objetivo de **reduzir o tempo de execução** e **analisar o comportamento da aplicação** através da ferramenta **Intel VTune Profiler**.

O trabalho está dividido nas seguintes etapas (para auxiliar na organização):

1. Para evitar duplicatas, assim que escolher o problema, o membro responsável pela equipe deve acessar o Fórum do Moodle para descrever sua escolha e inserir o nome dos discentes que fazem parte do grupo.

2. **Realizar a implementação do código paralelo em OpenMP.**
   A equipe pode utilizar qualquer uma das diretivas vistas em sala de aula.

3. **Realizar diversos testes de desempenho**, variando o número de threads e o tamanho do conjunto de entrada.
   Para cada conjunto de entrada, gerar gráficos de **speedup** e **eficiência de desempenho**.

4. **Coletar informações da aplicação através do Intel VTune Profiler**, para auxiliar na explicação crítica dos resultados obtidos.

   * Tipos de análise sugeridos: **performance-snapshot**, **hotspots** e/ou **hpc-performance**.

5. **Escrever um relatório**, de posse dos gráficos, informando:

   * As **decisões tomadas** durante a paralelização e justificando-as.
   * A análise do desempenho da implementação paralela.
   * Os dados obtidos através do Intel VTune Profiler.

6. **Preparar slides para apresentação.**

   * Cada grupo terá **10 minutos** para apresentar a solução e os resultados.
   * A ordem de apresentação será definida no dia da apresentação.

---

## Configuração do Ambiente de Execução – Intel VTune na Hype

* Comando para carregar o ambiente do Intel VTune nas máquinas Hype:

  ```bash
  source /home/intel/oneapi/vtune/2021.1.1/vtune-vars.sh
  ```

* Tutorial de análise de desempenho do Intel VTune Profiler para identificar pontos do algoritmo ou hardware que estejam afetando o comportamento da aplicação:
  [Intel VTune Profiler – Tutorial Common Bottlenecks (Linux)](https://www.intel.com/content/www/us/en/docs/vtune-profiler/tutorial-common-bottlenecks-linux/2025-0/overview.html)

* Tutorial para usar a interface de linha de comando na Hype:
  [Intel VTune Profiler – Command Line Interface](https://www.intel.com/content/www/us/en/docs/vtune-profiler/user-guide/2024-0/command-line-interface.html)

* Outros vídeos úteis sobre o uso do VTune:

  * [https://www.youtube.com/watch?v=4jwhjsNock](https://www.youtube.com/watch?v=4jwhjsNock)
  * [https://www.youtube.com/watch?v=xMPWiR0Jmcg](https://www.youtube.com/watch?v=xMPWiR0Jmcg)

