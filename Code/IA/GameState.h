/*
	
	Fichier contenant les fonctions concernant l'état du jeu (GameState, SMove, Square, Player...)

*/

#ifndef _GameState
	#define _GameState


#include "../Commun/backgammon.h"


void getDices( const unsigned char dices[2], unsigned char newDices[4] );

Player getCouleurAdverse( Player maCouleur );

void initialiserMouvement( SMove* mouvement, Player maCouleur, int depart, int nbCases );

// fonctions concernant une case Square
Square getCaseReelle( SGameState* gameState, Player maCouleur, int i );
int caseEstAuJoueur( Square laCase, Player couleurJoueur );
int caseEstSecurisee( Square laCase );
int casePossedeDesPions( Square laCase );

// fonction concernant le plateau GameState
int joueurPossedeDesPionsSurLaBarre( SGameState* gameState, Player maCouleur );
int peutDeplacerUnPion( SGameState* gameState, Player maCouleur, int depart, int nbCases );
int deplacerUnPion( SGameState* gameState, Player maCouleur, SMove mouvement );
void afficherGameState( SGameState gameState );


int getNbCasesSecurisees( SGameState* gameState, Player maCouleur ) ;
double getProbabilitePerdreUnPion( SGameState* gameState, Player maCouleur ) ;
void getEloignementsPointsNonSecurisees( SGameState* gameState, Player maCouleur, int eloignements[25] );
double getProbabiliteeDeFaireUnDesDes( int valeurDes[25] );

int getCoefficientEloignementOut( SGameState* gameState, Player maCouleur );

int randomINT( int min, int max );



// ??????????????????????????????????????????????????
int calculerCout( SGameState* gameState );
//int calculerMeilleurCoup( SGameState* gameState );





int rechercherPositionValeurDe( unsigned char dices[2], char valeurDe );
int peutEncoreJoueur( SGameState* gameState, Player maCouleur, unsigned char dices[2] );




#endif