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
int* nbCouleurAnnexables(ColorCode** , int , int );
int maximum(int *, int );
void playVSbot(int , int , int );
int nbCasesSousControle(ColorCode**, int, int);
int couleurAeliminer(int ** , int * , int , int );


// implementations    symbol  background  lettre
void afficheCouleur (int fg, int bg, char c){
  printf("\033[%d;%dm %c\033[m", FG_SHIFT + fg, BG_SHIFT + bg, c);
}


ColorCode ** creeGrille(int n, int val){//n est la taille de la matrice (n*n)

//creer le tableau de pointeurs
  ColorCode ** M;
  M = malloc(n*sizeof(int*));

//creer chaque ligne de la matrice
  for(int i = 0 ; i < n ; i++){
    M[i] = malloc(n*sizeof(int));
  }

//initialiser chaque ligne
  for(int i = 0 ; i < n ; i++){
    for(int j = 0 ; j < n ; j++){
      M[i][j] = val;
    }
  }
  return M;
}


void detruitGrille (ColorCode *** M, int n){//n est la taille de la matrice (n*n)
  for(int i = 0 ; i < n; i++){

    if((*M)[i] == NULL){
      printf("la ligne %d n'a pas ??t?? d??truite\n", i);
      i++;
    }

    free((*M)[i]);
    //printf("la ligne %d est d??truite\n", i);
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
void propagate (ColorCode ** M, int n, ColorCode c){    //colorier en c toutes les cases de la m??me couleur que la case en haut ?? gauche
  //propager le controle ?? toutes les cellules de m??me couleurs adjacentes au cellules sous control
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
  frontieres(ctrl, G, t);
  int * annexables =  nbCouleurAnnexables(ctrl,  t,  nb_col);
  printf("jouer %d (%d connexions)", maximum(annexables, nb_col), annexables[maximum(annexables, nb_col)] );


  // debut des interactions
  for (int i = 0; i < n; ++i){

    //afficher les controles
    char c = '0';
    printf("\n");
    afficheCouleur(WHITE, 0, c);
    c++;
    printf(" ");
    for(int k = 1 ; k < nb_col ; k++){
      afficheCouleur(0, k, c);
      c++;
      printf(" ");
    }
    printf("exit : -1 // il reste %d essai(s)\n", n-i);


    do{
      scanf("%d", &res);
    }while(res < -1 || res >= nb_col);



    if (res == -1){
      printf("Bye!\n");
      detruitGrille(&G, t);
      detruitGrille(&ctrl, t);
      return;
    }




    if(res != ancienRes)
      propagate(G, t, res);

    connexions(ctrl, G,  t, 0, 0);
    frontieres(ctrl, G, t);
    int * annexables =  nbCouleurAnnexables(ctrl,  t,  nb_col);

    afficheGrille(G, t);
  //  printf("\n===================================================\n");
   // afficheGrille(ctrl, t);

    printf("jouer %d (%d connexions)", maximum(annexables, nb_col), annexables[maximum(annexables, nb_col)] );


    for(int a = 0 ; a< t ;a++){
      for(int b = 0 ; b < t ; b++){//reset grille connexions
        ctrl[a][b] = -1;
      }
    }


    ancienRes = res;

    if (isFlooded(G,t)){
      printf("GAGNE!\n");
      detruitGrille(&G, t);
      detruitGrille(&ctrl, t);
      return;
    }
  }
  printf("perdu :(\n");
  detruitGrille(&G, t);
  detruitGrille(&ctrl, t);
}

  int nbCasesSousControle(ColorCode**G, int t, int n){
    ColorCode ** co = creeGrille(t, -1);
    connexions(co, G, t, 0, 0);

    int r = 0;

    for(int a = 0 ; a< t ;a++){
      for(int b = 0 ; b < t ; b++){//reset grille connexions
        if(co[a][b] != -1){
          r++;
        }
      }
    }
    detruitGrille(&co, t);
    return r;
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

  //matrice boleenne des cases visit??es
  ColorCode **comptee = creeGrille(taille, 0);


  //premi??re boucle : d??tecter les cellules frontalieres ?? la zone de controle

  for(int ligne = 0 ;ligne < taille ; ligne++){
      for(int colonne = 0 ; colonne < taille ; colonne++){

        //ajouter les 4 cases alentours :

        //haut :
        if(ligne != 0 && comptee[ligne-1][colonne] == 0 && Mcontrole[ligne][colonne] == coulcontrole){
          comptee[ligne-1][colonne] = 1;  //compter la case comme visit??e
          Mcontrole[ligne-1][colonne]  = GrilleJeu[ligne-1][colonne];
        }

        //bas :
        if(ligne != taille-1 && comptee[ligne+1][colonne] == 0 && Mcontrole[ligne][colonne] == coulcontrole){
          comptee[ligne+1][colonne] = 1;  //compter la case comme visit??e
          Mcontrole[ligne+1][colonne]  = GrilleJeu[ligne+1][colonne];

        }

        //gauche :
        if(colonne != 0 && comptee[ligne][colonne-1] == 0 && Mcontrole[ligne][colonne] == coulcontrole){
          comptee[ligne][colonne-1] = 1;  //compter la case comme visit??e
          Mcontrole[ligne][colonne-1]  = GrilleJeu[ligne][colonne-1];

        }

        //droite :
        if(colonne != taille-1 && comptee[ligne][colonne+1] == 0 && Mcontrole[ligne][colonne] == coulcontrole){
          comptee[ligne][colonne+1] = 1;  //compter la case comme visit??e
          Mcontrole[ligne][colonne+1]  = GrilleJeu[ligne][colonne+1];

        }
      }
    }

    //ici les cases alentoures ?? celles sous controles donc ajout??es ?? la matrice de controles
    //maintenant, il faut regarder la connexion des cases ajout??es...

    //une case fronti??re est caract??ris??e par comptee[i][j] = 1 et GrilleJeu[i][j] != coulcontrole

    for(int ligne = 0 ;ligne < taille ; ligne++){
      for(int colonne = 0 ; colonne < taille ; colonne++){

        if(comptee[ligne][colonne] == 1 && GrilleJeu[ligne][colonne] != coulcontrole){
          connexions(Mcontrole, GrilleJeu, taille, ligne, colonne);
        }

      }
    }
    //ici la matrice de controle affiche toutes les cases annexables en jouant leur couleur !
    detruitGrille(&comptee, taille);
  }

  int* nbCouleurAnnexables(ColorCode** Mcontrole, int taille, int nb_couleurs){

    int *res;

    res = malloc(nb_couleurs*sizeof(int));//allouer le tableau r??sultat

    for(int i = 0 ; i < nb_couleurs ; i++){//initialiser le tableau r??sultat
      (*res) = 0;
    }

    for(int ligne = 0 ;ligne < taille ; ligne++){
      for(int colonne = 0 ; colonne < taille ; colonne++){

        if(Mcontrole[ligne][colonne] != -1 && Mcontrole[ligne][colonne] != Mcontrole[0][0]){
          res[Mcontrole[ligne][colonne]]++;
        }

      }
    }

    return res;
  }

  int maximum(int * tab, int size){
    int index = 0;
    int r = tab[0];
    for(int i = 0 ; i < size ; i++){
      if(tab[i]>r){
        r = tab[i];
        index = i;
      }
    }
    return index;
  }

  int couleurAeliminer(int ** Mjeu, int*annexables, int taille, int nb_couleurs){
    //tester toutes les couleurs :
    int count = 0;
    for(int couleur =0 ; couleur < nb_couleurs ; couleur++){
      if(annexables[couleur]>0){
        count = 0;
        for(int ligne = 0 ;ligne < taille ; ligne++){
            for(int colonne = 0 ; colonne < taille ; colonne++){
              if(Mjeu[ligne][colonne] == couleur)
                count++;
              }
            }
            if(count == annexables[couleur] && couleur != Mjeu[0][0])
              return couleur;
      }
    }
    return -1;
  }




  /*

  int * t nbcouleursAdj(ColorCode ** M, int taille, int nb_col){

    int * res = malloc(nb_col*sizeof(int));//allouer le tableau r??sultat

    for(int i = 0 ; i < nb_col ; i++){//initialiser le tableau r??sultat
      res[i] = 0;
    }


    //allouer la matrice bool??enne de cases visit??s :
    int ** P = creeGrille(taille, 0);

    //allouer la matrice bool??enne de cases connect??es ?? celle en haut ?? gauche :
    int ** C = creeGrille(taille, 0);


    //le robot commence en haut ?? gauche
    int x = 0;//absice robot
    int y = 0;//ordonn??e robot
    ColorCode c = M[0][0];//couleure du robot
    P[0][0] = 1; // la case en haut ?? gauche est consid??r??e comme visit??e;

    int toutEstVisite = 0;//variable bool??enne pour savoir si toutes les cases ont ??t??s visit??es

    int depl = 0;

    while(!toutEstVisite){

      depl = 0;

      //enregistrer les cases aux 4 coins :

      if(y != 0 && P[y-1][x] == 0)//haut
        res(M[y-1][x]) += 1; //incr??menter la couleure pr??sente

      if(x != taille-1 && P[y][x+1] == 0)//droite
        res(M[y][x+1]) += 1; //incr??menter la couleure pr??sente

      if(y != taille -1 && P[y+1][x] == 0)//bas
        res(M[y+1][x]) += 1; //incr??menter la couleure pr??sente

      if(x != 0 && P[y][x-1] == 0)//gauche
        res(M[y][x-1]) += 1; //incr??menter la couleure pr??sente


      //d??placer le bot sur une case non visit??e, de m??me couleure :

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



      if(!depl){//si le robot n'a pas ??t?? d??plac?? : il faut le faire d??marrer d'un nouveau pt de d??part non visit?? mais sur lequel on a le controle

      }







    }



    detruitGrille(&P, taille);

  }


  */





  void botPlay(int t, int nb_col, int n){
  ColorCode ** G;
  int couleurselectionnee = 0;
  int ancienRes = 0;


  // initialisation
  G = creeGrille (t, 0);
  randomGrille (G, t, nb_col);
  afficheGrille (G, t);


  ColorCode **ctrl = creeGrille(t, -1);
  connexions(ctrl, G,  t, 0, 0);
  frontieres(ctrl, G, t);
  int * annexables =  nbCouleurAnnexables(ctrl,  t,  nb_col);
  couleurselectionnee = maximum(annexables, nb_col);


  // debut des interactions
  for (int i = 0; i < n; ++i){


    //afficher les controles
    char c = '0';
    printf("\n");
    afficheCouleur(WHITE, 0, c);
    c++;
    printf(" ");
    for(int k = 1 ; k < nb_col ; k++){
      afficheCouleur(0, k, c);
      c++;
      printf(" ");
    }

        propagate(G, t, couleurselectionnee);


    printf("exit : -1 // il reste %d essai(s)\n", n-i);

    printf("couleure %d \n", couleurselectionnee);

    //sleep(1);



    connexions(ctrl, G,  t, 0, 0);
    frontieres(ctrl, G, t);
    int * annexables =  nbCouleurAnnexables(ctrl,  t,  nb_col);

    afficheGrille(G, t);
    //printf("\n===================================================\n");
    //afficheGrille(ctrl, t);

    couleurselectionnee = maximum(annexables, nb_col);
    printf("couleure %d \n", couleurselectionnee);



    for(int a = 0 ; a< t ;a++){
      for(int b = 0 ; b < t ; b++){//reset grille connexions
        ctrl[a][b] = -1;
      }
    }





    ancienRes = couleurselectionnee;

    if (isFlooded(G,t)){
      printf("GAGNE!\n");
      return;
    }

  }

  printf("PERDU!\n");

  // desallocation memoire
  detruitGrille(&G, t);
    detruitGrille(&ctrl, t);

}



void playVSbot(int t, int nb_col, int n){


  //joueur :
  ColorCode**Gplay = creeGrille(t, 0);//cr??er la grille du joueur
  randomGrille(Gplay, t, nb_col);//avoir une grille al??atoire pour le joueur
  int couleurJoueur = -2;
  int ancienneCouleurJoueur = -2;
  int nbCasesCtrlPlay = 0;


  //robot :
  ColorCode**Gbot = creeGrille(t, 0);//cr??er la grille du robot
  for(int i = 0; i < t; i++){
    for(int j = 0 ; j < t; j++){//mettre la m??me grille pour le robot
      Gbot[i][j] = Gplay[i][j];
    }
  }
  int couleurBot;
  ColorCode **ctrl = creeGrille(t, -1);
  int nbCasesCtrlBot = 0;

  double rapport = 0;
  //ici, il y a deux grilles diff??rentes mais avec le m??me contenu







  // debut des interactions
  for (int tour = 0; tour < n; tour++){

    nbCasesCtrlBot = nbCasesSousControle(Gbot, t, nb_col);
    nbCasesCtrlPlay = nbCasesSousControle(Gplay, t, nb_col);
    rapport = (nbCasesCtrlBot - nbCasesCtrlPlay)/(double)nbCasesCtrlPlay;

    printf("Grille du robot (%d cases sous contr??le) :\n", nbCasesCtrlBot);
    afficheGrille(Gbot, t);

    if(rapport > 0){
      printf("\n\nles r??sultat du robot sont %.2f fois mieux que les votre", rapport);
    }else if(rapport < 0){
      printf("\n\nles r??sultat du robot sont %.2f fois moins bons que les votre", -rapport);
    }else
      printf("\n\nvos r??sultats sont identiques au robot");

    printf("\n\nVotre grille (%d cases sous contr??le) :\n", nbCasesCtrlPlay); //afficher les 2 grilles
    afficheGrille(Gplay, t);


    //afficher les controles
    char c = '0';
    printf("\n");
    afficheCouleur(WHITE, 0, c);
    c++;
    printf(" ");
    for(int k = 1 ; k < nb_col ; k++){
      afficheCouleur(0, k, c);
      c++;
      printf(" ");
    }
    printf("exit : -1 // il reste %d essai(s)\n", n-tour);


    //joueur entre sa couleure :
    printf("entrez votre code couleure : ");
    do{
      scanf("%d", &couleurJoueur);
    }while(couleurJoueur < -1 || couleurJoueur >= nb_col);

     if(couleurJoueur == -1){
      printf("Bye!\n");
      detruitGrille(&Gplay, t);
      detruitGrille(&ctrl, t);
      detruitGrille(&Gbot, t);
      return;
    }

    //grille du joueur propag??e :
    if(couleurJoueur != ancienneCouleurJoueur){
      propagate(Gplay, t, couleurJoueur);
      ancienneCouleurJoueur = couleurJoueur;
    }


    //robot fait son choix
    connexions(ctrl, Gbot,  t, 0, 0);
    frontieres(ctrl, Gbot, t);
    int * annexables =  nbCouleurAnnexables(ctrl,  t,  nb_col);// le robot cr??e ses grilles intelligentes
    if(couleurAeliminer(Gbot, annexables, t, nb_col) != -1){
      couleurBot = couleurAeliminer(Gbot, annexables, t, nb_col);
    }else{
      couleurBot = maximum(annexables, nb_col);
    }

    //printf("\nla couleure ?? ??liminer est %d", couleurAeliminer(Gbot, annexables, t, nb_col));

    printf("\nle robot joue : %d\n\n========================================\n\n", couleurBot);
    propagate(Gbot, t, couleurBot);


    if(isFlooded(Gbot, t) && isFlooded(Gplay, t)){
      printf("Grille du robot (%d cases sous contr??le) :\n", nbCasesCtrlBot);
      afficheGrille(Gbot, t);
      printf("\n\nVotre grille (%d cases sous contr??le) :\n", nbCasesCtrlPlay); //afficher les 2 grilles
      afficheGrille(Gplay, t);

      printf("??galit?? !\n");
      detruitGrille(&Gplay, t);
      detruitGrille(&ctrl, t);
      detruitGrille(&Gbot, t);
      return;
    }



    if(isFlooded(Gbot, t)){
      printf("Grille du robot (%d cases sous contr??le) :\n", nbCasesCtrlBot);
      afficheGrille(Gbot, t);
      printf("\n\nVotre grille (%d cases sous contr??le) :\n", nbCasesCtrlPlay); //afficher les 2 grilles
      afficheGrille(Gplay, t);

      printf("le robot a gagn?? ! il vous restait %d cases\n",t*t- nbCasesCtrlPlay);
      detruitGrille(&Gplay, t);
      detruitGrille(&ctrl, t);
      detruitGrille(&Gbot, t);
      return;
    }

    if(isFlooded(Gplay, t)){
      printf("Grille du robot (%d cases sous contr??le) :\n", nbCasesCtrlBot);
      afficheGrille(Gbot, t);
      printf("\n\nVotre grille (%d cases sous contr??le) :\n", nbCasesCtrlPlay); //afficher les 2 grilles
      afficheGrille(Gplay, t);

      printf("vous avez gagn?? ! il restait %d cases au robot\n", t*t-nbCasesCtrlBot);
      detruitGrille(&Gplay, t);
      detruitGrille(&ctrl, t);
      detruitGrille(&Gbot, t);
      return;
    }



  }

    printf("Grille du robot :\n");
    afficheGrille(Gbot, t);
    printf("\n\nVotre grille :\n"); //afficher les 2 grilles
    afficheGrille(Gplay, t);

    printf("Game over :\n\nil vous restait %d cases et il restait %d cases au robot",t*t-nbCasesCtrlPlay,t*t-nbCasesCtrlBot);
    if(rapport > 1)
      printf(" par ??l??mination le robot gagne ! \n");
    else if(rapport < 1)
      printf(" vous gagnez par ??l??mination ! \n");
    else
      printf(" ??galit?? pure !");




  detruitGrille(&Gplay, t);
  detruitGrille(&ctrl, t);
  detruitGrille(&Gbot, t);
}






int main (){/* gcc -c -Wall -Wextra floodit.c && gcc floodit.o -lm -o floodit && ./floodit */


  //botPlay(TAILLE, NB_COLORS, NB_ESSAIS);

  //play(TAILLE, NB_COLORS, NB_ESSAIS);

  playVSbot(20, 4, 50);


  return 0;
}
