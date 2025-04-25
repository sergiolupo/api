#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#define INFINITE 4294967295
#define string char*
#define null NULL

typedef struct stazione{
    int distanza;
    int num_auto;
    int capacity;
    int *macchine;
}stazione;

typedef struct bstNode {
    struct stazione *stazione;
    struct bstNode *parent;
    struct bstNode *left;
    struct bstNode *right;
}bstNode;

//l'autostrada è un bst di stazioni
typedef struct bstNode* bst;

typedef enum color {
    WHITE,
    BLACK,
    GREY
}color;

typedef struct node {
    int distanza;
    int num_passi;
    struct node *next;
}node;

typedef struct node* lista;

typedef struct graph {
    int distanza;
    int auto_max;
    int num_passi;
    color colore;
}graph;

typedef struct queue {
    lista head;
    lista tail;
}queue;

typedef struct pair {
    int distanza;
    int num_passi;
}pair;

typedef struct support {
    int distanza;
    int num_passi;
    int prec;
    int target;
}support;

typedef struct dynamicArray {
    int size;
    int capacity;
    support* array;
}dynamicArray;


typedef enum command {
    aggiungi_stazione,
    aggiungi_auto,
    demolisci_stazione,
    rottama_auto,
    pianifica_percorso
}command;

command getCommand(string cmd){
    switch(cmd[0]) {
        case 'a':
            if(cmd[9] == 's')
                return aggiungi_stazione;
            else return aggiungi_auto;
        case 'd':
            return demolisci_stazione;
        case 'r':
            return rottama_auto;
        case 'p':
            return pianifica_percorso;
        default: return aggiungi_stazione;  //useless
    }
}

//-------------------------------------------------------------INIZIO FUNZIONI DI INIZIALIZZAZIONE-------------------------------------------------------------

lista creaNodo(int dist)
{
    lista newNode = malloc(sizeof(node));
    newNode->next = null;
    newNode->distanza = dist;
    return newNode;
}

bst creaBstNode(stazione *s){
    bst newNode = (bst) malloc(sizeof(struct bstNode));
    if(newNode != null)
    {
        newNode->parent = null;
        newNode->left = null;
        newNode->right = null;
        newNode->stazione = s;
    }
    return newNode;
}

queue* creaQueue(){
    queue* q = malloc(sizeof(queue));
    q->head = null;
    q->tail = null;
    return q;
}

stazione* creaStazione(int dist) {
    stazione* s = (stazione*) malloc(sizeof(stazione));
    if (s != null) {
        s->distanza = dist;
        s->num_auto = 0;


        s->macchine = (int*) malloc(sizeof(int));
        s->capacity = 1;
    }
    return s;
}

pair* creaCoppia(int dist, int np){
    pair* p = (pair*) malloc(sizeof(pair));
    p->distanza = dist;
    p->num_passi = np;
    return p;
}

support* creaSupport(int dist,int np, int prec) {
    support* sup = malloc(sizeof(support));
    sup->distanza = dist;
    sup->num_passi = np;
    sup->prec = prec;
    return sup;
}

dynamicArray* creaDynamicArray() {
    dynamicArray* da = malloc(sizeof(dynamicArray));
    da->size = 0;
    da->capacity = 1;
    da->array =(support*) malloc(sizeof(support));
    return da;
}

//-------------------------------------------------------------FINE FUNZIONI DI INIZIALIZZAZIONE-------------------------------------------------------------

//-------------------------------------------------------------INIZIO FUNZIONI DI DISTRUZIONE-------------------------------------------------------------

void distruggiStazione(stazione* s) {
    free(s->macchine);

    free(s);
}

void distruggiAutostrada(bst autostrada) {
    if(autostrada == null)
        return;
    distruggiAutostrada(autostrada->left);
    distruggiAutostrada(autostrada->right);

    distruggiStazione(autostrada->stazione);
    free(autostrada);
    
}

void distruggiDynamicArray(dynamicArray* da){
    free(da->array);
    free(da);
}

void distruggiCoda(queue* q){
    lista curr,temp;
    curr = q->head;

    while(curr != null){
        temp = curr;
        curr = curr->next;
        free(temp);
    }
    free(q);
}



//-------------------------------------------------------------FINE FUNZIONI DI DISTRUZIONE-------------------------------------------------------------

//-------------------------------------------------------------INIZIO FUNZIONI AUSILIARIE-------------------------------------------------------------

//is used to get the index of the machine that has the autonomia == autonomia
int getIndex(stazione *s,int autonomia){
    
    int low = 0;
    int high = s->num_auto - 1;
    int mid;

    while (low <= high) {
        mid = (low + high) / 2;

        if (s->macchine[mid] == autonomia) {
            // Elemento presente
            return mid; 
        } else if (s->macchine[mid] < autonomia) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    //l'elemento non c'è
    return -1;
}

// restituisce l'indice dell'elemento che ha distanza = dist nell'array di support
int getSupIndex(dynamicArray* da,int dist,bool andata){
    int low = 0;
    int high = da->size - 1;
    int mid;

    while (low <= high) {
        mid = (low + high) / 2;

        // Elemento presente
        if (da->array[mid].distanza == dist)
            return mid;

        else if(andata) {
            if (da->array[mid].distanza < dist) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        } else {
            if (da->array[mid].distanza > dist) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }

    }
    // Elemento non presente
    return -1;
}

int getGraphIndex(graph* g,int dist,int dim){
    int low = 0;
    int high = dim - 1;
    int mid;

    while (low <= high) {
        mid = (low + high) / 2;
        // Elemento presente
        if (g[mid].distanza == dist)
            return mid; 
        else 
            if (g[mid].distanza < dist)
                low = mid + 1;
            else 
                high = mid - 1;
    }
    // Elemento non presente
    return -1;
}

int getDaIndex(dynamicArray* da,int dist){
    int low = 0;
    int high = da->size - 1;
    int mid;

    while (low <= high) {
        mid = (low + high) / 2;
        // Elemento presente
        if (da->array[mid].distanza == dist)
            return mid; 
        else 
            if (da->array[mid].distanza > dist)
                low = mid + 1;
            else 
                high = mid - 1;
    }
    // Elemento non presente
    return -1;
}



//return the station with distanza == dist if is present in the autostrada
bst bstSearch(bst *autostrada,int dist){
    bst cur = *autostrada;
    
    while(cur != null)
    {
        if(cur->stazione->distanza == dist)
            return cur;
        else if(dist < cur->stazione->distanza)
            cur = cur->left;
        else
            cur = cur->right;
    }
    return cur;
}

// Trova l'elemento minimo di un un albero
bst bstMin (bst tree) {
    if (tree == null)
        return null;

    while (tree->left != null)
        tree = tree->left;

    return tree;
}

bst bstMax (bst tree) {
    if(tree == null)
        return null;
    
    while(tree->right != null)
        tree = tree->right;

    return tree;
}

// Trova l'elemento successore di x
bst bstSucc (bst x) {
    if (x->right != null)
        return bstMin(x->right);

    bst y = x->parent;

    while (y != null && y->right == x) {
        x = y;
        y = y->parent;
    }

    return y;
}

// Trova l'elemento predecessore di x
bst bstPrec(bst x) {
    if(x->left != null)
        return bstMax(x->left);
    
    bst y = x->parent;

    while(y != null && y->left == x) {
        x = y;
        y = y->parent;
    }

    return y;
}

//restituisce il numero di stazioni che ci sono dalla start alla end
int getNumStations(bst start,bst end){
    int n=0;
    bst curr = start;
    while(curr->stazione->distanza != end->stazione->distanza){
        n++;
        curr = bstSucc(curr);
    }
    return n;
}
void populateGraph(bst tree,graph* g){
    int i=0;
    bst curr = bstMin(tree);
    while(curr!=null){
        g[i].distanza = curr->stazione->distanza;
        g[i].auto_max = curr->stazione->macchine[curr->stazione->num_auto-1];

        g[i].num_passi = INFINITE;
        g[i].colore = WHITE;

        i++;
        curr = bstSucc(curr); 
    }
}

//aggiunge il nodo nella coda
void enqueue(queue* q,lista n){
    if(q->head == null)
    {
        q->head = n;
        q->tail = n;       
    }
    else {
        q->tail->next = n;
        q->tail = n;
    }
}

bool isEmpty(queue* q){
    if(q->head == null)
        return true;
    else   
        return false;
}

pair* dequeue(queue* q) {
    if(!isEmpty(q)){
        pair* p ;
        lista temp = q->head;
        q->head = q->head->next;
        p = creaCoppia(temp->distanza,temp->num_passi);
        free(temp);
        return p;
    }else
        return creaCoppia(-1,-1);
}

void addSup(dynamicArray* da, support* sup){
    int k = 2;
    if(da->size == da->capacity){
        da->capacity = k * da->capacity;
        da->array = (support*) realloc(da->array,da->capacity * sizeof(support));
    }
    da->array[da->size] = *sup;
    da->size++;
}

void printSup(dynamicArray* da){
    printf("inzio sup:\n");
    for(int i = 0;i < da->size; i++)
    {
        printf("%d) distanza: ",i+1);
        printf("%d ", da->array[i].distanza);
        for(int j = 0; j < i; j++) {
            printf("num_passi: ");
            printf("%d ->", da->array[i].num_passi);
        }
        for(int j = 0; j < i; j++) {
            printf(" precedenti: ");
            printf("%d ->", da->array[i].prec);
        }
        printf("\n");
    }
    printf("\n");
    printf("fine sup:\n");
}

void printQueue(queue* q){

    if(q->head != null) {
        printf("inzio coda\n");
        lista curr;
        curr = q->head;
        while(curr != null)
        {
            printf("%d -> ",curr->distanza);
            curr = curr->next;
        }
        printf("NULL\n");
        printf("fine coda\n");
    } else 
        printf("la coda è vuota\n");
}

//-------------------------------------------------------------FINE FUNZIONI AUSILIARIE-------------------------------------------------------------

//-------------------------------------------------------------INIZIO FUNZIONI DEI COMANDI-------------------------------------------------------------


bool aggiungiStazione(bst *autostrada,stazione *s)
{
    bst pre = null;
    bst cur = *autostrada;

    while (cur != null) {
        pre = cur;

        if (s->distanza < cur->stazione->distanza)
            cur = cur->left;

        else if(s->distanza > cur->stazione->distanza)
            cur = cur->right;
        else
        {
            return false;
        }
            
    }

    bst temp = creaBstNode(s);
    temp->parent = pre;

    if (pre == null){
        *autostrada = temp;

        return true;
    }

    else if (s->distanza < pre->stazione->distanza){
        pre->left = temp;

        return true;
    }

    else if (s->distanza > pre->stazione->distanza){
        pre->right = temp;

        return true;
    }
    else{

        return false;
    }
        
}

// Rimozione di un nodo
void demolisciStazione (bst* autostrada, bst x) {
    bst daCanc;

    distruggiStazione(x->stazione);

    if (x->left == null || x->right == null)
        daCanc = x;

    else
        daCanc = bstSucc(x);

    bst sottoA;

    if (daCanc->left != null)
        sottoA = daCanc->left;

    else
        sottoA = daCanc->right;

    if (sottoA != null)
        sottoA->parent = daCanc->parent;

    if (daCanc->parent == null)
        *autostrada = sottoA;

    else if (daCanc == daCanc->parent->left)
        daCanc->parent->left = sottoA;

    else
        daCanc->parent->right = sottoA;

    if (daCanc != x)
        x->stazione = daCanc->stazione;
    
    free(daCanc);
}

void aggiungiAuto(stazione* s,int autonomia)
{
    int i,k=2;
    bool inserted = false;
    if(s->num_auto != 0)
    {   
        if(s->num_auto == s->capacity){
            s->capacity = s->capacity * k;
            s->macchine = realloc(s->macchine,s->capacity * sizeof(int));
        }
        if(s->num_auto < 512){
            for(i = s->num_auto-1; i >= 0 && !inserted; i--){
                if(autonomia >= s->macchine[i]){
                    s->macchine[i+1] = autonomia;
                    s->num_auto++;
                    inserted = true;
                }
                else
                {
                    s->macchine[i+1] = s->macchine[i];
                }
            }
            if(!inserted){
                s->macchine[0] = autonomia; 
                s->num_auto++;
            }   
        } else
            printf("stai cercando di aggiungere una macchina in più di quel che puoi\n");

    } else {
        s->macchine[s->num_auto] = autonomia;
        s->num_auto++;
    }
}

bool rottamaAuto(stazione* s,int autonomia)
{
    int index,i;
    if(s->num_auto != 0)
    {
        index = getIndex(s,autonomia);
        if(index != -1 && s->macchine[index] == autonomia)
        {
            for(i=index; i < s->num_auto-1;i++){
                s->macchine[i] = s->macchine[i+1];
            }
            s->macchine[s->num_auto-1] = -1;//istruzione ridondante
            s->num_auto--;
            return true;
        } else
            return false;
    } else 
        return false;
}

bool pianificaPercorso(bst* autostrada,int dist_partenza,int dist_arrivo,int num_staz)
{
    bst curr = bstSearch(autostrada,dist_partenza);
    if(curr == null)
        return false;
    bst end = bstSearch(autostrada,dist_arrivo);
    if(end == null)
        return false;
    graph* g = (graph*) malloc(sizeof(graph)*num_staz);

    bool andata;
    queue* q = creaQueue();
    dynamicArray* da = creaDynamicArray();
    support* sup = malloc(sizeof(support));
    pair* u;
    lista s;
    int i,j,idx,ic,*path,index,target,ida,corr;
    i=0;
    idx = 0;

    if(dist_partenza < dist_arrivo)
    {
        andata = true;
        //popolo il grafo 
        populateGraph(*autostrada,g);
        index = getGraphIndex(g,dist_partenza,num_staz);
        g[index].colore = GREY;
        g[index].num_passi = 0;
        s = creaNodo(dist_partenza);
        s->num_passi = 0;
        enqueue(q,s);
        sup->distanza = s->distanza;
        sup->num_passi = 0;
        sup->prec = -1;
        addSup(da,sup);
        ida=0;

        while(!isEmpty(q)) {
            u = dequeue(q);
            index = getGraphIndex(g,u->distanza,num_staz);

            if(index != -1) {    //controllo ridondante
                j = getGraphIndex(g,dist_arrivo,num_staz);
                target = g[index].distanza + g[index].auto_max;
                while(g[j].distanza > target){
                    j--;
                }

                for(i = ida; i<da->size;i++){
                    da->array[i].target = g[j].distanza;
                }
                ida++;

                i = index;
                ic = idx+1;
                corr = getDaIndex(da,g[index].distanza);
                if(corr!=-1)
                {
                    while(ic < da->size){
                        if(da->array[ic].target >= da->array[ic].distanza && da->array[ic].prec > g[index].distanza && da->array[ic].num_passi >= sup->num_passi)
                        {
                            da->array[ic].num_passi = sup->num_passi;
                            if(sup->num_passi > da->array[corr].num_passi) 
                                da->array[ic].prec = g[index].distanza;
                        }
                        ic++;
                    }
                }
                while(i <= j){
                    if(g[i].colore == WHITE){
                        g[i].colore = GREY;
                        g[i].num_passi = u->num_passi + 1;
                        free(sup);
                        sup = creaSupport(g[i].distanza,g[i].num_passi,u->distanza);
                        s = creaNodo(g[i].distanza);
                        s->num_passi = g[i].num_passi;
                        enqueue(q,s);
                        addSup(da,sup);
                    }
                    i++;
                }
                idx++;
            }
            free(u);
        }

        if(da->size == 0 || (da->array[da->size-1].distanza != dist_arrivo || da->array[0].distanza != dist_partenza))
        {
            free(g);
            distruggiDynamicArray(da);
            distruggiCoda(q);
            free(sup);
            return false;
        }    

        path =(int*) malloc(da->size * sizeof(int));
        j=da->size-1;
        i=0;
        while(da->array[j].distanza > dist_partenza){
            path[i] = da->array[j].distanza;
            j = getSupIndex(da,da->array[j].prec,andata);
            i++;
        }
        printf("%d ",dist_partenza);
        for(j=i-1;j>=0;j--){
            if(j==0)
                printf("%d",path[j]);
            else
                printf("%d ",path[j]);
        }
        printf("\n");

        free(g);
        distruggiDynamicArray(da);
        distruggiCoda(q);
        free(sup);
        free(path);
        return true;



    } else if(dist_partenza > dist_arrivo) 
        {
            andata = false;
            //popolo il grafo
            populateGraph(*autostrada,g);

            index = getGraphIndex(g,dist_partenza,num_staz);
            g[index].colore = GREY;
            g[index].num_passi = 0;
            //ricerca in ampiezza per trovare percorso minimo (BFS)
            i=0;
            
            s = creaNodo(dist_partenza);
            s->num_passi = 0;
            enqueue(q,s);
            sup->distanza = s->distanza;
            sup->num_passi = 0;
            sup->prec = -1;
            addSup(da,sup);
            idx = 0;
            ida = 0;

            while(!isEmpty(q)) {
                u = dequeue(q);
                index = getGraphIndex(g,u->distanza,num_staz);               
                j = getGraphIndex(g,dist_arrivo,num_staz);

                target = g[index].distanza - g[index].auto_max;
                while(g[j].distanza < target){ // si può ottimizzare con una ricerca binaria
                    j++;
                }

                for(i = ida; i<da->size;i++){
                    da->array[i].target = g[j].distanza;
                }
                ida++;

                i = index;
                ic = idx+1;
                corr = getDaIndex(da,g[index].distanza);

                if(corr != -1) 
                {
                    while(ic < da->size){
                        if(da->array[ic].target <= da->array[ic].distanza && da->array[ic].prec > g[index].distanza && da->array[ic].num_passi >= sup->num_passi)
                        {
                            da->array[ic].num_passi = sup->num_passi;
                            if(sup->num_passi > da->array[corr].num_passi) 
                                da->array[ic].prec = g[index].distanza;
                        }
                        ic++;
                    }
                }
                    
                while(i >= j){

                    if(g[i].colore == WHITE){
                        g[i].colore = GREY;
                        g[i].num_passi = u->num_passi + 1;
                    
                        free(sup);
                        sup = creaSupport(g[i].distanza,g[i].num_passi,u->distanza);
                        s = creaNodo(g[i].distanza);
                        s->num_passi = g[i].num_passi;
                        enqueue(q,s);
                        addSup(da,sup);
                    }
                    i--;
                }
                idx++;                
                free(u);
            }

            if(da->size == 0 || (da->array[da->size-1].distanza != dist_arrivo || da->array[0].distanza != dist_partenza))
            {
                free(g);
                distruggiDynamicArray(da);
                distruggiCoda(q);
                free(sup);
                return false;
            }

            path =(int*) malloc(da->size * sizeof(int));
            j=da->size-1;
            i=0;
            while(da->array[j].distanza < dist_partenza){
                path[i] = da->array[j].distanza;
                j = getSupIndex(da,da->array[j].prec,andata);
                i++;
            }
            printf("%d ",dist_partenza);
            for(j=i-1;j>=0;j--){
                if(j==0)
                    printf("%d",path[j]);
                else
                    printf("%d ",path[j]);
            }
            printf("\n");

            free(g);
            distruggiDynamicArray(da);
            distruggiCoda(q);
            free(sup);
            free(path);
            return true;

        } else {
            printf("%d %d\n",dist_partenza,dist_arrivo);
            free(g);
            distruggiDynamicArray(da);
            distruggiCoda(q);
            free(sup);
            return true;
        }
}
//-------------------------------------------------------------FINE FUNZIONI DEI COMANDI-------------------------------------------------------------



int main() {
    bst autostrada = null,temp=null;
    stazione *s = null;
    int dist,num_auto,autonomia,dist_partenza,dist_arrivo,num_staz=0;
    string comando =(string) malloc(sizeof(char)*18 + 1);
    while(scanf("%s",comando) != EOF){
        switch (getCommand(comando)) {
            case aggiungi_stazione:
                if(scanf("%d %d",&dist,&num_auto)){
                    s = creaStazione(dist);
                }

                if(aggiungiStazione(&autostrada,s))
                {
                    while(getchar() != '\n')
                    {
                        if(scanf("%d",&autonomia))
                            aggiungiAuto(s,autonomia);
                    }
                    
                    printf("aggiunta\n");
                    num_staz++;

                } else {
                    while(getchar() != '\n');
                    printf("non aggiunta\n");
                    free(s->macchine);
                    free(s);
                }
                break;
            case aggiungi_auto:
                if(scanf("%d %d",&dist,&autonomia)) {
                    temp = bstSearch(&autostrada,dist);
                    if(temp != null)
                    {
                        aggiungiAuto(temp->stazione,autonomia);
                        printf("aggiunta\n");
                    } else {
                        printf("non aggiunta\n");
                      }
                }

                break;
            case demolisci_stazione:
                if(scanf("%d",&dist))
                    temp = bstSearch(&autostrada,dist);
                if(temp != null)
                {
                    demolisciStazione(&autostrada,temp);
                    printf("demolita\n");
                    num_staz--;
                } else {
                    printf("non demolita\n");
                }

                break;
            case rottama_auto:
                if(scanf("%d %d",&dist,&autonomia)) 
                    temp = bstSearch(&autostrada,dist);
                if(temp != null)
                {
                    if(rottamaAuto(temp->stazione,autonomia))
                    {                    
                        printf("rottamata\n");
                    } else{
                        printf("non rottamata\n");
                    }
                }
                else {
                    printf("non rottamata\n");
                }

                break;
            case pianifica_percorso:
                if(scanf("%d %d",&dist_partenza,&dist_arrivo))
                {
                    if(!pianificaPercorso(&autostrada,dist_partenza,dist_arrivo,num_staz))
                        printf("nessun percorso\n");
                }

                break;
            default:
                break;
        }
        
    }

    free(comando);
    distruggiAutostrada(autostrada);

    return 0;
}

