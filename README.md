# Ordenação de Dados usando MPI

Este projeto envolve o desenvolvimento de uma aplicação paralela para ordenar grandes volumes de dados genômicos (sequências de DNA (A, C, G, T)).
Para isto foram utilizados a biblioteca Message Passing Interface (MPI) e as linguagens C e C++ em um ambiente com memória distribuída.

## Objetivos

Os principais objetivos deste projeto são:

- Implementar uma solução de ordenação paralela para um conjunto de sequências genômicas usando MPI.
- Comparar o desempenho da solução paralela com uma solução sequencial.
- Realizar experimentos em um computador com pelo menos 8 processadores para garantir que a solução paralela tenha um desempenho melhor que a sequencial.
- Elaborar um relatório descrevendo a solução, a configuração experimental e os resultados obtidos.

## Resultados

Os resultados estão dispostos em [`Relatório.pdf`]([https://link-url-here.org](https://github.com/shiro-sama404/Parallel-Sorting-SpeedUp/blob/main/Relat%C3%B3rio.pdf)) e foram obtidos utilizando grandes volumes de dados em 3 arquivos contendo 100 mil, 1 milhão e 10 milhões de sequências e estão dispostos [AQUI](https://drive.google.com/drive/folders/1v_0k624A_p1z2gTOr4E3EtSv9Y81Lp-O?usp=sharing).

## Requisitos

- Sistema Operacional Linux
  - Open MPI (ou outro MPI de sua escolha)
  - Compilador GCC/MinGW

- Sistema Operacional Windows
  - MS MPI
  - Compilador MSVC

## Como Executar

### Criando Dados de Entrada

1. Navegue até o diretório `/src`

2. Compile o gerador de dados para criar um arquivo executavel:
```bash
gcc InputGen.c -o InputGen
```

3. Execute o gerador de dados para criar um arquivo com um número específico de sequências.

```bash
./InputGen <número_sequências> <nome_arquivo_saída>
```

Por exemplo, para gerar um input de 1 milhão de sequências:

```bash
./InputGen 1000000 in_100k.txt
```

Isso criará um arquivo de entrada com 1 milhão de sequências de DNA.

### Ordenação Sequencial

1. Navegue até o diretório `/src`

2. Compile o código-fonte:
```bash
gcc SequentialSort.cpp -o sequential_sort
```

3. Execute a ordenação de sequências para um input específico:
```bash
./sequential_sort <nome_arquivo_entrada> <nome_arquivo_saída>
```
Por exemplo, para usar um arquivo "in_100k.txt" e gerar o arquivo com a sequência ordenada "seq_out_100k.txt":

```bash
./sequential_sort in_100k.txt seq_out_100k.txt
```

### Ordenação Paralela

1. Navegue até o diretório `/src`

2. Compile o código-fonte:
```bash
mpic++ -O2 -std=c++11 -o SampleSort SampleSort.cpp
```

3. Execute a ordenação de sequências para um input específico:
```bash
mpirun -np <número_processos> ./SampleSort <nome_arquivo_entrada> <nome_arquivo_saída>
```

Por exemplo, para usar um arquivo "in_100k.txt" e gerar o arquivo com a sequência ordenada "par_out_100k.txt" utilizando 4 processos:
```bash
mpirun -np 4 ./SampleSort2 in_100k.txt par_out_100k.txt
```
