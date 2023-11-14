/**************************************************************************/
/*                                                                        */
/* P2 - L'aplicació UEB amb sockets TCP/IP - Part I                       */
/* Fitxer serUEB.c que implementa la interfície aplicació-administrador   */
/* d'un servidor de l'aplicació UEB, sobre la capa d'aplicació de         */
/* (la part servidora de) UEB (fent crides a la interfície de la part     */
/* servidora de la capa UEB).                                             */

/*                                                                        */
/* Autors:                                                                */
/* Data:                                                                  */
/*                                                                        */
/**************************************************************************/

/* Inclusió de llibreries, p.e. #include <stdio.h> o #include "meu.h"     */

#include "p2-aUEBs.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

/* Definició de constants, p.e.,                                          */

/* #define XYZ       1500                                                 */

/* Declaració de funcions INTERNES que es fan servir en aquest fitxer     */
/* (les  definicions d'aquestes funcions es troben més avall) per així    */
/* fer-les conegudes des d'aquí fins al final d'aquest fitxer, p.e.,      */
void iniciarServidor(int *SckEsc, int portSer, char *TextRes);
int AcceptarConnexio(int SckEsc, char *IPserFunc, int *portSer, char *IPcliFunc, int *portCli, char *TextRes, int *Connexio);
void TancaSocket(int SckCon, char *TextRes);
int ServirPeticio(int SckCon, char *TipusPeticio, char *NomFitxer, char *TextRes, int *Connexio, const char *IPser, const int portTCPser, const char *arrel);
void obrirFitxer(const char *nomFitxer, int *port, char *arrel);

/* int FuncioInterna(arg1, arg2...);                                      */

int main(int argc,char *argv[])
{
 /* Declaració de variables, p.e., int n;                                 */
    char nom_fitxer[200] = "p2-serUEB.cfg";
    FILE *fp;
    int SocketServidor, SocketConnexio;
    int portTCPser = 3000;
    char IPcli[16], IPser[16];
    int portTCPcli;
    char errorsServidor[200];
    int hihaConnexio = 0;
    char arrel[2000];
    
    
    
 /* Expressions, estructures de control, crides a funcions, etc.          */

    //Llegir port del p2-serUEB.cfg
    obrirFitxer(nom_fitxer,&portTCPser, arrel);
    //Llegit port del p2-serUEB.cfg


    //TODO FER QUE TREGUI ERROR I PARI EL PROGRAMA QUAN HI HA ERROR, NO QUE CONTINUI.
    iniciarServidor(&SocketServidor, portTCPser, errorsServidor);
    printf("Servidor iniciat.\nSituació inicial:\n");
    printf("IP servidor: [0.0.0.0], port TCP servidor: [%d]\n\n", portTCPser);
    SocketConnexio = AcceptarConnexio(SocketServidor,IPser,&portTCPser,IPcli,&portTCPcli,errorsServidor, &hihaConnexio);
    
    printf("Connexió acceptada: \n");
    printf("IP servidor: [%s], port TCP servidor: [%d]\n",IPser, portTCPser);
    printf("IP client: [%s], port TCP client: [%d]\n\n", IPcli, portTCPcli);
    //printf("SocketSer: [%d], SocketCli: [%d]\n", SocketServidor, SocketConnexio);

    while(1){
        while(hihaConnexio){
            char NomFitx[10000];
            char tipus[4];

            memset(NomFitx,0,sizeof(NomFitx));
            memset(tipus,0,sizeof(tipus));

            ServirPeticio(SocketConnexio, tipus, NomFitx,errorsServidor, &hihaConnexio, IPser, portTCPser, arrel);
        }
        TancaSocket(SocketConnexio,errorsServidor);
        SocketConnexio = AcceptarConnexio(SocketServidor,IPser,&portTCPser,IPcli,&portTCPcli,errorsServidor, &hihaConnexio);
    }
    TancaSocket(SocketServidor,errorsServidor);

    printf("Final del programa\n");
}

/* Definició de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer. Les seves declaracions es troben */
/* a l'inici d'aquest fitxer.                                             */

/* Descripció de la funció, dels arguments, valors de retorn, etc.        */
/*int FuncioInterna(arg1, arg2...)
{
	
} */

void iniciarServidor(int *SckEsc, int portSer, char *TextRes){
    if(UEBs_IniciaServ(SckEsc, portSer, TextRes) == -1){
        printf("Error en UEBs_IniciaServ: %s\n",TextRes);
    }
}

int AcceptarConnexio(int SckEsc, char *IPserFunc, int *portSer, char *IPcliFunc, int *portCli, char *TextRes, int *Connexio){
    printf("Esperant connexió...\n");
    int SocketConnexio;
    if((SocketConnexio = UEBs_AcceptaConnexio(SckEsc,IPserFunc,portSer,IPcliFunc,portCli,TextRes))==-1){
        printf("Error en UEBs_AcceptaConnexio: %s\n", TextRes);
    }
    else{
        *Connexio = 1;
    }
    return SocketConnexio;
}

void TancaSocket(int SckCon, char *TextRes){
    if(SckCon != 0){
        if(UEBs_TancaConnexio(SckCon,TextRes) == -1){
            printf("Error %s\n", TextRes);
        }
    }
}

int ServirPeticio(int SckCon, char *TipusPeticio, char *NomFitxer, char *TextRes, int *Connexio, const char *IPser, const int portTCPser, const char * arrel){
    
    int bytes_missatge = 0;
    printf("Servint connexió...\n");
    double segons = 0;
    int errorPeticio = UEBs_ServeixPeticio(SckCon, TipusPeticio, NomFitxer,TextRes,arrel, &bytes_missatge, &segons);
    if( errorPeticio != 0 && errorPeticio !=-3){
        printf("%s%s@%d@%s:#%d\n",TipusPeticio,NomFitxer,SckCon,IPser,portTCPser);
        printf("Error %d: %s\n", errorPeticio, TextRes);
    }
    if(errorPeticio==-3){
        *Connexio = 0;
        printf("El client ha tancat la connexió\n");
    }
    else if(errorPeticio == 0){
        printf("%s%s@%d@%s:#%d\n",TipusPeticio,NomFitxer,SckCon,IPser,portTCPser);
        //Hi ha connexió i arriba un missatge amb dades.
        printf("Bytes_enviats: [%d]\n",bytes_missatge);
        double ZERO = 0;
        if(segons == ZERO){
            printf("Duració d'enviament: [%f] segons, Velocitat efectiva: [INF]\n",segons);
        }
        else
            printf("Duració d'enviament: [%f] segons, Velocitat efectiva: [%f]\n",segons,bytes_missatge/segons);
    }
}

void obrirFitxer(const char *nomFitxer, int *port, char *arrel){
    //#portTCP 8000
    FILE *fp = fopen(nomFitxer,"r");
    if(fp!=NULL){
        char buffer[10000];
        char stringconfig[1]; //variable temporal només per poder fer fscanf
        fscanf(fp,"%s %d",stringconfig,port);
        fscanf(fp,"%s %s",stringconfig,arrel);
    }
    else{
        printf("Fitxer configuració no trobat\n");
    }
}