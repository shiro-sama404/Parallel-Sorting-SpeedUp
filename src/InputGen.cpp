#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SEQ_LENGTH 100
#define DNA_CHARS "ACGT"

void generate_dna_sequence(char* seq, int length) {
    for (int i = 0; i < length; i++) {
        seq[i] = DNA_CHARS[rand() % 4];
    }
    seq[length] = '\0';
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Falha ao rodar o programa: quantidade inválida de argumentos\n");
        printf("Formato: %s <número_de_sequências> <nome_arquivo_saída>\n", argv[0]);
        return 1;
    }

    int num_sequences = atoi(argv[1]);
    char* output_filename = argv[2];

    srand(time(NULL));

    FILE* file = fopen(output_filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    char sequence[MAX_SEQ_LENGTH + 1];

    for (int i = 0; i < num_sequences; i++) {
        // Sequência aleatória entre 10 e 100
        int length = (rand() % 91) + 10; 
        generate_dna_sequence(sequence, length);
        fprintf(file, "%s\n", sequence);
    }

    fclose(file);
    printf("%d sequências de DNA geradas e salvadas em %s\n", num_sequences, output_filename);

    return 0;
}