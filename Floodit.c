#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

// "constantes"
#define TAILLE 12
#define NB_COLORS 6
#define NB_ESSAIS 22

#define FG_SHIFT 30
#define BG_SHIFT 40

// type de donnees        1     2     3       4     5         6     7       8
typedef enum {BLACK = 0, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE, MAX_COLORS} ColorCode;

// prototypes
ColorCode ** creeGrille (int);                                            // A TESTER
void detruitGrille (ColorCode ***, int);                                  // OK
void afficheCouleur (int, int, char);                                     // OK
void afficheGrille (ColorCode **, int);                                   // OK
void randomGrille (ColorCode **, int, int);                               // OK,
int isFlooded (ColorCode **, int);                                        // OK  es ce que la matrice est remplie de la meme couleur ?
void propagate (ColorCode **, int, ColorCode);                            // OK
void propagate_rec (ColorCode **, int, ColorCode, ColorCode, int, int);   // OK
void play (int, int, int);                                                // A TERMINER



// implementations    symbol  background  lettre
void afficheCouleur (int fg, int bg, char c){
  printf("\033[%d;%dm %c\033[m", FG_SHIFT + fg, BG_SHIFT + bg, c);
}


ColorCode ** creeGrille(int n){//n est la taille de la matrice (n*n)

//creer le tableau de pointeurs
  ColorCode ** M;
  M = malloc(n*sizeof(int*));

//creer chaque ligne de la matrice
  for(int i = 0 ; i <= n ; i++){
    M[i] = malloc(n*sizeof(int));
  }

//initialiser chaque ligne
  for(int i = 0 ; i <= n ; i++){
    for(int j = 0 ; j <= n ; j++){
      M[i][j] = 0;
    }
  }
  return M;
}


void detruitGrille (ColorCode *** M, int n){//n est la taille de la matrice (n*n)
  for(int i = 0 ; i < n; i++){

    if((*M)[i] == NULL){
      printf("la ligne %d n'a pas été détruite\n", i);
      i++;
    }

    free((*M)[i]);
    //printf("la ligne %d est détruite\n", i);
  }

  if((*M) == NULL)
    printf("la matrice est nulle");

  free((*M));
  *M = NULL;
}


void afficheGrille (ColorCode ** M, int n){

  for(int i = 0 ; i < n ; i ++){
    for(int j = 0 ; j < n ; j ++){
    //  printf("| %d ", M[i][j]);
      afficheCouleur (M[i][j], M[i][j], ' ');
    }
    printf("\n");

  //  printf("|\n");
  }

}


void randomGrille (ColorCode ** M, int n, int m){
  srand(clock());
  for(int i = 0 ; i < n ; i++){
    for(int j = 0 ; j < m ; j++){
      M[i][j] = (int)rand() % MAX_COLORS;
    }
  }
}


int isFlooded (ColorCode ** M, int n){//renvois 1 si la matrice est remplie et 0 sinon
  ColorCode c = M[0][0];
  for(int i = 0 ; i < n ; i++){
    for(int j = 0 ; j < n ; j++){
      if(M[i][j] != c){
        return 0;
      }
    }
  }
  return 1;
}

void propagate_rec (ColorCode ** M, int t, ColorCode ancien, ColorCode nouveau, int l, int c){
  if(M[l][c] != ancien)
    return;

  else{
    M[l][c] = nouveau;

      //haut :
      if(l != 0)
        propagate_rec(M, t, ancien, nouveau, l-1, c);

      //bas :
      if(l != t-1)
        propagate_rec(M, t, ancien, nouveau, l+1, c);

      //gauche :
      if(c != 0)
        propagate_rec(M, t, ancien, nouveau, l, c-1);

      //droite :
      if(c != t-1)
        propagate_rec(M, t, ancien, nouveau, l, c+1);
  }
}







//            matrice grille   taille  couleure desiree
void propagate (ColorCode ** M, int n, ColorCode c){    //colorier en c toutes les cases de la même couleur que la case en haut à gauche
  //propager le controle à toutes les cellules de même couleurs adjacentes au cellules sous control
  propagate_rec(M, n, M[0][0], c, 0, 0);
}





//        taille     larg        nbessais max
void play (int t, int nb_col, int n){
  ColorCode ** G;
  int res = -1;

  // initialisation
  G = creeGrille (t);
  randomGrille (G, t, nb_col);
  afficheGrille (G, t);


  // debut des interactions
  for (int i = 0; i < n; ++i){
    for(int k = 0 ; K < MAX_COLORS ; k++){
      afficheCouleur(WHITE, k, k);
    }
    do{
      scanf("%d", &res);
    }while(res < 0 || res <= MAX_COLORS);


  }
  //plus dinteraction

  // resultats
  if (res == -1)
    printf("Bye!\n");
  else if (isFlooded(G,t))
    printf("GAGNE!\n");
  else
    printf("PERDU!\n");

  // desallocation memoire
  detruitGrille(&G, t);
}



int main (){/* gcc -c -Wall -Wextra floodit.c && gcc floodit.o -lm -o floodit && ./floodit */

  //play(TAILLE, NB_COLORS, NB_ESSAIS);
  printf("\n\n\n");

  ColorCode **G = creeGrille(TAILLE);

  randomGrille(G, TAILLE, TAILLE);

  afficheGrille(G, TAILLE);

  propagate (G, TAILLE, BLUE);

  printf("\n\n\n");


  afficheGrille(G, TAILLE);


  detruitGrille(&G, TAILLE);


  printf("\n\n\n");



  return 0;
}
