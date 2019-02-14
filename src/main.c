/******************************************************************/
/******************************************************************/
/***							      	***/
/***        MinMax / Jeu d'échecs / ESI 2018 / TPGO / TP3   	***/
/***	version PC contre PC pour tester les fct d'estimation	***/
/***							      	***/
/******************************************************************/
/******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>  	// pour INT_MAX

// StockFishC includes and definitions
#include <math.h>
#include <ctype.h>
#include "global.h"
#include "eval.h"

#define TRUE 1
#define FALSE 0
// end StockFishC includes and definitions

#define MAX +1		// Joueur Maximisant
#define MIN -1  	// Joueur Minimisant

#define INFINI INT_MAX
#define MAXPARTIE 50	// Taille max du tableau Partie
			// qui sert à vérifier si une conf a déjà été générée
			// pour ne pas le re-considérer une 2e fois.
			// On se limitera donc aux MAXPARTIE derniers coups

// Type d'une configuration
struct config {
	char mat[8][8];			// Echiquier
	int val;			// Estimation de la config
	char xrN, yrN, xrB, yrB;	// Positions des rois Noir et Blanc
	char roqueN, roqueB;		// Indicateurs de roque pour N et B :
					// 'g' grand roque non réalisable
					// 'p' petit roque non réalisable
					// 'n' non réalisable des 2 cotés
					// 'r' réalisable (valeur initiale)
					// 'e' effectué
};

/**************************/
/* Entête des fonctions : */
/**************************/

// MinMax avec élagage Alpha-Bêta, les paramètres sont expliqués dans le commentaire en début de fonction
int minmax_ab( struct config *conf, int mode, int niv, int min, int max, int largeur, int numFctEst );

// La fonction d'estimation à utiliser, retourne une valeur dans ]-100, +100[
int estim( struct config *conf );
// quelques fonctions d'estimation disponibles (comme exemples)
int estim1( struct config *conf );
int estim2( struct config *conf );
int estim3( struct config *conf );
int estim4( struct config *conf );
int estim5( struct config *conf );
int estim6( struct config *conf );
/* Votre propre fonction d'estimation */
int estim7( struct config *conf );

// Génère les successeurs de la configuration conf dans le tableau T,
//   retourne aussi dans n le nb de configurations filles générées.
void generer_succ( struct config *conf, int mode, struct config T[], int *n );

// Génère dans T les configurations obtenues à partir de conf lorsqu'un pion atteint la limite de l'échiq
void transformPion( struct config *conf, int a, int b, int x, int y, struct config T[], int *n );

// Génere dans T tous les coups possibles de la pièce (de couleur N ou B) se trouvant à la pos x,y
void deplacementsN(struct config *conf, int x, int y, struct config T[], int *n );
void deplacementsB(struct config *conf, int x, int y, struct config T[], int *n );

// Vérifie si la case (x,y) est menacée par une des pièces du joueur 'mode'
int caseMenaceePar( int mode, int x, int y, struct config *conf );

// Intialise la disposition des pieces dans la configuration initiale conf
void init( struct config *conf );

// Affiche la configuration conf
void affich( struct config *conf, char *coup, int num );

// Teste si conf représente une configuration déjà jouée (dans le tableau Partie)
// auquel cas elle retourne le num du coup + 1
int dejaVisitee( struct config *conf );

// Savegarde la config courante dans le fichier f (global)
void sauvConf( struct config *conf );

// Pour copier les configurations
void copier( struct config *c1, struct config *c2 );

// Teste si les 2 grilles (échiquiers) c1 et c2 sont égales
int egal(char c1[8][8], char c2[8][8] );

// Teste s'il n'y a aucun coup possible à partir de la configuration conf
int AucunCoupPossible( struct config *conf );

// Teste si conf représente une fin de partie et retourne dans 'cout' son score -100, 0 ou +100
int feuille( struct config *conf, int *cout );

// Comparaisons de configuration avec qsort pour l'ordre croissant et décroissant
int confcmp123(const void *a, const void *b);
int confcmp321(const void *a, const void *b);

// Génère un texte décrivant le dernier coup effectué (pour l'affichage)
void formuler_coup( struct config *oldconf, struct config *newconf, char *coup );





/************************/
/* Variables Globales : */
/************************/

// Tableau de config pour garder la trace des conf déjà visitées
struct config Partie[MAXPARTIE];

// Fichier pour sauvegarder l'historique des parties
FILE *f;

// compteur de coups effectués
int num_coup = 0;
int count = 0;

// profondeur initiale d'exploration préliminaire avant le tri des alternatives
int h0 = 1;

// tableau des fonctions d'estimations
int (*Est[10])(struct config *);

// nb de fonctions d'estimation dans le tableau précédent
int nbEst;

// vecteurs pour générer les différents déplacements par type de pièce ...
//    cavalier :
int dC[8][2] = { {-2,+1} , {-1,+2} , {+1,+2} , {+2,+1} , {+2,-1} , {+1,-2} , {-1,-2} , {-2,-1} };
//    fou (indices impairs), tour (indices pairs), reine et roi (indices pairs et impairs):
int D[8][2] = { {+1,0} , {+1,+1} , {0,+1} , {-1,+1} , {-1,0} , {-1,-1} , {0,-1} , {+1,-1} };

// pour statistques sur le nombre de coupes effectuées
int nbAlpha = 0;
int nbBeta = 0;




/*******************************************/
/*********** Programme principal  **********/
/*******************************************/

int main( int argc, char *argv[] )
{

   int n, i, j, score, stop, cout, hauteur, largeur, tour, estMin, estMax;
   char coup[20] = "";
   char nomf[20];  // nom du fichier de sauvegarde


   struct config T[100], conf, conf1;

   // initialiser le tableau des fonctions d'estimation
   Est[0] = estim1;
   Est[1] = estim2;
   Est[2] = estim3;
   Est[3] = estim4;
   Est[4] = estim5;
   Est[5] = estim6;
   Est[6] = estim7;
   // Nombre de fonctions d'estimation disponibles
   nbEst = 7;

   do {
      printf("Les fonctions d'estimations disponibles sont:\n");
      printf("1- basée uniquement sur le nombre de pièces\n");
      printf("2- basée sur le nb de pieces, l'occupation, la défense du roi et les roques\n");
      printf("3- basée sur le nb de pièces et une perturbation aléatoire\n");
      printf("4- basée sur le nb de pieces et les menaces\n");
      printf("5- basée sur le nb de pieces et l'occupation\n");
      printf("6- basée sur une combinaisant de 3 estimations: (2 -> 5 -> 4)\n");
      printf("7- Votre propre fonction d'estimation (doit être meilleure que les 6 autres)\n\n");
      printf("Donnez le num (entre 1 et %d) de la fonction d'estimation utilisée pour le joueur Maximisant : ", nbEst);
      scanf(" %d", &estMax);
      printf("Donnez le num (entre 1 et %d) de la fonction d'estimation utilisée pour le joueur Minimisant : ", nbEst);
      scanf(" %d", &estMin);
   }
   while ( estMax < 1 || estMax > nbEst || estMin < 1 || estMin > nbEst );

   estMax--;
   estMin--;

   hauteur = 2;  	// par défaut on fixe la profondeur d'évaluation à 4
   largeur = +INFINI;	// et la largeur à l'infini (c-a-d le nb d'alternatives à chaque coup)

   // sinon on peut les récupérer depuis la ligne de commande
   if ( argc == 2 ) {
	hauteur = atoi( argv[1] );
	largeur = +INFINI;
   }
   else
	if ( argc == 3 ) {
	   hauteur = atoi( argv[1] );
	   largeur = atoi( argv[2] );
   	}

   if ( largeur == +INFINI ) {
   	printf("\n\nProfondeur d'exploration = %d\n\n", hauteur);
   }
   else {
        printf("\n\nProfondeur d'exploration = %d  \t largeur d'exploration = %d\n\n", \
		hauteur, largeur);
   }

   // Initialise la configuration de départ
   init( &conf );
   for (i=0; i<MAXPARTIE; i++)
      	copier( &conf, &Partie[i] );

   num_coup = 0;

   // initialise le générateur de nombre aléatoire pour la fonction estim3(...) uniquement
   srandom( time(NULL) );
   //srand( time(NULL) ); // l'équivalent de srandom() dans Windows

   printf("Donnez le nom du fichier texte où sera sauvegarder la partie : ");
   scanf(" %s", nomf);

   printf("\n\nLes '+' : joueur maximisant et les '-' : joueur minimisant\n\n");

   f = fopen(nomf, "w");
   fprintf(f, "--- Estim_B = %d \t Estim N = %d ---\n", estMax+1, estMin+1);

   // Boucle principale du déroulement d'une partie ...
   stop = 0;
   tour = MAX;	// le joueur MAX commence en premier
   while ( !stop ) {

	affich( &conf, coup, num_coup );
	copier( &conf, &Partie[num_coup % MAXPARTIE] );	// rajouter conf au tableau 'Partie'
	sauvConf( &conf );

	if ( tour == MAX ) {
		printf("Au tour du joueur maximisant '+' "); fflush(stdout);

	    	generer_succ(  &conf, MAX, T, &n );
		printf("\nhauteur = %d    nb alternatives = %d : ", hauteur, n); fflush(stdout);

		// Iterative Deepening ...
   		// On effectue un tri sur les alternatives selon l'estimation de leur qualité
   		// Le but est d'explorer les alternatives les plus prometteuses d'abord
   		// pour maximiser les coupes lors des évaluation minmax avec alpha-bêta

		// 1- on commence donc par une petite exploration de profondeur h0
		//    pour récupérer des estimations plus précises sur chaque coups:
	    	for (i=0; i<n; i++)
                T[i].val = minmax_ab( &T[i], MIN, h0, -INFINI, +INFINI, largeur, estMax );

		// 2- on réalise le tri des alternatives T suivant les estimations récupérées:
		qsort(T, n, sizeof(struct config), confcmp321);
		if ( largeur < n ) n = largeur;
		// 3- on lance l'exploration des alternatives triées avec la profondeur voulue:
	    	score = -INFINI;
	    	j = -1;
	    	for (i=0; i<n; i++) {
                cout = minmax_ab( &T[i], MIN, hauteur, score, +INFINI, largeur, estMax );
		   printf("."); fflush(stdout);
		   // printf(" %4d", cout); fflush(stdout);
		   if ( cout > score ) {  // Choisir le meilleur coup (c-a-d le plus grand score)
		   	score = cout;
		   	j = i;
		   }
	    	}
	    	if ( j != -1 ) { // jouer le coup et aller à la prochaine itération ...
		   printf("\n");
		   formuler_coup( &conf, &T[j], coup );
	    	   copier( &T[j], &conf );
		   conf.val = score;
	    	}
	    	else { // S'il n'y a pas de successeur possible, le joueur MAX à perdu
		   printf("\n *** le joueur maximisant '+' a perdu ***\n");
		   fprintf(f, "Victoire de N\n");
		   stop = 1;
	    	}

		tour = MIN;
	}

	else {  // donc tour == MIN
		printf("Au tour du joueur minimisant '-' "); fflush(stdout);

	    	generer_succ(  &conf, MIN, T, &n );
		printf("\nhauteur = %d    nb alternatives = %d : ", hauteur, n); fflush(stdout);

		// Iterative Deepening ...
   		// On effectue un tri sur les alternatives selon l'estimation de leur qualité
   		// Le but est d'explorer les alternatives les plus prometteuses d'abord
   		// pour maximiser les coupes lors des évaluation minmax avec alpha-bêta

		// 1- on commence donc par une petite exploration de profondeur h0
		//    pour récupérer des estimations plus précises sur chaque coups:
	    	for (i=0; i<n; i++)
		     T[i].val = minmax_ab( &T[i], MAX, h0, -INFINI, +INFINI, largeur, estMin );

		// 2- on réalise le tri des alternatives T suivant les estimations récupérées:
		qsort(T, n, sizeof(struct config), confcmp123);
		if ( largeur < n ) n = largeur;

		// 3- on lance l'exploration des alternatives triées avec la profondeur voulue:
	    	score = +INFINI;
	    	j = -1;

	    	for (i=0; i<n; i++) {
		   nbAlpha = nbBeta = 0;
		   cout = minmax_ab( &T[i], MAX, hauteur, -INFINI, score, largeur, estMin );
		   printf("."); fflush(stdout);
		   //printf(" %4d", cout); fflush(stdout);
		   if ( cout < score ) {  // Choisir le meilleur coup (c-a-d le plus petit score)
		   	score = cout;
		   	j = i;
		   }
	    	}
	    	if ( j != -1 ) { // jouer le coup et aller à la prochaine itération ...
		   printf("\n");
		   formuler_coup( &conf, &T[j], coup );
	    	   copier( &T[j], &conf );
		   conf.val = score;
	    	}
	    	else { // S'il n'y a pas de successeur possible, le joueur MIN à perdu
		   printf("\n *** le joueur minimisant '-' a perdu ***\n");
		   fprintf(f, "Victoire de B\n");
		   stop = 1;
	    	}

		tour = MAX;
	}

	num_coup++;

   } // while

   fclose(f);

}  // main




/*****************************************/
/* Implémentation des autres fonctions : */
/*****************************************/


// *****************************
// Partie: Fonctions utilitaires
// *****************************


/* Sauvegarder conf dans le fichier f (pour l'historique) */
void sauvConf( struct config *conf )
{

   char buf[72] = "";
   int i, j;
   for (i=7; i>=0; i--) {
	for (j=0; j<8; j++)
	   if (conf->mat[i][j] == 0)  strcat(buf," ");
	   else if (conf->mat[i][j] < 0) {
		   strcat(buf, "-");
		   buf[strlen(buf)-1] = -conf->mat[i][j];
		}
		else {
		   strcat(buf, "+");
		   buf[strlen(buf)-1] = conf->mat[i][j] - 32;
		}
	strcat(buf, "\n");
   }
   fprintf(f, "--- coup N° %d ---\n%s\n",num_coup, buf);
   fflush(f);

} // fin sauvConf


/* Intialise la disposition des pieces dans la configuration initiale conf */
void init( struct config *conf )
{
   	int i, j;

    	for (i=0; i<8; i++)
		for (j=0; j<8; j++)
			conf->mat[i][j] = 0;	// Les cases vides sont initialisées avec 0

	conf->mat[0][0]='t'; conf->mat[0][1]='c'; conf->mat[0][2]='f'; conf->mat[0][3]='n';
	conf->mat[0][4]='r'; conf->mat[0][5]='f'; conf->mat[0][6]='c'; conf->mat[0][7]='t';

	for (j=0; j<8; j++) {
		conf->mat[1][j] = 'p';
 		conf->mat[6][j] = -'p';
		conf->mat[7][j] = -conf->mat[0][j];
	}

	conf->xrB = 0; conf->yrB = 4;
	conf->xrN = 7; conf->yrN = 4;

	conf->roqueB = 'r';
	conf->roqueN = 'r';

	conf->val = 0;

} // fin de init


/* génère un texte décrivant le dernier coup effectué (pour l'affichage) */
void formuler_coup( struct config *oldconf, struct config *newconf, char *coup )
{
   	int i,j;
	char piece[20];

	// verifier si roqueB effectué ...
	if ( newconf->roqueB == 'e' && oldconf->roqueB != 'e' ) {
	   if ( newconf->yrB == 2 ) sprintf(coup, "g_roqueB" );
	   else  sprintf(coup, "p_roqueB" );
	   return;
	}

	// verifier si roqueN effectué ...
	if ( newconf->roqueN == 'e' && oldconf->roqueN != 'e' ) {
	   if ( newconf->yrN == 2 ) sprintf(coup, "g_roqueN" );
	   else  sprintf(coup, "p_roqueN" );
	   return;
	}

	// Autres mouvements de pièces
	for(i=0; i<8; i++)
	   for (j=0; j<8; j++)
		if ( oldconf->mat[i][j] != newconf->mat[i][j] )
		   if ( newconf->mat[i][j] != 0 ) {
			switch (newconf->mat[i][j]) {
			   case -'p' : sprintf(piece,"pionN"); break;
			   case 'p' : sprintf(piece,"pionB"); break;
			   case -'c' : sprintf(piece,"cavalierN"); break;
			   case 'c' : sprintf(piece,"cavalierB"); break;
			   case -'f' : sprintf(piece,"fouN"); break;
			   case 'f' : sprintf(piece,"fouB"); break;
			   case -'t' : sprintf(piece,"tourN"); break;
			   case 't' : sprintf(piece,"tourB"); break;
			   case -'n' : sprintf(piece,"reineN"); break;
			   case 'n' : sprintf(piece,"reineB"); break;
			   case -'r' : sprintf(piece,"roiN"); break;
			   case 'r' : sprintf(piece,"roiB"); break;
			}
			sprintf(coup, "%s en %c%d", piece, 'a'+j, i+1);
			return;
		   }
} // fin de formuler_coup


/* Affiche la configuration conf */
void affich( struct config *conf, char *coup, int num )
{
	int i, j, k;
	int pB = 0, pN = 0, cB = 0, cN = 0, fB = 0, fN = 0, tB = 0, tN = 0, nB = 0, nN = 0;

     	printf("Coup num:%3d : %s\n", num, coup);
     	printf("\n");
	for (i=0;  i<8; i++)
		printf("\t   %c", i+'a');
   	printf("\n");

	for (i=0;  i<8; i++)
		printf("\t-------");
   	printf("\n");

	for(i=8; i>0; i--)  {
		printf("    %d", i);
		for (j=0; j<8; j++) {
			if ( conf->mat[i-1][j] < 0 ) printf("\t  %cN", -conf->mat[i-1][j]);
			else if ( conf->mat[i-1][j] > 0 ) printf("\t  %cB", conf->mat[i-1][j]);
				  else printf("\t   ");
			switch (conf->mat[i-1][j]) {
			    case -'p' : pN++; break;
			    case 'p'  : pB++; break;
			    case -'c' : cN++; break;
			    case 'c'  : cB++; break;
			    case -'f' : fN++; break;
			    case 'f'  : fB++; break;
			    case -'t' : tN++; break;
			    case 't'  : tB++; break;
			    case -'n' : nN++; break;
			    case 'n'  : nB++; break;
			}
		}
		printf("\n");

		for (k=0;  k<8; k++)
			printf("\t-------");
   		printf("\n");

	}
	printf("\n\tB : p(%d) c(%d) f(%d) t(%d) n(%d) \t N : p(%d) c(%d) f(%d) t(%d) n(%d)\n\n",
		pB, cB, fB, tB, nB, pN, cN, fN, tN, nN);
	printf("\n");

} // fin de  affich


/* Copie la configuration c1 dans c2  */
void copier( struct config *c1, struct config *c2 )
{
	int i, j;

	for (i=0; i<8; i++)
		for (j=0; j<8; j++)
			c2->mat[i][j] = c1->mat[i][j];

	c2->val = c1->val;
	c2->xrB = c1->xrB;
	c2->yrB = c1->yrB;
	c2->xrN = c1->xrN;
	c2->yrN = c1->yrN;

	c2->roqueB = c1->roqueB;
	c2->roqueN = c1->roqueN;
} // fin de copier


/* Teste si les échiquiers c1 et c2 sont égaux */
int egal(char c1[8][8], char c2[8][8] )
{
	int i, j;

	for (i=0; i<8; i++)
		for (j=0; j<8; j++)
			if (c1[i][j] != c2[i][j]) return 0;
	return 1;
} // fin de egal


/* Teste si conf a déjà été jouée (dans le tableau partie) */
int dejaVisitee( struct config *conf )
{
   int i = 0;
   int trouv = 0;
   while ( i < MAXPARTIE && trouv == 0 )
	if ( egal( conf->mat, Partie[i].mat ) ) trouv = i+1;
	else i++;
   return trouv;
}



// ***********************************
// Partie:  Evaluations et Estimations
// ***********************************


/* Teste s'il n'y a aucun coup possible dans la configuration conf */
int AucunCoupPossible( struct config *conf )
{
      	// ... A completer pour les matchs nuls
	// ... vérifier que generer_succ retourne 0 configurations filles ...
	// ... ou qu'une même configuration a été générée plusieurs fois ...
	return 0;

} // fin de AucunCoupPossible


/* Teste si conf représente une fin de partie et retourne dans 'cout' la valeur associée */
int feuille( struct config *conf, int *cout )
{
	//int i, j, rbx, rnx, rby, rny;

	*cout = 0;

	// Si victoire pour les Noirs cout = -100
	if ( conf->xrB == -1 ) {
	   *cout = -100;
	   return 1;
	}

	// Si victoire pour les Blancs cout = +100
	if ( conf->xrN == -1 ) {
	   *cout = +100;
	   return 1;
	}

	// Si Match nul cout = 0
	if (  conf->xrB != -1 &&  conf->xrN != -1 && AucunCoupPossible( conf ) )
	   return 1;

	// Sinon ce n'est pas une config feuille
	return 0;

}  // fin de feuille


/* Quelques exemples de fonctions d'estimation simples (estim1, estim2, ...estim5) */
/* Voir fonction estim plus bas pour le choix l'estimation à utiliser */

/* cette estimation est basée uniquement sur le nombre de pièces */
int estim1( struct config *conf )
{

	int i, j, ScrQte;
	int pionB = 0, pionN = 0, cfB = 0, cfN = 0, tB = 0, tN = 0, nB = 0, nN = 0;

	// parties : nombre de pièces
	for (i=0; i<8; i++)
	   for (j=0; j<8; j++) {
		switch (conf->mat[i][j]) {
		   case 'p' : pionB++;   break;
		   case 'c' :
		   case 'f' : cfB++;  break;
		   case 't' : tB++; break;
		   case 'n' : nB++;  break;

		   case -'p' : pionN++;  break;
		   case -'c' :
		   case -'f' : cfN++;  break;
		   case -'t' : tN++; break;
		   case -'n' : nN++;  break;
		}
	   }

	// Somme pondérée de pièces de chaque joueur.
	// Les poids sont fixés comme suit: pion:2  cavalier/fou:6  tour:8  et  reine:20
	// Le facteur 100/76 pour ne pas sortir de l'intervalle ]-100 , +100[
	ScrQte = ( (pionB*2 + cfB*6 + tB*8 + nB*20) - (pionN*2 + cfN*6 + tN*8 + nN*20) ) * 100.0/76;

	if (ScrQte > 95) ScrQte = 95;		// pour rétrécir l'intervalle à
	if (ScrQte < -95) ScrQte = -95;		// ]-95 , +95[ car ce n'est qu'une estimation

	return ScrQte;

} // fin de estim1


// estimation basée sur le nb de pieces, l'occupation, la défense du roi et les roques
int estim2(  struct config *conf )
{
	int i, j, a, b, stop, bns, ScrQte, ScrDisp, ScrDfs, ScrDivers, Score;
	int pionB = 0, pionN = 0, cfB = 0, cfN = 0, tB = 0, tN = 0, nB = 0, nN = 0;
	int occCentreB = 0, occCentreN = 0, protectRB = 0, protectRN = 0, divB = 0, divN = 0;

	// parties : nombre de pièces et occupation du centre
	for (i=0; i<8; i++)
	   for (j=0; j<8; j++) {
		bns = 0;  // bonus pour l'occupation du centre de l'échiquier
		if (i>1 && i<6 && j>=0 && j<=7 ) bns = 1;
		if (i>2 && i<5 && j>=2 && j<=5 ) bns = 2;
		switch (conf->mat[i][j]) {
		   case 'p' : pionB++; occCentreB += bns;  break;
		   case 'c' :
		   case 'f' : cfB++; occCentreB += 4*bns; break;
		   case 't' : tB++; break;
		   case 'n' : nB++; occCentreB += 4*bns; break;

		   case -'p' : pionN++; occCentreN += bns; break;
		   case -'c' :
		   case -'f' : cfN++; occCentreN += 4*bns; break;
		   case -'t' : tN++; break;
		   case -'n' : nN++; occCentreN += 4*bns; break;
		}
	   }

	ScrQte = ( (pionB*2 + cfB*6 + tB*8 + nB*20) - (pionN*2 + cfN*6 + tN*8 + nN*20) );
	// donc ScrQteMax ==> 76

	ScrDisp = occCentreB - occCentreN;
	// donc ScrDispMax ==> 42

	// partie : défense du roi B ...
	for (i=0; i<8; i += 1) {
	   // traitement des 8 directions paires et impaires
	   stop = 0;
	   a = conf->xrB + D[i][0];
	   b = conf->yrB + D[i][1];
	   while ( !stop && a >= 0 && a <= 7 && b >= 0 && b <= 7 )
		if ( conf->mat[a][b] != 0 )  stop = 1;
		else {
		    a = a + D[i][0];
		    b = b + D[i][1];
		}
	   if ( stop )
		if ( conf->mat[a][b] > 0 ) protectRB++;
	} // for

	// partie : défense du roi N ...
	for (i=0; i<8; i += 1) {
	   // traitement des 8 directions paires et impaires
	   stop = 0;
	   a = conf->xrN + D[i][0];
	   b = conf->yrN + D[i][1];
	   while ( !stop && a >= 0 && a <= 7 && b >= 0 && b <= 7 )
		if ( conf->mat[a][b] != 0 )  stop = 1;
		else {
		    a = a + D[i][0];
		    b = b + D[i][1];
		}
	   if ( stop )
		if ( conf->mat[a][b] < 0 ) protectRN++;
	} // for

	ScrDfs = protectRB - protectRN;
	// donc ScrDfsMax ==> 8

	// Partie : autres considérations ...
	if ( conf->roqueB == 'e' ) divB = 24;	// favoriser les roques B
	if ( conf->roqueB == 'r' ) divB = 12;
	if ( conf->roqueB == 'p' || conf->roqueB == 'g' ) divB = 10;

	if ( conf->roqueN == 'e' ) divN = 24;	// favoriser les roques N
	if ( conf->roqueN == 'r' ) divN = 12;
	if ( conf->roqueN == 'p' || conf->roqueN == 'g' ) divN = 10;

	ScrDivers = divB - divN;
	// donc ScrDiversMax ==> 24

	Score = (4*ScrQte + ScrDisp + ScrDfs + ScrDivers) * 100.0/(4*76+42+8+24);
	// pour les poids des pièces et le facteur multiplicatif voir commentaire dans estim1

        if (Score > 98 ) Score = 98;
        if (Score < -98 ) Score = -98;

	return Score;

} // fin de estim2


/* prise en compte du nombre de pièces avec petite perturbation aléatoire */
int estim3( struct config *conf )
{

	int i, j, ScrQte, Score;
	int pionB = 0, pionN = 0, cfB = 0, cfN = 0, tB = 0, tN = 0, nB = 0, nN = 0;

	// parties : nombre de pièces
	for (i=0; i<8; i++)
	   for (j=0; j<8; j++) {
		switch (conf->mat[i][j]) {
		   case 'p' : pionB++; break;
		   case 'c' :
		   case 'f' : cfB++; break;
		   case 't' : tB++; break;
		   case 'n' : nB++; break;

		   case -'p' : pionN++; break;
		   case -'c' :
		   case -'f' : cfN++; break;
		   case -'t' : tN++; break;
		   case -'n' : nN++; break;
		}
	   }

	ScrQte = ( (pionB*2 + cfB*6 + tB*8 + nB*20) - (pionN*2 + cfN*6 + tN*8 + nN*20) );
	// donc ScrQteMax ==> 76

	Score = (10*ScrQte + random()%10) * 100.0 / (10*76+10);
	//Score = (10*ScrQte + rand()%10) * 100.0 / (10*76+10); // l'équivalent de random() dans Windows
	// pour les poids des pièces et le facteur multiplicatif voir commentaire dans estim1

        if (Score > 98 ) Score = 98;
        if (Score < -98 ) Score = -98;

	return Score;

} // fin de estim3


// estimation basée sur le nb de pieces et les menaces
int estim4( struct config *conf )
{

	int i, j, Score;
	int pionB = 0, pionN = 0, cfB = 0, cfN = 0, tB = 0, tN = 0, nB = 0, nN = 0;
	int npmB = 0, npmN = 0;

	// parties : nombre de pièces
	for (i=0; i<8; i++)
	   for (j=0; j<8; j++) {
		switch (conf->mat[i][j]) {
		   case 'p' : pionB++;   break;
		   case 'c' :
		   case 'f' : cfB++;  break;
		   case 't' : tB++; break;
		   case 'n' : nB++;  break;

		   case -'p' : pionN++;  break;
		   case -'c' :
		   case -'f' : cfN++;  break;
		   case -'t' : tN++; break;
		   case -'n' : nN++;  break;
		}
	   }

	for (i=0; i<8; i++)
	   for (j=0; j<8; j++) {
		if ( conf->mat[i][j] < 0 && caseMenaceePar(MAX, i, j, conf) ) {
		   npmB++;
		   if ( conf->mat[i][j] == -'c' || conf->mat[i][j] == -'f' )
		   	npmB++;
		   if ( conf->mat[i][j] == -'t' || conf->mat[i][j] == -'n' )
		   	npmB += 2;
		   if ( conf->mat[i][j] == -'r' )
		   	npmB += 5;
		}
		if ( conf->mat[i][j] > 0 && caseMenaceePar(MIN, i, j, conf) ) {
		   npmN++;
		   if ( conf->mat[i][j] == 'c' || conf->mat[i][j] == 'f' )
		   	npmN++;
		   if ( conf->mat[i][j] == 't' || conf->mat[i][j] == 'n' )
		   	npmN += 2;
		   if ( conf->mat[i][j] == 'r' )
		   	npmN += 5;
		}
	   }

	Score = ( 4*((pionB*2 + cfB*6 + tB*8 + nB*20) - (pionN*2 + cfN*6 + tN*8 + nN*20)) + \
		  (npmB - npmN) ) * 100.0/(4*76+31);

	// pour les poids des pièces et le facteur multiplicatif voir commentaire dans estim1

	if (Score > 95) Score = 95;		// pour rétrécir l'intervalle à
	if (Score < -95) Score = -95;		// ]-95 , +95[ car ce n'est qu'une estimation

	return Score;

} // fin de estim4


// estimation basée sur le nb de pieces et l'occupation
int estim5(  struct config *conf )
{
	int i, j, a, b, stop, bns, ScrQte, ScrDisp, ScrDfs, ScrDivers, Score;
	int pionB = 0, pionN = 0, cfB = 0, cfN = 0, tB = 0, tN = 0, nB = 0, nN = 0;
	int occCentreB = 0, occCentreN = 0, protectRB = 0, protectRN = 0, divB = 0, divN = 0;

	// parties : nombre de pièces et occupation du centre
	for (i=0; i<8; i++)
	   for (j=0; j<8; j++) {
		bns = 0;  // bonus pour l'occupation du centre de l'échiquier
		if (i>1 && i<6 && j>=0 && j<=7 ) bns = 1;
		if (i>2 && i<5 && j>=2 && j<=5 ) bns = 2;
		switch (conf->mat[i][j]) {
		   case 'p' : pionB++; occCentreB += bns;  break;
		   case 'c' :
		   case 'f' : cfB++; occCentreB += 4*bns; break;
		   case 't' : tB++; break;
		   case 'n' : nB++; occCentreB += 4*bns; break;

		   case -'p' : pionN++; occCentreN += bns; break;
		   case -'c' :
		   case -'f' : cfN++; occCentreN += 4*bns; break;
		   case -'t' : tN++; break;
		   case -'n' : nN++; occCentreN += 4*bns; break;
		}
	   }

	ScrQte = ( (pionB*2 + cfB*6 + tB*8 + nB*20) - (pionN*2 + cfN*6 + tN*8 + nN*20) );

	ScrDisp = occCentreB - occCentreN;

	Score = (4*ScrQte + ScrDisp) * 100.0/(4*76+42);
	// pour les poids des pièces et le facteur multiplicatif voir commentaire dans estim1

        if (Score > 98 ) Score = 98;
        if (Score < -98 ) Score = -98;

	return Score;

} // fin de estim5


/* on peut aussi combiner entre plusieurs estimation comme cet exemple: */
int estim6( struct config *conf )
{
	// Combinaison de 3 fonctions d'estimation:
	// -au début on utilise estim2 pour favoriser l'occupation du centre et la defense du roi
	// -au milieu on utilise estim5 pour continuer à bien se positionner
	// -dans la dernière phase on utilise estim4 pour favoriser l'attaque
	if ( num_coup < 25 )			// 1ere phase
	   return estim2(conf);
	if ( num_coup >= 25 && num_coup < 35 )	// 2e phase
	   return estim5(conf);
	if ( num_coup >= 35 )			// 3e phase
	   return estim4(conf);

} // fin de estim6


/* Votre propre fonction d'estimation : */
int estim7( struct config *conf )
{
	// Mettre ici votre code ...
	// 	...
	// 	...
	Pos pos;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            pos.b[i][j] = conf->mat[7-j][i];
            switch (pos.b[i][j]) {
                case 't':
                    pos.b[i][j] = 'R';
                    break;
                case 'c':
                    pos.b[i][j] = 'N';
                    break;
                case 'f':
                    pos.b[i][j] = 'B';
                    break;
                case 'n':
                    pos.b[i][j] = 'Q';
                    break;
                case 'r':
                    pos.b[i][j] = 'K';
                    break;
                case 'p':
                    pos.b[i][j] = 'P';
                    break;
                case -'t':
                    pos.b[i][j] = 'r';
                    break;
                case -'c':
                    pos.b[i][j] = 'n';
                    break;
                case -'f':
                    pos.b[i][j] = 'b';
                    break;
                case -'n':
                    pos.b[i][j] = 'q';
                    break;
                case -'r':
                    pos.b[i][j] = 'k';
                    break;
                case -'p':
                    pos.b[i][j] = 'p';
                    break;
                case 0:
                    pos.b[i][j] = '-';
                    break;
            }
        }
    }
    switch (conf->roqueB) {
        case 'g':
            pos.c[0] = TRUE;
            pos.c[1] = FALSE;
            break;
        case 'p':
            pos.c[0] = FALSE;
            pos.c[1] = TRUE;
            break;
        case 'n':
            pos.c[0] = FALSE;
            pos.c[1] = FALSE;
            break;
        case 'r':
            pos.c[0] = TRUE;
            pos.c[1] = TRUE;
            break;
        case 'e':
            pos.c[0] = FALSE;
            pos.c[1] = FALSE;
            break;
    }
    switch (conf->roqueN) {
        case 'g':
            pos.c[2] = TRUE;
            pos.c[3] = FALSE;
            break;
        case 'p':
            pos.c[2] = FALSE;
            pos.c[3] = TRUE;
            break;
        case 'n':
            pos.c[2] = FALSE;
            pos.c[3] = FALSE;
            break;
        case 'r':
            pos.c[2] = TRUE;
            pos.c[3] = TRUE;
            break;
        case 'e':
            pos.c[2] = FALSE;
            pos.c[3] = FALSE;
            break;
    }
    for (int i = 0; i < 2; i++) {
        pos.e[i] = NULL;
    }
    pos.w = (num_coup%2==0) ? TRUE : FALSE;
    pos.m[0] = 0;
    pos.m[1] = num_coup/2 + 1;

    double me = main_evaluation(&pos);

	// et surtout remplacer le return ci-dessous par qlq chose de mieux !!!
	return (int) me/250;

} // fin de estim7



// ***********************************
// Partie:  Génération des Successeurs
// ***********************************

/* Génère dans T les configurations obtenues à partir de conf lorsqu'un pion atteint la limite de l'échiq */
void transformPion( struct config *conf, int a, int b, int x, int y, struct config T[], int *n )
{
	int signe = +1;
	if (conf->mat[a][b] < 0 ) signe = -1;
	copier(conf, &T[*n]);
	T[*n].mat[a][b] = 0;
	T[*n].mat[x][y] = signe *'n';	// transformation en Reine
	(*n)++;
	copier(conf, &T[*n]);
	T[*n].mat[a][b] = 0;
	T[*n].mat[x][y] = signe *'c';	// transformation en Cavalier
	(*n)++;
	copier(conf, &T[*n]);
	T[*n].mat[a][b] = 0;
	T[*n].mat[x][y] = signe *'f';	// transformation en Fou
	(*n)++;
	copier(conf, &T[*n]);
	T[*n].mat[a][b] = 0;
	T[*n].mat[x][y] = signe *'t';	// transformation en Tour
	(*n)++;

} // fin de transformPion


// Vérifie si la case (x,y) est menacée par une des pièces du joueur 'mode'
int caseMenaceePar( int mode, int x, int y, struct config *conf )
{
	int i, j, a, b, stop;

	// menace par le roi ...
	for (i=0; i<8; i += 1) {
	   // traitement des 8 directions paires et impaires
	   a = x + D[i][0];
	   b = y + D[i][1];
	   if ( a >= 0 && a <= 7 && b >= 0 && b <= 7 )
		if ( conf->mat[a][b]*mode == 'r' ) return 1;
	} // for

	// menace par cavalier ...
	for (i=0; i<8; i++)
	   if ( x+dC[i][0] <= 7 && x+dC[i][0] >= 0 && y+dC[i][1] <= 7 && y+dC[i][1] >= 0 )
		if ( conf->mat[ x+dC[i][0] ] [ y+dC[i][1] ] * mode == 'c' )
		   return 1;

	// menace par pion ...
	if ( (x-mode) >= 0 && (x-mode) <= 7 && y > 0 && conf->mat[x-mode][y-1]*mode == 'p' )
	   return 1;
	if ( (x-mode) >= 0 && (x-mode) <= 7 && y < 7 && conf->mat[x-mode][y+1]*mode == 'p' )
	   return 1;

	// menace par fou, tour ou reine ...
	for (i=0; i<8; i += 1) {
	   // traitement des 8 directions paires et impaires
	   stop = 0;
	   a = x + D[i][0];
	   b = y + D[i][1];
	   while ( !stop && a >= 0 && a <= 7 && b >= 0 && b <= 7 )
		if ( conf->mat[a][b] != 0 )  stop = 1;
		else {
		    a = a + D[i][0];
		    b = b + D[i][1];
		}
	   if ( stop )  {
		if ( conf->mat[a][b]*mode == 'f' && i % 2 != 0 ) return 1;
		if ( conf->mat[a][b]*mode == 't' && i % 2 == 0 ) return 1;
		if ( conf->mat[a][b]*mode == 'n' ) return 1;
	   }
	} // for

	// sinon, aucune menace ...
	return 0;

} // fin de caseMenaceePar


/* Génere dans T tous les coups possibles de la pièce (de couleur N) se trouvant à la pos x,y */
void deplacementsN(struct config *conf, int x, int y, struct config T[], int *n )
{
	int i, j, a, b, stop;

	switch(conf->mat[x][y]) {
	// mvmt PION ...
	case -'p' :
		if ( x > 0 && conf->mat[x-1][y] == 0 ) {				// avance d'une case
			copier(conf, &T[*n]);
			T[*n].mat[x][y] = 0;
			T[*n].mat[x-1][y] = -'p';
			(*n)++;
			if ( x == 1 ) transformPion( conf, x, y, x-1, y, T, n );
		}
		if ( x == 6 && conf->mat[5][y] == 0 && conf->mat[4][y] == 0) {	// avance de 2 cases
			copier(conf, &T[*n]);
			T[*n].mat[6][y] = 0;
			T[*n].mat[4][y] = -'p';
			(*n)++;
		}
		if ( x > 0 && y >0 && conf->mat[x-1][y-1] > 0 ) {		// mange à droite (en descendant)
			copier(conf, &T[*n]);
			T[*n].mat[x][y] = 0;
			T[*n].mat[x-1][y-1] = -'p';
			// cas où le roi adverse est pris...
			if (T[*n].xrB == x-1 && T[*n].yrB == y-1) {
				T[*n].xrB = -1; T[*n].yrB = -1;
			}

			(*n)++;
			if ( x == 1 ) transformPion( conf, x, y, x-1, y-1, T, n );
		}
		if ( x > 0 && y < 7 && conf->mat[x-1][y+1] > 0 ) {		// mange à gauche (en descendant)
			copier(conf, &T[*n]);
			T[*n].mat[x][y] = 0;
			T[*n].mat[x-1][y+1] = -'p';
			// cas où le roi adverse est pris...
			if (T[*n].xrB == x-1 && T[*n].yrB == y+1) {
				T[*n].xrB = -1; T[*n].yrB = -1;
			}

			(*n)++;
			if ( x == 1 ) transformPion( conf, x, y, x-1, y+1, T, n );
		}
		break;

	// mvmt CAVALIER ...
	case -'c' :
		for (i=0; i<8; i++)
		   if ( x+dC[i][0] <= 7 && x+dC[i][0] >= 0 && y+dC[i][1] <= 7 && y+dC[i][1] >= 0 )
			if ( conf->mat[ x+dC[i][0] ] [ y+dC[i][1] ] >= 0 )  {
			   copier(conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   T[*n].mat[ x+dC[i][0] ][ y+dC[i][1] ] = -'c';
			   // cas où le roi adverse est pris...
			   if (T[*n].xrB == x+dC[i][0] && T[*n].yrB == y+dC[i][1]) {
				T[*n].xrB = -1; T[*n].yrB = -1;
			   }

			   (*n)++;
			}
		break;

	// mvmt FOU ...
	case -'f' :
		for (i=1; i<8; i += 2) {
		   // traitement des directions impaires (1, 3, 5 et 7)
		   stop = 0;
		   a = x + D[i][0];
		   b = y + D[i][1];
		   while ( !stop && a >= 0 && a <= 7 && b >= 0 && b <= 7 ) {
			if ( conf->mat[ a ] [ b ] < 0 )  stop = 1;
			else {
			   copier(conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   if ( T[*n].mat[a][b] > 0 ) stop = 1;
			   T[*n].mat[a][b] = -'f';
			   // cas où le roi adverse est pris...
			   if (T[*n].xrB == a && T[*n].yrB == b) { T[*n].xrB = -1; T[*n].yrB = -1; }

			   (*n)++;
		   	   a = a + D[i][0];
		   	   b = b + D[i][1];
			}
		   } // while
		} // for
		break;

	// mvmt TOUR ...
	case -'t' :
		for (i=0; i<8; i += 2) {
		   // traitement des directions paires (0, 2, 4 et 6)
		   stop = 0;
		   a = x + D[i][0];
		   b = y + D[i][1];
		   while ( !stop && a >= 0 && a <= 7 && b >= 0 && b <= 7 ) {
			if ( conf->mat[ a ] [ b ] < 0 )  stop = 1;
			else {
			   copier(conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   if ( T[*n].mat[a][b] > 0 ) stop = 1;
			   T[*n].mat[a][b] = -'t';
			   // cas où le roi adverse est pris...
			   if (T[*n].xrB == a && T[*n].yrB == b) { T[*n].xrB = -1; T[*n].yrB = -1; }

			   if ( conf->roqueN != 'e' && conf->roqueN != 'n' ) {
			      if ( x == 7 && y == 0 && conf->roqueN != 'p')
			   	T[*n].roqueN = 'g'; // le grand roque ne sera plus possible
			      else if ( x == 7 && y == 0 )
			   	   T[*n].roqueN = 'n'; // ni le grand roque ni le petit roque ne seront possibles
			      if ( x == 7 && y == 7 && conf->roqueN != 'g' )
			   	T[*n].roqueN = 'p'; // le petit roque ne sera plus possible
			      else if ( x == 7 && y == 7 )
			   	   T[*n].roqueN = 'n'; // ni le grand roque ni le petit roque ne seront possibles
			   }

			   (*n)++;
		   	   a = a + D[i][0];
		   	   b = b + D[i][1];
			}
		   } // while
		} // for
		break;

	// mvmt REINE ...
	case -'n' :
		for (i=0; i<8; i += 1) {
		   // traitement des 8 directions paires et impaires
		   stop = 0;
		   a = x + D[i][0];
		   b = y + D[i][1];
		   while ( !stop && a >= 0 && a <= 7 && b >= 0 && b <= 7 ) {
			if ( conf->mat[ a ] [ b ] < 0 )  stop = 1;
			else {
			   copier(conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   if ( T[*n].mat[a][b] > 0 ) stop = 1;
			   T[*n].mat[a][b] = -'n';
			   // cas où le roi adverse est pris...
			   if (T[*n].xrB == a && T[*n].yrB == b) { T[*n].xrB = -1; T[*n].yrB = -1; }

			   (*n)++;
		   	   a = a + D[i][0];
		   	   b = b + D[i][1];
			}
		   } // while
		} // for
		break;

	// mvmt ROI ...
	case -'r' :
		// vérifier possibilité de faire un roque ...
		if ( conf->roqueN != 'n' && conf->roqueN != 'e' ) {
		   if ( conf->roqueN != 'g' && conf->mat[7][1] == 0 && conf->mat[7][2] == 0 && conf->mat[7][3] == 0 )
		      if ( !caseMenaceePar( MAX, 7, 1, conf ) && !caseMenaceePar( MAX, 7, 2, conf ) && \
			   !caseMenaceePar( MAX, 7, 3, conf ) && !caseMenaceePar( MAX, 7, 4, conf ) )  {
			// Faire un grand roque ...
			copier(conf, &T[*n]);
			T[*n].mat[7][4] = 0;
			T[*n].mat[7][0] = 0;
			T[*n].mat[7][2] = -'r'; T[*n].xrN = 7; T[*n].yrN = 2;
			T[*n].mat[7][3] = -'t';
			T[*n].roqueN = 'e'; // aucun roque ne sera plus possible à partir de cette config
			(*n)++;
		      }
		   if ( conf->roqueN != 'p' && conf->mat[7][5] == 0 && conf->mat[7][6] == 0 )
		      if ( !caseMenaceePar( MAX, 7, 4, conf ) && !caseMenaceePar( MAX, 7, 5, conf ) && \
			   !caseMenaceePar( MAX, 7, 6, conf ) )  {
			// Faire un petit roque ...
			copier(conf, &T[*n]);
			T[*n].mat[7][4] = 0;
			T[*n].mat[7][7] = 0;
			T[*n].mat[7][6] = -'r'; T[*n].xrN = 7; T[*n].yrN = 6;
			T[*n].mat[7][5] = -'t';
			T[*n].roqueN = 'e'; // aucun roque ne sera plus possible à partir de cette config
			(*n)++;

		      }
		}

		// vérifier les autres mouvements du roi ...
		for (i=0; i<8; i += 1) {
		   // traitement des 8 directions paires et impaires
		   a = x + D[i][0];
		   b = y + D[i][1];
		   if ( a >= 0 && a <= 7 && b >= 0 && b <= 7 )
			if ( conf->mat[a][b] >= 0 ) {
			   copier(conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   T[*n].mat[a][b] = -'r'; T[*n].xrN = a; T[*n].yrN = b;
			   // cas où le roi adverse est pris...
			   if (T[*n].xrB == a && T[*n].yrB == b) { T[*n].xrB = -1; T[*n].yrB = -1; }

			   T[*n].roqueN = 'n'; // aucun roque ne sera plus possible à partir de cette config
			   (*n)++;
			}
		} // for
		break;

	}

} // fin de deplacementsN


/* Génere dans T tous les coups possibles de la pièce (de couleur B) se trouvant à la pos x,y */
void deplacementsB(struct config *conf, int x, int y, struct config T[], int *n )
{
	int i, j, a, b, stop;

	switch(conf->mat[x][y]) {
	// mvmt PION ...
	case 'p' :
		if ( x <7 && conf->mat[x+1][y] == 0 ) {				// avance d'une case
			copier(conf, &T[*n]);
			T[*n].mat[x][y] = 0;
			T[*n].mat[x+1][y] = 'p';
			(*n)++;
			if ( x == 6 ) transformPion( conf, x, y, x+1, y, T, n );
		}
		if ( x == 1 && conf->mat[2][y] == 0 && conf->mat[3][y] == 0) {	// avance de 2 cases
			copier(conf, &T[*n]);
			T[*n].mat[1][y] = 0;
			T[*n].mat[3][y] = 'p';
			(*n)++;
		}
		if ( x < 7 && y > 0 && conf->mat[x+1][y-1] < 0 ) {		// mange à gauche (en montant)
			copier(conf, &T[*n]);
			T[*n].mat[x][y] = 0;
			T[*n].mat[x+1][y-1] = 'p';
			// cas où le roi adverse est pris...
			if (T[*n].xrN == x+1 && T[*n].yrN == y-1) {
				T[*n].xrN = -1; T[*n].yrN = -1;
			}

			(*n)++;
			if ( x == 6 ) transformPion( conf, x, y, x+1, y-1, T, n );
		}
		if ( x < 7 && y < 7 && conf->mat[x+1][y+1] < 0 ) {		// mange à droite (en montant)
			copier(conf, &T[*n]);
			T[*n].mat[x][y] = 0;
			T[*n].mat[x+1][y+1] = 'p';
			// cas où le roi adverse est pris...
			if (T[*n].xrN == x+1 && T[*n].yrN == y+1) {
				T[*n].xrN = -1; T[*n].yrN = -1;
			}

			(*n)++;
			if ( x == 6 ) transformPion( conf, x, y, x+1, y+1, T, n );
		}
		break;

	// mvmt CAVALIER ...
	case 'c' :
		for (i=0; i<8; i++)
		   if ( x+dC[i][0] <= 7 && x+dC[i][0] >= 0 && y+dC[i][1] <= 7 && y+dC[i][1] >= 0 )
			if ( conf->mat[ x+dC[i][0] ] [ y+dC[i][1] ] <= 0 )  {
			   copier(conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   T[*n].mat[ x+dC[i][0] ][ y+dC[i][1] ] = 'c';
			   // cas où le roi adverse est pris...
			   if (T[*n].xrN == x+dC[i][0] && T[*n].yrN == y+dC[i][1]) {
				T[*n].xrN = -1; T[*n].yrN = -1;
			   }

			   (*n)++;
			}
		break;

	// mvmt FOU ...
	case 'f' :
		for (i=1; i<8; i += 2) {
		   // traitement des directions impaires (1, 3, 5 et 7)
		   stop = 0;
		   a = x + D[i][0];
		   b = y + D[i][1];
		   while ( !stop && a >= 0 && a <= 7 && b >= 0 && b <= 7 ) {
			if ( conf->mat[ a ] [ b ] > 0 )  stop = 1;
			else {
			   copier(conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   if ( T[*n].mat[a][b] < 0 ) stop = 1;
			   T[*n].mat[a][b] = 'f';
			   // cas où le roi adverse est pris...
			   if (T[*n].xrN == a && T[*n].yrN == b) { T[*n].xrN = -1; T[*n].yrN = -1; }

			   (*n)++;
		   	   a = a + D[i][0];
		   	   b = b + D[i][1];
			}
		   } // while
		} // for
		break;

	// mvmt TOUR ...
	case 't' :
		for (i=0; i<8; i += 2) {
		   // traitement des directions paires (0, 2, 4 et 6)
		   stop = 0;
		   a = x + D[i][0];
		   b = y + D[i][1];
		   while ( !stop && a >= 0 && a <= 7 && b >= 0 && b <= 7 ) {
			if ( conf->mat[ a ] [ b ] > 0 )  stop = 1;
			else {
			   copier(conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   if ( T[*n].mat[a][b] < 0 ) stop = 1;
			   T[*n].mat[a][b] = 't';
			   // cas où le roi adverse est pris...
			   if (T[*n].xrN == a && T[*n].yrN == b) { T[*n].xrN = -1; T[*n].yrN = -1; }

			   if ( conf->roqueB != 'e' && conf->roqueB != 'n' ) {
			     if ( x == 0 && y == 0 && conf->roqueB != 'p')
			   	T[*n].roqueB = 'g'; // le grand roque ne sera plus possible
			     else if ( x == 0 && y == 0 )
			   	   T[*n].roqueB = 'n'; // ni le grand roque ni le petit roque ne seront possibles
			     if ( x == 0 && y == 7 && conf->roqueB != 'g' )
			   	T[*n].roqueB = 'p'; // le petit roque ne sera plus possible
			     else if ( x == 0 && y == 7 )
			   	   T[*n].roqueB = 'n'; // ni le grand roque ni le petit roque ne seront possibles
			   }

			   (*n)++;
		   	   a = a + D[i][0];
		   	   b = b + D[i][1];
			}
		   } // while
		} // for
		break;

	// mvmt REINE ...
	case 'n' :
		for (i=0; i<8; i += 1) {
		   // traitement des 8 directions paires et impaires
		   stop = 0;
		   a = x + D[i][0];
		   b = y + D[i][1];
		   while ( !stop && a >= 0 && a <= 7 && b >= 0 && b <= 7 ) {
			if ( conf->mat[ a ] [ b ] > 0 )  stop = 1;
			else {
			   copier(conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   if ( T[*n].mat[a][b] < 0 ) stop = 1;
			   T[*n].mat[a][b] = 'n';
			   // cas où le roi adverse est pris...
			   if (T[*n].xrN == a && T[*n].yrN == b) { T[*n].xrN = -1; T[*n].yrN = -1; }

			   (*n)++;
		   	   a = a + D[i][0];
		   	   b = b + D[i][1];
			}
		   } // while
		} // for
		break;

	// mvmt ROI ...
	case 'r' :
		// vérifier possibilité de faire un roque ...
		if ( conf->roqueB != 'n' && conf->roqueB != 'e' ) {
		   if ( conf->roqueB != 'g' && conf->mat[0][1] == 0 && conf->mat[0][2] == 0 && conf->mat[0][3] == 0 )
		      if ( !caseMenaceePar( MIN, 0, 1, conf ) && !caseMenaceePar( MIN, 0, 2, conf ) && \
			   !caseMenaceePar( MIN, 0, 3, conf ) && !caseMenaceePar( MIN, 0, 4, conf ) )  {
			// Faire un grand roque ...
			copier(conf, &T[*n]);
			T[*n].mat[0][4] = 0;
			T[*n].mat[0][0] = 0;
			T[*n].mat[0][2] = 'r'; T[*n].xrB = 0; T[*n].yrB = 2;
			T[*n].mat[0][3] = 't';
			T[*n].roqueB = 'e'; // aucun roque ne sera plus possible à partir de cette config
			(*n)++;
		      }
		   if ( conf->roqueB != 'p' && conf->mat[0][5] == 0 && conf->mat[0][6] == 0 )
		      if ( !caseMenaceePar( MIN, 0, 4, conf ) && !caseMenaceePar( MIN, 0, 5, conf ) && \
			   !caseMenaceePar( MIN, 0, 6, conf ) )  {
			// Faire un petit roque ...
			copier(conf, &T[*n]);
			T[*n].mat[0][4] = 0;
			T[*n].mat[0][7] = 0;
			T[*n].mat[0][6] = 'r'; T[*n].xrB = 0; T[*n].yrB = 6;
			T[*n].mat[0][5] = 't';
			T[*n].roqueB = 'e'; // aucun roque ne sera plus possible à partir de cette config
			(*n)++;

		      }
		}

		// vérifier les autres mouvements du roi ...
		for (i=0; i<8; i += 1) {
		   // traitement des 8 directions paires et impaires
		   a = x + D[i][0];
		   b = y + D[i][1];
		   if ( a >= 0 && a <= 7 && b >= 0 && b <= 7 )
			if ( conf->mat[a][b] <= 0 ) {
			   copier(conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   T[*n].mat[a][b] = 'r'; T[*n].xrB = a; T[*n].yrB = b;
			   // cas où le roi adverse est pris...
			   if (T[*n].xrN == a && T[*n].yrN == b) { T[*n].xrN = -1; T[*n].yrN = -1; }

			   T[*n].roqueB = 'n'; // aucun roque ne sera plus possible à partir de cette config
			   (*n)++;
			}
		} // for
		break;

	}

} // fin de deplacementsB


/* Génère les successeurs de la configuration conf dans le tableau T,
   retourne aussi dans n le nb de configurations filles générées */
void generer_succ( struct config *conf, int mode, struct config T[], int *n )
{
	int i, j, k, stop;

	*n = 0;

	if ( mode == MAX ) {		// mode == MAX
	   for (i=0; i<8; i++)
	      for (j=0; j<8; j++)
		 if ( conf->mat[i][j] > 0 )
		    deplacementsB(conf, i, j, T, n );

	   // vérifier si le roi est en echec, auquel cas on ne garde que les succ évitants l'échec
	   // ou alors si une conf est déjà visitée dans Partie, auquel cas on l'enlève aussi ...
	   for (k=0; k < *n; k++) {
	      	i = T[k].xrB; j = T[k].yrB;  // pos du roi B dans T[k]
		// vérifier si roi menacé dans la config T[k] ou alors T[k] est dejà visitée ...
		if ( caseMenaceePar( MIN, i, j, &T[k] ) || dejaVisitee( &T[k] ) ) {
		    T[k] = T[(*n)-1];	// alors supprimer T[k] de la liste des succ...
		    (*n)--;
		    k--;
		}
	    } // for k
	}

	else { 				// mode == MIN
	   for (i=0; i<8; i++)
	      for (j=0; j<8; j++)
		 if ( conf->mat[i][j] < 0 )
		    deplacementsN(conf, i, j, T, n );

	   // vérifier si le roi est en echec, auquel cas on ne garde que les succ évitants l'échec
	   // ou alors si une conf est déjà visitée dans Partie, auquel cas on l'enlève aussi ...
	   for (k=0; k < *n; k++) {
		i = T[k].xrN; j = T[k].yrN;
		// vérifier si roi menacé dans la config T[k] ou alors T[k] est dejà visitée ...
		if ( caseMenaceePar( MAX, i, j, &T[k] ) || dejaVisitee( &T[k] ) ) {
		    T[k] = T[(*n)-1];	// alors supprimer T[k] de la liste des succ...
		    (*n)--;
		    k--;
		}
	   } // for k
	} // if (mode == MAX) ... else ...

} // fin de generer_succ



// ******************************
// Partie:  MinMax avec AlphaBeta
// ******************************

/* Fonctions de comparaison utilisées avec qsort     */
// 'a' et 'b' sont des configurations

int confcmp123(const void *a, const void *b)	// pour l'ordre croissant
{
    int x = ((struct config *)a)->val, y = ((struct config *)b)->val;
    if ( x < y )
	return -1;
    if ( x == y )
	return 0;
    return 1;
}  // fin confcmp123

int confcmp321(const void *a, const void *b)	// pour l'ordre decroissant
{
    int x = ((struct config *)a)->val, y = ((struct config *)b)->val;
    if ( x > y )
	return -1;
    if ( x == y )
	return 0;
    return 1;
}  // fin confcmp321


/* MinMax avec élagage alpha-beta */
// évalue la configuration 'conf' du joueur 'mode' en descendant de 'niv' niveaux
// 'min' et 'max' représentent les bornes initiales (alpha et bêta)
// 'largeur' représente le nombre max d'alternatives à explorer en profondeur à chaque niveau
// si largeur == +INFINI toutes les alternatives seront prises en compte (comportement par défaut)
// numFctEst est le numéro de la fonction d'estimation à utiliser lorsqu'on arrive au niveau 0

int minmax_ab( struct config *conf, int mode, int niv, int alpha, int beta, int largeur, int numFctEst )
{
 	int n, i, score, score2;
 	struct config T[100];

   	if ( feuille(conf, &score) )
		return score;

   	if ( niv == 0 )
		return Est[numFctEst]( conf );

   	if ( mode == MAX ) {

	   generer_succ( conf, MAX, T, &n );

 	   if ( largeur != +INFINI ) {
	    	for (i=0; i<n; i++)
		     T[i].val = Est[numFctEst]( &T[i] );

		qsort(T, n, sizeof(struct config), confcmp321);
		if ( largeur < n ) n = largeur;	  // pour limiter la largeur d'exploration
	   }

	   score = alpha;
	   for ( i=0; i<n; i++ ) {
        if ((numFctEst==6)&&(score > num_coup/10)) {
            break;
        }
   	    	score2 = minmax_ab( &T[i], MIN, niv-1, score, beta, largeur, numFctEst);
		if (score2 > score) score = score2;
		if (score >= beta) {
			// Coupe Beta
			nbBeta++;	// compteur de courpes beta
   	      		return beta;
	    	}
	   }
	}
	else  { // mode == MIN

	   generer_succ( conf, MIN, T, &n );

 	   if ( largeur != +INFINI ) {
	    	for (i=0; i<n; i++)
		     T[i].val = Est[numFctEst]( &T[i] );

		qsort(T, n, sizeof(struct config), confcmp123);
		if ( largeur < n ) n = largeur;	  // pour limiter la largeur d'exploration
	   }

	   score = beta;
	   for ( i=0; i<n; i++ ) {
        if ((numFctEst==6)&&(score < -num_coup/10)) {
            break;
        }
   	    	score2 = minmax_ab( &T[i], MAX, niv-1, alpha, score, largeur, numFctEst );
		if (score2 < score) score = score2;
		if (score <= alpha) {
			// Coupe Alpha
			nbAlpha++;	// compteur de courpes alpha
   	      		return alpha;
	    	}

	   }
	}

        if ( score == +INFINI ) score = +100;
        if ( score == -INFINI ) score = -100;

	return score;

} // fin de minmax_ab

