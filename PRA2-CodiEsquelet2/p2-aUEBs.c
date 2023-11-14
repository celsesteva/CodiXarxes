/**************************************************************************/
/*                                                                        */
/* P2 - L'aplicació UEB amb sockets TCP/IP - Part I                       */
/* Fitxer aUEB.c que implementa la capa d'aplicació de UEB, sobre la      */
/* cap de transport TCP (fent crides a la "nova" interfície de la         */
/* capa TCP o "nova" interfície de sockets TCP), en la part servidora.    */
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
#include <time.h>
#include <sys/time.h>

/* Definició de constants, p.e.,                                          */

/* #define XYZ       1500                                                 */

/* Declaració de funcions INTERNES que es fan servir en aquest fitxer     */
/* (les  definicions d'aquestes funcions es troben més avall) per així    */
/* fer-les conegudes des d'aquí fins al final d'aquest fitxer, p.e.,      */

/* int FuncioInterna(arg1, arg2...);                                      */

int ConstiEnvMis(int SckCon, const char *tipus, const char *info1, int long1);
int RepiDesconstMis(int SckCon, char *tipus, char *info1, int *long1);

/* Definició de funcions EXTERNES, és a dir, d'aquelles que es cridaran   */
/* des d'altres fitxers, p.e., int UEBs_FuncioExterna(arg1, arg2...) { }  */
/* En termes de capes de l'aplicació, aquest conjunt de funcions externes */
/* formen la interfície de la capa UEB, en la part servidora.             */

/* Inicia el S UEB: crea un nou socket TCP "servidor" a una @IP local     */
/* qualsevol i al #port TCP “portTCPser”. Escriu l'identificador del      */
/* socket creat a "SckEsc".                                               */
/* Escriu un text que descriu el resultat de la funció a "TextRes".       */
/*                                                                        */
/* "TextRes" és un "string" de C (vector de chars imprimibles acabat en   */
/* '\0') d'una longitud màxima de 200 chars (incloent '\0').              */
/*                                                                        */
/* Retorna:                                                               */
/*  0 si tot va bé;                                                       */
/* -1 si hi ha un error en la interfície de sockets.                      */
int UEBs_IniciaServ(int *SckEsc, int portTCPser, char *TextRes)
{
	if((*SckEsc = TCP_CreaSockServidor("0.0.0.0",portTCPser)) == -1){
		strcpy(TextRes,"Error en crear el socket del servidor TCP_CreaSocketServidor de UEBs_IniciaServ");
		return -1;
	}
    strcpy(TextRes,"Success");
	return 0;
}

/* Accepta una connexió d'un C UEB que arriba a través del socket TCP     */
/* "servidor" d'identificador "SckEsc". Escriu l'@IP i el #port TCP del   */
/* socket TCP "client" a "IPcli" i "portTCPcli", respectivament, i l'@IP  */
/* i el #port TCP del socket TCP "servidor" a "IPser" i "portTCPser",     */
/* respectivament.                                                        */
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
int UEBs_AcceptaConnexio(int SckEsc, char *IPser, int *portTCPser, char *IPcli, int *portTCPcli, char *TextRes)
{
	int SocketConnexio;
	if((SocketConnexio = TCP_AcceptaConnexio(SckEsc, IPcli, portTCPcli)) == -1){
		strcpy(TextRes,"Error en TCP_AcceptaConnexio de UEBs_Acceptaconnexio");
		return -1;
	}

	if(TCP_TrobaAdrSockRem(SocketConnexio, IPcli, portTCPcli)==-1){
		strcpy(TextRes,"Error en TCP_TrobaAdrSockRem de UEBs_Acceptaconnexio");
		return -1;
	}

    if(TCP_TrobaAdrSockLoc(SocketConnexio,IPser,portTCPser)== -1){
        strcpy(TextRes,"Error en TCP_TrobaAdrSockLoc de UEBs_Acceptaconnexio");
		return -1;
    }
	strcpy(TextRes,"succes UEBs_AcceptaConnexio");
	return SocketConnexio;
}

/* Serveix una petició UEB d'un C a través de la connexió TCP             */
/* d'identificador "SckCon". A "TipusPeticio" hi escriu el tipus de       */
/* petició (p.e., OBT) i a "NomFitx" el nom del fitxer de la petició.     */
/* Escriu un text que descriu el resultat de la funció a "TextRes".       */
/*                                                                        */
/* "TipusPeticio" és un "string" de C (vector de chars imprimibles acabat */
/* en '\0') d'una longitud de 4 chars (incloent '\0').                    */
/* "NomFitx" és un "string" de C (vector de chars imprimibles acabat en   */
/* '\0') d'una longitud <= 10000 chars (incloent '\0').                   */
/* "TextRes" és un "string" de C (vector de chars imprimibles acabat en   */
/* '\0') d'una longitud màxima de 200 chars (incloent '\0').              */
/*                                                                        */
/* Retorna:                                                               */
/*  0 si el fitxer existeix al servidor;                                  */
/*  1 la petició és ERRònia (p.e., el fitxer no existeix);                */
/* -1 si hi ha un error a la interfície de sockets;                       */
/* -2 si protocol és incorrecte (longitud camps, tipus de peticio, etc.); */
/* -3 si l'altra part tanca la connexió;                                  */
/* -4 si hi ha problemes amb el fitxer de la petició (p.e., nomfitxer no  */
/*  comença per /, fitxer no es pot llegir, fitxer massa gran, etc.).     */
int UEBs_ServeixPeticio(int SckCon, char *TipusPeticio, char *NomFitx, char *TextRes, const char * arrel, int *bytes_missatge_enviats, double *seconds)
{
    struct timeval tval_before, tval_after, tval_result;
    int bytes_missatge = 0;

    int errorReturn = 0;
    char message[9999]; //per als missatges d'error de tornada.
    memset(message,0,sizeof(message));

	int errorRep;
    char NomFitxer[9999];
    memset(NomFitxer,0,sizeof(NomFitxer));
    memcpy(NomFitxer,NomFitx,9999);
    errorRep = RepiDesconstMis(SckCon,TipusPeticio,NomFitxer,&bytes_missatge);

    if(errorRep == -1){strcpy(TextRes,"Error en la interficie de sockets UEBs_ServeixPeticio.");}
    else if(errorRep == -2){strcpy(TextRes,"Error en el protocol UEBs_ServeixPeticio.");}
    else if(errorRep == -3){strcpy(TextRes,"S'ha tancat la connexió UEBs_ServeixPeticio.");}

    if(errorRep == 0){ //TODO si tot va bé.
        memcpy(NomFitx,NomFitxer,9999);
        NomFitx[bytes_missatge] = '\0';
        bytes_missatge++;
        if(strcmp(TipusPeticio,"OBT")!=0 && strcmp(TipusPeticio,"COR")!=0 && strcmp(TipusPeticio,"ERR")!=0){
            strcpy(message,"Error la petició és ERRònia: no és OBT, COR ni ERR.");
            strcpy(TextRes,"Error en el protocol UEBs_ServeixPeticio.");
            errorReturn = -2;
        }
        if(strcmp(TipusPeticio,"COR")==0 || strcmp(TipusPeticio,"ERR")==0){//no pot ser un d'aquets
            strcpy(message,"Error la petició és ERRònia: és COR o ERR, ha de ser OBT.");
            strcpy(TextRes,"Error en el protocol UEBs_ServeixPeticio.");
            errorReturn = 1;
        }
        //comprovar si hi ha problemes amb el fitxer de la petició:
        if(errorReturn == 0){//comprova si comença per /
            if(NomFitx[0] != '/'){
                strcpy(message,"Error la petició és ERRònia: El nom del fitxer no comença per /.");
                strcpy(TextRes,"Error la petició és ERRònia: El nom del fitxer no comença per /.");
                errorReturn = -4;
            }
        }
        
        //fer més comprovacions????????????????
        //comprova si el fitxer existeix:
        int midaNova = 10000+strlen(arrel);
        char nomFitxerNou[midaNova+1];
        memcpy(nomFitxerNou,arrel,strlen(arrel));
        nomFitxerNou[strlen(arrel)] = '\0';
        memcpy(nomFitxerNou+strlen(arrel),NomFitx,10000);
        FILE *fp = fopen(nomFitxerNou, "r");
        if(errorReturn == 0){ //todo retorna error fitxer no trobat.
            if(fp == NULL){
                strcpy(message,"Error la petició és ERRònia: No s'ha trobat el fitxer.");
                strcpy(TextRes,"Error la petició és ERRònia: No s'ha trobat el fitxer.");
                errorReturn = -4;
            }
        }
        int size;
        if(errorReturn == 0){ //comprova que la mida no sigui massa gran
            fseek(fp, 0L, SEEK_END);
            size = ftell(fp);
            if(size>9999){
                strcpy(message,"Error la petició és ERRònia: La mida ha de ser <=9999.");
                strcpy(TextRes,"Error la petició és ERRònia: La mida ha de ser <=9999.");
                errorReturn = -4;
            }
            else if(size<1){
                strcpy(message,"Error la petició és ERRònia: La mida ha de ser >=1.");
                strcpy(TextRes,"Error la petició és ERRònia: La mida ha de ser >=1.");
                errorReturn = -4;
            }
            rewind(fp); // igual a fseek(fp, 0L, SEEK_SET);
        }
        if(errorReturn == 0){
            //enviar missatge de tornada:
            char missatge[9999];
            memset(missatge,0,sizeof(missatge));
            fread(missatge, 1, size, fp);
            *bytes_missatge_enviats = size;

            gettimeofday(&tval_before, NULL);
            int errorEnv = ConstiEnvMis(SckCon,"COR",missatge,size);
            gettimeofday(&tval_after, NULL);
            timersub(&tval_after, &tval_before, &tval_result);
            *seconds = (long int)tval_result.tv_sec;
            *seconds += ((long int)tval_result.tv_usec)/1000000.0;

            if(errorEnv == -1){strcpy(TextRes,"Error en la interficie de sockets UEBs_ObteFitxer ConsiEnvMis"); errorReturn = -1;}
            else if(errorEnv == -2){strcpy(TextRes,"Error en el protocol UEBs_ObteFitxer ConsiEnvMis"); errorReturn = -2;}
            fclose(fp);
        }
        if(errorReturn != 0){
            //enviar missatge de tornada error:
            *bytes_missatge_enviats = strlen(message);
            gettimeofday(&tval_before, NULL);
            int errorEnv = ConstiEnvMis(SckCon,"ERR",message,strlen(message)+1);//message és d'error, per tant un string imprimible, +1 per asuma el \0
            gettimeofday(&tval_after, NULL);
            timersub(&tval_after, &tval_before, &tval_result);
            *seconds = (long int)tval_result.tv_sec;
            *seconds += ((long int)tval_result.tv_usec)/1000000.0;

            if(errorEnv == -1){strcpy(TextRes,"Error en la interficie de sockets UEBs_ObteFitxer ConsiEnvMis"); errorReturn = -1;}
            else if(errorEnv == -2){strcpy(TextRes,"Error en el protocol UEBs_ObteFitxer ConsiEnvMis"); errorReturn = -2;}
        }
    }
    ///////////////////////////////////////////////
    else if(errorRep == -1){ //TODO si hi ha un error en la interficie de sockets.
        errorReturn = -1;
        char missatge[9999];
        memset(missatge,0,sizeof(missatge));
        strcpy(missatge,"Error en la interficie de sockets");
        *bytes_missatge_enviats = strlen(message);
        gettimeofday(&tval_before, NULL);
        int errorEnv = ConstiEnvMis(SckCon,"ERR",missatge,strlen(missatge)+1);
        gettimeofday(&tval_after, NULL);
        timersub(&tval_after, &tval_before, &tval_result);
        *seconds = (long int)tval_result.tv_sec;
        *seconds += ((long int)tval_result.tv_usec)/1000000.0;

        if(errorEnv == -1){strcpy(TextRes,"Error en la interficie de sockets UEBs_ServeixPeticio, Error en la interficie de sockets ConsiEnvMis");}
        else if(errorEnv == -2){strcpy(TextRes,"Error en el protocol UEBs_ServeixPeticio, Error en el protocol ConsiEnvMis");}
    }
    else if(errorRep == -2){ //TODO si el protocol es incorrecte.
        errorReturn = -2;
        char missatge[9999];
        memset(missatge,0,sizeof(missatge));
        strcpy(missatge,"Error el protocol és incorrecte");
        *bytes_missatge_enviats = strlen(message);
        gettimeofday(&tval_before, NULL);
        int errorEnv = ConstiEnvMis(SckCon,"ERR",missatge,strlen(missatge)+1);
        gettimeofday(&tval_after, NULL);
        timersub(&tval_after, &tval_before, &tval_result);
        *seconds = (long int)tval_result.tv_sec;
        *seconds += ((long int)tval_result.tv_usec)/1000000.0;

        if(errorEnv == -1){strcpy(TextRes,"Error en la interficie de sockets UEBs_ServeixPeticio, Error en la interficie de sockets UEBs_ObteFitxer ConsiEnvMis");}
        else if(errorEnv == -2){strcpy(TextRes,"Error en el protocol UEBs_ServeixPeticio, Error en el protocol ConsiEnvMis");}
    }
    else if(errorRep == -3){ //Connexió tancada.
        errorReturn = -3;
    }
    return errorReturn;
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
int UEBs_TancaConnexio(int SckCon, char *TextRes)
{
	if(TCP_TancaSock(SckCon) == -1){
        strcpy(TextRes,"Error en tancar el socket de UEBs_TancaConnexio");
        return -1;
    }
    strcpy(TextRes,"Success");
    return 0;
}

/* Si ho creieu convenient, feu altres funcions EXTERNES                  */

/* Descripció de la funció, dels arguments, valors de retorn, etc.        */
/* int UEBs_FuncioExterna(arg1, arg2...)
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
/*  0 si tot va bé,                                                       */
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
    else if(bytes_llegits>10006 || bytes_llegits<8){
        return -2;
    }

    memcpy(tipus,missatge,3);
    tipus[3] = '\0';

    char auxLong[5];
    memcpy(auxLong,missatge+3,4);
    auxLong[4] = '\0';
    *long1 = atoi(auxLong);
    memcpy(info1,missatge+7,bytes_llegits-7);

    if(*long1<1 || *long1>9999 || strlen(tipus)!=3 || bytes_llegits-7!=*long1 || (strcmp(tipus,"COR") !=0 && strcmp(tipus,"ERR") !=0 && strcmp(tipus,"OBT") !=0))
        return -2;

    return 0;
}
