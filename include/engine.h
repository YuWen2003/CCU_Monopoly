#ifndef MONO_GAME_H
#define MONO_GAME_H

#include <SFML/Audio.h>
#include <SFML/Graphics.h>
#include "object.h"
#include "screens.h"

#define PLAYER_NUM 4
#define DICE_NUM 6
#define BUILDING_NUM 36

typedef enum game_state {
    WaitPlayerDice,
    DiceMove,
    WaitPlayerBuy,
    WaitPlayerFinish,
    TurnPlayer,
    WaitAnim,
    GameEnd
} game_state;


typedef struct Game_engine {
    sfMusic *dice_sound, *buy_sound, *win_sound;
    sfClock* anim_timer;
    Building* buildings;
    int chance_building[4];

    Player player[PLAYER_NUM];

    int currentPlayer;
    int winner;
    int currentDice;

    game_state state;
    game_state next_state;

} Game_engine;

void create_engine(Game_engine *game_engine, int player_count, char player_name[4][50], sfFont *font);
void destroy_engine(Game_engine *game_engine);
Building* getBuildings();
void do_dice(Game_engine *game_engine, wchar_t * status_string);
void buy_player(Game_engine *game_engine, wchar_t * status_string);
void handle_state(Game_engine *game_engine, wchar_t * status_string, screen_position *destination);
#endif 