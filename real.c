#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node{
    char nome[999];
    int periodo;
    int burst;
    int indice;
    int indiceOriginal;
    struct node *next;
};

void addOrdenada(struct node **head, int periodoAtual, char nomeAtual[999], int burstAtual, int indiceAtual, int indiceOriginal);
void print_fila(struct node *head);
void delFila(struct node **head);
char* acharNome(struct node *head, int indice);
void ordenaIndice(struct node **head);
char* acharNomeOriginal(struct node *head, int indiceOriginal);
int valorCondizente(struct node *head, int *array, int indiceOrigem);
int valorPeriodoRestante(int *arrayP, int tempo, int indiceA);
int quantosTerminam(int *array, int tempo, int i, int qtdEl);

// Função para o algoritmo Rate Monotonic
int rate_monotonic(char *namefile) {
    FILE *arquivo = fopen(namefile, "r");

    // Verifica se o arquivo foi aberto com sucesso
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }
    //-------------------------------------------------------------------//
    
    int TEMPOMAX;//tempo de exec
    fscanf(arquivo, "%d" , &TEMPOMAX);//le o tempo
    
    char nome[999];
    int periodo;
    int burst;
    struct node *headTemp = NULL;
    int qtdElementos=0;

    //lendo os termos
    while (fscanf(arquivo, "%s" "%d" "%d",nome, &periodo, &burst) == 3) {
        addOrdenada(&headTemp,periodo,nome,burst,qtdElementos, qtdElementos);
        qtdElementos++;
    }

    // Fecha o arquivo
    fclose(arquivo);
    //ordena indice
    ordenaIndice(&headTemp);

    int arrayPeriodos[qtdElementos];
    int arrayBurst[qtdElementos];
    int arrayBurstRecarregador[qtdElementos];
    int i = 0, tempo = 0, contz = 0, tempoIdle=0;
    
    struct node *beta = headTemp;
    for(int a=0 ; a<qtdElementos; a++){
        arrayPeriodos[a] = beta->periodo;
        arrayBurst[a] = beta->burst;
        arrayBurstRecarregador[a] = beta->burst;
        beta = beta->next;
    }
    //abrir o arquivo
    FILE *fptr;
    fptr = fopen("rate.out", "w");
    if(fptr == NULL){
        printf("error");
    }
    fprintf(fptr, "EXECUTION BY RATE\n");
    fclose(fptr);
    //execução

    int tempoExec = 0, tempoHold=0;
    int arrayLost[qtdElementos];
    int arrayCompleted[qtdElementos];
    int arrayKilled[qtdElementos];
    for(int l = 0; l<qtdElementos;l++){
        arrayLost[l] = 0;
        arrayCompleted[l] = 0;
        arrayKilled[l] = 0;
    }
    char* nomeAtual;
    
    while (tempo < TEMPOMAX){
        int x = quantosTerminam(arrayPeriodos, tempo, i, qtdElementos);
        for(int j = 0; j <= (qtdElementos-1); j++){
            if(x == 0){
                if (tempo>0){
                    if (tempo % arrayPeriodos[j] == 0){
                        //printf("Entrou Recarga ");
                        //printf("%d\n", arrayBurst[j]);
                        //printf("i%d j%d tempo %d\n",i ,j, tempo);
                        if(arrayBurst[i] != 0 && i == j){
                            nomeAtual = acharNome(headTemp, i);
                            fptr = fopen("rate.out", "a");
                            fprintf(fptr, "[%s] for %d units - L\n", nomeAtual, tempoExec);
                            fclose(fptr);
                            arrayLost[i]++;
                            tempoExec = 0;
                        }else if(arrayBurst[j] != 0 && j != i){
                            arrayLost[j]++;
                        }
                        arrayBurst[j] = arrayBurstRecarregador[j];
                        if(i>j){
                            if(tempoExec != 0){
                                tempoHold = tempoExec - arrayBurstRecarregador[i];
                                tempoHold = tempoHold*(-1);
                                nomeAtual = acharNome(headTemp, i);
                                fptr = fopen("rate.out", "a");
                                fprintf(fptr, "[%s] for %d units - H\n", nomeAtual, tempoExec);
                                fclose(fptr);
                            }
                            tempoExec = 0;
                        }
                    }
                }
            }else{
                if (tempo>0){
                    if (tempo % arrayPeriodos[j] == 0){
                        //printf("i%d j%d tempo %d\n",i ,j, tempo);
                        if(arrayBurst[i] != 0 && i == j){
                            nomeAtual = acharNome(headTemp, i);
                            fptr = fopen("rate.out", "a");
                            fprintf(fptr, "[%s] for %d units - L\n", nomeAtual, tempoExec);
                            fclose(fptr);
                            arrayLost[i]++;
                            tempoExec = 0;
                        }else if(arrayBurst[j] != 0 && j != i){
                            arrayLost[j]++;
                        }
                        arrayBurst[j] = arrayBurstRecarregador[j];
                    }
                }
            }
        }
        i=0;
        for(int gama = i; gama < qtdElementos; gama++){
            if(arrayBurst[gama]==0){
                i++;
                if(i == qtdElementos){
                    i =0;
                }
            }else{
                break;
            }
        }
        if(arrayBurst[i] != 0){
            if(tempoIdle != 0){
                fptr = fopen("rate.out", "a");
                fprintf(fptr, "idle for %d units\n", tempoIdle);
                fclose(fptr);
            }
            tempoIdle = 0;
            arrayBurst[i] -= 1;
            //printf("%d %d\n",arrayBurst[i], arrayPeriodos[i]);
            tempoExec++;
            if(arrayBurst[i] == 0){
                //printf("Completo %d\n", i);
                arrayCompleted[i]++;
                nomeAtual = acharNome(headTemp, i);
                fptr = fopen("rate.out", "a");
                fprintf(fptr, "[%s] for %d units - F\n", nomeAtual, tempoExec);
                fclose(fptr);
                tempoExec = 0;
            }
            tempo++;
        }else{
            i++;
            if(i == qtdElementos){
                i = 0;
            }
            contz = 0;
            for(int k = 0; k < (qtdElementos); k++){
                contz += arrayBurst[k];
                //printf("%d ", arrayBurst[k]);
            }
            //printf("%d", contz);
            if(contz == 0){
                tempo++;
                tempoIdle++;
                //printf("Entrou idle\n");
            }
        }
        //printf("%d ", tempo);
    }
    if(tempoIdle != 0){
        fptr = fopen("rate.out", "a");
        fprintf(fptr, "idle for %d units\n", tempoIdle);
        fclose(fptr);
    }



    if(tempo == TEMPOMAX){
        for(int j = 0; j <= (qtdElementos-1);j++){
            if (tempo%arrayPeriodos[j] == 0){
                if (arrayBurst[j] != 0){
                    arrayLost[j]++;
                }
                
                arrayBurst[j] = arrayBurstRecarregador[j];
                //printf("Entrou Recarga ");
                //printf("%d\n", arrayPeriodos[j]);
            }
            
        }
    }

    if(arrayBurst[i] != 0 && tempoExec > 0 && (TEMPOMAX%arrayPeriodos[i] == 0)){
        nomeAtual = acharNome(headTemp, i);
        fptr = fopen("rate.out", "a");
        fprintf(fptr, "[%s] for %d units - L\n", nomeAtual, tempoExec);
        fclose(fptr);
        tempoExec = 0;
    }

    for(int alfa = 0; alfa<(qtdElementos);alfa++){
        if(arrayBurst[alfa] != 0){
            arrayKilled[alfa]++;
        }
    }
   
    if(arrayBurst[i] != 0 && tempoExec > 0){
        nomeAtual = acharNome(headTemp, i);
        fptr = fopen("rate.out", "a");
        fprintf(fptr, "[%s] for %d units - K\n", nomeAtual, tempoExec);
        fclose(fptr);
        
    }
    
    int indiceNome =0;
    fptr = fopen("rate.out", "a");
    fprintf(fptr, "\n");
    fprintf(fptr, "LOST DEADLINES\n");
    for(int l = 0; l<qtdElementos;l++){
        nomeAtual = acharNomeOriginal(headTemp, l);
        indiceNome = valorCondizente(headTemp, arrayLost, l);
        fprintf(fptr, "[%s] %d\n", nomeAtual, indiceNome);
    }
    fprintf(fptr, "\n");
    fprintf(fptr, "COMPLETE EXECUTION\n");
    for(int l = 0; l<qtdElementos;l++){
        nomeAtual = acharNomeOriginal(headTemp, l);
        indiceNome = valorCondizente(headTemp, arrayCompleted, l);
        fprintf(fptr, "[%s] %d\n", nomeAtual, indiceNome);
    }
    fprintf(fptr, "\n");
    fprintf(fptr, "KILLED\n");
    for(int l = 0; l<qtdElementos;l++){
        nomeAtual = acharNomeOriginal(headTemp, l);
        indiceNome = valorCondizente(headTemp, arrayKilled, l);
        if(l < (qtdElementos-1)){
            fprintf(fptr, "[%s] %d\n", nomeAtual, indiceNome);
        }else{
            fprintf(fptr, "[%s] %d", nomeAtual, indiceNome);
        }
    }
    fclose(fptr);

    
    //------------------------------------------------------------------//
    delFila(&headTemp);

    return 0;
}

// Função para o algoritmo Earliest Deadline First
int earliest_deadline_first(char *namefile) {
    // Abre o arquivo em modo de leitura
    FILE *arquivo = fopen(namefile, "r");

    // Verifica se o arquivo foi aberto com sucesso
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }
    //-------------------------------------------------------------------//
    
    int TEMPOMAX;//tempo de exec
    fscanf(arquivo, "%d" , &TEMPOMAX);//le o tempo
    
    char nome[999];
    int periodo;
    int burst;
    struct node *headTemp = NULL;
    int qtdElementos=0;

    //lendo os termos
    while (fscanf(arquivo, "%s" "%d" "%d",nome, &periodo, &burst) == 3) {
        addOrdenada(&headTemp,periodo,nome,burst,qtdElementos, qtdElementos);
        qtdElementos++;
    }

    // Fecha o arquivo
    fclose(arquivo);
    //ordena indice
    ordenaIndice(&headTemp);

    int arrayPeriodos[qtdElementos];
    int arrayBurst[qtdElementos];
    int arrayBurstRecarregador[qtdElementos];
    int i = 0, tempo = 0, contz = 0, tempoIdle=0;
    
    struct node *beta = headTemp;
    for(int a=0 ; a<qtdElementos; a++){
        arrayPeriodos[a] = beta->periodo;
        arrayBurst[a] = beta->burst;
        arrayBurstRecarregador[a] = beta->burst;
        beta = beta->next;
    }
    //abrir o arquivo
    FILE *fptr;
    fptr = fopen("edf.out", "w");
    if(fptr == NULL){
        printf("error");
    }
    fprintf(fptr, "EXECUTION BY EDF\n");
    fclose(fptr);
    //execução

    int tempoExec = 0, tempoHold=0;
    int arrayLost[qtdElementos];
    int arrayCompleted[qtdElementos];
    int arrayKilled[qtdElementos];
    int arrayPeriodoRemanecente[qtdElementos];
    for(int l = 0; l<qtdElementos;l++){
        arrayLost[l] = 0;
        arrayCompleted[l] = 0;
        arrayKilled[l] = 0;
    }
    char* nomeAtual;
    
    while (tempo < TEMPOMAX){
        for(int j = 0; j <= (qtdElementos-1); j++){
            if (tempo>0){
                if (tempo % arrayPeriodos[j] == 0){
                    //printf("Entrou Recarga ");
                    //printf("%d\n", arrayPeriodos[j]);
                    if(arrayBurst[i] != 0 && i == j && tempoExec != 0){
                        nomeAtual = acharNome(headTemp, i);
                        fptr = fopen("edf.out", "a");
                        fprintf(fptr, "[%s] for %d units - L\n", nomeAtual, tempoExec);
                        fclose(fptr);
                        arrayLost[i]++;
                        tempoExec = 0;
                    }else if(arrayBurst[j] != 0){
                        arrayLost[j]++;
                    }
                    arrayBurst[j] = arrayBurstRecarregador[j];
                    int restI = valorPeriodoRestante(arrayPeriodos, tempo, i);
                    if(restI>arrayPeriodos[j]){
                        if(tempoExec != 0){
                            tempoHold = tempoExec - arrayBurstRecarregador[i];
                            tempoHold = tempoHold*(-1);
                            nomeAtual = acharNome(headTemp, i);
                            fptr = fopen("edf.out", "a");
                            fprintf(fptr, "[%s] for %d units - H\n", nomeAtual, tempoExec);
                            fclose(fptr);
                        }
                        tempoExec = 0;
                    }
                }
            }
        }
        for(int gama = 0; gama < (qtdElementos); gama++){
            arrayPeriodoRemanecente[gama] = valorPeriodoRestante(arrayPeriodos, tempo, gama);
        }
        int rodadas = 0;
        int q = i;
        int indiceReturn = 0;
        int auxMenor = 9999;
        while(rodadas != qtdElementos){
            if(q == qtdElementos){
                q = 0;
            }
            if(arrayBurst[q] != 0){
                int auxTemp = arrayPeriodoRemanecente[q];
                if(auxTemp < auxMenor){
                    auxMenor = auxTemp;
                    indiceReturn = q;
                }
                i = indiceReturn;
            }
            rodadas++;
            q++;
        }
        if(arrayBurst[i] != 0){
            if(tempoIdle != 0){
                fptr = fopen("edf.out", "a");
                fprintf(fptr, "idle for %d units\n", tempoIdle);
                fclose(fptr);
            }
            tempoIdle = 0;
            arrayBurst[i] -= 1;
            //printf("%d %d\n",arrayBurst[i], arrayPeriodos[i]);
            tempoExec++;
            if(arrayBurst[i] == 0){
                arrayCompleted[i]++;
                nomeAtual = acharNome(headTemp, i);
                fptr = fopen("edf.out", "a");
                fprintf(fptr, "[%s] for %d units - F\n", nomeAtual, tempoExec);
                fclose(fptr);
                tempoExec = 0;
            }
            tempo++;
        }else{
            contz = 0;
            for(int k = 0; k < (qtdElementos); k++){
                contz += arrayBurst[k];
                //printf("%d ", arrayBurst[k]);
            }
            //printf("%d", contz);
            if(contz == 0){
                tempo++;
                tempoIdle++;
                //printf("Entrou idle\n");
            }
        }
        //printf("%d ", tempo);
    }
    if(tempoIdle != 0){
        fptr = fopen("edf.out", "a");
        fprintf(fptr, "idle for %d units\n", tempoIdle);
        fclose(fptr);
    }

    if(tempo == TEMPOMAX){
        for(int j = 0; j <= (qtdElementos-1);j++){
            if (tempo%arrayPeriodos[j] == 0){
                arrayBurst[j] = arrayBurstRecarregador[j];
                //printf("Entrou Recarga ");
                //printf("%d\n", arrayPeriodos[j]);
            }
            
        }
    }
    for(int alfa = 0; alfa<(qtdElementos);alfa++){
        if(arrayBurst[alfa] != 0){
            arrayKilled[alfa]++;
        }
    }
   
    if(arrayBurst[i] != 0 && tempoExec > 0){
        nomeAtual = acharNome(headTemp, i);
        fptr = fopen("edf.out", "a");
        fprintf(fptr, "[%s] for %d units - K\n", nomeAtual, tempoExec);
        fclose(fptr);
        
    }
    
    int indiceNome =0;
    fptr = fopen("edf.out", "a");
    fprintf(fptr, "\n");
    fprintf(fptr, "LOST DEADLINES\n");
    for(int l = 0; l<qtdElementos;l++){
        nomeAtual = acharNomeOriginal(headTemp, l);
        indiceNome = valorCondizente(headTemp, arrayLost, l);
        fprintf(fptr, "[%s] %d\n", nomeAtual, indiceNome);
    }
    fprintf(fptr, "\n");
    fprintf(fptr, "COMPLETE EXECUTION\n");
    for(int l = 0; l<qtdElementos;l++){
        nomeAtual = acharNomeOriginal(headTemp, l);
        indiceNome = valorCondizente(headTemp, arrayCompleted, l);
        fprintf(fptr, "[%s] %d\n", nomeAtual, indiceNome);
    }
    fprintf(fptr, "\n");
    fprintf(fptr, "KILLED\n");
    for(int l = 0; l<qtdElementos;l++){
        nomeAtual = acharNomeOriginal(headTemp, l);
        indiceNome = valorCondizente(headTemp, arrayKilled, l);
        if(l < (qtdElementos-1)){
            fprintf(fptr, "[%s] %d\n", nomeAtual, indiceNome);
        }else{
            fprintf(fptr, "[%s] %d", nomeAtual, indiceNome);
        }
    }
    fclose(fptr);

    
    //------------------------------------------------------------------//
    delFila(&headTemp);

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s [namefile]\n", argv[0]);
        return 1;
    }

    char *namefile = argv[1];
    char *program_name = strrchr(argv[0], '/');
    if (program_name != NULL) {
        program_name++; 
    } else {
        program_name = argv[0]; 
    }

    if (strcmp(program_name, "rate") == 0) {
        rate_monotonic(namefile);
    } else if (strcmp(program_name, "edf") == 0) {
        earliest_deadline_first(namefile);
    } else {
        printf("Invalid program name. Please use 'rate' or 'edf'\n");
        return 1;
    }

    return 0;
}

void addOrdenada(struct node **head, int periodoAtual, char nomeAtual[999], int burstAtual, int indiceAtual, int indiceOriginal){
    struct node *novo = (struct node *)malloc(sizeof(struct node));
    strcpy(novo->nome, nomeAtual);
    novo->burst = burstAtual;
    novo->periodo = periodoAtual;
    novo->indice = indiceAtual;
    novo->indiceOriginal = indiceOriginal;
    novo->next = NULL;

    if (*head == NULL || periodoAtual < (*head)->periodo) {
        novo->next = *head;
        *head = novo;
    } else {
        struct node *n = *head;
        while (n->next != NULL && n->next->periodo <= periodoAtual) {
            n = n->next;
        }
        novo->next = n->next;
        n->next = novo;
    }
}

void print_fila(struct node *head) {
    struct node *n = head;
    printf("\n");
    while (n != NULL) {
        printf("%s %d\n", n->nome, n->indice);
        n = n->next;
    }
    printf("\n");
}

void delFila(struct node **head) {
    if(*head != NULL){
    struct node *n = *head;
    struct node *temp;
    while (n != NULL) {
        temp = n;
        free(temp);
        n = n->next;
    }
    }
    *head = NULL;
}

char* acharNome(struct node *head, int indice){
    struct node *n = head;
    while (n->indice != indice) {
        n = n->next;
    }
    return n->nome;
}

void ordenaIndice(struct node **head){
    struct node *n = *head;
    int ind = 0;
    while (n != NULL) {
        n->indice = ind;
        n = n->next;
        ind++;
    }
}

char* acharNomeOriginal(struct node *head, int indiceOriginal){
    struct node *n = head;
    while (n->indiceOriginal != indiceOriginal) {
        n = n->next;
    }
    return n->nome;
}

int valorCondizente(struct node *head, int *array, int indiceOrigem){
    struct node *n = head;
    while (n->indiceOriginal != indiceOrigem) {
        n = n->next;
    }
    int indiceArr = n->indice;
    return array[indiceArr];
}

int valorPeriodoRestante(int *arrayP, int tempo, int indiceA){
    int tempoRest = tempo +1;
    while (tempoRest % arrayP[indiceA] != 0){
        tempoRest++;
    }
    return tempoRest - tempo;
}

int quantosTerminam(int *array, int tempo, int i, int qtdEl){
    int cont= 0;
    for(int p=0; p<qtdEl; p++){
        if(tempo%array[p] == 0){
            cont++;
        }
    }
    if(tempo%array[i]==0){
        return cont;
    }
    return 0;
}