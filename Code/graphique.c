#include "graphique.h"
#include "arbitre.h"

#include <stdlib.h>
#include <stdio.h>

#include <math.h>


#define HAUTEUR_FENETRE 1280
#define LARGEUR_FENETRE 752

#define TAILLE_TEXTE 30

#define LARGEUR_CASE        84 // 84 px de large pour chaque case
#define HAUTEUR_CASE        260
#define NB_CASES_LARGEUR    12 // 12 cases de largeur


struct Graphique {

    SDL_Surface* ecran;
    SDL_Surface* fond;

    SDL_Surface* de1;
    SDL_Surface* de2;

    TTF_Font* police;

    SDL_Surface* texte_MiseCourante;
    SDL_Surface* texte_ScoreBlanc;
    SDL_Surface* texte_ScoreNoir;
    SDL_Surface* texte_ScoreCible;

    SDL_Surface* texte_LabelMiseCourante;
    SDL_Surface* texte_LabelScoreBlanc;
    SDL_Surface* texte_LabelScoreNoir;
    SDL_Surface* texte_LabelScoreCible;


    Plateau plateau;
    Deplacement deplacement;
    SDL_TimerID timer;
};
typedef struct Graphique Graphique;


static Graphique graphique;

/**
* Actualise tous les �l�ments graphiques, actualise l'�cran puis lib�re les ressources
*/
void rafraichirGraphique()
{

    SDL_Rect position;

    //rafraichissement del'image de fond
    position.x = 0;
    position.y = 0;
    SDL_BlitSurface(graphique.fond, NULL, graphique.ecran, &position);

    //rafraichissement de la mise courante
    position.x = 25;
    position.y = 330;
    SDL_BlitSurface( graphique.texte_MiseCourante, NULL, graphique.ecran, &position );

    //rafraichissement du texte du score blanc
    position.x = 200;
    position.y = 30;
    SDL_BlitSurface( graphique.texte_ScoreBlanc, NULL, graphique.ecran, &position );

    //rafraichissement du texte du score noir
    position.x = 900;
    position.y = 30;
    SDL_BlitSurface( graphique.texte_ScoreNoir, NULL, graphique.ecran, &position );

    //rafraichissement du texte du score cible
    position.x = 500;
    position.y = 30;
    SDL_BlitSurface( graphique.texte_ScoreCible, NULL, graphique.ecran, &position );

    position.x = 25;
    position.y = 300;
    SDL_BlitSurface( graphique.texte_LabelMiseCourante, NULL, graphique.ecran, &position );


    position.x = 200;
    position.y = 30;
    SDL_BlitSurface( graphique.texte_LabelScoreBlanc, NULL, graphique.ecran, &position );


    position.x = 800;
    position.y = 30;
    SDL_BlitSurface( graphique.texte_LabelScoreNoir, NULL, graphique.ecran, &position );


    position.x = 40;
    position.y = 40;
    SDL_BlitSurface( graphique.texte_LabelScoreCible, NULL, graphique.ecran, &position );

    rafraichirDes();

    updatePionsGraphique();

    SDL_Flip( graphique.ecran );

    /*SDL_FreeSurface(graphique.texte_MiseCourante);
    SDL_FreeSurface(graphique.texte_ScoreBlanc);
    SDL_FreeSurface(graphique.texte_ScoreNoir);
    SDL_FreeSurface(graphique.texte_ScoreCible);*/

}

int initialiserFenetre() {

    /* ---------- Initialisation de la fenetre ---------- */


    // Initialisation de la SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0 ) {      // | SDL_INIT_TIMER
        printf( "Impossible de charger SDL : %s\n", SDL_GetError() );
        exit(EXIT_FAILURE);
        //return -1;
    }

    // create a new window
    graphique.ecran = SDL_SetVideoMode( HAUTEUR_FENETRE, LARGEUR_FENETRE, 32, SDL_HWSURFACE | SDL_DOUBLEBUF );
    if ( ! graphique.ecran ) {
        printf("Impossible d'afficher la fenetre SDL a l'ecran : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Titre de la fen�tre
    SDL_WM_SetCaption("Backgammon", NULL);


    // chargement de l'image du plateau
    graphique.fond = SDL_LoadBMP("./Images/plateau.bmp");

    if ( ! graphique.fond ) {
        printf("Impossible de charger l'image de fond du plateau : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
        //return -1;
    }


    // avoir un fonc blanc au cas ou
    SDL_FillRect( graphique.ecran, NULL, SDL_MapRGB( graphique.ecran->format, 255, 255, 255) );

    // on applique l'image de fond sur l'�cran
    SDL_Rect position;
    position.x = 0;
    position.y = 0;
    SDL_BlitSurface( graphique.fond, NULL, graphique.ecran, &position );



    /* ---------- Affichage des textes ---------- */


    // d�marrage de la biblioth�que ttf pour �crire du texte
    if( TTF_Init() == -1 ) {
        fprintf(stderr, "Erreur d'initialisation de TTF_Init : %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
        //return -1;
    }

    // polices d'�criture sur le plateau
    graphique.police = TTF_OpenFont( "Polices/arial.ttf", TAILLE_TEXTE );
    if( ! graphique.police ) {
        fprintf(stderr, "Erreur lors du chargement de la police : %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }


    SDL_Color couleurNoire = {0, 0, 0};

    // ----------------- labels


    graphique.texte_LabelMiseCourante = TTF_RenderText_Blended( graphique.police, "Mise : ", couleurNoire );
    graphique.texte_LabelScoreBlanc = TTF_RenderText_Blended( graphique.police, "Score joueur blanc : ", couleurNoire );
    graphique.texte_LabelScoreNoir = TTF_RenderText_Blended( graphique.police, "Score joueur noir : ", couleurNoire );
    graphique.texte_LabelScoreCible = TTF_RenderText_Blended( graphique.police, "Score cible : ", couleurNoire );


    // ----------------- texte informatifs

    graphique.texte_MiseCourante = TTF_RenderText_Blended( graphique.police, "", couleurNoire );
    graphique.texte_ScoreBlanc = TTF_RenderText_Blended( graphique.police, "", couleurNoire );
    graphique.texte_ScoreNoir = TTF_RenderText_Blended( graphique.police, "", couleurNoire );
    graphique.texte_ScoreCible = TTF_RenderText_Blended( graphique.police, "", couleurNoire );


    //rafraichissement de l'image
    rafraichirGraphique();

    creerPlateau( &(graphique.plateau) );
    initCases( &(graphique.plateau) );

    graphique.deplacement.plateau = graphique.plateau;

    return EXIT_SUCCESS;
}

/**
* Lib�re les ressources du plateau (le fond et l'�cran)
*/
void fermerFenetre() {


    TTF_CloseFont( graphique.police ); //fermeture de la police d'�criture
    TTF_Quit();

    SDL_FreeSurface( graphique.fond );
    SDL_FreeSurface( graphique.ecran );

    SDL_Quit();

    printf("Termin� correctement\n");


}

/**
* Initialise les pions � leur bonne position sur le plateau et les affecte aux bonnes cases
*/
void initialiserPlateauGraphique( SGameState* gameState ) {

    // attention aux cases BAR et OUT � l'initialisation (ils peuvent chang�s)

    int i;
    int j;
    for(i = 0; i < 24; i++)//pour chaque Square
    {
        for(j = 0; j < gameState-> board[i].nbDames; j++) // pour chaque dame dans la case, on cr�� un pion et on l'ajoute � la case
        {
            Pion pion;

            if(gameState -> board[i].owner == WHITE) // si joueur blanc
              pion  = creerPion(graphique.plateau.tabCases[i].posX, graphique.plateau.tabCases[i].posY, "./Images/blanc.bmp");
            else if (gameState ->board[i].owner == BLACK)// si joueur noir
               pion = creerPion(graphique.plateau.tabCases[i].posX, graphique.plateau.tabCases[i].posY, "./Images/noir.bmp");

            SDL_Rect pos = positionnerPion(&graphique.plateau.tabCases[i], i ); // positionnement du pion sur la case
            pion.posPion = pos;

            graphique.plateau.tabCases[i].tabPions[j] = pion; // ajout du pion dans la bonne case
            graphique.plateau.tabCases[i].nbPions ++;
            SDL_SetColorKey(graphique.plateau.tabCases[i].tabPions[j].imagePion, SDL_SRCCOLORKEY, SDL_MapRGB(graphique.plateau.tabCases[i].tabPions[j].imagePion->format, 255, 255, 255));
            SDL_BlitSurface(graphique.plateau.tabCases[i].tabPions[j].imagePion, NULL, graphique.ecran, &graphique.plateau.tabCases[i].tabPions[j].posPion);
        }
    }

    SDL_Flip( graphique.ecran );


    updateScoreJoueurBlanc( gameState -> whiteScore );
    updateScoreJoueurNoir( gameState -> blackScore );
    updateMiseCouranteGraphique( gameState -> stake );
    // updateTourJoueurGraphique( gameState -> turn );
}

/**
* Permet de blitter les pions sur le plateau
*/
void updatePionsGraphique()
{
    int i, j;

    for(i = 0; i < 24; i++)
    {
        Case case_b = graphique.plateau.tabCases[i];
        for(j = 0; j < case_b.nbPions; j++)
        {
            Pion pion = case_b.tabPions[j];
            SDL_BlitSurface(pion.imagePion, NULL, graphique.ecran, &pion.posPion);
        }
    }

    // blit des pions du bar
    for(i = 0; i < 2; i++)
    {
        Case case_b = graphique.plateau.barGraphique[i];

        for(j = 0; j < case_b.nbPions; j++)
        {
            Pion pion = case_b.tabPions[j];
            SDL_BlitSurface(pion.imagePion, NULL, graphique.ecran, &pion.posPion);
        }
    }
}

/**
* Permet de blitter les d�s sur le plateau
*/
void updateDesGraphique( unsigned char dices[2] ) {

    char* pathCompletDe1 = retournerPathDe( dices[0] );
    char* pathCompletDe2 = retournerPathDe( dices[1] );

    graphique.de1 = SDL_LoadBMP(pathCompletDe1);
    graphique.de2 = SDL_LoadBMP(pathCompletDe2);

    rafraichirDes();
    // les positions des d�s peuvent �tre sauvegard�s quelque part pour ne pas � avoir � les recr�er � chaque fois.

}

void rafraichirDes()
{
    SDL_Rect posDe1;
    posDe1.x = 883;
    posDe1.y = 360;

    SDL_Rect posDe2;
    posDe2.x = 945;
    posDe2.y = 360;

    SDL_BlitSurface( graphique.de1, 0, graphique.ecran, &posDe1 );
    SDL_BlitSurface( graphique.de2, 0, graphique.ecran, &posDe2 );

    SDL_Flip( graphique.ecran );

   // SDL_FreeSurface( graphique.de1 );
  //  SDL_FreeSurface( graphique.de2 );
}

void updateTourJoueurGraphique( Player joueur ) {

}

void updateScoreJoueurBlanc( int score ) {

}

void updateScoreJoueurNoir(int score) {

}

void updateScoreCibleGraphique( int scoreCible ) {

}

/**
* Met � jour la valeur de la mise courante et l'affiche sur le plateau
*/
void updateMiseCouranteGraphique( int nouvelleMise ) {

    SDL_Color couleurNoire = {0, 0, 0};

    char chaine[15];
    sprintf( chaine, "%d", nouvelleMise );


    graphique.texte_MiseCourante = TTF_RenderText_Blended( graphique.police, chaine, couleurNoire );

    rafraichirGraphique();

}









// pour libJoueur.h
// retourne la position de la case graphique s�lectionn�e par l'utilisateur (avec la souris)
int selectionnerCaseGraphique() {

    // SDL event clic souris sur une des cases

    return -1;
}


// retourne 1 si le joueur a cliqu� sur "OUI", 0 sinon
int getChoixUtilisateurGraphique() {


    // sdl event clic souris sur bouton "YES/NO"
    SDL_Event event;

    int posX, posY;

    while( 1 ) {

        SDL_WaitEvent(&event);

        switch(event.type) {

            /*
            case SDL_QUIT:
                continuer = 0;
                break;
            */

            case SDL_MOUSEBUTTONUP:

                if (event.button.button == SDL_BUTTON_LEFT ) {

                    posX = event.motion.x;
                    posY = event.motion.y;

                    /*
                    if( ... <= posY && posY <= ... ) {

                        if( ... <= posX && posX <= ... ) {      // bouton "OUI"
                            return 1;
                        }
                        else if( ... <= posX && posX <= ... ) { // bouton "NON"
                            return 0;
                        }
                        // sinon, on est pas dans l'une des zones des boutons

                    }
                    // sinon on est pas dans la zone des boutons
                    */
                }

                break;

            default:
                break;
        }
    }

    return 0;
}


void ouvrirFenetreDoublerMiseGraphique( int miseCourante ) {
    char message[50];
    sprintf( message, "Mise actuelle : %i ", miseCourante );
    creerFenetrePopup( message, "Voulez-vous doubler la mise ?" );
}

void ouvrirFenetreAccepterDoublerMise( int nouvelleMise ) {
    char message[50];
    sprintf( message, "Nouvelle mise : %i ", nouvelleMise );
    creerFenetrePopup( message, "Acceptez-vous la nouvelle mise (le refus entraine un abandon de la partie en cours) ?" );
}


void creerFenetrePopup( char* messageMise, char* messageQuestion ) {

    // chargement de l'image
    SDL_Surface* imagePopup = SDL_LoadBMP("./Images/popup.bmp");
    if ( ! imagePopup ) {
        printf("Impossible de charger l'image de la fenetre popup : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // cr�ation du texte de la fen�tre
    SDL_Color couleurNoire = {0, 0, 0};
    SDL_Surface* texteMessageMise = TTF_RenderText_Blended( graphique.police, messageMise, couleurNoire );
    if ( ! texteMessageMise ) {
        printf("Impossible de creer le texte de la fenetre popup : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Surface* texteMessageQuestion = TTF_RenderText_Blended( graphique.police, messageQuestion, couleurNoire );
    if ( ! texteMessageQuestion ) {
        printf("Impossible de creer le texte de la fenetre popup : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }


    // affichage de la fenetre
    SDL_Rect position;
    position.x = 0;
    position.y = 0;
    SDL_BlitSurface( imagePopup, NULL, graphique.ecran, &position );

    // affichage du texte de la fenetre
    position.x = 0;
    position.y = 0;
    SDL_BlitSurface( texteMessageMise, NULL, graphique.ecran, &position );

    position.x = 0;
    position.y = 0;
    SDL_BlitSurface( texteMessageQuestion, NULL, graphique.ecran, &position );


    // liberation des ressources non utilis�s
    SDL_FreeSurface( texteMessageMise );
    SDL_FreeSurface( texteMessageQuestion );
    SDL_FreeSurface( imagePopup );


    // mise � jour de l'�cran
    SDL_Flip( graphique.ecran );
}










/**
* Met en pause le jeu en attendant un �v�nement
*/
void pause() {

    int continuer = 1;
    SDL_Event event;

    SMove move;

    while (continuer) {

        SDL_WaitEvent(&event);

        switch(event.type) {

            case SDL_QUIT:
                continuer = 0;
                break;

            case SDL_KEYDOWN:

                switch(event.key.keysym.sym) {

                    case SDLK_ESCAPE:
                        printf("leave \n");
                        continuer = 0;
                        break;

                    case SDLK_SPACE:
                        printf("leave \n");
                        continuer = 0;

                        break;
                    case SDLK_g:
                        move.dest_point = 25;
                        move.src_point = 12;
                        deplacerPionGraphique(move, WHITE);
                       // updateOutGraphic(1);
                    break;
                    case SDLK_h:
                        move.dest_point = 25;
                        move.src_point = 13;
                        deplacerPionGraphique(move, BLACK);
                       // updateOutGraphic(0);
                    break;
                    default:

                        break;
                }

            break;

            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    move.dest_point = retournerNumCase(event.motion.x, event.motion.y, graphique.plateau);
                    printf("destination : %i\n", move.dest_point);
                    deplacerPionGraphique(move, 0);

                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    move.src_point = retournerNumCase(event.motion.x, event.motion.y, graphique.plateau);
                    printf("source : %i\n", move.src_point);
                   // surlignerCase(move.src_point);
                }
            break;

            default:
                break;
        }
    }
}



/**
* retourne le chemin de l'image selon la valeur retourn�e par le lancement de d�s
*/
char* retournerPathDe(char dice)
{
    switch(dice)
    {
        case 1:
            return "./Images/Des/de1.bmp";
        case 2:
            return "./Images/Des/de2.bmp";
        case 3:
            return "./Images/Des/de3.bmp";
        case 4:
            return "./Images/Des/de4.bmp";
        case 5:
            return "./Images/Des/de5.bmp";
        case 6:
            return "./Images/Des/de6.bmp";
        default:
            return NULL;
    }
}


/**
*Initialisation de la position des d�s
**/
void positionnerDes(SDL_Rect* posDe1, SDL_Rect* posDe2)
{
    posDe1 -> x = 883;
    posDe1 -> y = 360;

    posDe2 -> x = 945;
    posDe2 -> y = 360;
}

/**
*Cr�� un pion � la position sp�cifi�e et avec l'image sp�cifi�e
**/
Pion creerPion(int posX, int posY, char* image)
{
    Pion pion;
    SDL_Rect pos;
    SDL_Surface* imagePion = SDL_LoadBMP(image);

    pos.x = posX;
    pos.y = posY;

    pion.posPion = pos;
    pion.imagePion = imagePion;

    return pion;
}

void updateOutGraphic(int numJoueur)
{
    int epaisseurPion = 10;

    int nbOut;

    if(numJoueur == BLACK){
        graphique.plateau.outGraphique[BLACK].valeur = graphique.plateau.out[BLACK];
        graphique.plateau.outGraphique[BLACK].valeur ++; // on incr�mente le nombre d epions sortis du joueur
        nbOut = graphique.plateau.outGraphique[BLACK].valeur;

    }
    else if(numJoueur == WHITE){
        graphique.plateau.outGraphique[WHITE].valeur = graphique.plateau.out[WHITE];
        graphique.plateau.outGraphique[WHITE].valeur ++;
        nbOut = graphique.plateau.outGraphique[WHITE].valeur;
    }

    int hauteurARemplir = nbOut*epaisseurPion;
    SDL_Rect posOut;

    SDL_Surface *outJoueur = SDL_CreateRGBSurface(SDL_HWSURFACE, 30, hauteurARemplir, 32, 0, 0, 0, 0);
    if(numJoueur == WHITE){

        SDL_FillRect(outJoueur, NULL, SDL_MapRGB(graphique.ecran->format, 255, 255, 255));
        posOut.x = graphique.plateau.outGraphique[WHITE].posX;
        posOut.y = graphique.plateau.outGraphique[WHITE].posY;

        SDL_BlitSurface(outJoueur, NULL, graphique.fond, &posOut);
       // drawEmptyRect(outJoueur, posOut.x-1, posOut.y-1, 150, 50, 255, 0, 0 );
    }
    else if(numJoueur == BLACK){

        SDL_FillRect(outJoueur, NULL, SDL_MapRGB(graphique.ecran->format, 0, 0, 0));
        posOut.x = graphique.plateau.outGraphique[BLACK].posX;
        posOut.y = graphique.plateau.outGraphique[BLACK].posY;

        SDL_BlitSurface(outJoueur, NULL, graphique.fond, &posOut);

        //drawEmptyRect(outJoueur, posOut.x-1, posOut.y-1, 150, 50, 255, 0, 0 );
    }


    rafraichirGraphique();
}

/**
*   Retourne la bonne position que devra avoir le pion qui se d�place sur cette case
*/
SDL_Rect positionnerPion(Case *case_pos, int numCase){


    int nbPions = case_pos -> nbPions;
    SDL_Rect pos;

    int hauteurPion = 54;


    if(numCase >= 0 && numCase <= 11) // cases du bas
    {
        pos.x = case_pos->posX;
        pos.y = case_pos->posY - nbPions * (hauteurPion - 20) - hauteurPion;

    }
    else // cases du haut
    {
        pos.x = case_pos->posX;
        pos.y = case_pos->posY + nbPions * (hauteurPion - 20);

    }

    return pos;
}

/**
* Fonction � appeler qui permet de d�placer le dernier pion d'une case vers une autre avec le SMove pass� en param�tre
*/
void deplacerPionGraphique(SMove move, Player couleur)
{
    if(move.src_point != move.dest_point && move.src_point != 25)
    {
        if(move.dest_point >=0)
        {

            int src = move.src_point;
            int dest = move.dest_point;

            graphique.deplacement.dest = dest;
            graphique.deplacement.source = src;

            if(src == 0 || dest == 0){ // on teste si le mouvement concerne le bar

                if(dest == 0) // on d�place le pion vers le bar
                {
                    src --;

                    Case case_src = graphique.plateau.tabCases[src];
                    Case case_dest = graphique.plateau.barGraphique[couleur];

                    int nbPionsSrc = case_src.nbPions; //nombre de pions dans la case source
                    int nbPionsDest = case_dest.nbPions; // nombre de pions dans la case destination

                    if(nbPionsSrc > 0)
                    {

                        graphique.plateau.tabCases[src] = case_src;
                        graphique.plateau.barGraphique[couleur] = case_dest;

                        //on effectue le d�placement
                        deplacerPionVers(&case_src.tabPions[nbPionsSrc -1], &case_dest);

                        case_dest.nbPions++;// on augmente le nombre de pions de la case destinataire
                        nbPionsDest = case_dest.nbPions;

                        // affectation du pion � la nouvelle case
                        case_dest.tabPions[nbPionsDest-1] = case_src.tabPions[nbPionsSrc-1];

                        case_src.nbPions --; // on d�cr�mente le nombre de pions de la source
                       // printf("Apr�s d�placement, pos pion : x = %i, y = %i\n", graphique.plateau.tabCases[src].tabPions[nbPionsSrc-1].posPion.x, graphique.plateau.tabCases[src].tabPions[nbPionsSrc-1].posPion.y);
                        nbPionsSrc = case_src.nbPions ;
                        nbPionsDest = case_dest.nbPions;

                        graphique.plateau.tabCases[src] = case_src;
                        graphique.plateau.barGraphique[couleur] = case_dest;

                    }
                }

                else if(src == 0) // on sort le pion du bar
                {
                    dest --;

                    Case case_src = graphique.plateau.barGraphique[couleur];
                    Case case_dest = graphique.plateau.tabCases[dest];

                    int nbPionsSrc = case_src.nbPions; //nombre de pions dans la case source
                    int nbPionsDest = case_dest.nbPions; // nombre de pions dans la case destination

                    if(nbPionsSrc > 0)
                    {

                        graphique.plateau.barGraphique[couleur] = case_src;
                        graphique.plateau.tabCases[dest] = case_dest;

                        //on effectue le d�placement
                        deplacerPionVers(&case_src.tabPions[nbPionsSrc -1], &case_dest);

                        case_dest.nbPions++;// on augmente le nombre de pions de la case destinataire
                        nbPionsDest = case_dest.nbPions;

                        // affectation du pion � la nouvelle case
                        case_dest.tabPions[nbPionsDest-1] = case_src.tabPions[nbPionsSrc-1];

                        case_src.nbPions --; // on d�cr�mente le nombre de pions de la source
                       // printf("Apr�s d�placement, pos pion : x = %i, y = %i\n", graphique.plateau.tabCases[src].tabPions[nbPionsSrc-1].posPion.x, graphique.plateau.tabCases[src].tabPions[nbPionsSrc-1].posPion.y);
                        nbPionsSrc = case_src.nbPions ;
                        nbPionsDest = case_dest.nbPions;

                        graphique.plateau.barGraphique[couleur] = case_src;
                        graphique.plateau.tabCases[dest] = case_dest;

                    }

                }

            }//bar
            else if(dest == 25) // out
            {
                int nbDames = graphique.plateau.tabCases[src -1].nbPions -1;
              //  graphique.plateau.tabCases[src-1].nbPions --;

                graphique.plateau.outGraphique[couleur].caseOut.nbPions ++;
                deplacerPionGraphique(move, couleur);
                graphique.plateau.out[couleur] ++; // on ajoute 1 dans le compteur des pions sortis du joueur
                freePion(&graphique.plateau.tabCases[src -1].tabPions[nbDames]);
                updateOutGraphic(couleur);
            }
            else{

                //on d�cr�mente la source et la destination pour correspondre avec les bonnes valeurs du tableau
                src --;
                dest --;

                Case case_src = graphique.plateau.tabCases[src];
                Case case_dest = graphique.plateau.tabCases[dest];

                printf("Case %i poss�de %i pions\n", src, case_src.nbPions);

                int nbPionsSrc = case_src.nbPions; //nombre de pions dans la case source
                int nbPionsDest = case_dest.nbPions; // nombre de pions dans la case destination

                if(nbPionsSrc > 0)
                {

                    graphique.plateau.tabCases[src] = case_src;
                    graphique.plateau.tabCases[dest] = case_dest;

                    //on effectue le d�placement
                    deplacerPionVers(&case_src.tabPions[nbPionsSrc -1], &case_dest);

                    case_dest.nbPions++;// on augmente le nombre de pions de la case destinataire
                    nbPionsDest = case_dest.nbPions;

                    // affectation du pion � la nouvelle case
                    case_dest.tabPions[nbPionsDest-1] = case_src.tabPions[nbPionsSrc-1];

                    case_src.nbPions --; // on d�cr�mente le nombre de pions de la source
                   // printf("Apr�s d�placement, pos pion : x = %i, y = %i\n", graphique.plateau.tabCases[src].tabPions[nbPionsSrc-1].posPion.x, graphique.plateau.tabCases[src].tabPions[nbPionsSrc-1].posPion.y);
                    nbPionsSrc = case_src.nbPions ;
                    nbPionsDest = case_dest.nbPions;

                    graphique.plateau.tabCases[src] = case_src;
                    graphique.plateau.tabCases[dest] = case_dest;

                    printf("Apr�s deplacement : src contient %i pions, dest contient %i pions\n", case_src.nbPions, case_dest.nbPions);
                    printf("[Case tableau]Apr�s deplacement : src contient %i pions, dest contient %i pions\n", graphique.plateau.tabCases[src].nbPions, graphique.plateau.tabCases[dest].nbPions);

                }

            }

        }

        rafraichirGraphique();
    }
}

/**
*   fonction qui d�place le pion pass� en param�tre
*/
void deplacerPionVers(Pion *pion, Case* case_dest)
{
    int dest = graphique.deplacement.dest;

    SDL_Rect posFinale = positionnerPion(case_dest, dest -1);

    int x = posFinale.x;
    int y = posFinale.y;

   // printf("x : %i, y : %i\n", pion ->posPion.x, pion->posPion.y);

    // calcul de la valeur absolue entre la position du pion et sa position d'arriv�e
    int distanceX = fabs(pion->posPion.x - x);
    int distanceY = fabs(pion->posPion.y - y);

    int incrementPos = 1;

    while(distanceX > 1 || distanceY > 1)
    {
        //test de la position en x du pion par rapport � la position finale
        if(pion->posPion.x < x)
            pion->posPion.x +=incrementPos;
        else if(pion->posPion.x >= x)
            pion->posPion.x  -= incrementPos;

        //test de la position en y du pion par rapport � la position finale
        if(pion-> posPion.y < y)
            pion-> posPion.y +=incrementPos;
        else if(pion-> posPion.y >= y)
            pion-> posPion.y -= incrementPos;

        //M�J de la distance entre le pion et sa position finale
        distanceX = fabs(pion->posPion.x - x);
        distanceY = fabs(pion->posPion.y - y);
        //updatePionsGraphique();

        SDL_Rect pos;
        pos.x = 0;
        pos.y = 0;

        //SDL_BlitSurface(graphique.fond, NULL, graphique.ecran, &pos);
        SDL_BlitSurface(pion->imagePion, NULL, graphique.ecran, &pion ->posPion);
       // updatePionsGraphique();
        SDL_Flip( graphique.ecran );
        //rafraichirGraphique();

    }
}

/**
*Initialise les cases du plateau avec leur bonne position
**/
void initCases(Plateau *plateau)
{
    int i;
    int width = plateau ->largeur;
    int height = plateau -> hauteur;

    for(i = 0; i <= 5; i++) // partie inf�rieure droite
    {
        Case case_b;
        width -= LARGEUR_CASE;
        case_b.posX = width;

        case_b.posY = height;
        case_b.nbPions = 0;
        case_b.largeur = LARGEUR_CASE;
        case_b.hauteur = HAUTEUR_CASE;

        SDL_Surface* surface;
        case_b.imageCase = surface;

        plateau -> tabCases[i] = case_b;

       // printf("%i : x : %i - y : %i\n", i, case_b.posX, case_b.posY);
    }
    width = 760;
    for(i = 18; i <= 23; i++) // partie sup�rieure droite
    {
        Case case_b;

        case_b.posX = width;
        width += LARGEUR_CASE;
        case_b.posY = 90;
        case_b.nbPions = 0;
        case_b.largeur = LARGEUR_CASE;
        case_b.hauteur = HAUTEUR_CASE;

        SDL_Surface* surface;
        case_b.imageCase = surface;

        plateau -> tabCases[i] = case_b;
       // printf("%i : x : %i - y : %i\n", i, case_b.posX, case_b.posY);
    }

    width = 640; // d�but de la partie inferieure droite - le bord noir
    for(i = 6; i <= 11; i++) // partie inf�rieure gauche
    {
        Case case_b;
        width -= LARGEUR_CASE;
        case_b.posX = width;

        case_b.posY = height;
        case_b.nbPions = 0;
        case_b.largeur = LARGEUR_CASE;
        case_b.hauteur = HAUTEUR_CASE;

        SDL_Surface* surface;
        case_b.imageCase = surface;

        plateau -> tabCases[i] = case_b;
       // printf("%i : x : %i - y : %i\n", i, case_b.posX, case_b.posY);
    }
    width = 140;
    for(i = 12; i <= 17; i++) // partie superieure gauche
    {
        Case case_b;

        case_b.posX = width;
        width += LARGEUR_CASE;
        case_b.posY = 90;
        case_b.nbPions = 0;
        case_b.largeur = LARGEUR_CASE;
        case_b.hauteur = HAUTEUR_CASE;

        SDL_Surface* surface;
        case_b.imageCase = surface;

        plateau -> tabCases[i] = case_b;
      //  printf("%i : x : %i - y : %i\n", i, case_b.posX, case_b.posY);
    }

    //initialisation du bar

    Case case_b;

    case_b.hauteur = HAUTEUR_CASE;
    case_b.largeur = LARGEUR_CASE;
    case_b.nbPions = 0;
    case_b.posX = 650;
    case_b.posY = 100;

    plateau ->barGraphique[0] = case_b;

    case_b.posX = 650;
    case_b.posY = 400;

    plateau ->barGraphique[1] = case_b;

    //initialisation des outGraphique
    OutGraphique outG;

    outG.valeur = 0;
    outG.hauteur = 150;
    outG.largeur = 50;
    outG.posX = 40;
    outG.posY = 100;

    case_b.posX = 40;
    case_b.posY = 100;

    outG.caseOut = case_b;

    plateau ->outGraphique[0] = outG;

    case_b.posY = 400;

    outG.caseOut = case_b;
    outG.posY = 400;
    plateau ->outGraphique[1] = outG;

}

/**
*Affecte les bonnes dimensions au plateau
**/
void creerPlateau(Plateau *plateau)
{
    plateau -> hauteur = 725;
    plateau -> largeur = 1270;
}

/**
* Lib�re la surface d'un pion
*/
void freePion(Pion *pion)
{
    SDL_FreeSurface(pion ->imagePion);
}
/**
* Retourne le num�ro de la case dans laquelle on clique
*/
int retournerNumCase(int sourisX, int sourisY, Plateau plateau)
{
    int i;

    Case case_b;

    for(i = 0; i <= 12; i++) //cases du bas
    {
        if( i == 0){
            case_b = plateau.barGraphique[0];
                //bar du haut
            if(sourisX >= case_b.posX && sourisX <= case_b.posX + case_b.largeur && sourisY >= case_b.posY && sourisY <= case_b.posY + case_b.hauteur)
                return 0;
            else{
                case_b = plateau.barGraphique[1];
                if(sourisX >= case_b.posX && sourisX <= case_b.posX + case_b.largeur && sourisY >= case_b.posY && sourisY <= case_b.posY + case_b.hauteur)
                    return 0;
            }
        }
        else{
            case_b = plateau.tabCases[i-1];
            if(sourisX >= case_b.posX &&
            sourisX <= case_b.posX + case_b.largeur && sourisY <= case_b.posY && sourisY >= case_b.posY - case_b.hauteur)
            return i;
        }

    }

    i = 12;
    for(i = 12; i <= 25; i++)//cases du haut
    {
        if( i == 25)
            return 25;
        else{
            case_b = plateau.tabCases[i];
            if(sourisX >= case_b.posX && sourisX <= case_b.posX + case_b.largeur && sourisY <= case_b.posY + case_b.hauteur && sourisY >= case_b.posY)
                return i+1;
        }

    }

    return -1;
}

void surlignerCase(int numCase)
{
    int x = graphique.plateau.tabCases[numCase-1].posX;
    int y = graphique.plateau.tabCases[numCase-1].posY;
    int largeur = LARGEUR_CASE;
    int hauteur = HAUTEUR_CASE;

    drawEmptyRect(graphique.plateau.tabCases[numCase-1].imageCase, x, y, hauteur, largeur, 0,255,0 );

    SDL_Rect posCase;

    posCase.x = graphique.plateau.tabCases[numCase].posX;
    posCase.y = graphique.plateau.tabCases[numCase].posY;


    SDL_BlitSurface(graphique.plateau.tabCases[numCase].imageCase, NULL, graphique.fond, &posCase);
    //rafraichirGraphique();
    //SDL_BlitSurface(graphique.fond, NULL, graphique.ecran, &posEcran);
    SDL_Flip(graphique.ecran);
}


void drawEmptyRect(SDL_Surface* surf,int posX, int posY, int width, int length, int R, int G, int B)
{
	SDL_Rect ligneHaut;
	ligneHaut.x = posX-1;
	ligneHaut.y = posY-1;
	ligneHaut.w = length;
	ligneHaut.h = 1;

	SDL_FillRect(surf, &ligneHaut, SDL_MapRGB(surf->format, R, G, B));

	SDL_Rect ligneDroite;
	ligneDroite.x = posX+length-1;
	ligneDroite.y = posY-1;
	ligneDroite.w = 1;
	ligneDroite.h = width;

	SDL_FillRect(surf, &ligneDroite, SDL_MapRGB(surf->format, R, G, B));

	SDL_Rect ligneGauche;
	ligneGauche.x = posX-1;
	ligneGauche.y = posY-1;
	ligneGauche.w = 1;
	ligneGauche.h = width;

	SDL_FillRect(surf, &ligneGauche, SDL_MapRGB(surf->format, R, G, B));

	SDL_Rect ligneBas;
	ligneBas.x = posX-1;
	ligneBas.y = posY+width-1;
	ligneBas.w = length;
	ligneBas.h = 1;

	SDL_FillRect(surf, &ligneBas, SDL_MapRGB(surf->format, R, G, B));
}
