/**
 * @file ParallelSampleSort.cpp
 * @brief Programa paralelo de ordenação de sequências de DNA usando Sample Sort com MPI.
 *
 * Este programa lê sequências de DNA de um arquivo texto, distribui as sequências entre múltiplos
 * processos MPI, ordena localmente cada partição, utiliza amostras para calcular pivôs globais
 * e redistribui os dados de acordo com esses pivôs. No final, cada processo ordena seus dados
 * finais e o processo mestre reúne os resultados ordenados em um único arquivo de saída.
 *
 * Funções principais:
 * - sequential_sort: Ordena um vetor de sequências de DNA em ordem lexicográfica.
 * - read_file: Lê um arquivo texto contendo sequências de DNA (uma por linha).
 * - write_file: Escreve as sequências ordenadas em um arquivo texto (uma por linha).
 *
 * Execução:
 *   mpirun -np <N> ./ParallelSampleSort <arquivo_entrada> <arquivo_saida>
 *
 * Argumentos:
 *   arquivo_entrada - Caminho para o arquivo contendo as sequências de DNA.
 *   arquivo_saida   - Caminho para o arquivo onde as sequências ordenadas serão gravadas.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <mpi.h>

#define MASTER 0

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
 * @brief Função principal. Ordena sequências de DNA paralelamente usando Sample Sort com MPI.
 * @param argc Número de argumentos.
 * @param argv Vetor de argumentos (entrada, saída).
 * @return 0 em caso de sucesso, 1 em erro.
 */
int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 3) {
        if (rank == MASTER) { cerr << "Uso: mpirun -np <N> " << argv[0] << " <arquivo_entrada> <arquivo_saida>\n"; }
        MPI_Finalize();
        return 1;
    }

    vector<string> all_data;
    vector<string> local_data;
    int n = 0;

    // Leitura inicial apenas no processo MASTER
    if (rank == MASTER) {
        all_data = read_file(argv[1]);
        n = all_data.size();
    }

    double total_start = MPI_Wtime();

    // Broadcast do número total de sequências
    MPI_Bcast(&n, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

    // Distribuição inicial das sequências
    int local_n = n / size + (rank < (n % size) ? 1 : 0);
    local_data.resize(local_n);

    if (rank == MASTER) {
        int offset = 0;
        for (int p = 0; p < size; p++) {
            int count = n / size + (p < (n % size) ? 1 : 0);
            if (p == MASTER) {
                copy(all_data.begin(), all_data.begin() + count, local_data.begin());
            } else {
                for (int i = 0; i < count; i++) {
                    const string& s = all_data[offset + i];
                    int len = s.size() + 1;
                    MPI_Send(&len, 1, MPI_INT, p, 0, MPI_COMM_WORLD);
                    MPI_Send(s.c_str(), len, MPI_CHAR, p, 1, MPI_COMM_WORLD);
                }
            }
            offset += count;
        }
    } else {
        for (int i = 0; i < local_n; i++) {
            int len;
            MPI_Recv(&len, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            vector<char> buf(len);
            MPI_Recv(buf.data(), len, MPI_CHAR, MASTER, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            local_data[i] = string(buf.data());
        }
    }

    // Ordenação local
    double local_sort_start = MPI_Wtime();
    sequential_sort(local_data);
    double local_sort_end = MPI_Wtime();

    // Seleção das amostras locais
    int s = size - 1;
    vector<string> samples;
    for (int i = 1; i <= s; i++) {
        int idx = i * local_data.size() / (s + 1);
        if (idx < (int)local_data.size())
            samples.push_back(local_data[idx]);
    }

    // Coleta de amostras
    vector<string> gathered_samples;
    if (rank == MASTER) {
        gathered_samples = samples;
        for (int p = 1; p < size; p++) {
            int count;
            MPI_Recv(&count, 1, MPI_INT, p, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int i = 0; i < count; i++) {
                int len;
                MPI_Recv(&len, 1, MPI_INT, p, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                vector<char> buf(len);
                MPI_Recv(buf.data(), len, MPI_CHAR, p, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                gathered_samples.push_back(string(buf.data()));
            }
        }
    } else {
        int count = samples.size();
        MPI_Send(&count, 1, MPI_INT, MASTER, 2, MPI_COMM_WORLD);
        for (auto& smp : samples) {
            int len = smp.size() + 1;
            MPI_Send(&len, 1, MPI_INT, MASTER, 3, MPI_COMM_WORLD);
            MPI_Send(smp.c_str(), len, MPI_CHAR, MASTER, 4, MPI_COMM_WORLD);
        }
    }

    // Escolha dos pivôs globais
    vector<string> pivots(size - 1);
    if (rank == MASTER) {
        sort(gathered_samples.begin(), gathered_samples.end());
        for (int i = 1; i < size; i++) {
            pivots[i - 1] = gathered_samples[i * gathered_samples.size() / size];
        }
    }

    // Broadcast dos pivôs
    for (int i = 0; i < size - 1; i++) {
        int len;
        if (rank == MASTER) len = pivots[i].size() + 1;
        MPI_Bcast(&len, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
        vector<char> buf(len);
        if (rank == MASTER) strcpy(buf.data(), pivots[i].c_str());
        MPI_Bcast(buf.data(), len, MPI_CHAR, MASTER, MPI_COMM_WORLD);
        pivots[i] = string(buf.data());
    }

    // Particionamento das sequências locais
    vector<vector<string>> buckets(size);
    for (auto& seq : local_data) {
        int pos = upper_bound(pivots.begin(), pivots.end(), seq) - pivots.begin();
        buckets[pos].push_back(seq);
    }

    // Troca de dados entre processos
    vector<int> send_sizes(size), recv_sizes(size);
    for (int i = 0; i < size; i++) send_sizes[i] = buckets[i].size();
    MPI_Alltoall(send_sizes.data(), 1, MPI_INT, recv_sizes.data(), 1, MPI_INT, MPI_COMM_WORLD);

    vector<string> new_local;
    for (int p = 0; p < size; p++) {
        if (p == rank) {
            new_local.insert(new_local.end(), buckets[p].begin(), buckets[p].end());
        } else {
            for (auto& smp : buckets[p]) {
                int len = smp.size() + 1;
                MPI_Send(&len, 1, MPI_INT, p, 5, MPI_COMM_WORLD);
                MPI_Send(smp.c_str(), len, MPI_CHAR, p, 6, MPI_COMM_WORLD);
            }
        }
    }
    for (int p = 0; p < size; p++) {
        if (p != rank) {
            for (int i = 0; i < recv_sizes[p]; i++) {
                int len;
                MPI_Recv(&len, 1, MPI_INT, p, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                vector<char> buf(len);
                MPI_Recv(buf.data(), len, MPI_CHAR, p, 6, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                new_local.push_back(string(buf.data()));
            }
        }
    }

    // Ordenação final local
    double final_sort_start = MPI_Wtime();
    sequential_sort(new_local);
    double final_sort_end = MPI_Wtime();

    // Coleta final no MASTER
    int final_local_n = new_local.size();
    vector<int> final_counts(size);
    MPI_Gather(&final_local_n, 1, MPI_INT, final_counts.data(), 1, MPI_INT, MASTER, MPI_COMM_WORLD);

    vector<string> final_all;
    if (rank == MASTER) {
        int total = accumulate(final_counts.begin(), final_counts.end(), 0);
        final_all.reserve(total);

        final_all.insert(final_all.end(), new_local.begin(), new_local.end());
        for (int p = 1; p < size; p++) {
            for (int i = 0; i < final_counts[p]; i++) {
                int len;
                MPI_Recv(&len, 1, MPI_INT, p, 7, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                vector<char> buf(len);
                MPI_Recv(buf.data(), len, MPI_CHAR, p, 8, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                final_all.push_back(string(buf.data()));
            }
        }

        // Grava resultado final
        write_file(argv[2], final_all);
    } else {
        for (auto& s : new_local) {
            int len = s.size() + 1;
            MPI_Send(&len, 1, MPI_INT, MASTER, 7, MPI_COMM_WORLD);
            MPI_Send(s.c_str(), len, MPI_CHAR, MASTER, 8, MPI_COMM_WORLD);
        }
    }

    double total_end = MPI_Wtime();

    // Impressão dos tempos de execução
    if (rank == MASTER) {
        cout << endl << "=== Tempos de execução ===" << endl;
        cout << "Ordenação local:      " << (local_sort_end - local_sort_start) << " segundos" << endl;
        cout << "Ordenação final:      " << (final_sort_end - final_sort_start) << " segundos" << endl;
        cout << "Tempo total:          " << (total_end - total_start) << " segundos" << endl;
        cout << "==========================" << endl;
    }

    MPI_Finalize();
    return 0;
}