/**************************************************************************/
/*                                                                        */
/* P2 - L'aplicació UEB amb sockets TCP/IP - Part I                       */
/* Fitxer aUEBc.c que implementa la capa d'aplicació de UEB, sobre la     */
/* cap de transport TCP (fent crides a la "nova" interfície de la         */
/* capa TCP o "nova" interfície de sockets TCP), en la part client.       */
/*                                                                        */
/* Autors:                                                                */
/* Data:                                                                  */
/*                                                                        */
/**************************************************************************/

/* Inclusió de llibreries, p.e. #include <sys/types.h> o #include "meu.h" */
/*  (si les funcions externes es cridessin entre elles, faria falta fer   */
/*   un #include del propi fitxer capçalera)                              */

#include "p2-tTCP.h"
#include <stdio.h>
#include <string.h>


/* Definició de constants, p.e.,                                          */

/* #define XYZ       1500                                                 */

/* Declaració de funcions INTERNES que es fan servir en aquest fitxer     */
/* (les  definicions d'aquestes funcions es troben més avall) per així    */
/* fer-les conegudes des d'aquí fins al final d'aquest fitxer, p.e.,      */

/* int FuncioInterna(arg1, arg2...);                                      */

int ConstiEnvMis(int SckCon, const char *tipus, const char *info1, int long1);
int RepiDesconstMis(int SckCon, char *tipus, char *info1, int *long1);

/* Definició de funcions EXTERNES, és a dir, d'aquelles que es cridaran   */
/* des d'altres fitxers, p.e., int UEBc_FuncioExterna(arg1, arg2...) { }  */
/* En termes de capes de l'aplicació, aquest conjunt de funcions externes */
/* formen la interfície de la capa UEB, en la part client.                */

/* Crea un socket TCP "client" en una @IP i #port TCP local qualsevol, a  */
/* través del qual demana una connexió a un S UEB que escolta peticions   */
/* al socket TCP "servidor" d'@IP "IPser" i #portTCP "portTCPser".        */
/* Escriu l'@IP i el #port TCP del socket "client" creat a "IPcli" i      */
/* "portTCPcli", respectivament.                                          */
/* Escriu un text que descriu el resultat de la funció a "TextRes".       */
/*                                                                        */
/* "IPser" i "IPcli" són "strings" de C (vector de chars imprimibles      */
/* acabats en '\0') d'una longitud màxima de 16 chars (incloent '\0').    */
/* "TextRes" és un "string" de C (vector de chars imprimibles acabat en   */
/* '\0') d'una longitud màxima de 200 chars (incloent '\0').              */
/*                                                                        */
/* Retorna:                                                               */
/*  l'identificador del socket TCP connectat si tot va bé;                */
/* -1 si hi ha un error a la interfície de sockets.                       */
int UEBc_DemanaConnexio(const char *IPser, int portTCPser, char *IPcli, int *portTCPcli, char *TextRes)
{
    int socketClient;
	if((socketClient = TCP_CreaSockClient("0.0.0.0",0))==-1){
        strcpy(TextRes,"Error TCP_CreaScokClient");
        return -1;
    }
    if((TCP_DemanaConnexio(socketClient,IPser,portTCPser))==-1){
        strcpy(TextRes,"Error TCP_DemanaConnexio");
        return -1;
    }
    if(TCP_TrobaAdrSockLoc(socketClient,IPcli,portTCPcli)==-1){
        strcpy(TextRes,"Error TCP_TrobaAdrSockLoc");
        return-1;
    }

    strcpy(TextRes,"Success");
    return socketClient;
}

/* Obté el fitxer de nom "NomFitx" del S UEB a través de la connexió TCP  */
/* d'identificador "SckCon". Escriu els bytes del fitxer a "Fitx" i la    */
/* longitud del fitxer en bytes a "LongFitx".                             */
/* Escriu un text que descriu el resultat de la funció a "TextRes".       */
/*                                                                        */
/* "NomFitx" és un "string" de C (vector de chars imprimibles acabat en   */
/* '\0') d'una longitud <= 10000 chars (incloent '\0').                   */
/* "Fitx" és un vector de chars (bytes) qualsevol (recordeu que en C,     */
/* un char és un enter de 8 bits) d'una longitud <= 9999 bytes.           */
/* "TextRes" és un "string" de C (vector de chars imprimibles acabat en   */
/* '\0') d'una longitud màxima de 200 chars (incloent '\0').              */
/*                                                                        */
/* Retorna:                                                               */
/*  0 si el fitxer existeix al servidor;                                  */
/*  1 la petició és ERRònia (p.e., el fitxer no existeix);                */
/* -1 si hi ha un error a la interfície de sockets;                       */
/* -2 si protocol és incorrecte (longitud camps, tipus de peticio, etc.); */
/* -3 si l'altra part tanca la connexió.                                  */
int UEBc_ObteFitxer(int SckCon, const char *NomFitx, char *Fitx, int *LongFitx, char *TextRes)
{   
    char NomFitxer[9999]; //Nom fitx 10000, Nom fitxer 9999
    memset(NomFitxer,0,sizeof(NomFitxer));
    memcpy(NomFitxer,NomFitx,9999);
    int errorEnvia = ConstiEnvMis(SckCon,"OBT",NomFitxer,strlen(NomFitxer)); //Nom fitxer sí que és imprimible.

    if(errorEnvia == -1){strcpy(TextRes,"Error en la interficie de sockets UEBc_ObteFitxer ConsiEnvMis"); return -1;}
    else if(errorEnvia == -2){strcpy(TextRes,"Error en el protocol UEBc_ObteFitxer ConsiEnvMis"); return -2;}

    char tipus[4];
    int errorRep = RepiDesconstMis(SckCon,tipus,Fitx,LongFitx);
    if(errorRep == -1){strcpy(TextRes,"Error en la interficie de sockets UEBc_ObteFitxer RepiDesconstMis"); return -1;}
    else if(errorRep == -2){strcpy(TextRes,"Error en el protocol UEBc_ObteFitxer RepiDesconstMis"); return -2;}
    else if(errorRep == -3){strcpy(TextRes,"Connexió tancada UEBc_ObteFitxer RepiDesconstMis"); return -3;}
    if(strcmp(tipus,"ERR") == 0){strcpy(TextRes,"La petició és errònia en UEBc_ObteFitxer: [ERR]"); return 1;}
    else if(strcmp(tipus,"COR") != 0){strcpy(TextRes,"La petició és errònia en UEBc_ObteFitxer: [???]"); return -2;}

    strcpy(TextRes,"Succes");
    return 0;
}

/* Tanca la connexió TCP d'identificador "SckCon".                        */
/* Escriu un text que descriu el resultat de la funció a "TextRes".       */
/*                                                                        */
/* "TextRes" és un "string" de C (vector de chars imprimibles acabat en   */
/* '\0') d'una longitud màxima de 200 chars (incloent '\0').              */
/*                                                                        */
/* Retorna:                                                               */
/*   0 si tot va bé;                                                      */
/*  -1 si hi ha un error a la interfície de sockets.                      */
int UEBc_TancaConnexio(int SckCon, char *TextRes)
{
	if(TCP_TancaSock(SckCon) == -1){
        strcpy(TextRes,"Error en tancar el socket de UEBc_TancaConnexio");
        return -1;
    }
    strcpy(TextRes,"Success");
    return 0;
}

/* Si ho creieu convenient, feu altres funcions EXTERNES                  */

/* Descripció de la funció, dels arguments, valors de retorn, etc.        */
/* int UEBc_FuncioExterna(arg1, arg2...)
{
	
} */

/* Definició de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer. Les seves declaracions es        */
/* troben a l'inici d'aquest fitxer.                                      */

/* Descripció de la funció, dels arguments, valors de retorn, etc.        */
/* int FuncioInterna(arg1, arg2...)
{
	
} */

/* "Construeix" un missatge de PUEB a partir dels seus camps tipus,       */
/* long1 i info1, escrits, respectivament a "tipus", "long1" i "info1"    */
/* (que té una longitud de "long1" bytes), i l'envia a través del         */
/* socket TCP “connectat” d’identificador “SckCon”.                       */
/*                                                                        */
/* "tipus" és un "string" de C (vector de chars imprimibles acabat en     */
/* '\0') d'una longitud de 4 chars (incloent '\0').                       */
/* "info1" és un vector de chars (bytes) qualsevol (recordeu que en C,    */
/* un char és un enter de 8 bits) d'una longitud <= 9999 bytes.           */
/*                                                                        */
/* Retorna:                                                               */
/*  0 si tot va bé;                                                       */
/* -1 si hi ha un error a la interfície de sockets;                       */
/* -2 si protocol és incorrecte (longitud camps, tipus de peticio).       */
int ConstiEnvMis(int SckCon, const char *tipus, const char *info1, int long1)
{
    if(long1<1 || long1>9999 || strlen(tipus)!=3 || (strcmp(tipus,"COR") !=0 && strcmp(tipus,"ERR")  !=0 && strcmp(tipus,"OBT") !=0))
        return -2;

    int messageSize = long1+7;
    char missatge[messageSize];
    memset(missatge,0,sizeof(missatge));
    memcpy(missatge,tipus,3);

    char longitudStgring[5];
    sprintf(longitudStgring, "%.4d", long1);
    memcpy(missatge+3,longitudStgring,4);

    memcpy(missatge+7,info1,long1);
    if(TCP_Envia(SckCon,missatge,sizeof(missatge))==-1){
        return -1;
    }
    return 0;
}

/* Rep a través del socket TCP “connectat” d’identificador “SckCon” un    */
/* missatge de PUEB i el "desconstrueix", és a dir, obté els seus camps   */
/* tipus, long1 i info1, que escriu, respectivament a "tipus", "long1"    */
/* i "info1" (que té una longitud de "long1" bytes).                      */
/*                                                                        */
/* "tipus" és un "string" de C (vector de chars imprimibles acabat en     */
/* '\0') d'una longitud de 4 chars (incloent '\0').                       */
/* "info1" és un vector de chars (bytes) qualsevol (recordeu que en C,    */
/* un char és un enter de 8 bits) d'una longitud <= 9999 bytes.           */
/*                                                                        */
/* Retorna:                                                               */
/*  0 si tot va bé;                                                       */
/* -1 si hi ha un error a la interfície de sockets;                       */
/* -2 si protocol és incorrecte (longitud camps, tipus de peticio);       */
/* -3 si l'altra part tanca la connexió.                                  */
int RepiDesconstMis(int SckCon, char *tipus, char *info1, int *long1)
{
    int bytes_llegits;
    char missatge[10006];
    memset(missatge,0,sizeof(missatge));
    if((bytes_llegits = TCP_Rep(SckCon,missatge,sizeof(missatge)))==-1){
        return -1;
    }
    if(bytes_llegits == 0){
        return -3;
    }
    if(bytes_llegits>10006 || bytes_llegits<8){
        return -2;
    }
    //missatge[bytes_llegits] = '\0';

    memcpy(tipus,missatge,3);
    tipus[3] = '\0';

    char auxLong[5];
    memcpy(auxLong,missatge+3,4);
    auxLong[4] = '\0';
    *long1 = atoi(auxLong);

    memcpy(info1,missatge+7,bytes_llegits-7);

    if(*long1<0 || *long1>9999 || strlen(tipus)!=3 || (strcmp(tipus,"COR") !=0 && strcmp(tipus,"ERR")  !=0 && strcmp(tipus,"OBT") !=0))
        return -2;

    return 0;
}