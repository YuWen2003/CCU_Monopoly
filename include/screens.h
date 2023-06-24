#ifndef MONO_SCREEN_H
#define MONO_SCREEN_H

typedef enum screen_position {
    MenuScreen,
    RuleScreen,
    UserScreen,
    PlayScreen,
    NoneScreen
} screen_position;

screen_position show_menu_screen(sfRenderWindow* window);
screen_position show_rule_screen(sfRenderWindow* window);
screen_position show_user_screen(sfRenderWindow* window, int *player_count, char player_name[4][50]);
screen_position show_play_screen(sfRenderWindow* window, int player_count, char player_name[4][50]);

#endif 