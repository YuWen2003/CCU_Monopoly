#include <SFML/Audio.h>
#include <SFML/Graphics.h>
#include "screens.h"

int main()
{
    size_t count = 0;
    /* Enter full screen mode */
    sfRenderWindow* window = sfRenderWindow_create(sfVideoMode_getFullscreenModes(&count)[0], "CCU Monopoly", sfFullscreen, NULL);
    if (!window)
        return 1;
    
    sfMusic* music;
    /* Load a music to play */
    music = sfMusic_createFromFile("resource/sound/bgm.wav");
    if (!music)
        return 1;

    /* Play the music */
    sfMusic_play(music);
    sfMusic_setLoop(music, 30);

    screen_position position = MenuScreen;
    int player_count;
    char player_name[4][50];

    /* show ui screen */
    while(position != NoneScreen) {
        switch(position) {
            case MenuScreen:
                position = show_menu_screen(window);
                break;
            case RuleScreen:
                position = show_rule_screen(window);
                break;
            case UserScreen:
                position = show_user_screen(window, &player_count, player_name);
                break;
            case PlayScreen:
                sfMusic_stop(music);
                position = show_play_screen(window, player_count, player_name);
                sfMusic_play(music);
                break;
            default:
                break;
        }
    }

    /* recycle */
    sfMusic_destroy(music);
    sfRenderWindow_destroy(window);
    return 0;
}

/*THIS CODE IS MY OWN WORK. IT WAS WRITTEN WITHOUT CONSULTING CODE WRITTEN BY OTHER STUDENTS OR ANY ONE WHO IS NOT AWARE OF MY REFERENCE. 410125034 ³¯«\Ê¹ 408520059 ªL¸a®Ë*/