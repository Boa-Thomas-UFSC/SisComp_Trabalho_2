#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    // Verifique se temos os argumentos mínimos necessários
    if (argc < 2) {
        fprintf(stderr, "Uso: \t%s <arquivo_de_saida> <arquivo1> <arquivo2> ... <arquivoN>\n\t%s -l\n", argv[0], argv[0]);
        return 1;
    }

    // O primeiro argumento é o arquivo de saída ou opção de listagem -l
    char *output_file = argv[1];

    if (strcmp(output_file, "-l") == 0) {
        // Listar todos os arquivos e tamanhos no arquivamento
        return 0;
    }

    // Os arquivos de entrada são os restantes argumentos
    char **input_files = &argv[2];
    int num_input_files = argc - 2;

    printf("Arquivo de saída: %s\n", output_file);
    for(int i=0; i < num_input_files; i++) {
        printf("Arquivo de entrada %d: %s\n", i, input_files[i]);
    }

    return 0;
}