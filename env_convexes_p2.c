#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <MLV/MLV_all.h>

#define SIZE_X 800
#define SIZE_Y 800
#define NB_COULEURS 9

typedef struct s_point{
    double x;
    double y;
} Point;


/**
 * @brief Liste chaînée simple pour gérer les points
 * 
 */
typedef struct s_lst_point{
    Point p;
    struct s_lst_point *next;
} CellulePoint, *ListePoint;


/**
 * @brief Structure liste doublement chaînée circulaire d'un vertex contenant l'adresse d'un point 
 * et son vertex suivant et précédent
 * 
 */
typedef struct _vrtx_{
    Point *s; /* un point de l’ensemble */
    struct _vrtx_ *prev; /* le vertex précédent */
    struct _vrtx_ *next; /* le vertex suivant */
} Vertex, *Polygon;


/**
 * @brief Structure liste chaînée de l'enveloppe convexe contenant un Polygone (Une liste de vertex) 
 * et sa taille actuelle et maximale, ainsi que son enveloppe prochaine
 * 
 */
typedef struct s_convex{
    Polygon pol; /* le polygône */
    struct s_convex *next;
    int curlen; /* la longueur courante */
    int maxlen; /* la longueur maximale */
    MLV_Color couleur;
} ConvexHull, *ListeConvexe;

/////////////////////////////////
// Fonctions fenêtre et dessin //
/////////////////////////////////

void creerFenetre();
void effaceEcran();

/**
 * @brief Genere un point aléatoirement et qui fait partie de la fenêtre
 * 
 * @param forme 1: Génération aléatoire en forme de cercle 2: Génération aléatoire en forme de carré 
 * @param R Rayon du cercle/carré
 * @param centre Centre du cercle/carré
 * @return Point 
 */
Point getPoint(int forme, int R, Point centre);

/**
 * @brief Récupère un point après un clic de l'utilisateur
 * 
 * @return Point 
 */
Point getPointOnClic();

/**
 * @brief Dessine un point de coordonnées x et y sur l'écran avec une couleur
 * 
 * @param p Le point à dessiner
 * @param couleur La couleur du point
 */
void drawPoint(Point p, MLV_Color couleur);

/**
 * @brief Dessine le polygone donné en paramètre
 * 
 * @param poly Le polygone à dessiner
 * @param curlen La taille actuelle du polygone (nombre de sommets)
 * @param utilisateur Mode du programme (0: Aléatoire; 1: Clic-souris)
 */
void dessineConvexe(Polygon poly, MLV_Color couleur, int curlen, int utilisateur);

/**
 * @brief Dessine tous les points dans la liste chaînée listePoint sur l'écran
 * 
 * @param listePoint La liste chaînée de points
 */
void dessinePointsListe(ListePoint listePoint);

/**
 * @brief Ouvre le menu principal du programme
 * 
 * @param mode Mode du programme (aléatoire ou clic-souris)
 * @param forme Forme de la génération aléatoire si mode aléatoire (carré ou cercle)
 * @param deroulement Mode d'affichage si aléatoire (Point par point ou terminal)
 */
void menu(int *mode, int *forme, int *deroulement);

/**
 * @brief Renvoie si la souris est sur un bouton ou non
 * 
 * @param x Coordonnée x de la souris
 * @param y Coordonnée y de la souris
 * @param x_button Coordonnée x du coin supérieur gauche du bouton
 * @param y_button Coordonnée y du coin supérieur gauche du bouton
 * @param widthButton Largeur du bouton
 * @param heightButton Hauteur du bouton
 * @return 1 si la souris est sur le bouton, 0 sinon
 */
int mouseOnButton(int x, int y, int x_button, int y_button, int widthButton, int heightButton);


/////////////////////////
// Fonctions de calcul //
/////////////////////////

/**
 * @brief Renvoie l'orientation d'un triangle
 * 
 * @param Sommet A du triangle
 * @param Sommet B du triangle
 * @param Sommet C du triangle
 * @return 0 si l'orientation est indirecte, 1 sinon
 */
int orientationTriangle(Point A, Point B, Point C);

/**
 * @brief Genere l'enveloppe convexe initiale de 3 points
 * 
 * @param enveloppe L'adresse de l'enveloppe convexe vide
 * @param listePoint La liste des points vide
 * @param utilisateur Mode du programme (0: Aléatoire; 1: Clic-souris)
 * @param nbPoint Nombre de points si aléatoire
 * @param deroulement Mode d'affichage (0: Point par point; 1: Terminal)
 * @param couleurs Liste des couleurs des enveloppes
 */
void genereEnveloppe(ConvexHull *enveloppe, ListePoint *listePoint, int utilisateur, int nbPoint, int deroulement, MLV_Color *couleurs);

/**
 * @brief Effectue le nettoyage avant de l'enveloppe après insertion, et supprime les vertex qu'il
 * faut supprimer
 * 
 * @param poly Adresse du polygone
 * @param enveloppe Adresse de l'enveloppe
 * @param couleurs Liste des couleurs des enveloppes
 */
void nettoyageAvant2(Polygon *poly, ConvexHull *enveloppe, MLV_Color *couleurs);

/**
 * @brief Effectue le nettoyage après de l'enveloppe après insertion, et supprime les vertex qu'il
 * faut supprimer
 * 
 * @param poly Adresse du polygone
 * @param enveloppe Adresse de l'enveloppe
 * @param couleurs Liste des couleurs des enveloppes
 */
void nettoyageArriere2(Polygon *poly, ConvexHull *enveloppe, MLV_Color *couleurs);

int appartientCercle(Point P, Point centre, int R);
int appartientCarre(Point P, Point centre, int R);

/**
 * @brief Calcule l'orientation et insère un point dans l'enveloppe à sa place
 * 
 * @param P L'adresse du point à vérifier dans la liste des points
 * @param poly L'adresse du polygone
 * @param enveloppe L'adresse de l'enveloppe convexe
 * @return 0 si pas d'insertion (orientation directe), 1 sinon
 * @param couleurs Liste des couleurs des enveloppes
 */
int insertionPoint(Point *P, Polygon *poly, ConvexHull *enveloppe, MLV_Color *couleurs);

/**
 * @brief Fonction qui vérifie si un point est à l'exterieur où à l'interieur d'une enveloppe
 * et appele récursivement avec toute la liste des enveloppes
 * 
 * @param P Point à traiter
 * @param listeConvexe La liste des enveloppes
 * @param couleurs Liste des couleurs des enveloppes
 */
void traitementRecursif(Point *P, ListeConvexe *listeConvexe, MLV_Color *couleurs);

/////////////////////
// Fonctions start //
/////////////////////

/**
 * @brief Commence le programme (mode clic-souris)
 * 
 * @param enveloppe Adresse de l'enveloppe convexe
 * @param listePoint Adresse de la liste de points
 * @param couleurs Liste des couleurs des enveloppes
 */
void commenceClic(ConvexHull *enveloppe, ListePoint *listePoint, MLV_Color *couleurs);

/**
 * @brief Commence le programme (mode aléatoire)
 * 
 * @param enveloppe Adresse de l'enveloppe convexe
 * @param listePoint Adresse de la liste de points
 * @param nbPoint Nombre de points
 * @param choix Forme de la génération aléatoire (1: Cercle; 2: Carré)
 * @param deroulement Mode d'affichage (0: Point par point; 1: Terminal)
 * @param couleurs Liste des couleurs des enveloppes
 */
void commenceAleatoire(ConvexHull *enveloppe, ListePoint *listePoint, int nbPoint , int choix, int deroulement, MLV_Color *couleurs);

/////////////////////////
// Fonctions enveloppe //
/////////////////////////

void printConvex(ConvexHull enveloppe);
void printConvexAdresse(Polygon pol);
void supprimeVertex(Polygon *poly);

////////////////////////////
// Fonctions liste points //
////////////////////////////

ListePoint alloueCellule(Point p);
ListePoint insereTete(ListePoint *liste, Point p);
void printListePoint(ListePoint liste);

/////////////////////////////
// Fonctions liste convexe //
/////////////////////////////

ListeConvexe alloueCelluleConvexe(Polygon pol);
void insereQueueConvexe(ListeConvexe *liste, Polygon pol);
void printListeConvexe(ListeConvexe ListeConvexe);

/**
 * @brief Libère la liste chaînée de points ainsi que toutes les enveloppes convexes
 * et tous leurs polygones
 * 
 * @param listePoint Adresse de la liste chaînée de points
 * @param listeConvexe Adresse de la liste chaînée des enveloppes
 */
void freeListes(ListePoint *listePoint, ListeConvexe *listeConvexe);


/**
 * @brief Fonction d'arrêt appelée lors d'un clic sur le bouton fermer
 * 
 * @param data Variable arret à modifier par adresse
 */
void exit_function(void* data){
    int* arret = (int*) data;
    *arret = 1;
}

// Variable pour vérifier l'arrêt du programme
int arret = 0;

int nbConvexe = 0;

int main(void){
    int utilisateur = 0;
    int forme = 0;
    int deroulement = 0;
    int nbPoint = 0;
    int lu = 0 ;


    srand(time(NULL));
    
    menu(&utilisateur, &forme, &deroulement);
    if (!utilisateur){
    do {
        int c;
        printf("Veuillez entrer un nombre entier : ");
        lu = scanf("%d", &nbPoint);
        while ( (c = getchar()) != '\n' && c != EOF);
    }while (lu == 0);
    }
    
    MLV_execute_at_exit(exit_function, &arret);

    if (deroulement == 0){
        creerFenetre();
        effaceEcran(); 
        MLV_actualise_window(); 
    }

    // Liste des couleurs de l'enveloppe (alterne entre NB_COULEURS)
    MLV_Color couleurs[NB_COULEURS] = {  
                                        MLV_rgba(255,0,0,255), MLV_rgba(175,0,0,255),
                                        MLV_rgba(255,255,0,255), MLV_rgba(255,200,0,255), 
                                        MLV_rgba(0,255,0,255), MLV_rgba(0,100,0,255),
                                        MLV_rgba(155,48,255,255),
                                        MLV_rgba(0,0,255,255), MLV_rgba(0,0,175,255), 
                                    };

    Polygon debut = NULL;
    ConvexHull *listeConvexe = NULL;
    insereQueueConvexe(&listeConvexe, debut);
    
    ListePoint listePoint = NULL;
    
    genereEnveloppe(listeConvexe, &listePoint, utilisateur, nbPoint, deroulement, couleurs);

    if (utilisateur){
        commenceClic(listeConvexe, &listePoint, couleurs);
    }
    else{        
        // Cercle = 1; Carré = 2
        commenceAleatoire(listeConvexe, &listePoint, nbPoint, forme, deroulement, couleurs);
    }


    if (!utilisateur){
        while(!arret){
            MLV_wait_seconds(1);
        }
    }

    freeListes(&listePoint, &listeConvexe);
    MLV_free_window();

    return 0;
}

void creerFenetre(){
    MLV_create_window("Projet", "Enveloppe convexe", SIZE_X, SIZE_Y);
}

Point getPointOnClic(){
    double PERTURB = 0.0001/RAND_MAX;
    Point P;
    int x = -1; int y = -1;
    
    while(x < 0 && y < 0 && !(arret)){
        MLV_wait_mouse_or_seconds(&x,&y, 1);
    }
    
    P.x = x + (rand()%2 ? +1. : -1.)*PERTURB*rand();
    P.y = y + (rand()%2 ? +1. : -1.)*PERTURB*rand();

    return P;
}

void drawPoint(Point p, MLV_Color couleur){
    MLV_draw_filled_circle(p.x, p.y, 4, couleur);
    MLV_actualise_window();
}

int orientationTriangle(Point A, Point B, Point C){
    return (B.x - A.x) * (C.y - A.y) - (C.x - A.x) * (B.y - A.y);
}

Polygon newCell(Point *P){
    Polygon poly;
    poly = (Polygon) malloc(sizeof(Vertex));
    if (!poly){ 
        fprintf(stderr,"Plus de memoire "); 
        exit(-1);
    }
    poly->s = P;
    poly->next = poly->prev = poly;

    return poly;
}

void addBefore(Polygon poly, Polygon ins, Polygon *head){
    
    if (!poly)
       { *head = ins; return ;}
    ins->next = poly;
    ins->prev = poly->prev;
    poly->prev = ins ;
    if (! ins->prev)
    { *head = ins; }
    else 
    { 
    ins->prev->next = ins ;
    }

} 

void addAfter(Polygon after, Polygon ins, Polygon *head){
    if (! after){ // empty list
        *head = ins; 
       return ;
       }
    ins->next = after->next;
    if (after->next)  ins->next->prev = ins ;
    ins->prev = after;
    after->next = ins ;
} 

void printConvex(ConvexHull enveloppe){ 
    if (enveloppe.pol != NULL){
        Point *premierPoint = enveloppe.pol->s;
        printf("x: %f, y: %f\n", (enveloppe.pol->s)->x, (enveloppe.pol->s)->y);
        enveloppe.pol = enveloppe.pol->next;
        
        
        for(; enveloppe.pol->s != premierPoint ; enveloppe.pol = enveloppe.pol->next){
            printf("x: %f, y: %f\n", (enveloppe.pol->s)->x, (enveloppe.pol->s)->y);
        }
        
        printf("\n");
    }
}

void printConvexAdresse(Polygon pol){ 
    if (pol != NULL){
        Point *premierPoint = pol->s;
        printf("x: %f, y: %f, ADRESSE P: %p, ADRESSE: %p,  ADRESSE PREV: %p, ADRESSE NEXT: %p\n", (pol->s)->x, (pol->s)->y, pol->s , pol, pol->prev, pol->next);
        pol = pol->next;
        
        
        for(; pol->s != premierPoint ; pol = pol->next){
            printf("x: %f, y: %f, ADRESSE P: %p, ADRESSE: %p,  ADRESSE PREV: %p, ADRESSE NEXT: %p\n", (pol->s)->x, (pol->s)->y, pol->s , pol, pol->prev, pol->next);
        }
        
        printf("\n");
    }
}


int appartientCercle(Point P, Point centre, int R){
    return ((int)((P.x - centre.x)*(P.x - centre.x)) + ((P.y - centre.y)*(P.y - centre.y)) <= (int)R*R);
}

int appartientCarre(Point P, Point centre, int R){
    return ((int)fabs((P.x - centre.x)+(P.y - centre.y)) + (int)fabs((P.x - centre.x) - (P.y - centre.y)) <= (int)R*2);
}

Point getPoint(int forme, int R, Point centre){
    Point P;
    
    // Cercle
    if (forme == 1){
        do{
            P.x = centre.x - R + (rand() % (2*R));
            P.y = centre.y - R + (rand() % (2*R));
        } while(!appartientCercle(P, centre, R));
    }
    // Carré
    else{
        do{
            P.x = centre.x - R + (rand() % (2*R));
            P.y = centre.y - R + (rand() % (2*R));
        } while(!appartientCarre(P, centre, R));
    }
    
    return P;
}

void genereEnveloppe(ConvexHull *enveloppe, ListePoint *listePoint, int utilisateur, int nbPoint, int deroulement, MLV_Color *couleurs){
    enveloppe->curlen = 0;
    enveloppe->couleur = couleurs[nbConvexe % NB_COULEURS];
    
    Point P0, P1, P2;
    
    if (!utilisateur){
        enveloppe->maxlen = nbPoint;

        Point centre; centre.x = SIZE_X/2; centre.y = SIZE_Y/2;

        P0 = getPoint(1, 5, centre);
        P1 = getPoint(1, 5, centre);
        P2 = getPoint(1, 5, centre);
    }
    else{
        enveloppe->maxlen = 0;

        P0 = getPointOnClic();
        drawPoint(P0, enveloppe->couleur);
        P1 = getPointOnClic();
        drawPoint(P1, enveloppe->couleur);
        P2 = getPointOnClic();
        drawPoint(P2, enveloppe->couleur);
    }
    
    if(orientationTriangle(P0,P1,P2) >= 0){
        insereTete(listePoint, P2);
        if (!listePoint) { return; }
        insereTete(listePoint, P1);
        if (!listePoint) { return; }
        insereTete(listePoint, P0);
        if (!listePoint) { return; }
    }
    else if(orientationTriangle(P0,P2,P1) >= 0){
        insereTete(listePoint, P1);
        if (!listePoint) { return; }
        insereTete(listePoint, P2);
        if (!listePoint) { return; }
        insereTete(listePoint, P0);
        if (!listePoint) { return; }
    }

    enveloppe->maxlen = 3;

    
    ListePoint parcours = *listePoint;
    for (; parcours ; parcours = parcours->next){
        Polygon poly = newCell(&(parcours->p));
        addBefore(enveloppe->pol, poly, &enveloppe->pol);
    }

    enveloppe->curlen = 3;
    nbConvexe += 1;

    if (deroulement == 0 && !arret){
        dessineConvexe(enveloppe->pol, enveloppe->couleur, enveloppe->curlen, utilisateur);
    }
}

int insertionPoint(Point *P, Polygon *poly, ConvexHull *enveloppe, MLV_Color *couleurs){
    Point *premierPoint = (*poly)->s;
    Polygon copy = *poly;
    int orientation;
    
    do{
        Point P1 = *((copy)->s);
        Point P2 = *((copy)->next->s);

        orientation = orientationTriangle(*P, P1, P2);
        
        (copy) = (copy)->next;
    } while ((copy)->s != premierPoint && orientation >= 0);


    if (orientation < 0){
        Polygon ins = newCell(P);
        addBefore(copy, ins, &((copy)));
        copy = copy->prev ;
        enveloppe->curlen += 1;
        nettoyageAvant2(&(copy), enveloppe, couleurs);
        nettoyageArriere2(&(copy), enveloppe, couleurs);
        enveloppe->pol = copy ;
        
        return 1;
    }


    return 0;
}

void effaceEcran(){
    MLV_clear_window(MLV_COLOR_WHITE);
}

void dessineConvexe(Polygon poly, MLV_Color couleur, int curlen, int utilisateur){
    Uint8 r; Uint8 g; Uint8 b; Uint8 a;
    MLV_convert_color_to_rgba(couleur, &r, &g, &b, &a);
    MLV_Color couleurTransparente = MLV_convert_rgba_to_color(r, g, b, 64);
    MLV_Color couleurLigne = MLV_convert_rgba_to_color(r, g, b, 80);


    Point *premierPoint = poly->s;
    int tab_x[curlen]; int tab_y[curlen];
    
    int i = 0;
    do{
        Point P0 = (*poly->s);
        Point P1 = (*poly->next->s);
        if (utilisateur){
            MLV_draw_filled_circle(P0.x, P0.y, 4, couleur);
            MLV_draw_filled_circle(P1.x, P1.y, 4, couleur);
        }

        if (curlen >= 3){
            MLV_draw_line(P0.x, P0.y, P1.x, P1.y, couleurLigne);
            tab_x[i] = P0.x;
            tab_y[i] = P0.y;
        }

        poly = poly->next;
        i += 1;
    } while (poly->s != premierPoint);

    if (curlen >= 3){
        MLV_draw_filled_polygon(tab_x, tab_y, curlen, couleurTransparente);
    }

    MLV_actualise_window();
}

void dessinePointsListe(ListePoint listePoint){

    for(; listePoint; listePoint = listePoint->next){
        MLV_draw_filled_circle(listePoint->p.x, listePoint->p.y, 4, MLV_COLOR_BLACK);
    }

}

void commenceClic(ConvexHull *enveloppe, ListePoint *listePoint, MLV_Color *couleurs){
    
    ConvexHull *parcours;
    while (!(arret)){
        parcours = enveloppe;
        
        Point P = getPointOnClic();

        insereTete(listePoint, P);
        
        if (listePoint && !arret){
            traitementRecursif(&((*listePoint)->p), &enveloppe, couleurs);
            
            effaceEcran();
            for (; parcours; parcours = parcours->next){
                dessineConvexe(parcours->pol, parcours->couleur, parcours->curlen, 1);
            }
        }
    }
    
}

void nettoyageAvant2(Polygon *poly, ConvexHull *enveloppe, MLV_Color *couleurs){
    Point *sauvegarde;
    Point P = *((*poly)->s );
    Point P1 = *((*poly)->next->s );
    Point P2 = *((*poly)->next->next->s );
    
    while(orientationTriangle(P,P1,P2) <= 0 ){
        
        Polygon adresseSupp = (*poly)->next;
        
        (*poly)->next = (*poly)->next->next;
        (*poly)->next->prev = (*poly);
        
        sauvegarde = adresseSupp->s;
        free(adresseSupp);
        
        enveloppe->curlen -= 1 ;

        traitementRecursif(sauvegarde, &(enveloppe->next), couleurs);
        
        P1 = *((*poly)->next->s );
        P2 = *((*poly)->next->next->s );
    }
}

void nettoyageArriere2(Polygon *poly, ConvexHull *enveloppe, MLV_Color *couleurs){
    Point *sauvegarde;
    Point P = *((*poly)->s );
    Point P1 = *((*poly)->prev->prev->s );
    Point P2 = *((*poly)->prev->s );
    
    while(orientationTriangle(P,P1,P2) <= 0 ){
        
        Polygon adresseSupp = (*poly)->prev;

        (*poly)->prev = (*poly)->prev->prev;
        (*poly)->prev->next = (*poly);
        
        sauvegarde = adresseSupp->s;
        free(adresseSupp);

        enveloppe->curlen -= 1 ;

        traitementRecursif(sauvegarde, &(enveloppe->next), couleurs);
        
        P1 = *((*poly)->prev->prev->s );
        P2 = *((*poly)->prev->s );
    }
}

void commenceAleatoire(ConvexHull *enveloppe, ListePoint *listePoint, int nbPoint , int choix, int deroulement, MLV_Color *couleurs){
    Point centre; centre.x = SIZE_X/2; centre.y = SIZE_Y/2;

    double rayonplus = 2;

    if (deroulement == 1){
        printf("Calcul en cours...\n");
    }
    
    ConvexHull *parcours;
    for(int i = 3 ; (i < nbPoint) && !arret; i++ ){
        parcours = enveloppe;
        
        if (rayonplus <= ((SIZE_X/2) - 5)){
            rayonplus += 1; 
        }

        Point P;
        P = getPoint(choix, rayonplus, centre);
        
        insereTete(listePoint, P);
        
        if (deroulement == 0){
            traitementRecursif(&((*listePoint)->p), &enveloppe, couleurs);
            
            effaceEcran();
            
            for (; parcours; parcours = parcours->next){
                if (parcours->curlen >= 3){
                    dessineConvexe(parcours->pol, parcours->couleur, parcours->curlen, 0);
                }
            }
        }
    }

    if (deroulement == 1){
        ListePoint parcours = (*listePoint);

        for (; parcours->next; parcours = parcours->next){
            traitementRecursif(&(parcours->p), &enveloppe, couleurs);
        }

        creerFenetre();

        printf("Calcul terminé\n");

        effaceEcran();

        ConvexHull *parcoursEnv = enveloppe;
        for (; parcoursEnv; parcoursEnv = parcoursEnv->next){
            if (parcoursEnv->curlen >= 3){
                dessineConvexe(parcoursEnv->pol, parcoursEnv->couleur, parcoursEnv->curlen, 0);
            }
        }
        MLV_actualise_window();
    }

}

ListePoint alloueCellule(Point p){
    ListePoint tmp;

    tmp = (CellulePoint *) malloc(sizeof(CellulePoint));

    if (tmp != NULL){
        tmp->p = p;
        tmp->next = NULL;
    }

    return tmp;
}

ListePoint insereTete(ListePoint *liste, Point p){
    ListePoint tmp;

    tmp = alloueCellule(p);

    if (tmp != NULL){
        tmp->next = *liste;
        *liste = tmp;
    }

    return tmp;
}

void printListePoint(ListePoint liste){
    for (; liste; liste = liste->next){
        printf("x: %f, y: %f, adresse: %p\n", liste->p.x, liste->p.y, &(liste->p));
    }
}

int mouseOnButton(int x, int y, int x_button, int y_button, int widthButton, int heightButton){
    return (x >= x_button && x <= x_button + widthButton) && (y >= y_button && y <= y_button + heightButton);
}

void menu(int *mode, int *forme, int *deroulement){
    creerFenetre();

    int x, y;
    int widthButton1, heightButton1, widthButton2, heightButton2;
    
    MLV_draw_text(SIZE_X/2.3, 100, "Convex Hull", MLV_COLOR_WHITE);
    MLV_draw_text(SIZE_X/2.8, 325, "Choisissez le mode du programme", MLV_COLOR_WHITE);

    int x_button1 = SIZE_X/4.2; 
    int x_button2 = SIZE_X/1.55; 
    int y_buttons = SIZE_Y/2; 

    MLV_draw_adapted_text_box(x_button1, y_buttons, "Souris", 10, MLV_COLOR_WHITE, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_CENTER);
    MLV_draw_adapted_text_box(x_button2, y_buttons, "Aléatoire", 10, MLV_COLOR_WHITE, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_CENTER);
    MLV_get_size_of_adapted_text_box("Souris", 10, &widthButton1, &heightButton1);
    MLV_get_size_of_adapted_text_box("Aléatoire", 10, &widthButton2, &heightButton2);

    MLV_actualise_window();

    // Choix souris et aléatoire
    int button1 = 0; int button2 = 0;
    do{
        MLV_wait_mouse(&x, &y);

        if (mouseOnButton(x, y, x_button1, y_buttons, widthButton1, heightButton1)){
            button1 = 1;
        }
        else if (mouseOnButton(x, y, x_button2, y_buttons, widthButton2, heightButton2)){
            button2 = 1;
        }
    } while (!button1 && !button2);

    
    // Modes:
    // Souris = 1
    // Aleatoire = 0
    if (button1){
        *mode = 1;
        MLV_free_window();
        return;
    }
    else if (button2){
        *mode = 0;
    }

    MLV_clear_window(MLV_COLOR_BLACK);

    MLV_draw_text(SIZE_X/2.3, 100, "Convex Hull", MLV_COLOR_WHITE);
    MLV_draw_text(SIZE_X/3.5, 325, "Choisissez la forme de la distribution aléatoire", MLV_COLOR_WHITE);

    MLV_draw_adapted_text_box(x_button1, y_buttons, "Cercle", 10, MLV_COLOR_WHITE, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_CENTER);
    MLV_draw_adapted_text_box(x_button2, y_buttons, "Carré", 10, MLV_COLOR_WHITE, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_CENTER);
    MLV_get_size_of_adapted_text_box("Cercle", 10, &widthButton1, &heightButton1);
    MLV_get_size_of_adapted_text_box("Carré", 10, &widthButton2, &heightButton2);

    MLV_actualise_window();

    // Choix cercle et carré
    button1 = 0; button2 = 0;
    do{
        MLV_wait_mouse(&x, &y);

        if (mouseOnButton(x, y, x_button1, y_buttons, widthButton1, heightButton1)){
            button1 = 1;
        }
        else if (mouseOnButton(x, y, x_button2, y_buttons, widthButton2, heightButton2)){
            button2 = 1;
        }
    } while (!button1 && !button2);

    // Modes:
    // Cercle = 1
    // Carré = 2
    if (button1){
        *forme = 1;
    }
    else if (button2){
        *forme = 2;
    }

    MLV_clear_window(MLV_COLOR_BLACK);

    MLV_draw_text(SIZE_X/2.3, 100, "Convex Hull", MLV_COLOR_WHITE);
    MLV_draw_text(SIZE_X/2.9, 325, "Choisissez le mode de déroulement", MLV_COLOR_WHITE);

    MLV_draw_adapted_text_box(x_button1, y_buttons, "Point par point", 10, MLV_COLOR_WHITE, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_CENTER);
    MLV_draw_adapted_text_box(x_button2, y_buttons, "Terminal", 10, MLV_COLOR_WHITE, MLV_COLOR_WHITE, MLV_COLOR_BLACK, MLV_TEXT_CENTER);
    MLV_get_size_of_adapted_text_box("Point par point", 10, &widthButton1, &heightButton1);
    MLV_get_size_of_adapted_text_box("Terminal", 10, &widthButton2, &heightButton2);

    MLV_actualise_window();

    // Choix point par point et terminal
    while(1){
        MLV_wait_mouse(&x, &y);

        // Modes:
        // Point par point = 0
        // Terminal = 1
        if (mouseOnButton(x, y, x_button1, y_buttons, widthButton1, heightButton1)){
            *deroulement = 0;
            MLV_free_window();
            return;
        }
        else if (mouseOnButton(x, y, x_button2, y_buttons, widthButton2, heightButton2)){
            *deroulement = 1;
            MLV_free_window();
            return;
        }
    }
}

ListeConvexe alloueCelluleConvexe(Polygon pol){
    ConvexHull *newHull;

    newHull = (ConvexHull *) malloc(sizeof(ConvexHull));

    if (newHull){
        newHull->pol = pol;
        newHull->next = NULL;
    }

    return newHull;
}

void insereQueueConvexe(ListeConvexe *liste, Polygon pol){
    ConvexHull *newHull = alloueCelluleConvexe(pol);
    ListeConvexe parcours = *liste;
    
    if (newHull){
        if(*liste == NULL){
            *liste = newHull;
        }
        else{
            for(; parcours->next; parcours = parcours->next);

            parcours->next = newHull ;
        }
    }
}

void printListeConvexe(ListeConvexe listeConvexe){
    int i = 0;
    for (; listeConvexe; listeConvexe = listeConvexe->next){
        printf("CONV %d, adresse: %p\n", i, listeConvexe);
        printConvexAdresse(listeConvexe->pol);
        i += 1;
    }
}

void traitementRecursif(Point *P, ListeConvexe *listeConvexe, MLV_Color *couleurs){
    if (*listeConvexe == NULL){
        Polygon ins = newCell(P);
        insereQueueConvexe(listeConvexe, ins);

        (*listeConvexe)->curlen = 1;
        (*listeConvexe)->couleur = couleurs[nbConvexe % NB_COULEURS];
        nbConvexe += 1;

        return;
    }
    if ((*listeConvexe)->curlen == 1){
        Polygon ins = newCell(P);
        addBefore((*listeConvexe)->pol, ins, &((*listeConvexe)->pol));
        // addAfter((*listeConvexe)->pol, ins, &((*listeConvexe)->pol));

        (*listeConvexe)->curlen += 1;

        return;
    }
    if ((*listeConvexe)->curlen == 2){
        Point P0 = *((*listeConvexe)->pol->s);
        Point P1 = *((*listeConvexe)->pol->next->s);
        Point P2 = *P;
        
        Polygon ins = newCell(P);
        if(orientationTriangle(P0,P1,P2) >= 0){
            addAfter((*listeConvexe)->pol->next, ins, &((*listeConvexe)->pol->next));
        }
        else if(orientationTriangle(P0,P2,P1) >= 0){
            addBefore((*listeConvexe)->pol->next, ins, &((*listeConvexe)->pol->next));
        }

        (*listeConvexe)->curlen += 1;
        (*listeConvexe)->maxlen = 3;

        return;
    }

    int insertion = insertionPoint(P, &((*listeConvexe)->pol), *listeConvexe, couleurs);
    if (!insertion){
        traitementRecursif(P, &((*listeConvexe)->next), couleurs);
    }
}

void freeListes(ListePoint *listePoint, ListeConvexe *listeConvexe){

    ListePoint p;
    ListePoint tmp_p = *listePoint;
    while(tmp_p){
        p = tmp_p;
        tmp_p = tmp_p->next;
        free(p);
    }
    *listePoint = NULL;
    printf("Toutes les cellules de listePoint ont été libérées\n");

    // FREE CONVEXE
    
    ListeConvexe c;
    ListeConvexe tmp_c = *listeConvexe;
    
    Polygon next;
    Polygon parcours;
    while(tmp_c){
        parcours = tmp_c->pol;
        do {
            next = parcours->next;
            free(parcours);
            parcours = next;
        } while (parcours != tmp_c->pol);

        (tmp_c->pol) = NULL;

        // Free liste
        c = tmp_c;
        tmp_c = tmp_c->next;
        free(c);
        (*listeConvexe) = NULL;
    }

    printf("Tout les polygones et les enveloppes ont été libérés\n");
}