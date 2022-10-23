#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include<unistd.h>

// "constantes"
#define TAILLE 12
#define NB_COLORS 6
#define NB_ESSAIS 22

#define FG_SHIFT 30
#define BG_SHIFT 40

// type de donnees        1     2     3       4     5         6     7       8
typedef enum {BLACK = 0, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE, MAX_COLORS} ColorCode;

// prototypes
ColorCode ** creeGrille (int, int);                                       // A TESTER
void detruitGrille (ColorCode ***, int);                                  // OK
void afficheCouleur (int, int, char);                                     // OK
void afficheGrille (ColorCode **, int);                                   // OK
void randomGrille (ColorCode **, int, int);                               // OK,
int isFlooded (ColorCode **, int);                                        // OK  es ce que la matrice est remplie de la meme couleur ?
void propagate (ColorCode **, int, ColorCode);                            // OK
void propagate_rec (ColorCode **, int, ColorCode, ColorCode, int, int);   // OK
void play (int, int, int);                                                // A TERMINER
void connexions(ColorCode ** ,ColorCode ** , int , int, int);
void controle_rec(ColorCode ** ,ColorCode ** , int , ColorCode , int , int);
void frontieres(ColorCode **, ColorCode **, int );


// implementations    symbol  background  lettre
void afficheCouleur (int fg, int bg, char c){
  printf("\033[%d;%dm %c\033[m", FG_SHIFT + fg, BG_SHIFT + bg, c);
}


ColorCode ** creeGrille(int n, int val){//n est la taille de la matrice (n*n)

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
      M[i][j] = val;
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
   //   printf("| %d ", M[i][j]);

        afficheCouleur (M[i][j], M[i][j], ' ');

    }
    printf("\n");

  //  printf("|\n");
  }

}


void randomGrille (ColorCode ** M, int n, int m){
  srand(clock());
  for(int i = 0 ; i < n ; i++){
    for(int j = 0 ; j < n ; j++){
      M[i][j] = (int)rand() % m;
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





//        taille     couleurs       nbessais max
void play (int t, int nb_col, int n){
  ColorCode ** G;
  int res = -2;
  int ancienRes = -2;

  // initialisation
  G = creeGrille (t, 0);
  randomGrille (G, t, nb_col);
  afficheGrille (G, t);
  
  ColorCode **ctrl = creeGrille(t, -1);
  
 /* for(int a = 0 ; a< t ;a++){
      for(int b = 0 ; b < t ; b++){//reset grille connexions
        ctrl[a][b] = -1;
      }
    }*/
  
  connexions(ctrl, G,  t, 0, 0);




  // debut des interactions
  for (int i = 0; i < n; ++i){
    
    char c = '0';
    printf("\n");
    
    
    for(int k = 0 ; k < nb_col ; k++){
      afficheCouleur(nb_col - k-1, k, c);
      c++;
      printf(" ");
    }
    printf("exit : -1 // il reste %d essai(s)\n", n-i);
    
    
    do{
      scanf("%d", &res);
    }while(res < -1 || res >= MAX_COLORS);



    if (res == -1){
      printf("Bye!\n");
      return;
    }
    
    
     
    
    if(res != ancienRes)
      propagate(G, t, res);
    connexions(ctrl, G,  t, 0, 0);
    frontieres(ctrl, G, t);
    
    afficheGrille(G, t);
    printf("\n===================================================\n");
    afficheGrille(ctrl, t);
    
    
    for(int a = 0 ; a< t ;a++){
      for(int b = 0 ; b < t ; b++){//reset grille connexions
        ctrl[a][b] = -1;
      }
    }
    
    
    ancienRes = res;

    if (isFlooded(G,t)){
      printf("GAGNE!\n");
      return;
    }
  }
}
  
  
  
  void connexions(ColorCode ** Mcontrole, ColorCode **GrilleJeu, int taille, int ligne, int colonne){
    
     
    
    ColorCode couleurr = GrilleJeu[ligne][colonne];
    controle_rec(Mcontrole, GrilleJeu, taille, couleurr, ligne, colonne);
    
  }

  void controle_rec(ColorCode ** Mcontrole, ColorCode **GrilleJeu, int taille, ColorCode couleurr, int ligne, int colonne){
    
    if(GrilleJeu[ligne][colonne] != couleurr )
      return;
      
    else{
      
        Mcontrole[ligne][colonne] = couleurr;
        //printf("case (%d, %d) sous controle\n", ligne, colonne);
  
        //haut :
        if(ligne != 0 && Mcontrole[ligne-1][colonne] != couleurr)
          controle_rec(Mcontrole, GrilleJeu, taille, couleurr, ligne-1, colonne);
  
        //bas :
        if(ligne != taille-1 && Mcontrole[ligne+1][colonne] != couleurr)
          controle_rec(Mcontrole, GrilleJeu, taille, couleurr, ligne+1, colonne);
  
        //gauche :
        if(colonne != 0 && Mcontrole[ligne][colonne-1] != couleurr)
          controle_rec(Mcontrole, GrilleJeu, taille, couleurr, ligne, colonne-1);
  
        //droite :
        if(colonne != taille-1 && Mcontrole[ligne][colonne+1] != couleurr)
          controle_rec(Mcontrole, GrilleJeu, taille, couleurr, ligne, colonne+1);
    }
}


void frontieres(ColorCode ** Mcontrole, ColorCode **GrilleJeu, int taille){
  
  //stocker la couleure de controle :
  ColorCode coulcontrole = GrilleJeu[0][0];
  
  //matrice boleenne des cases visitées
  ColorCode **comptee = creeGrille(taille, 0);
  
  
  //première boucle : détecter les cellules frontalieres à la zone de controle
  
  for(int ligne = 0 ;ligne < taille ; ligne++){
      for(int colonne = 0 ; colonne < taille ; colonne++){
        
        //ajouter les 4 cases alentours :
        
        //haut :
        if(ligne != 0 && comptee[ligne-1][colonne] == 0 && Mcontrole[ligne][colonne] == coulcontrole){
          comptee[ligne-1][colonne] = 1;  //compter la case comme visitée 
          Mcontrole[ligne-1][colonne]  = GrilleJeu[ligne-1][colonne];     
        }
        
        //bas :
        if(ligne != taille-1 && comptee[ligne+1][colonne] == 0 && Mcontrole[ligne][colonne] == coulcontrole){
          comptee[ligne+1][colonne] = 1;  //compter la case comme visitée
          Mcontrole[ligne+1][colonne]  = GrilleJeu[ligne+1][colonne];
          
        }
        
        //gauche :
        if(colonne != 0 && comptee[ligne][colonne-1] == 0 && Mcontrole[ligne][colonne] == coulcontrole){
          comptee[ligne][colonne-1] = 1;  //compter la case comme visitée
          Mcontrole[ligne][colonne-1]  = GrilleJeu[ligne][colonne-1];
          
        }
        
        //droite :
        if(colonne != taille-1 && comptee[ligne][colonne+1] == 0 && Mcontrole[ligne][colonne] == coulcontrole){
          comptee[ligne][colonne+1] = 1;  //compter la case comme visitée
          Mcontrole[ligne][colonne+1]  = GrilleJeu[ligne][colonne+1];
          
        }
      }
    }
    
    //ici les cases alentoures à celles sous controles donc ajoutées à la matrice de controles
    //maintenant, il faut regarder la connexion des cases ajoutées...
    
    //une case frontière est caractérisée par comptee[i][j] = 1 et GrilleJeu[i][j] != coulcontrole
    
    for(int ligne = 0 ;ligne < taille ; ligne++){
      for(int colonne = 0 ; colonne < taille ; colonne++){
        
        if(comptee[ligne][colonne] == 1 && GrilleJeu[ligne][colonne] != coulcontrole){
          connexions(Mcontrole, GrilleJeu, taille, ligne, colonne);
        }
        
      }
    }
    //ici la matrice de controle affiche toutes les cases annexables en jouant leur couleur !
  }
  
  

  
  
  
  
  /*
  
  int * t nbcouleursAdj(ColorCode ** M, int taille, int nb_col){
    
    int * res = malloc(nb_col*sizeof(int));//allouer le tableau résultat
    
    for(int i = 0 ; i < nb_col ; i++){//initialiser le tableau résultat
      res[i] = 0;
    }
    
    
    //allouer la matrice booléenne de cases visités : 
    int ** P = creeGrille(taille, 0);
    
    //allouer la matrice booléenne de cases connectées à celle en haut à gauche : 
    int ** C = creeGrille(taille, 0);
    
    
    //le robot commence en haut à gauche
    int x = 0;//absice robot
    int y = 0;//ordonnée robot
    ColorCode c = M[0][0];//couleure du robot
    P[0][0] = 1; // la case en haut à gauche est considérée comme visitée;
    
    int toutEstVisite = 0;//variable booléenne pour savoir si toutes les cases ont étés visitées
    
    int depl = 0;
    
    while(!toutEstVisite){
      
      depl = 0;
      
      //enregistrer les cases aux 4 coins : 
      
      if(y != 0 && P[y-1][x] == 0)//haut
        res(M[y-1][x]) += 1; //incrémenter la couleure présente
        
      if(x != taille-1 && P[y][x+1] == 0)//droite
        res(M[y][x+1]) += 1; //incrémenter la couleure présente
        
      if(y != taille -1 && P[y+1][x] == 0)//bas
        res(M[y+1][x]) += 1; //incrémenter la couleure présente
        
      if(x != 0 && P[y][x-1] == 0)//gauche
        res(M[y][x-1]) += 1; //incrémenter la couleure présente
      
      
      //déplacer le bot sur une case non visitée, de même couleure :
      
      if(y != 0 && P[y-1][x] == 0 && M[y-1][x] == c && !depl){//haut
        y = y-1;
        depl = 1;//la variable deplacement deviens vraie pour ce tour
      }
      if(x != taille-1 && P[y][x+1] == 0 && M[y][x+1] == c && !depl){//droite
        x = x+1;
        depl = 1;//la variable deplacement deviens vraie pour ce tour
      }
      if(y != taille -1 && P[y+1][x] == 0 && M[y+1][x] == c && !depl){//bas
        y = y+1;
        depl = 1;//la variable deplacement deviens vraie pour ce tour
      }
      if(x != 0 && P[y][x-1] == 0 && M[y][x-1] == c && !depl){//gauche
        x = x+1;  
        depl = 1;//la variable deplacement deviens vraie pour ce tour
      }
      
      
      
      if(!depl){//si le robot n'a pas été déplacé : il faut le faire démarrer d'un nouveau pt de départ non visité mais sur lequel on a le controle
        
      }
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
    }
    
    
    
    detruitGrille(&P, taille);
    
  }
  
  
  */
  
  
  
  
  
  
  
  
  
  
  
  
  
  void botPlay(int t, int nb_col, int n){
  ColorCode ** G;
  int res = 0;
  int ancienRes = 0;


  // initialisation
  G = creeGrille (t, 0);
  randomGrille (G, t, nb_col);
  afficheGrille (G, t);


  // debut des interactions
  for (int i = 0; i < n; ++i){
    
    
    //afficher les controles
    char c = '0';
    printf("\n");
    for(int k = 0 ; k < MAX_COLORS ; k++){
      afficheCouleur(MAX_COLORS - k-1, k, c);
      c++;
      printf(" ");
    }
    
    
    
    printf("exit : -1 // il reste %d essai(s)\n", n-i);
    
    res = (res + 1) % (nb_col);
    
    printf("couleure %d \n", res);
  
    //sleep(1);   
    

    if(res != ancienRes)
      propagate(G, t, res);
    

    afficheGrille(G, t);
    ancienRes = res;

    if (isFlooded(G,t)){
      printf("GAGNE!\n");
      return;
    }

  }
  
  printf("PERDU!\n");

  // desallocation memoire
  detruitGrille(&G, t);
}






















int main (){/* gcc -c -Wall -Wextra floodit.c && gcc floodit.o -lm -o floodit && ./floodit */


  //botPlay(TAILLE, NB_COLORS, NB_ESSAIS);
  
  play(TAILLE, NB_COLORS, NB_ESSAIS);




  return 0;
}
