#include <SFML/Audio.h>
#include <SFML/Graphics.h>

#include <string.h>
#include "screens.h"
#include "widgets.h"

#define INPUT_NUM 5
#define MAX_STR_LENGTH 100

screen_position show_user_screen(sfRenderWindow* window, int *player_count, char player_name[4][50])
{
    int current_progress = 0;
    int show_indicator = -1;
    int indicator_status = 0;
    int mouse_indicator = -1;
    int user_count = 0;
    char tmp_str[1024] = "";
    wchar_t questions[INPUT_NUM][MAX_STR_LENGTH] =  {L"輸入玩家數量", L"輸入玩家1姓名", L"輸入玩家2姓名", L"輸入玩家3姓名", L"輸入玩家4姓名"};
    
    sfText* userInputText[INPUT_NUM];
    sfText* userAnswerText[INPUT_NUM];
    sfRectangleShape* inputBox[INPUT_NUM];
    sfText* warningText;
    sfFont* font;
    sfClock* inputIndicatorClock;
    sfCursor* cursorArrow;
    sfCursor* cursorText;
    sfRectangleShape* inputIndicator;
    sfTime inputIndicatorTime = sfTime_Zero;
    sfEvent event;
    sfFloatRect bounds;
    
    screen_position destination = NoneScreen;
    float SCRWIDTH = sfVideoMode_getDesktopMode().width;
    float SCRHEIGHT = sfVideoMode_getDesktopMode().height;

    /* ================= create resources ================== */
    font = sfFont_createFromFile("resource/font/mingliu.ttc");
    warningText = createWideText(L"輸入1到4的數字", 25, sfWhite, font);
    inputIndicatorClock = sfClock_create();
    inputIndicator = sfRectangleShape_create();
    cursorArrow = sfCursor_createFromSystem(sfCursorArrow);
    cursorText = sfCursor_createFromSystem(sfCursorText);

    // place input box
    putText(warningText, 350.0f, 175.0f);
    for(int i = 0; i < INPUT_NUM; i++) {
        userInputText[i] = createWideText(questions[i], 25, sfWhite, font);
        putText(userInputText[i], 100.0f, 330.0f + i * 50);
        inputBox[i] = createTextBox(400, 50, sfWhite, sfBlack);
        putRectangleShapeBox(inputBox[i], 400.0f, 330.0f + i * 50);
        userAnswerText[i] = createText("", 25, sfWhite, font);
        putText(userAnswerText[i], 200.0f, 315.0f + i * 50);
    }

    // setup input indicator
    sfRectangleShape_setSize(inputIndicator, (sfVector2f){2, 24});
    sfRectangleShape_setFillColor(inputIndicator, sfWhite);
    
    /* ================= game loop ================== */
    while (sfRenderWindow_isOpen(window) && destination == NoneScreen)
    {
        sfVector2i mouse_position = sfMouse_getPosition((sfWindow*)window);
        
        // update indicator position
        mouse_indicator = -1;
        for(int i = 0; i < user_count + 1; i++) {
            bounds = sfRectangleShape_getGlobalBounds(inputBox[i]);
            if (sfFloatRect_contains(&bounds, mouse_position.x, mouse_position.y))
                mouse_indicator = i;
        }
        /* Process events */
        while (sfRenderWindow_pollEvent(window, &event))
        {
            /* Close window : exit */
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);
            /* Update indicator status */
            if (event.type == sfEvtMouseButtonPressed) {
                show_indicator = mouse_indicator;
            /* user input */
            } else if (event.type == sfEvtTextEntered) {
                if (show_indicator != -1) {
                    if (event.text.unicode < 128)
                    {
                        const char * content = sfText_getString(userAnswerText[show_indicator]);
                        int len = strlen(content);
                        strcpy(tmp_str, content);
                        // delete
                        if (event.text.unicode == 8) {
                            if(len > 0) {
                                tmp_str[len - 1] = '\0';
                                len -= 1;
                            }
                        // enter
                        } else if (event.text.unicode == 13) {
                            if(user_count <= 4 && user_count > 0) {
                                *player_count = user_count;
                                for(int i = 0; i < user_count; i++) {
                                    content = sfText_getString(userAnswerText[i + 1]);
                                    strcpy(player_name[i], content);
                                }
                                destination = PlayScreen;
                                break;
                            } else
                                continue;
                        // tab
                        } else if (event.text.unicode == 9) {
                            if(show_indicator != -1 && show_indicator + 1 <= user_count) {
                                show_indicator += 1;
                                continue;
                            }
                        }
                        else {
                            if(len > 20)
                                continue;
                            tmp_str[len] = event.text.unicode;
                            tmp_str[len + 1] = '\0';
                            len += 1;
                        }
                        sfText_setString(userAnswerText[show_indicator], tmp_str);                        
                        if(show_indicator == 0) {
                            user_count = 0;
                            if (len == 1) {
                                user_count = tmp_str[0] - 48;
                                if(user_count > 4 || user_count <= 0)
                                    user_count = 0;
                            }
                        }
                    }
                }
            }
        }

        /* Clear the screen */
        sfRenderWindow_clear(window, sfBlack);
        
        /* Draw the sprite */
        inputIndicatorTime = sfClock_getElapsedTime(inputIndicatorClock);
        if (inputIndicatorTime.microseconds >= 500000) // Blink every 0.5 seconds
        {
            indicator_status = !indicator_status;
            sfClock_restart(inputIndicatorClock);
        }

        // update mouse cursor
        if(mouse_indicator == -1)
            sfWindow_setMouseCursor((sfWindow*)window, cursorArrow);
        else
            sfWindow_setMouseCursor((sfWindow*)window, cursorText);
        
        /* Draw the text */
        // warn invalid user count
        if(user_count > 4 || user_count < 1)
            sfRenderWindow_drawText(window, warningText, NULL);

        for(int i = 0; i < user_count + 1; i++) {
            sfRenderWindow_drawRectangleShape(window, inputBox[i], NULL);
            sfRenderWindow_drawText(window, userInputText[i], NULL);
            sfRenderWindow_drawText(window, userAnswerText[i], NULL);
        }
        
        // draw indicator
        if (show_indicator != -1 && indicator_status) {
            bounds = sfText_getGlobalBounds(userAnswerText[show_indicator]);
            sfRectangleShape_setPosition(inputIndicator, (sfVector2f){bounds.left + bounds.width + 3, bounds.top});
            sfRenderWindow_drawRectangleShape(window, inputIndicator, NULL);
        }

        /* Update the window */
        sfRenderWindow_display(window);
    }

    sfWindow_setMouseCursor((sfWindow*)window, cursorArrow);
    /* ================= Cleanup resources ================== */
    sfFont_destroy(font);
    sfText_destroy(warningText);
    sfCursor_destroy(cursorArrow);
    sfCursor_destroy(cursorText);
    for(int i = 0; i < INPUT_NUM; i++) {
        sfText_destroy(userInputText[i]);
        sfText_destroy(userAnswerText[i]);
        sfRectangleShape_destroy(inputBox[i]);
    }
    sfClock_destroy(inputIndicatorClock);
    sfRectangleShape_destroy(inputIndicator);
    return destination;
}

/*THIS CODE IS MY OWN WORK. IT WAS WRITTEN WITHOUT CONSULTING CODE WRITTEN BY OTHER STUDENTS OR ANY ONE WHO IS NOT AWARE OF MY REFERENCE. 410125034 陳俞妏 408520059 林筠晏*/