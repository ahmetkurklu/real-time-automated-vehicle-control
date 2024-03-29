/***********************************************************************/
/*                                                                     */
/*  FILE        :test_compil.c                                         */
/*  DATE        :Fri, Sep 29, 2006                                     */
/*  DESCRIPTION :main program file.                                    */
/*  CPU GROUP   :87                                                    */
/*                                                                     */
/*  This file is generated by Renesas Project Generator (Ver.4.5).     */
/*   m308															   */
/*   nc308lib														   */
/*   c308mr															   */
/*   nc382lib														   */
/*                                                                     */
/***********************************************************************/
#include "sfr32c87.h"
#include <stdio.h>
#include <stdlib.h>
#include <itron.h>
#include <kernel.h>
#include "kernel_id.h"
#include "lcd.h"
#include "clavier.h"
#include "periph.h"
#include "uart0.h"
#include "can.h"
#include "carte_io.h"
#include "carte_m32.h"
#include <math.h>

// Potentiometre: lire les registres ad00 et ad01, les valeurs sont sur 10 bits.

// Clavier: vrcv_dtq(QdmTouche,&code_touche) pour lire la derniere touche appuyee sur le clavier.
// 			la variable code_touche doit etre du type short.

// Bouton poussoir: Bp_G, Bp_M, Bp_D permettent de lire l'etat des boutons de la carte I/O

// Leds: LED_R=1 ou LED_R=0 Pour allumer ou eteindre les leds (LED_R, LED_J, LED_V).

// Pour communiquer avec le simulateur utiliser une variable de type CanFrame,
// Definir les differents champs en utilisant la structure (S)eparee (comm.data)
// Envoyer le message complet en utilisant l'union (comm.msg)

// Exemple:
// 		CanFrame comm;
//			comm.data.id='T'; comm.data.rtr=0; comm.data.val=-100;
//			snd_dtq (CanTx,comm.msg);

// Pour interroger un peripherique et recuperer les donnees brutes renvoyees simulateur:
//		CanFrame demande;
//		CanFrame reponse;
//
//			demande.data.id='R'; demande.data.rtr=1;
//			snd_dtq (CanTx,demande.msg); // Interrogation du peripherique
// 			rcv_dtq (CanRx,&reponse.msg); // Attente de la reponse
// 			reponse.data.val contient la valeur de retour du simulateur.
// ATTENTION: Ne pas utiliser rcv_dtq(CanRx... si la tache ID_periph_rx est active

// Lors de l'utilisation de la tache de reception et distribution des messages ID_periph_rx
// Demarrer cette tache : sta_tsk(ID_periph_rx);
// Pour lire la valeur d'un peripherique:
// Il faut envoyer une demande de lecture:
// 	CanFrame comm;
// 		comm.data.id='R'; comm.data.rtr=1;
// 		snd_dtq (CanTx,comm.msg);
//
// Des l'arrivee de la reponse du simlateur, les variables suivantes sont mises a jour:
// periph[ADDR('R')].val : contient la derniere valeur renvoyee par le simulateur.
//
// Pour verifier si une nouvelle valeur a ete recue utiliser:
// periph[ADDR('R')].maj (incremente a chaque reception).

// Pour qu'un evenement soit declenche lors de la reception d'une donnee pour un peripherique:
// periph[ADDR('R')].ev=0x01;
// Pour se mettre en attente de l'evenement: wai_flg (ev_periph,0x0001,TWF_ORW,&flag); // Declarer la variable flag comme : FLGPTN flag
// Attention l'evenement n'est pas efface apres reception, il faut donc utiliser clr_flg(ev_periph,~(flag & 0x0001)); par example

// Les evenements:
// Si le simulateur envoi un evenement sur 16 bits il est recu grace a:
// par exemple:  wai_flg(event,(FLGPTN) 0x0007,TWF_ORW,&flag); // Declarer la variable flag comme : FLGPTN flag
// Attention l'evenement n'est pas efface apres reception, il faut donc utiliser clr_flg(event,~(flag & 0x0007)); par example
//Bit   Information associee            Remarque
//0     Capteur Vert,                   remis a zero lors de la lecture du peripherique 'C'
//1     Capteur Jaune,                  remis a zero lors de la lecture du peripherique 'C'
//2     Capteur Rouge,                  remis a zero lors de la lecture du peripherique 'C'
//3     Capteur Bleu,                   remis a zero lors de la lecture du peripherique 'C'
//4     Capteur Cyan,                   remis a zero lors de la lecture du peripherique 'C'
//5
//6     Collision avec le sol,          remise a zero au changement de piste.
//7     Fin de course (capteur vert),   remis a zero lors de la lecture du peripherique 'C'
//8     La piste a change ,             remis a zero lors de la lecture du peripherique 'M'
//9     Le mode de course a change ,    remis a zero lors de la lecture du peripherique 'M'
//10
//11    Le vehicule a termine un tour,   remis a zero au changement du mode de course.
//12    Sortie de la piste,
//13    Teleportation a ete utilisee,   remis a zero au changement de piste ou du mode de course.
//14    Faux depart                     remise a zero au changement du mode de course.
//15


// Peripheriques disponibles:
//'V'/86/0x56�: Commande en vitesse des roues motrices du vehicule (en radian /secondes).
//'D'/68/0x44�: Commande de l'angle des roues directrices (en 1/10 de degre).
//'T'/84/0x54�: Commande en vitesse de la tourelle portant le telemetre (en 1/10 de degre/secondes).
//'R'/82/0x52�: Lecture de l'angle effectif de la tourelle portant le telemetre (en 1/10 de degre).
//'U'/85/0x55�: Distance mesuree par le telemetre (1/100 de metre)
//'N'/78/0x4E�: Numero de la voiture (en fonction de l'ordre de connexion)
//'E'/69/0x45�: Lecture des evenements,
//'H'/72/0x48�: Donne le temps de course actuel
//'S'/83/0x53�: Temps du tour precedent
//'M'/77/0x7D�: Mode de course :
//  Bit 15�: Etat feu tricolore ( 1 -> Vert, 0 -> Orange ou Rouge),
//  Bits 14-8�: 1 Attente, 2 course, 3 essais libres)
//  Bits 7-0�: numero de la piste
//'C'/67/0x43�: Informations sur le dernier capteur touche :
//  8 bits de poids faible�: numero du capteur
//  8 bits de poids fort�: couleur ('C','R','J','B' ou 'V')
//'J'/74/0x4A : Proposition d'un code de d�v�rouillage. 
//				Une valeur de 0 � 5 par quartet
//'j'/106/06A : R�cup�ration du r�sultat de dernier code envoy�. 
//				0x77 si aucun code n'a �t� soumis. 
//				<0 si la r�ponse n'est pas
//				disponible. 0x0a0b avec a-> nombre de couleurs bien plac�es et b -> couleurs pr�sentes mais mal plac�es.
//'I'/73/Ox49 : D�finition du nom du v�hicule. Doit d�buter par le caract�re '#' et entraine le chargement de la configuration de piste
//				correspondant au nom du v�hicule si le nom se termine par '*'




CanFrame comm;
CanFrame requete,reponse;
unsigned short K=2;
unsigned short epsilon;
int capteurs,v,piste,etat_feu;

unsigned short consigneR,consigneT;



short decode_int(char c);
short decode_chaine(char chaine[]);


void main()
{
	ports_mcu();
	simulateur_use();
    lcd_init();
	periph_init();
	periph_nom("#MCQUEEN*");

    can_init();
    clavier_init(1);
	capture_init();
	
	sta_cyc(ID_acqui);
//	sta_cyc(id_regulation);
	sta_tsk(ID_periph_rx);

	dly_tsk(5000);
	
}	

void acqui()
{
	LED_V=!LED_V;
}

/*
void clavierLcd()
{
	short code_touche;
	
	while(1){
		vrcv_dtq(QdmTouche,&code_touche);
		lcd_putc(code_touche);
	}
}*/

/*
void clavierLcd()
{
	short code_touche;
	short vitesse;
	char vit[4];
	int i;
	
	while(1){
		vrcv_dtq(QdmTouche,&code_touche);
		lcd_putc(code_touche);	
		
		if(code_touche=='*')
		{
			i=0;
			while(code_touche!='#')
			{
				vrcv_dtq(QdmTouche,&code_touche);
				vit[i]=code_touche;
				i=i+1;	
			}
			vit[i-1]='\0';
			vitesse = decode_chaine(vit);
			comm.data.id='V';
			comm.data.rtr=0;
			comm.data.val=vitesse;
			snd_dtq(CanTx,comm.msg);	
		}
	}
}
*/

int param, val;
void clavierLcd()
{
	short code_touche;
	char tab[7];
	char vall[5];
	int i;
	
	while(1){
		vrcv_dtq(QdmTouche,&code_touche);
		lcd_putc(code_touche);	
		
		if(code_touche=='#')
		{
			i=0;
			while(code_touche!='*')
			{
				vrcv_dtq(QdmTouche,&code_touche);
				tab[i]=code_touche;
				i=i+1;	
			}
			tab[i-1]='\0';
			param = decode_int(tab[0]);
			
			vall[0]=tab[2];
			vall[1]=tab[3];
			vall[2]=tab[4];
			vall[3]=tab[5];
			vall[4]=tab[6];
			val = decode_chaine(vall);	
		}
		dly_tsk(10);
	}
}


void regulation(VP_INT stacd)
{
	
//	unsigned short consigneT;
	unsigned short alpha=0;	
	while(1){
		
		if(Bp_M==1){// consigne avec Boutton du millieu 
			consigneT=(ad01-512)*2;//Calcule de la consigne
		}
	/*	else
		{
			consigneT=450;
			//consigneT=600;
		}*/
		requete.data.id='R';
		requete.data.rtr =1;
		periph[ADDR('R')].maj =0;
		snd_dtq(CanTx,requete.msg);
		while(periph[ADDR('R')].maj==0)
		{
			dly_tsk(10);	
		}
		alpha= periph[ADDR('R')].val;
	
		comm.data.id='T';
		comm.data.rtr=0;
		comm.data.val=(consigneT-alpha)*K;
		snd_dtq(CanTx,comm.msg);
			
	}
}



unsigned short alphaR=0;

void regulation_roue(VP_INT stacd)
{
//	unsigned short consigneR;
	
	
	while(1){		
		
		if(Bp_M==1){// consigne avec Boutton du millieu 
			consigneR=(ad00-512)*2;
		}
	/*	else
		{
			consigneR=700; 
			//consigneR=565;
		}*/
		requete.data.id='U';
		requete.data.rtr =1;
		periph[ADDR('U')].maj = 0;
		snd_dtq(CanTx,requete.msg);
		while(periph[ADDR('U')].maj==0)
		{
			dly_tsk(10);
		}
		alphaR=periph[ADDR('U')].val;
	
	
		if(alphaR<1000){
			comm.data.id='D';
			comm.data.rtr=0;
			comm.data.val=-(consigneR-alphaR)*K;
			snd_dtq(CanTx,comm.msg);
		}
		
	}
}

//int vitV,vitJ,vitR,vitB,vitC,vitDep;
void strategie(VP_INT stacd)//Differente valeurs des vitesse + consigne de roue et tourelle pour chaque capteur de chaque piste 
{	
	while(1){
		dly_tsk(100);
		switch(piste)
		{
			case 1:
				consigneR=700;
				consigneT=450;
				lcd_str("PISTE VERTE");
				switch(capteurs)
				{
					case 0x5600 ://Vert
						//v = vitV;
						v = 65;
						break;
					case 0x4A00 ://Jaune
						//v = vitJ;
						v = 30;
						break;
					case 0x5200 ://Rouge
						//v = vitR;
						v = 30;
						break;
					case 0x4200 ://Bleu
						//v = vitB;
						v = 30;
						break;
					case 0x4300 ://Cyan
						//v = vitC;
						v = 30;
						break;
					default:
						v = 15;
				}
				break;
			case 2:
				consigneR=700;
				consigneT=450;
				lcd_str("PISTE BLEU");
				switch(capteurs)
				{
					case 0x5600 ://Vert
						//v = vitV;
						v = 50;
						break;
					case 0x4A00 ://Jaune
						//v = vitJ;
						v = 25;
						break;
					case 0x5200 ://Rouge
						//v = vitR;
						v = 25;
						break;
					case 0x4200 ://Bleu
						//v = vitB;
						v = 25;
						break;
					case 0x4300 ://Cyan
						//v = vitC;
						v = 25;
						break;
					default:
						v = 15;
				}
				break;
			case 3:
				consigneR=565;
				consigneT=600;
				lcd_str("PISTE ROUGE ");
				switch(capteurs)
				{
					case 0x5600 ://Vert
						//v = vitV;
						v = 45;
						break;
					case 0x4A00 ://Jaune
						//v = vitJ;
						v = 20;
						break;
					case 0x5200 ://Rouge
						//v = vitR;
						v = 20;
						break;
					case 0x4200 ://Bleu
						//v = vitB;
						v = 15;
						break;
					case 0x4300 ://Cyan
						//v = vitC;
						v = 20;
						break;
					default:
						v = 18;
				}
				break;
		}
		lcd_cls();
	}
	
}


void definirParametre(VP_INT stacd){
	/*while(1)
	{
		dly_tsk(100);
		switch(param)
		{
			case 0:
				vitDep = val;
				break;
			case 1:
				vitV = val;
				break;
			case 2:
				vitJ = val;
				break;
			case 3:
				vitR = val;
				break;
			case 4:
				vitB = val;
				break;
			case 5:
				vitC = val;
				break;
			
		}	
		
	}*/
}


void ModeDeCourse(VP_INT stacd)
{
	CanFrame req;
	int m;
	UINT flag2;
	
	while(1){
		req.data.id ='M';
		req.data.rtr = 1;
		periph[ADDR('M')].ev = 0x02;
		snd_dtq(CanTx,req.msg);
		wai_flg(ev_periph,0x02,TWF_ANDW,&flag2);
		m=periph[ADDR('M')].val;
		clr_flg(ev_periph,~(flag2 & 0x02));
		
		etat_feu = m;
		piste = m & 0x00FF;
		dly_tsk(20);
	}	
}

void attente_feu(VP_INT stacd)
{
		while(1){				
			if(((etat_feu>>15 & 1) == 0))// Arret d'urgence
			{
				comm.data.id='V';
				comm.data.rtr=0;
				comm.data.val=0;
				snd_dtq(CanTx,comm.msg);
			}		
			else if(Bp_G == 1)
			{
				comm.data.id='V';
				comm.data.rtr=0;
				comm.data.val=0;
				snd_dtq(CanTx,comm.msg);
				LED_R = 1;
					
			}
			else if((etat_feu>>15 & 1) == 1)	
			{
				comm.data.id='V';
				comm.data.rtr=0;
				comm.data.val=v;
				snd_dtq(CanTx,comm.msg);
				LED_R = 0;
			}
			dly_tsk(20);
	}
}



void capteur(VP_INT stacd)
{
	CanFrame req;
	int c;
	UINT flag1;
	
	while(1){
		req.data.id ='C';
		req.data.rtr = 1;
		periph[ADDR('C')].ev = 0x01;
		snd_dtq(CanTx,req.msg);
		wai_flg(ev_periph,0x01,TWF_ANDW,&flag1);
		c=periph[ADDR('C')].val;
		clr_flg(ev_periph,~(flag1 & 0x01));
		
		capteurs = c & 0xFF00;
		dly_tsk(20);
	}	
}



short decode_int(char c){
        short chiffre;
        if (c <= 57 && c >= 48){
        	chiffre = c - 48;
        }
        else{
               chiffre = -1;
        }
        return chiffre;
}

short decode_chaine(char chaine[]){
        short i = 0, nb = 0;
        while (chaine[i] != '\0'){
			nb = 10 * nb;
            nb = decode_int(chaine[i]) + nb;
            i++;
        }
        return nb;
}