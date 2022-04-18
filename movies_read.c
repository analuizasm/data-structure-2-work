#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "lista_encadeada_lib/lista_encadeada.h"

#define M 10000

typedef struct stFilm{
  char  titleType[13],
        primaryTitle[422],
        originalTitle[422];
  unsigned char isAdult;
  unsigned short int startYear,
                      endYear;
  char runtimeMinutes[6],
       genres[33];
}tpFilm;

void ordenar_arquivo(char *nome_arquivo, int *qtd_particoes);
tpFilm encontrar_menor_registro(tpFilm* memoria, int tamanho, int *index_menor);
void gravar_particao(FILE* particao, tpFilm *registro);
int ler_reposicao(FILE *arquivo, FILE *particao, FILE *reservatorio, int *tamanho_reservatorio, tpFilm* menor_registro, tpFilm* registro_reposicao);
int gravar_reservatorio(FILE *reservatorio, int *tamanho_reservatorio, tpFilm *registro_reposicao);
void gravar_memoria(FILE *particao, tpFilm *memoria, int tamanho);
tpFilm* ordernar_memoria(tpFilm *memoria, int tam);
void intercalacao_F3(char *particao1, char *particao2, char *particao_saida);
void gerenciar_intercalacao(int *qtd_particoes);
void imprimir_particao(char* nome_arquivo);
void acesso_direto(char* nome_arquivo, char* originalTitle);


int main (int ac, char **av){
    if(strcmp(av[1], "classificacao") == 0 ){
        FILE *arquivo_entrada = fopen("filmes/films.dat", "rb");
        tpFilm film;
        int qtd_particoes = 1;
        assert(arquivo_entrada != NULL);
        
        int i = 1;
        while ((fread(&film, sizeof(tpFilm), 1, arquivo_entrada) > 0)){
            printf("%d - %s \n", i, film.originalTitle);
            i++;
        }//while (fread(&film, sizeof(tpFilm), 1, arquivo_entrada) > 0){

        ordenar_arquivo("filmes/films.dat", &qtd_particoes);  
        gerenciar_intercalacao(&qtd_particoes);
        fclose(arquivo_entrada);
    } else if(strcmp(av[1], "acesso_direto") == 0){

        acesso_direto("acesso_direto/arquivo_ordenado.dat", av[2]);
    }

    return 1;
}

void ordenar_arquivo(char *nome_arquivo, int *qtd_particoes){
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if(arquivo == NULL){
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", nome_arquivo);
        exit(EXIT_FAILURE);
    }

    FILE *particao = NULL;
    FILE *reservatorio = NULL;
    char *titulo_particao = (char *) malloc(2 * M * sizeof(char));

    tpFilm *memoria = (tpFilm *) malloc(M * sizeof(tpFilm));

    int tamanho_reservatorio = 0,
        tamanho_memoria = M - 1,
        indice_menor_registro = 0;
    
    tpFilm registro_reposicao;

    fread(memoria, sizeof(tpFilm), M, arquivo);
    
    sprintf(titulo_particao, "part-%.5d.dat", *qtd_particoes);
 
    particao = fopen(titulo_particao, "wb");
    assert(particao != NULL);

    reservatorio = fopen("reservatorio.dat", "wb+");

    tpFilm menor_registro;
    int fim = 0;

    while(!fim){
        menor_registro = encontrar_menor_registro(memoria, M, &indice_menor_registro);

        gravar_particao(particao, &menor_registro);
        int resultado = ler_reposicao(arquivo, particao, reservatorio, &tamanho_reservatorio, &menor_registro, &registro_reposicao);

        if(resultado == 1){
            memoria[indice_menor_registro] = registro_reposicao;
        } else {
            gravar_memoria(particao, memoria, tamanho_memoria);
            fclose(particao);
            int aux = *qtd_particoes;
            aux++;
            *qtd_particoes = aux;
            sprintf(titulo_particao, "part-%.5d.dat", *qtd_particoes);
            
            if(resultado == 2){
                fflush(reservatorio);

                fseek(reservatorio, 0, SEEK_SET);

                assert(fread(memoria, sizeof(tpFilm), tamanho_reservatorio, reservatorio) > 0);

                fseek(reservatorio, 0, SEEK_SET);

                tamanho_reservatorio = 0;

                particao = fopen(titulo_particao, "wb");

                assert(particao != NULL);

            } else if(resultado == -1)  {
                if(tamanho_reservatorio > 0) { 
                    fseek(reservatorio, 0, SEEK_SET);

                    assert(fread(memoria, sizeof(tpFilm), tamanho_reservatorio, reservatorio) > 0);
                    
                    fclose(reservatorio);

                    particao = fopen(titulo_particao, "wb");

                    assert(particao != NULL);

                    gravar_memoria(particao, memoria, tamanho_reservatorio);

                    fclose(particao);
                }
                fim = 1;
            }
        }
    }
}

tpFilm encontrar_menor_registro(tpFilm *memoria, int tamanho, int *index_menor){
    tpFilm menor_registro = memoria[0];
    int indice = 0;
    for(int i = 1; i < tamanho; i++){
        if(strcmp(menor_registro.originalTitle, memoria[i].originalTitle) > 0) {
            indice = i;
            menor_registro = memoria[i];
        }
    }
    *index_menor = indice; 
    return menor_registro;
}

void gravar_particao(FILE *particao, tpFilm *registro){

    assert(fwrite(registro, sizeof(tpFilm), 1, particao) > 0);

    fflush(particao);
}

int ler_reposicao(FILE *arquivo, FILE *particao, FILE *reservatorio, int *tamanho_reservatorio, tpFilm* menor_registro, tpFilm* registro_reposicao){
    int resultado;
    int fim = 0;

    while(!fim){
        if(fread(registro_reposicao, sizeof(tpFilm), 1, arquivo) > 0){
            if(strcmp(menor_registro->originalTitle, registro_reposicao->originalTitle) < 0){
                resultado = 1;
                fim = 1;
            } else {
                int resultado_reservatorio = 0;
                resultado_reservatorio = gravar_reservatorio(reservatorio, tamanho_reservatorio, registro_reposicao);

                if(resultado_reservatorio == 1){
                    resultado = 2;
                    fim = 1;
                }
            }
        }else{
            resultado = -1;
            fim = 1;
        }
    }    
    return resultado;
}

int gravar_reservatorio(FILE *reservatorio, int *tamanho_reservatorio, tpFilm *registro_reposicao){
    assert(fwrite(registro_reposicao, sizeof(tpFilm), 1, reservatorio) > 0);
    
    int indice = *tamanho_reservatorio;
    indice++;
    *tamanho_reservatorio = indice;

    if(indice == M) return 1;

    return -1;
}

void gravar_memoria(FILE *particao, tpFilm *memoria, int tamanho){
    memoria = ordernar_memoria(memoria, tamanho);

    for(int i = 0; i < tamanho; i++){
        assert(fwrite((memoria+i), sizeof(tpFilm), 1, particao) > 0);
    }
}

tpFilm* ordernar_memoria(tpFilm *memoria, int tam){
    tpFilm temp;

    for(int end = tam - 1; end > 0; end--){
        int posicao_maior = 0;

        for(int i = 0; i < end; i++){
            if(strcmp(memoria[i].originalTitle, memoria[posicao_maior].originalTitle) > 0){
                posicao_maior = i;
            }
        }

        temp = memoria[end];
        memoria[end] = memoria[posicao_maior];
        memoria[posicao_maior] = temp;
    }
    
    return memoria;
}

void intercalacao_F3(char *particao1, char *particao2, char *particao_saida){
    FILE *part1 = fopen(particao1, "rb");
    FILE *part2 = fopen(particao2, "rb");
    FILE *saida = fopen(particao_saida, "wb");

    tpFilm* registro_part1 = (tpFilm*) malloc(sizeof(tpFilm));
    tpFilm* registro_part2 = (tpFilm*) malloc(sizeof(tpFilm));

    int fim_part1 = 0,
        fim_part2 = 0,
        resultado = 0;

    assert(part1 != NULL);
    assert(part2 != NULL);
    assert(saida != NULL);

    fim_part1 = fread(registro_part1, sizeof(tpFilm), 1, part1);
    fim_part2 = fread(registro_part2, sizeof(tpFilm), 1, part2);

    do{
        resultado = strcmp(registro_part1->originalTitle, registro_part2->originalTitle);
        if(resultado < 0){
            fwrite(registro_part1, sizeof(tpFilm), 1, saida);
            fim_part1 = (fread(registro_part1, sizeof(tpFilm), 1, part1));
        } else {
            fwrite(registro_part2, sizeof(tpFilm), 1, saida);
            fim_part2 = (fread(registro_part2, sizeof(tpFilm), 1, part2));
        }
    } while(((fim_part1) > 0) && ((fim_part2) > 0 ));

    if(fim_part1 == 0){
        do{
            fwrite(registro_part2, sizeof(tpFilm), 1, saida);
            fim_part2 = fread(registro_part2, sizeof(tpFilm), 1, part2);
        } while (fim_part2 > 0);
    } else if(fim_part2 == 0){
        do{
            fwrite(registro_part1, sizeof(tpFilm), 1, saida);
            fim_part1 = fread(registro_part1, sizeof(tpFilm), 1, part1);
        } while(fim_part1 > 0);
    } else { 
        fprintf(stderr, "Erro ao fazer merge");
        exit(EXIT_FAILURE);
    }
    fclose(part1);
    fclose(part2);
    
    assert(remove(particao1) == 0); /*remover partições que já foram unidas para poupar espaço em disco */
    assert(remove(particao2) == 0);

    fclose(saida);
}

void gerenciar_intercalacao(int *qtd_particoes){
    if(*qtd_particoes > 1){
        char *titulo_saida = (char*) malloc(2*M*sizeof(char));
        int tamanho_lista = 0;

        Lista* lista_particoes = lst_cria();

        char *titulo_particoes = (char*) malloc(sizeof(char)*M*2);
        
        for(int i = 1; i <= *qtd_particoes; i++){
            sprintf(titulo_particoes, "part-%.5d.dat", i);
            lista_particoes = lista_insere(lista_particoes, titulo_particoes, &tamanho_lista);
        }

        Lista *p = lista_particoes;

        int qtd_particoes_saida = 0;
        
        int qtd_merges = 0;
        while((tamanho_lista) > 1){
            sprintf(titulo_saida, "merge-%.5d.dat", ++qtd_particoes_saida);

            printf("Fazendo merge entre %s e %s\n..", p->nomeParticao, p->prox->nomeParticao);

            intercalacao_F3(p->nomeParticao, p->prox->nomeParticao, titulo_saida);
            qtd_merges++;
            lista_particoes = lista_insere(lista_particoes, titulo_saida, &tamanho_lista);
            tamanho_lista -=2;
            p = p->prox->prox;
            lista_particoes = remove_inicio(lista_particoes);
            lista_particoes = remove_inicio(lista_particoes);
        }
        printf("-------------------------------------------------------");
        printf("Part final = %s \n Quantidade de merges realizados: %d", titulo_saida, qtd_merges);
        printf("---------------------------------------------------------");
        
        libera_lst(lista_particoes);

        imprimir_particao(titulo_saida);
    }
}

void imprimir_particao(char *nome_arquivo){
    printf("Lendo saida...");
    FILE *particao = NULL;
    tpFilm film;
    particao = fopen(nome_arquivo, "rb");
    assert(particao != NULL);

    int i = 1;

    while ((fread(&film, sizeof(tpFilm), 1, particao) > 0)){
        printf("%d - %s \n", i, film.originalTitle);
        i++;
    }//while (fread(&film, sizeof(tpFilm), 1, particao) > 0){
    fclose(particao);
}

void acesso_direto(char* nome_arquivo, char* originalTitle){
    unsigned long int tam_estrutura      = sizeof(tpFilm);
    unsigned long int tam_arquivo        = 0;
    unsigned long int total_registros    = 0;
    unsigned long int posicao_no_arquivo;
    int achou = 0;
    tpFilm film; 
    
    FILE *arquivo = fopen(nome_arquivo, "rb");
    assert(arquivo != NULL);
    
    fseek(arquivo, 0, SEEK_END); 
    tam_arquivo = ftell(arquivo); 
    fseek(arquivo, 0, SEEK_SET); 

    total_registros = tam_arquivo / tam_estrutura;

    printf("Iniciando busca\n");

    unsigned long int inicio = 0;
    unsigned long int fim = total_registros - 1;
    unsigned long int meio = 0;
    
    while(inicio <= fim && !achou){
        meio = (inicio + fim)/2;
        fseek(arquivo, sizeof(tpFilm) * meio, SEEK_SET);

        assert(fread(&film, sizeof(tpFilm), 1, arquivo) == 1);

        if(strcmp(film.originalTitle, originalTitle) == 0){
            achou = 1;
        } else if(strcmp(film.originalTitle, originalTitle) < 0){
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }

    if(achou == 0){
        printf("Não foi possível encontrar o registro!\n");
    } else {
        printf("%s | %s | %s | %c | %d | %d | %s | %s \n", film.titleType, film.primaryTitle, film.originalTitle, film.isAdult, film.startYear, film.endYear, film.runtimeMinutes, film.genres);
    }
    fseek(arquivo, 0, SEEK_SET);
    
    fclose(arquivo);
}

