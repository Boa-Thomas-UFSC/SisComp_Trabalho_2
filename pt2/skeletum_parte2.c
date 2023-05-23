#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXNAME 256

#pragma pack ( push , 1 )
struct fileheader {
    char name[MAXNAME]; 
    unsigned int filesize; 
};
#pragma pack ( pop )

int list_files(char *input_file) {
    struct fileheader header;
    FILE *in_fp = fopen(input_file, "rb");
    if (in_fp == NULL) {
        fprintf(stderr, "Não foi possível abrir o arquivo de entrada\n");
        return 1;
    }

    printf("Arquivos no arquivo %s:\n", input_file);
    while (fread(&header, sizeof(header), 1, in_fp) == 1) {
        printf("Nome do arquivo: %s, Tamanho do arquivo: %u bytes\n", header.name, header.filesize);
        // Pula o conteúdo do arquivo
        if (fseek(in_fp, header.filesize, SEEK_CUR) != 0) {
            fprintf(stderr, "Falha ao ler o arquivo %s\n", input_file);
            return 1;
        }
    }

    fclose(in_fp);
    return 0;
}

int join_files(char *output_file, char **input_files, int num_input_files) {
    struct fileheader header;
    FILE *out_fp = fopen(output_file, "wb");
    if (out_fp == NULL) {
        fprintf(stderr, "Não foi possível abrir o arquivo de saída\n");
        return 1;
    }

    for(int i=0; i < num_input_files; i++) {
        FILE *in_fp = fopen(input_files[i], "rb");
        if (in_fp == NULL) {
            fprintf(stderr, "Não foi possível abrir o arquivo de entrada %s\n", input_files[i]);
            continue;
        }

        // Preenche a estrutura do cabeçalho
        strncpy(header.name, input_files[i], MAXNAME - 1);
        fseek(in_fp, 0, SEEK_END);
        header.filesize = ftell(in_fp);
        rewind(in_fp);

        // Escreve o cabeçalho no arquivo de saída
        fwrite(&header, sizeof(header), 1, out_fp);

        // Copia o conteúdo do arquivo de entrada para o arquivo de saída
        char buffer[1024];
        size_t bytes;
        while ((bytes = fread(buffer, 1, sizeof(buffer), in_fp)) > 0) {
            fwrite(buffer, 1, bytes, out_fp);
        }

        fclose(in_fp);
    }

    fclose(out_fp);
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: \t%s <arquivo_de_saida> <arquivo1> <arquivo2> ... <arquivoN>\n\t%s -l\n", argv[0], argv[0]);
        return 1;
    }

    char *output_file = argv[1];

    if (strcmp(output_file, "-l") == 0) {
        // Implementar funcionalidade de listagem aqui...
        return list_files(input_file);
    }

    char **input_files = &argv[2];
    int num_input_files = argc - 2;

    printf("Arquivo de saída: %s\n", output_file);
    for(int i=0; i < num_input_files; i++) {
        printf("Arquivo de entrada %d: %s\n", i, input_files[i]);
    }

    // Junta todos os arquivos de entrada no arquivo de saída
    if (join_files(output_file, input_files, num_input_files) != 0) {
        fprintf(stderr, "Falha ao juntar os arquivos\n");
        return 1;
    }

    printf("Arquivos juntados com sucesso no arquivo %s\n", output_file);
    return 0;
}
