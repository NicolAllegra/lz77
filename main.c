//
// Created by nicol on 05/12/2017.
//
#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const int max=8192;//2^13
//const int max=4096;
int main(){
    clock_t begin=clock();
    unsigned short temp=0;
    int bitposition=0;
    unsigned char buf[21];
    for(int i=0;i<21;i++)
        buf[i]=0;
    int indice=0;
    FILE *input=fopen("C:\\Users\\nicol\\Desktop\\fileprove\\alice.txt","rb");
    FILE *output=fopen("C:\\Users\\nicol\\Desktop\\fileprove\\compresso.txt","wb");

    if(input==NULL){
        printf("errore");
    }
    fseek(input,0,SEEK_END);
    size_t sizeFile=(size_t)ftell(input);
    fseek(input,0,SEEK_SET);
    if(sizeFile==0){
        printf("File vuoto");
        exit(0);
    }
    unsigned char *buffer=(unsigned char *)malloc(sizeFile * sizeof(unsigned char));//file che leggo
    fread(buffer, sizeof(unsigned char),sizeFile,input);
    int k=0;
    int lunghezza_ric;
    unsigned char **dizionario=(unsigned char **)malloc(max* sizeof(unsigned char *));//dizionario grnade grande
    int *lunghezza_dizionario=(int *)calloc(max,sizeof(int));//per sapere quanto è lunga la stringa nel dizionario
    int inizio=0;
    int fine;//inizio e fine mi servono per "scorrere" l'array

    while(inizio<sizeFile) {//scorro tutto il file
        if (inizio >= sizeFile) {
            break;
        }
        int trovato = 1;//questa variabile serve per verifare se ho trovato la stringa che mi interessa
        int ultimo_trovato = 0;//mi devo memorizzare il codice dell'ultima corrispondenza trovata
        int cont = 1;
        while (trovato == 1) {//ciclofinchè trovo qualcosa
            trovato = -1;//appena entro nel ciclo setto in un valore diverso da 1,se lo trovo lo devo rimettere a 1
            fine = inizio + cont;//posiziono la mia fine subito dopo l'inizio

            int j = 0;//semplice indice per ciclare
            lunghezza_ric = fine - inizio;//questa indica la lunghezza della mia stringa che voglio confrontare
            while (lunghezza_dizionario[j] > 0) {//finchè scorro la mia lunghezza del dizionario
                if (lunghezza_dizionario[j] ==
                    lunghezza_ric) {//confronto solo se la lunghezza del dizionario è uguale o quello che voglio ricercare
                    if (memcmp(&buffer[inizio], dizionario[j], lunghezza_ric) == 0 &&
                        fine < sizeFile) {//comparo da inizio del buffer per la lunghezza della ricorrenza
                        cont++;//sposto la fine
                        trovato = 1;//ho trovato qualcosa
                        ultimo_trovato = j + 1;//salvo l'inidice dell'ultimo trovato
                        break;
                    } else if (memcmp(&buffer[inizio], dizionario[j], lunghezza_ric) == 0 && fine == sizeFile) {
                        trovato = 2;//ho trovato qualcosa
                        ultimo_trovato = j + 1;//salvo l'inidice dell'ultimo trovato
                        break;
                    }
                }
                j++;
            }

        }
        //fwrite(&ultimo_trovato, sizeof(unsigned short), 1, output);


        //inizio compressione

        if(indice==21){
            fwrite(buf,sizeof(char),indice,output);
          // printf("\n\n\n\n\n\nScrittura buffer\n");
            //for(int i=0;i<21;i++)
              //  printf("Buffer: %d\n",buf[i]);
            for(int i=0;i<21;i++)
                buf[i]=0;
            indice=0;
        }

        temp=0;
        temp=ultimo_trovato<<(3);
        temp= temp >>11;
        //printf("bitposition : %d\n",bitposition);
        temp=temp<<3;
        buf[indice]=buf[indice] | (temp >> (bitposition));  //disegno 4


        bitposition+=5;
        if(bitposition>8) {
            bitposition %= 8;
            //printf(" 1-Buf[indice]: %d\n",buf[indice]);
            indice++;
            buf[indice]=(temp << (8-bitposition-3));    //disegno 5
        }
        if(bitposition==8){
            //printf("bitposition : %d\n",bitposition);
            //printf(" 2-Buf[indice]: %d\n",buf[indice]);
            indice++;
            bitposition%=8;
        }
        temp=ultimo_trovato<<8;
        temp=temp>>8;
        //printf("\ntemp significativo %d\n",temp);//disegno 7
        buf[indice]=buf[indice] | (temp >> (bitposition));
        bitposition+=8;
           //printf(" 3-Buf[indice]: %d\n",buf[indice]);
        indice++;
        if(bitposition>8) {
            bitposition %= 8;
            buf[indice]=(temp << (8-bitposition));    //disegno 5
        }
        bitposition%=8;

        if(trovato==2)
            break;

        temp=buffer[fine-1];
            // printf("%d",temp);
        buf[indice]=buf[indice] | (temp >> (bitposition));
        bitposition+=8;
        //printf(" 4-Buf[indice]: %d\n",buf[indice]);
        indice++;
        if(bitposition>8) {
            bitposition %= 8;
            buf[indice]=(temp << (8-bitposition));    //disegno 5
        }
        bitposition%=8;

        //fine compressione

        dizionario[k] = (unsigned char *) malloc(lunghezza_ric * sizeof(unsigned char));
        lunghezza_dizionario[k] = lunghezza_ric;
        for (int i = 0; i < lunghezza_ric; i++) {
            dizionario[k][i] = buffer[inizio + i];
        }

        if (k == (max - 1)) {
          //printf("t'azzero");
            for (; k >= 0; k--) {
                dizionario[k] = 0;
                lunghezza_dizionario[k] = 0;
            }
        }
        k++;
        inizio = fine;

    }
    if(bitposition!=0)
        fwrite(buf,1,indice+1,output);
    else
        fwrite(buf,1,indice,output);
    //printf("finito!!");
    clock_t end= clock();
    double time=(double) (end-begin)/CLOCKS_PER_SEC;
    printf("%lfs",time);
    printf("\a");
}
