/**************************************************************************/
/*                                                                        */
/* P2 - L'aplicació UEB amb sockets TCP/IP - Part I                       */
/* Fitxer cliUEB.c que implementa la interfície aplicació-usuari          */
/* d'un client de l'aplicació UEB, sobre la capa d'aplicació de           */
/* (la part client de) UEB (fent crides a la interfície de la part        */
/* client de la capa UEB).                                                */
/*                                                                        */
/* Autors:                                                                */
/* Data:                                                                  */
/*                                                                        */
/**************************************************************************/

/* Inclusió de llibreries, p.e. #include <stdio.h> o #include "meu.h"     */

#include "p2-aUEBc.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

/* Definició de constants, p.e.,                                          */

/* #define XYZ       1500                                                 */

/* Declaració de funcions INTERNES que es fan servir en aquest fitxer     */
/* (les  definicions d'aquestes funcions es troben més avall) per així    */
/* fer-les conegudes des d'aquí fins al final d'aquest fitxer, p.e.,      */
void demanarFitxer(char *fitxer);
void demanarPeticio(char *tipusPeticio);


/* int FuncioInterna(arg1, arg2...);                                      */

int main(int argc,char *argv[])
{
    printf("p2-cliUEB.c: client ha començat\n");
 /* Declaració de variables, p.e., int n;                                 */
    char IPser[16];
    char IPcli[16];
    int portTCPser = 3000;
    int portTCPcli;
    char errorClient[200];
    int socketConnexio;
    char tipusPeticio[200];
    //char fitxer[200]; //com de llarg pot ser el nom del fitxer a entrar?
    char NomFitx[10000];
    char Fitx[10000];
    int LongFitx;
    int error;
    int portTCPtipic = 8000;
 /* Expressions, estructures de control, crides a funcions, etc.          */
    int continuar = 1;
    while(continuar){
        demanarPeticio(tipusPeticio);
        if(strcmp(tipusPeticio,"obtenir")==0){
            //demanar ip
            printf("Entra la ip del servidor: ");
            fflush(stdout);
            scanf("%s", IPser);
            getchar();//per treure el '\n' que ha quedat.
            printf("Ip entrada: [%s]\n",IPser);

            //demanar port ser
            printf("Entra el port del servidor: ");
            fflush(stdout);
            if(scanf(" %d", &portTCPser) != 1){printf("Error en llegir el portTCPser");}
            if(portTCPser==0){portTCPser=portTCPtipic;}
            getchar(); //treu el '\n' que ha quedat;
            printf("Port: [%d]\n",portTCPser);

            memset(NomFitx, 0,sizeof(NomFitx));
            memset(Fitx,0,sizeof(NomFitx));
            demanarFitxer(NomFitx);
        
            if((socketConnexio = UEBc_DemanaConnexio(IPser,portTCPser,IPcli,&portTCPcli,errorClient))==-1){
                printf("p2-cliUEB.c: Error en UEBc_DemanaConnexio: [%s]\n", errorClient);
            }
            else{
                printf("IPser: [%s], portSer:[%d], IPcli: [%s], portCli: [%d]\n", IPser, portTCPser, IPcli, portTCPcli);
                printf("SocketCon: [%d]\n", socketConnexio);


                printf("%s%s@%d@%s:#%d\n",tipusPeticio,NomFitx,socketConnexio,IPser,portTCPser);

                struct timeval tval_before, tval_after, tval_result;

                gettimeofday(&tval_before, NULL);
                if((error = UEBc_ObteFitxer(socketConnexio,NomFitx,Fitx,&LongFitx,errorClient)) != 0){
                    if(error != 1)
                        printf("Error [%s]\n", errorClient);
                    if(error == -3){printf("El servidor ha tancat la connexió.\n");}
                    if(error == -2){printf("Protocol incorrecte.\n");}
                    if(error == -1){printf("Error en la interficie de sockets.\n");}
                    if(error == 1){
                        write(1,Fitx,LongFitx);
                        printf("\n");
                    }
                }
                gettimeofday(&tval_after, NULL);
                timersub(&tval_after, &tval_before, &tval_result);

                printf("Temps de resposta: [%ld.%06ld] segons\n",(long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
                
                if(error == 0){
                    char NomFitxerTest[10007];
                    memset(NomFitxerTest,0,sizeof(NomFitxerTest));
                    memcpy(NomFitxerTest,".",1);
                    memcpy(NomFitxerTest+1,NomFitx,sizeof(NomFitx));

                    FILE *fp = fopen(NomFitxerTest, "w");
                    if(fp != NULL){
                        printf("\n");
                        write(1,Fitx,LongFitx);
                        printf("\n");
                        fwrite(Fitx,1,LongFitx,fp);
                        fclose(fp);
                    }
                    else{
                        printf("El fitxer [%s] no s'ha pogut obrir.\n",NomFitxerTest);
                    }
                }
                
                memset(Fitx,0,sizeof(Fitx));
                memset(NomFitx,0,sizeof(NomFitx));
                
                sleep(5);
                if(UEBc_TancaConnexio(socketConnexio,errorClient)==-1){printf("Error en tancar la connexió\n");}
                else{printf("Connexió tancada!\n");}
            }
        }
        int opcioIncorrecte = 1;
        while(opcioIncorrecte){
            char buffer[2000];
            char opcio[2];
            printf("Vols cintinuar i anar a la situació inicial [y] o acabar [n]\n");
            fgets(buffer,sizeof(buffer),stdin);
            buffer[strcspn(buffer, "\n")] = 0;
            opcio[0] = buffer[0];
            opcio[1] = '\0';
            if(strcmp(opcio,"n")==0){opcioIncorrecte = 0; continuar = 0;}
            else if(strcmp(opcio,"y")==0){opcioIncorrecte = 0; continuar = 1;}
        }
    }
    printf("p2-cliUEB.c: client ha acabat\n");
}

/* Definició de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer. Les seves declaracions es troben */
/* a l'inici d'aquest fitxer.                                             */

/* Descripció de la funció, dels arguments, valors de retorn, etc.        */
/*int FuncioInterna(arg1, arg2...)
{
	
} */

void demanarPeticio(char *tipusPeticio){
    printf("Entra el tipus de petició: ");
    fflush(stdout);
    memset(tipusPeticio,0,sizeof(tipusPeticio));
    scanf("%s", tipusPeticio);
    getchar();//per treure el '\n' que ha quedat.
}

void demanarFitxer(char *fitxer){
    printf("Entra el nom del fitxer: ");
    fflush(stdout);
    memset(fitxer,0,sizeof(fitxer));
    fgets(fitxer,10000,stdin);
    fitxer[strcspn(fitxer,"\n")] = '\0';
}
