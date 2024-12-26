#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include <time.h> // Pour le random
#include <string.h>

#include "inout.h"


// Variables et parametre
#define tempsPause 2 // Corespond au temps d'attente entre deux génération automatique (en seconde)
#define displaySize 2 // Correspond à la taille prise par les charactere . et # dans notre procédure display
#define MAX_SEED_SIZE 50
#define MAX_SEED_NAME_SIZE 20
#define MAX_RULES 9
int isUniversePeriodic; //Correspond a la périodicité de l'univers, de type INT pour pouvoir faire un scanf sans Warning

//-------------------------------//

//GESTION DES RELGES

typedef struct {
  int rules[MAX_RULES];
  int last;
} rule;

// Regle du jeu de la vie classique : B3/S23
rule bornRule = {{3}, 0}; // Initialisation de la règle de naissance
rule surviveRule = {{2,3}, 1}; // Initialisation de la règle de survie

//-------------------------------//

// GESTION DU HASARD

void initialise_rand(){
  srand(time(NULL));
}

//tire un entier au hasard entre inf et sup
int hasard(int inf, int sup)
{
  int rando = rand();
  return (inf + (rando % (sup - inf +1)));
}

// Creer une seed aléatoire
void random_seeds(bool matrice[HEIGHT][WIDTH]){
  int i=0,j=0;
  for(i=0;i<HEIGHT;i++){
    for(j=0;j<WIDTH;j++){
      matrice[i][j]=hasard(0,1);
    }
  }
}



//-------------------------------//





// Affichage de l'univers
void display(bool matrix[HEIGHT][WIDTH]){
    int i=0,j=0;
    for(i=0;i<HEIGHT;i++){ //cycle les lignes des matrices    ==v
      for(j=0;j<WIDTH;j++){ //cycle les colonnes de matrices ==> affiche une matrice 2 dimensions
        if (matrix[i][j] == 1){
          printf("\033[1;31m"); //Permet d'écrire en rouge les cellules vivantes
          printf("%-*s",displaySize,"#"); // Print le symbole # pour plus de visibilité, et le format permet de choisir l'espace du charactere pour bien alligner les colonnes
          printf("\033[0;30m"); //Retour en noir
        }
        else printf("%-*s",displaySize,".");
      }
      printf("\n");
    }
}


// Premiere façon de compter les voisins vivants autour d'une case
// Dans un univers fini
int count_neighbours(bool matrice[HEIGHT][WIDTH], int i, int j) {
  int neighbours = 0;
  for (int k = -1; k<=1; k++){
    for (int l = -1; l<=1; l++){
      if ((k+i>=0 && k+i<HEIGHT) && (l+j>=0 && l+j<WIDTH)){
        if ((k!=0)||(l!=0)){ // Equivalent a ((k,l) != (0,0)), pour pas se prendre en compte soit meme comme si la case est vivante
          if (matrice[i+k][j+l]==1) neighbours++;
        }
      }
    }
  }
  return neighbours;
}

// Deuxieme façon de compter les voisins vivants autour d'une case
// Mais dans un univers périodique, ex: une case tout à droite est voisine a la case tout à gauche sur la meme ligne
int count_neighbours_periodic(bool matrice[HEIGHT][WIDTH], int i, int j) {
  int neighbours = 0, periodich = 0, periodicw = 0;
  for (int k = -1; k<=1; k++){
    for (int l = -1; l<=1; l++){
        if ((k!=0)||(l!=0)){
          periodich = 0; // h pour HEIGHT
          periodicw = 0; // w pour WIDTH
          if (k+i>=HEIGHT) periodich -= HEIGHT;
          if (k+i<0) periodich += HEIGHT;
          if (l+j>=WIDTH) periodicw -= WIDTH;
          if (l+j<0) periodicw += WIDTH;

          if (matrice[i+k+periodich][j+l+periodicw]==1) neighbours++;
        }
      }
    }
  return neighbours;
}

// Permet de calculer l'état suivant de l'univers
void evolve(bool matrice1[HEIGHT][WIDTH]){
  int neighbours = 0;
  bool matrice2[HEIGHT][WIDTH]; // la  matrice 2 est une copie de la matrice 1. Sans ça, on ne pourrait pas mettre à jour l'univers en une seule fois.
  //L'état d'une cellule à un temps t+1 dépend de son nombre voisins vivants à l'état t.

  bool willBorn = false;
  bool willSurvive = false;

  display(matrice1); // pour visualiser
  printf("Matrice à l'état précedent\n");


  for (int i = 0; i<HEIGHT; i++) {
    for (int j=0; j<WIDTH;j++){
      if (isUniversePeriodic) // Choix du type d'univers
        neighbours = count_neighbours_periodic(matrice1,i,j);
      else
        neighbours = count_neighbours(matrice1,i,j);


      willBorn = false;
      willSurvive = false;

      for (int indiceB = 0; indiceB <= bornRule.last; indiceB++){
        if (neighbours == bornRule.rules[indiceB]){
          //matrice2[i][j]=1; // La cellule nait
          willBorn = true;

        }
      }

      for (int indiceS = 0; indiceS <= surviveRule.last; indiceS++){
        if (neighbours == surviveRule.rules[indiceS]){
          //matrice2[i][j] = matrice1[i][j]; // La cellule survie
          willSurvive = true;

        }
      }

      if (willBorn) matrice2[i][j]  =1;
      else
        if (willSurvive) matrice2[i][j] = matrice1[i][j];
        else matrice2[i][j] = 0;


    }
  }

  printf("\n");

  for (int i = 0; i<HEIGHT; i++) { //écrase la version précédente de la première matrice avec la deuxième matrice
      for (int j=0; j<WIDTH;j++){
    matrice1[i][j]=matrice2[i][j];}
  }

}



// Permet l'animation, le choix du type d'animation
void animate(bool mat[HEIGHT][WIDTH]){

  int i=0,gen=0;

  char reponse='o';
  printf("voulez-vous une animation 'automatique' ou 'interactive ? Entrez votre choix 'a' ou 'i'\n");
  scanf("%c",&reponse);

  while(reponse != 'a' || reponse != 'i' || reponse != 'q'){
    scanf("%c",&reponse);

    if(reponse=='a'){
      printf("combien de générations voulez vous ? : ");
      scanf("%d",&gen);
      for(i=1;i<=gen;i++){
        system("clear");
        evolve(mat);
        display(mat);
        printf("Génération %d\n", i);
        sleep(tempsPause);
      }
      return;
    }

    else if (reponse=='i'){
      /* trouver la manière de passer à une génération suivante en appuyant sur n'importe qu'elle touche est en dehors
      de notre niveau actuel. Nous avons donc trouvé une solution temporaire à votre demande et notre problème en attente d'une solution viable */

      char c='a'; // On initialise un charactere c, de manière à ne pas avoir d'erreur au moment de la saisies de nos choix

      while (c != 'q') {

        printf("Génération %d - Appuyez sur Entrée pour continuer, ou 'q' pour quitter\n", i);
        i++;

        c = getchar();
        while (c != EOF && c != '\n' && c != 'q') { //Permet de ne pas prendre en comptes les caractères inutiles et éviter de passer X génération quand notre réponse possède X caracteres
          c = getchar();
            // Ignorer les caractères indésirables
        }

        system("clear");
        evolve(mat);
        display(mat);
      }
      system("clear");
      return;
    }
    else if (reponse=='q'){
      return;
    }
  }
}

int lireEntierDepuisTerminal() {
    int entier;
    while (scanf("%d", &entier) != 1) {
        while (getchar() != '\n');
        printf("Veuillez entrer un entier valide : ");
    }
    return entier;
}


int main() {

  system("clear"); // Pour plus de lisibilité

  initialise_rand(); // Initialise le temps pour le hasard

  bool univ[HEIGHT][WIDTH];
  char seed[MAX_SEED_SIZE], seedName[MAX_SEED_NAME_SIZE];

  printf("Choix de seed disponible: random / blinker / beacon / glider / full / empty / carre / cross / fumarole / octagon / acorn / diehard / toad / lwspaceship / beehive / block \n");
  printf("Entrez le nom de la seed que vous voulez : ");
  scanf("%s",seedName);
  sprintf(seed,"../seeds/%s-10x10.life",seedName); // Envoie la concaténation dans seed


  if (strcmp(seedName,"random") != 0){
    int charge = load_seed(seed,univ);
    if (charge == 1)
      printf("Successfully loaded seed.\n");
    else{
      printf("Something went wrong.\n");
      return 0;
    }
  }
  else random_seeds(univ);

  printf("Voulez-vous un univers periodique (1) ou non (0) ? : ");
  scanf("%d",&isUniversePeriodic);


  printf("Voulez-vous utiliser les regles de base du jeu de la vie (1) ou non (0) ? : ");
  int choixRegle;
  scanf("%d",&choixRegle);
  if (choixRegle == 0) {
    printf("\nVeuillez choisir vos régles de vie ou de mort. Autres set de règles connues :  \n Highlife : B36/S23 ; \n Replicator : B1357/S1357 (fonctionne bien avec un 'block') ; \n Life without Death : B3/S012345678 ; \n Seeds : B2/S  \n");
    printf("Entrez '-1' pour terminer la saisie des conditions de naissances B. \n B[N] signifie qu'une cellule morte vivra selon le nombre de voisins que vous avez saisie, entre 0 et 8  \n S[N] signifie qu'une cellule vivante survivra selon le nombre de voisins que vous avez saisie entre 0 et 8");

    int i = 0;
    do{
      printf("B[%d]= ",i);
      bornRule.rules[i] = lireEntierDepuisTerminal();
      if (bornRule.rules[i] != -1)
        i++;
    }while (bornRule.rules[i] != -1 && i < MAX_RULES);
    bornRule.last = i-1;


    printf("\n");

    printf("Entrez '-1' pour terminer la saisie des conditions de Survie S \n");

    i = 0;
    surviveRule.last = 0;
    do{
      printf("S[%d]= ",i);
      surviveRule.rules[i] = lireEntierDepuisTerminal();
      if (surviveRule.rules[i] != -1)
        i++;
    }while (surviveRule.rules[i] != -1 && i < MAX_RULES );
    surviveRule.last = i-1;
    printf("\n");


    // AFFICHER LES NOUVELLES REGLES
    printf("Les regles sont: B");
    for (int indiceB = 0; indiceB <= bornRule.last; indiceB++){
      printf("%d",bornRule.rules[indiceB]);
    }
    printf("/S");
    for (int indiceS = 0; indiceS <= surviveRule.last; indiceS++){
      printf("%d",surviveRule.rules[indiceS]);
    }
    printf("\n");

  }


  display(univ);
  animate(univ);

  return 0;
}
