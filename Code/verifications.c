


#include "verifications.h"
#include <stdio.h>		// fopen(), FILE
#include <string.h>		// strlen(), strcat(), strcmp()

// #include "backgammon.h"
// #include <stdlib.h>		// malloc



/*
* Convertie une chaine de caractere en nombre entier positif,
* et place la valeur de ce nombre à l'adresse pointé par le pointeur *nombre
*
* @param const char* string 
*	la chaine a convertir en nombre entier positif
*
* @param int* nombre
*	pointeur d'entier vers l'adresse mémoire ou sera stocké le nombre correspondant en mémoire
*
* @return int
*	retourne 0 si la conversion s'est bien passé, -1 sinon.
*/
int stringToPositiveInteger( const char* string, int* nombre ) {

	*nombre = 0;

	int i;
	char c;
	for( i = 0; i < strlen(string); i++ ) {
		c = string[i];

		if( c >= '0' && c <= '9' ) {
			*nombre = *nombre * 10 + (c - '0');
		}
		else return -1;
	}
	return 0;
}





/*
* Copie le contenu d'un fichier source vers un fichier destination (ecrase le contenu existant)
*/
int copierFichier( char* source, char* destination ) {

	// ouverture du fichier à copier
	FILE* p_fichierAcopier = fopen( source, "r" );
	if( ! p_fichierAcopier ) {
		return -1;
	}

	// ouverture du fichier de destination
	FILE* p_nouveauFichier = fopen( destination, "w" );
	if( ! p_nouveauFichier ) {
		fclose( p_fichierAcopier );
		return -1;
	}

	// on copie le contenu
	int c;
	while( (c=fgetc(p_fichierAcopier) ) != EOF && !feof(p_nouveauFichier) ) fputc( c, p_nouveauFichier );

	// on ferme les fichiers utilisés
	fclose( p_fichierAcopier );
	fclose( p_nouveauFichier );

	return 0;
}




void ajouterRepertoireCourant( char* chemin, char nouveauChemin[] ) {
	
	int taille = strlen(chemin);
	
	if( taille > 2 && chemin[0] == '.' && chemin[1] == '/' ) strcpy(nouveauChemin,chemin);
	else {
		strcpy(nouveauChemin, "./" );
		strcat(nouveauChemin,chemin);
	}

}


// retourne -1 si il y a eu une erreur lors de la copie, 0 sinon
int verifierLibrairies( char* cheminLibrairie_1, char* cheminLibrairie_2, char* librairie_1, char* librairie_2, int* copieEffectuee ) {

	// on ajoute les caracteres "./"
	ajouterRepertoireCourant( cheminLibrairie_1, librairie_1 );
	ajouterRepertoireCourant( cheminLibrairie_2, librairie_2 );


	*copieEffectuee = 0;

	// cas ou on entre deux fois la même librairie
	// on copie la librairie pour pouvoir l'ouvrir deux fois
	if( ! strcmp(librairie_1, librairie_2) ) {

		strcat( librairie_2, ".copy" );

		// on copie la librairie
		if( copierFichier( librairie_1, librairie_2 ) ) return -1;

		*copieEffectuee = 1;

	}

	return 0;

}

