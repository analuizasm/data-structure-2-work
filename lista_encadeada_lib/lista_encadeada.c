#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista_encadeada.h"

Lista* lst_cria(void){
    return NULL;
}

void libera_lst(Lista* lista){
    Lista *p = lista;
    while(p != NULL){
        Lista *t = p->prox;
        free(p);
        p = t;
    }
}

Lista* lista_insere(Lista *lista, char *nome, int* tamanho_da_lista){
    char *nomeParticao = (char *) malloc(sizeof(nome));
    Lista *novo = (Lista*) malloc(sizeof(Lista));
    Lista *aux;
    int temp;

    if(novo){
        strcpy(nomeParticao, nome);
        novo->nomeParticao = nomeParticao;
        novo->prox = NULL;
    }

    if(lista == NULL){
        lista = novo;

        temp = *tamanho_da_lista;
        temp++;
        *tamanho_da_lista = temp;
        
        return lista;
    } else {
        aux = lista;
        while(aux->prox){
            aux = aux->prox;
        }
        aux->prox = novo;
        aux = lista;
        temp = *tamanho_da_lista;
        temp++;
        *tamanho_da_lista = temp;
        return aux;
    }
    exit(EXIT_FAILURE);
}

Lista* remove_inicio(Lista* lista){
    if(lista != NULL){
        Lista* temp = lista;
        lista = lista->prox;
        free(temp);
        return lista;
    }

    return NULL;
}

void imprime(Lista* lista){
    Lista* p;
    for(p = lista; p != NULL; p = p->prox){
        printf("Nome particao: %s", p->nomeParticao);
        printf("\n");
    }
}