#include "graphique.h"
#include "arbitre.h"
#include <stdlib.h>
#include <windows.h>
#include <math.h>

// Permet d'afficher la fen�tre de jeu
int afficherJeu()
{

    // Initialisation de la SDL
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "Impossible de d�marrer la fen�tre SDL : %s\n", SDL_GetError() );
        return 1;
    }
    // make sure SDL cleans up before exit
    atexit(SDL_Quit);

    const SDL_VideoInfo *videoInfo = SDL_GetVideoInfo();

    int maxW=videoInfo->current_w;
    int maxH=videoInfo->current_h;
  // create a new window
    SDL_Surface* screen = SDL_SetVideoMode(maxW, maxH, 16, SDL_FULLSCREEN|SDL_DOUBLEBUF);
    SDL_Surface* de1;
    SDL_Surface* de2;

    char* pathCompletDe1 = "./Images/Des/de1.bmp";
    char* pathCompletDe2 = "./Images/Des/de1.bmp";
    SDL_Event event;

    Plateau s_plateau;


    unsigned char dices[2];

    if ( !screen )
    {
        printf("Impossible d'afficher l'�cran : %s\n", SDL_GetError());
        return 1;
    }

    // Titre de la fen�tre
    SDL_WM_SetCaption("Backgammon", NULL);


    // chargement de l'image du plateau
    SDL_Surface* plateau = SDL_LoadBMP("./Images/plateau.bmp");

    if (!plateau)
    {
        printf("Impossible de charger l'image bitmap: %s\n", SDL_GetError());
        return 1;
    }

    // centrer l'image � l'�cran
    SDL_Rect dstrect;
    dstrect.x = (screen->w - plateau->w) / 2;
    dstrect.y = (screen->h - plateau->h) / 2;

    SDL_Rect posDe1; // rect du d�1
    SDL_Rect posDe2; // rect du d�2

    //initialisation des d�s
    de1 = SDL_LoadBMP(pathCompletDe1);
    de2 = SDL_LoadBMP(pathCompletDe2);




    // si impossible de charger les images des d�s
    if (!de1 || !de2)
    {
        printf("Impossible de charger l'image : %s", SDL_GetError());
        return 1;
    }

    positionnerDes(&posDe1, &posDe2);
    creerPlateau(&s_plateau);
    //cr�ation des cases et affectation de leur position
    initCases(&s_plateau);
    //initPions(&s_plateau, etatJeux);
    Pion p = creerPion(130, 100, "./Images/noir.bmp");
    Pion p2 = creerPion(200, 200, "./Images/blanc.bmp");

    s_plateau.tabCases[0].tabPions[0] = p;
    s_plateau.tabCases[0].tabPions[1] = p2;
    //s_plateau.tabCases[0].nbPions = 2;

    int tempsPrecedent = 0;

    /*SDL_Rect pos1 = positionnerPion(&s_plateau, s_plateau.tabCases[0], 0);
   // printf("Position du pion : x : %d - y : %d\n", pos.x, pos.y);
   //initialisation de la position des 2 pions
    deplacerPionVers(&p, 0, tempsPrecedent, s_plateau, &s_plateau.tabCases[0], pos1);

    deplacerPionVers(&p2, 0, tempsPrecedent, s_plateau, &s_plateau.tabCases[0], pos1);*/
    s_plateau.tabCases[0].nbPions = 2;

    int i;
    for(i = 0; i < 24; i++)
    {
        printf("Case n� %i, x = %i - y = %i\n", i+1, s_plateau.tabCases[i].posX, s_plateau.tabCases[i].posY);
    }
    //movePion(move, &s_plateau, tempsPrecedent);
    SDL_SetColorKey(p.imagePion, SDL_SRCCOLORKEY, SDL_MapRGB(p.imagePion->format, 255, 255, 255));
    SDL_SetColorKey(p2.imagePion, SDL_SRCCOLORKEY, SDL_MapRGB(p2.imagePion->format, 255, 255, 255));
    // program main loop
    int done = 0;


    while (done != 1)
    {
        // message processing loop
        SDL_WaitEvent(&event);

            // Gestion des �v�nements
            switch (event.type)
            {
                // sortie si on ferme la fen�tre
            case SDL_QUIT:
                done = 1;
                break;

                // check for keypresses
            case SDL_KEYDOWN:
                {
                    switch(event.key.keysym.sym)
                    {
                        // si appui sur touche echap
                        case SDLK_ESCAPE:
                            done = 1;
                        break;
                        // si appui sur touche espace
                        case SDLK_SPACE:
                                lancerLesDes(dices);

                            // on actualise le chemin de l'image des d�s
                            pathCompletDe1 = retournerPathDe(dices[0]);
                            pathCompletDe2 = retournerPathDe(dices[1]);

                            printf("%i - %i\n", dices[0], dices[1]);
                            de1 = SDL_LoadBMP(pathCompletDe1);
                            de2 = SDL_LoadBMP(pathCompletDe2);

                        break;
                        case SDLK_g:

                        break;
                        default:
                            break;
                    }
                }
                break;

                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        SMove move;
                        move.src_point = 1;
                        move.dest_point = 8;

                        int num = move.dest_point;
                        //printf("Position de la case  avant move: x :%d - y : %d\n", s_plateau.tabCases[move.dest_point-1].posX, s_plateau.tabCases[move.dest_point-1].posY);
                        movePion(move, &s_plateau, tempsPrecedent);
                       // printf("Nombre de dames sur la case : %d\n", s_plateau.tabCases[move.dest_point-1].nbPions);
                        //printf("Position de la case  apr�s move: x :%d - y : %d\n", s_plateau.tabCases[move.dest_point-1].posX, s_plateau.tabCases[move.dest_point-1].posY);
                        //SDL_Rect pos = positionnerPion(&s_plateau, s_plateau.tabCases[num-1], num-1);
                        //printf("Position du pion 2 : x : %d - y : %d\n", pos.x, pos.y);
                       // deplacerPionVers(&p, num, tempsPrecedent, s_plateau, &s_plateau.tabCases[num-1], pos);
                    }

                break;

            default:
                break;

        }

        // D�but de la partie pour redessiner les �l�ments
        // r�initialisation de l'�cran
        SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));


        // on applique l'image de fond
        SDL_BlitSurface(plateau, 0, screen, &dstrect);

        SDL_BlitSurface(p.imagePion, 0, screen, &p.posPion);
        SDL_BlitSurface(p2.imagePion, 0, screen, &p2.posPion);
        //int i, j;

        /*for(i = 0; i < 24; i++)
        {
            for(j = 0; j < s_plateau.tabCases[i].nbPions; j++)
            {
                SDL_BlitSurface(s_plateau.tabCases[i].tabPions[j].imagePion, 0, plateau, &s_plateau.tabCases[i].tabPions[j].posPion );
            }

        }*/

        // on applique l'image des d�s
        SDL_BlitSurface(de1, 0, screen, &posDe1);
        SDL_BlitSurface(de2, 0, screen, &posDe2);

        // On met � jour l'�cran
        SDL_Flip(screen);
    } // end main loop

    // lib�ration des surfaces
    SDL_FreeSurface(plateau);
    SDL_FreeSurface(de1);
    SDL_FreeSurface(de2);
    //freePlateau(&s_plateau);
    SDL_FreeSurface(p.imagePion);
    SDL_FreeSurface(p2.imagePion);
    printf("Termin� correctement\n");
    return 0;
}

// retourne le chemin de l'image selon la valeur retourn�e par le lancement de d�s
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
*Cr�� un pion � la position sp�cifi�d et avec l'image sp�cifi�e
**/
Pion creerPion(int posX, int posY, char* image)
{

    Pion pion;
    SDL_Rect pos;
    SDL_Surface* imagePion = SDL_LoadBMP(image);

    pos.x = posX;
    pos.y = posY;

    pion.hauteurPion = 59;
    pion.largeurPion = 57;
    pion.posPion = pos;
    //printf("x :%i - y : %i\n", pion.posPion.x, pion.posPion.y);
    pion.imagePion = imagePion;

    return pion;
}

/**
*   Effectue le d�placement du pion vers la case sp�cifi�e en param�tre
*/
void deplacerPionVers(Pion *pion, int numCase, int tempsPrecedent, Plateau plateau, Case *case_b, SDL_Rect posPion)
{
    int x = posPion.x;
    int y = posPion.y;

    int distanceX = fabs(pion -> posPion.x - x);
    int distanceY = fabs(pion -> posPion.y - y);
    int tempsActuel;

    tempsActuel = SDL_GetTicks();
    distanceX = fabs(pion -> posPion.x - x);
    distanceY = fabs(pion -> posPion.y - y);
    int incrementPos = 15;

    if (tempsActuel - tempsPrecedent > 30) /* Si 30 ms se sont �coul�es depuis le dernier tour de boucle */
    {
        if(distanceX > 0 && distanceY > 0)
        {
            if(pion -> posPion.x < x)
                pion -> posPion.x +=incrementPos;
            else if(pion -> posPion.x > x)
                pion -> posPion.x -=incrementPos;

            if(pion -> posPion.y < y)
                pion -> posPion.y +=incrementPos;
            else if(pion -> posPion.y > y)
                pion -> posPion.y -=incrementPos;

            tempsPrecedent = tempsActuel;
            pion -> posPion = posPion;
        }
        else
        {
            pion -> posPion = posPion;
        }

        /*if(pion ->posPion.x == posPion.x && pion ->posPion.y == posPion.y)
        {
            case_b ->nbPions ++;
            case_b -> tabPions[case_b -> nbPions -1] = *pion;
        }*/

    }
}
/**
*   Retourne la bonne position que devra avoir le pion qui se d�place sur cette case
*/
SDL_Rect positionnerPion(Plateau *plateau, Case *case_pos, int numCase){

    int nbPions = case_pos -> nbPions;
    SDL_Rect pos;
    printf("[entr�e positionnerPion]Case n� %i, de position x: %i, y : %i, nbPions = : %i\n",numCase, case_pos->posX, case_pos->posY, nbPions);
    int hauteurPion = 54;


    if(numCase >= 0 && numCase <= 11) // cases du bas
    {
        pos.x = case_pos->posX;
        pos.y = case_pos->posY - nbPions * hauteurPion;
        printf("Case n� %i, de position x: %i, y : %i, nbPions = : %i\n",numCase, case_pos->posX, case_pos->posY, nbPions);
    }
    else // cases du haut
    {
        pos.x = case_pos->posX;
        pos.y = case_pos->posY + nbPions * hauteurPion; // 87 = largeur du bandeau au dessus du plateau de jeu
    }

    return pos;
}


void movePion(SMove move, Plateau *plateau, int tempsPrecedent)
{
    int src = move.src_point;
    int dest = move.dest_point;

    if(src == 0 || dest == 0){}//bar
    else if(src == 25 || dest == 25){} // out
    else{

        src --;
        dest --;

        int nbPionsSrc = plateau ->tabCases[src].nbPions ; //index du pion a d�placer
        int nbPionsDest = plateau ->tabCases[dest].nbPions;

        if(nbPionsSrc > 0)
        {
            //transfert du pion d'une case � une autre
            Pion pion = plateau ->tabCases[src].tabPions[nbPionsSrc-1];


            printf("Avant deplacement : src contient %i pions, dest contient %i pions\n", nbPionsSrc, nbPionsDest);
            plateau ->tabCases[dest].nbPions ++; // on augmente le nombre de pions de la case destinataire

            SDL_Rect newPos = positionnerPion(plateau, &plateau->tabCases[dest], dest); //calcul de la position qu'aura le pion
            printf("Destination : %i - Position que le pion est cens� avoir : x : %d, y : %d\n", dest, newPos.x, newPos.y);
            //deplacerPionVers(&pion, dest, tempsPrecedent, *plateau, &plateau->tabCases[dest], newPos );

            // affectation du pion � la nouvelle case
            plateau -> tabCases[dest].tabPions[nbPionsDest-1] = pion;

            plateau ->tabCases[src].nbPions --;

            nbPionsSrc = plateau ->tabCases[src].nbPions ;
            nbPionsDest = plateau ->tabCases[dest].nbPions;

            printf("Apr�s deplacement : src contient %i pions, dest contient %i pions\n", nbPionsSrc, nbPionsDest);
        }

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

        case_b.posX = width;
        width -= 84;
        case_b.posY = height;
        case_b.nbPions = 0;

        plateau -> tabCases[i] = case_b;
       // printf("%i : x : %i - y : %i\n", i, case_b.posX, case_b.posY);
    }
    width = 800;
    for(i = 18; i <= 23; i++) // partie sup�rieure droite
    {
        Case case_b;

        case_b.posX = width;
        width += 84;
        case_b.posY = 100;
        case_b.nbPions = 0;

        plateau -> tabCases[i] = case_b;
       // printf("%i : x : %i - y : %i\n", i, case_b.posX, case_b.posY);
    }

    width = 600; // d�but de la partie inferieure droite - le bord noir
    for(i = 6; i <= 11; i++) // partie inf�rieure gauche
    {
        Case case_b;

        case_b.posX = width;
        width -= 84;
        case_b.posY = height;
        case_b.nbPions = 0;

        plateau -> tabCases[i] = case_b;
       // printf("%i : x : %i - y : %i\n", i, case_b.posX, case_b.posY);
    }
    width = 180;
    for(i = 12; i <= 17; i++) // partie superieure gauche
    {
        Case case_b;

        case_b.posX = width;
        width += 84;
        case_b.posY = 100;
        case_b.nbPions = 0;

        plateau -> tabCases[i] = case_b;
      //  printf("%i : x : %i - y : %i\n", i, case_b.posX, case_b.posY);
    }
}

/**
* initialise les pions aux bonnes cases
**/
void initPions(Plateau *plateau, SGameState gameState)
{
    int i;
    int j;
    for(i = 0; i < 24; i++)//pour chaque Square
    {
        for(j = 0; j < gameState.board[i].nbDames; j++) // pour chaque dame dans la case, on cr�� un pion et on l'ajoute � la case
        {
            Pion pion;
            if(gameState.board[i].owner == WHITE) // si joueur blanc
              pion  = creerPion(0,0, "./Images/blanc.bmp");
            else if (gameState.board[i].owner == BLACK)// si joueur noir
               pion = creerPion(0,0, "./Images/blanc.bmp");

            plateau -> tabCases[i].tabPions[j] = pion; // ajout du pion dans la bonne case
            plateau -> tabCases[i].nbPions ++;
            SDL_Rect pos = positionnerPion(plateau, &plateau -> tabCases[i], i ); // positionnement du pion sur la case
            pion.posPion = pos;
        }
    }
}
void creerPlateau(Plateau *plateau)
{
    plateau -> hauteur = 725;
    plateau -> largeur = 1225;
}

void freePlateau(Plateau *plateau)
{
    int i;
    int j;

    for(i = 0; i < 24; i++)
    {
        for(j = 0; j < plateau ->tabCases[i].nbPions; j++)
            SDL_FreeSurface(plateau ->tabCases[i].tabPions[j].imagePion);
    }
}
