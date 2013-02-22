#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL/SDL.h>

#include "calque.h"
#include "misc.h"
#include "main.h"




// int main(int argc, char **argv)
int main(int argc, char **argv)
{

// Paramètres de la texture (valeurs d'entrée)
    int seed;
	int octaves;
	int frequence;
	float persistance;
    int largeur;

    int seuil_rouge;
    int seuil_vert;
    int seuil_bleu;

    couleur couleur1;
    couleur couleur2;
    couleur couleur3;


    int option_lissage;
    int degre_lissage;

    char chaine[100] = "";

    FILE* fichier = NULL;
    FILE* errorlog = NULL;

    // fichier = fopen("test.ptx", "r");
    fichier = fopen(argv[1], "r");

    if (fichier != NULL) {
        fscanf(fichier, "%s %d\n", chaine, &seed);
        fscanf(fichier, "%s %d\n", chaine, &octaves);
        fscanf(fichier, "%s %d\n", chaine, &frequence);
        fscanf(fichier, "%s %f\n", chaine, &persistance);
        fscanf(fichier, "%s %d\n", chaine, &largeur);
        fscanf(fichier, "%s %d\n", chaine, &seuil_rouge);
        fscanf(fichier, "%s %d\n", chaine, &seuil_vert);
        fscanf(fichier, "%s %d\n", chaine, &seuil_bleu);
        fscanf(fichier, "%s %d\n", chaine, &couleur1.rouge);
        fscanf(fichier, "%s %d\n", chaine, &couleur1.vert);
        fscanf(fichier, "%s %d\n", chaine, &couleur1.bleu);
        fscanf(fichier, "%s %d\n", chaine, &couleur2.rouge);
        fscanf(fichier, "%s %d\n", chaine, &couleur2.vert);
        fscanf(fichier, "%s %d\n", chaine, &couleur2.bleu);
        fscanf(fichier, "%s %d\n", chaine, &couleur3.rouge);
        fscanf(fichier, "%s %d\n", chaine, &couleur3.vert);
        fscanf(fichier, "%s %d\n", chaine, &couleur3.bleu);
        fscanf(fichier, "%s %d\n", chaine, &option_lissage);
        fscanf(fichier, "%s %d\n", chaine, &degre_lissage);

        fclose(fichier);
    }
    else {
        errorlog = fopen("error.txt", "a");
        if (errorlog != NULL) fprintf(errorlog,"\nImpossible d'ouvrir le fichier.\n");
        fclose(errorlog);
        return 1;
    }

// Génération aléatoire : utilisation de la graine.
    srand(seed);

// Création de calque
    struct calque *base;

// Initialisation du calque
    base = init_calque(largeur,1);
    if (!base){
        return 1;
    }


// Transformation via l'algorithme de Perlin.
    calque* random = generer_calque_alea(base);
    enregistrer_bmp(random, "bitmap/aleatoire.bmp");
    generer_calque_travail(frequence, octaves, persistance, base, random);

    enregistrer_bmp(base, "bitmap/resultat_GS.bmp");
    enregistrer_bmp_rgb(base, "bitmap/resultat_RGB.bmp", seuil_rouge, seuil_vert, seuil_bleu, couleur1, couleur2, couleur3);
    enregistrer_bmp_alt(base, "bitmap/resultat_alt.bmp", seuil_rouge, couleur1, couleur2);



// Version lissée si lissage activé en option.

    if (option_lissage != 0) {
        // lissage_calque(degre_lissage, base_lisse, seuil_rouge, seuil_vert, seuil_bleu);
        calque* calque_lisse = lissage_calque(degre_lissage, base);

        enregistrer_bmp(calque_lisse, "bitmap/resultat_GS_lisse.bmp");
        enregistrer_bmp_rgb(calque_lisse, "bitmap/resultat_RGB_lisse.bmp", seuil_rouge, seuil_vert, seuil_bleu, couleur1, couleur2, couleur3);
        enregistrer_bmp_alt(calque_lisse, "bitmap/resultat_alt_lisse.bmp", seuil_rouge, couleur1, couleur2);

        free_calque(calque_lisse);

    }

	return 0;
}

// Fin de fichier.
