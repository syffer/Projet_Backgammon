/*

    Gestion de l'arbitre du jeu

    Auteurs :
    - Titouan PASQUET

*/
#include "arbitre.h"

#include "backgammon.h"
#include "joueur.h"
#include "../GUI/graphique.h"

#include <time.h>		// time
#include <stdlib.h> 	// srand
#include <stdio.h>		// printf()
#include <unistd.h>     // usleep()


#define ICI printf("ICIIIIII\n");


/*
* Affiche le contenu du tableau contenant la valeurs des dés.
*
*/
void afficherDes( unsigned char dices[2] ) {
	printf( " des : %i %i \n", dices[0], dices[1] );
    updateDesGraphique(dices);
}


/*
* Lance les dés
*
*/
void lancerLesDes( unsigned char dices[2] ) {
	dices[0] = (unsigned char) randomINT(1,6);
	dices[1] = (unsigned char) randomINT(1,6);
}



/*
* Retourne un nombre généré aléatoirement entre min et max compris.
*
*/
int randomINT( int min, int max ) {

	// on vérifie si le générateur aléatoire a été initialisé
	static int generateurInitialise = 0;
    if( ! generateurInitialise ) {		// si ce n'est pas le cas, on l'initialise
        generateurInitialise = 1;
        srand( time(NULL) );
    }

	return ( rand() % (max-min+1) ) + min;
}



/**
* board : tableau des cases
* numeroCase : la case à mettre
* couleurJoueur : la couleur du joueur qui la possède
* nbDames : le nombre de jetons à poser
*/
int remplirCase( Square board[], int laCase, int couleurJoueur, int nbDames ) {
	if( laCase < 0 || laCase > 23 ){
    perror("laCase pointe vers rien du tout !");
	exit(EXIT_FAILURE);
	}

	board[laCase].owner = couleurJoueur;
	board[laCase].nbDames = nbDames;

	return 0;
}





SGameState initialiserEtatJeux() {

	SGameState etatJeux;

	// pions sur la barre centrales (ceux pas encore en jeu)
	etatJeux.bar[ BLACK ] = 0;
	etatJeux.bar[ WHITE ] = 0;

	// pions déjà sorties
	etatJeux.out[ BLACK ] = 0;
	etatJeux.out[ WHITE ] = 0;

	// score des joueurs
	etatJeux.whiteScore = 0;
	etatJeux.blackScore = 0;

	etatJeux.turn = WHITE;

	//mise courante (du videau)
	etatJeux.stake = 1;

	// on initialise tout le plateau à "rien/personne/NOBODY"
	int i;
	for( i = 0; i < 24; i++ ) {
		remplirCase( etatJeux.board, i, NOBODY, 0 );
	}

	return etatJeux;
}



void initialiserPlateau( Square board[24] ) {
    int i;
    for( i = 0; i < 24; i++ ) {
		remplirCase( board, i, NOBODY, 0 );
	}
	remplirCase( board, 0, WHITE, 2 );
	remplirCase( board, 5, BLACK, 5 );
	remplirCase( board, 7, BLACK, 3 );
	remplirCase( board, 11, WHITE, 5 );
	remplirCase( board, 12, BLACK, 5 );
	remplirCase( board, 16, WHITE, 3 );
	remplirCase( board, 18, WHITE, 5 );
	remplirCase( board, 23, BLACK, 2 );
}

const SGameState const copierEtatJeux( SGameState* etatJeux ) {
	SGameState copie;
	copie.bar[0] = etatJeux->bar[0];
	copie.bar[1] = etatJeux->bar[1];
	copie.out[0] = etatJeux->out[0];
	copie.out[1] = etatJeux->out[1];
	//copie de la board
	int i;
	for(i=0;i<24;i++){
        copie.board[i].nbDames = etatJeux->board[i].nbDames;
        copie.board[i].owner = etatJeux->board[i].owner;
	}
	copie.turn = etatJeux->turn;
	copie.stake = etatJeux->stake;

	copie.whiteScore = etatJeux->whiteScore;
	copie.blackScore = etatJeux->blackScore;
	//pour l'instant on copie que ça
	return copie;
}


void jouerPartie( int score, Joueur joueurs[2] ) {

    int estBot[2];
    estBot[0] = joueurs[0].estUnBot;
    estBot[1] = joueurs[1].estUnBot;

	char nomJoueurBlanc[50];
	char nomJoueurNoir[50];
	Joueur joueurBlanc = joueurs[WHITE];
	Joueur joueurNoir = joueurs[BLACK];
	joueurBlanc.InitLibrary(nomJoueurBlanc);
	joueurNoir.InitLibrary(nomJoueurNoir);

    // variables de jeux
    SMove moves[4];
    unsigned int nbMoves = 0;
    unsigned char dices[2];
    //le videau
    int videau = -1;
    int winner; //vainqueur de la partie

    printf("initialisation de l'etat Jeux\n");
    SGameState etatJeux = initialiserEtatJeux();
    SGameState etatCopie;

	unsigned int triesb, triesw;
	//unsigned int couleur[2]; //les couleurs du j1 puis du j2
	SMove bonsCoups[4];
	SMove move; //move utilisé une fois...

    printf("envoi du startMatch aux joueurs\n");
    joueurBlanc.StartMatch(score);
    joueurNoir.StartMatch(score);


    initialiserFenetre();




    updateScoreCibleGraphique(score);

    int continuerPartie = 1; //boolean a 0 lorsque la partie est terminée


    int i = 0;
    while(etatJeux.whiteScore < score && etatJeux.blackScore < score) {

        printf("(arbitre) -> partie %i \n", i+1 );
        //SDL --> afficher début partie i
        initialiserPlateau( etatJeux.board );

        //SDL --> Mettre les jetons sur le plateau
        triesb = 3;
        triesw = 3;
        etatJeux.bar[BLACK] = 0;
        etatJeux.bar[WHITE] = 0;
        etatJeux.out[WHITE] = 0;
        etatJeux.out[BLACK] = 0;
        etatJeux.stake = 1;

        joueurBlanc.StartGame(WHITE);
        joueurNoir.StartGame(BLACK);
        continuerPartie = 1;


        initialiserPlateauGraphique(&etatJeux);

        // on lance les dés jusqu'a ce que l'on obtienne deux nombre différents
        do
        {
            lancerLesDes(dices);
        } while( dices[0] == dices[1] );
        afficherDes(dices);

        while(continuerPartie){
            printf("\nC'est au tour du joueur de couleur %d\n\n",etatJeux.turn);
            etatCopie = copierEtatJeux(&etatJeux);

            printf("ETAT DU PLATEAU :\n\n");
            afficherEtatJeu(etatJeux);
            printf("\n\n");


            //On lui donne ici normalement l'état copié
            if(etatJeux.turn == WHITE) joueurBlanc.PlayTurn( &etatCopie, dices, moves, &nbMoves, triesw );
            else joueurNoir.PlayTurn( &etatCopie, dices, moves, &nbMoves, triesb );
            //printf("Le joueur a donné son ordre --> vérification\n");
            //on vérifie que ce que veut faire le joueur est correct
            if(!verifierCoup(&etatJeux,dices,moves,nbMoves,bonsCoups)){
            printf("coup pas maximisé ou impossible\n");
                //coup foireux !
                if(etatJeux.turn == WHITE){
                     triesw--;
                }
                else{
                     triesb--;
                }

                //si le joueur est un humain, on remet l'état du jeu à l'état ou il était avant son tour
                if(!estBot[etatJeux.turn])
                    initialiserPlateauGraphique(&etatJeux);

            //ici, on a un joeur qui a fait une faute, la valeur de etatValide n'est donc pas modifiée, il recommence à jouer (sauf s'il a déjà perdu)
            }else{
            printf("coup valide !\n");

                if(estBot[etatJeux.turn]){
                    //le joueur n'est pas humain
                    //coup valide, on modifie la gameState, on appele les fonctions graphiques et on passe le tour
                    for(i=0;i<nbMoves;i++){
                        if(etatJeux.board[bonsCoups[i].dest_point-1].owner == !etatJeux.turn){ //on vérifie ici pour afficher le déplacement d'un jeton mangé
                            move.src_point = bonsCoups[i].dest_point;
                            move.dest_point = 0;
                            deplacerPionGraphique( move, !etatJeux.turn );
                        }
                        jouerCoup(&etatJeux,bonsCoups[i],etatJeux.turn);
                        deplacerPionGraphique( bonsCoups[i], etatJeux.turn );

                    }

                }else{
                    etatJeux = etatCopie;
                }
            }//fin tour



            if(finPartie(&etatJeux,triesw,triesb,&winner)){
                continuerPartie = 0;
            }else{
                //on change le tour du joueur
                etatJeux.turn = !etatJeux.turn;


                updateTourJoueurGraphique( etatJeux.turn );

                //videau
                if(videau != etatJeux.turn){
                    printf("le joueur n'a pas le videau et la mise courante est de %d\n",etatJeux.stake);
                    if(etatJeux.turn == WHITE){
                        etatCopie = copierEtatJeux(&etatJeux);
                        if(joueurBlanc.DoubleStack(&etatCopie)){
                            printf("le joueur blanc propose de doubler la mise\n");
                            etatCopie = copierEtatJeux(&etatJeux);
                            if(joueurNoir.TakeDouble(&etatCopie)){
                                videau = WHITE;
                                etatJeux.stake *= 2;
                                updateMiseCouranteGraphique( etatJeux.stake );
                                printf("le joueur Noir accepte de doubler, il donne le videau et la mise est alors de %d\n",etatJeux.stake);
                            }else{
                                printf("le joueur noir refuse de doubler la mise --> abandon\n");
                                continuerPartie = 0;
                                winner = WHITE;
                            }
                        }else{
                            printf("le joueur blanc ne veut pas doubler la mise\n");
                        }
                    }else{
                        etatCopie = copierEtatJeux(&etatJeux);
                        if(joueurNoir.DoubleStack(&etatCopie)){
                            printf("le joueur noir propose de doubler la mise\n");
                            etatCopie = copierEtatJeux(&etatJeux);
                            if(joueurBlanc.TakeDouble(&etatCopie)){
                                videau = BLACK;
                                etatJeux.stake *= 2;
                                updateMiseCouranteGraphique( etatJeux.stake );
                                printf("le joueur blanc accepte de doubler, il donne le videau et la mise est alors de %d\n",etatJeux.stake);
                            }else{
                                printf("le joueur blanc refuse de doubler la mise --> abandon\n");
                                continuerPartie = 0;
                                winner = BLACK;
                            }
                        }else{
                            printf("le joueur noir ne veut pas doubler la mise\n");
                        }
                    }
                }

                //on lance les dés
                lancerLesDes(dices);
                afficherDes(dices);
            }

        }
        printf("(arbitre)-> fin de la partie %i \n", i+1 );
        if(winner == WHITE){
            printf("joueur blanc marque %d points\n",etatJeux.stake);
            etatJeux.whiteScore+= etatJeux.stake;
            updateScoreJoueurBlanc(etatJeux.whiteScore);
        }else{
            printf("joueur noir marque %d points\n",etatJeux.stake);
            etatJeux.blackScore+= etatJeux.stake;
            updateScoreJoueurNoir(etatJeux.blackScore);
        }
        printf("//////////////\n");
        printf("SCORE ACTUEL : \n");
        printf("BLANC : %d\n",etatJeux.whiteScore);
        printf("NOIR  : %d\n",etatJeux.blackScore);
        printf("score a atteindre : %d",score);
        printf("//////////////\n\n");

        joueurBlanc.EndGame();
        joueurNoir.EndGame();


    }

    printf("(arbitre) fin du match\n");
    printf("//////////////\n");
    printf("SCORE FINAL : \n");
    printf("BLANC : %d\n",etatJeux.whiteScore);
    printf("NOIR  : %d\n",etatJeux.blackScore);
    printf("//////////////\n\n");

    if(etatJeux.whiteScore > etatJeux.blackScore){
        printf("Blanc l'emporte avec %d points\n",etatJeux.whiteScore);
    }else{
        printf("Noir l'emporte avec %d points\n",etatJeux.blackScore);
    }
    joueurBlanc.EndMatch();
    joueurNoir.EndMatch();
    fermerFenetre();
}

int finPartie(SGameState* etatJeux, int triesw, int triesb , int* winner){
    //perte due aux fautes de jeu
    if( ! triesw ) {
        printf("joueur blanc perd par faute de jeu !");
        *winner = BLACK;
        return 1;
    }
    if( ! triesb ) {
        printf("joueur noir perd par faute de jeu !");
        *winner = WHITE;
        return 1;
    }
    if( etatJeux->out[0] == 15 ) {
        printf("joueur noir à gagné !");
        *winner = BLACK;
        return 1;
    }
    if( etatJeux->out[1] == 15 ) {
        printf("joueur blanc à gagné !");
        *winner = WHITE;
        return 1;
    }
    return 0;
}

//la couleur est dans etatJeux.turn
int verifierCoup(SGameState* etatJeux ,unsigned char dices[2], SMove moves[4], int nbMoves, SMove bonsCoups[4]){
    int x=calculerMaxCoup(etatJeux,dices, etatJeux->turn);
    if(x != nbMoves){
        printf("--------------Le joueur n'a pas maximisé le nombre de coups !!!\n");
        printf("Coups du joueur : %d\n",nbMoves);
        printf("Coups possibles : %d\n",x);
        return 0;
    }
    //printf("VERIF : nbCoups resonnable\n");
    SGameState copie = copierEtatJeux(etatJeux);
    int modificationEffectuee, i, nbCoupsRestants = nbMoves;
    int movesEffectues[4] = {0,0,0,0};

    while(nbCoupsRestants != 0){
        modificationEffectuee = 0;
        for(i=0;i<nbMoves;i++){
            //si le move n'a pas déjà été fait
            if(movesEffectues[i] == 0){
                //on regarde si on peut jouer le coup
                if(coupPossible(copie.board,moves[i],copie.turn)){
                    //printf("VERIF : le coup de %d vers %d est possible\n",moves[i].src_point,moves[i].dest_point);
                    //on le joue, on le définit comme joué, on réduit le nombre de coups restants
                    jouerCoup(&copie,moves[i],copie.turn);
                    movesEffectues[i] = 1;

                    //on ajoute le coups aux bons coups, ce qui permet de gagner du temps après lorsqu'on les fera un par un dans l'ordre
                    bonsCoups[nbMoves-nbCoupsRestants] = moves[i];

                    nbCoupsRestants--;
                    //on a fait une modif
                    modificationEffectuee = 1;
                }
            }
        }
        if(modificationEffectuee == 0) return 0;
    }
    return 1;
}


//calculer le maximum de coups possibles par un joueur
int calculerMaxCoup(SGameState* etatJeux, unsigned char dices[2], int couleur){
    //combien on peut faire de coups avec les dés qu'on à
    unsigned int coups[4];
    int nbCoups = 0, coupsJoues = 0;
    transformerDesEnCoups(dices, coups, &nbCoups);


    //copie de gameState
    SGameState copieGame = copierEtatJeux(etatJeux);

    //on enlève les coups obligatoires dus au bar
    coupsJoues = jouerBar(&copieGame, coups, &nbCoups, couleur);
    //etatJeux->bar[couleur] -= coupsJoues;
    if(copieGame.bar[couleur] > 0){
        return coupsJoues;
    }
    if(nbCoups == 0){
        return coupsJoues;
    }else if(nbCoups == 1){
        return coupsJoues + algoCoupPareil(&copieGame,coups[0],1,couleur);
    }else{
        //il y a au moins deux coups.
        //si les dés sont les même, on prend le premier algo, sinon le deuxième
        if(coups[0] == coups[1]){
            return coupsJoues + algoCoupPareil(&copieGame,*coups,nbCoups, couleur);
        }else{
            if(nbCoups != 2){
                perror("PAS NORMAL : erreur pas possible à ce stade, on ne doit pas pouvoir faire l'algo de dés différents si il y en a plus de 2");
                exit(1);
            }
            return coupsJoues + algoCoupDifferent(&copieGame,coups, couleur);
        }
    }
}

///fonctions pour vérifier nombre de coups
void transformerDesEnCoups(unsigned char dices[2], unsigned int coups[4], int* nbCoups){
    if(dices[0] == dices[1]){
        unsigned int num = (unsigned int) dices[0];
        coups[0] = num;
        coups[1] = num;
        coups[2] = num;
        coups[3] = num;
        *nbCoups = 4;
    }else{
        unsigned int num1, num2;
        num1 = (unsigned int) dices[0];
        num2 = (unsigned int) dices[1];
        coups[0] = num1;
        coups[1] = num2;
        *nbCoups = 2;
    }
}

//retourne le nombre de coups joués
int jouerBar(SGameState* etatJeux, unsigned int coups[4],int* nbCoups, int couleur){
    if(etatJeux->bar[couleur] == 0){
        return 0;
    }
    //cas particulier à traiter : un jeton en bar, et 2 dés différents --> il peut perdre un coup si il joue mal
    //on traite donc le cas ici, fais chier...
    if(etatJeux->bar[couleur] == 1 && coups[0] != coups[1]){
        //si on peut pas faire le coup avec le premier dé
        SMove move0 = faireMove(0,coups[0],couleur);
        SMove move1 = faireMove(0,coups[1],couleur);
        int coup0 = coupPossible(etatJeux->board,move0,couleur);
        int coup1 = coupPossible(etatJeux->board,move1,couleur);

        if(!coup0 && !coup1){
            //les deux coups ne peuvent être joués
            return 0;
        }
        else if(!coup0){
            jouerCoup(etatJeux,move1,couleur);
            *coups = coups[0];
            *nbCoups = 1;
            return 1;
        }else if(!coup1){
            jouerCoup(etatJeux,move0,couleur);
            *coups = coups[1];
            *nbCoups = 1;
            return 1; //on réapplique l'algo avec l'autre dé
        }else{
            //les deux dés peuvent être joués, fais chier...
            SGameState copie = copierEtatJeux(etatJeux);

            //on teste avec le premier dé
            jouerCoup(&copie,move0,couleur);
            if(algoCoupPareil(&copie,coups[1],1,couleur) == 1){
                jouerCoup(etatJeux,move0,couleur);
                *coups = coups[1];
                *nbCoups = 1;
                return 1;
            }
            //si on a pas réussi à obtenir 2 coup en jouant le 1er dé en premier, on lance le deuxième et on verra bien ce qui se passe
            jouerCoup(etatJeux,move1,couleur);
            *coups = coups[0];
            *nbCoups = 1;
            return 1;
            }
        }
    //la on est dans un cas normal, c'est bon, ouf !
    int coupsJoues = 0;
    int coupATester = *nbCoups-1;
    SMove move;
    unsigned int coupsRestants[4] = {0,0,0,0}, courant = 0; //servent à recrééer le tableau de coups
    while(etatJeux->bar[couleur] != 0 && coupATester != -1){
        move = faireMove(0,coups[coupATester],couleur);
        if(coupPossible(etatJeux->board,move,couleur)){
            jouerCoup(etatJeux,move,couleur);
            *nbCoups = *nbCoups-1;
            coupsJoues++;
        }else{
            coupsRestants[courant] = coups[coupATester];
            courant++;
        }
        coupATester--;
    }
    coups = coupsRestants; //coups deviennent les coups restants --> A TESTER !!! j'ai jamais fait ça !
    return coupsJoues;
}

//retourne le nombre de coups joués
int algoCoupPareil(SGameState* etatJeux, unsigned int coup, int nbCoups, int couleur){
    int coupJoues = 0;
    SMove move;
    int courant; //case courante à tester
    if(couleur == WHITE) courant = 1;
    else courant = 24;
    while(nbCoups != 0 && courant != 0 && courant != 25){
        if(etatJeux->board[courant-1].owner == couleur){
            move = faireMove(courant,coup,couleur);
            while(nbCoups != 0 && coupPossible(etatJeux->board,move,couleur) && etatJeux->board[courant-1].nbDames != 0){
                jouerCoup(etatJeux,move,couleur);
                coupJoues++;
                nbCoups--;
            }
        }
        if(couleur == WHITE) courant++;
        else courant--;
    }
    return coupJoues;
}


int algoCoupDifferent(SGameState* etatJeux, unsigned int coup[2], int couleur){
    //si on en trouve un qui peut se faire que avec un dé, on le fait puis on lance l'autre algo avec le dé restant.
    //on peut regarder aussi si un pion peut être joué avec les deux dés et qu'il y a au moins deux pions
    SMove move1, move2, move3;
    SGameState etatCopie;
    int courant; //case courante à tester
    if(couleur == WHITE) courant = 1;
    else courant = 24;
    int iJetonChiant = 0, jetonSeul = 0,coup1,coup2;
    while(courant != 0 && courant != 25){
        //printf("DIFF : test de la case %d\n",courant);
        if(etatJeux->board[courant-1].owner == couleur){
            move1 = faireMove(courant,coup[0],couleur);
            move2 = faireMove(courant,coup[1],couleur);
            move3 = faireMove(courant,coup[0]+coup[1],couleur);
            coup1 = coupPossible(etatJeux->board,move1,couleur);
            coup2 = coupPossible(etatJeux->board,move2,couleur);
            if(coup1 && coup2){
                //si on est arrivé à la fin...
                if(etatJeux->out[couleur] == 14 && (move1.dest_point == 25 && move2.dest_point == 25) && peutSortir(etatJeux->board,couleur)) return 1;
                if((move1.dest_point == 25 || move2.dest_point == 25) && peutSortir(etatJeux->board,couleur)) return 2; //ICI, celuyi qui est à 25 doit pouvoir sortir !!!!
                if(((move1.dest_point == 25 || move2.dest_point == 25) && !peutSortir(etatJeux->board,couleur))){
                    //cas particulier, le joueur peut pas sortir, après un mouvement il peut, et il sort avec le deuxième
                    if(move1.dest_point != 25){
                        jouerCoup(etatJeux,move2,couleur);
                        if(coupPossible(etatJeux->board,move1,couleur)) return 2;
                        else return 1;
                    }else{
                        jouerCoup(etatJeux,move1,couleur);
                        if(coupPossible(etatJeux->board,move2,couleur)) return 2;
                        else return 1;
                    }
                }
                //les deux coups sont possibles
                //s'il y a au moins 2 jetons, c'est bon
                //si ya qu'un jeton mais qu'on peut faire la somme des dés, c'est bon aussi
                if(etatJeux->board[courant-1].nbDames >= 2 || coupPossible(etatJeux->board,move3,couleur)) return 2;

                //ensuite on joue un coup, on regarde si on peut obtenir 2, sinon on l'enlève, puis on fait pareil avec le deuxième
                etatCopie = copierEtatJeux(etatJeux);
                jouerCoup(&etatCopie,move1,couleur);
                if(algoCoupPareil(&etatCopie,coup[1],1,couleur) == 1){
                    return 2;
                }
                etatCopie = copierEtatJeux(etatJeux);
                jouerCoup(&etatCopie,move2,couleur);
                if(algoCoupPareil(&etatCopie,coup[0],1,couleur) == 1){
                    return 2;
                }

                //sinon on ajoute ce pion au tableau des "jetons chiants", mais on a pas besoin de savoir sa valeur, juste le nombre de jeton Chiants
                //si on ajoute le deuxième jeton chiant, c'est bon car on a deux jetons différents où on peut faire les deux combinaisons
                if(iJetonChiant == 1) return 2;
                iJetonChiant++;
            }else if(coup1){
                etatCopie = copierEtatJeux(etatJeux);
                jouerCoup(&etatCopie,move1,couleur);
                if(algoCoupPareil(&etatCopie,coup[1],1,couleur) == 1){
                    return 2;
                }else{
                    jetonSeul = 1;
                }
            }else if(coup2){
                etatCopie = copierEtatJeux(etatJeux);
                jouerCoup(&etatCopie,move2,couleur);
                if(algoCoupPareil(&etatCopie,coup[0],1,couleur) == 1){
                    return 2;
                }else{
                    jetonSeul = 1;
                }
            }//sinon on peut rien faire avec ce jeton
        }
        //passer au jeton suivant
        if(couleur == WHITE) courant++;
        else courant--;
    }
    if(jetonSeul == 0 && iJetonChiant == 0)
        return 0;
    if(iJetonChiant > 1)
        return 2;
    return 1; //si y'en a 1, alors on peut jouer qu'un coup, sinon on peut en jouer aucun
}


int coupPossible(Square board[], SMove move, int couleur){
    if(move.dest_point == 25){
        if(peutSortir(board,couleur)) return 1;
        else return 0;
    }
    //sinon faut que ça soit pas une case adverse, ou alors y a qu'un jeton
    else return (board[move.dest_point-1].owner != (!couleur) || board[move.dest_point-1].nbDames == 1);
}

void jouerCoup(SGameState* etatJeux, SMove move, int couleur){
    if(move.src_point == 0){
         etatJeux->bar[couleur]--;
         //si c'est une case adverse, on mange le pion
         if(etatJeux->board[move.dest_point-1].owner == (!couleur)){
            etatJeux->bar[(!couleur)]++;
            etatJeux->board[move.dest_point-1].nbDames = 0;
         }
         etatJeux->board[move.dest_point-1].nbDames++;
         etatJeux->board[move.dest_point-1].owner = couleur;
    }
    else if(move.dest_point == 25){
         etatJeux->out[couleur]++;
         etatJeux->board[move.src_point-1].nbDames--;
         if(etatJeux->board[move.src_point-1].nbDames == 0) etatJeux->board[move.src_point-1].owner = NOBODY;
    }else{
        etatJeux->board[move.src_point-1].nbDames--;
        if(etatJeux->board[move.src_point-1].nbDames == 0) etatJeux->board[move.src_point-1].owner = NOBODY;
        //si c'est une case adverse, on mange le pion
         if(etatJeux->board[move.dest_point-1].owner == (!couleur)){
            etatJeux->bar[(!couleur)]++;
            etatJeux->board[move.dest_point-1].nbDames = 0;
         }
         etatJeux->board[move.dest_point-1].owner = couleur;
         etatJeux->board[move.dest_point-1].nbDames++;
    }
}

SMove faireMove(int src, int numDe, int couleur){
    int dest;
    if(couleur == WHITE){
        dest = src + numDe;
        if(dest > 25) dest = 25;
    }else{
        if(src == 0){
            dest = 25 - numDe;
        }else{
            dest = src - numDe;
            if(dest < 1) dest = 25;
        }
    }
    SMove move = {src,dest};
    return move;
}



void afficherEtatJeu( SGameState gameState ) {

    char* nom;

    printf("\n");

    printf( "   out BLACK nbDames %i \n", gameState.out[BLACK] );
    printf( "   bar WHITE nbDames %i \n", gameState.bar[WHITE] );

    int i;
    for( i = 0; i < 24; i++ ) {

        if( gameState.board[i].owner == BLACK ) nom = "BLACK";
        else if( gameState.board[i].owner == WHITE  ) nom = "WHITE";
        else nom = "NOBODY";

        printf( "   case %i nbDames %i %s \n", i+1, gameState.board[i].nbDames, nom );
    }

    printf( "   bar BLACK nbDames %i \n", gameState.bar[BLACK] );
    printf( "   out WHITE nbDames %i \n", gameState.out[WHITE] );

    printf("\n");
}


int peutSortir(Square board[], int couleur){
    int i;
    if(couleur == WHITE){
        //parcours de toutes les cases de 0 à 17, dès qu'on trouve une case blanche, on retourne 0
        for(i=0;i<=17;i++){
            if(board[i].owner == WHITE) return 0;
        }
        return 1;
    }else{
        for(i=6;i<=23;i++){
            if(board[i].owner == BLACK){
                 return 0;
            }
        }
        return 1;
    }
}
