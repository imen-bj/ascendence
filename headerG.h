#ifndef GAME_HEADER_H
#define GAME_HEADER_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_image.h>

// Macro pour la fonction MIN
#define MIN(a,b) (((a)<(b))?(a):(b))

#define GROUND_LEVEL 620  // 720 - 200 from bottom
#define GRAVITY 0.5f

/*------------------------------------*
 *          STRUCTURES DE DONNÉES     *
 *------------------------------------*/
typedef struct {
    SDL_Surface *image;
    int x, y;
    int volume;
    int is_fullscreen;
    char niveau[20]; // "Menu", "Options", "Jouer", "Enigme"
} Background;

typedef struct {
    SDL_Surface *image_normal;
    SDL_Surface *image_actif;
    int x, y;
    int actif;
} Bouton;

typedef enum {
    PLAYER_IDLE,
    PLAYER_RUN,
    PLAYER_JUMP,
    PLAYER_HURT,
    PLAYER_DEATH,
    PLAYER_ATTACK  // Add attack state
} PlayerState;

typedef struct {
    SDL_Surface *sprites_R[6];    // Right-facing animations [idle,run,jump,hurt,death,attack]
    SDL_Surface *sprites_L[6];    // Left-facing animations
    int frame_counts[6];          // Number of frames for each animation
    int current_frame;            // Current frame in animation
    int animation_speed;          // Speed of animation
    PlayerState state;            // Current animation state
} PlayerAnimation;

typedef struct {
    PlayerAnimation anim;
    int x, y;
    int vie;
    int score;
    int vitesse;
    float velocity_y;  // Vertical velocity for jumping
    int is_jumping;    // Jump state
    char direction[10]; // "left", "right", "up"
    // Add movement flags
    int moving_left;
    int moving_right;
    int moving_up;
    int moving_down;
    int frame_timer;              // For controlling animation speed
    int is_attacking;  // Attack state flag
    int can_double_jump;
    int has_double_jumped;
    int dash_cooldown;
    int is_dashing;
    int combo_multiplier;
    int knockback_timer;
    float knockback_velocity_x;
} Personnage;

typedef struct {
    SDL_Surface *image;
    int x, y;
} EntiteSecondaire;

typedef struct {
    char question[100];
    char rep1[50];
    char rep2[50];
    char rep3[50];
    int numQuestSelect;      // Numéro de la question sélectionnée aléatoirement
    int numbr;               // Numéro de la bonne réponse
    SDL_Surface* questionSurface; 
    SDL_Surface* rep1Surface;
    SDL_Surface* rep2Surface;
    SDL_Surface* rep3Surface;
    SDL_Rect questionRect;
    SDL_Rect rep1Rect;
    SDL_Rect rep2Rect;
    SDL_Rect rep3Rect;
    int etat;                     // Etat de l'énigme (-1: échec, 0: non résolu, 1: succès)
} Enigme;

typedef enum {
    ENEMY_ALIVE,
    ENEMY_WOUNDED,
    ENEMY_NEUTRALIZED
} EnemyState;

typedef enum {
    ENEMY_IDLE,
    ENEMY_WALKING,
    ENEMY_ATTACKING
} EnemyAnimation;

typedef struct {
    SDL_Surface *images[4];     // 4 images pour l'animation de mouvement
    int x, y;                   // Position
    int vitesse;                // Vitesse de déplacement
    int direction;              // Direction (0: gauche, 1: droite, 2: haut, 3: bas)
    int niveau;                 // Niveau de l'ennemi (1 ou 2)
    int health;                 // Points de vie
    EnemyState state;           // État (vivant, blessé, neutralisé)
    EnemyAnimation animation;   // Type d'animation actuelle
    int frame_actuelle;         // Frame d'animation actuelle
    int trajectoire;            // Type de trajectoire (1 ou 2)
    int damage_cooldown;        // Cooldown timer for damage
    int movement_pause;         // Timer for movement pause
    float velocity_y;           // Vertical velocity
    int is_grounded;            // Ground state
    int stun_timer;    // How long enemy remains stunned
    int is_stunned;    // Stun state flag
} Enemy;

typedef struct {
    int x, y;
    int type;  // 0=coin, 1=gem, 2=powerup
    int active;
    SDL_Surface *image;
} Collectible;

typedef struct {
    int x, y, w, h;
    int type;  // 0=normal, 1=moving, 2=breakable, 3=bounce
    int active;
    float velocity_x, velocity_y;  // For moving platforms
} Platform;

typedef struct {
    SDL_Surface *sprite_L;
    SDL_Surface *sprite_R;
    int x, y;
    int health;
    int current_frame;
    int frame_timer;
    int animation_speed;
    int frame_count;
    int is_active;
    int side;  // 0 for left, 1 for right
} Boss;

/*------------------------------------*
 *         MODULE INITIALISATION      *
 *------------------------------------*/
void initialiser_SDL();
void initialiser_background(Background *bg, const char *chemin_image);
void initialiser_boutons(Bouton boutons[], int nb_boutons);
void initialiser_personnage(Personnage *pers, const char *chemin_image);
void initialiser_entites(EntiteSecondaire entites[], int nb_entites);
void initialiser_ennemis(Enemy ennemis[], int nb_ennemis, int niveau);
void initialiser_plateformes(Platform *platforms, int count);
void initialiser_collectibles(Collectible *collectibles, int count);
void initialiser_boss(Boss *boss);

/*------------------------------------*
 *         MODULE AFFICHAGE           *
 *------------------------------------*/
void afficher_background(Background *bg);
void afficher_boutons_menu(Bouton boutons[], int nb_boutons);
void afficher_boutons_options(Bouton boutons[], int nb_boutons);
void afficher_personnage(Personnage *pers);
void afficher_entites(EntiteSecondaire entites[], int nb_entites);
void afficherEnigme(SDL_Surface *screen, Enigme *enigme); // Afficher l'énigme sur l'écran
void afficher_interface_jeu(int vie, int score);
void afficher_ennemis(Enemy ennemis[], int nb_ennemis);
void afficher_plateformes(Platform *platforms, int count);
void afficher_boss(Boss *boss);

/*------------------------------------*
 *         MODULE INPUT               *
 *------------------------------------*/
void gerer_evenements_clavier(SDL_Event event, char *input);
void gerer_evenements_souris(SDL_Event event, int *click, int *motion_x, int *motion_y);
Enigme generer(char nom_du_fichier); // Fonction pour générer l'énigme à partir d'un fichier
void Decomposer(char ch, Enigme* E);  // Fonction pour décomposer une ligne de texte en éléments d'énigme
int detecterClic(SDL_Event event, Enigme *enigme); // Détecter le clic de la souris sur les réponses

/*------------------------------------*
 *         MODULE GAMEPLAY            *
 *------------------------------------*/
void deplacer_ennemis(Enemy ennemis[], int nb_ennemis);
void animer_ennemis(Enemy ennemis[], int nb_ennemis);
int detecter_collision_joueur_ennemi(Personnage *pers, Enemy *ennemi);
int detecter_collision_attaque_ennemi(Personnage *pers, Enemy *ennemi);
void gerer_sante_ennemis(Enemy ennemis[], int nb_ennemis);
void ia_mouvement_ennemi(Enemy *ennemi, Personnage *pers);
int detecter_collision_entre_ennemis(Enemy *ennemi1, Enemy *ennemi2);
int detecter_position_spawn_valide(Enemy ennemis[], int nb_ennemis, int x, int y);
int detecter_collision_position(Personnage *pers, int new_x, int new_y, Enemy *ennemi);
void update_plateformes(Platform *platforms, int count);
void check_platform_collisions(Personnage *pers, Platform *platforms, int count);
void collecter_items(Personnage *pers, Collectible *collectibles, int count);
void update_boss(Boss *boss);
int detecter_collision_joueur_boss(Personnage *pers, Boss *boss);

// Add new function prototypes
void charger_animations_personnage(Personnage *pers);
void update_animation_personnage(Personnage *pers);
void liberer_animations_personnage(Personnage *pers);
void liberer_boss(Boss *boss);
void gerer_degats_boss(Boss *boss, int degats);

#endif