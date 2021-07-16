// Método de ordenação por intercalação polifásica
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define phi 1.618033989
#define sqrt5 2.236067977


typedef struct Arquivo {
    FILE * fp;
    int g;
    char* n;
} Arq;


/* numero de fibonacci na posicao n */
int fib(int n) {
    return (pow(phi, n) - pow(-phi, -n)) / sqrt5;
}

/* fecha e abre arquivo com w+ */
void limpaArquivo(Arq *a) {
    fclose(a->fp);
    a->fp = fopen(a->n, "w+");
}

/* printa arquivo desde 0 até EOF */
void printaTudo(Arq *a) {

    if (a < 0) {
        return;
    }

    int p;
    int counter = 0;

    FILE *f = a->fp;
    int pos = ftell(f);
    rewind(f);
    while (!feof(f)) {
        for (int i = 0; i < a->g; i++) {
            fread(&p, sizeof(int), 1, f);
            if (feof(f)) {
                //printf("EOF");
                break;
            }
            printf("%i ", p);
            counter++;
        }
    }

    fseek(f, pos, SEEK_SET);
}

/* Lê um inteiro, incrementa seu contador e retorna se foi lido com sucesso */
int readFile(int *n, int *fReads, FILE *f) {

    fread(n, sizeof(int), 1, f);

    (*fReads)++;

    return feof(f) ? NULL : 1;
}

/* retorna tamanho do arquivo */
int file_size(FILE* a) {

    int pos = ftell(a);
    rewind(a);

    int begin = ftell(a);
    fseek(a, 0, SEEK_END);

    int dist = ftell(a) - begin; 
    fseek(a, pos, SEEK_SET);

    printf("%d\n", dist);

    return dist/sizeof(int);
}

/* Copia f -> t e retorna quantidade de numeros escritos
n = -1 copia até o final do arquivo */
int copia(FILE *f, FILE *t, int n) {

    int b = 0;
    int tmp;

    for (; 1; b++) {
        if (b == n) break;
        fread(&tmp,sizeof(int),1,f);
        if (feof(f)) break;
        fwrite(&tmp,sizeof(int),1,t);
    }

    return b;

}

/* escreve 2 inteiros ordenadamente */
void escreve2(int a1_, int a2_, FILE* a3) {
    if (a1_ < a2_){
        fwrite(&a1_, sizeof(int), 1, a3);
        fwrite(&a2_, sizeof(int), 1, a3);
    } else {
        fwrite(&a2_, sizeof(int), 1, a3);
        fwrite(&a1_, sizeof(int), 1, a3);
    }
}

/* copiará a1 em a3 até que a2_ seja maior */
int copiaAteMenor_(int a1_, int a2_, int *a1reads, int *a1g, Arq *a1, Arq *a3) {
    while (a1_ <= a2_ && *a1reads < *a1g && !feof(a1->fp)) {
        //escrever conteudo de a1 até a2 ser menor
        fwrite(&a1_, sizeof(int), 1, a3->fp);
        readFile(&a1_, a1reads, a1->fp);
    }

    return a1_;
}

/* função principal que ordena tudo */
int ordena(Arq *a1, Arq *a2, Arq *a3, int a1_, int a2_, int *a1reads, int *a2reads, int *a1g, int *a2g) {
    //o else indica que a2 é menor que a
    if (*a1reads == *a1g) {
        a2_ = copiaAteMenor_(a2_, a1_, a2reads, a2g, a2, a3);
        escreve2(a1_, a2_, a3->fp);
        copia(a2->fp, a3->fp, *a2g-*a2reads);
        return 1;
    } else {
        //faltam ler coisas do a1
        
        while (*a1reads != *a1g && *a2reads != *a2g && !feof(a1->fp) && !feof(a2->fp)) {

            a2_ = copiaAteMenor_(a2_, a1_, a2reads, a2g, a2, a3);
            a1_ = copiaAteMenor_(a1_, a2_, a1reads, a1g, a1, a3);
        
            if (*a2reads == *a2g || feof(a2->fp)) {
                a1_ = copiaAteMenor_(a1_, a2_, a1reads, a1g, a1, a3);
                if (feof(a2->fp)) {
                    fwrite(&a1_, sizeof(int), 1, a3->fp);
                } else escreve2(a1_, a2_, a3->fp);
                copia(a1->fp, a3->fp, *a1g-*a1reads);
                *a1reads = *a1g;
                return 1;
            }

            if (*a1reads == *a1g || feof(a1->fp)) {
                a2_ = copiaAteMenor_(a2_, a1_, a2reads, a2g, a2, a3);
                if (feof(a2->fp)) {
                    fwrite(&a2_, sizeof(int), 1, a3->fp);
                } else escreve2(a1_, a2_, a3->fp);
                copia(a2->fp, a3->fp, *a2g-*a2reads);
                *a2reads = *a2g;
                return 1;
            }
        }
    }
    return 0;
}

/* retorna o ponteiro para arquivo que está em EOF */
Arq* poliphaseSort(Arq *a1, Arq *a2, Arq *a3, int tam, int max){

    int *a1g = &a1->g;
    int *a2g = &a2->g;
    int *a3g = &a3->g;

    *a3g = *a1g+*a2g;

    int a1_;
    int a2_;

    while (!feof(a1->fp) && !feof(a2->fp)) {
        // Quando o A1 acaba mas não tem EOF
        if (ftell(a1->fp)/sizeof(int) == tam && a1->g == 1) break;
        
        int a1reads = 0, a2reads = 0;
        while (1) {

            //LEITURA
            if (!readFile(&a1_, &a1reads, a1->fp)) break;
            if (!readFile(&a2_, &a2reads, a2->fp)) {
                fseek(a1->fp, ftell(a1->fp)-sizeof(int), SEEK_SET);
                break;
            }

            if (*a1g != 1 || *a2g != 1) {
                if (ordena(a1, a2, a3, a1_, a2_, &a1reads,
                           &a2reads, a1g, a2g))
                    break;
            } else {
                //estamos com grão 1 nos 2, entao é uma operação simples
                escreve2(a1_, a2_, a3->fp);
            }

            if (a1reads == *a1g && a2reads == *a2g || (feof(a1->fp) || feof(a2->fp)))
                break;
        }
    }

    //Hora de terminar a execução
    if (round(ftell(a3->fp)/sizeof(int)) >= max) return (void*) -1;

    // Retorna o arquivo que acabou
    if (feof(a1->fp)) { return a1; }
    if (feof(a2->fp)) { return a2; }
    if (feof(a3->fp)) { return a3; }

    // Quando o A1 acaba mas não tem EOF
    if (round(ftell(a1->fp)/sizeof(int)) == tam) return a1;

    return 0;
}

/* faz iteração entre o proprio arquivo*/
void  ordenaParcial(Arq *read1, Arq *read2, Arq *wr, int tam, int max) {

    Arq* k;
    limpaArquivo(read2);
    read1->g = wr->g;

    while (!feof(wr->fp)) {
        //copiar g numeros para read1 e armazenar em r1
        limpaArquivo(read1);
        copia(wr->fp, read1->fp, wr->g);
        rewind(read1->fp);
        if (feof(wr->fp)) break;

        //iterar read1 com o restante do arquivo da escrita
        k = poliphaseSort(read1, wr, read2, tam, max);

        if (k == wr) break;
    }

    //caso tam/grão for impar, então copia o restante que está em r1
    //para o arquivo final
    if ((int)ceil(tam/read1->g)%2 != 0 || tam%read1->g != 0)
        copia(read1->fp, read2->fp, read1->g);

    //rewind pra proxima leitura
    rewind(read2->fp);
}

/* prepara os arquivos para leitura */
void initiateValues(Arq* a1, Arq* a2, Arq* a3, char *argv[]) {
    FILE *numbers = fopen(argv[1], "r");
    if (!argv[1] || !numbers){
        printf("Uso: %s <nome_arquivo>\n", argv[0]);
        exit(1);
    }
    
    //Criamos nosso a1 com os valores de input
    a1->fp = fopen("a1", "w+");
    copia(numbers, a1->fp, -1);
    rewind(a1->fp);
    fclose(numbers);


    a2->fp = fopen("a2", "w+");
    a3->fp = fopen("a3", "w+");
                                //fopen com w+ faz com que os arquivos usados saiam vazios
    a1->g = a2->g = a3->g = 1;

    a1->n = "a1";
    a2->n = "a2";
    a3->n = "a3";
}


int main(int argc, char *argv[]) {

    Arq a1;
    Arq a3;
    Arq a2;

	//o main recebe o nome do arquivo de numeros aleatorios a ser aberto em argv[1]
    initiateValues(&a1, &a2, &a3, argv);

    int tam = file_size(a1.fp);

    int i;
    for(i = 0; fib(i) < tam; i++) {}
    i-=2;
    int max = fib(i+1);

    //printf("Arquivo de %d números\ndividindo em %d - %d\n\n", tam, fib(i), max);

    //dividir a1 no maior pedaço de fib
    fseek(a1.fp, max*sizeof(int), SEEK_SET);
    copia(a1.fp, a2.fp, -1);

    rewind(a1.fp); rewind(a2.fp);

    //valores iniciais
    Arq *read1 = &a1, *read2 = &a2, *wr = &a3;
    Arq *k;

    while (1) {

        k = poliphaseSort(read1, read2, wr, max, tam);

        /*  k é o arquivo que deu EOF. então rewind e ele será nosso arquivo 
            para escrever. read2 será rewind */
        if (k == read1) {
            limpaArquivo(read1);
            read1 = read2;
        } else if (k == (void*) -1) break; //intercalações entre arquivos finalizada

        rewind(wr->fp);
        read2 = wr;

        limpaArquivo(k);
        wr = k;
    } 

    if (wr->g != tam) {
        /* copiando primeira parte do wr para read1 */
        rewind(wr->fp);
        limpaArquivo(read1);

        while (wr->g+1 <= tam) {
            rewind(read2->fp);
            ordenaParcial(read1, read2, wr, tam, max);

            k = wr;
            wr = read2;
            read2 = k;
        }
    }

    puts("Vetor ordenado:");
    /* printaTudo(wr); */

    return 0;
}
