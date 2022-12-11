/*********************************************************************/
/*********************************************************************/
/***							      	   ***/
/***         	TP MinMax / Module TPGO / ESI 2016-2017            ***/
/***							      	   ***/
/*** Il est demandé d'améliorer la fonction estim(...) ci-dessous. ***/
/*** Son rôle est d'évaluer la qualité d'une configuration donnée. ***/
/*** Elle doit retourner une valeur dans ]-100 , +100[		   ***/
/***							      	   ***/
/*********************************************************************/
/*********************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>  // pour INT_MAX


#define MAX +1	// Niveau Maximisant
#define MIN -1  // Niveau Minimisant
#define INFINI INT_MAX


// Type d'une configuration formant l'espace de recherche
struct config {
	char mat[8][8];			// l'échiquier
	int val;			// estimation de la config
	char xrN, yrN, xrB, yrB;	// positions des rois Noir et Blanc
	char roqueN, roqueB;		// indicateurs de roque pour N et B : 
						// 'g'rand roque non réalisable  
						// 'p'etit roque non réalisable 
						// 'n'on réalisable des 2 cotés
						// 'r'éalisable (valeur initiale)
						// 'e'ffectué
};

// compteur de coups effectués
int coups_jouees = 0;

// vecteurs des déplacements par type de piece ...
// cavalier
int dC[8][2] = { {-2,+1} , {-1,+2} , {+1,+2} , {+2,+1} , {+2,-1} , {+1,-2} , {-1,-2} , {-2,-1} };
// fou (indices impairs), tour (indices pairs), reine et roi (indices pairs et impairs) ...
int D[8][2] = { {+1,0} , {+1,+1} , {0,+1} , {-1,+1} , {-1,0} , {-1,-1} , {0,-1} , {+1,-1} }; 

// evalue avec alpha beta la configuration 'conf' du joueur 'mode' en descendant de 'niv' niveaux
int minmax_ab( struct config conf, int mode, int niv, int min, int max );


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
} // copier


/* Teste si les conf c1 et c2 sont égales */
int egal(char c1[8][8], char c2[8][8] )
{
	int i, j;

	for (i=0; i<8; i++)
		for (j=0; j<8; j++)
			if (c1[i][j] != c2[i][j]) return 0;
	return 1;
} // egal




/***********************************************************/
/*********** Partie:  Evaluations et Estimations ***********/
/***********************************************************/


/* Teste s'il n'y a aucun coup possible dans la configuration conf */
int AucunCoupPossible( struct config conf )
{
      	// ... A completer pour les matchs nuls
	// ... vérifier que generer_succ retourne 0 configurations filles ...
	return 0;

} // AucunCoupPossible


/* Teste si conf représente une fin de partie et retourne dans 'cout' la valeur associée */
int feuille( struct config conf, int *cout )
{
	//int i, j, rbx, rnx, rby, rny;
	
	*cout = 0;

	// Si victoire pour les Noirs cout = -100
	if ( conf.xrB == -1 ) { 
	   *cout = -100;
	   return 1; 
	}

	// Si victoire pour les Blancs cout = +100
	if ( conf.xrN == -1 ) {
	   *cout = +100;
	   return 1;
	}

	// Si Match nul cout = 0
	if (  conf.xrB != -1 &&  conf.xrN != -1 && AucunCoupPossible( conf ) )
	   return 1;

	// Sinon ce n'est pas une config feuille 
	return 0;

}  // feuille


/* Retourne une estimation de la configuration conf */
int estim( struct config conf )
{
	/*
		En analysant quelques statistique sur les parties des jeux d'echec,
		On se rend compte que la moyenne des coups en une partie c'est 41.
		Donc on décidé de diviser la partie en trois:
			-Début de partie: les 20 premiers coups
			-Milieu de partie: entre le 21ème et le 30ème coup.
			-Fin de partie à partir du 31ème coup
		Pour chaque intervalle on a adopté une stratégie pour construire la fonction d'estimation.	
	 */

	/* 	
		Dans le début de la partie on prend en considération
    	le nombre de pièce avec leurs poids et leurs positionnement,
	 	l'occupation du centre ainsi que la défence du roi et les roques. 
	*/
	if ( coups_jouees < 20 ){
		int i, j, a, b, stop, bns, ScrQte, ScrDisp, ScrDfs, ScrDivers, Score;
		int pionB = 0, pionN = 0, cfB = 0, cfN = 0, tB = 0, tN = 0, nB = 0, nN = 0;
		int occCentreB = 0, occCentreN = 0, protectRB = 0, protectRN = 0, divB = 0, divN = 0;

		/*
		Nombre de pièces et occupation du centre:
			On parcours l'échiquier et on calcule le nombre de pièce ainsi que le bonus d'occupation du centre
			Pour le bonus d'occupation du centre, on l'initialise à 1 pour les cases avec i entre 2 et 5 et j entre 0 et 7
			et on l'initialise à 2 pour i entre 3 et 4 et j entre 2 et 5
			mis à part les pions, on multiplie ce bonus fois 4 pour les autres types de pièces.
			Les poids des pièces sont les suivants : 
				pion : 			2
				Cavalier | Fou: 6
				Tour : 			8
				Reine : 		20
			On multiplie le nombre des pièces fois leurs poids et on fait la différence entre les blancs et les noirs
			Le maximum de la variable ScrQte est donc égal à 8*2+6*4+2*8+20*1 = 76
			Et le maximum de la variable ScrDisp qui représente la différence dans l'occupation du centre est donc 42
		*/
	
		for (i=0; i<8; i++)
			for (j=0; j<8; j++) {
				bns = 0;  
				if (i>1 && i<6 && j>=0 && j<=7 ) bns = 1;
				if (i>2 && i<5 && j>=2 && j<=5 ) bns = 2;
				switch (conf.mat[i][j]) {
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

		/*	
		Défense du roi:
			D Matrice des déplacements possibles pour le roi
			Les attributs conf.xrB ... contiennent l'emplacement du roi ( Blanc ou Noir )
			Ici on a une boucle qui fait déplacer le roi dans toute les directions possibles le maximum possible
			On s'arrete lorsque on rencotre une pièce
			On incrémente la variable protectRB (resp protectRN) lorsque la pièce rencontrée est blanche (resp noire)
			A la fin on calcule la différence entre protectRB et protectRN
			Le maximum de cette différence est 8
		*/
		for (i=0; i<8; i += 1) {
			stop = 0;
			a = conf.xrB + D[i][0];
			b = conf.yrB + D[i][1];
			while ( !stop && a >= 0 && a <= 7 && b >= 0 && b <= 7 )
				if ( conf.mat[a][b] != 0 )  stop = 1;
				else {
					a = a + D[i][0];
					b = b + D[i][1];
				}
			if ( stop )
				if ( conf.mat[a][b] > 0 ) protectRB++;
		}

		for (i=0; i<8; i += 1) {
			stop = 0;
			a = conf.xrN + D[i][0];
			b = conf.yrN + D[i][1];
			while ( !stop && a >= 0 && a <= 7 && b >= 0 && b <= 7 )
				if ( conf.mat[a][b] != 0 )  stop = 1;
				else {
					a = a + D[i][0];
					b = b + D[i][1];
				}
			if ( stop )
				if ( conf.mat[a][b] < 0 ) protectRN++;
		}

		ScrDfs = protectRB - protectRN;

		/*
			Les roques:
				'g'rand roque non réalisable 	==> 10  
				'p'etit roque non réalisable 	==> 10 
		   		'n'on réalisable des 2 cotés	==> 0 
				'r'éalisable (valeur initiale)	==> 12 
				'e'ffectué						==> 24 
			Le maximum alors est 24
		*/
		if ( conf.roqueB == 'e' ) divB = 24;
		if ( conf.roqueB == 'r' ) divB = 12;
		if ( conf.roqueB == 'p' || conf.roqueB == 'g' ) divB = 10;

		if ( conf.roqueN == 'e' ) divN = 24;
		if ( conf.roqueN == 'r' ) divN = 12;
		if ( conf.roqueN == 'p' || conf.roqueN == 'g' ) divN = 10;
		ScrDivers = divB - divN;

		/*
			A la fin on calcule le total en donnant une coéfficient 4 à ScrQte vu que c'est la plus importante
			Pour se réduire à l'intervalle [-100,100] :
				score ------> 4*76+42+8+24 ( Les maximum déja expliqué lors de chaque étape )
				x	  ------> 100
				x = score * 100 / 4*76+42+8+24
			Vu que ce n'est qu'une éstimation et que la valeur 100 est réservé aux feuilles, on va rétricir l'intervalle à [-95,95]
		*/
		Score = (4*ScrQte + ScrDisp + ScrDfs + ScrDivers) * 100.0/(4*76+42+8+24);
		if (Score > 95 ) Score = 95;
		if (Score < -95 ) Score = -95;
		return Score;
	}

	/* 
		Dans le milieu de la partie on ne prend en compte que  
		le nombre de pièce avec leurs poids avec leurs positionnemens
		et l'occupation du centre. On ignore par rapport au debut de partie 
		la défense du roi et les roques afin de commencer à favoriser l'attaque.
	*/
	if ( coups_jouees >= 20 && coups_jouees < 30 ){
		int i, j, a, b, stop, bns, ScrQte, ScrDisp, ScrDfs, ScrDivers, Score;
		int pionB = 0, pionN = 0, cfB = 0, cfN = 0, tB = 0, tN = 0, nB = 0, nN = 0;
		int occCentreB = 0, occCentreN = 0, protectRB = 0, protectRN = 0, divB = 0, divN = 0;

		/*
		Nombre de pièces et occupation du centre: (Expliquée dans la partie précédente)
		*/
		for (i=0; i<8; i++)
			for (j=0; j<8; j++) {
				bns = 0;  
				if (i>1 && i<6 && j>=0 && j<=7 ) bns = 1;
				if (i>2 && i<5 && j>=2 && j<=5 ) bns = 2;
				switch (conf.mat[i][j]) {
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
		if (Score > 95 ) Score = 95;
		if (Score < -95 ) Score = -95;
		return Score;
	}

	/* 
		Et enfin, à la fin de la partie on utilise cette estimation qui
	   	favorise l'attaque explicitement en utilisant la fonction case
	   	menacée par.
	*/
	if ( coups_jouees >= 30 ){
		int i, j, Score, ScrQte;
		int pionB = 0, pionN = 0, cfB = 0, cfN = 0, tB = 0, tN = 0, nB = 0, nN = 0;
		int npmB = 0, npmN = 0;

		/*
		Nombre de pièces : (Expliquée dans la partie précédente)
		*/
		for (i=0; i<8; i++)
			for (j=0; j<8; j++) {
				switch (conf.mat[i][j]) {
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
		ScrQte = ( (pionB*2 + cfB*6 + tB*8 + nB*20) - (pionN*2 + cfN*6 + tN*8 + nN*20) );
		/*
		Favoriser l'attaque en calculant le nombre de case menacées
			Pion menacé 		=> 1
			Fou|Cavalier menacé => 3
			Tour|Reine menacé 	=> 6
			Roi menacé 			=> 11

			Le max est 11 + 3*6 + 3*4 + 1*8 = 49
		*/
		for (i=0; i<8; i++)
			for (j=0; j<8; j++) {
				if ( conf.mat[i][j] < 0 && caseMenaceePar(MAX, i, j, conf) ) {
				npmB++;
				if ( conf.mat[i][j] == -'c' || conf.mat[i][j] == -'f' )
					npmB += 2;
				if ( conf.mat[i][j] == -'t' || conf.mat[i][j] == -'n' )
					npmB += 5;
				if ( conf.mat[i][j] == -'r' )
					npmB += 10;
				}
				if ( conf.mat[i][j] > 0 && caseMenaceePar(MIN, i, j, conf) ) {
				npmN++;
				if ( conf.mat[i][j] == 'c' || conf.mat[i][j] == 'f' )
					npmN += 2;
				if ( conf.mat[i][j] == 't' || conf.mat[i][j] == 'n' )
					npmN += 5;
				if ( conf.mat[i][j] == 'r' )
					npmN += 10;
				}
		}
		

		Score = ( 4* ScrQte+ (npmB - npmN) ) * 100.0/(4*76+49);

		if (Score > 95) Score = 95;		
		if (Score < -95) Score = -95;	

		return Score;
	}
}


/***********************************************************/
/*********** Partie:  Génération des Successeurs ***********/
/***********************************************************/


/* Génère dans T les configurations obtenues à partir de conf lorsqu'un pion atteint la limite de l'échiq */
void transformPion( struct config conf, int a, int b, int x, int y, struct config T[], int *n )
{
	int signe = +1;
	if (conf.mat[a][b] < 0 ) signe = -1;
	copier(&conf, &T[*n]);
	T[*n].mat[a][b] = 0;
	T[*n].mat[x][y] = signe *'n';
	(*n)++;
	copier(&conf, &T[*n]);
	T[*n].mat[a][b] = 0;
	T[*n].mat[x][y] = signe *'c';
	(*n)++;
	copier(&conf, &T[*n]);
	T[*n].mat[a][b] = 0;
	T[*n].mat[x][y] = signe *'f';
	(*n)++;
	copier(&conf, &T[*n]);
	T[*n].mat[a][b] = 0;
	T[*n].mat[x][y] = signe *'t';
	(*n)++;

} // transformPion


// Vérifie si la case (x,y) est menacée par une des pièces du joueur 'mode'
int caseMenaceePar( int mode, int x, int y, struct config conf )
{
	int i, j, a, b, stop;

	// menace par le roi ...
	for (i=0; i<8; i += 1) {
	   // traitement des 8 directions paires et impaires
	   a = x + D[i][0];
	   b = y + D[i][1];	  	 
	   if ( a >= 0 && a <= 7 && b >= 0 && b <= 7 ) 
		if ( conf.mat[a][b]*mode == 'r' ) return 1;
	} // for

	// menace par cavalier ...
	for (i=0; i<8; i++)
	   if ( x+dC[i][0] <= 7 && x+dC[i][0] >= 0 && y+dC[i][1] <= 7 && y+dC[i][1] >= 0 )
		if ( conf.mat[ x+dC[i][0] ] [ y+dC[i][1] ] * mode == 'c' )  
		   return 1;

	// menace par pion ...
	if ( (x-mode) >= 0 && (x-mode) <= 7 && y > 0 && conf.mat[x-mode][y-1]*mode == 'p' )
	   return 1;
	if ( (x-mode) >= 0 && (x-mode) <= 7 && y < 7 && conf.mat[x-mode][y+1]*mode == 'p' )
	   return 1;

	// menace par fou, tour ou reine ...
	for (i=0; i<8; i += 1) {
	   // traitement des 8 directions paires et impaires
	   stop = 0;
	   a = x + D[i][0];
	   b = y + D[i][1];	  	 
	   while ( !stop && a >= 0 && a <= 7 && b >= 0 && b <= 7 ) 
		if ( conf.mat[a][b] != 0 )  stop = 1;
		else {
		    a = a + D[i][0];
		    b = b + D[i][1];
		}
	   if ( stop )  {
		if ( conf.mat[a][b]*mode == 'f' && i % 2 != 0 ) return 1; 
		if ( conf.mat[a][b]*mode == 't' && i % 2 == 0 ) return 1;
		if ( conf.mat[a][b]*mode == 'n' ) return 1;
	   }
	} // for

	// sinon, aucune menace ...
	return 0;

} // caseMenaceePar


/* Génere dans T tous les coups possibles de la pièce (de couleur N) se trouvant à la pos x,y */
void deplacementsN(struct config conf, int x, int y, struct config T[], int *n )
{
	int i, j, a, b, stop;

	switch(conf.mat[x][y]) {
	// mvmt PION ...
	case -'p' : 
		//***printf("PION N à la pos (%d,%d) \n", x,y);
		if ( x > 0 && conf.mat[x-1][y] == 0 ) {				// avance d'une case
			copier(&conf, &T[*n]);
			T[*n].mat[x][y] = 0;
			T[*n].mat[x-1][y] = -'p';
			(*n)++;
			if ( x == 1 ) transformPion( conf, x, y, x-1, y, T, n );
		}
		if ( x == 6 && conf.mat[5][y] == 0 && conf.mat[4][y] == 0) {	// avance de 2 cases
			copier(&conf, &T[*n]);
			T[*n].mat[6][y] = 0;
			T[*n].mat[4][y] = -'p';
			(*n)++;
		}
		if ( x > 0 && y >0 && conf.mat[x-1][y-1] > 0 ) {		// mange à droite (en descendant)
			copier(&conf, &T[*n]);
			T[*n].mat[x][y] = 0;
			T[*n].mat[x-1][y-1] = -'p';
			// cas où le roi adverse est pris...
			if (T[*n].xrB == x-1 && T[*n].yrB == y-1) { 
				T[*n].xrB = -1; T[*n].yrB = -1; 
			}

			(*n)++;
			if ( x == 1 ) transformPion( conf, x, y, x-1, y-1, T, n ); 
		}
		if ( x > 0 && y < 7 && conf.mat[x-1][y+1] > 0 ) {		// mange à gauche (en descendant)
			copier(&conf, &T[*n]);
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
			if ( conf.mat[ x+dC[i][0] ] [ y+dC[i][1] ] >= 0 )  {
			   copier(&conf, &T[*n]);
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
			if ( conf.mat[ a ] [ b ] < 0 )  stop = 1;
			else {
			   copier(&conf, &T[*n]);
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
			if ( conf.mat[ a ] [ b ] < 0 )  stop = 1;
			else {
			   copier(&conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   if ( T[*n].mat[a][b] > 0 ) stop = 1;
			   T[*n].mat[a][b] = -'t';
			   // cas où le roi adverse est pris...
			   if (T[*n].xrB == a && T[*n].yrB == b) { T[*n].xrB = -1; T[*n].yrB = -1; }

			   if ( conf.roqueN != 'e' && conf.roqueN != 'n' ) {
			      if ( x == 7 && y == 0 && conf.roqueN != 'p')
			   	T[*n].roqueN = 'g'; // le grand roque ne sera plus possible
			      else if ( x == 7 && y == 0 )
			   	   T[*n].roqueN = 'n'; // ni le grand roque ni le petit roque ne seront possibles
			      if ( x == 7 && y == 7 && conf.roqueN != 'g' )
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
			if ( conf.mat[ a ] [ b ] < 0 )  stop = 1;
			else {
			   copier(&conf, &T[*n]);
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
		if ( conf.roqueN != 'n' && conf.roqueN != 'e' ) {
		   if ( conf.roqueN != 'g' && conf.mat[7][1] == 0 && conf.mat[7][2] == 0 && conf.mat[7][3] == 0 )
		      if ( !caseMenaceePar( MAX, 7, 1, conf ) && !caseMenaceePar( MAX, 7, 2, conf ) && \
			   !caseMenaceePar( MAX, 7, 3, conf ) && !caseMenaceePar( MAX, 7, 4, conf ) )  {
			// Faire un grand roque ...
			copier(&conf, &T[*n]);
			T[*n].mat[7][4] = 0;
			T[*n].mat[7][0] = 0;
			T[*n].mat[7][2] = -'r'; T[*n].xrN = 7; T[*n].yrN = 2;
			T[*n].mat[7][3] = -'t';
			T[*n].roqueN = 'e'; // aucun roque ne sera plus possible à partir de cette config
			(*n)++;
		      }
		   if ( conf.roqueN != 'p' && conf.mat[7][5] == 0 && conf.mat[7][6] == 0 )
		      if ( !caseMenaceePar( MAX, 7, 4, conf ) && !caseMenaceePar( MAX, 7, 5, conf ) && \
			   !caseMenaceePar( MAX, 7, 6, conf ) )  {
			// Faire un petit roque ...
			copier(&conf, &T[*n]);
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
			if ( conf.mat[a][b] >= 0 ) {
			   copier(&conf, &T[*n]);
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

} // deplacementsN


/* Génere dans T tous les coups possibles de la pièce (de couleur B) se trouvant à la pos x,y */
void deplacementsB(struct config conf, int x, int y, struct config T[], int *n )
{
	int i, j, a, b, stop;

	switch(conf.mat[x][y]) {
	// mvmt PION ...
	case 'p' :  
		if ( x <7 && conf.mat[x+1][y] == 0 ) {				// avance d'une case
			copier(&conf, &T[*n]);
			T[*n].mat[x][y] = 0;
			T[*n].mat[x+1][y] = 'p';
			(*n)++;
			if ( x == 6 ) transformPion( conf, x, y, x+1, y, T, n );
		}
		if ( x == 1 && conf.mat[2][y] == 0 && conf.mat[3][y] == 0) {	// avance de 2 cases
			copier(&conf, &T[*n]);
			T[*n].mat[1][y] = 0;
			T[*n].mat[3][y] = 'p';
			(*n)++;
		}
		if ( x < 7 && y > 0 && conf.mat[x+1][y-1] < 0 ) {		// mange à gauche (en montant)
			copier(&conf, &T[*n]);
			T[*n].mat[x][y] = 0;
			T[*n].mat[x+1][y-1] = 'p';
			// cas où le roi adverse est pris...
			if (T[*n].xrN == x+1 && T[*n].yrN == y-1) { 
				T[*n].xrN = -1; T[*n].yrN = -1; 
			}

			(*n)++;
			if ( x == 6 ) transformPion( conf, x, y, x+1, y-1, T, n );
		}
		if ( x < 7 && y < 7 && conf.mat[x+1][y+1] < 0 ) {		// mange à droite (en montant)
			copier(&conf, &T[*n]);
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
			if ( conf.mat[ x+dC[i][0] ] [ y+dC[i][1] ] <= 0 )  {
			   copier(&conf, &T[*n]);
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
			if ( conf.mat[ a ] [ b ] > 0 )  stop = 1;
			else {
			   copier(&conf, &T[*n]);
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
			if ( conf.mat[ a ] [ b ] > 0 )  stop = 1;
			else {
			   copier(&conf, &T[*n]);
			   T[*n].mat[x][y] = 0;
			   if ( T[*n].mat[a][b] < 0 ) stop = 1;
			   T[*n].mat[a][b] = 't';
			   // cas où le roi adverse est pris...
			   if (T[*n].xrN == a && T[*n].yrN == b) { T[*n].xrN = -1; T[*n].yrN = -1; }

			   if ( conf.roqueB != 'e' && conf.roqueB != 'n' ) {
			     if ( x == 0 && y == 0 && conf.roqueB != 'p')
			   	T[*n].roqueB = 'g'; // le grand roque ne sera plus possible
			     else if ( x == 0 && y == 0 )
			   	   T[*n].roqueB = 'n'; // ni le grand roque ni le petit roque ne seront possibles
			     if ( x == 0 && y == 7 && conf.roqueB != 'g' )
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
			if ( conf.mat[ a ] [ b ] > 0 )  stop = 1;
			else {
			   copier(&conf, &T[*n]);
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
		if ( conf.roqueB != 'n' && conf.roqueB != 'e' ) {
		   if ( conf.roqueB != 'g' && conf.mat[0][1] == 0 && conf.mat[0][2] == 0 && conf.mat[0][3] == 0 )
		      if ( !caseMenaceePar( MIN, 0, 1, conf ) && !caseMenaceePar( MIN, 0, 2, conf ) && \
			   !caseMenaceePar( MIN, 0, 3, conf ) && !caseMenaceePar( MIN, 0, 4, conf ) )  {
			// Faire un grand roque ...
			copier(&conf, &T[*n]);
			T[*n].mat[0][4] = 0;
			T[*n].mat[0][0] = 0;
			T[*n].mat[0][2] = 'r'; T[*n].xrB = 0; T[*n].yrB = 2;
			T[*n].mat[0][3] = 't';
			T[*n].roqueB = 'e'; // aucun roque ne sera plus possible à partir de cette config
			(*n)++;
		      }
		   if ( conf.roqueB != 'p' && conf.mat[0][5] == 0 && conf.mat[0][6] == 0 )
		      if ( !caseMenaceePar( MIN, 0, 4, conf ) && !caseMenaceePar( MIN, 0, 5, conf ) && \
			   !caseMenaceePar( MIN, 0, 6, conf ) )  {
			// Faire un petit roque ...
			copier(&conf, &T[*n]);
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
			if ( conf.mat[a][b] <= 0 ) {
			   copier(&conf, &T[*n]);
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

} // deplacementsB


/* Génère les successeurs de la configuration conf dans le tableau T, 
   retourne aussi dans n le nb de configurations filles générées */
void generer_succ( struct config conf, int mode, struct config T[], int *n )
{
	int i, j, k, stop;

	*n = 0;

	if ( mode == MAX ) {		// mode == MAX
	   for (i=0; i<8; i++)
	      for (j=0; j<8; j++)
		 if ( conf.mat[i][j] > 0 )
		    deplacementsB(conf, i, j, T, n );

	   // vérifier si le roi est en echec, auquel cas on ne garde que les succ évitants l'échec...
	   for (k=0; k < *n; k++) {
	      	i = T[k].xrB; j = T[k].yrB;  // pos du roi B dans T[k]
		// vérifier s'il est menacé dans la config T[k] ...
		if ( caseMenaceePar( MIN, i, j, T[k] ) ) {
		    T[k] = T[(*n)-1];	// alors supprimer T[k] de la liste des succ...
		    (*n)--;
		    k--;
		}
	    } // for k
	}

	else { 				// mode == MIN
	   for (i=0; i<8; i++)
	      for (j=0; j<8; j++)
		 if ( conf.mat[i][j] < 0 )
		    deplacementsN(conf, i, j, T, n );

	   // vérifier si le roi est en echec, auquel cas on ne garde que les succ évitants l'échec...
	   for (k=0; k < *n; k++) {
		i = T[k].xrN; j = T[k].yrN;
		// vérifier s'il est menacé dans la config T[k] ...
		if ( caseMenaceePar( MAX, i, j, T[k] ) ) {
		    T[k] = T[(*n)-1];	// alors supprimer T[k] de la liste des succ...
		    (*n)--;
		    k--;
		}
	   } // for k	
	} // if (mode == MAX) ... else ...

} // generer_succ



/***********************************************************************/
/*********** Partie:  AlphaBeta, Initialisation et affichahe ***********/
/***********************************************************************/


/* MinMax avec elagage alpha-beta */
int minmax_ab( struct config conf, int mode, int niv, int alpha, int beta )
{
 	int n, i, score, score2;
 	struct config T[100];

   	if ( feuille(conf, &score) ) 
		return score;

   	if ( niv == 0 ) 
		return estim( conf );

   	if ( mode == MAX ) {

	   generer_succ( conf, MAX, T, &n );

	   score = alpha;
	   for ( i=0; i<n; i++ ) {
   	    	score2 = minmax_ab( T[i], MIN, niv-1, score, beta );
		if (score2 > score) score = score2;
		if (score > beta) {
			// Coupe Beta
   	      		return beta;   
	    	}
	   } 
	}
	else  { // mode == MIN 

	   generer_succ( conf, MIN, T, &n );

	   score = beta;
	   for ( i=0; i<n; i++ ) {
   	    	score2 = minmax_ab( T[i], MAX, niv-1, alpha, score );
		if (score2 < score) score = score2;
		if (score < alpha) {
			// Coupe Alpha
   	      		return alpha;   
	    	}
	   }
	}

        if ( score == +INFINI ) score = +100;
        if ( score == -INFINI ) score = -100;

	return score;

} // minmax_ab


/* Intialise la disposition des pieces dans la configuration initiale conf */
void init( struct config *conf )
{
   	int i, j;

    	for (i=0; i<8; i++)
		for (j=0; j<8; j++)
			conf->mat[i][j] = 0;	// Les cases vides sont initialisées avec 0

	conf->mat[0][0] =  't'; conf->mat[0][1] =  'c'; conf->mat[0][2] = 'f'; conf->mat[0][3] =  'n';
	conf->mat[0][4] =  'r'; conf->mat[0][5] =  'f'; conf->mat[0][6] = 'c'; conf->mat[0][7] =  't';

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

} // init


/* Affiche la configuration conf */
void affich( struct config conf )
{
	int i, j, k;
	for (i=0;  i<8; i++)
		printf("\t  %c", i+'a');
   	printf("\n");

	for (i=0;  i<8; i++)
		printf("\t----- ");
   	printf("\n");

	for(i=8; i>0; i--)  {
		printf("    %d", i);
		for (j=0; j<8; j++)
			if ( conf.mat[i-1][j] < 0 ) printf("\t -%c", -conf.mat[i-1][j]);
			else if ( conf.mat[i-1][j] > 0 ) printf("\t +%c", conf.mat[i-1][j]);
				  else printf("\t  ");
		printf("\n");

		for (k=0;  k<8; k++)
			printf("\t----- ");
   		printf("\n");

	}
	printf("\n");

} // affich





/*******************************************/
/*********** Programme princiapl ***********/
/*******************************************/



int main( int argc, char *argv[] )
{
   char sy, dy, ch[10];
   int sx, dx, n, i, j, score, stop, cout, cout2, legal, hauteur, sauter;
   int cmin, cmax;
   coups_jouees = 0;

   struct config T[100], conf, conf1;

   if ( argc == 1 ) 
	hauteur = 4;  // par défaut on fixe la profondeur d'évaluation à 4
   else
	hauteur = atoi( argv[1] ); // sinon elle est récupérée depuis la ligne de commande

   printf("\n\nProfondeur d'exploration = %d\n\n", hauteur);

   // Initialise la configuration de départ
   init( &conf );
  
   printf("\n\nVous êtes les + (Blancs) et je suis les - (Noirs)\n\n");

   // Boucle principale du dérouleùment d'une partie ...
   stop = 0;
   while ( !stop ) {
	coups_jouees++;
	affich( conf );

	// récupérer le coup du joueur ...
	printf("Coup (sy sx dy dx : depl normal / 0 0 0 0 : pt roq / 1 1 1 1 : grd roq / 2 niv 2 2 : estim profonde ) : ");
	scanf(" %c %d %c %d", &sy, &sx, &dy, &dx );

	copier(&conf, &conf1);

	// Traitement du coup du joueur ...
	sauter = 0;
	if (sy == '0') { // petit roque ...
	   conf1.mat[0][4] = 0;
	   conf1.mat[0][7] = 0;
	   conf1.mat[0][6] = 'r'; conf1.xrB = 0; conf1.yrB = 6;
	   conf1.mat[0][5] = 't';
	   conf1.roqueB = 'e';
	}
	else
	   if (sy == '1') {  // grand roque ...
		conf1.mat[0][4] = 0;
		conf1.mat[0][0] = 0;
		conf1.mat[0][2] = 'r'; conf1.xrB = 0; conf1.yrB = 2;
		conf1.mat[0][3] = 't';
		conf1.roqueB = 'e';
	   }
	   else 
	      if ( sy == '2' ) { // Estimation à la profondeur spécifiée dans sx
		  cout = minmax_ab( conf, MAX, sx, -INFINI, +INFINI );
		  printf("Estimation à %d niveaux = %d\n", sx, cout);
		  sauter = 1;
	      }
	      else
		{  // deplacement normal (les autres coups) ...
		conf1.mat[dx-1][dy-'a'] = conf1.mat[sx-1][sy-'a'];
		conf1.mat[sx-1][sy-'a'] = 0;
		// vérifier possibilité de transformation d'un pion arrivé en fin d'échiquier ...
		if (dx == 8 && conf1.mat[dx-1][dy-'a'] == 'p') {
		   printf("Pion arrivé en ligne 8, transformer en (p/c/f/t/n) : ");
		   scanf(" %s", ch);
		   switch (ch[0]) {
			case 'c' : conf1.mat[dx-1][dy-'a'] = 'c'; break;
			case 'f' : conf1.mat[dx-1][dy-'a'] = 'f'; break;
			case 't' : conf1.mat[dx-1][dy-'a'] = 't'; break;
			case 'p' : conf1.mat[dx-1][dy-'a'] = 'p'; break;
			default  : conf1.mat[dx-1][dy-'a'] = 'n';
		   }
		}
		// vérifier si victoire (le roi N n'existe plus) ...
		if ( conf1.xrN == dx-1 && conf1.yrN == dy-'a' ) {
			conf1.xrN = -1;
			conf1.yrN = -1;
		}
	   }

	if ( sauter == 0) {

	   // vérification de la légalité du coup effectué par le joueur ...
    	   generer_succ(  conf, MAX, T, &n );

	   legal = 0;
	   for (i=0; i<n && !legal; i++)
	    	if ( egal(T[i].mat, conf1.mat) )  legal = 1;

	   if ( legal && !feuille(conf1,&cout) ) {
	    	printf("OK\n\n");
	    	i--;
	    	copier( &T[i], &conf );
	    	affich( conf );

		// L'ordinateur joue son coup ...
	    	printf("A mon tour maintenant ...\n");
	    
	    	generer_succ(  conf, MIN, T, &n );
	
	    	score = +INFINI;
	    	j = -1;

	    	for (i=0; i<n; i++) {
		   cout = minmax_ab( T[i], MAX, hauteur, -INFINI, +INFINI );
		   if ( cout < score ) {  // Choisir le meilleur coup (c-a-d le plus petit score)
		   	score = cout;
		   	j = i;
		   }
	    	}
	    	if ( j != -1 ) { // jouer le coup et aller à la prochaine itération ...
	    	   copier( &T[j], &conf );
		   conf.val = score;
	    	}
	    	else { // S'il n'y a pas de successeur possible, l'ordinateur à perdu
		   printf(" *** J'ai perdu ***\n");
		   stop = 1;
	    	}
	   }
	   else
	    	if ( !legal ) 
	   	   printf("Coup illégal -- réessayer\n");
	    	else
		   stop = 1;
	}  // if (sauter == 0)
   } // while

}
