#include "headerG.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#define MAX_ENNEMIS 5 
/*------------------------------------*
 *    FONCTIONS D'INITIALISATION     *
 *------------------------------------*/

void initialiser_SDL() {
    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        printf("Erreur d'initialisation de la SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    
    // Initialisation de SDL_ttf
    if (TTF_Init() == -1) {
        printf("Erreur d'initialisation de TTF_Init: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    
    // Pour SDL_image 1.2, pas besoin d'initialisation explicite
    // avec IMG_Init, c'est fait automatiquement lors du premier appel
    // à une fonction de SDL_image
}

void charger_animations_personnage(Personnage *pers) {
    const char *anim_types[] = {"idle", "run", "jump", "hurt", "death", "attack"};
    const int frame_counts[] = {6, 8, 3, 4, 11, 12}; // Correct frame counts for each animation
    
    for (int i = 0; i < 6; i++) {
        char path[100];
        sprintf(path, "resources/player/spritesheetmc_%s_R.png", anim_types[i]);
        pers->anim.sprites_R[i] = IMG_Load(path);
        
        sprintf(path, "resources/player/spritesheetmc_%s_L.png", anim_types[i]);
        pers->anim.sprites_L[i] = IMG_Load(path);
        
        if (!pers->anim.sprites_R[i] || !pers->anim.sprites_L[i]) {
            printf("Failed to load animation: %s\n", path);
            exit(1);
        }
        pers->anim.frame_counts[i] = frame_counts[i];
    }
    
    pers->anim.current_frame = 0;
    pers->anim.animation_speed = 6;  // Changed from 8 to 6 (faster animation)
    pers->anim.state = PLAYER_IDLE;
    pers->frame_timer = 0;
}

void initialiser_personnage(Personnage *pers, const char *chemin_image) {
    pers->x = 100;
    pers->y = GROUND_LEVEL - 100; // Changed from 64 to 100 to match new height
    pers->velocity_y = 0;
    pers->is_jumping = 0;
    pers->vie = 100;
    pers->score = 0;
    pers->vitesse = 5;
    pers->moving_left = 0;
    pers->moving_right = 0;
    strcpy(pers->direction, "right");
    pers->is_attacking = 0;
    
    charger_animations_personnage(pers);
}

void liberer_animations_personnage(Personnage *pers) {
    for (int i = 0; i < 6; i++) {  // Changed from 5 to 6
        if (pers->anim.sprites_R[i]) SDL_FreeSurface(pers->anim.sprites_R[i]);
        if (pers->anim.sprites_L[i]) SDL_FreeSurface(pers->anim.sprites_L[i]);
    }
}

int detecter_collision_entre_ennemis(Enemy *ennemi1, Enemy *ennemi2) {
    if (!ennemi1 || !ennemi2 || ennemi1 == ennemi2) return 0;
    
    // Enemy hitboxes (same as in other collision functions)
    SDL_Rect enemy1_box = {
        .x = ennemi1->x + 15,
        .y = ennemi1->y + 5,
        .w = 45,
        .h = 55
    };
    
    SDL_Rect enemy2_box = {
        .x = ennemi2->x + 15,
        .y = ennemi2->y + 5,
        .w = 45,
        .h = 55
    };
    
    // Check if rectangles overlap
    return (enemy1_box.x < enemy2_box.x + enemy2_box.w &&
            enemy1_box.x + enemy1_box.w > enemy2_box.x &&
            enemy1_box.y < enemy2_box.y + enemy2_box.h &&
            enemy1_box.y + enemy1_box.h > enemy2_box.y);
}

int detecter_position_spawn_valide(Enemy ennemis[], int nb_ennemis, int x, int y) {
    for (int i = 0; i < nb_ennemis; i++) {
        if (ennemis[i].state == ENEMY_NEUTRALIZED) continue;
        
        // Enemy hitboxes (same as in other collision functions)
        SDL_Rect new_enemy_box = {
            .x = x + 15,
            .y = y + 5,
            .w = 45,
            .h = 55
        };
        
        SDL_Rect existing_enemy_box = {
            .x = ennemis[i].x + 15,
            .y = ennemis[i].y + 5,
            .w = 45,
            .h = 55
        };
        
        // Check if rectangles overlap
        if (new_enemy_box.x < existing_enemy_box.x + existing_enemy_box.w &&
            new_enemy_box.x + new_enemy_box.w > existing_enemy_box.x &&
            new_enemy_box.y < existing_enemy_box.y + existing_enemy_box.h &&
            new_enemy_box.y + new_enemy_box.h > existing_enemy_box.y) {
            return 0;
        }
    }
    return 1;
}

void initialiser_ennemis(Enemy ennemis[], int nb_ennemis, int niveau) {
    int i;
    char chemin[100];
    
    srand(time(NULL)); // Initialisation du générateur de nombres aléatoires
    
    for (i = 0; i < nb_ennemis; i++) {
        // Chargement des images d'animation (4 frames)
        int j;
        for (j = 0; j < 4; j++) {
            sprintf(chemin, "resources/enemies/enemy%d.png", j+1);
            ennemis[i].images[j] = IMG_Load(chemin);
            if (!ennemis[i].images[j]) {
                printf("Erreur de chargement de l'image d'ennemi: %s\n", IMG_GetError());
                printf("Chemin: %s\n", chemin);
                exit(EXIT_FAILURE);
            }
        }
        
        // Position initiale aléatoire avec vérification de collision
        int spawn_x, spawn_y;
        int tentatives = 0;
        do {
            spawn_x = rand() % (1280 - 64);
            spawn_y = GROUND_LEVEL - 64;
            tentatives++;
            if (tentatives > 100) {
                // Si trop de tentatives, décaler sur l'axe X
                spawn_x = (i * 100) % (1280 - 64);
                break;
            }
        } while (!detecter_position_spawn_valide(ennemis, i, spawn_x, spawn_y));
        
        ennemis[i].x = spawn_x;
        ennemis[i].y = spawn_y;
        ennemis[i].velocity_y = 0;
        ennemis[i].is_grounded = 1;
        
        // Propriétés initiales
        ennemis[i].vitesse = niveau == 1 ? 2 : 3; // Niveau 2 plus rapide
        ennemis[i].direction = rand() % 4; // Direction aléatoire
        ennemis[i].niveau = niveau;
        ennemis[i].health = niveau == 1 ? 300 : 150; // Plus de vie pour niveau 2
        ennemis[i].state = ENEMY_ALIVE;
        ennemis[i].animation = ENEMY_IDLE;
        ennemis[i].frame_actuelle = 0;
        ennemis[i].trajectoire = (rand() % 2) + 1; // Trajectoire 1 ou 2
        ennemis[i].damage_cooldown = 0;
        ennemis[i].movement_pause = 0;
        ennemis[i].stun_timer = 0;
        ennemis[i].is_stunned = 0;
    }
}

/*------------------------------------*
 *      FONCTIONS D'AFFICHAGE        *
 *------------------------------------*/

void afficher_ennemis(Enemy ennemis[], int nb_ennemis) {
    // Add null check
    if (!ennemis || nb_ennemis <= 0) return;

    int i;
    SDL_Rect position;
    
    for (i = 0; i < nb_ennemis; i++) {
        // Ne pas afficher les ennemis neutralisés
        if (ennemis[i].state == ENEMY_NEUTRALIZED) 
            continue;
            
        position.x = ennemis[i].x;
        position.y = ennemis[i].y;
        
        // Utiliser l'image correspondant à la frame actuelle
        SDL_Surface *image_actuelle = ennemis[i].images[ennemis[i].frame_actuelle];
        
        // Si l'ennemi est blessé, modifier la teinte (en rouge)
        if (ennemis[i].state == ENEMY_WOUNDED) {
            // On pourrait utiliser SDL_SetAlpha ou une surface temporaire avec une teinte rouge
            // Pour simplifier, on va juste afficher sans modification mais dans une implémentation
            // complète, il faudrait modifier la teinte
        }
        
        // Afficher l'ennemi
        SDL_BlitSurface(image_actuelle, NULL, SDL_GetVideoSurface(), &position);
        
        // Afficher barre de vie au-dessus de l'ennemi
        SDL_Rect barre_vie;
        barre_vie.x = ennemis[i].x;
        barre_vie.y = ennemis[i].y - 10;
        barre_vie.w = 50; // Largeur totale de la barre
        barre_vie.h = 5;  // Hauteur de la barre
        
        // Fond gris de la barre de vie
        SDL_FillRect(SDL_GetVideoSurface(), &barre_vie, SDL_MapRGB(SDL_GetVideoSurface()->format, 100, 100, 100));
        
        // Partie verte/rouge selon la santé
        barre_vie.w = (ennemis[i].health * 50) / (ennemis[i].niveau == 1 ? 300 : 150);
        Uint32 couleur;
        if (ennemis[i].health > 50)
            couleur = SDL_MapRGB(SDL_GetVideoSurface()->format, 0, 255, 0); // Vert
        else
            couleur = SDL_MapRGB(SDL_GetVideoSurface()->format, 255, 0, 0); // Rouge
            
        SDL_FillRect(SDL_GetVideoSurface(), &barre_vie, couleur);
    }
}

/*------------------------------------*
 *      FONCTIONS DE GAMEPLAY        *
 *------------------------------------*/

void deplacer_ennemis(Enemy ennemis[], int nb_ennemis) {
    int i;
    
    for (i = 0; i < nb_ennemis; i++) {
        // Ne pas déplacer les ennemis neutralisés
        if (ennemis[i].state == ENEMY_NEUTRALIZED)
            continue;
            
        // Déplacement aléatoire selon la trajectoire
        if (ennemis[i].trajectoire == 1) {
            // Trajectoire 1: Déplacement linéaire avec changement aléatoire
            if (rand() % 50 == 0) { // 1 chance sur 50 de changer de direction
                ennemis[i].direction = rand() % 4;
            }
            
            // Déplacement selon la direction
            switch (ennemis[i].direction) {
                case 0: // Gauche
                    ennemis[i].x -= ennemis[i].vitesse;
                    break;
                case 1: // Droite
                    ennemis[i].x += ennemis[i].vitesse;
                    break;
                case 2: // Haut
                    ennemis[i].y -= ennemis[i].vitesse;
                    break;
                case 3: // Bas
                    ennemis[i].y += ennemis[i].vitesse;
                    break;
            }
        } else {
            // Trajectoire 2: Mouvement en zigzag
            static int compteur = 0;
            compteur++;
            
            if (compteur % 30 < 15) { // Alterne toutes les 15 frames
                ennemis[i].x += ennemis[i].vitesse;
                ennemis[i].y += ennemis[i].vitesse/2;
            } else {
                ennemis[i].x -= ennemis[i].vitesse;
                ennemis[i].y += ennemis[i].vitesse/2;
            }
        }
        
        // Empêcher l'ennemi de sortir de l'écran (limites à ajuster)
        if (ennemis[i].x < 0) ennemis[i].x = 0;
        if (ennemis[i].x > 1280 - 64) ennemis[i].x = 1280 - 64; // Supposant que la sprite fait 64x64
        if (ennemis[i].y < 0) ennemis[i].y = 0;
        if (ennemis[i].y > 720 - 64) ennemis[i].y = 720 - 64;
    }
}

void animer_ennemis(Enemy ennemis[], int nb_ennemis) {
    int i;
    static int compteur_animation = 0;
    
    compteur_animation++;
    
    // Changer la frame toutes les 10 frames du jeu
    if (compteur_animation % 10 == 0) {
        for (i = 0; i < nb_ennemis; i++) {
            if (ennemis[i].state != ENEMY_NEUTRALIZED) {
                // Passer à la frame suivante
                ennemis[i].frame_actuelle = (ennemis[i].frame_actuelle + 1) % 4;
                
                // Déterminer le type d'animation selon l'état
                if (ennemis[i].state == ENEMY_WOUNDED) {
                    ennemis[i].animation = ENEMY_IDLE; // Moins de mouvement quand blessé
                } else {
                    // Alterner entre marche et attaque
                    if (rand() % 20 == 0) { // 5% de chance d'attaquer
                        ennemis[i].animation = ENEMY_ATTACKING;
                    } else {
                        ennemis[i].animation = ENEMY_WALKING;
                    }
                }
            }
        }
    }
}

int detecter_collision_joueur_ennemi(Personnage *pers, Enemy *ennemi) {
    // Don't detect collision if enemy is neutralized or player isn't in attack phase
    if (ennemi->state == ENEMY_NEUTRALIZED || !pers->is_attacking)
        return 0;
        
    // Attack frames (5-8) are the only ones that can damage enemies
    if (pers->anim.state != PLAYER_ATTACK || 
        pers->anim.current_frame < 5 || 
        pers->anim.current_frame > 8)
        return 0;

    // Use exact hitbox collision instead of range-based detection
    SDL_Rect player_attack_box;
    
    if (strcmp(pers->direction, "right") == 0) {
        player_attack_box.x = pers->x + 80;  // Extend to the right
        player_attack_box.y = pers->y + 20;  // Vertical center
        player_attack_box.w = 70;           // Attack range 
        player_attack_box.h = 60;           // Vertical range
    } else {
        player_attack_box.x = pers->x - 30;  // Extend to the left
        player_attack_box.y = pers->y + 20;  // Vertical center
        player_attack_box.w = 70;           // Attack range
        player_attack_box.h = 60;           // Vertical range
    }
    
    // Enemy hitbox (same as in detecter_collision_position)
    SDL_Rect enemy_box = {
        .x = ennemi->x + 15,    // Add offset to center hitbox
        .y = ennemi->y + 5,     // Add offset to adjust hitbox from top
        .w = 45,                // Smaller width than visual sprite
        .h = 55                 // Smaller height than visual sprite
    };
    
    // Check if rectangles overlap
    if (player_attack_box.x < enemy_box.x + enemy_box.w &&
        player_attack_box.x + player_attack_box.w > enemy_box.x &&
        player_attack_box.y < enemy_box.y + enemy_box.h &&
        player_attack_box.y + player_attack_box.h > enemy_box.y) {
        
        ennemi->is_stunned = 1;
        ennemi->stun_timer = 30; // Half second stun
        return 1;
    }
    
    return 0;
}

int detecter_collision_attaque_ennemi(Personnage *pers, Enemy *ennemi) {
    if (ennemi->damage_cooldown > 0 || 
        ennemi->is_stunned || 
        pers->anim.state == PLAYER_HURT) {
        ennemi->damage_cooldown--;
        return 0;
    }

    if (ennemi->animation != ENEMY_ATTACKING || ennemi->state == ENEMY_NEUTRALIZED)
        return 0;

    // Player hitbox (same as in detecter_collision_position)
    SDL_Rect player_box = {
        .x = pers->x + 30,        // Add offset to adjust player hitbox
        .y = pers->y + 10,        // Add offset to adjust player hitbox
        .w = 85,                  // Smaller width than the sprite
        .h = 80                   // Smaller height than the sprite
    };
    
    // Enemy attack hitbox
    SDL_Rect enemy_attack_box;
    
    // Direction-based enemy attack box
    if (ennemi->direction == 1) { // Right
        enemy_attack_box.x = ennemi->x + 40;
        enemy_attack_box.y = ennemi->y + 10;
        enemy_attack_box.w = 50;
        enemy_attack_box.h = 40;
    } else if (ennemi->direction == 0) { // Left
        enemy_attack_box.x = ennemi->x - 30;
        enemy_attack_box.y = ennemi->y + 10;
        enemy_attack_box.w = 50;
        enemy_attack_box.h = 40;
    } else { // Up/Down (fallback to proximity check)
        enemy_attack_box.x = ennemi->x - 10;
        enemy_attack_box.y = ennemi->y - 10;
        enemy_attack_box.w = 65;
        enemy_attack_box.h = 75;
    }
    
    // Check if rectangles overlap
    if (player_box.x < enemy_attack_box.x + enemy_attack_box.w &&
        player_box.x + player_box.w > enemy_attack_box.x &&
        player_box.y < enemy_attack_box.y + enemy_attack_box.h &&
        player_box.y + player_box.h > enemy_attack_box.y) {
        
        ennemi->damage_cooldown = 180;  // 3 seconds at 60 FPS
        ennemi->movement_pause = 60;    // 1 second at 60 FPS
        
        // Add knockback
        float knockback_force = 10.0f;  // Adjust this value to control knockback strength
        if (strcmp(pers->direction, "right") == 0) {
            pers->knockback_velocity_x = -knockback_force;  // Push left if facing right
        } else {
            pers->knockback_velocity_x = knockback_force;   // Push right if facing left
        }
        pers->knockback_timer = 15;  // Knockback duration in frames
        
        return 1;
    }
    
    return 0;
}

void gerer_sante_ennemis(Enemy ennemis[], int nb_ennemis) {
    int i;
    
    for (i = 0; i < nb_ennemis; i++) {
        // Mettre à jour l'état en fonction de la santé
        if (ennemis[i].health <= 0) {
            ennemis[i].state = ENEMY_NEUTRALIZED;
        } else if (ennemis[i].health < (ennemis[i].niveau == 1 ? 50 : 75)) {
            ennemis[i].state = ENEMY_WOUNDED;
            // Réduire la vitesse quand blessé
            ennemis[i].vitesse = ennemis[i].niveau == 1 ? 1 : 2;
        }
        
        // Récupération graduelle si WOUNDED (optionnel)
        if (ennemis[i].state == ENEMY_WOUNDED && rand() % 500 == 0) {
            ennemis[i].health += 5;
            // S'assurer que la santé ne dépasse pas le maximum
            int max_health = ennemis[i].niveau == 1 ? 300 : 150;
            if (ennemis[i].health > max_health)
                ennemis[i].health = max_health;
                
            // Revenir à l'état ALIVE si assez récupéré
            if (ennemis[i].health >= (ennemis[i].niveau == 1 ? 50 : 75)) {
                ennemis[i].state = ENEMY_ALIVE;
                ennemis[i].vitesse = ennemis[i].niveau == 1 ? 2 : 3;
            }
        }
    }
}

void ia_mouvement_ennemi(Enemy *ennemi, Personnage *pers) {
    // Add null check
    if (!ennemi || !pers) return;

    if (ennemi->state == ENEMY_NEUTRALIZED || ennemi->movement_pause > 0) {
        if (ennemi->movement_pause > 0) {
            ennemi->movement_pause--;
        }
        return;
    }

    // Check for stun state
    if (ennemi->is_stunned) {
        ennemi->stun_timer--;
        if (ennemi->stun_timer <= 0) {
            ennemi->is_stunned = 0;
        }
        return;
    }

    // Apply gravity
    ennemi->velocity_y += GRAVITY;
    ennemi->y += ennemi->velocity_y;

    // Ground collision
    if (ennemi->y > GROUND_LEVEL - 64) {
        ennemi->y = GROUND_LEVEL - 64;
        ennemi->velocity_y = 0;
        ennemi->is_grounded = 1;
    }
        
    // Distance entre l'ennemi et le joueur
    float dx = pers->x - ennemi->x;
    float dy = pers->y - ennemi->y;
    float distance = sqrt(dx*dx + dy*dy);
    
    // Si le joueur est assez proche, l'ennemi le poursuit
    if (distance < 200) { // Rayon de détection
        // Calculate new position
        int new_x = dx > 0 ? 
            ennemi->x + ennemi->vitesse : 
            ennemi->x - ennemi->vitesse;
        
        // Check for collision with all enemies
        int can_move = 1;
        // Get base address of enemy array
        Enemy *all_enemies = (Enemy*)((char*)ennemi - (long)(ennemi->niveau == 1 ? 0 : 2) * sizeof(Enemy));
        for (int i = 0; i < MAX_ENNEMIS; i++) {
            if (&all_enemies[i] != ennemi && all_enemies[i].state != ENEMY_NEUTRALIZED) {
                Enemy temp = *ennemi;
                temp.x = new_x;
                if (detecter_collision_entre_ennemis(&temp, &all_enemies[i])) {
                    can_move = 0;
                    break;
                }
            }
        }
        
        if (can_move) {
            ennemi->x = new_x;
            ennemi->direction = dx > 0 ? 1 : 0; // Update direction
        }
        
        // Mettre à jour la direction pour l'animation
        if (fabs(dx) > fabs(dy)) {
            // Mouvement horizontal dominant
            ennemi->direction = dx > 0 ? 1 : 0; // Droite ou gauche
        } else {
            // Mouvement vertical dominant
            ennemi->direction = dy > 0 ? 3 : 2; // Bas ou haut
        }
        
        // Si très proche, passer en mode attaque
        if (distance < 80) {
            ennemi->animation = ENEMY_ATTACKING;
        } else {
            ennemi->animation = ENEMY_WALKING;
        }
    } else {
        // Comportement normal (déplacement aléatoire)
        if (rand() % 100 == 0) {
            ennemi->direction = rand() % 4;
        }
    }
}

/*------------------------------------*
 *    FONCTIONS MANQUANTES           *
 *------------------------------------*/

int detecter_collision_position(Personnage *pers, int new_x, int new_y, Enemy *ennemi) {
    // Calculate collision boxes
    SDL_Rect player_box = {
        .x = new_x + 30,        // Add offset to adjust player hitbox
        .y = new_y + 10,        // Add offset to adjust player hitbox
        .w = 85,                // Smaller width than the sprite
        .h = 80                 // Smaller height than the sprite
    };
    
    SDL_Rect enemy_box = {
        .x = ennemi->x + 15,    // Add offset to center hitbox
        .y = ennemi->y + 5,     // Add offset to adjust hitbox from top
        .w = 45,                // Smaller width than visual sprite
        .h = 55                 // Smaller height than visual sprite
    };
    
    // Check if rectangles overlap
    return (player_box.x < enemy_box.x + enemy_box.w &&
            player_box.x + player_box.w > enemy_box.x &&
            player_box.y < enemy_box.y + enemy_box.h &&
            player_box.y + player_box.h > enemy_box.y);
}

void update_animation_personnage(Personnage *pers) {
    // Apply knockback if timer is active
    if (pers->knockback_timer > 0) {
        pers->x += pers->knockback_velocity_x;
        pers->knockback_timer--;
        
        // Prevent going off screen during knockback
        if (pers->x < 0) pers->x = 0;
        if (pers->x > 1280 - 145) pers->x = 1280 - 145;
    }

    pers->frame_timer++;
    
    // Handle special states first
    if (pers->anim.state == PLAYER_ATTACK) {
        if (pers->frame_timer >= pers->anim.animation_speed) {
            pers->frame_timer = 0;
            pers->anim.current_frame++;
            
            // Attack hits on middle frames (frames 5-8 out of 12)
            pers->is_attacking = (pers->anim.current_frame >= 5 && pers->anim.current_frame <= 8);
            
            if (pers->anim.current_frame >= pers->anim.frame_counts[PLAYER_ATTACK]) {
                pers->anim.current_frame = 0;
                pers->anim.state = PLAYER_IDLE;
                pers->is_attacking = 0;
            }
        }
        return;
    }
    
    if (pers->anim.state == PLAYER_HURT) {
        if (pers->frame_timer >= pers->anim.animation_speed) {
            pers->frame_timer = 0;
            pers->anim.current_frame++;
            
            if (pers->anim.current_frame >= pers->anim.frame_counts[PLAYER_HURT]) {
                pers->anim.current_frame = 0;
                pers->anim.state = PLAYER_IDLE;
                pers->is_attacking = 0; // Reset attack state after hurt
            }
        }
        return;
    }
    
    if (pers->anim.state == PLAYER_DEATH) {
        if (pers->frame_timer >= pers->anim.animation_speed) {
            pers->frame_timer = 0;
            if (pers->anim.current_frame < pers->anim.frame_counts[PLAYER_DEATH] - 1) {
                pers->anim.current_frame++;
            }
        }
        return;
    }

    // Update state based on movement
    PlayerState new_state;
    if (pers->is_jumping) {
        new_state = PLAYER_JUMP;
    } else if (pers->moving_left || pers->moving_right) {
        new_state = PLAYER_RUN;
    } else {
        new_state = PLAYER_IDLE;
    }

    // Reset frame counter if state changed
    if (new_state != pers->anim.state) {
        pers->anim.state = new_state;
        pers->anim.current_frame = 0;
        pers->frame_timer = 0;
    }
    
    // Update animation frame
    if (pers->frame_timer >= pers->anim.animation_speed) {
        pers->frame_timer = 0;
        pers->anim.current_frame = (pers->anim.current_frame + 1) % 
            pers->anim.frame_counts[pers->anim.state];
    }
}

void afficher_personnage(Personnage *pers) {
    SDL_Rect src, dest;
    SDL_Surface *current_spritesheet;
    
    // Select the appropriate spritesheet based on direction and state
    if (strcmp(pers->direction, "right") == 0) {
        current_spritesheet = pers->anim.sprites_R[pers->anim.state];
    } else {
        current_spritesheet = pers->anim.sprites_L[pers->anim.state];
    }
    
    // Attack frames are 66px wide, others are 64px
    int frame_width = (pers->anim.state == PLAYER_ATTACK) ? 145 : 145; // All frames now 145px wide
    
    // Calculate source rectangle from spritesheet
    src.x = pers->anim.current_frame * frame_width;
    src.y = 0;
    src.w = frame_width;
    src.h = 100;  // Updated height to 100px
    
    // Set destination rectangle
    dest.x = pers->x;
    dest.y = pers->y;
    dest.w = frame_width;
    dest.h = 100;  // Updated height to 100px
    
    SDL_BlitSurface(current_spritesheet, &src, SDL_GetVideoSurface(), &dest);
}

void afficher_interface_jeu(int vie, int score) {
    // Création d'une police pour afficher les informations
    TTF_Font *police = NULL;
    
    // Essayer plusieurs chemins possibles pour les polices
    const char *chemins_polices[] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/FreeSans.ttf"
    };
    
    int i;
    for (i = 0; i < 4; i++) {
        police = TTF_OpenFont(chemins_polices[i], 16);
        if (police) break;
    }
    
    if (!police) {
        printf("Erreur: Impossible de charger une police. Le texte ne sera pas affiché.\n");
        return;
    }
    
    // Création d'une chaîne de caractères pour afficher la vie et le score
    char texte_vie[50];
    char texte_score[50];
    sprintf(texte_vie, "Vie: %d", vie);
    sprintf(texte_score, "Score: %d", score);
    
    // Couleur du texte (blanc)
    SDL_Color couleur_texte = {255, 255, 255};
    
    // Rendu du texte
    SDL_Surface *surface_vie = TTF_RenderText_Solid(police, texte_vie, couleur_texte);
    SDL_Surface *surface_score = TTF_RenderText_Solid(police, texte_score, couleur_texte);
    
    if (!surface_vie || !surface_score) {
        printf("Erreur lors du rendu du texte: %s\n", TTF_GetError());
        TTF_CloseFont(police);
        return;
    }
    
    // Position du texte (en haut à gauche)
    SDL_Rect position_vie = {10, 10, 0, 0};
    SDL_Rect position_score = {10, 30, 0, 0};
    
    // Affichage du texte
    SDL_BlitSurface(surface_vie, NULL, SDL_GetVideoSurface(), &position_vie);
    SDL_BlitSurface(surface_score, NULL, SDL_GetVideoSurface(), &position_score);
    
    // Libération des ressources
    SDL_FreeSurface(surface_vie);
    SDL_FreeSurface(surface_score);
    TTF_CloseFont(police);
}

void initialiser_boss(Boss *boss) {
    // Load boss sprites from the correct path
    const char *path_l = "resources/enemies/typhon_not_smile_spritesheet_L.png";
    const char *path_r = "resources/enemies/typhon_not_smile_spritesheet_R.png";
    
    boss->sprite_L = IMG_Load(path_l);
    boss->sprite_R = IMG_Load(path_r);
    
    if (!boss->sprite_L || !boss->sprite_R) {
        printf("Failed to load boss sprites from:\n%s\n%s\n", path_l, path_r);
        printf("Error: %s\n", IMG_GetError());
        exit(1);
    }
    
    printf("Successfully loaded boss sprites\n");
    
    boss->health = 2000;
    boss->current_frame = 0;
    boss->frame_timer = 0;
    boss->animation_speed = 8;
    boss->frame_count = 4;  // Updated to 4 frames
    boss->is_active = 0;
    boss->side = 0;
    boss->x = boss->side ? 1080 : 0;
    boss->y = GROUND_LEVEL - 150;  // Updated to match sprite height
}

void update_boss(Boss *boss) {
    // Don't update if boss is dead
    if (boss->health <= 0) {
        boss->is_active = 0;  // Deactivate boss when dead
        return;
    }

    boss->frame_timer++;
    
    if (boss->frame_timer >= boss->animation_speed) {
        boss->frame_timer = 0;
        boss->current_frame = (boss->current_frame + 1) % boss->frame_count;
        
        // Switch sides more frequently (every ~2 seconds at 60 FPS)
        if (rand() % 120 == 0) {  // Changed from 300 to 120
            boss->side = !boss->side;
            boss->x = boss->side ? 1080 : 0;
        }
    }
}

void afficher_boss(Boss *boss) {
    if (!boss->is_active) return;
    
    SDL_Surface *current_sprite = boss->side ? boss->sprite_L : boss->sprite_R;
    
    SDL_Rect src = {
        .x = boss->current_frame * 100,  // Updated width
        .y = 0,
        .w = 100,  // Updated width
        .h = 150   // Updated height
    };
    
    SDL_Rect dest = {
        .x = boss->x,
        .y = boss->y,
        .w = 100,  // Updated width
        .h = 150   // Updated height
    };
    
    SDL_BlitSurface(current_sprite, &src, SDL_GetVideoSurface(), &dest);
    
    // Only show health bar if boss is alive
    if (boss->health > 0) {
        SDL_Rect health_bar = {
            .x = boss->x,
            .y = boss->y - 20,
            .w = 100,
            .h = 10
        };
        
        // Background (gray)
        SDL_FillRect(SDL_GetVideoSurface(), &health_bar, 
                     SDL_MapRGB(SDL_GetVideoSurface()->format, 100, 100, 100));
        
        // Health (red)
        health_bar.w = (boss->health * 100) / 2000;
        SDL_FillRect(SDL_GetVideoSurface(), &health_bar, 
                     SDL_MapRGB(SDL_GetVideoSurface()->format, 255, 0, 0));
    }
}

int detecter_collision_joueur_boss(Personnage *pers, Boss *boss) {
    if (!boss->is_active || !pers->is_attacking || boss->health <= 0) return 0;
    
    // Only check collision during attack frames 5-8
    if (pers->anim.state != PLAYER_ATTACK || 
        pers->anim.current_frame < 5 || 
        pers->anim.current_frame > 8)
        return 0;
    
    // Player attack hitbox (similar to detecter_collision_joueur_ennemi)
    SDL_Rect player_attack_box;
    
    if (strcmp(pers->direction, "right") == 0) {
        player_attack_box.x = pers->x + 80;  // Extend to the right
        player_attack_box.y = pers->y + 20;  // Vertical center
        player_attack_box.w = 70;           // Attack range
        player_attack_box.h = 60;           // Vertical range
    } else {
        player_attack_box.x = pers->x - 30;  // Extend to the left
        player_attack_box.y = pers->y + 20;  // Vertical center
        player_attack_box.w = 70;           // Attack range
        player_attack_box.h = 60;           // Vertical range
    }
    
    // Boss hitbox (adjusted for better collision detection)
    SDL_Rect boss_box = {
        .x = boss->x + 20,         // Add offset to center hitbox
        .y = boss->y + 15,         // Add offset from top
        .w = 70,                  // Smaller width than visual sprite
        .h = 120                  // Smaller height than visual sprite
    };
    
    // Check if rectangles overlap
    return (player_attack_box.x < boss_box.x + boss_box.w &&
            player_attack_box.x + player_attack_box.w > boss_box.x &&
            player_attack_box.y < boss_box.y + boss_box.h &&
            player_attack_box.y + player_attack_box.h > boss_box.y);
}

// Add this new function to handle boss damage
void gerer_degats_boss(Boss *boss, int degats) {
    if (boss->health > 0) {
        boss->health = (boss->health > degats) ? boss->health - degats : 0;
        printf("Boss hit! Health: %d\n", boss->health);
    }
}
