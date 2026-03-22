#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>  // Ajout de la bibliothèque SDL_ttf
#include <SDL/SDL_mixer.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "zeus_defs.h"  // Inclusion des définitions pour Zeus

// Collision types
#define COLLISION_NONE 0
#define COLLISION_PLATFORM 1
#define COLLISION_INVISIBLE 2
#define COLLISION_DEATH 3
#define COLLISION_CLIMB 4
#define COLLISION_PLATFORM_ONE_WAY 5

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define MAP_WIDTH 5120
#define MAP_HEIGHT 1000

// Dimensions des sprites
#define SPRITE_WIDTH 128
#define SPRITE_HEIGHT 88
#define IDLE_FRAMES 6
#define IDLE_FRAME_WIDTH SPRITE_WIDTH

#define ATTACK_FRAMES 12
#define ATTACK_FRAME_WIDTH SPRITE_WIDTH

#define RUN_FRAMES 8
#define RUN_FRAME_WIDTH SPRITE_WIDTH

#define HURT_FRAMES 3
#define HURT_FRAME_WIDTH SPRITE_WIDTH

#define CROUCH_FRAMES 6
#define CROUCH_FRAME_WIDTH 138

// Animation speeds (ms per frame)
#define IDLE_SPEED 100
#define RUN_SPEED 60
#define ATTACK_SPEED 40
#define HURT_SPEED 100
#define JUMP_SPEED 100  // Accéléré pour une animation plus fluide (était 70)
#define FALL_SPEED 100  // Accéléré pour une animation plus fluide (était 100)

// Physics constants
#define MAX_FALL_SPEED 15
#define TILE_SIZE 32

// Add climbing animation constants
#define CLIMB_FRAMES 6  // Assuming your climbing spritesheet has 6 frames
#define CLIMB_FRAME_WIDTH 50  // Modifié pour correspondre à la largeur correcte
#define CLIMB_SPEED 100  // Animation speed for climbing

// Add the path to the falling spritesheet
#define PLAYER_FALL_R "resources/player/app1/Warrior_Fall_1-sheet.png"
#define FALL_FRAMES 3  // Adjust based on your actual spritesheet
#define FALL_FRAME_WIDTH SPRITE_WIDTH

// Add these constants at the top of your file with other game constants
#define LEVEL_MIN_X 0
#define LEVEL_MAX_X 5120   // Corrigé pour correspondre à MAP_WIDTH
#define LEVEL_MIN_Y 0
#define LEVEL_MAX_Y 1000  // Corrigé pour correspondre à MAP_HEIGHT

// Add the missing CROUCH_SPEED constant
#define CROUCH_SPEED 70  // Animation speed for crouching (match other animation speeds)

// Add the function prototype for check_collision
int check_collision(SDL_Surface* collision_map, int x, int y, int check_climbing);

// Add these missing constants at the top of your file with other constants
#define PLAYER_CROUCH_R "resources/player/app1/Warrior_Crouch_1-sheet.png"
#define JUMP_FRAMES 3  // Number of frames in jump animation
#define JUMP_FRAME_WIDTH SPRITE_WIDTH

// Add the function prototype for render_player at the top with other prototypes
void render_player();
void kill_player();
void respawn_player();
void render_enemies();
void render_typhon();
void render_health_bar();
void damage_player(int amount);
void load_level(int level_state);
void render_game_completion();
void render_score();
void check_player_boss_collision();
void spawn_fireball(float x, float y, int direction);
void update_fireballs();
void render_fireballs();

// Définitions pour l'animation de mort
#define PLAYER_DEATH_R "resources/player/app1/Warrior_Death_1-sheet.png"
#define PLAYER_DEATH_L "resources/player/app1/Warrior_Death_1-sheet.png"
#define DEATH_FRAMES 11  // Nombre de frames dans l'animation de mort
#define DEATH_FRAME_WIDTH SPRITE_WIDTH
#define DEATH_SPEED 5  // Vitesse de l'animation de mort

// Points de respawn
#define RESPAWN_X 30
#define RESPAWN_Y 700

// Définitions pour l'entité secondaire (gollux)
#define GOLLUX_ATTACK "resources/enemies/2nd entities/gollux_attack_A-sheet.png"
#define GOLLUX_HIT "resources/enemies/2nd entities/gollux_hit-sheet.png"
#define GOLLUX_IDLE "resources/enemies/2nd entities/gollux_idle-sheet.png"
#define GOLLUX_MOVE "resources/enemies/2nd entities/gollux_move-sheet.png"

#define GOLLUX_ATTACK_FRAMES 9
#define GOLLUX_HIT_FRAMES 4
#define GOLLUX_IDLE_FRAMES 5
#define GOLLUX_MOVE_FRAMES 8

#define GOLLUX_ATTACK_WIDTH 84
#define GOLLUX_HIT_WIDTH 72
#define GOLLUX_IDLE_WIDTH 64
#define GOLLUX_MOVE_WIDTH 84

#define GOLLUX_ATTACK_HEIGHT 70
#define GOLLUX_HIT_HEIGHT 70
#define GOLLUX_IDLE_HEIGHT 60
#define GOLLUX_MOVE_HEIGHT 70

#define GOLLUX_SPEED 2  // Vitesse de déplacement
#define GOLLUX_ATTACK_RANGE 100  // Distance d'attaque
#define GOLLUX_DETECTION_RANGE 300  // Distance de détection du joueur
#define GOLLUX_ATTACK_SPEED 8  // Vitesse d'animation d'attaque
#define GOLLUX_HIT_SPEED 10  // Vitesse d'animation quand touché
#define GOLLUX_IDLE_SPEED 15  // Vitesse d'animation idle
#define GOLLUX_MOVE_SPEED 10  // Vitesse d'animation de mouvement

#define MAX_ENEMIES 10  // Nombre maximum d'ennemis à l'écran

// Points d'apparition des ennemis
#define GOLLUX_SPAWN_X1 2950
#define GOLLUX_SPAWN_Y1 900
#define GOLLUX_SPAWN_X2 4050
#define GOLLUX_SPAWN_Y2 900

// Limites de mouvement des gollux
#define GOLLUX_MIN_X 2300
#define GOLLUX_MAX_X 4400

#define GOLLUX_MAX_HEALTH 300     // Points de vie maximum changed 
#define GOLLUX_KNOCKBACK 5     // Force de recul quand le gollux est touché
#define GOLLUX_HIT_COOLDOWN 40 // Cooldown en frames avant de pouvoir être touché à nouveau
#define PLAYER_DAMAGE_COOLDOWN 60 // Cooldown en frames avant que le joueur puisse être touché à nouveau
#define PLAYER_ATTACK_DAMAGE 30  // Dégâts infligés par une attaque du joueur

#define PLAYER_MAX_HEALTH 100  // Points de vie maximum du joueur
#define PLAYER_DAMAGE_AMOUNT 1  // Quantité de dégâts infligés au joueur lors d'une collision avec un ennemi
#define HEALTH_BAR_WIDTH 200     // Largeur de la barre de vie
#define HEALTH_BAR_HEIGHT 15     // Hauteur de la barre de vie
#define HEALTH_BAR_X 20          // Position X de la barre de vie
#define HEALTH_BAR_Y 20          // Position Y de la barre de vie
#define LOW_HEALTH_THRESHOLD 30  // Seuil pour considérer la santé comme basse

#define BOSS_ARENA_X 4990  // Position X à laquelle le joueur déclenche le combat contre le boss

// Définitions pour le boss Typhon
#define TYPHON_HURT "resources/enemies/typhon sprites/HURT TYPHON ANI.png"
#define TYPHON_ATTACK_CLOSE "resources/enemies/typhon sprites/typhon basic attack collision .png"
#define TYPHON_ATTACK_RANGE "resources/enemies/typhon sprites/typhon cast attack.png"
#define TYPHON_DEATH "resources/enemies/typhon sprites/typhon death ani .png"
#define TYPHON_IDLE "resources/enemies/typhon sprites/typhon not smile spritesheet.png"
#define TYPHON_WALK "resources/enemies/typhon sprites/typhon walk ani.png"

#define TYPHON_HURT_FRAMES 10
#define TYPHON_ATTACK_CLOSE_FRAMES 8
#define TYPHON_ATTACK_RANGE_FRAMES 23
#define TYPHON_DEATH_FRAMES 21
#define TYPHON_IDLE_FRAMES 4
#define TYPHON_WALK_FRAMES 8

#define TYPHON_HURT_WIDTH 100
#define TYPHON_ATTACK_CLOSE_WIDTH 118
#define TYPHON_ATTACK_RANGE_WIDTH 112
#define TYPHON_DEATH_WIDTH 150
#define TYPHON_IDLE_WIDTH 100
#define TYPHON_WALK_WIDTH 100

#define TYPHON_HURT_HEIGHT 150
#define TYPHON_ATTACK_CLOSE_HEIGHT 150
#define TYPHON_ATTACK_RANGE_HEIGHT 150
#define TYPHON_DEATH_HEIGHT 150
#define TYPHON_IDLE_HEIGHT 150
#define TYPHON_WALK_HEIGHT 150

#define TYPHON_HEALTH 10
#define TYPHON_DAMAGE_CLOSE 25
#define TYPHON_DAMAGE_RANGE 15
#define TYPHON_ATTACK_COOLDOWN 60
#define TYPHON_HURT_COOLDOWN 30
#define BOSS_ARENA_SPAWN_X 400
#define BOSS_ARENA_SPAWN_Y 700

#define DEBUG_HITBOXES 1  // Mettre à 1 pour afficher les hitboxes, 0 pour les masquer

// États pour le boss
typedef enum {
    TYPHON_STATE_IDLE,
    TYPHON_STATE_WALK,
    TYPHON_STATE_ATTACK_CLOSE,
    TYPHON_STATE_ATTACK_RANGE,
    TYPHON_STATE_HURT,
    TYPHON_STATE_DEATH
} TyphonState;

// Structure pour le boss Typhon
typedef struct {
    float x, y;
    float vx, vy;
    int health;
    int max_health;
    int active;
    int direction;
    TyphonState state;
    int current_frame;
    int frame_timer;
    int attack_cooldown;
    int hurt_cooldown;
    SDL_Surface* sprite_hurt;
    SDL_Surface* sprite_attack_close;
    SDL_Surface* sprite_attack_range;
    SDL_Surface* sprite_death;
    SDL_Surface* sprite_idle;
    SDL_Surface* sprite_walk;
    
    SDL_Surface* sprite_hurt_flipped;
    SDL_Surface* sprite_attack_close_flipped;
    SDL_Surface* sprite_attack_range_flipped;
    SDL_Surface* sprite_death_flipped;
    SDL_Surface* sprite_idle_flipped;
    SDL_Surface* sprite_walk_flipped;
} Typhon;

// Variables globales
Typhon typhon;
int in_boss_arena = 0;
SDL_Surface* arena_background;
SDL_Surface* arena_collision_map;

// Variables pour le système de score
int player_score = 0;           // Score total du joueur
int enemies_killed = 0;         // Nombre d'ennemis tués
int player_deaths = 0;          // Nombre de morts du joueur
Uint32 boss_battle_start_time = 0; // Moment où le combat de boss a commencé
Uint32 boss_battle_end_time = 0;   // Moment où le combat de boss s'est terminé
int boss_defeated = 0;          // Si le boss a été vaincu

typedef enum {
    ANIM_IDLE,
    ANIM_RUN,
    ANIM_ATTACK,
    ANIM_JUMP,
    ANIM_HURT,
    ANIM_DEATH,
    ANIM_DASH,
    ANIM_CROUCH,
    ANIM_SLIDE,
    ANIM_CLIMB  // Add the climbing animation state
} AnimationState;

typedef struct {
    AnimationState state;
    int current_frame;
    int frame_timer;
    int animation_speed;
    int frame_counts[10];  // Pour chaque état d'animation (mis à jour pour inclure ANIM_DEATH)
} Animation;

// Définition des chemins des sprites du joueur
#define PLAYER_IDLE_R "resources/player/app1/Warrior_Idle_1-sheet.png"
#define PLAYER_IDLE_L "resources/player/app1/Warrior_Idle_1-sheet.png"   // On utilisera la version retournée pour la gauche
#define PLAYER_RUN_R "resources/player/app1/Warrior_Run_1-sheet.png"
#define PLAYER_RUN_L "resources/player/app1/Warrior_Run_1-sheet.png"    // On utilisera la version retournée pour la gauche
#define PLAYER_JUMP_R "resources/player/app1/Warrior_Jump_1-sheet.png"
#define PLAYER_JUMP_L "resources/player/app1/Warrior_Jump_1-sheet.png"  // On utilisera la version retournée pour la gauche
#define PLAYER_ATTACK_R "resources/player/app1/Warrior_Attack_1-sheet.png"
#define PLAYER_ATTACK_L "resources/player/app1/Warrior_Attack_1-sheet.png" // On utilisera la version retournée pour la gauche
#define PLAYER_CLIMB "resources/player/app1/Warrior-Ladder-Grab_1-sheet.png"  // Path to ladder spritesheet

// Taille du joueur (adaptée aux sprites 128x88)
#define PLAYER_WIDTH SPRITE_WIDTH
#define PLAYER_HEIGHT SPRITE_HEIGHT
#define PLAYER_SPEED 6
#define JUMP_VELOCITY -15  // Ajusté pour la nouvelle taille
#define GRAVITY 0.8f      // Ajusté pour la nouvelle taille

// Direction
#define RIGHT 1
#define LEFT -1

// Couleurs de collision
#define PLATFORM_COLOR 0x03fe03
#define INVISIBLE_COLOR 0xfef603

#define FONT_PATH "resources/font.ttf"  // Chemin vers la police de caractères
#define FONT_SIZE 16                   // Taille de la police

// Structure du jeu
typedef struct {
    SDL_Surface* screen;
    SDL_Surface* background;
    SDL_Surface* collision_map;
    float camera_x;
    float camera_y;
    TTF_Font* font;        // Ajout de la police pour le rendu du texte
} Game;

Game game;

// Structure simplifiée pour le joueur
typedef struct {    
    float x, y;
    float vx, vy;
    int on_ground;
    int direction;
    int is_jumping;
    int is_falling;  // Add falling state
    int is_running;
    int is_attacking;
    int is_crouching;
    int is_climbing;
    int is_dying;    // Nouvel état pour la mort
    int death_frame; // Pour suivre la progression de l'animation de mort
    int death_timer; // Durée avant respawn
    int crouch_state;  // 0=not crouching, 1=crouching down, 2=holding crouch, 3=standing up
    int is_visible;
    int current_frame;
    int frame_timer;
    int health;      // Points de vie actuels du joueur
    int max_health;  // Points de vie maximum du joueur
    int is_hurt;     // Indique si le joueur est actuellement blessé (pour l'effet de clignotement)
    int hurt_timer;  // Timer pour l'effet de clignotement
    SDL_Surface* sprite_idle_R;
    SDL_Surface* sprite_idle_L;
    SDL_Surface* sprite_run_R;
    SDL_Surface* sprite_run_L;
    SDL_Surface* sprite_jump_R;
    SDL_Surface* sprite_jump_L;
    SDL_Surface* sprite_fall_R;  // Add falling spritesheet
    SDL_Surface* sprite_fall_L;
    SDL_Surface* sprite_attack_R;
    SDL_Surface* sprite_attack_L;
    SDL_Surface* sprite_crouch_R;
    SDL_Surface* sprite_crouch_L;
    SDL_Surface* sprite_climb;
    SDL_Surface* sprite_death_R;  // Animation de mort
    SDL_Surface* sprite_death_L;  // Animation de mort (côté gauche)
} Player;

Player player;

// Add J key state variable instead of L
#define KEY_PRESSED 1
#define KEY_RELEASED 0

int key_left = KEY_RELEASED;
int key_right = KEY_RELEASED;
int key_up = KEY_RELEASED;
int key_down = KEY_RELEASED;
int key_k = KEY_RELEASED;
int key_space = KEY_RELEASED;
int key_j = KEY_RELEASED;  // Change from L to J key for climbing
int key_b = KEY_RELEASED; // Touche pour déclencher manuellement l'arène du boss

// États possibles pour l'ennemi
typedef enum {
    ENEMY_IDLE,
    ENEMY_MOVE,
    ENEMY_ATTACK,
    ENEMY_HIT,
    ENEMY_DEAD
} EnemyState;

// Structure pour représenter une entité ennemie
typedef struct {
    float x, y;
    float vx, vy;
    int direction;         // 1 = droite, -1 = gauche
    int health;            // Points de vie
    int on_ground;
    int active;            // Si l'ennemi est actif (visible et en jeu)
    int hit_cooldown;      // Temps avant de pouvoir être touché à nouveau
    int knockback_counter; // Compteur pour l'effet de recul
    
    EnemyState state;      // État actuel
    int current_frame;     // Frame actuelle de l'animation
    int frame_timer;       // Temporisateur pour l'animation
    
    SDL_Surface* sprite_attack;
    SDL_Surface* sprite_hit;
    SDL_Surface* sprite_idle;
    SDL_Surface* sprite_move;
    
    SDL_Surface* sprite_attack_flipped;
    SDL_Surface* sprite_hit_flipped;
    SDL_Surface* sprite_idle_flipped;
    SDL_Surface* sprite_move_flipped;
} Enemy;

// Tableau d'ennemis
Enemy enemies[MAX_ENEMIES];
int num_enemies = 0;

int player_damage_cooldown = 0; // Cooldown global pour éviter les dégâts excessifs au joueur

#define LEVEL_STATE_MAIN_PARKOUR 0    // Level 1 part 1 : parcours initial
#define LEVEL_STATE_BOSS_ARENA 1     // Level 1 part 2 : arène du boss
#define LEVEL_STATE_ZEUS_ARENA 2      // Level 2 : arène du boss Zeus

// Déclaration de la variable pour suivre l'état du niveau
int current_level_state = LEVEL_STATE_MAIN_PARKOUR;

#define BACKGROUND_SPEED 4

// Constantes pour le système de score 
#define ENEMY_KILL_POINTS 100     // Points gagnés pour chaque ennemi tué
#define PLAYER_DEATH_PENALTY 200  // Points perdus à chaque mort du joueur
#define BOSS_KILL_BASE_POINTS 5000 // Points de base pour avoir tué le boss
#define BOSS_TIME_BONUS_MAX 10000  // Bonus maximum pour avoir tué le boss rapidement
#define BOSS_TIME_BONUS_THRESHOLD 180 // Temps en secondes à partir duquel le bonus commence à diminuer

#define FIREBALL_SPRITE "resources/enemies/fire.png"
#define FIREBALL_WIDTH 18
#define FIREBALL_HEIGHT 15
#define FIREBALL_FRAMES 3
#define FIREBALL_SPEED 8  // Vitesse de déplacement horizontal
#define FIREBALL_DAMAGE 15  // Dégâts infligés par la boule de feu
#define MAX_FIREBALLS 5  // Nombre maximum de boules de feu à l'écran
#define FIREBALL_DELAY 30  // Délai entre les tirs consécutifs

// Structure pour un projectile
typedef struct {
    float x, y;
    float vx, vy;
    int active;
    int current_frame;
    int frame_timer;
    int animation_loops;  // Nombre de fois que l'animation a bouclé
    int direction;        // Direction (1 = droite, -1 = gauche)
} Fireball;

// Variables globales
Typhon typhon;
Fireball fireballs[MAX_FIREBALLS];  // Tableau de boules de feu
SDL_Surface* fireball_sprite;       // Sprite des boules de feu (direction droite)
SDL_Surface* fireball_sprite_flipped; // Sprite des boules de feu (direction gauche)
int fireball_timer = 0;             // Timer pour espacer les tirs consécutifs

// Constantes pour le fondu
#define FADE_DURATION 60              // Durée du fondu en frames
#define FADE_STATE_NONE 0             // Pas de fondu en cours
#define FADE_STATE_OUT 1              // Fondu vers le noir
#define FADE_STATE_IN 2               // Fondu depuis le noir

int fade_state = FADE_STATE_NONE;     // État actuel du fondu
int fade_timer = 0;                   // Timer pour le fondu
int transition_to_level = -1;         // Niveau vers lequel transitionner après le fondu

void init_game() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    // Initialiser SDL_ttf
    if (TTF_Init() < 0) {
        printf("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        return;
    }

    game.screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE);
    if (!game.screen) {
        printf("Screen could not be created! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    // Charger la police
    game.font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    if (!game.font) {
        printf("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        printf("Attempted to load font from: %s\n", FONT_PATH);
        // On continue même sans police, le score sera juste affiché en console
    }

    game.background = IMG_Load("resources/cs.png");
    if (!game.background) {
        printf("Could not load background image! SDL_Error: %s\n", SDL_GetError());
        return;
    }
    
    printf("Background loaded - Size: %d x %d pixels\n", game.background->w, game.background->h);
    
    // Vérifier si le background est de la bonne taille
    if (game.background->w < MAP_WIDTH || game.background->h < MAP_HEIGHT) {
        printf("WARNING: Background image (%d x %d) is smaller than the defined map size (%d x %d)!\n",
               game.background->w, game.background->h, MAP_WIDTH, MAP_HEIGHT);
    }
    
    game.collision_map = IMG_Load("resources/cscm1.png");
    if (!game.collision_map) {
        printf("Could not load collision map! SDL_Error: %s\n", SDL_GetError());
        return;
    }
    
    printf("Collision map loaded - Size: %d x %d pixels\n", game.collision_map->w, game.collision_map->h);
    
    // Initialize camera at the center of the level
    game.camera_x = 0;
    game.camera_y = 0;
    
    // Print debug information
    printf("Game initialized:\n");
    printf("Screen size: %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
    printf("Map size: %dx%d\n", MAP_WIDTH, MAP_HEIGHT);
    printf("Initial camera position: (%.2f, %.2f)\n", game.camera_x, game.camera_y);
}

// Fonction pour extraire une seule frame d'un spritesheet
SDL_Surface* extract_frame(SDL_Surface* spritesheet, int frame_index, int frame_width) {
    SDL_Surface* frame = SDL_CreateRGBSurface(
        spritesheet->flags,
        frame_width,
        spritesheet->h,
        spritesheet->format->BitsPerPixel,
        spritesheet->format->Rmask,
        spritesheet->format->Gmask,
        spritesheet->format->Bmask,
        spritesheet->format->Amask
    );
    
    if (!frame) {
        return NULL;
    }
    
    SDL_Rect src_rect = {
        frame_index * frame_width,
        0,
        frame_width,
        spritesheet->h
    };
    
    SDL_Rect dest_rect = {
        0, 0, frame_width, spritesheet->h
    };
    
    SDL_BlitSurface(spritesheet, &src_rect, frame, &dest_rect);
    return frame;
}

SDL_Surface* load_and_flip_png(const char* filepath) {
    SDL_Surface* loaded = IMG_Load(filepath);
    if (!loaded) {
        printf("IMG_Load failed for %s: %s\n", filepath, IMG_GetError());
        return NULL;
    }
    
    printf("Chargement de %s: %dx%d pixels, format: %d bits\n", 
           filepath, loaded->w, loaded->h, loaded->format->BitsPerPixel);
    
    SDL_Surface* optimized = SDL_DisplayFormatAlpha(loaded);
    SDL_FreeSurface(loaded);
    if (!optimized) {
        printf("SDL_DisplayFormatAlpha failed: %s\n", SDL_GetError());
        return NULL;
    }
    
    SDL_Surface* flipped = SDL_CreateRGBSurface(
        optimized->flags, optimized->w, optimized->h, optimized->format->BitsPerPixel,
        optimized->format->Rmask, optimized->format->Gmask,
        optimized->format->Bmask, optimized->format->Amask
    );
    
    if (!flipped) {
        SDL_FreeSurface(optimized);
        printf("SDL_CreateRGBSurface failed: %s\n", SDL_GetError());
        return NULL;
    }
    
    // Vérification supplémentaire que les surfaces sont valides
    if (optimized->pixels == NULL || flipped->pixels == NULL) {
        printf("ERREUR: Pixels non valides dans optimized ou flipped\n");
        if (optimized) SDL_FreeSurface(optimized);
        if (flipped) SDL_FreeSurface(flipped);
        return NULL;
    }
    
    SDL_LockSurface(optimized);
    SDL_LockSurface(flipped);
    
    Uint8* src_pixels = (Uint8*)optimized->pixels;
    Uint8* dst_pixels = (Uint8*)flipped->pixels;
    int bpp = optimized->format->BytesPerPixel;
    
    // Debug info
    printf("Création du sprite flippé: taille %dx%d, bpp: %d\n", 
           optimized->w, optimized->h, bpp);
    
    for (int y = 0; y < optimized->h; ++y) {
        for (int x = 0; x < optimized->w; ++x) {
            Uint8* src_pixel = src_pixels + y * optimized->pitch + x * bpp;
            Uint8* dst_pixel = dst_pixels + y * flipped->pitch + (optimized->w - 1 - x) * bpp;
            
            // Copier les pixels de droite à gauche pour inverser l'image
            memcpy(dst_pixel, src_pixel, bpp);
        }
    }
    
    SDL_UnlockSurface(optimized);
    SDL_UnlockSurface(flipped);
    SDL_FreeSurface(optimized);
    
    printf("Sprite flippé créé avec succès: %dx%d pixels\n", 
           flipped->w, flipped->h);
    
    return flipped;
}

void init_player() {
    player.x = 30  ;// Centré horizontalement
    player.y = 700 ; // Centré verticalement
    player.vx = 0;
    player.vy = 0;
    player.on_ground = 0;
    player.direction = 1;
    player.is_jumping = 0;
    player.is_falling = 0;
    player.is_running = 0;
    player.is_attacking = 0;
    player.is_crouching = 0;
    player.is_climbing = 0;
    player.is_dying = 0;
    player.death_frame = 0;
    player.death_timer = 0;
    player.crouch_state = 0;
    player.is_visible = 1;
    player.current_frame = 0;
    player.frame_timer = 0;
    player.health = PLAYER_MAX_HEALTH;  // Initialiser la santé au maximum
    player.max_health = PLAYER_MAX_HEALTH;
    player.is_hurt = 0;
    player.hurt_timer = 0;
    
    // Load all spritesheets with better error handling
    player.sprite_idle_R = IMG_Load(PLAYER_IDLE_R);
    if (!player.sprite_idle_R) {
        printf("Error loading idle sprite: %s\n", IMG_GetError());
        printf("Path: %s\n", PLAYER_IDLE_R);
    }
    
    player.sprite_idle_L = load_and_flip_png(PLAYER_IDLE_R);
    
    player.sprite_run_R = IMG_Load(PLAYER_RUN_R);
    if (!player.sprite_run_R) {
        printf("Error loading run sprite: %s\n", IMG_GetError());
        printf("Path: %s\n", PLAYER_RUN_R);
    }
    
    player.sprite_run_L = load_and_flip_png(PLAYER_RUN_R);
    
    player.sprite_jump_R = IMG_Load(PLAYER_JUMP_R);
    if (!player.sprite_jump_R) {
        printf("Error loading jump sprite: %s\n", IMG_GetError());
        printf("Path: %s\n", PLAYER_JUMP_R);
    }
    
    player.sprite_jump_L = load_and_flip_png(PLAYER_JUMP_R);
    
    player.sprite_fall_R = IMG_Load(PLAYER_FALL_R);
    if (!player.sprite_fall_R) {
        printf("Error loading fall sprite: %s\n", IMG_GetError());
        printf("Path: %s\n", PLAYER_FALL_R);
    }
    
    player.sprite_fall_L = load_and_flip_png(PLAYER_FALL_R);
    
    player.sprite_attack_R = IMG_Load(PLAYER_ATTACK_R);
    if (!player.sprite_attack_R) {
        printf("Error loading attack sprite: %s\n", IMG_GetError());
        printf("Path: %s\n", PLAYER_ATTACK_R);
    }
    
    player.sprite_attack_L = load_and_flip_png(PLAYER_ATTACK_R);
    
    player.sprite_crouch_R = IMG_Load(PLAYER_CROUCH_R);
    if (!player.sprite_crouch_R) {
        printf("Error loading crouch sprite: %s\n", IMG_GetError());
        printf("Path: %s\n", PLAYER_CROUCH_R);
    }
    
    player.sprite_crouch_L = load_and_flip_png(PLAYER_CROUCH_R);
    
    player.sprite_climb = IMG_Load(PLAYER_CLIMB);
    if (!player.sprite_climb) {
        printf("Error loading climb sprite: %s\n", IMG_GetError());
        printf("Path: %s\n", PLAYER_CLIMB);
    }
    
    player.sprite_death_R = IMG_Load(PLAYER_DEATH_R);
    if (!player.sprite_death_R) {
        printf("Error loading death sprite: %s\n", IMG_GetError());
        printf("Path: %s\n", PLAYER_DEATH_R);
    }
    
    player.sprite_death_L = IMG_Load(PLAYER_DEATH_L);
    if (!player.sprite_death_L) {
        printf("Error loading death sprite: %s\n", IMG_GetError());
        printf("Path: %s\n", PLAYER_DEATH_L);
    }
}

void update_player_state() {
    player.vx = 0;
    // Ne pas permettre de mouvement pendant l'attaque
    if (player.is_attacking) return;

    // Check if player is touching a climbable surface
    int touching_climbable = 0;
    int climb_check_center = check_collision(game.collision_map, 
                                    (int)(player.x + SPRITE_WIDTH/2), 
                                    (int)(player.y + SPRITE_HEIGHT/2), 
                                    1); // Explicitly check for climbing
                                    
    int climb_check_top = check_collision(game.collision_map, 
                                 (int)(player.x + SPRITE_WIDTH/2), 
                                 (int)(player.y + 10), 
                                 1); // Explicitly check for climbing
                                 
    int climb_check_bottom = check_collision(game.collision_map, 
                                    (int)(player.x + SPRITE_WIDTH/2), 
                                    (int)(player.y + SPRITE_HEIGHT - 10), 
                                    1); // Explicitly check for climbing
    
    if (climb_check_center == COLLISION_CLIMB || 
        climb_check_top == COLLISION_CLIMB || 
        climb_check_bottom == COLLISION_CLIMB) {
        touching_climbable = 1;
    }

    // Start or continue climbing when J is pressed and touching climbable surface
    if (key_j == KEY_PRESSED && touching_climbable) {
        player.is_climbing = 1;
    } else if (!touching_climbable || key_j == KEY_RELEASED) {
        // Stop climbing if no longer touching climbable surface or J is released
        player.is_climbing = 0;
    }

    // Left/Right movement
    if (key_left == KEY_PRESSED) {
        player.vx = -PLAYER_SPEED;
        player.direction = -1;
    }
    if (key_right == KEY_PRESSED) {
        player.vx = PLAYER_SPEED;
        player.direction = 1;
    }
    
    // Handle vertical movement during climbing
    if (player.is_climbing) {
        if (key_up == KEY_PRESSED) {
            player.vy = -3; // Slower climbing speed
            player.on_ground = 0;
        } else if (key_down == KEY_PRESSED) {
            player.vy = 3;
            player.on_ground = 0;
        } else {
            // If climbing but not pressing up/down, stop vertical movement
            player.vy = 0;
        }
        
        // If climbing, can still move horizontally but slower
        if (player.vx != 0) {
            player.vx = player.vx * 0.5; // Slower horizontal movement while climbing
        }
    }
    
    // Jump (with SPACE) - only if on ground or climbing
    if (key_space == KEY_PRESSED && (player.on_ground || player.is_climbing)) {
        player.vy = JUMP_VELOCITY;
        player.on_ground = 0;
        player.is_jumping = 1;
        player.is_climbing = 0; // Stop climbing when jumping
    }
    
    // Crouch - only if on ground and not climbing
    if (key_down == KEY_PRESSED && player.on_ground && !player.is_climbing) {
        player.is_crouching = 1;
        player.vx = 0; // Can't move while crouching
    } else {
        player.is_crouching = 0;
    }

    // Attack (only if not crouching and not climbing)
    if (key_k == KEY_PRESSED && !player.is_crouching && !player.is_climbing) {
        player.is_attacking = 1;
    }
    
    // Update running state
    player.is_running = (player.vx != 0 && player.on_ground && !player.is_crouching && !player.is_climbing) ? 1 : 0;
}

void update_animation() {
    player.frame_timer += 2; // Incrémente de 2 au lieu de 1 pour aller plus vite
    
    // Déterminer la vitesse d'animation selon l'état
    int frame_speed = IDLE_SPEED;
    int max_frames = IDLE_FRAMES;
    
    if (player.is_attacking) {
        frame_speed = ATTACK_SPEED;
        max_frames = ATTACK_FRAMES;
    } else if (!player.on_ground) {
        frame_speed = IDLE_SPEED;
        max_frames = IDLE_FRAMES;
    } else if (player.is_running) {
        frame_speed = RUN_SPEED;
        max_frames = RUN_FRAMES;
    } else {
        frame_speed = IDLE_SPEED;
        max_frames = IDLE_FRAMES;
    }
    
    // Mettre à jour la frame actuelle (encore plus rapide)
    if (player.frame_timer >= frame_speed) {
        player.frame_timer = 0;
        player.current_frame = (player.current_frame + 1) % max_frames;
        
        // Reset attack state after animation completes
        if (player.is_attacking && player.current_frame == 0) {
            player.is_attacking = 0;
        }
    }
}

// Alternative: Utilisons un timer basé sur le temps réel SDL_GetTicks()
void update_animation_ticks() {
    // Obtenir le temps actuel en millisecondes
    Uint32 current_time = SDL_GetTicks();
    
    // Déterminer la vitesse d'animation selon l'état
    int frame_speed = IDLE_SPEED;
    int max_frames = IDLE_FRAMES;
    
    if (player.is_attacking) {
        frame_speed = ATTACK_SPEED;
        max_frames = ATTACK_FRAMES;
    } else if (!player.on_ground) {
        frame_speed = IDLE_SPEED;
        max_frames = IDLE_FRAMES;
    } else if (player.is_running) {
        frame_speed = RUN_SPEED;
        max_frames = RUN_FRAMES;
    } else {
        frame_speed = IDLE_SPEED;
        max_frames = IDLE_FRAMES;
    }
    
    // Vérifier si assez de temps s'est écoulé pour changer d'image

    static Uint32 last_frame_time = 0;    if (current_time - last_frame_time > (Uint32)frame_speed) {
        last_frame_time = current_time;
        player.current_frame = (player.current_frame + 1) % max_frames;
        
        // Reset attack state after animation completes
        if (player.is_attacking && player.current_frame == 0) {
            player.is_attacking = 0;
        }
    }
}

void update_camera(float target_x, float target_y) {
    // Center the camera on the player
    float desired_camera_x = target_x - SCREEN_WIDTH / 2;
    float desired_camera_y = target_y - SCREEN_HEIGHT / 2;
    
    // Limites de caméra spéciales pour l'arène du boss
    if (current_level_state == LEVEL_STATE_BOSS_ARENA) {
        // Limites horizontales pour l'arène du boss (de 0 à 2550)
        if (desired_camera_x < 0) {
            game.camera_x = 0;
            if (SDL_GetTicks() % 60 == 0) { // Réduire la fréquence des messages
                printf("Caméra en limite gauche (x=0)\n");
            }
        }
        else if (desired_camera_x > 2550) {
            game.camera_x = 2550;
            if (SDL_GetTicks() % 60 == 0) { // Réduire la fréquence des messages
                printf("Caméra en limite droite (x=2550)\n");
            }
        }
        else {
            game.camera_x = desired_camera_x;
            if (SDL_GetTicks() % 300 == 0) { // Messages moins fréquents pour la zone normale
                printf("Caméra en mouvement normal (x=%.1f)\n", game.camera_x);
            }
        }
        
        // Fixer la caméra verticalement
        game.camera_y = 280.0;
        return;
    }
    
    // Strictly enforce camera boundaries for main level
    // Left boundary
    if (desired_camera_x < LEVEL_MIN_X) {
        game.camera_x = LEVEL_MIN_X;
    }
    // Right boundary
    else if (desired_camera_x + SCREEN_WIDTH > LEVEL_MAX_X) {
        game.camera_x = LEVEL_MAX_X - SCREEN_WIDTH;
    }
    // Normal case
    else {
        game.camera_x = desired_camera_x;
    }
    
    // Top boundary
    if (desired_camera_y < LEVEL_MIN_Y) {
        game.camera_y = LEVEL_MIN_Y;
    }
    // Bottom boundary
    else if (desired_camera_y + SCREEN_HEIGHT > LEVEL_MAX_Y) {
        game.camera_y = LEVEL_MAX_Y - SCREEN_HEIGHT;
    }
    // Normal case
    else {
        game.camera_y = desired_camera_y;
    }
    
    // Extra safety check to ensure camera is always within bounds
    if (game.camera_x < LEVEL_MIN_X) game.camera_x = LEVEL_MIN_X;
    if (game.camera_y < LEVEL_MIN_Y) game.camera_y = LEVEL_MIN_Y;
    if (game.camera_x > LEVEL_MAX_X - SCREEN_WIDTH) game.camera_x = LEVEL_MAX_X - SCREEN_WIDTH;
    if (game.camera_y > LEVEL_MAX_Y - SCREEN_HEIGHT) game.camera_y = LEVEL_MAX_Y - SCREEN_HEIGHT;
}

Uint32 get_pixel(SDL_Surface* surface, int x, int y) {
    // Vérification des limites
    if (x < 0 || x >= surface->w || y < 0 || y >= surface->h) {
        return 0;
    }
    
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
    
    switch (bpp) {
        case 1:
            return *p;
        case 2:
            return *(Uint16*)p;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
        case 4:
            return *(Uint32*)p;
        default:
            return 0;
    }
}

int check_collision(SDL_Surface* collision_map, int x, int y, int check_climbing) {
    // Dans l'arène du boss Typhon, utiliser une logique de collision simplifiée
    if (current_level_state == LEVEL_STATE_BOSS_ARENA) {
        // Sol simple à y=890
        if (y >= 890) {
            return COLLISION_PLATFORM;
        }
        
        // Murs aux bords de l'arène
        if (x < 20) {
            return COLLISION_INVISIBLE; // Mur invisible à gauche
        }
        if (x > MAP_WIDTH - 20) {
            return COLLISION_INVISIBLE; // Mur invisible à droite
        }
        
        // Pas d'autres collisions dans l'arène du boss
        return COLLISION_NONE;
    }
    // Dans l'arène de Zeus, utiliser une logique encore plus simple
    else if (current_level_state == LEVEL_STATE_ZEUS_ARENA) {
        // Sol simple à 100px de hauteur depuis le bas de l'écran
        if (y >= MAP_HEIGHT - 100) {
            return COLLISION_PLATFORM;
        }
        
        // Murs aux bords de l'arène
        if (x < 20) {
            return COLLISION_INVISIBLE; // Mur invisible à gauche
        }
        if (x > MAP_WIDTH - 20) {
            return COLLISION_INVISIBLE; // Mur invisible à droite
        }
        
        // Pas d'autres collisions dans l'arène de Zeus
        return COLLISION_NONE;
    }
    
    // Logique de collision standard pour le reste du jeu
    // Boundary check
    if (x < 0 || x >= collision_map->w || y < 0 || y >= collision_map->h) {
        return COLLISION_NONE;
    }
    
    // Get pixel value at the given coordinates
    Uint32 pixel = get_pixel(collision_map, x, y);
    Uint8 r, g, b;
    SDL_GetRGB(pixel, collision_map->format, &r, &g, &b);

    // Debug - afficher la couleur détectée à des points clés
    if (x == (int)(player.x + SPRITE_WIDTH/2) && y == (int)(player.y + SPRITE_HEIGHT/2)) {
        printf("Couleur au centre du joueur: %02x%02x%02x\n", r, g, b);
    }

    if (r == 3 && g == 254 && b == 3) {
        return COLLISION_PLATFORM_ONE_WAY;  // Green (03fe03) - one-way platform
    } else if (r == 254 && g == 3 && b == 3) {
        return COLLISION_DEATH;  // Red (fe0303) - death
    } else if (r == 254 && g == 246 && b == 3) {
        return COLLISION_PLATFORM;  // Yellow (fef603) - invisible wall
    } else if (r == 3 && g == 27 && b == 254) {  
        return check_climbing ? COLLISION_CLIMB : COLLISION_NONE;  // Blue (031bfe) - climb
    } else {
        return COLLISION_NONE;
    }
}

void check_player_visibility() {
    // Ne rien faire, le joueur est toujours visible
    player.is_visible = 1;
}

void update_player() {
    // Gestion du cooldown de dégâts du joueur
    if (player_damage_cooldown > 0) {
        player_damage_cooldown--;
    }
    
    // Gestion de l'effet de clignotement quand le joueur est blessé
    if (player.is_hurt) {
        player.hurt_timer--;
        if (player.hurt_timer <= 0) {
            player.is_hurt = 0;
        }
    }
    
    // Si le joueur est en train de mourir, gérer l'animation de mort
    if (player.is_dying) {
        player.death_timer++;
        
        // Faire avancer l'animation de mort
        if (player.death_timer >= DEATH_SPEED) {
            player.death_timer = 0;
            
            // Incrémenter seulement si on n'a pas atteint la dernière frame
            if (player.death_frame < DEATH_FRAMES - 1) {
                player.death_frame++;
            } 
            // Ajouter un délai supplémentaire après la dernière frame
            else if (player.death_frame == DEATH_FRAMES - 1) {
                // Attendre un peu sur la dernière frame (équivalent à environ 1 seconde)
                static int last_frame_delay = 0;
                last_frame_delay++;
                
                // Respawn après le délai
                if (last_frame_delay >= 20) {
                    last_frame_delay = 0;
                    respawn_player();
                    return;
                }
            }
        }
        
        // Ne pas traiter la physique pendant l'animation de mort
        return;
    }
    
    // Check if still on ground
    if (player.on_ground) {
        // Check if there's still ground beneath both feet
        int ground_check_left = check_collision(game.collision_map, 
                                              (int)(player.x + 10 + 40),
                                              (int)(player.y + SPRITE_HEIGHT + 1),
                                              0);
        int ground_check_right = check_collision(game.collision_map, 
                                               (int)(player.x + SPRITE_WIDTH - 10 - 40),
                                               (int)(player.y + SPRITE_HEIGHT + 1),
                                               0);
        
        // If no ground beneath feet, start falling
        if ((ground_check_left == COLLISION_NONE || ground_check_left == COLLISION_PLATFORM_ONE_WAY) && 
            (ground_check_right == COLLISION_NONE || ground_check_right == COLLISION_PLATFORM_ONE_WAY)) {
            player.on_ground = 0;
            player.vy = 0; // Start with zero velocity when walking off edge
        }
    }
    
    // Apply gravity only if not climbing
    if (!player.on_ground && !player.is_climbing) {
        player.vy += GRAVITY;
        if (player.vy > MAX_FALL_SPEED) {
            player.vy = MAX_FALL_SPEED;
        }
        
        // Set falling state based on vertical velocity
        if (player.vy > 2.0) {  // Petit seuil pour éviter les transitions trop fréquentes
            if (player.is_jumping && !player.is_falling) {
                // Transition de saut vers chute - conserver la dernière frame du saut
                // pour commencer l'animation de chute à partir de cette position
                player.is_falling = 1;
                player.is_jumping = 0;
                player.current_frame = 0;  // Première frame de l'animation de chute
            } else {
                player.is_falling = 1;
                player.is_jumping = 0;
            }
        } else if (player.vy < 0) {
            if (!player.is_jumping && player.is_falling) {
                // Transition de chute vers saut - situation rare mais possible
                // (ex: rebond sur trampoline)
                player.is_falling = 0;
                player.is_jumping = 1;
                player.current_frame = 0;  // Première frame de l'animation de saut
            } else {
                player.is_falling = 0;
                player.is_jumping = 1;
            }
        }
        // Dans la zone de transition (entre 0 et 2.0), on conserve l'état actuel
    } else {
        player.is_falling = 0;
        player.is_jumping = 0;
    }

    // Calculate new position with velocity
    float new_x = player.x + player.vx;
    float new_y = player.y + player.vy;
    
    // Limites spéciales pour l'arène du boss
    if (current_level_state == LEVEL_STATE_BOSS_ARENA) {
        // Limite horizontale du joueur dans l'arène du boss
        if (new_x > 3790) {
            new_x = 3790;
            player.vx = 0;
            if (player.x != 3790) { // Éviter de spammer la console
                printf("Joueur a atteint la limite horizontale droite à x=3790\n");
            }
        }
        // Limite horizontale gauche pour l'arène du boss
        else if (new_x < 200) {
            new_x = 200;
            player.vx = 0;
            if (player.x != 200) { // Éviter de spammer la console
                printf("Joueur a atteint la limite horizontale gauche à x=200\n");
            }
        }
    }
    
    // Apply strict horizontal boundary restrictions
    if (new_x < LEVEL_MIN_X) {
        new_x = LEVEL_MIN_X;
        player.vx = 0;
    } else if (new_x + SPRITE_WIDTH > LEVEL_MAX_X) {
        new_x = LEVEL_MAX_X - SPRITE_WIDTH;
        player.vx = 0;
    }
    
    // Apply strict vertical boundary restrictions
    if (new_y < LEVEL_MIN_Y) {
        new_y = LEVEL_MIN_Y;
        player.vy = 0;
    } else if (new_y + SPRITE_HEIGHT > LEVEL_MAX_Y) {
        // If player falls below the level, respawn
        player.x = SCREEN_WIDTH / 2 - SPRITE_WIDTH / 2;  // Centré horizontalement
        player.y = SCREEN_HEIGHT / 2 - SPRITE_HEIGHT / 2;  // Centré verticalement
        player.vy = 0;
        player.vx = 0;
        return;
    }
    
    // Vérifier la collision avec des surfaces mortelles sur tout le corps
    // Centre du joueur
    int center_collision = check_collision(game.collision_map, 
                                      (int)(new_x + SPRITE_WIDTH/2),
                                      (int)(new_y + SPRITE_HEIGHT/2), 
                                      0);
    
    // Tête du joueur
    int head_collision = check_collision(game.collision_map, 
                                    (int)(new_x + SPRITE_WIDTH/2),
                                    (int)(new_y + 10), 
                                    0);
                                    
    // Si une collision de mort est détectée n'importe où sur le corps
    if (center_collision == COLLISION_DEATH || head_collision == COLLISION_DEATH) {
        kill_player();
        return;
    }
    
    // Left side collision
    int left_collision = check_collision(game.collision_map, 
                                       (int)new_x + 10 + 20,
                                       (int)new_y + SPRITE_HEIGHT/2, 
                                       0);
    
    // Right side collision
    int right_collision = check_collision(game.collision_map, 
                                        (int)new_x + SPRITE_WIDTH - 10 - 20,
                                        (int)new_y + SPRITE_HEIGHT/2, 
                                        0);
    
    // Vérification des collisions mortelles sur les côtés
    if (left_collision == COLLISION_DEATH || right_collision == COLLISION_DEATH) {
        kill_player();
        return;
    }
    
    // For one-way platforms, allow horizontal movement through them
    if (left_collision == COLLISION_PLATFORM_ONE_WAY) {
        left_collision = COLLISION_NONE;
    }
    
    if (right_collision == COLLISION_PLATFORM_ONE_WAY) {
        right_collision = COLLISION_NONE;
    }
    
    // Update horizontal position if no collisions with solid objects
    if (((left_collision != COLLISION_PLATFORM && left_collision != COLLISION_INVISIBLE) || player.vx > 0) &&
        ((right_collision != COLLISION_PLATFORM && right_collision != COLLISION_INVISIBLE) || player.vx < 0)) {
        player.x = new_x;
    }
    
    // Check vertical collisions
    if (player.vy > 0) { // Falling
        // Check both feet for ground collision
        int bottom_collision_left = check_collision(game.collision_map, 
                                                (int)(player.x + 10 + 40),
                                                (int)(new_y + SPRITE_HEIGHT - 5), 
                                                0);
        int bottom_collision_right = check_collision(game.collision_map, 
                                                 (int)(player.x + SPRITE_WIDTH - 10 - 40),
                                                 (int)(new_y + SPRITE_HEIGHT - 5), 
                                                 0);
        
        // Death collision
        if (bottom_collision_left == COLLISION_DEATH || bottom_collision_right == COLLISION_DEATH) {
            // Déclencher la mort du joueur
            kill_player();
            return;
        }
        
        // Handle one-way platforms - land on them only when falling down from above
        if ((bottom_collision_left == COLLISION_PLATFORM_ONE_WAY || bottom_collision_right == COLLISION_PLATFORM_ONE_WAY) &&
            player.vy > 0) {
            // Calculate the platform top position
            int platform_y = (int)(new_y + SPRITE_HEIGHT - 5);
            
            // Find the exact pixel where the platform starts
            while (platform_y > new_y && 
                  (check_collision(game.collision_map, (int)(player.x + 10 + 40), platform_y, 0) != COLLISION_PLATFORM_ONE_WAY) &&
                  (check_collision(game.collision_map, (int)(player.x + SPRITE_WIDTH - 10 - 40), platform_y, 0) != COLLISION_PLATFORM_ONE_WAY)) {
                platform_y--;
            }
            
            // Land on the platform
            player.y = platform_y - SPRITE_HEIGHT + 5;  // Position player exactly on platform
            player.vy = 0;
            
            // Ajouter une transition en douceur vers l'état "au sol"
            if (player.is_falling || player.is_jumping) {
                player.is_falling = 0;
                player.is_jumping = 0;
                // Remettre à zéro les frames d'animation pour un retour à l'animation idle ou run
                player.current_frame = 0;
            }
            
            player.on_ground = 1;
            player.is_jumping = 0;
            player.is_falling = 0;
        }
        // Normal ground collision (non-one-way platforms)
        else if (bottom_collision_left == COLLISION_PLATFORM || bottom_collision_right == COLLISION_PLATFORM) {
            // Find the exact pixel where the platform starts
            int platform_y = (int)(new_y + SPRITE_HEIGHT - 5);
            while (platform_y > new_y && 
                  (check_collision(game.collision_map, (int)(player.x + 10 + 40), platform_y, 0) != COLLISION_PLATFORM) &&
                  (check_collision(game.collision_map, (int)(player.x + SPRITE_WIDTH - 10 - 40), platform_y, 0) != COLLISION_PLATFORM)) {
                platform_y--;
            }
            
            player.y = platform_y - SPRITE_HEIGHT + 5;  // Position player exactly on platform
            player.vy = 0;
            
            // Ajouter une transition en douceur vers l'état "au sol"
            if (player.is_falling || player.is_jumping) {
                player.is_falling = 0;
                player.is_jumping = 0;
                // Remettre à zéro les frames d'animation pour un retour à l'animation idle ou run
                player.current_frame = 0;
            }
            
            player.on_ground = 1;
            player.is_jumping = 0;
            player.is_falling = 0;
        }
        // No collision or one-way platform from below, continue falling
        else if ((bottom_collision_left == COLLISION_NONE || bottom_collision_left == COLLISION_PLATFORM_ONE_WAY) && 
                 (bottom_collision_right == COLLISION_NONE || bottom_collision_right == COLLISION_PLATFORM_ONE_WAY)) {
            player.y = new_y;
            player.on_ground = 0;
        }
    } 
    else if (player.vy < 0) { // Jumping/moving up
        // Check head for ceiling collision
        int top_collision = check_collision(game.collision_map, 
                                         (int)(player.x + SPRITE_WIDTH/2),
                                         (int)(new_y + 10), 
                                         0);
        
        // For one-way platforms, allow upward movement through them
        if (top_collision == COLLISION_PLATFORM_ONE_WAY) {
            top_collision = COLLISION_NONE;
        }
        
        if (top_collision == COLLISION_NONE) {
            player.y = new_y;
        } else {
            player.vy = 0; // Stop upward movement
            player.y = ((int)(new_y + 10) / TILE_SIZE + 1) * TILE_SIZE - 10;
        }
    }
    else { // vy == 0
        player.y = new_y;
    }
    
    // For climbing, check if player is still touching climbable surface
    if (player.is_climbing) {
        int can_climb = 0;
        int climb_check_center = check_collision(game.collision_map, 
                                              (int)(player.x + SPRITE_WIDTH/2), 
                                              (int)(player.y + SPRITE_HEIGHT/2), 
                                              1);
        
        if (climb_check_center == COLLISION_CLIMB) {
            can_climb = 1;
        }
        
        if (!can_climb) {
            player.is_climbing = 0;
        }
    }
    
    // Update the camera
    update_camera(player.x + SPRITE_WIDTH/2, player.y + SPRITE_HEIGHT/2);
}

// Fonction pour afficher le score à l'écran
void render_score() {
    // Si la police n'est pas chargée, on ne peut pas afficher le texte
    if (!game.font) {
        // Afficher le score en console uniquement pour le débogage
        if (SDL_GetTicks() % 60 == 0) {
            printf("Score actuel: %d (police non disponible)\n", player_score);
        }
        return;
    }
    
    // Définir la position du score directement sous la barre de vie
    int score_x = HEALTH_BAR_X;
    int score_y = HEALTH_BAR_Y + HEALTH_BAR_HEIGHT + 5;
    
    // Créer un buffer pour le texte du score
    char score_text[100];
    sprintf(score_text, "Score: %d", player_score);
    
    // Couleur du texte (blanc)
    SDL_Color text_color = {255, 255, 255};
    
    // Rendu du texte avec SDL_ttf
    SDL_Surface* text_surface = TTF_RenderText_Solid(game.font, score_text, text_color);
    if (!text_surface) {
        printf("Erreur lors du rendu du texte: %s\n", TTF_GetError());
        return;
    }
    
    // Position du texte
    SDL_Rect text_rect = {score_x, score_y, text_surface->w, text_surface->h};
    
    // Afficher le texte
    SDL_BlitSurface(text_surface, NULL, game.screen, &text_rect);
    
    // Libérer la surface
    SDL_FreeSurface(text_surface);
}

// Fonction pour le rendu du jeu
void render() {
    // Effacer l'écran
    SDL_FillRect(game.screen, NULL, SDL_MapRGB(game.screen->format, 0, 0, 0));
    
    // Rendre le fond
    SDL_Rect src_rect = {game.camera_x, game.camera_y, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_Rect dst_rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_BlitSurface(game.background, &src_rect, game.screen, &dst_rect);
    
    // Rendre les ennemis si dans le niveau principal
    if (current_level_state == LEVEL_STATE_MAIN_PARKOUR) {
        render_enemies();
    }
    
    // Rendre le joueur s'il est visible ou pendant l'effet de clignotement quand blessé
    if (!player.is_hurt || (player.hurt_timer % 4 < 2)) {
        render_player();
    }
    
    // Rendre le boss si dans l'arène du boss (Typhon ou Zeus)
    if (current_level_state == LEVEL_STATE_BOSS_ARENA || current_level_state == LEVEL_STATE_ZEUS_ARENA) {
        render_typhon();
        render_fireballs();  // Rendre les boules de feu
    }
    
    // Rendre la barre de vie du joueur
    render_health_bar();
    
    // Rendre le score
    render_score();
    
    // Rendre l'effet de fondu si nécessaire
    render_fade();
    
    // Mettre à jour l'écran
    SDL_Flip(game.screen);
}

void render_player() {
    if (!player.is_visible) return;

    SDL_Surface* current_sprite = NULL;
    SDL_Rect src_rect, dest_rect;
    int frame_width = SPRITE_WIDTH;
    int max_frames = 1;
    
    // Select sprite based on state with priority order
    if (player.is_dying) {
        // Animation de mort prioritaire sur toutes les autres
        current_sprite = (player.direction == 1) ? player.sprite_death_R : player.sprite_death_L;
        max_frames = DEATH_FRAMES;
        frame_width = DEATH_FRAME_WIDTH;
        
        // Utiliser death_frame au lieu de current_frame pour l'animation de mort
        src_rect.x = player.death_frame * frame_width;
        src_rect.y = 0;
        src_rect.w = frame_width;
        src_rect.h = SPRITE_HEIGHT;
        
        dest_rect.x = (int)(player.x - game.camera_x);
        dest_rect.y = (int)(player.y - game.camera_y);
        dest_rect.w = frame_width;
        dest_rect.h = SPRITE_HEIGHT;
        
        if (current_sprite) {
            SDL_BlitSurface(current_sprite, &src_rect, game.screen, &dest_rect);
        }
        
        // Sortir de la fonction après avoir rendu l'animation de mort
        return;
    }
    else if (player.is_attacking) {
        current_sprite = (player.direction == 1) ? player.sprite_attack_R : player.sprite_attack_L;
        max_frames = ATTACK_FRAMES;
        frame_width = ATTACK_FRAME_WIDTH;
        
        // Update frame for attack animation
        player.frame_timer++;
        if (player.frame_timer >= ATTACK_SPEED) {
            player.frame_timer = 0;
            player.current_frame++;
            if (player.current_frame >= max_frames) {
                player.current_frame = 0;
                player.is_attacking = 0;
            }
        }
    }
    else if (player.is_crouching) {
        current_sprite = (player.direction == 1) ? player.sprite_crouch_R : player.sprite_crouch_L;
        max_frames = CROUCH_FRAMES;
        frame_width = CROUCH_FRAME_WIDTH;
        
        // Handle crouching animation states
        player.frame_timer++;
        if (player.frame_timer >= CROUCH_SPEED) {
            player.frame_timer = 0;
            
            if (player.crouch_state == 1) {
                // Crouching down animation (frames 0-3)
                player.current_frame++;
                if (player.current_frame >= 4) {
                    player.current_frame = 4;
                    player.crouch_state = 2; // Now in full crouch position
                }
            } 
            else if (player.crouch_state == 2) {
                // Hold the crouched position (frame 4)
                player.current_frame = 4;
            }
            else if (player.crouch_state == 3) {
                // Standing up animation (frames 5+)
                player.current_frame++;
                if (player.current_frame >= max_frames) {
                    // Animation complete, return to normal state
                    player.current_frame = 0;
                    player.is_crouching = 0;
                    player.crouch_state = 0;
                }
            }
        }
    } 
    else if (player.is_climbing) {
        current_sprite = player.sprite_climb;
        max_frames = CLIMB_FRAMES;
        frame_width = CLIMB_FRAME_WIDTH;
        
        // Only animate if moving up or down
        player.frame_timer++;
        if (player.frame_timer >= CLIMB_SPEED) {
            player.frame_timer = 0;
            if (player.vy != 0) {
                player.current_frame = (player.current_frame + 1) % max_frames;
            }
        }
    }
    else if (player.is_jumping) {
        // Animation de saut améliorée
        current_sprite = (player.direction == 1) ? player.sprite_jump_R : player.sprite_jump_L;
        max_frames = JUMP_FRAMES;
        frame_width = JUMP_FRAME_WIDTH;
        
        // Adaptation de la frame en fonction de la vitesse verticale
        // Plus la vitesse est proche de 0, plus on avance dans l'animation
        float jump_progress = 1.0f - (-player.vy / JUMP_VELOCITY);  // 0 = début du saut, 1 = sommet
        if (jump_progress < 0) jump_progress = 0;
        if (jump_progress > 1) jump_progress = 1;
        
        // Sélection de la frame en fonction de la progression du saut
        int desired_frame = (int)(jump_progress * (max_frames - 1));
        
        // Transition progressive vers la frame désirée
        if (player.current_frame < desired_frame) {
            player.frame_timer++;
            if (player.frame_timer >= JUMP_SPEED) {
                player.frame_timer = 0;
                player.current_frame++;
            }
        } else if (player.current_frame > desired_frame) {
            player.frame_timer++;
            if (player.frame_timer >= JUMP_SPEED) {
                player.frame_timer = 0;
                player.current_frame--;
            }
        }
        
        // Sécurité pour rester dans les limites
        if (player.current_frame >= max_frames) {
            player.current_frame = max_frames - 1;
        }
        if (player.current_frame < 0) {
            player.current_frame = 0;
        }
    }
    else if (player.is_falling) {
        current_sprite = (player.direction == 1) ? player.sprite_fall_R : player.sprite_fall_L;
        max_frames = FALL_FRAMES;
        frame_width = FALL_FRAME_WIDTH;
        
        // Animation progressive de la chute en fonction de la vitesse
        float fall_progress = player.vy / MAX_FALL_SPEED;  // 0 = début de la chute, 1 = vitesse max
        if (fall_progress < 0) fall_progress = 0;
        if (fall_progress > 1) fall_progress = 1;
        
        int desired_frame = (int)(fall_progress * (max_frames - 1));
        
        // Transition progressive vers la frame désirée
        if (player.current_frame < desired_frame) {
            player.frame_timer++;
            if (player.frame_timer >= FALL_SPEED) {
                player.frame_timer = 0;
                player.current_frame++;
            }
        } else if (player.current_frame > desired_frame) {
            player.frame_timer++;
            if (player.frame_timer >= FALL_SPEED) {
                player.frame_timer = 0;
                player.current_frame--;
            }
        }
        
        // Sécurité pour rester dans les limites
        if (player.current_frame >= max_frames) {
            player.current_frame = max_frames - 1;
        }
        if (player.current_frame < 0) {
            player.current_frame = 0;
        }
    }
    else if (player.is_running) {
        current_sprite = (player.direction == 1) ? player.sprite_run_R : player.sprite_run_L;
        max_frames = RUN_FRAMES;
        frame_width = RUN_FRAME_WIDTH;
        
        // Update frame for running animation
        player.frame_timer++;
        if (player.frame_timer >= RUN_SPEED) {
            player.frame_timer = 0;
            player.current_frame = (player.current_frame + 1) % max_frames;
        }
    }
    else {
        // Fix idle animation flashing
        current_sprite = (player.direction == 1) ? player.sprite_idle_R : player.sprite_idle_L;
        max_frames = IDLE_FRAMES;
        frame_width = IDLE_FRAME_WIDTH;
        
        // Update frame for idle animation
        player.frame_timer++;
        if (player.frame_timer >= IDLE_SPEED) {
            player.frame_timer = 0;
            player.current_frame = (player.current_frame + 1) % max_frames;
        }
    }
    
    // Safety check to prevent out-of-bounds frame access
    if (player.current_frame >= max_frames) {
        player.current_frame = 0;
    }
    
    // Calculate source rectangle for the current frame
    src_rect.x = player.current_frame * frame_width;
    src_rect.y = 0;
    src_rect.w = frame_width;
    src_rect.h = SPRITE_HEIGHT;
    
    // Calculate destination rectangle (position on screen)
    dest_rect.x = (int)(player.x - game.camera_x);
    dest_rect.y = (int)(player.y - game.camera_y);
    dest_rect.w = frame_width;
    dest_rect.h = SPRITE_HEIGHT;
    
    // Draw the sprite only if we have a valid sprite
    if (current_sprite) {
        SDL_BlitSurface(current_sprite, &src_rect, game.screen, &dest_rect);
    }
}

void cleanup_player() {
    SDL_FreeSurface(player.sprite_idle_R);
    SDL_FreeSurface(player.sprite_idle_L);
    SDL_FreeSurface(player.sprite_run_R);
    SDL_FreeSurface(player.sprite_run_L);
    SDL_FreeSurface(player.sprite_jump_R);
    SDL_FreeSurface(player.sprite_jump_L);
    SDL_FreeSurface(player.sprite_fall_R);  // Free falling spritesheet
    SDL_FreeSurface(player.sprite_fall_L);
    SDL_FreeSurface(player.sprite_attack_R);
    SDL_FreeSurface(player.sprite_attack_L);
    SDL_FreeSurface(player.sprite_crouch_R);
    SDL_FreeSurface(player.sprite_crouch_L);
    SDL_FreeSurface(player.sprite_climb);
    SDL_FreeSurface(player.sprite_death_R);  // Free death spritesheet
    SDL_FreeSurface(player.sprite_death_L);  // Free death spritesheet
}

void cleanup() {
    SDL_FreeSurface(game.background);
    SDL_FreeSurface(game.collision_map);
    SDL_FreeSurface(game.screen);
    
    // Libérer la police
    if (game.font) {
        TTF_CloseFont(game.font);
        game.font = NULL;
    }
    
    // Quitter SDL_ttf
    TTF_Quit();
    
    SDL_Quit();
}

// Fonction pour déclencher la mort du joueur
void kill_player() {
    if (!player.is_dying) {  // Seulement si le joueur n'est pas déjà en train de mourir
        player.is_dying = 1;
        player.death_frame = 0;
        player.death_timer = 0;
        player.vx = 0;
        player.vy = 0;
        
        // Désactiver tous les autres états d'animation
        player.is_jumping = 0;
        player.is_falling = 0;
        player.is_running = 0;
        player.is_attacking = 0;
        player.is_crouching = 0;
        player.is_climbing = 0;
        player.on_ground = 0;
        
        // Jouer un son de mort si disponible
        // (à implémenter si des sons sont ajoutés au jeu)
    }
}

// Fonction pour respawn le joueur
void respawn_player() {
    // Ne réinitialiser le joueur que si l'animation de mort est terminée
    if (player.death_frame >= DEATH_FRAMES - 1) {
        player.x = RESPAWN_X;
        player.y = RESPAWN_Y;
        player.vx = 0;
        player.vy = 0;
        player.is_dying = 0;
        player.death_frame = 0;
        player.death_timer = 0;
        player.is_jumping = 0;
        player.is_falling = 0;
        player.is_running = 0;
        player.is_attacking = 0;
        player.is_crouching = 0;
        player.is_climbing = 0;
        player.on_ground = 0;
        player.current_frame = 0;
        player.frame_timer = 0;
        player.health = PLAYER_MAX_HEALTH; // Réinitialiser la santé
        player.is_hurt = 0;
        player.hurt_timer = 0;
    }
}

// Prototypes de fonctions pour les ennemis
void init_enemies();
void update_enemies();
void render_enemies();
void add_enemy(float x, float y);
void cleanup_enemies();
int check_enemy_collision(float x, float y, float w, float h);

// Initialisation des ennemis
void init_enemies() {
    // Réinitialiser le tableau d'ennemis
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = 0;
    }
    
    num_enemies = 0;
    
    // Ajouter les ennemis aux positions spécifiées
    add_enemy(GOLLUX_SPAWN_X1, GOLLUX_SPAWN_Y1);
    add_enemy(GOLLUX_SPAWN_X2, GOLLUX_SPAWN_Y2);
    
    // Initialiser le générateur de nombres aléatoires
    srand(time(NULL));
}

// Ajout d'un nouvel ennemi
void add_enemy(float x, float y) {
    if (num_enemies >= MAX_ENEMIES) {
        return; // Maximum atteint
    }
    
    int idx = num_enemies++;
    Enemy* enemy = &enemies[idx];
    
    // Initialiser les propriétés de base
    enemy->x = x;
    enemy->y = y;
    enemy->vx = 0;
    enemy->vy = 0;
    enemy->direction = -1; // Commence en regardant à gauche
    enemy->health = GOLLUX_MAX_HEALTH;
    enemy->on_ground = 0;
    enemy->active = 1;
    enemy->hit_cooldown = 0;
    enemy->knockback_counter = 0;
    
    enemy->state = ENEMY_IDLE;
    enemy->current_frame = 0;
    enemy->frame_timer = 0;
    
    // Charger les sprites
    enemy->sprite_attack = IMG_Load(GOLLUX_ATTACK);
    if (!enemy->sprite_attack) {
        printf("Error loading enemy attack sprite: %s\n", IMG_GetError());
    }
    
    enemy->sprite_hit = IMG_Load(GOLLUX_HIT);
    if (!enemy->sprite_hit) {
        printf("Error loading enemy hit sprite: %s\n", IMG_GetError());
    }
    
    enemy->sprite_idle = IMG_Load(GOLLUX_IDLE);
    if (!enemy->sprite_idle) {
        printf("Error loading enemy idle sprite: %s\n", IMG_GetError());
    }
    
    enemy->sprite_move = IMG_Load(GOLLUX_MOVE);
    if (!enemy->sprite_move) {
        printf("Error loading enemy move sprite: %s\n", IMG_GetError());
    }
    
    // Créer les versions retournées des sprites
    enemy->sprite_attack_flipped = load_and_flip_png(GOLLUX_ATTACK);
    enemy->sprite_hit_flipped = load_and_flip_png(GOLLUX_HIT);
    enemy->sprite_idle_flipped = load_and_flip_png(GOLLUX_IDLE);
    enemy->sprite_move_flipped = load_and_flip_png(GOLLUX_MOVE);
}

// Mise à jour de tous les ennemis
void update_enemies() {
    for (int i = 0; i < num_enemies; i++) {
        Enemy* enemy = &enemies[i];
        
        if (!enemy->active) continue;
        
        // Réduire le temps de cooldown de hit
        if (enemy->hit_cooldown > 0) {
            enemy->hit_cooldown--;
        }
        
        // Gérer l'effet de recul
        if (enemy->knockback_counter > 0) {
            enemy->knockback_counter--;
            if (enemy->knockback_counter == 0) {
                enemy->vx = 0; // Arrêter le recul
            }
        }
        
        // Appliquer la gravité si l'ennemi n'est pas au sol
        if (!enemy->on_ground) {
            enemy->vy += GRAVITY;
            if (enemy->vy > MAX_FALL_SPEED) {
                enemy->vy = MAX_FALL_SPEED;
            }
        }
        
        // Mouvement aléatoire si l'ennemi est inactif et pas en recul
        if (enemy->state == ENEMY_IDLE && enemy->on_ground && enemy->knockback_counter == 0) {
            // Environ 1% de chance de changer de direction
            if (rand() % 100 < 1) {
                enemy->vx = (rand() % 2 == 0 ? 1 : -1) * GOLLUX_SPEED;
                enemy->direction = (enemy->vx > 0) ? 1 : -1;
                enemy->state = ENEMY_MOVE;
            }
            
            // Environ 0.5% de chance de s'arrêter si en mouvement
            if (enemy->vx != 0 && rand() % 200 < 1) {
                enemy->vx = 0;
                enemy->state = ENEMY_IDLE;
            }
        }
        
        // Calculer la nouvelle position avec la vélocité
        float new_x = enemy->x + enemy->vx;
        float new_y = enemy->y + enemy->vy;
        
        // Vérifier les limites de mouvement
        if (new_x < GOLLUX_MIN_X) {
            new_x = GOLLUX_MIN_X;
            enemy->vx = -enemy->vx; // Inverser la direction
            enemy->direction = 1;
        } else if (new_x + GOLLUX_IDLE_WIDTH > GOLLUX_MAX_X) {
            new_x = GOLLUX_MAX_X - GOLLUX_IDLE_WIDTH;
            enemy->vx = -enemy->vx; // Inverser la direction
            enemy->direction = -1;
        }
        
        // Vérifier les collisions horizontales (plus précises)
        int left_collision = check_collision(game.collision_map, 
                                          (int)new_x + 10,
                                          (int)enemy->y + GOLLUX_IDLE_HEIGHT/2, 
                                          0);
        
        int right_collision = check_collision(game.collision_map, 
                                           (int)new_x + GOLLUX_IDLE_WIDTH - 10,
                                           (int)enemy->y + GOLLUX_IDLE_HEIGHT/2, 
                                           0);
        
        // Mettre à jour la position horizontale en tenant compte des collisions
        if ((left_collision != COLLISION_PLATFORM && left_collision != COLLISION_INVISIBLE) || enemy->vx > 0) {
            if ((right_collision != COLLISION_PLATFORM && right_collision != COLLISION_INVISIBLE) || enemy->vx < 0) {
                enemy->x = new_x;
            } else {
                enemy->vx = -enemy->vx; // Changer de direction si collision à droite
                enemy->direction = -1;
            }
        } else {
            enemy->vx = -enemy->vx; // Changer de direction si collision à gauche
            enemy->direction = 1;
        }
        
        // Vérifier les collisions verticales (si en train de tomber)
        if (enemy->vy > 0) {
            // Vérifier les deux pieds pour la collision avec le sol
            int bottom_collision_left = check_collision(game.collision_map, 
                                                    (int)(enemy->x + 10),
                                                    (int)(new_y + GOLLUX_IDLE_HEIGHT), 
                                                    0);
            int bottom_collision_right = check_collision(game.collision_map, 
                                                     (int)(enemy->x + GOLLUX_IDLE_WIDTH - 10),
                                                     (int)(new_y + GOLLUX_IDLE_HEIGHT), 
                                                     0);
            
            // Collision avec des plateformes standard ou one-way
            if (bottom_collision_left == COLLISION_PLATFORM || bottom_collision_right == COLLISION_PLATFORM ||
                bottom_collision_left == COLLISION_PLATFORM_ONE_WAY || bottom_collision_right == COLLISION_PLATFORM_ONE_WAY) {
                
                // Chercher la position exacte du haut de la plateforme
                int platform_y = (int)(new_y + GOLLUX_IDLE_HEIGHT);
                for (int h = 0; h < 10; h++) {  // Vérifier jusqu'à 10 pixels vers le haut pour trouver le bord exact
                    int test_y = platform_y - h;
                    int test_left = check_collision(game.collision_map, 
                                                  (int)(enemy->x + 10), 
                                                  test_y, 
                                                  0);
                    int test_right = check_collision(game.collision_map, 
                                                   (int)(enemy->x + GOLLUX_IDLE_WIDTH - 10), 
                                                   test_y, 
                                                   0);
                                                   
                    if ((test_left != COLLISION_PLATFORM && test_left != COLLISION_PLATFORM_ONE_WAY) && 
                        (test_right != COLLISION_PLATFORM && test_right != COLLISION_PLATFORM_ONE_WAY)) {
                        platform_y = test_y + 1;  // Position juste au-dessus de la plateforme
                        break;
                    }
                }
                
                // Placer l'ennemi exactement sur la plateforme
                enemy->y = platform_y - GOLLUX_IDLE_HEIGHT;
                enemy->vy = 0;
                enemy->on_ground = 1;
            } else {
                // Si pas de collision au sol, continuer à tomber
                enemy->y = new_y;
                enemy->on_ground = 0;
            }
        } else if (enemy->vy < 0) {
            // Collision avec le plafond
            int top_collision = check_collision(game.collision_map, 
                                             (int)(enemy->x + GOLLUX_IDLE_WIDTH/2),
                                             (int)(new_y), 
                                             0);
            
            if (top_collision == COLLISION_PLATFORM || top_collision == COLLISION_INVISIBLE) {
                // Arrêter le mouvement vers le haut
                enemy->vy = 0;
                // Trouver la position exacte du plafond
                int y_adj = ((int)new_y / TILE_SIZE) * TILE_SIZE + TILE_SIZE;
                enemy->y = y_adj;
            } else {
                // Continuer le mouvement vertical
                enemy->y = new_y;
            }
        } else {
            // Si pas de mouvement vertical
            enemy->y = new_y;
        }
        
        // Comportement d'IA basique
        // Calcul de la distance au joueur
        float dx = player.x - enemy->x;
        float dy = player.y - enemy->y;
        float dist = sqrt(dx*dx + dy*dy);
        
        // Détecter si le joueur est à portée
        if (dist < GOLLUX_DETECTION_RANGE && enemy->state != ENEMY_HIT) {
            // Si le joueur est à portée d'attaque
            if (dist < GOLLUX_ATTACK_RANGE) {
                // Attaquer
                if (enemy->state != ENEMY_ATTACK) {
                    enemy->state = ENEMY_ATTACK;
                    enemy->current_frame = 0;
                    enemy->frame_timer = 0;
                    enemy->vx = 0; // Arrêter pendant l'attaque
                }
                
                // Faire face au joueur
                enemy->direction = (dx > 0) ? 1 : -1;
            }
            // Sinon, se déplacer vers le joueur
            else if (enemy->state != ENEMY_ATTACK && enemy->on_ground) {
                enemy->state = ENEMY_MOVE;
                enemy->direction = (dx > 0) ? 1 : -1;
                enemy->vx = enemy->direction * GOLLUX_SPEED;
            }
        }
        // Si le joueur n'est pas à portée, revenir à l'état d'inactivité
        else if (enemy->state != ENEMY_HIT && enemy->state != ENEMY_ATTACK) {
            // Seulement changer l'état, ne pas réinitialiser vx pour permettre le mouvement aléatoire
            if (enemy->vx == 0) {
                enemy->state = ENEMY_IDLE;
            } else {
                enemy->state = ENEMY_MOVE;
            }
        }
        
        // Mettre à jour l'animation
        int anim_speed;
        int max_frames;
        
        switch (enemy->state) {
            case ENEMY_ATTACK:
                anim_speed = GOLLUX_ATTACK_SPEED;
                max_frames = GOLLUX_ATTACK_FRAMES;
                break;
            case ENEMY_HIT:
                anim_speed = GOLLUX_HIT_SPEED;
                max_frames = GOLLUX_HIT_FRAMES;
                break;
            case ENEMY_MOVE:
                anim_speed = GOLLUX_MOVE_SPEED;
                max_frames = GOLLUX_MOVE_FRAMES;
                break;
            case ENEMY_IDLE:
            default:
                anim_speed = GOLLUX_IDLE_SPEED;
                max_frames = GOLLUX_IDLE_FRAMES;
                break;
        }
        
        enemy->frame_timer++;
        if (enemy->frame_timer >= anim_speed) {
            enemy->frame_timer = 0;
            enemy->current_frame++;
            
            // Gestion de fin d'animation
            if (enemy->current_frame >= max_frames) {
                if (enemy->state == ENEMY_ATTACK || enemy->state == ENEMY_HIT) {
                    enemy->state = ENEMY_IDLE;
                    enemy->current_frame = 0;
                } else {
                    enemy->current_frame = 0; // Boucler les autres animations
                }
            }
        }
        
        // Vérifier si l'ennemi est mort
        if (enemy->health <= 0) {
            enemy->active = 0;
        }
    }
}

// Rendu des ennemis
void render_enemies() {
    for (int i = 0; i < num_enemies; i++) {
        Enemy* enemy = &enemies[i];
        
        if (!enemy->active) continue;
        
        SDL_Surface* current_sprite = NULL;
        int frame_width;
        int frame_height;
        
        // Sélectionner le sprite en fonction de l'état et de la direction
        switch (enemy->state) {
            case ENEMY_ATTACK:
                current_sprite = enemy->direction == 1 ? enemy->sprite_attack : enemy->sprite_attack_flipped;
                frame_width = GOLLUX_ATTACK_WIDTH;
                frame_height = GOLLUX_ATTACK_HEIGHT;
                break;
            case ENEMY_HIT:
                current_sprite = enemy->direction == 1 ? enemy->sprite_hit : enemy->sprite_hit_flipped;
                frame_width = GOLLUX_HIT_WIDTH;
                frame_height = GOLLUX_HIT_HEIGHT;
                break;
            case ENEMY_MOVE:
                current_sprite = enemy->direction == 1 ? enemy->sprite_move : enemy->sprite_move_flipped;
                frame_width = GOLLUX_MOVE_WIDTH;
                frame_height = GOLLUX_MOVE_HEIGHT;
                break;
            case ENEMY_IDLE:
            default:
                current_sprite = enemy->direction == 1 ? enemy->sprite_idle : enemy->sprite_idle_flipped;
                frame_width = GOLLUX_IDLE_WIDTH;
                frame_height = GOLLUX_IDLE_HEIGHT;
                break;
        }
        
        if (!current_sprite) continue;
        
        SDL_Rect src_rect, dest_rect;
        
        // Calculer le rectangle source pour la frame actuelle
        src_rect.x = enemy->current_frame * frame_width;
        src_rect.y = 0;
        src_rect.w = frame_width;
        src_rect.h = frame_height;
        
        // Calculer le rectangle destination (position à l'écran)
        dest_rect.x = (int)(enemy->x - game.camera_x);
        dest_rect.y = (int)(enemy->y - game.camera_y);
        dest_rect.w = frame_width;
        dest_rect.h = frame_height;
        
        // Dessiner l'ennemi
        SDL_BlitSurface(current_sprite, &src_rect, game.screen, &dest_rect);
    }
}

// Nettoyer les ressources des ennemis
void cleanup_enemies() {
    for (int i = 0; i < num_enemies; i++) {
        if (enemies[i].active) {
            SDL_FreeSurface(enemies[i].sprite_attack);
            SDL_FreeSurface(enemies[i].sprite_hit);
            SDL_FreeSurface(enemies[i].sprite_idle);
            SDL_FreeSurface(enemies[i].sprite_move);
            
            SDL_FreeSurface(enemies[i].sprite_attack_flipped);
            SDL_FreeSurface(enemies[i].sprite_hit_flipped);
            SDL_FreeSurface(enemies[i].sprite_idle_flipped);
            SDL_FreeSurface(enemies[i].sprite_move_flipped);
        }
    }
}

// Vérifier si le joueur est en collision avec un ennemi
int check_enemy_collision(float x, float y, float w, float h) {
    for (int i = 0; i < num_enemies; i++) {
        Enemy* enemy = &enemies[i];
        
        if (!enemy->active || enemy->hit_cooldown > 0) continue;
        
        // Définir la zone de collision de l'ennemi en fonction de son état
        float enemy_w, enemy_h;
        
        switch (enemy->state) {
            case ENEMY_ATTACK:
                enemy_w = GOLLUX_ATTACK_WIDTH;
                enemy_h = GOLLUX_ATTACK_HEIGHT;
                break;
            case ENEMY_HIT:
                enemy_w = GOLLUX_HIT_WIDTH;
                enemy_h = GOLLUX_HIT_HEIGHT;
                break;
            case ENEMY_MOVE:
                enemy_w = GOLLUX_MOVE_WIDTH;
                enemy_h = GOLLUX_MOVE_HEIGHT;
                break;
            case ENEMY_IDLE:
            default:
                enemy_w = GOLLUX_IDLE_WIDTH;
                enemy_h = GOLLUX_IDLE_HEIGHT;
                break;
        }
        
        // Vérifier la collision par AABB (Axis-Aligned Bounding Box)
        if (x < enemy->x + enemy_w &&
            x + w > enemy->x &&
            y < enemy->y + enemy_h &&
            y + h > enemy->y) {
            return i; // Retourner l'index de l'ennemi en collision
        }
    }
    
    return -1; // Pas de collision
}

// Fonction pour vérifier si le joueur touche un ennemi avec son attaque
void check_player_attack_collision() {
    if (!player.is_attacking) return;
    
    // Ne vérifie la collision qu'à certains moments de l'animation d'attaque
    // (environ à la moitié de l'animation)
    if (player.current_frame < ATTACK_FRAMES / 3 || player.current_frame > ATTACK_FRAMES * 2 / 3) {
        return;
    }
    
    // Zone d'attaque devant le joueur
    int attack_width = 60;
    int attack_x = player.x + (player.direction == 1 ? SPRITE_WIDTH - 20 : -attack_width + 20);
    int attack_y = player.y + 30;
    int attack_height = 40;
    
    for (int i = 0; i < num_enemies; i++) {
        Enemy* enemy = &enemies[i];
        
        if (!enemy->active || enemy->hit_cooldown > 0) continue;
        
        // Déterminer la largeur et hauteur de l'ennemi selon son état
        int enemy_width, enemy_height;
        switch (enemy->state) {
            case ENEMY_ATTACK:
                enemy_width = GOLLUX_ATTACK_WIDTH;
                enemy_height = GOLLUX_ATTACK_HEIGHT;
                break;
            case ENEMY_HIT:
                enemy_width = GOLLUX_HIT_WIDTH;
                enemy_height = GOLLUX_HIT_HEIGHT;
                break;
            case ENEMY_MOVE:
                enemy_width = GOLLUX_MOVE_WIDTH;
                enemy_height = GOLLUX_MOVE_HEIGHT;
                break;
            case ENEMY_IDLE:
            default:
                enemy_width = GOLLUX_IDLE_WIDTH;
                enemy_height = GOLLUX_IDLE_HEIGHT;
                break;
        }
        
        // Vérifier la collision (AABB)
        if (attack_x < enemy->x + enemy_width &&
            attack_x + attack_width > enemy->x &&
            attack_y < enemy->y + enemy_height &&
            attack_y + attack_height > enemy->y) {
            
            // Toucher l'ennemi
            enemy->health -= PLAYER_ATTACK_DAMAGE;
            enemy->hit_cooldown = GOLLUX_HIT_COOLDOWN;
            enemy->state = ENEMY_HIT;
            enemy->current_frame = 0;
            
            // Effet de recul
            enemy->knockback_counter = 10;
            enemy->vx = player.direction * GOLLUX_KNOCKBACK;
            
            // Si l'ennemi n'a plus de points de vie
            if (enemy->health <= 0) {
                enemy->active = 0;
                
                // Ajouter des points au score pour avoir tué un ennemi
                player_score += ENEMY_KILL_POINTS;
                enemies_killed++;
                
                printf("Ennemi tué! +%d points (Total: %d, Ennemis tués: %d)\n", 
                       ENEMY_KILL_POINTS, player_score, enemies_killed);
            }
        }
    }
}

// Vérifier si le joueur est en collision avec un ennemi et subit des dégâts
void check_player_enemy_collision() {
    // Si le joueur est invulnérable ou en train de mourir, ignorer
    if (player_damage_cooldown > 0 || player.is_dying) return;
    
    // Zone de collision du joueur (plus petite que le sprite pour être plus précise)
    int player_hitbox_x = player.x + 30;
    int player_hitbox_y = player.y + 20;
    int player_hitbox_width = SPRITE_WIDTH - 60;
    int player_hitbox_height = SPRITE_HEIGHT - 30;
    
    for (int i = 0; i < num_enemies; i++) {
        Enemy* enemy = &enemies[i];
        
        if (!enemy->active) continue;
        
        // Déterminer la largeur et hauteur de l'ennemi selon son état
        int enemy_width, enemy_height;
        switch (enemy->state) {
            case ENEMY_ATTACK:
                enemy_width = GOLLUX_ATTACK_WIDTH;
                enemy_height = GOLLUX_ATTACK_HEIGHT;
                break;
            case ENEMY_HIT:
                enemy_width = GOLLUX_HIT_WIDTH;
                enemy_height = GOLLUX_HIT_HEIGHT;
                break;
            case ENEMY_MOVE:
                enemy_width = GOLLUX_MOVE_WIDTH;
                enemy_height = GOLLUX_MOVE_HEIGHT;
                break;
            case ENEMY_IDLE:
            default:
                enemy_width = GOLLUX_IDLE_WIDTH;
                enemy_height = GOLLUX_IDLE_HEIGHT;
                break;
        }
        
        // Zone de collision de l'ennemi (légèrement plus petite pour être précise)
        int enemy_hitbox_x = enemy->x + 10;
        int enemy_hitbox_y = enemy->y + 10;
        int enemy_hitbox_width = enemy_width - 20;
        int enemy_hitbox_height = enemy_height - 20;
        
        // Vérifier la collision (AABB)
        if (player_hitbox_x < enemy_hitbox_x + enemy_hitbox_width &&
            player_hitbox_x + player_hitbox_width > enemy_hitbox_x &&
            player_hitbox_y < enemy_hitbox_y + enemy_hitbox_height &&
            player_hitbox_y + player_hitbox_height > enemy_hitbox_y) {
            
            // Le joueur est touché
            if (enemy->state == ENEMY_ATTACK) {
                // Si l'ennemi est en train d'attaquer, infliger plus de dégâts
                damage_player(PLAYER_DAMAGE_AMOUNT * 2);
            } else {
                // Sinon, infliger des dégâts standard
                damage_player(PLAYER_DAMAGE_AMOUNT);
            }
            
            // Effet de recul
            player.vx = (enemy->x > player.x) ? -8 : 8; // Recul
            player.vy = -5; // Petit saut vers le haut
            player.on_ground = 0;
            
            return; // Sortir après la première collision
        }
    }
}

// Fonction pour rendre la barre de vie du joueur
void render_health_bar() {
    // Dessiner le fond de la barre de vie (rouge pour la partie vide)
    SDL_Rect background_rect = {HEALTH_BAR_X, HEALTH_BAR_Y, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT};
    SDL_FillRect(game.screen, &background_rect, SDL_MapRGB(game.screen->format, 255, 0, 0));
    
    // Calculer la largeur correspondant à la santé actuelle
    int health_width = (player.health * HEALTH_BAR_WIDTH) / player.max_health;
    
    // Choisir la couleur de la barre de vie en fonction du niveau de santé
    Uint32 health_color;
    if (player.health < LOW_HEALTH_THRESHOLD) {
        // Santé basse: orange clignotant
        if (SDL_GetTicks() % 1000 < 500) {
            health_color = SDL_MapRGB(game.screen->format, 255, 128, 0); // Orange
        } else {
            health_color = SDL_MapRGB(game.screen->format, 255, 80, 0); // Orange foncé
        }
    } else if (player.health < player.max_health / 2) {
        // Santé moyenne: jaune
        health_color = SDL_MapRGB(game.screen->format, 255, 255, 0);
    } else {
        // Santé élevée: vert
        health_color = SDL_MapRGB(game.screen->format, 0, 255, 0);
    }
    
    // Dessiner la partie remplie de la barre de vie
    SDL_Rect health_rect = {HEALTH_BAR_X, HEALTH_BAR_Y, health_width, HEALTH_BAR_HEIGHT};
    SDL_FillRect(game.screen, &health_rect, health_color);
    
    // Dessiner le contour de la barre de vie
    // Supprimer la variable border_rect qui n'est pas utilisée
    
    // On ne peut pas dessiner juste le contour avec SDL_FillRect, donc on utilise une couleur claire pour le cadre
    Uint32 border_color = SDL_MapRGB(game.screen->format, 255, 255, 255);
    
    // Dessiner les 4 lignes du cadre
    SDL_Rect top_line = {HEALTH_BAR_X - 1, HEALTH_BAR_Y - 1, HEALTH_BAR_WIDTH + 2, 1};
    SDL_Rect bottom_line = {HEALTH_BAR_X - 1, HEALTH_BAR_Y + HEALTH_BAR_HEIGHT, HEALTH_BAR_WIDTH + 2, 1};
    SDL_Rect left_line = {HEALTH_BAR_X - 1, HEALTH_BAR_Y - 1, 1, HEALTH_BAR_HEIGHT + 2};
    SDL_Rect right_line = {HEALTH_BAR_X + HEALTH_BAR_WIDTH, HEALTH_BAR_Y - 1, 1, HEALTH_BAR_HEIGHT + 2};
    
    SDL_FillRect(game.screen, &top_line, border_color);
    SDL_FillRect(game.screen, &bottom_line, border_color);
    SDL_FillRect(game.screen, &left_line, border_color);
    SDL_FillRect(game.screen, &right_line, border_color);
}

// Fonction pour infliger des dégâts au joueur
void damage_player(int amount) {
    // Si le joueur est invulnérable ou en train de mourir, ignorer
    if (player_damage_cooldown > 0 || player.is_dying) return;
    
    // Réduire la santé
    player.health -= amount;
    player.is_hurt = 1;
    player.hurt_timer = 20; // Durée de l'effet de clignotement
    
    // Effet de recul - déjà présent dans check_player_enemy_collision()
    
    // Vérifier si le joueur est mort
    if (player.health <= 0) {
        player.health = 0;
        kill_player();
    } else {
        // Activer la période d'invulnérabilité
        player_damage_cooldown = PLAYER_DAMAGE_COOLDOWN;
    }
}

// Fonction pour initialiser le boss Typhon
void init_typhon() {
    // Positionner le boss à un endroit bien visible dans l'arène
    typhon.x = 600;
    typhon.y = 600;
    typhon.vx = 0;
    typhon.vy = 0;
    typhon.health = TYPHON_HEALTH;
    typhon.max_health = TYPHON_HEALTH;
    typhon.active = 1;
    typhon.direction = -1;
    typhon.state = TYPHON_STATE_IDLE;
    typhon.current_frame = 0;
    typhon.frame_timer = 0;
    typhon.attack_cooldown = 0;
    typhon.hurt_cooldown = 0;
    
    printf("Boss initialisé à la position x=%.1f, y=%.1f\n", typhon.x, typhon.y);
    
    // Initialiser les boules de feu
    fireball_sprite = IMG_Load(FIREBALL_SPRITE);
    if (!fireball_sprite) {
        printf("Erreur lors du chargement du sprite de boule de feu: %s\n", IMG_GetError());
    } else {
        printf("Sprite de boule de feu chargé: %dx%d\n", fireball_sprite->w, fireball_sprite->h);
        
        // Créer la version flippée horizontalement
        fireball_sprite_flipped = load_and_flip_png(FIREBALL_SPRITE);
        if (!fireball_sprite_flipped) {
            printf("Erreur lors de la création du sprite de boule de feu flippé\n");
        } else {
            printf("Sprite de boule de feu flippé créé\n");
        }
    }
    
    // Initialiser le tableau de boules de feu
    for (int i = 0; i < MAX_FIREBALLS; i++) {
        fireballs[i].active = 0;
        fireballs[i].current_frame = 0;
        fireballs[i].frame_timer = 0;
        fireballs[i].animation_loops = 0;
    }
    
    // Charger les sprites
    printf("Chargement des sprites du boss à partir de: %s\n", TYPHON_IDLE);
    
    typhon.sprite_hurt = IMG_Load(TYPHON_HURT);
    if (!typhon.sprite_hurt) {
        printf("Error loading Typhon hurt sprite: %s\n", IMG_GetError());
        printf("Path: %s\n", TYPHON_HURT);
    } else {
        printf("Sprite HURT chargé: %dx%d\n", typhon.sprite_hurt->w, typhon.sprite_hurt->h);
    }
    
    typhon.sprite_attack_close = IMG_Load(TYPHON_ATTACK_CLOSE);
    if (!typhon.sprite_attack_close) {
        printf("Error loading Typhon close attack sprite: %s\n", IMG_GetError());
        printf("Path: %s\n", TYPHON_ATTACK_CLOSE);
    } else {
        printf("Sprite ATTACK_CLOSE chargé: %dx%d\n", typhon.sprite_attack_close->w, typhon.sprite_attack_close->h);
    }
    
    typhon.sprite_attack_range = IMG_Load(TYPHON_ATTACK_RANGE);
    if (!typhon.sprite_attack_range) {
        printf("Error loading Typhon range attack sprite: %s\n", IMG_GetError());
        printf("Path: %s\n", TYPHON_ATTACK_RANGE);
    } else {
        printf("Sprite ATTACK_RANGE chargé: %dx%d\n", typhon.sprite_attack_range->w, typhon.sprite_attack_range->h);
    }
    
    typhon.sprite_death = IMG_Load(TYPHON_DEATH);
    if (!typhon.sprite_death) {
        printf("Error loading Typhon death sprite: %s\n", IMG_GetError());
        printf("Path: %s\n", TYPHON_DEATH);
    } else {
        printf("Sprite DEATH chargé: %dx%d\n", typhon.sprite_death->w, typhon.sprite_death->h);
    }
    
    typhon.sprite_idle = IMG_Load(TYPHON_IDLE);
    if (!typhon.sprite_idle) {
        printf("Error loading Typhon idle sprite: %s\n", IMG_GetError());
        printf("Path: %s\n", TYPHON_IDLE);
    } else {
        printf("Sprite IDLE chargé: %dx%d\n", typhon.sprite_idle->w, typhon.sprite_idle->h);
    }
    
    typhon.sprite_walk = IMG_Load(TYPHON_WALK);
    if (!typhon.sprite_walk) {
        printf("Error loading Typhon walk sprite: %s\n", IMG_GetError());
        printf("Path: %s\n", TYPHON_WALK);
    } else {
        printf("Sprite WALK chargé: %dx%d\n", typhon.sprite_walk->w, typhon.sprite_walk->h);
    }
    
    // Créer les versions retournées des sprites
    printf("Création des versions retournées des sprites...\n");
    
    // Vérifier si les sprites originaux sont chargés avant de créer les versions flippées
    if (typhon.sprite_hurt) {
        typhon.sprite_hurt_flipped = load_and_flip_png(TYPHON_HURT);
    }
    if (typhon.sprite_attack_close) {
        typhon.sprite_attack_close_flipped = load_and_flip_png(TYPHON_ATTACK_CLOSE);
    }
    if (typhon.sprite_attack_range) {
        typhon.sprite_attack_range_flipped = load_and_flip_png(TYPHON_ATTACK_RANGE);
    }
    if (typhon.sprite_death) {
        typhon.sprite_death_flipped = load_and_flip_png(TYPHON_DEATH);
    }
    if (typhon.sprite_idle) {
        typhon.sprite_idle_flipped = load_and_flip_png(TYPHON_IDLE);
    }
    if (typhon.sprite_walk) {
        typhon.sprite_walk_flipped = load_and_flip_png(TYPHON_WALK);
    }
    
    // Charger la carte de l'arène du boss
    arena_background = IMG_Load("resources/typhonmap/typhonmap.png");
    if (!arena_background) {
        printf("Could not load boss arena background: %s\n", IMG_GetError());
        printf("Path: resources/typhonmap/typhonmap.png\n");
        
        // Utiliser le fond d'écran principal s'il n'est pas possible de charger celui de l'arène
        arena_background = game.background;
        printf("Utilisation du fond d'écran principal par défaut.\n");
    } else {
        printf("Boss arena background loaded successfully. Size: %dx%d\n", 
               arena_background->w, arena_background->h);
    }
    
    arena_collision_map = IMG_Load("resources/typhonmap/typhon-cm.png");
    if (!arena_collision_map) {
        printf("Could not load boss arena collision map: %s\n", IMG_GetError());
        printf("Path: resources/typhonmap/typhon-cm.png\n");
        
        // Utiliser la carte de collision principale s'il n'est pas possible de charger celle de l'arène
        arena_collision_map = game.collision_map;
        printf("Utilisation de la carte de collision principale par défaut.\n");
    } else {
        printf("Boss arena collision map loaded successfully. Size: %dx%d\n", 
               arena_collision_map->w, arena_collision_map->h);
    }
    
    // À la fin de la fonction init_typhon, ajoutez ces lignes pour afficher des informations sur la création des sprites flippés
    // Vérifier que les sprites flippés de Zeus/Typhon ont été correctement créés
    if (current_level_state == LEVEL_STATE_ZEUS_ARENA) {
        // Afficher des informations détaillées sur les sprites flippés de Zeus
        printf("Vérification des sprites flippés de Zeus:\n");
        printf("- Idle flippé: %s (dimensions: %dx%d)\n", 
               typhon.sprite_idle_flipped ? "OK" : "MANQUANT", 
               typhon.sprite_idle_flipped ? typhon.sprite_idle_flipped->w : 0,
               typhon.sprite_idle_flipped ? typhon.sprite_idle_flipped->h : 0);
        
        printf("- Walk flippé: %s (dimensions: %dx%d)\n", 
               typhon.sprite_walk_flipped ? "OK" : "MANQUANT",
               typhon.sprite_walk_flipped ? typhon.sprite_walk_flipped->w : 0,
               typhon.sprite_walk_flipped ? typhon.sprite_walk_flipped->h : 0);
        
        printf("- Attack Close flippé: %s (dimensions: %dx%d)\n", 
               typhon.sprite_attack_close_flipped ? "OK" : "MANQUANT",
               typhon.sprite_attack_close_flipped ? typhon.sprite_attack_close_flipped->w : 0,
               typhon.sprite_attack_close_flipped ? typhon.sprite_attack_close_flipped->h : 0);
        
        printf("- Attack Range flippé: %s (dimensions: %dx%d)\n", 
               typhon.sprite_attack_range_flipped ? "OK" : "MANQUANT",
               typhon.sprite_attack_range_flipped ? typhon.sprite_attack_range_flipped->w : 0,
               typhon.sprite_attack_range_flipped ? typhon.sprite_attack_range_flipped->h : 0);
        
        printf("- Hurt flippé: %s (dimensions: %dx%d)\n", 
               typhon.sprite_hurt_flipped ? "OK" : "MANQUANT",
               typhon.sprite_hurt_flipped ? typhon.sprite_hurt_flipped->w : 0,
               typhon.sprite_hurt_flipped ? typhon.sprite_hurt_flipped->h : 0);
        
        printf("- Death flippé: %s (dimensions: %dx%d)\n", 
               typhon.sprite_death_flipped ? "OK" : "MANQUANT",
               typhon.sprite_death_flipped ? typhon.sprite_death_flipped->w : 0,
               typhon.sprite_death_flipped ? typhon.sprite_death_flipped->h : 0);
    }
}

// Fonction pour nettoyer les ressources du boss Typhon
void cleanup_typhon() {
    SDL_FreeSurface(typhon.sprite_hurt);
    SDL_FreeSurface(typhon.sprite_attack_close);
    SDL_FreeSurface(typhon.sprite_attack_range);
    SDL_FreeSurface(typhon.sprite_death);
    SDL_FreeSurface(typhon.sprite_idle);
    SDL_FreeSurface(typhon.sprite_walk);
    
    SDL_FreeSurface(typhon.sprite_hurt_flipped);
    SDL_FreeSurface(typhon.sprite_attack_close_flipped);
    SDL_FreeSurface(typhon.sprite_attack_range_flipped);
    SDL_FreeSurface(typhon.sprite_death_flipped);
    SDL_FreeSurface(typhon.sprite_idle_flipped);
    SDL_FreeSurface(typhon.sprite_walk_flipped);
    
    SDL_FreeSurface(arena_background);
    SDL_FreeSurface(arena_collision_map);
    
    // Libérer la ressource de la boule de feu
    if (fireball_sprite) {
        SDL_FreeSurface(fireball_sprite);
        fireball_sprite = NULL;
    }
}

// Fonction pour mettre à jour le boss Typhon
void update_typhon() {
    if (!typhon.active) return;

    // Si le boss est en état de mort
    if (typhon.state == TYPHON_STATE_DEATH) {
        // Mise à jour de l'animation de mort
        typhon.frame_timer++;
        
        // Vitesse d'animation ajustée pour la mort (plus lente)
        int death_anim_speed = 8;
        int max_death_frames = (current_level_state == LEVEL_STATE_ZEUS_ARENA) ? 
                              ZEUS_DEATH_FRAMES : TYPHON_DEATH_FRAMES;
        
        // Afficher des informations de débogage sur l'animation de mort
        if (typhon.frame_timer % 10 == 0) {
            printf("Animation de mort: frame %d/%d, timer=%d\n", 
                   typhon.current_frame, max_death_frames-1, typhon.frame_timer);
        }
        
        // Faire avancer l'animation
        if (typhon.frame_timer >= death_anim_speed) {
            typhon.frame_timer = 0;
            typhon.current_frame++;
            
            // Vérifier si l'animation est terminée
            if (typhon.current_frame >= max_death_frames) {
                printf("Animation de mort terminée! (%d/%d)\n", 
                       typhon.current_frame, max_death_frames);
                
                // L'animation de mort est terminée, désactiver le boss pour qu'il disparaisse
                typhon.active = 0;
                printf("Boss désactivé! typhon.active = %d\n", typhon.active);
                       
                // Si c'est Typhon, démarrer la transition vers Zeus
                if (current_level_state == LEVEL_STATE_BOSS_ARENA && fade_state == FADE_STATE_NONE) {
                    printf("Typhon est vaincu! Début de la transition vers l'arène de Zeus\n");
                    fade_state = FADE_STATE_OUT;
                    fade_timer = 0;
                    transition_to_level = LEVEL_STATE_ZEUS_ARENA;
                }
                // Si c'est Zeus, considérer la fin du jeu
                else if (current_level_state == LEVEL_STATE_ZEUS_ARENA) {
                    printf("Zeus est vaincu! Fin du jeu!\n");
                    // Ici on pourrait ajouter un écran de fin ou revenir au menu
                }
                
                // Maintenir le boss dans la dernière frame mais le rendre invisible
                typhon.current_frame = max_death_frames - 1;
            }
        }
        
        return;
    }
    
    // Obtenir les bonnes constantes selon le boss actuel
    int attack_cooldown_value = (current_level_state == LEVEL_STATE_ZEUS_ARENA) ? 
                             ZEUS_ATTACK_COOLDOWN : TYPHON_ATTACK_COOLDOWN;
    int hurt_cooldown_value = (current_level_state == LEVEL_STATE_ZEUS_ARENA) ? 
                           ZEUS_HURT_COOLDOWN : TYPHON_HURT_COOLDOWN;
    int damage_close_value = (current_level_state == LEVEL_STATE_ZEUS_ARENA) ? 
                          ZEUS_DAMAGE_CLOSE : TYPHON_DAMAGE_CLOSE;
    int damage_range_value = (current_level_state == LEVEL_STATE_ZEUS_ARENA) ? 
                          ZEUS_DAMAGE_RANGE : TYPHON_DAMAGE_RANGE;
    
    // Réduire les cooldowns
    if (typhon.attack_cooldown > 0) {
        typhon.attack_cooldown--;
    }
    
    if (typhon.hurt_cooldown > 0) {
        typhon.hurt_cooldown--;
        // Si le cooldown de blessure est terminé, revenir à l'état d'inactivité
        if (typhon.hurt_cooldown == 0 && typhon.state == TYPHON_STATE_HURT) {
            typhon.state = TYPHON_STATE_IDLE;
        }
    }
    
    // Comportement du boss
    float dx = player.x - typhon.x;
    float dy = player.y - typhon.y;
    float dist = sqrt(dx*dx + dy*dy);
    
    // Débogage périodique
    if (SDL_GetTicks() % 120 == 0) {
        printf("Boss état: %d, position: (%.1f, %.1f), direction: %d, distance au joueur: %.1f\n", 
               typhon.state, typhon.x, typhon.y, typhon.direction, dist);
    }
    
    // Mise à jour de la direction en fonction de la position du joueur pour Zeus
    // Pour que Zeus se tourne correctement vers le joueur et utilise les sprites flippés
    if (current_level_state == LEVEL_STATE_ZEUS_ARENA) {
        // Logique inversée pour Zeus - une valeur positive de dx signifie que le joueur est à droite de Zeus
        // donc Zeus devrait regarder vers la droite (direction = 1)
        // Si dx est négatif, Zeus devrait regarder vers la gauche (direction = -1)
        int new_direction = (dx > 0) ? 1 : -1;
        
        // Si la direction change, l'afficher
        if (typhon.direction != new_direction) {
            printf("Zeus change de direction: %d -> %d (joueur à x=%.1f, dx=%.1f)\n", 
                   typhon.direction, new_direction, player.x, dx);
            
            // En mode attaque, ne pas changer de direction pour éviter les sauts d'animation
            if (typhon.state != TYPHON_STATE_ATTACK_CLOSE && typhon.state != TYPHON_STATE_ATTACK_RANGE) {
                typhon.direction = new_direction;
            }
        } else {
            // Direction inchangée, mise à jour simple
            typhon.direction = new_direction;
        }
    } else {
        // Pour Typhon, comportement original
        typhon.direction = (dx > 0) ? 1 : -1;
    }
    
    // Comportement basé sur l'état
    switch (typhon.state) {
        case TYPHON_STATE_IDLE:
            typhon.vx = 0;
            
            // Passer à l'état de marche si le joueur est loin
            if (dist > 200 && typhon.attack_cooldown == 0) {
                typhon.state = TYPHON_STATE_WALK;
            }
            // Attaque à distance si le joueur est à moyenne distance
            else if (dist > 100 && dist <= 800 && typhon.attack_cooldown == 0) {
                typhon.state = TYPHON_STATE_ATTACK_RANGE;
                typhon.current_frame = 0;
                typhon.attack_cooldown = attack_cooldown_value;
            }
            // Attaque rapprochée si le joueur est proche
            else if (dist <= 120 && typhon.attack_cooldown == 0) {
                typhon.state = TYPHON_STATE_ATTACK_CLOSE;
                typhon.current_frame = 0;
                typhon.attack_cooldown = attack_cooldown_value / 2;
            }
            break;
            
        case TYPHON_STATE_WALK:
            // Se déplacer vers le joueur
            // La vélocité est toujours dans la direction du joueur (gauche ou droite)
            typhon.vx = typhon.direction * 3;
            
            // Passer à l'attaque à distance si à moyenne distance
            if (dist <= 200 && typhon.attack_cooldown == 0) {
                typhon.state = TYPHON_STATE_ATTACK_RANGE;
                typhon.current_frame = 0;
                typhon.vx = 0;
                typhon.attack_cooldown = attack_cooldown_value;
            }
            // Passer à l'attaque rapprochée si très proche
            else if (dist <= 120 && typhon.attack_cooldown == 0) {
                typhon.state = TYPHON_STATE_ATTACK_CLOSE;
                typhon.current_frame = 0;
                typhon.vx = 0;
                typhon.attack_cooldown = attack_cooldown_value / 2;
            }
            break;
            
        case TYPHON_STATE_ATTACK_CLOSE:
        case TYPHON_STATE_ATTACK_RANGE:
            // Ne pas bouger pendant l'attaque
            typhon.vx = 0;
            break;
            
        case TYPHON_STATE_HURT:
            // Légèrement repoussé pendant la blessure
            typhon.vx = -typhon.direction * 1;
            break;
            
        default:
            break;
    }
    
    // Appliquer la gravité
    typhon.vy += GRAVITY;
    if (typhon.vy > MAX_FALL_SPEED) {
        typhon.vy = MAX_FALL_SPEED;
    }
    
    // Calculer la nouvelle position
    float new_x = typhon.x + typhon.vx;
    float new_y = typhon.y + typhon.vy;
    
    // Choisir les dimensions en fonction du niveau et de l'état
    int width_to_use;
    if (current_level_state == LEVEL_STATE_ZEUS_ARENA) {
        // Dimensions de Zeus
        switch (typhon.state) {
            case TYPHON_STATE_IDLE:
                width_to_use = ZEUS_IDLE_WIDTH;
                break;
            case TYPHON_STATE_WALK:
                width_to_use = ZEUS_WALK_WIDTH;
                break;
            case TYPHON_STATE_ATTACK_CLOSE:
                width_to_use = ZEUS_ATTACK_CLOSE_WIDTH;
                break;
            case TYPHON_STATE_ATTACK_RANGE:
                width_to_use = ZEUS_ATTACK_RANGE_WIDTH;
                break;
            case TYPHON_STATE_HURT:
                width_to_use = ZEUS_HURT_WIDTH;
                break;
            case TYPHON_STATE_DEATH:
                width_to_use = ZEUS_DEATH_WIDTH;
                break;
            default:
                width_to_use = ZEUS_IDLE_WIDTH;
        }
    } else {
        // Dimensions de Typhon
        switch (typhon.state) {
            case TYPHON_STATE_IDLE:
                width_to_use = TYPHON_IDLE_WIDTH;
                break;
            case TYPHON_STATE_WALK:
                width_to_use = TYPHON_WALK_WIDTH;
                break;
            case TYPHON_STATE_ATTACK_CLOSE:
                width_to_use = TYPHON_ATTACK_CLOSE_WIDTH;
                break;
            case TYPHON_STATE_ATTACK_RANGE:
                width_to_use = TYPHON_ATTACK_RANGE_WIDTH;
                break;
            case TYPHON_STATE_HURT:
                width_to_use = TYPHON_HURT_WIDTH;
                break;
            case TYPHON_STATE_DEATH:
                width_to_use = TYPHON_DEATH_WIDTH;
                break;
            default:
                width_to_use = TYPHON_IDLE_WIDTH;
        }
    }
    
    // Vérifier les limites de la carte
    if (new_x < 0) {
        new_x = 0;
    } else if (new_x + width_to_use > MAP_WIDTH) {
        new_x = MAP_WIDTH - width_to_use;
    }
    
    // Vérifier la collision avec le joueur avant de bouger horizontalement
    // On utilise les mêmes variables de hitbox que dans check_player_boss_collision
    int player_hitbox_x = player.x + 20;
    int player_hitbox_y = player.y + 5;
    int player_hitbox_width = SPRITE_WIDTH - 40;
    int player_hitbox_height = SPRITE_HEIGHT - 10; // Revenir à une taille raisonnable
    
    int boss_hitbox_x = new_x + 10;
    int boss_hitbox_y = typhon.y + 5;
    int boss_hitbox_width = width_to_use - 20;
    int boss_hitbox_height = TYPHON_IDLE_HEIGHT - 10; // Revenir à une taille raisonnable
    
    // Vérifier si le boss va entrer en collision avec le joueur
    int collision_with_player = 
        boss_hitbox_x < player_hitbox_x + player_hitbox_width &&
        boss_hitbox_x + boss_hitbox_width > player_hitbox_x &&
        boss_hitbox_y < player_hitbox_y + player_hitbox_height &&
        boss_hitbox_y + boss_hitbox_height > player_hitbox_y;
    
    // Collisions horizontales
    int left_collision = check_collision(game.collision_map, 
                                      (int)new_x + 20, 
                                      (int)typhon.y + TYPHON_IDLE_HEIGHT/2, // Revenir au milieu du corps
                                      0);
    
    int right_collision = check_collision(game.collision_map, 
                                       (int)new_x + width_to_use - 20, 
                                       (int)typhon.y + TYPHON_IDLE_HEIGHT/2, // Revenir au milieu du corps
                                       0);
    
    // Ne pas traverser le joueur, sauf pendant les attaques
    if (collision_with_player && typhon.state != TYPHON_STATE_ATTACK_CLOSE && typhon.state != TYPHON_STATE_ATTACK_RANGE && dist < 80) {
        // S'arrêter avant de traverser le joueur
        typhon.vx = 0;
        if (typhon.state == TYPHON_STATE_WALK) {
            typhon.state = TYPHON_STATE_IDLE;
            if (SDL_GetTicks() % 120 == 0) {
                printf("Typhon s'arrête: collision avec joueur à distance %.1f\n", dist);
            }
        }
    }
    // Mettre à jour la position horizontale si pas de collision
    else if ((left_collision != COLLISION_PLATFORM && left_collision != COLLISION_INVISIBLE) || typhon.vx > 0) {
        if ((right_collision != COLLISION_PLATFORM && right_collision != COLLISION_INVISIBLE) || typhon.vx < 0) {
            typhon.x = new_x;
            if (SDL_GetTicks() % 120 == 0 && typhon.vx != 0) {
                printf("Typhon se déplace: vx=%.1f, nouveau x=%.1f\n", typhon.vx, new_x);
            }
        } else {
            // Collision à droite
            typhon.vx = 0;
            typhon.state = TYPHON_STATE_IDLE;
            if (SDL_GetTicks() % 120 == 0) {
                printf("Typhon s'arrête: collision à droite\n");
            }
        }
    } else {
        // Collision à gauche
        typhon.vx = 0;
        typhon.state = TYPHON_STATE_IDLE;
        if (SDL_GetTicks() % 120 == 0) {
            printf("Typhon s'arrête: collision à gauche\n");
        }
    }
    
    // Vérifier les collisions verticales (chute)
    if (typhon.vy > 0) {
        // Vérifier le sol
        int bottom_left = check_collision(game.collision_map, 
                                       (int)typhon.x + 30, 
                                       (int)new_y + TYPHON_IDLE_HEIGHT, 
                                       0);
        
        int bottom_right = check_collision(game.collision_map, 
                                        (int)typhon.x + width_to_use - 30, 
                                        (int)new_y + TYPHON_IDLE_HEIGHT, 
                                        0);
        
        if (SDL_GetTicks() % 120 == 0) {
            printf("Vérification sol Typhon: bottom_left=%d, bottom_right=%d, y=%d\n", 
                   bottom_left, bottom_right, (int)new_y + TYPHON_IDLE_HEIGHT);
        }
        
        // Si collision avec le sol
        if (bottom_left == COLLISION_PLATFORM || bottom_right == COLLISION_PLATFORM || 
            bottom_left == COLLISION_PLATFORM_ONE_WAY || bottom_right == COLLISION_PLATFORM_ONE_WAY) {
            
            // Trouver la position exacte du sol
            int floor_y = (int)(new_y + TYPHON_IDLE_HEIGHT);
            while (floor_y > new_y && 
                  (check_collision(game.collision_map, (int)typhon.x + 30, floor_y, 0) != COLLISION_PLATFORM) &&
                  (check_collision(game.collision_map, (int)typhon.x + width_to_use - 30, floor_y, 0) != COLLISION_PLATFORM) &&
                  (check_collision(game.collision_map, (int)typhon.x + 30, floor_y, 0) != COLLISION_PLATFORM_ONE_WAY) &&
                  (check_collision(game.collision_map, (int)typhon.x + width_to_use - 30, floor_y, 0) != COLLISION_PLATFORM_ONE_WAY)) {
                floor_y--;
            }
            
            // Positionner le boss sur le sol
            typhon.y = floor_y - TYPHON_IDLE_HEIGHT;
            typhon.vy = 0;
        } else {
            // Pas de collision, continuer à tomber
            typhon.y = new_y;
        }
    } else if (typhon.vy < 0) {
        // Collision avec le plafond
        int top_collision = check_collision(game.collision_map, 
                                         (int)typhon.x + width_to_use/2, 
                                         (int)new_y, 
                                         0);
        
        if (top_collision == COLLISION_PLATFORM || top_collision == COLLISION_INVISIBLE) {
            // Arrêter le mouvement vers le haut
            typhon.vy = 0;
            // Trouver la position exacte du plafond
            typhon.y = ((int)new_y / TILE_SIZE + 1) * TILE_SIZE;
        } else {
            // Pas de collision, continuer à monter
            typhon.y = new_y;
        }
    } else {
        // Pas de mouvement vertical, simplement mettre à jour la position
        typhon.y = new_y;
    }
    
    // Mise à jour de l'animation
    int max_frames;
    int anim_speed;
    
    switch (typhon.state) {
        case TYPHON_STATE_IDLE:
            max_frames = TYPHON_IDLE_FRAMES;
            anim_speed = 12;
            break;
        case TYPHON_STATE_WALK:
            max_frames = TYPHON_WALK_FRAMES;
            anim_speed = 8;
            break;
        case TYPHON_STATE_ATTACK_CLOSE:
            max_frames = TYPHON_ATTACK_CLOSE_FRAMES;
            anim_speed = 6;
            break;
        case TYPHON_STATE_ATTACK_RANGE:
            max_frames = TYPHON_ATTACK_RANGE_FRAMES;
            anim_speed = 3;
            break;
        case TYPHON_STATE_HURT:
            max_frames = TYPHON_HURT_FRAMES;
            anim_speed = 5;
            break;
        case TYPHON_STATE_DEATH:
            max_frames = TYPHON_DEATH_FRAMES;
            anim_speed = 8;
            break;
        default:
            max_frames = 1;
            anim_speed = 10;
    }
    
    // Avancer l'animation
    typhon.frame_timer++;
    if (typhon.frame_timer >= anim_speed) {
        typhon.frame_timer = 0;
        typhon.current_frame++;
        
        // Vérifier la fin de l'animation
        if (typhon.current_frame >= max_frames) {
            if (typhon.state == TYPHON_STATE_DEATH) {
                // Laisser la logique dans la section TYPHON_STATE_DEATH gérer cela
                typhon.current_frame = max_frames - 1;
                // Ne pas désactiver ici, c'est géré dans la section spécifique à la mort
                // typhon.active = 0;  <- Commenté pour éviter la confusion
                
                if (DEBUG_HITBOXES) {
                    printf("Animation de mort terminée dans la section animation générale\n");
                }
            } else if (typhon.state == TYPHON_STATE_ATTACK_CLOSE) {
                // À la fin de l'animation d'attaque rapprochée, vérifier si le joueur est touché
                if (dist <= 120) {
                    damage_player(TYPHON_DAMAGE_CLOSE);
                }
                typhon.state = TYPHON_STATE_IDLE;
                typhon.current_frame = 0;
            } else if (typhon.state == TYPHON_STATE_ATTACK_RANGE) {
                // À la fin de l'animation d'attaque à distance, vérifier si le joueur est touché
                typhon.state = TYPHON_STATE_IDLE;
                typhon.current_frame = 0;
            } else {
                // Pour les autres animations, revenir au début
                typhon.current_frame = 0;
            }
        }
    }
    
    // Si dans l'état d'attaque à distance et au frame spécifique (entre 7 et 20), lancer une boule de feu
    if (typhon.state == TYPHON_STATE_ATTACK_RANGE && typhon.current_frame == 15) {
        // Position de départ de la boule de feu selon la direction du boss
        float fireball_x = typhon.x;
        float fireball_y = typhon.y + 60;  // Ajustement vertical pour que ça parte du centre
        
        if (typhon.direction == 1) {  // Boss regarde à droite
            fireball_x += TYPHON_ATTACK_RANGE_WIDTH - 10;
        } else {  // Boss regarde à gauche
            fireball_x += 10;
        }
        
        spawn_fireball(fireball_x, fireball_y, typhon.direction);
    }
    
    // Si dans l'état d'attaque à distance et à certains frames, lancer des boules de feu
    // Déclenchement des tirs à différents moments de l'animation
    if (typhon.state == TYPHON_STATE_ATTACK_RANGE) {
        // Diminuer le timer entre les tirs consécutifs
        if (fireball_timer > 0) {
            fireball_timer--;
        }
            
        // Générer des boules de feu aux frames 7, 12 et 17 avec un délai entre chaque
        if ((typhon.current_frame == 7 || typhon.current_frame == 12 || typhon.current_frame == 17) 
            && fireball_timer == 0) {
            // Position de départ de la boule de feu selon la direction du boss
            float fireball_x = typhon.x;
            float fireball_y = typhon.y + 60;  // Ajustement vertical pour que ça parte du centre
            
            if (typhon.direction == 1) {  // Boss regarde à droite
                fireball_x += TYPHON_ATTACK_RANGE_WIDTH - 10;
            } else {  // Boss regarde à gauche
                fireball_x += 10;
            }
            
            spawn_fireball(fireball_x, fireball_y, typhon.direction);
            
            // Réinitialiser le timer pour espacer les tirs
            fireball_timer = FIREBALL_DELAY;
        }
    }
}

// Fonction pour rendre le boss Typhon
void render_typhon() {
    // Vérifier que le boss est actif
    if (!typhon.active) {
        // Afficher un message de débogage occasionnel
        if (SDL_GetTicks() % 300 == 0) {
            printf("Boss inactif, aucun rendu effectué (typhon.active = %d)\n", typhon.active);
        }
        return;
    }
    
    // Afficher des informations de débogage périodiquement, pas à chaque frame
    if (SDL_GetTicks() % 300 == 0) {
        printf("Rendu du boss à la position x=%.1f, y=%.1f (camera: %.1f, %.1f), état=%d, frame=%d, direction=%d\n", 
               typhon.x, typhon.y, game.camera_x, game.camera_y, typhon.state, typhon.current_frame, typhon.direction);
    }
    
    SDL_Surface* current_sprite = NULL;
    int frame_width;
    int frame_height;
    
    // Sélectionner le sprite en fonction de l'état, de la direction et du boss (Zeus ou Typhon)
    switch (typhon.state) {
        case TYPHON_STATE_IDLE:
            current_sprite = typhon.direction == 1 ? typhon.sprite_idle : typhon.sprite_idle_flipped;
            frame_width = current_level_state == LEVEL_STATE_ZEUS_ARENA ? ZEUS_IDLE_WIDTH : TYPHON_IDLE_WIDTH;
            frame_height = current_level_state == LEVEL_STATE_ZEUS_ARENA ? ZEUS_IDLE_HEIGHT : TYPHON_IDLE_HEIGHT;
            break;
        case TYPHON_STATE_WALK:
            current_sprite = typhon.direction == 1 ? typhon.sprite_walk : typhon.sprite_walk_flipped;
            frame_width = current_level_state == LEVEL_STATE_ZEUS_ARENA ? ZEUS_WALK_WIDTH : TYPHON_WALK_WIDTH;
            frame_height = current_level_state == LEVEL_STATE_ZEUS_ARENA ? ZEUS_WALK_HEIGHT : TYPHON_WALK_HEIGHT;
            break;
        case TYPHON_STATE_ATTACK_CLOSE:
            current_sprite = typhon.direction == 1 ? typhon.sprite_attack_close : typhon.sprite_attack_close_flipped;
            frame_width = current_level_state == LEVEL_STATE_ZEUS_ARENA ? ZEUS_ATTACK_CLOSE_WIDTH : TYPHON_ATTACK_CLOSE_WIDTH;
            frame_height = current_level_state == LEVEL_STATE_ZEUS_ARENA ? ZEUS_ATTACK_CLOSE_HEIGHT : TYPHON_ATTACK_CLOSE_HEIGHT;
            break;
        case TYPHON_STATE_ATTACK_RANGE:
            current_sprite = typhon.direction == 1 ? typhon.sprite_attack_range : typhon.sprite_attack_range_flipped;
            frame_width = current_level_state == LEVEL_STATE_ZEUS_ARENA ? ZEUS_ATTACK_RANGE_WIDTH : TYPHON_ATTACK_RANGE_WIDTH;
            frame_height = current_level_state == LEVEL_STATE_ZEUS_ARENA ? ZEUS_ATTACK_RANGE_HEIGHT : TYPHON_ATTACK_RANGE_HEIGHT;
            break;
        case TYPHON_STATE_HURT:
            current_sprite = typhon.direction == 1 ? typhon.sprite_hurt : typhon.sprite_hurt_flipped;
            frame_width = current_level_state == LEVEL_STATE_ZEUS_ARENA ? ZEUS_HURT_WIDTH : TYPHON_HURT_WIDTH;
            frame_height = current_level_state == LEVEL_STATE_ZEUS_ARENA ? ZEUS_HURT_HEIGHT : TYPHON_HURT_HEIGHT;
            break;
        case TYPHON_STATE_DEATH:
            current_sprite = current_level_state == LEVEL_STATE_ZEUS_ARENA ? 
                (typhon.direction == 1 ? typhon.sprite_death : typhon.sprite_death_flipped) : 
                (typhon.direction == -1 ? typhon.sprite_death : typhon.sprite_death_flipped);
            frame_width = current_level_state == LEVEL_STATE_ZEUS_ARENA ? ZEUS_DEATH_WIDTH : TYPHON_DEATH_WIDTH;
            frame_height = current_level_state == LEVEL_STATE_ZEUS_ARENA ? ZEUS_DEATH_HEIGHT : TYPHON_DEATH_HEIGHT;
            
            // Afficher des informations spécifiques pour l'animation de mort
            if (SDL_GetTicks() % 60 == 0) {
                int max_frames = current_level_state == LEVEL_STATE_ZEUS_ARENA ? ZEUS_DEATH_FRAMES : TYPHON_DEATH_FRAMES;
                printf("Animation de mort rendue: frame=%d/%d\n", typhon.current_frame, max_frames-1);
            }
            break;
        default:
            break;
    }
    
    // Rectangle destination pour l'affichage
    SDL_Rect dest_rect = {
        (int)(typhon.x - game.camera_x),
        (int)(typhon.y - game.camera_y),
        frame_width,
        frame_height
    };
    
    // Vérifier que le boss est visible à l'écran seulement en mode débogage
    if (DEBUG_HITBOXES) {
        if (dest_rect.x + dest_rect.w < 0 || dest_rect.x > SCREEN_WIDTH || 
            dest_rect.y + dest_rect.h < 0 || dest_rect.y > SCREEN_HEIGHT) {
            printf("Boss hors écran: dest_rect = (%d, %d, %d, %d)\n", 
                   dest_rect.x, dest_rect.y, dest_rect.w, dest_rect.h);
        } else {
            printf("Boss visible à l'écran: dest_rect = (%d, %d, %d, %d)\n", 
                   dest_rect.x, dest_rect.y, dest_rect.w, dest_rect.h);
        }
    }
    
    // Vérifier si le sprite est valide
    if (!current_sprite) {
        if (DEBUG_HITBOXES) {
            printf("ERREUR: Sprite du boss non disponible - Affichage d'un rectangle de substitution\n");
            
            // Dessiner un rectangle de substitution de couleur rouge
            SDL_Rect debug_rect = {dest_rect.x, dest_rect.y, 100, 150};
            SDL_FillRect(game.screen, &debug_rect, SDL_MapRGB(game.screen->format, 255, 0, 0));
            
            // Dessiner un cadre pour mieux voir la position du boss
            SDL_Rect outline_rects[4] = {
                {dest_rect.x, dest_rect.y, 100, 2},               // Haut
                {dest_rect.x, dest_rect.y + 148, 100, 2},         // Bas
                {dest_rect.x, dest_rect.y, 2, 150},               // Gauche
                {dest_rect.x + 98, dest_rect.y, 2, 150}           // Droite
            };
            
            for (int i = 0; i < 4; i++) {
                SDL_FillRect(game.screen, &outline_rects[i], SDL_MapRGB(game.screen->format, 255, 255, 255));
            }
        }
    }
    else if (current_sprite->w == 0 || current_sprite->h == 0) {
        if (DEBUG_HITBOXES) {
            printf("ERREUR: Dimensions du sprite du boss invalides (%dx%d)\n", 
                   current_sprite->w, current_sprite->h);
            
            // Dessiner un rectangle jaune pour les sprites mal dimensionnés
            SDL_Rect debug_rect = {dest_rect.x, dest_rect.y, 100, 150};
            SDL_FillRect(game.screen, &debug_rect, SDL_MapRGB(game.screen->format, 255, 255, 0));
        }
    }
    else {
        // Nombre maximal de frames selon l'animation et le boss actuel
        int max_frames;
        if (current_level_state == LEVEL_STATE_ZEUS_ARENA) {
            switch (typhon.state) {
                case TYPHON_STATE_IDLE:
                    max_frames = ZEUS_IDLE_FRAMES;
                    break;
                case TYPHON_STATE_WALK:
                    max_frames = ZEUS_WALK_FRAMES;
                    break;
                case TYPHON_STATE_ATTACK_CLOSE:
                    max_frames = ZEUS_ATTACK_CLOSE_FRAMES;
                    break;
                case TYPHON_STATE_ATTACK_RANGE:
                    max_frames = ZEUS_ATTACK_RANGE_FRAMES;
                    break;
                case TYPHON_STATE_HURT:
                    max_frames = ZEUS_HURT_FRAMES;
                    break;
                case TYPHON_STATE_DEATH:
                    max_frames = ZEUS_DEATH_FRAMES;
                    break;
                default:
                    max_frames = 1;
            }
        } else {
            switch (typhon.state) {
                case TYPHON_STATE_IDLE:
                    max_frames = TYPHON_IDLE_FRAMES;
                    break;
                case TYPHON_STATE_WALK:
                    max_frames = TYPHON_WALK_FRAMES;
                    break;
                case TYPHON_STATE_ATTACK_CLOSE:
                    max_frames = TYPHON_ATTACK_CLOSE_FRAMES;
                    break;
                case TYPHON_STATE_ATTACK_RANGE:
                    max_frames = TYPHON_ATTACK_RANGE_FRAMES;
                    break;
                case TYPHON_STATE_HURT:
                    max_frames = TYPHON_HURT_FRAMES;
                    break;
                case TYPHON_STATE_DEATH:
                    max_frames = TYPHON_DEATH_FRAMES;
                    break;
                default:
                    max_frames = 1;
            }
        }
        
        // S'assurer que la frame actuelle est valide pour l'animation
        if (typhon.current_frame >= max_frames) {
            // Pour l'animation de mort, on reste sur la dernière frame
            if (typhon.state == TYPHON_STATE_DEATH) {
                typhon.current_frame = max_frames - 1;
            } else {
                typhon.current_frame = 0;
            }
        }
        
        // Calculer le rectangle source pour l'animation
        SDL_Rect src_rect = {
            typhon.current_frame * frame_width,
            0,
            frame_width,
            frame_height
        };
        
        // S'assurer que le rectangle source reste dans les limites du sprite
        if (src_rect.x + src_rect.w > current_sprite->w) {
            if (DEBUG_HITBOXES) {
                printf("ATTENTION: Animation hors limites, ajustement du rectangle source\n");
            }
            src_rect.x = 0;
        }
        
        // Dessiner le sprite normalement
        SDL_BlitSurface(current_sprite, &src_rect, game.screen, &dest_rect);
        
        // En mode debug, afficher des indications sur la direction
        if (DEBUG_HITBOXES) {
            // Dessiner une ligne indiquant la direction du boss
            SDL_Rect direction_rect = {
                dest_rect.x + frame_width / 2,
                dest_rect.y + frame_height / 2,
                typhon.direction * 30,
                2
            };
            if (typhon.direction < 0) {
                direction_rect.x += direction_rect.w;
                direction_rect.w = -direction_rect.w;
            }
            SDL_FillRect(game.screen, &direction_rect, SDL_MapRGB(game.screen->format, 0, 255, 255));
        }
    }
}

// Fonction pour infliger des dégâts au boss Typhon
void damage_typhon(int amount) {
    if (typhon.hurt_cooldown > 0 || typhon.state == TYPHON_STATE_DEATH) return;
    
    typhon.health -= amount;
    
    if (typhon.health <= 0) {
        typhon.health = 0;
        typhon.state = TYPHON_STATE_DEATH;
        typhon.current_frame = 0;
        
        // Enregistrer le moment où le boss est vaincu
        boss_battle_end_time = SDL_GetTicks();
        boss_defeated = 1;
        
        // Calculer le score basé sur le temps de combat
        Uint32 battle_duration = (boss_battle_end_time - boss_battle_start_time) / 1000; // en secondes
        int time_bonus = 0;
        
        if (battle_duration <= BOSS_TIME_BONUS_THRESHOLD) {
            // Score maximum si le combat est terminé rapidement
            time_bonus = BOSS_TIME_BONUS_MAX;
        } else {
            // Réduction du bonus en fonction du temps écoulé (limité à 0)
            int overtime = battle_duration - BOSS_TIME_BONUS_THRESHOLD;
            time_bonus = BOSS_TIME_BONUS_MAX - (overtime * 50); // Réduction de 50 points par seconde supplémentaire
            if (time_bonus < 0) time_bonus = 0;
        }
        
        // Ajouter les points au score total
        player_score += BOSS_KILL_BASE_POINTS + time_bonus;
        
        printf("Boss vaincu en %u secondes! Score obtenu: %d (Base: %d, Bonus temps: %d)\n", 
               battle_duration, BOSS_KILL_BASE_POINTS + time_bonus, BOSS_KILL_BASE_POINTS, time_bonus);
    } else {
        typhon.state = TYPHON_STATE_HURT;
        typhon.current_frame = 0;
        typhon.hurt_cooldown = TYPHON_HURT_COOLDOWN;
    }
}

// Fonction pour vérifier si une attaque du joueur touche le boss
void check_player_attack_typhon() {
    if (!in_boss_arena || !typhon.active || typhon.state == TYPHON_STATE_DEATH || !player.is_attacking) return;
    
    // Vérifier que le joueur est au bon moment de l'animation d'attaque (au milieu de l'animation)
    // Cela rend l'attaque plus intuitive en la synchronisant avec le moment où l'épée est déployée
    if (player.current_frame < ATTACK_FRAMES / 3 || player.current_frame > ATTACK_FRAMES * 2 / 3) {
        return;
    }
    
    // Zone d'attaque devant le joueur avec une hitbox plus grande et mieux positionnée
    int attack_width = 80;  // Maintenir une bonne portée
    int attack_height = 80; // Maintenir une bonne portée verticale
    
    // Position de la zone d'attaque en fonction de la direction du joueur
    int attack_x;
    if (player.direction == 1) { // Face à droite
        attack_x = player.x + SPRITE_WIDTH - 40; // Légèrement ajusté pour mieux atteindre le boss
    } else { // Face à gauche
        attack_x = player.x - attack_width + 40; // Légèrement ajusté pour mieux atteindre le boss
    }
    int attack_y = player.y + 10; // Positionné plus haut pour mieux couvrir le sprite du boss
    
    // Log pour le débogage
    if (SDL_GetTicks() % 300 == 0 && player.is_attacking) {
        printf("Hitbox d'attaque: x=%d, y=%d, w=%d, h=%d\n", 
               attack_x, attack_y, attack_width, attack_height);
    }
    
    // Déterminer les dimensions du boss selon le niveau
    int boss_width, boss_height;
    
    if (current_level_state == LEVEL_STATE_ZEUS_ARENA) {
        // Dimensions de Zeus selon son état
        switch (typhon.state) {
            case TYPHON_STATE_IDLE:
                boss_width = ZEUS_IDLE_WIDTH;
                boss_height = ZEUS_IDLE_HEIGHT;
                break;
            case TYPHON_STATE_WALK:
                boss_width = ZEUS_WALK_WIDTH;
                boss_height = ZEUS_WALK_HEIGHT;
                break;
            case TYPHON_STATE_ATTACK_CLOSE:
                boss_width = ZEUS_ATTACK_CLOSE_WIDTH;
                boss_height = ZEUS_ATTACK_CLOSE_HEIGHT;
                break;
            case TYPHON_STATE_ATTACK_RANGE:
                boss_width = ZEUS_ATTACK_RANGE_WIDTH;
                boss_height = ZEUS_ATTACK_RANGE_HEIGHT;
                break;
            case TYPHON_STATE_HURT:
                boss_width = ZEUS_HURT_WIDTH;
                boss_height = ZEUS_HURT_HEIGHT;
                break;
            case TYPHON_STATE_DEATH:
                boss_width = ZEUS_DEATH_WIDTH;
                boss_height = ZEUS_DEATH_HEIGHT;
                break;
            default:
                boss_width = ZEUS_IDLE_WIDTH;
                boss_height = ZEUS_IDLE_HEIGHT;
        }
    } else {
        // Dimensions de Typhon selon son état
        switch (typhon.state) {
            case TYPHON_STATE_IDLE:
                boss_width = TYPHON_IDLE_WIDTH;
                boss_height = TYPHON_IDLE_HEIGHT;
                break;
            case TYPHON_STATE_WALK:
                boss_width = TYPHON_WALK_WIDTH;
                boss_height = TYPHON_WALK_HEIGHT;
                break;
            case TYPHON_STATE_ATTACK_CLOSE:
                boss_width = TYPHON_ATTACK_CLOSE_WIDTH;
                boss_height = TYPHON_ATTACK_CLOSE_HEIGHT;
                break;
            case TYPHON_STATE_ATTACK_RANGE:
                boss_width = TYPHON_ATTACK_RANGE_WIDTH;
                boss_height = TYPHON_ATTACK_RANGE_HEIGHT;
                break;
            case TYPHON_STATE_HURT:
                boss_width = TYPHON_HURT_WIDTH;
                boss_height = TYPHON_HURT_HEIGHT;
                break;
            case TYPHON_STATE_DEATH:
                boss_width = TYPHON_DEATH_WIDTH;
                boss_height = TYPHON_DEATH_HEIGHT;
                break;
            default:
                boss_width = TYPHON_IDLE_WIDTH;
                boss_height = TYPHON_IDLE_HEIGHT;
                break;
        }
    }
    
    // Ajuster la hitbox du boss en fonction de sa direction
    // Si le boss regarde vers la gauche (direction = -1), décaler la hitbox
    int offset_x = 0;
    if (typhon.direction == -1) {
        // Décaler la hitbox vers la gauche quand le boss regarde à gauche
        // pour tenir compte du fait que le sprite est retourné
        offset_x = 20;
    }
    
    // Zone de collision du boss adaptée à sa direction
    int boss_hitbox_x = typhon.x + 15 - offset_x;
    int boss_hitbox_y = typhon.y + 5;
    int boss_hitbox_width = boss_width - 30;
    int boss_hitbox_height = boss_height - 10;
    
    // Afficher les hitboxes en mode debug
    if (DEBUG_HITBOXES) {
        // Hitbox de l'attaque (bleu)
        SDL_Rect attack_hitbox = {
            attack_x - game.camera_x,
            attack_y - game.camera_y,
            attack_width,
            attack_height
        };
        SDL_FillRect(game.screen, &attack_hitbox, SDL_MapRGB(game.screen->format, 0, 0, 255));
        
        // Hitbox du boss (rouge)
        SDL_Rect boss_hitbox = {
            boss_hitbox_x - game.camera_x,
            boss_hitbox_y - game.camera_y,
            boss_hitbox_width,
            boss_hitbox_height
        };
        SDL_FillRect(game.screen, &boss_hitbox, SDL_MapRGB(game.screen->format, 255, 0, 0));
        
        // Afficher des informations sur la hitbox du boss
        if (SDL_GetTicks() % 30 == 0) {
            printf("Boss hitbox: x=%d, y=%d, w=%d, h=%d, direction=%d\n", 
                   boss_hitbox_x, boss_hitbox_y, boss_hitbox_width, boss_hitbox_height, 
                   typhon.direction);
        }
    }
    
    // Vérifier si le joueur est assez proche du boss pour l'attaquer
    // Cela empêche les attaques à travers les murs ou à distance irréaliste
    float distance_x = fabs((player.x + SPRITE_WIDTH/2) - (typhon.x + boss_width/2));
    if (distance_x > 250) { // Augmenté pour permettre des attaques à distance plus grande
        return;
    }
    
    // Vérifier la collision (AABB) entre la zone d'attaque et le boss
    if (attack_x < boss_hitbox_x + boss_hitbox_width &&
        attack_x + attack_width > boss_hitbox_x &&
        attack_y < boss_hitbox_y + boss_hitbox_height &&
        attack_y + attack_height > boss_hitbox_y) {
        
        printf("ATTAQUE RÉUSSIE CONTRE LE BOSS! Direction du boss: %d\n", typhon.direction);
        damage_typhon(PLAYER_ATTACK_DAMAGE);
    }
}

// Fonction pour gérer la transition vers l'arène du boss
void check_boss_arena_transition() {
    // Ne rien faire si déjà dans l'arène du boss
    if (current_level_state == LEVEL_STATE_BOSS_ARENA) {
        return;
    }
    
    // Déboggage pour vérifier la position
    if (player.x > BOSS_ARENA_X - 100 && player.x < BOSS_ARENA_X + 100) {
        printf("Approche de l'arène du boss: joueur x=%.1f, seuil=%d\n", player.x, BOSS_ARENA_X);
    }
    
    // Vérifier si le joueur atteint la position pour déclencher le combat
    if (player.x >= BOSS_ARENA_X) {
        printf("Transition vers l'arène du boss activée! (x=%.1f)\n", player.x);
        
        // Utiliser la fonction load_level pour charger l'arène du boss avec la logique de collision simplifiée
        load_level(LEVEL_STATE_BOSS_ARENA);
        
        printf("Transition vers l'arène du boss terminée. Sol à y=890.\n");
    }
}

// Fonction pour charger les ressources d'un niveau spécifique
void load_level(int level_state) {
    // Sauvegarder l'état actuel avant de passer au nouveau niveau
    int previous_state = current_level_state;
    
    // Mettre à jour l'état du niveau
    current_level_state = level_state;
    
    
    printf("Transition de niveau: %d -> %d\n", previous_state, current_level_state);
    
    // Charger les ressources en fonction du niveau
    if (level_state == LEVEL_STATE_MAIN_PARKOUR) {
        // Level 1 Part 1: Main parkour - Charger les ressources du parcours principal
        printf("Chargement du niveau 1 partie 1: Parcours principal\n");
        
        // Charger/recharger le fond et la carte de collision
        if (game.background && game.background != IMG_Load("resources/cs.png")) {
            SDL_FreeSurface(game.background);
        }
        game.background = IMG_Load("resources/cs.png");
        
        if (game.collision_map && game.collision_map != IMG_Load("resources/cscm1.png")) {
            SDL_FreeSurface(game.collision_map);
        }
        game.collision_map = IMG_Load("resources/cscm1.png");
        
        // Vérifier le chargement des ressources
        if (!game.background || !game.collision_map) {
            printf("ERREUR: Impossible de charger les ressources du niveau principal!\n");
            return;
        }
        
        // Initialiser les ennemis si nécessaire
        if (previous_state == LEVEL_STATE_BOSS_ARENA || previous_state == LEVEL_STATE_ZEUS_ARENA) {
            init_enemies();
        }
        
        // Positionner le joueur au début du niveau ou à un checkpoint
        if (previous_state == -1) { // Premier chargement
            player.x = 30;
            player.y = 700;
        }
        
        // Mettre à jour la variable in_boss_arena pour la compatibilité
        in_boss_arena = 0;
    }
    else if (level_state == LEVEL_STATE_BOSS_ARENA) {
        // Level 1 Part 2: Boss arena - Charger les ressources de l'arène du boss
        printf("Chargement du niveau 1 partie 2: Arène du boss Typhon\n");
        
        // Initialiser le boss
        init_typhon();
        
        // Charger uniquement le fond de l'arène (pas de carte de collision)
        if (!arena_background) {
            printf("ERREUR: Fond de l'arène manquant!\n");
            // Revenir au niveau précédent
            current_level_state = previous_state;
            return;
        }
        
        // Mettre à jour les ressources du jeu
        if (game.background != arena_background) {
            SDL_FreeSurface(game.background);
            game.background = arena_background;
        }
        
        // Nous n'utilisons plus la carte de collision pour l'arène du boss
        printf("Utilisation d'une logique de collision simplifiée pour l'arène du boss (sol à y=890)\n");
        
        // Positionner le joueur au début de l'arène, juste au-dessus du sol
        player.x = 300; // Ajusté pour être dans les limites (entre 200 et 3790)
        player.y = 800; // Au-dessus du sol à y=890
        player.vx = 0;
        player.vy = 0;
        
        // Positionner le boss au-dessus du sol, vers la droite de l'arène
        typhon.x = 3000; // Moins loin que la limite maximum pour permettre de l'attaquer
        typhon.y = 750; // Ajusté pour être au-dessus du sol
        
        // Mettre à jour la variable in_boss_arena pour la compatibilité
        in_boss_arena = 1;
        
        // Démarrer le chronomètre pour le combat de boss
        boss_battle_start_time = SDL_GetTicks();
        boss_defeated = 0;
        
        // Initialiser la caméra pour qu'elle suive le joueur
        // La fonction update_camera s'occupera des limites appropriées
        update_camera(player.x, player.y);
        
        printf("Arène du boss initialisée - Limites de caméra: x=[0, 2550], y=280, limites du joueur: x=[200, 3790]\n");
    }
    else if (level_state == LEVEL_STATE_ZEUS_ARENA) {
        // Level 2: Zeus arena - Charger les ressources de l'arène de Zeus
        printf("Chargement du niveau 2: Arène du boss Zeus\n");
        
        // Charger le fond de l'arène de Zeus
        SDL_Surface* zeus_background = IMG_Load("resources/background_zeus_battle.png");
        if (!zeus_background) {
            printf("ERREUR: Impossible de charger le fond de l'arène de Zeus!\n");
            printf("Path: resources/background_zeus_battle.png\n");
            // Utiliser le fond d'écran principal par défaut
            zeus_background = game.background;
            printf("Utilisation du fond d'écran principal par défaut.\n");
        } else {
            printf("Fond de l'arène de Zeus chargé avec succès. Taille: %dx%d\n", 
                   zeus_background->w, zeus_background->h);
        }
        
        // Mettre à jour les ressources du jeu
        if (game.background != zeus_background) {
            if (game.background && game.background != arena_background) {
                SDL_FreeSurface(game.background);
            }
            game.background = zeus_background;
        }
        
        // Réinitialiser le joueur à 100% de santé pour le nouveau combat
        player.health = PLAYER_MAX_HEALTH;
        
        // Initialiser Zeus avec la même structure que Typhon
        // Mais en utilisant les constantes pour Zeus
        typhon.x = 3000;
        typhon.y = 750;
        typhon.vx = 0;
        typhon.vy = 0;
        typhon.health = ZEUS_HEALTH;
        typhon.max_health = ZEUS_HEALTH;
        typhon.active = 1;
        typhon.direction = -1;  // Direction vers la droite (1) pour Zeus
        typhon.state = TYPHON_STATE_IDLE;
        typhon.current_frame = 0;
        typhon.frame_timer = 0;
        typhon.attack_cooldown = 0;
        typhon.hurt_cooldown = 0;
        
        // Libérer les anciennes textures
        if (typhon.sprite_hurt) SDL_FreeSurface(typhon.sprite_hurt);
        if (typhon.sprite_attack_close) SDL_FreeSurface(typhon.sprite_attack_close);
        if (typhon.sprite_attack_range) SDL_FreeSurface(typhon.sprite_attack_range);
        if (typhon.sprite_death) SDL_FreeSurface(typhon.sprite_death);
        if (typhon.sprite_idle) SDL_FreeSurface(typhon.sprite_idle);
        if (typhon.sprite_walk) SDL_FreeSurface(typhon.sprite_walk);
        
        if (typhon.sprite_hurt_flipped) SDL_FreeSurface(typhon.sprite_hurt_flipped);
        if (typhon.sprite_attack_close_flipped) SDL_FreeSurface(typhon.sprite_attack_close_flipped);
        if (typhon.sprite_attack_range_flipped) SDL_FreeSurface(typhon.sprite_attack_range_flipped);
        if (typhon.sprite_death_flipped) SDL_FreeSurface(typhon.sprite_death_flipped);
        if (typhon.sprite_idle_flipped) SDL_FreeSurface(typhon.sprite_idle_flipped);
        if (typhon.sprite_walk_flipped) SDL_FreeSurface(typhon.sprite_walk_flipped);
        
        // Charger les sprites de Zeus
        printf("Chargement des sprites de Zeus à partir de: %s\n", ZEUS_IDLE);
        
        typhon.sprite_hurt = IMG_Load(ZEUS_HURT);
        typhon.sprite_attack_close = IMG_Load(ZEUS_ATTACK_CLOSE);
        typhon.sprite_attack_range = IMG_Load(ZEUS_ATTACK_RANGE);
        typhon.sprite_death = IMG_Load(ZEUS_DEATH);
        typhon.sprite_idle = IMG_Load(ZEUS_IDLE);
        typhon.sprite_walk = IMG_Load(ZEUS_WALK);
        
        // Vérifier le chargement des sprites
        if (!typhon.sprite_hurt || !typhon.sprite_attack_close || !typhon.sprite_attack_range ||
            !typhon.sprite_death || !typhon.sprite_idle || !typhon.sprite_walk) {
            printf("ERREUR: Certains sprites de Zeus n'ont pas pu être chargés!\n");
            // Afficher des informations détaillées sur les sprites manquants
            if (!typhon.sprite_hurt) printf("  - Sprite HURT manquant\n");
            if (!typhon.sprite_attack_close) printf("  - Sprite ATTACK_CLOSE manquant\n");
            if (!typhon.sprite_attack_range) printf("  - Sprite ATTACK_RANGE manquant\n");
            if (!typhon.sprite_death) printf("  - Sprite DEATH manquant\n");
            if (!typhon.sprite_idle) printf("  - Sprite IDLE manquant\n");
            if (!typhon.sprite_walk) printf("  - Sprite WALK manquant\n");
        }
        
        // Créer les versions flippées
        printf("Création des versions flippées des sprites de Zeus...\n");
        
        // Vérifier d'abord si les sprites originaux sont valides avant de créer les versions flippées
        if (typhon.sprite_hurt) {
            typhon.sprite_hurt_flipped = load_and_flip_png(ZEUS_HURT);
            if (!typhon.sprite_hurt_flipped) {
                printf("ERREUR: Impossible de créer le sprite HURT flippé de Zeus\n");
            }
        }
        
        if (typhon.sprite_attack_close) {
            typhon.sprite_attack_close_flipped = load_and_flip_png(ZEUS_ATTACK_CLOSE);
            if (!typhon.sprite_attack_close_flipped) {
                printf("ERREUR: Impossible de créer le sprite ATTACK_CLOSE flippé de Zeus\n");
            }
        }
        
        if (typhon.sprite_attack_range) {
            typhon.sprite_attack_range_flipped = load_and_flip_png(ZEUS_ATTACK_RANGE);
            if (!typhon.sprite_attack_range_flipped) {
                printf("ERREUR: Impossible de créer le sprite ATTACK_RANGE flippé de Zeus\n");
            }
        }
        
        if (typhon.sprite_death) {
            typhon.sprite_death_flipped = load_and_flip_png(ZEUS_DEATH);
            if (!typhon.sprite_death_flipped) {
                printf("ERREUR: Impossible de créer le sprite DEATH flippé de Zeus\n");
            }
        }
        
        if (typhon.sprite_idle) {
            typhon.sprite_idle_flipped = load_and_flip_png(ZEUS_IDLE);
            if (!typhon.sprite_idle_flipped) {
                printf("ERREUR: Impossible de créer le sprite IDLE flippé de Zeus\n");
            }
        }
        
        if (typhon.sprite_walk) {
            typhon.sprite_walk_flipped = load_and_flip_png(ZEUS_WALK);
            if (!typhon.sprite_walk_flipped) {
                printf("ERREUR: Impossible de créer le sprite WALK flippé de Zeus\n");
            }
        }
        
        // Vérification finale des sprites flippés
        printf("Vérification des dimensions des sprites flippés de Zeus:\n");
        if (typhon.sprite_idle_flipped) {
            printf("- Idle flippé: %dx%d\n", typhon.sprite_idle_flipped->w, typhon.sprite_idle_flipped->h);
        }
        if (typhon.sprite_walk_flipped) {
            printf("- Walk flippé: %dx%d\n", typhon.sprite_walk_flipped->w, typhon.sprite_walk_flipped->h);
        }
        if (typhon.sprite_attack_close_flipped) {
            printf("- Attack Close flippé: %dx%d\n", typhon.sprite_attack_close_flipped->w, typhon.sprite_attack_close_flipped->h);
        }
        if (typhon.sprite_attack_range_flipped) {
            printf("- Attack Range flippé: %dx%d\n", typhon.sprite_attack_range_flipped->w, typhon.sprite_attack_range_flipped->h);
        }
        
        // Positionner le joueur au début de l'arène
        player.x = 300;
        player.y = MAP_HEIGHT - 200; // Positionnement au-dessus du sol qui est à 100px du bas
        player.vx = 0;
        player.vy = 0;
        
        // Mettre à jour la variable in_boss_arena
        in_boss_arena = 1;
        
        // Réinitialiser le timer du combat
        boss_battle_start_time = SDL_GetTicks();
        boss_defeated = 0;
        
        printf("Arène de Zeus initialisée - Sol à %d pixels du bas de l'écran\n", 100);
    }
    
    printf("Niveau %d chargé avec succès\n", level_state);
}

void reset_player() {
    player.x = 50;
    player.y = 800;
    player.is_running = 0;        // au lieu de player.state = PLAYER_IDLE
    player.is_jumping = 0;
    player.is_falling = 0;
    player.is_attacking = 0;
    player.is_crouching = 0;
    player.is_climbing = 0;
    player.is_dying = 0;
    player.health = PLAYER_MAX_HEALTH;
    player_damage_cooldown = 0;   // au lieu de player.invincibility = 0
    player.direction = 1;        // au lieu de player.flip = 0
    player.on_ground = 0;
    player.vy = 0;
    player.current_frame = 0;     // au lieu de player.frame = 0
    player.frame_timer = 0;       // au lieu de player.attack_frame = 0 et player.frame_time = SDL_GetTicks()
    
    // Appliquer une pénalité au score lors de la mort du joueur
    player_deaths++;
    if (player_score >= PLAYER_DEATH_PENALTY) {
        player_score -= PLAYER_DEATH_PENALTY;
    }
}

// Fonction pour afficher l'écran de fin avec le score
void render_game_completion() {
    // Si le boss n'est pas vaincu, on n'affiche pas l'écran de fin
    if (!boss_defeated) return;
    
    // Calculer les différentes composantes du score
    int enemy_score = enemies_killed * ENEMY_KILL_POINTS;
    
    // Calculer le bonus de temps
    Uint32 battle_duration = (boss_battle_end_time - boss_battle_start_time) / 1000; // en secondes
    int time_bonus = 0;
    
    if (battle_duration <= BOSS_TIME_BONUS_THRESHOLD) {
        // Score maximum si le combat est terminé rapidement
        time_bonus = BOSS_TIME_BONUS_MAX;
    } else {
        // Réduction du bonus en fonction du temps écoulé (limité à 0)
        int overtime = battle_duration - BOSS_TIME_BONUS_THRESHOLD;
        time_bonus = BOSS_TIME_BONUS_MAX - (overtime * 50); // Réduction de 50 points par seconde supplémentaire
        if (time_bonus < 0) time_bonus = 0;
    }
    
    // Calculer la pénalité de mort
    int death_penalty = player_deaths * PLAYER_DEATH_PENALTY;

    // Effacer une partie de l'écran pour l'écran de fin
    SDL_Rect overlay_rect = {SCREEN_WIDTH/4, SCREEN_HEIGHT/4, SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
    SDL_FillRect(game.screen, &overlay_rect, SDL_MapRGB(game.screen->format, 0, 0, 100));
    
    // Bordure de l'écran de fin
    SDL_Rect border1 = {overlay_rect.x - 2, overlay_rect.y - 2, overlay_rect.w + 4, 2};
    SDL_Rect border2 = {overlay_rect.x - 2, overlay_rect.y + overlay_rect.h, overlay_rect.w + 4, 2};
    SDL_Rect border3 = {overlay_rect.x - 2, overlay_rect.y - 2, 2, overlay_rect.h + 4};
    SDL_Rect border4 = {overlay_rect.x + overlay_rect.w, overlay_rect.y - 2, 2, overlay_rect.h + 4};
    
    SDL_FillRect(game.screen, &border1, SDL_MapRGB(game.screen->format, 255, 215, 0));
    SDL_FillRect(game.screen, &border2, SDL_MapRGB(game.screen->format, 255, 215, 0));
    SDL_FillRect(game.screen, &border3, SDL_MapRGB(game.screen->format, 255, 215, 0));
    SDL_FillRect(game.screen, &border4, SDL_MapRGB(game.screen->format, 255, 215, 0));
    
    // Créer un tableau pour stocker les textes à afficher
    char texts[10][100]; // 10 lignes de texte max, 100 caractères par ligne
    int num_texts = 0;
    
    // Titre
    sprintf(texts[num_texts++], "VICTOIRE!");
    
    // Informations sur le temps de combat
    sprintf(texts[num_texts++], "Temps de combat: %u secondes", battle_duration);
    
    // Informations sur les ennemis tués
    sprintf(texts[num_texts++], "Ennemis tues: %d (+ %d points)", enemies_killed, enemy_score);
    
    // Bonus de temps
    sprintf(texts[num_texts++], "Bonus de temps: + %d points", time_bonus);
    
    // Pénalité de mort
    sprintf(texts[num_texts++], "Morts: %d (- %d points)", player_deaths, death_penalty);
    
    // Défaite du boss
    sprintf(texts[num_texts++], "Boss vaincu: + %d points", BOSS_KILL_BASE_POINTS);
    
    // Score total
    sprintf(texts[num_texts++], "SCORE TOTAL: %d", player_score);
    
    // Afficher tous les textes
    int line_height = 24;
    int start_y = overlay_rect.y + 30;

    for (int i = 0; i < num_texts; i++) {
        int line_y = start_y + i * (line_height + 10);
        int line_width = strlen(texts[i]) * 10; // Approximation de la largeur du texte
        
        // Couleur différente selon le type de ligne
        Uint32 text_color;
        if (i == 0) { // Titre
            text_color = SDL_MapRGB(game.screen->format, 255, 215, 0); // Or
            line_width = 250; // Titre plus large
        } 
        else if (i == num_texts - 1) { // Score total
            text_color = SDL_MapRGB(game.screen->format, 255, 255, 0); // Jaune
            line_width = 300; // Score total plus large
            line_height = 32; // Score total plus grand
        }
        else if (strstr(texts[i], "Morts")) {
            text_color = SDL_MapRGB(game.screen->format, 255, 100, 100); // Rouge clair
        }
        else if (strstr(texts[i], "Bonus")) {
            text_color = SDL_MapRGB(game.screen->format, 100, 255, 100); // Vert clair
        }
        else if (strstr(texts[i], "Ennemis")) {
            text_color = SDL_MapRGB(game.screen->format, 100, 200, 255); // Bleu clair
        }
        else {
            text_color = SDL_MapRGB(game.screen->format, 255, 255, 255); // Blanc
        }
        
        // Créer une surface pour le texte
        SDL_Surface* text_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, line_width, line_height, 32, 0, 0, 0, 0);
        SDL_FillRect(text_surface, NULL, text_color);
        
        // Afficher le texte (simulé par un rectangle coloré)
        SDL_Rect text_rect = {SCREEN_WIDTH/2 - line_width/2, line_y, line_width, line_height};
        SDL_BlitSurface(text_surface, NULL, game.screen, &text_rect);
        SDL_FreeSurface(text_surface);
    }
}

// Fonction pour vérifier si le joueur est en collision avec le boss
void check_player_boss_collision() {
    // Si le joueur est invulnérable ou en train de mourir, ou si le boss est mort, ignorer
    if (player_damage_cooldown > 0 || player.is_dying || typhon.state == TYPHON_STATE_DEATH || !typhon.active) {
        return;
    }
    
    // Zone de collision du joueur (plus grande pour faciliter les collisions)
    int player_hitbox_x = player.x + 20;
    int player_hitbox_y = player.y + 5; // Réduit pour augmenter la hauteur par le haut
    int player_hitbox_width = SPRITE_WIDTH - 40;
    int player_hitbox_height = SPRITE_HEIGHT - 10; // Taille raisonnable
    
    // Déterminer les dimensions et dégâts du boss selon le niveau
    int boss_width, boss_height, damage_close, damage_range;
    if (current_level_state == LEVEL_STATE_ZEUS_ARENA) {
        // Dimensions pour Zeus
        switch (typhon.state) {
            case TYPHON_STATE_IDLE:
                boss_width = ZEUS_IDLE_WIDTH;
                boss_height = ZEUS_IDLE_HEIGHT;
                break;
            case TYPHON_STATE_WALK:
                boss_width = ZEUS_WALK_WIDTH;
                boss_height = ZEUS_WALK_HEIGHT;
                break;
            case TYPHON_STATE_ATTACK_CLOSE:
                boss_width = ZEUS_ATTACK_CLOSE_WIDTH;
                boss_height = ZEUS_ATTACK_CLOSE_HEIGHT;
                break;
            case TYPHON_STATE_ATTACK_RANGE:
                boss_width = ZEUS_ATTACK_RANGE_WIDTH;
                boss_height = ZEUS_ATTACK_RANGE_HEIGHT;
                break;
            case TYPHON_STATE_HURT:
                boss_width = ZEUS_HURT_WIDTH;
                boss_height = ZEUS_HURT_HEIGHT;
                break;
            default:
                boss_width = ZEUS_IDLE_WIDTH;
                boss_height = ZEUS_IDLE_HEIGHT;
        }
        damage_close = ZEUS_DAMAGE_CLOSE;
        damage_range = ZEUS_DAMAGE_RANGE;
    } else {
        // Dimensions pour Typhon
        switch (typhon.state) {
            case TYPHON_STATE_IDLE:
                boss_width = TYPHON_IDLE_WIDTH;
                boss_height = TYPHON_IDLE_HEIGHT;
                break;
            case TYPHON_STATE_WALK:
                boss_width = TYPHON_WALK_WIDTH;
                boss_height = TYPHON_WALK_HEIGHT;
                break;
            case TYPHON_STATE_ATTACK_CLOSE:
                boss_width = TYPHON_ATTACK_CLOSE_WIDTH;
                boss_height = TYPHON_ATTACK_CLOSE_HEIGHT;
                break;
            case TYPHON_STATE_ATTACK_RANGE:
                boss_width = TYPHON_ATTACK_RANGE_WIDTH;
                boss_height = TYPHON_ATTACK_RANGE_HEIGHT;
                break;
            case TYPHON_STATE_HURT:
                boss_width = TYPHON_HURT_WIDTH;
                boss_height = TYPHON_HURT_HEIGHT;
                break;
            default:
                boss_width = TYPHON_IDLE_WIDTH;
                boss_height = TYPHON_IDLE_HEIGHT;
        }
        damage_close = TYPHON_DAMAGE_CLOSE;
        damage_range = TYPHON_DAMAGE_RANGE;
    }
    
    // Zone de collision du boss (plus grande pour faciliter les collisions)
    int boss_hitbox_x = typhon.x + 10;
    int boss_hitbox_y = typhon.y + 5; // Réduit pour augmenter la hauteur par le haut
    int boss_hitbox_width = boss_width - 20;
    int boss_hitbox_height = boss_height - 10; // Taille raisonnable
    
    // Afficher les hitboxes en permanence en mode debug
    if (DEBUG_HITBOXES) {
        // Hitbox du joueur (vert)
        SDL_Rect player_hitbox = {
            player_hitbox_x - game.camera_x,
            player_hitbox_y - game.camera_y,
            player_hitbox_width,
            player_hitbox_height
        };
        SDL_FillRect(game.screen, &player_hitbox, SDL_MapRGB(game.screen->format, 0, 255, 0));
        
        // Hitbox du boss (rouge)
        SDL_Rect boss_hitbox = {
            boss_hitbox_x - game.camera_x,
            boss_hitbox_y - game.camera_y,
            boss_hitbox_width,
            boss_hitbox_height
        };
        SDL_FillRect(game.screen, &boss_hitbox, SDL_MapRGB(game.screen->format, 255, 0, 0));
    }
    
    // Afficher les positions pour le débogage périodiquement
    if (SDL_GetTicks() % 60 == 0) {
        printf("Position joueur: (%.1f, %.1f), hitbox: (%d, %d, %d, %d)\n", 
               player.x, player.y, 
               player_hitbox_x, player_hitbox_y, 
               player_hitbox_width, player_hitbox_height);
        
        printf("Position Boss: (%.1f, %.1f), hitbox: (%d, %d, %d, %d)\n", 
               typhon.x, typhon.y, 
               boss_hitbox_x, boss_hitbox_y, 
               boss_hitbox_width, boss_hitbox_height);
    }
    
    // Calcul de la distance entre le joueur et le boss pour le débogage
    float dx = (player.x + SPRITE_WIDTH/2) - (typhon.x + boss_width/2);
    float dy = (player.y + SPRITE_HEIGHT/2) - (typhon.y + boss_height/2);
    float dist = sqrt(dx*dx + dy*dy);
    
    if (SDL_GetTicks() % 60 == 0) {
        printf("Distance entre joueur et boss: %.1f pixels\n", dist);
    }
    
    // Vérifier la collision (AABB)
    if (player_hitbox_x < boss_hitbox_x + boss_hitbox_width &&
        player_hitbox_x + player_hitbox_width > boss_hitbox_x &&
        player_hitbox_y < boss_hitbox_y + boss_hitbox_height &&
        player_hitbox_y + player_hitbox_height > boss_hitbox_y) {
        
        // Empêcher le joueur de traverser le boss
        // Déterminer la direction de la collision
        if (player.vx > 0) {  // Si le joueur se déplace vers la droite
            // Repousser le joueur vers la gauche
            player.x = boss_hitbox_x - player_hitbox_width - 20;
        } 
        else if (player.vx < 0) {  // Si le joueur se déplace vers la gauche
            // Repousser le joueur vers la droite
            player.x = boss_hitbox_x + boss_hitbox_width + 20;
        }
        else {
            // Si le joueur est immobile, le repousser en fonction de sa position par rapport au boss
            if (player.x + player_hitbox_width/2 < typhon.x + boss_hitbox_width/2) {
                // Si le joueur est à gauche du boss, le repousser vers la gauche
                player.x = boss_hitbox_x - player_hitbox_width - 20;
            } else {
                // Si le joueur est à droite du boss, le repousser vers la droite
                player.x = boss_hitbox_x + boss_hitbox_width + 20;
            }
        }
        
        // Le joueur est touché - les dégâts dépendent de l'état du boss
        printf("COLLISION DÉTECTÉE AVEC LE BOSS!\n");
        
        if (typhon.state == TYPHON_STATE_ATTACK_CLOSE) {
            // Si le boss est en train d'attaquer au corps à corps, infliger des dégâts élevés
            damage_player(damage_close);
            printf("Joueur touché par attaque rapprochée! Dégâts: %d\n", damage_close);
        }
        else if (typhon.state == TYPHON_STATE_ATTACK_RANGE) {
            // Si le boss est en train d'attaquer à distance, infliger des dégâts moyens
            damage_player(damage_range);
            printf("Joueur touché par attaque à distance! Dégâts: %d\n", damage_range);
        }
        else {
            // Contact simple avec le boss, infliger des dégâts faibles
            damage_player(damage_range / 2);
            printf("Joueur touché par contact avec le boss! Dégâts: %d\n", damage_range / 2);
        }
        
        // Effet de recul - plus fort car le boss est plus grand
        player.vx = (typhon.x > player.x) ? -12 : 12; // Recul horizontal plus fort
        player.vy = -8; // Petit saut vers le haut plus haut
        player.on_ground = 0;
    }
    
    // Pour le débogage, testons une approche alternative basée sur la distance
    // Si les joueurs sont très proches, considérons cela comme une collision même si les hitboxes ne se chevauchent pas
    if (dist < 100 && player_damage_cooldown <= 0 && !player.is_dying) {
        printf("COLLISION PAR DISTANCE DÉTECTÉE! Distance: %.1f\n", dist);
        
        // Empêcher le joueur de traverser le boss en fonction de sa position relative
        if (player.x + SPRITE_WIDTH/2 < typhon.x + boss_width/2) {
            // Si le joueur est à gauche du boss, le repousser vers la gauche
            player.x = typhon.x - SPRITE_WIDTH - 10;
        } else {
            // Si le joueur est à droite du boss, le repousser vers la droite
            player.x = typhon.x + boss_width + 10;
        }
        
        // Infliger des dégâts
        damage_player(damage_range / 3); // Dégâts réduits pour ce type de collision
        
        // Effet de recul
        player.vx = (typhon.x > player.x) ? -8 : 8;
        player.vy = -5;
        player.on_ground = 0;
    }
}

// Fonction pour créer une nouvelle boule de feu
void spawn_fireball(float x, float y, int direction) {
    // Trouver un emplacement libre dans le tableau
    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (!fireballs[i].active) {
            fireballs[i].x = x;
            fireballs[i].y = y;
            fireballs[i].vx = direction * FIREBALL_SPEED;
            fireballs[i].vy = 0;  // Pas de mouvement vertical
            fireballs[i].active = 1;
            fireballs[i].current_frame = 0;
            fireballs[i].frame_timer = 0;
            fireballs[i].animation_loops = 0;
            fireballs[i].direction = direction;
            printf("Boule de feu créée à x=%.1f, y=%.1f, direction=%d\n", x, y, direction);
            return;
        }
    }
    printf("Impossible de créer une boule de feu, toutes les slots sont occupées.\n");
}

// Fonction pour mettre à jour toutes les boules de feu
void update_fireballs() {
    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (!fireballs[i].active) continue;
        
        // Mettre à jour la position
        fireballs[i].x += fireballs[i].vx;
        fireballs[i].y += fireballs[i].vy;
        
        // Animer la boule de feu
        fireballs[i].frame_timer++;
        if (fireballs[i].frame_timer >= 5) {  // Animation assez rapide
            fireballs[i].frame_timer = 0;
            fireballs[i].current_frame = (fireballs[i].current_frame + 1) % FIREBALL_FRAMES;
            
            // Si on a fait un cycle complet d'animation
            if (fireballs[i].current_frame == 0) {
                fireballs[i].animation_loops++;
            }
        }
        
        // Vérifier collision avec le joueur
        if (!player.is_dying && player_damage_cooldown <= 0) {
            // Hitbox du joueur
            int player_hitbox_x = player.x + 20;
            int player_hitbox_y = player.y + 5;
            int player_hitbox_width = SPRITE_WIDTH - 40;
            int player_hitbox_height = SPRITE_HEIGHT - 10;
            
            // Hitbox de la boule de feu
            int fireball_hitbox_x = fireballs[i].x;
            int fireball_hitbox_y = fireballs[i].y;
            
            // Vérifier collision (AABB)
            if (fireball_hitbox_x < player_hitbox_x + player_hitbox_width &&
                fireball_hitbox_x + FIREBALL_WIDTH > player_hitbox_x &&
                fireball_hitbox_y < player_hitbox_y + player_hitbox_height &&
                fireball_hitbox_y + FIREBALL_HEIGHT > player_hitbox_y) {
                
                // Toucher le joueur
                damage_player(FIREBALL_DAMAGE);
                printf("Joueur touché par une boule de feu! Dégâts: %d\n", FIREBALL_DAMAGE);
                
                // Désactiver la boule de feu
                fireballs[i].active = 0;
            }
        }
        
        // Vérifier si la boule de feu sort de l'écran
        if (fireballs[i].x < 0 || fireballs[i].x > MAP_WIDTH ||
            fireballs[i].y < 0 || fireballs[i].y > MAP_HEIGHT) {
            fireballs[i].active = 0;
        }
    }
}

// Fonction pour afficher toutes les boules de feu
void render_fireballs() {
    if (!fireball_sprite || !fireball_sprite_flipped) return;
    
    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (!fireballs[i].active) continue;
        
        // Choisir le bon sprite selon la direction
        SDL_Surface* current_sprite = (fireballs[i].direction > 0) ? 
                                      fireball_sprite : fireball_sprite_flipped;
        
        // Rectangle source pour l'animation
        SDL_Rect src_rect = {
            fireballs[i].current_frame * FIREBALL_WIDTH,
            0,
            FIREBALL_WIDTH,
            FIREBALL_HEIGHT
        };
        
        // Rectangle destination à l'écran
        SDL_Rect dest_rect = {
            (int)(fireballs[i].x - game.camera_x),
            (int)(fireballs[i].y - game.camera_y),
            FIREBALL_WIDTH,
            FIREBALL_HEIGHT
        };
        
        // Dessiner la boule de feu
        SDL_BlitSurface(current_sprite, &src_rect, game.screen, &dest_rect);
    }
}

// Fonction pour gérer les fondus
void update_fade() {
    if (fade_state == FADE_STATE_NONE) return;
    
    fade_timer++;
    
    // Si le fondu au noir est terminé, charger le nouveau niveau
    if (fade_state == FADE_STATE_OUT && fade_timer >= FADE_DURATION) {
        if (transition_to_level >= 0) {
            load_level(transition_to_level);
            transition_to_level = -1;
        }
        fade_state = FADE_STATE_IN;
        fade_timer = 0;
    }
    
    // Si le fondu depuis le noir est terminé, terminer la transition
    if (fade_state == FADE_STATE_IN && fade_timer >= FADE_DURATION) {
        fade_state = FADE_STATE_NONE;
    }
}

// Fonction pour rendre les fondus
void render_fade() {
    if (fade_state == FADE_STATE_NONE) return;
    
    // Calculer l'opacité en fonction du timer
    int opacity;
    if (fade_state == FADE_STATE_OUT) {
        // Fondu vers le noir - augmenter l'opacité progressivement
        opacity = (fade_timer * 255) / FADE_DURATION;
    } else { // FADE_STATE_IN
        // Fondu depuis le noir - diminuer l'opacité progressivement
        opacity = 255 - ((fade_timer * 255) / FADE_DURATION);
    }
    
    // S'assurer que l'opacité reste dans les limites
    if (opacity < 0) opacity = 0;
    if (opacity > 255) opacity = 255;
    
    // Créer une surface noire avec l'opacité calculée
    SDL_Surface* fade_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);
    if (!fade_surface) {
        printf("Erreur lors de la création de la surface de fondu: %s\n", SDL_GetError());
        return;
    }
    
    // Remplir la surface en noir
    SDL_FillRect(fade_surface, NULL, SDL_MapRGB(game.screen->format, 0, 0, 0));
    
    // Appliquer l'opacité
    if (SDL_SetAlpha(fade_surface, SDL_SRCALPHA, opacity) != 0) {
        printf("Erreur lors de l'application de l'alpha: %s\n", SDL_GetError());
    }
    
    // Dessiner la surface de fondu
    SDL_BlitSurface(fade_surface, NULL, game.screen, NULL);
    
    // Afficher des informations de débogage
    if (fade_timer % 10 == 0) {
        printf("Fondu: état=%d, timer=%d, opacité=%d\n", fade_state, fade_timer, opacity);
    }
    
    // Libérer la surface
    SDL_FreeSurface(fade_surface);
}

void render_typhon();
void update_zeus();
void render_zeus();
void render_health_bar();

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    
    // Initialiser le score et les compteurs associés
    player_score = 0;
    enemies_killed = 0;
    player_deaths = 0;
    boss_battle_start_time = 0;
    boss_battle_end_time = 0;
    boss_defeated = 0;
    
    init_game();
    init_player();
    init_enemies();
    
    // Charger le niveau initial (Level 1 Part 1: Parcours principal)
    load_level(LEVEL_STATE_MAIN_PARKOUR);

    SDL_Event e;
    int quit = 0;

    printf("Jeu lancé - Utiliser flèches pour se déplacer, K pour attaquer, ESC pour quitter\n");
    printf("Niveau initial: Level 1 Part 1 - Parcours principal\n");

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
            // Gestion des entrées du clavier
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        key_left = KEY_PRESSED;
                        break;
                    case SDLK_RIGHT:
                        key_right = KEY_PRESSED;
                        break;
                    case SDLK_UP:
                        key_up = KEY_PRESSED;
                        break;
                    case SDLK_DOWN:
                        key_down = KEY_PRESSED;
                        break;
                    case SDLK_SPACE:
                        key_space = KEY_PRESSED;
                        break;
                    case SDLK_k:
                        key_k = KEY_PRESSED;
                        player.is_attacking = 1;
                        player.current_frame = 0;
                        player.frame_timer = 0;
                        break;
                    case SDLK_j:  // Change from L to J key for climbing
                        key_j = KEY_PRESSED;
                        break;
                    case SDLK_ESCAPE:
                        quit = 1;
                        break;
                    case SDLK_b:
                        key_b = KEY_PRESSED;
                        printf("Touche B pressée - Tentative de transition manuelle vers l'arène du boss\n");
                        if (current_level_state == LEVEL_STATE_MAIN_PARKOUR) {
                            printf("Transition forcée vers l'arène du boss!\n");
                            player.x = BOSS_ARENA_X + 1; // Forcer la position pour déclencher la transition
                        }
                        break;
                    case SDLK_r:
                        // Touche pour retourner au niveau principal (pour tests)
                        if (current_level_state == LEVEL_STATE_BOSS_ARENA) {
                            printf("Retour au niveau principal (mode test)\n");
                            load_level(LEVEL_STATE_MAIN_PARKOUR);
                        }
                        break;
                    default:
                        break;
                }
            }
            else if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT:
                        key_left = KEY_RELEASED;
                        break;
                    case SDLK_RIGHT:
                        key_right = KEY_RELEASED;
                        break;
                    case SDLK_UP:
                        key_up = KEY_RELEASED;
                        break;
                    case SDLK_DOWN:
                        key_down = KEY_RELEASED;
                        break;
                    case SDLK_SPACE:
                        key_space = KEY_RELEASED;
                        break;
                    case SDLK_k:
                        key_k = KEY_RELEASED;
                        break;
                    case SDLK_j:  // Change from L to J for climbing
                        key_j = KEY_RELEASED;
                        break;
                    case SDLK_b:
                        key_b = KEY_RELEASED;
                        break;
                    case SDLK_r:
                        // Relâchement de la touche pour retourner au niveau principal
                        break;
                    default:
                        break;
                }
            }
        }

        // Vérifier la transition entre les niveaux si dans le niveau principal
        if (current_level_state == LEVEL_STATE_MAIN_PARKOUR) {
            check_boss_arena_transition();
        }
        
        // Mise à jour du fondu
        update_fade();
        
        // Mise à jour de l'état du jeu
        if (fade_state != FADE_STATE_OUT) { // Ne pas mettre à jour le jeu pendant un fondu au noir
            update_player_state();
            update_player();
            update_animation_ticks();
            
            // Mise à jour des ennemis selon le niveau
            if (current_level_state == LEVEL_STATE_MAIN_PARKOUR) {
                // Niveau principal - Mise à jour des ennemis normaux
                update_enemies();
                
                // Vérification des collisions avec les ennemis
                check_player_enemy_collision();
                check_player_attack_collision();
            } 
            else if (current_level_state == LEVEL_STATE_BOSS_ARENA || 
                     current_level_state == LEVEL_STATE_ZEUS_ARENA) {
                // Arène du boss - Mise à jour du boss
                update_typhon();
                update_fireballs();
                
                // Vérification des collisions avec le boss
                check_player_boss_collision();
                check_player_attack_typhon();
            }
            
            // Mettre à jour l'animation morte du boss Typhon même pendant le fondu de transition
            if (current_level_state == LEVEL_STATE_BOSS_ARENA && 
                typhon.state == TYPHON_STATE_DEATH && typhon.active) {
                // Afficher des informations de débogage
                if (SDL_GetTicks() % 60 == 0) {
                    printf("Mise à jour animation de mort pendant le fondu: frame=%d/%d, fade_state=%d\n", 
                           typhon.current_frame, TYPHON_DEATH_FRAMES-1, fade_state);
                }
            }
        } else {
            // Même pendant le fondu, continuer à avancer l'animation de mort
            if ((current_level_state == LEVEL_STATE_BOSS_ARENA || 
                 current_level_state == LEVEL_STATE_ZEUS_ARENA) && 
                typhon.state == TYPHON_STATE_DEATH && typhon.active) {
                
                // Faire avancer l'animation de mort plus lentement pendant le fondu
                typhon.frame_timer++;
                if (typhon.frame_timer >= 10) {  // Vitesse ralentie pendant le fondu
                    typhon.frame_timer = 0;
                    if (typhon.current_frame < (current_level_state == LEVEL_STATE_ZEUS_ARENA ? 
                                             ZEUS_DEATH_FRAMES : TYPHON_DEATH_FRAMES) - 1) {
                        typhon.current_frame++;
                        
                        // Afficher des informations de débogage
                        printf("Animation de mort avance pendant le fondu: frame=%d/%d\n", 
                               typhon.current_frame, (current_level_state == LEVEL_STATE_ZEUS_ARENA ? 
                                                      ZEUS_DEATH_FRAMES : TYPHON_DEATH_FRAMES) - 1);
                    }
                }
            }
        }

        // Rendre le jeu
        render();
        
        // Limiter le frame rate
        SDL_Delay(16); // Environ 60 FPS (1000ms / 60 = 16.67ms)
    }

    cleanup_player();
    cleanup_enemies();
    
    if (current_level_state == LEVEL_STATE_BOSS_ARENA || current_level_state == LEVEL_STATE_ZEUS_ARENA) {
        cleanup_typhon();
    }
    
    cleanup();
    return 0;
}

void render_zeus() {
    // Vérifier que Zeus est actif
    if (!typhon.active) {
        // Afficher un message de débogage occasionnel
        if (SDL_GetTicks() % 300 == 0) {
            printf("Zeus inactif, aucun rendu effectué (typhon.active = %d)\n", typhon.active);
        }
        return;
    }
    
    // Afficher des informations de débogage périodiquement, pas à chaque frame
    if (SDL_GetTicks() % 300 == 0) {
        printf("Rendu de Zeus à la position x=%.1f, y=%.1f (camera: %.1f, %.1f), état=%d, frame=%d, direction=%d\n", 
               typhon.x, typhon.y, game.camera_x, game.camera_y, typhon.state, typhon.current_frame, typhon.direction);
    }
    
    SDL_Surface* current_sprite = NULL;
    int frame_width;
    int frame_height;
    
    // Sélectionner le sprite en fonction de l'état et de la direction
    // Pour Zeus, le comportement des sprites est inversé par rapport à Typhon :
    // Utiliser le sprite normal pour la direction -1 (gauche) et le sprite flippé pour la direction 1 (droite)
    switch (typhon.state) {
        case TYPHON_STATE_IDLE:
            current_sprite = typhon.direction == -1 ? typhon.sprite_idle_flipped : typhon.sprite_idle;
            frame_width = ZEUS_IDLE_WIDTH;
            frame_height = ZEUS_IDLE_HEIGHT;
            break;
        case TYPHON_STATE_WALK:
            current_sprite = typhon.direction == -1 ? typhon.sprite_walk_flipped : typhon.sprite_walk;
            frame_width = ZEUS_WALK_WIDTH;
            frame_height = ZEUS_WALK_HEIGHT;
            break;
        case TYPHON_STATE_ATTACK_CLOSE:
            current_sprite = typhon.direction == -1 ? typhon.sprite_attack_close_flipped : typhon.sprite_attack_close;
            frame_width = ZEUS_ATTACK_CLOSE_WIDTH;
            frame_height = ZEUS_ATTACK_CLOSE_HEIGHT;
            break;
        case TYPHON_STATE_ATTACK_RANGE:
            current_sprite = typhon.direction == -1 ? typhon.sprite_attack_range_flipped : typhon.sprite_attack_range;
            frame_width = ZEUS_ATTACK_RANGE_WIDTH;
            frame_height = ZEUS_ATTACK_RANGE_HEIGHT;
            break;
        case TYPHON_STATE_HURT:
            current_sprite = typhon.direction == -1 ? typhon.sprite_hurt_flipped : typhon.sprite_hurt;
            frame_width = ZEUS_HURT_WIDTH;
            frame_height = ZEUS_HURT_HEIGHT;
            break;
        case TYPHON_STATE_DEATH:
            current_sprite = typhon.direction == -1 ? typhon.sprite_death_flipped : typhon.sprite_death;
            frame_width = ZEUS_DEATH_WIDTH;
            frame_height = ZEUS_DEATH_HEIGHT;
            
            // Afficher des informations spécifiques pour l'animation de mort
            if (SDL_GetTicks() % 60 == 0) {
                printf("Animation de mort de Zeus rendue: frame=%d/%d\n", typhon.current_frame, ZEUS_DEATH_FRAMES-1);
            }
            break;
        default:
            break;
    }
    
    // Rectangle destination pour l'affichage
    SDL_Rect dest_rect = {
        (int)(typhon.x - game.camera_x),
        (int)(typhon.y - game.camera_y),
        frame_width,
        frame_height
    };
    
    // Vérifier que Zeus est visible à l'écran seulement en mode débogage
    if (DEBUG_HITBOXES) {
        if (dest_rect.x + dest_rect.w < 0 || dest_rect.x > SCREEN_WIDTH || 
            dest_rect.y + dest_rect.h < 0 || dest_rect.y > SCREEN_HEIGHT) {
            printf("Zeus hors écran: dest_rect = (%d, %d, %d, %d)\n", 
                   dest_rect.x, dest_rect.y, dest_rect.w, dest_rect.h);
        } else {
            printf("Zeus visible à l'écran: dest_rect = (%d, %d, %d, %d)\n", 
                   dest_rect.x, dest_rect.y, dest_rect.w, dest_rect.h);
        }
    }
    
    // Vérifier si le sprite est valide
    if (!current_sprite) {
        if (DEBUG_HITBOXES) {
            printf("ERREUR: Sprite de Zeus non disponible - Affichage d'un rectangle de substitution\n");
            
            // Dessiner un rectangle de substitution de couleur bleue
            SDL_Rect debug_rect = {dest_rect.x, dest_rect.y, 100, 150};
            SDL_FillRect(game.screen, &debug_rect, SDL_MapRGB(game.screen->format, 0, 0, 255));
        }
    }
    else if (current_sprite->w == 0 || current_sprite->h == 0) {
        if (DEBUG_HITBOXES) {
            printf("ERREUR: Dimensions du sprite de Zeus invalides (%dx%d)\n", 
                   current_sprite->w, current_sprite->h);
            
            // Dessiner un rectangle jaune pour les sprites mal dimensionnés
            SDL_Rect debug_rect = {dest_rect.x, dest_rect.y, 100, 150};
            SDL_FillRect(game.screen, &debug_rect, SDL_MapRGB(game.screen->format, 255, 255, 0));
        }
    }
    else {
        // Nombre maximal de frames selon l'animation
        int max_frames;
        switch (typhon.state) {
            case TYPHON_STATE_IDLE:
                max_frames = ZEUS_IDLE_FRAMES;
                break;
            case TYPHON_STATE_WALK:
                max_frames = ZEUS_WALK_FRAMES;
                break;
            case TYPHON_STATE_ATTACK_CLOSE:
                max_frames = ZEUS_ATTACK_CLOSE_FRAMES;
                break;
            case TYPHON_STATE_ATTACK_RANGE:
                max_frames = ZEUS_ATTACK_RANGE_FRAMES;
                break;
            case TYPHON_STATE_HURT:
                max_frames = ZEUS_HURT_FRAMES;
                break;
            case TYPHON_STATE_DEATH:
                max_frames = ZEUS_DEATH_FRAMES;
                break;
            default:
                max_frames = 1;
        }
        
        // Vérifier que la frame actuelle est valide
        if (typhon.current_frame >= max_frames) {
            printf("ERREUR: Frame de Zeus invalide! %d/%d\n", typhon.current_frame, max_frames-1);
            typhon.current_frame = 0;
        }
        
        // Calcul de la région source dans la spritesheet
        SDL_Rect src_rect = {
            typhon.current_frame * frame_width,
            0,
            frame_width,
            frame_height
        };
        
        // Afficher le sprite
        SDL_BlitSurface(current_sprite, &src_rect, game.screen, &dest_rect);
        
        // Afficher la barre de vie au-dessus de Zeus
        SDL_Rect health_bg = {
            (int)(typhon.x - game.camera_x),
            (int)(typhon.y - game.camera_y) - 20,
            100,
            10
        };
        
        SDL_FillRect(game.screen, &health_bg, SDL_MapRGB(game.screen->format, 100, 0, 0));
        
        SDL_Rect health_bar = {
            (int)(typhon.x - game.camera_x),
            (int)(typhon.y - game.camera_y) - 20,
            (int)(100 * (float)typhon.health / (float)typhon.max_health),
            10
        };
        
        SDL_FillRect(game.screen, &health_bar, SDL_MapRGB(game.screen->format, 0, 255, 0));
        
        // Afficher les hitboxes (en mode débogage uniquement)
        if (DEBUG_HITBOXES) {
            SDL_Rect hitbox = {
                (int)(typhon.x - game.camera_x) + frame_width / 4,
                (int)(typhon.y - game.camera_y) + frame_height / 4,
                frame_width / 2,
                frame_height / 2
            };
            
            SDL_Rect hitbox_outline[4] = {
                {hitbox.x, hitbox.y, hitbox.w, 1},                      // Haut
                {hitbox.x, hitbox.y + hitbox.h - 1, hitbox.w, 1},       // Bas
                {hitbox.x, hitbox.y, 1, hitbox.h},                      // Gauche
                {hitbox.x + hitbox.w - 1, hitbox.y, 1, hitbox.h}        // Droite
            };
            
            for (int i = 0; i < 4; i++) {
                SDL_FillRect(game.screen, &hitbox_outline[i], SDL_MapRGB(game.screen->format, 255, 0, 0));
            }
        }
    }
}
