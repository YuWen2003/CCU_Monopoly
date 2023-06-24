#include <SFML/Audio.h>
#include <SFML/Graphics.h>

#include "screens.h"
#include "widgets.h"


screen_position show_menu_screen(sfRenderWindow* window)
{
    SpriteHolder* background_sprite, *option_sprite;
    sfText* title, *op1, *op2, *op3;
    sfFont* font;
    sfEvent event;
    sfFloatRect bounds;
    
    screen_position destination = NoneScreen;
    float SCRWIDTH = sfVideoMode_getDesktopMode().width;
    float SCRHEIGHT = sfVideoMode_getDesktopMode().height;

    /* ================= create resources ================== */
    font = sfFont_createFromFile("resource/font/mingliu.ttc");
    background_sprite = create_sprite_holder("resource/pic/menu_background.png");
    option_sprite = create_sprite_holder("resource/pic/button.png");
    if (!font || !background_sprite || !option_sprite)
        return destination;

    title = createText("MAIN MENU", 25, sfBlack, font);
    op1 = createText("PLAY", 25, sfWhite, font);
    op2 = createText("ROLE", 25, sfWhite, font);
    op3 = createText("QUIT", 25, sfWhite, font);
    
    // resize background
    bounds = sfSprite_getLocalBounds(background_sprite->sprite);
    sfSprite_setScale(background_sprite->sprite, (sfVector2f){ SCRWIDTH / bounds.width, SCRHEIGHT / bounds.height });
    // resize button
    bounds = sfSprite_getLocalBounds(option_sprite->sprite);
    sfSprite_setScale(option_sprite->sprite, (sfVector2f){ 100 / bounds.width, 43 / bounds.height });
    
    // place text
    putText(title, SCRWIDTH/2.0f, 70.0f);
    putText(op1, SCRWIDTH/2.0f, 250.0f);
    putText(op2, SCRWIDTH/2.0f, 400.0f);
    putText(op3, SCRWIDTH/2.0f, 550.0f);

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
            /* Mouse click */
            if (event.type == sfEvtMouseButtonPressed) {
                if(isMouseInsideText(op1, mouse_position))
                    destination = UserScreen;
                if(isMouseInsideText(op2, mouse_position))
                    destination = RuleScreen;
                if(isMouseInsideText(op3, mouse_position))
                    sfRenderWindow_close(window);
            }   
        }

        /* Clear the screen */
        sfRenderWindow_clear(window, sfBlack);

        /* reset buttons */
        sfText_setFillColor(op1, sfWhite);
        sfText_setFillColor(op2, sfWhite);
        sfText_setFillColor(op3, sfWhite);
        
        /* change button when hover */
        if(isMouseInsideText(op1, mouse_position))
           sfText_setFillColor(op1, sfBlack);
        if(isMouseInsideText(op2, mouse_position))
           sfText_setFillColor(op2, sfBlack);
        if(isMouseInsideText(op3, mouse_position))
            sfText_setFillColor(op3, sfBlack);
        
        /* Draw sprite */
        sfRenderWindow_drawSprite(window, background_sprite->sprite, NULL);
        sfRenderWindow_drawText(window, title, NULL);
        /* Draw button background */
        putSprite(option_sprite, SCRWIDTH/2.0f, 250.0f);        
        sfRenderWindow_drawSprite(window, option_sprite->sprite, NULL);
        putSprite(option_sprite, SCRWIDTH/2.0f, 400.0f);
        sfRenderWindow_drawSprite(window, option_sprite->sprite, NULL);
        putSprite(option_sprite, SCRWIDTH/2.0f, 550.0f);
        sfRenderWindow_drawSprite(window, option_sprite->sprite, NULL);
        /* Draw button text */
        sfRenderWindow_drawText(window, op1, NULL);
        sfRenderWindow_drawText(window, op2, NULL);
        sfRenderWindow_drawText(window, op3, NULL);
        /* Update the window */
        sfRenderWindow_display(window);
    }

    /* ================= Cleanup resources ================== */
    sfFont_destroy(font);
    sprite_holder_destroy(background_sprite);
    sprite_holder_destroy(option_sprite);
    sfText_destroy(title);
    sfText_destroy(op1);
    sfText_destroy(op2);
    sfText_destroy(op3);

    return destination;
}

/*THIS CODE IS MY OWN WORK. IT WAS WRITTEN WITHOUT CONSULTING CODE WRITTEN BY OTHER STUDENTS OR ANY ONE WHO IS NOT AWARE OF MY REFERENCE. 410125034 ³¯«\Ê¹ 408520059 ªL¸a®Ë*/