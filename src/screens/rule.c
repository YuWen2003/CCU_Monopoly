#include <SFML/Audio.h>
#include <SFML/Graphics.h>

#include "screens.h"
#include "widgets.h"

#define RULE_LENGTH 6

screen_position show_rule_screen(sfRenderWindow* window)
{
    wchar_t rules[RULE_LENGTH][100] =  {
        L"遊戲規則:初始金額10000元", 
        L"玩法:1.停在無人之系館,選擇是否購買", 
        L"2.停在他人之系館,支付過路費", 
        L"3.停在自己之系館,選擇是否捐一棟系館", 
        L"勝利條件:1. 賺到20000元",
        L"2.擁有總共15棟系館"
        };
    sfText* back_text, *rule_text[RULE_LENGTH];
    sfFont* font;
    sfEvent event;
    sfFloatRect bounds;

    float SCRWIDTH = sfVideoMode_getDesktopMode().width;
    float SCRHEIGHT = sfVideoMode_getDesktopMode().height;
    screen_position destination = NoneScreen;

    /* ================= create resources ================== */
    font = sfFont_createFromFile("resource/font/mingliu.ttc");
    back_text = createText("Back", 25, sfWhite, font);
    for(int i = 0; i < RULE_LENGTH; i++) {
        rule_text[i] = createWideText(rules[i], 25, sfWhite, font);
        putText(rule_text[i], SCRWIDTH/2.0f, SCRHEIGHT/2.0f + 80 * (i - 3));
    }
    putText(back_text, SCRWIDTH/2.0f, SCRHEIGHT/2.0f + 80 * (7 - 3));
    
    /* ================= game loop ================== */
    while (sfRenderWindow_isOpen(window) && destination == NoneScreen)
    {
        /* Process events */
        sfVector2i mouse_position = sfMouse_getPosition((sfWindow*)window);
        while (sfRenderWindow_pollEvent(window, &event))
        {
            /* Close window : exit */
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);
            if (event.type == sfEvtMouseButtonPressed) {
                if (isMouseInsideText(back_text, mouse_position))
                    destination = MenuScreen;
            }   
        }

        /* Clear the screen */
        sfRenderWindow_clear(window, sfBlack);

        sfText_setFillColor(back_text, sfWhite);
        if(isMouseInsideText(back_text, mouse_position))
            sfText_setFillColor(back_text, sfGreen);

        /* Draw the text */
        sfRenderWindow_drawText(window, back_text, NULL);
        for(int i = 0; i < RULE_LENGTH; i++)
            sfRenderWindow_drawText(window, rule_text[i], NULL);

        /* Update the window */
        sfRenderWindow_display(window);
    }

    /* ================= Cleanup resources ================== */
    sfFont_destroy(font);
    for(int i = 0; i < RULE_LENGTH; i++)
        sfText_destroy(rule_text[i]);

    return destination;
}

/*THIS CODE IS MY OWN WORK. IT WAS WRITTEN WITHOUT CONSULTING CODE WRITTEN BY OTHER STUDENTS OR ANY ONE WHO IS NOT AWARE OF MY REFERENCE. 410125034 陳俞妏 408520059 林筠晏*/