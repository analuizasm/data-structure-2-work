#ifndef LISTA_ENCADEADA_H_INCLUDED
#define LISTA_ENCADEADA_H_INCLUDED
#define M 10000


typedef struct lista Lista;

struct lista {
   char *nomeParticao;
    Lista* prox;
};
Lista* lst_cria(void); //criar lista
void libera_lst(Lista* lista);
Lista* lista_insere(Lista *lista, char *nome, int* tamanho_da_lista);  //insere elemento na lista
Lista* remove_inicio(Lista* lista); //remove primeiro elemento da lista
void imprime(Lista* lista); //imprime lista



#endif //LISTA_ENCADEADA_H_INCLUDED