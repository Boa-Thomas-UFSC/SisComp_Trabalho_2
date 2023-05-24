#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXNAME 256 // Tamanho máximo do nome do arquivo

#pragma pack ( push , 1 ) // Força o compilador a não adicionar bytes de padding
struct fileheader { // Cabeçalho de um arquivo
    char name[MAXNAME]; // Nome do arquivo
    unsigned int filesize; // Tamanho do arquivo
};
#pragma pack ( pop ) // Volta as configurações de padding para o padrão

int list_files(char *input_file) { // Função para listar os arquivos dentro de um arquivo de saída
    struct fileheader header; // Cabeçalho de um arquivo
    FILE *in_fp = fopen(input_file, "rb"); // Abre o arquivo de entrada
    if (in_fp == NULL) { // Verifica se o arquivo foi aberto com sucesso
        fprintf(stderr, "Não foi possível abrir o arquivo de entrada\n"); // Imprime uma mensagem de erro
        return 1; // Retorna 1 para indicar que houve um erro
    }

    printf("Arquivos no arquivo %s:\n", input_file); // Imprime uma mensagem de sucesso
    while (fread(&header, sizeof(header), 1, in_fp) == 1) { // Lê o cabeçalho de um arquivo
        printf("Nome do arquivo: %s, Tamanho do arquivo: %u bytes\n", header.name, header.filesize); // Imprime o nome e o tamanho do arquivo
        // Pula o conteúdo do arquivo para o próximo cabeçalho
        if (fseek(in_fp, header.filesize, SEEK_CUR) != 0) { // Verifica se o arquivo foi pulado com sucesso
            fprintf(stderr, "Falha ao ler o arquivo %s\n", input_file); // Imprime uma mensagem de erro
            return 1; // Retorna 1 para indicar que houve um erro
        }
    }

    fclose(in_fp); // Fecha o arquivo de entrada
    return 0; // Retorna 0 para indicar que não houve erros
}

int join_files(char *output_file, char **input_files, int num_input_files) { // Função para juntar arquivos
    struct fileheader header; // Cabeçalho de um arquivo
    FILE *out_fp = fopen(output_file, "wb"); // Abre o arquivo de saída
    if (out_fp == NULL) { // Verifica se o arquivo foi aberto com sucesso
        fprintf(stderr, "Não foi possível abrir o arquivo de saída\n"); // Imprime uma mensagem de erro
        return 1; // Retorna 1 para indicar que houve um erro
    }

    for(int i=0; i < num_input_files; i++) { // Percorre todos os arquivos de entrada
        FILE *in_fp = fopen(input_files[i], "rb"); // Abre o arquivo de entrada
        if (in_fp == NULL) { // Verifica se o arquivo foi aberto com sucesso
            fprintf(stderr, "Não foi possível abrir o arquivo de entrada %s\n", input_files[i]); // Imprime uma mensagem de erro
            continue; // Pula para o próximo arquivo
        }

        // Preenche a estrutura do cabeçalho do arquivo
        strncpy(header.name, input_files[i], MAXNAME - 1); // Copia o nome do arquivo para o cabeçalho
        fseek(in_fp, 0, SEEK_END); // Pula para o final do arquivo
        header.filesize = ftell(in_fp); // Pega o tamanho do arquivo
        rewind(in_fp); // Volta para o início do arquivo

        // Escreve o cabeçalho no arquivo de saída
        fwrite(&header, sizeof(header), 1, out_fp); // Escreve o cabeçalho no arquivo de saída

        // Copia o conteúdo do arquivo de entrada para o arquivo de saída
        char buffer[1024]; // Buffer para copiar o conteúdo do arquivo
        size_t bytes; // Número de bytes lidos
        while ((bytes = fread(buffer, 1, sizeof(buffer), in_fp)) > 0) { // Lê o conteúdo do arquivo de entrada
            fwrite(buffer, 1, bytes, out_fp); // Escreve o conteúdo do arquivo de entrada no arquivo de saída
        }

        fclose(in_fp); // Fecha o arquivo de entrada
    }

    fclose(out_fp); // Fecha o arquivo de saída
    return 0; // Retorna 0 para indicar que não houve erros
}

int extract_files(char *input_file) { // Função para extrair arquivos de um arquivo de saída
    struct fileheader header; // Cabeçalho de um arquivo
    FILE *in_fp = fopen(input_file, "rb"); // Abre o arquivo de entrada
    if (in_fp == NULL) { // Verifica se o arquivo foi aberto com sucesso
        fprintf(stderr, "Não foi possível abrir o arquivo de entrada\n"); // Imprime uma mensagem de erro
        return 1; 
    }

    while (fread(&header, sizeof(header), 1, in_fp) == 1) { // Lê o cabeçalho de um arquivo
        FILE *out_fp = fopen(header.name, "wb"); // Abre o arquivo de saída
        if (out_fp == NULL) { // Verifica se o arquivo foi aberto com sucesso
            fprintf(stderr, "Não foi possível abrir o arquivo de saída %s\n", header.name);
            return 1;
        }

        char *buffer = malloc(header.filesize); // Aloca memória para o arquivo
        if (buffer == NULL) { // Verifica se a memória foi alocada com sucesso
            fprintf(stderr, "Não foi possível alocar memória para o arquivo %s\n", header.name);
            return 1;
        }

        if (fread(buffer, 1, header.filesize, in_fp) != header.filesize) { // Lê o conteúdo do arquivo
            fprintf(stderr, "Falha ao ler o arquivo %s\n", header.name); // Imprime uma mensagem de erro
            return 1;
        }

        fwrite(buffer, 1, header.filesize, out_fp); // Escreve o conteúdo do arquivo no arquivo de saída
        fclose(out_fp); // Fecha o arquivo de saída
        free(buffer); // Libera a memória alocada para o arquivo

        printf("Arquivo %s extraído com sucesso\n", header.name);
    }

    fclose(in_fp);
    return 0;
}



int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: \t%s <arquivo_de_saida> <arquivo1> <arquivo2> ... <arquivoN>\n\t%s -l <arquivo_binario>\n", argv[0], argv[0]);
        return 1;
    }

    char *first_arg = argv[1];

//LISTAR ARQUIVOS

    if (strcmp(first_arg, "-l") == 0) {
        // O segundo argumento deve ser o arquivo que você deseja listar
        if (argc < 3) {
            fprintf(stderr, "Uso: \t%s -l <arquivo_binario>\n", argv[0]);
            return 1;
        }
        char *input_file = argv[2];
        list_files(input_file);
        return 0;
    }

//EXTRAIR ARQUIVOS

    if (strcmp(first_arg, "-e") == 0) {
        // O segundo argumento deve ser o arquivo que você deseja extrair
        if (argc < 3) {
            fprintf(stderr, "Uso: \t%s -e <arquivo_binario>\n", argv[0]);
            return 1;
        }
        char *input_file = argv[2];
        extract_files(input_file);
        return 0;
    }

    char *output_file = argv[1];
    char **input_files = &argv[2];
    int num_input_files = argc - 2;

    printf("Arquivo de saída: %s\n", output_file);
    for(int i=0; i < num_input_files; i++) {
        printf("Arquivo de entrada %d: %s\n", i, input_files[i]);
    }

//

    // Junta todos os arquivos de entrada no arquivo de saída
    if (join_files(output_file, input_files, num_input_files) != 0) {
        fprintf(stderr, "Falha ao juntar os arquivos\n");
        return 1;
    }

    printf("Arquivos juntados com sucesso no arquivo %s\n", output_file);
    return 0;
}

