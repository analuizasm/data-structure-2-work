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

typedef struct ltHead{
    int tamanho_lista;
    Lista* lista;
}Head;


void ordenar_arquivo(char *nome_arquivo, int *qtd_particoes);
tpFilm encontrar_menor_registro(tpFilm* memoria, int tamanho, int *index_menor);
void gravar_particao(FILE* particao, tpFilm *registro);
int ler_reposicao(FILE *arquivo, FILE *particao, FILE *reservatorio, int *tamanho_reservatorio, tpFilm* menor_registro, tpFilm* registro_reposicao);
int gravar_reservatorio(FILE *reservatorio, int *tamanho_reservatorio, tpFilm *registro_reposicao);
void grava_memoria(FILE *particao, tpFilm *memoria, int tamanho);
tpFilm* ordernar_memoria(tpFilm *memoria, int tam);
void intercalacao_F3(char *particao1, char *particao2, char *particao_saida);
char* gerenciar_intercalacao(int *qtd_particoes);

int main (int ac, char **av){
    FILE *arquivo_entrada = fopen("part_saida-00003.dat", "rb");
    tpFilm film;
    int qtd_particoes = 425; //mudar para 1
    assert(arquivo_entrada != NULL);
   
    int i = 1;
    while ((fread(&film, sizeof(tpFilm), 1, arquivo_entrada) > 0) && i < 3){
        printf("%d - %s \n", i, film.originalTitle);
        i++;
    }//while (fread(&film, sizeof(tpFilm), 1, arquivo_entrada) > 0){
 
    //ordenar_arquivo("src/filmes/films.dat", &qtd_particoes);  
    
    
    intercalacao_F3("part_saida-00001.dat", "part_saida-00002.dat", "part_saida-00003.dat");
    // gerenciar_intercalacao(qtd);
    //gerenciar_intercalacao(&qtd_particoes);
    //fclose(arquivo_entrada);
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
    
    sprintf(titulo_particao, "src/particoes/part-%.5d.dat", *qtd_particoes);
 
    particao = fopen(titulo_particao, "wb");
    assert(particao != NULL);

    reservatorio = fopen("reservatorio.dat", "wb+");

    tpFilm menor_registro;
    int fim = 0;
    while(!fim){
        menor_registro = encontrar_menor_registro(memoria, M, &indice_menor_registro);
        printf("%s \n", menor_registro.originalTitle);

        gravar_particao(particao, &menor_registro);
        printf("menor_registro: %s i: %d \n", menor_registro.originalTitle, indice_menor_registro);
        int resultado = ler_reposicao(arquivo, particao, reservatorio, &tamanho_reservatorio, &menor_registro, &registro_reposicao);
        printf("%d\n", resultado);

        if(resultado == 1){
            memoria[indice_menor_registro] = registro_reposicao;
            printf("memoria apos resultado 1: %s i: %d\n", memoria[indice_menor_registro].originalTitle, indice_menor_registro);
        } else {
            grava_memoria(particao, memoria, tamanho_memoria);
            fclose(particao);
            int aux = *qtd_particoes;
            aux++;
            *qtd_particoes = aux;
            sprintf(titulo_particao, "src/particoes/part-%.5d.dat", *qtd_particoes);

            if(resultado == 2){
                fflush(reservatorio);
                fseek(reservatorio, 0, SEEK_SET);
                assert(fread(&(memoria[0]), sizeof(tpFilm), tamanho_reservatorio, reservatorio) > 0);
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
                    grava_memoria(particao, memoria, tamanho_memoria);
                    fclose(particao);
                    printf("FINALIZEI MANO----------------------");
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
        if(strcmp(menor_registro.originalTitle, memoria[i].originalTitle) < 0) {
            indice = i;
            menor_registro = memoria[i];
        }
    }
    *index_menor = indice; 
   // printf("encontrar index: %d $$$$$$$$\n", *index_menor);
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
            printf("%s, %s\n", menor_registro->originalTitle, registro_reposicao->originalTitle);
            if(strcmp(menor_registro->originalTitle, registro_reposicao->originalTitle) < 0){
                resultado = 1;
                fim = 1;
            } else {
                int resultado_reservatorio = 0;
                resultado_reservatorio = gravar_reservatorio(reservatorio, tamanho_reservatorio, registro_reposicao);
                printf("%d\n", resultado_reservatorio);
                if(resultado_reservatorio == 1){
                    resultado = 2;
                    fim = 1;
                }
            }
        } else {
            resultado = -1;
            printf("--------------------------------RETORNOU -1-------------------------------------");
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
    //printf("TAMANHO: %d\n", *tamanho_reservatorio);
    if(indice == M) return 1;

    return -1;
}

void grava_memoria(FILE *particao, tpFilm *memoria, int tamanho){
    int fim = 0;
    int posicao_menor_registro = 0;
    tpFilm menor_registro;

    memoria = ordernar_memoria(memoria, tamanho);

    for(int i = 0; i < tamanho; i++){
        assert(fwrite(&memoria[i], sizeof(tpFilm), 1, particao) > 0);
    }
}

tpFilm* ordernar_memoria(tpFilm *memoria, int tam){
    tpFilm temp;

    for(unsigned long end = tam - 1; end > 0; end--){
        unsigned long posicao_maior = 0;

        for(unsigned long i = 0; i < end; i++){
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
    int qtd_gravados_pt1 = 0,
        qtd_gravados_pt2 = 0;

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
       // printf("resultado: %d, titulo1 = %s, titulo2 = %s\n", resultado, registro_part1->originalTitle, registro_part2->originalTitle);
        if(resultado == -1){
            fwrite(registro_part1, sizeof(tpFilm), 1, saida);
            fim_part1 = (fread(registro_part1, sizeof(tpFilm), 1, part1));
            qtd_gravados_pt1++;
        } else {
            fwrite(registro_part2, sizeof(tpFilm), 1, saida);
            fim_part2 = (fread(registro_part2, sizeof(tpFilm), 1, part2));
            qtd_gravados_pt2++;
        }
    } while(((fim_part1) > 0) && (fim_part2 > 0 ));

    if(fim_part1 == 0){
        do{
            fwrite(registro_part2, sizeof(tpFilm), 1, saida);
            fim_part2 = fread(registro_part2, sizeof(tpFilm), 1, part2);
            qtd_gravados_pt2++;
        } while (fim_part2 > 0);
    } else if(fim_part2 == 0){
        do{
            fwrite(registro_part1, sizeof(tpFilm), 1, saida);
            fim_part1 = fread(registro_part1, sizeof(tpFilm), 1, part1);
            qtd_gravados_pt1++;
        } while(fim_part1 > 0);
    } else { 
        fprintf(stderr, "ERRO LINHA %d \n", __LINE__);
        exit(EXIT_FAILURE);
    }
    fclose(part1);
    fclose(part2);
    fflush(saida);
    fclose(saida);
    printf("pt1 = %d \n pt2 = %d\n", qtd_gravados_pt1, qtd_gravados_pt2);
}

char* gerenciar_intercalacao(int *qtd_particoes){
    Head* head = (Head*) malloc(sizeof(head));
    head->tamanho_lista = 0;

    Lista* lista_particoes = lst_cria();

    head->lista = lista_particoes;

    char *titulo_particoes = (char*) malloc(sizeof(char)*M*2);
    
    for(int i = 1; i <= *qtd_particoes; i++){
      sprintf(titulo_particoes, "src/particoes/part-%.5d.dat", i);
      lista_particoes = lista_insere(lista_particoes, titulo_particoes, &head->tamanho_lista);
    }

    Lista* p = lista_particoes;

    head->lista = p;

    if(*qtd_particoes > 1){
        Lista *p = lista_particoes;

        int qtd_particoes_saida = 0;
        char *titulo_saida = (char*) malloc(2*M*sizeof(char));
        int i = 0;
       
        while((head->tamanho_lista - i) > 1){
            sprintf(titulo_saida, "part_saida-%.5d.dat", ++qtd_particoes_saida);
            printf("mesclando %s e %s\n", p->nomeParticao, p->prox->nomeParticao);
            intercalacao_F3(p->nomeParticao, p->prox->nomeParticao, titulo_saida);
            lista_particoes = lista_insere(lista_particoes, titulo_saida, &head->tamanho_lista);
            i +=2;
            p = p->prox->prox;
        }
        printf("%s", titulo_saida);
        // tpFilm film;
        // FILE *saida = fopen(titulo_saida, "rb");
        // assert(saida != NULL);

        // i = 1;
        // while (fread(&film, sizeof(tpFilm), 1, saida) > 0){
        //     printf("%d - %s \n", i,  film.originalTitle);
        //     i++;
        // }//while (fread(&film, sizeof(tpFilm), 1, arquivo_entrada) > 0){
    }
}


