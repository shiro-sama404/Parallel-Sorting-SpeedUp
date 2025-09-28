/**
 * @file SequencialSort.cpp
 * @brief Programa para ordenação sequencial de sequências de DNA lidas de um arquivo.
 *
 * Este programa lê sequências de DNA de um arquivo texto, armazena-as em um vetor de strings,
 * ordena as sequências em ordem lexicográfica utilizando sort da biblioteca padrão C++,
 * e grava o resultado ordenado em um arquivo de saída. O tempo de execução da ordenação é medido e exibido ao usuário.
 *
 * Funções principais:
 * - sequential_sort: Ordena um vetor de sequências de DNA utilizando sort.
 * - read_file: Lê um arquivo texto contendo sequências de DNA (uma por linha) e armazena-as em um vetor de strings.
 * - write_file: Escreve as sequências ordenadas em um arquivo texto, uma por linha.
 *
 * Execução:
 *   ./SequencialSort <arquivo_entrada> <arquivo_saida>
 *
 * Argumentos:
 *   arquivo_entrada - Caminho para o arquivo contendo as sequências de DNA a serem ordenadas.
 *   arquivo_saida   - Caminho para o arquivo onde as sequências ordenadas serão gravadas.
 *
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>

using namespace std;

/**
 * @brief Ordena um vetor de sequências de DNA em ordem lexicográfica.
 * @param data Vetor de strings representando sequências de DNA.
 */
void sequential_sort(vector<string>& data) {
    sort(data.begin(), data.end());
}

/**
 * @brief Lê arquivo texto com sequências de DNA e retorna vetor de strings.
 * @param filename Nome do arquivo de entrada.
 * @return Vetor de strings contendo as sequências de DNA.
 */
vector<string> read_file(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {throw runtime_error("Erro ao abrir o arquivo de entrada: " + filename);}

    vector<string> data;
    string line;
    while (getline(file, line)) {
        if (!line.empty()) { data.push_back(line); }
    }
    file.close();
    return data;
}

/**
 * @brief Escreve as sequências ordenadas em um arquivo texto.
 * @param filename Nome do arquivo de saída.
 * @param data Vetor de strings com as sequências ordenadas.
 */
void write_file(const string& filename, const vector<string>& data) {
    ofstream file(filename);
    if (!file.is_open()) {throw runtime_error("Erro ao abrir o arquivo de saída: " + filename);}

    for (const auto& seq : data) {
        file << seq << "\n";
    }
    file.close();
}

/**
 * @brief Função principal. Lê, ordena e grava sequências de DNA.
 * @param argc Número de argumentos.
 * @param argv Vetor de argumentos (entrada, saída).
 * @return 0 em caso de sucesso, 1 em erro.
 */
int main(int argc, char** argv) {
    if (argc != 3) {
        cerr << "Formato de execução: " << argv[0] << " <arquivo_entrada> <arquivo_saida>\n";
        return 1;
    }

    try {
        const string input_filename = argv[1];
        const string output_filename = argv[2];

        // Lê os dados do arquivo de entrada
        vector<string> dna_sequences = read_file(input_filename);

        // Mede o tempo de execução da ordenação
        auto start_time = chrono::high_resolution_clock::now();

        // Ordena os dados usando sort
        sequential_sort(dna_sequences);

        // Finaliza medição do tempo
        auto end_time = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed_time = end_time - start_time;

        // Escreve os dados ordenados no arquivo de saída
        write_file(output_filename, dna_sequences);

        cout << "Ordenação sequencial concluída em " << elapsed_time.count() << " segundos.\n";

    } catch (const exception& e) {
        cerr << "Erro: " << e.what() << "\n";
        return 1;
    }

    return 0;
}