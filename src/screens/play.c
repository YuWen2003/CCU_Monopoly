#include <SFML/Audio.h>
#include <SFML/Graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>

#include "screens.h"
#include "widgets.h"
#include "object.h"
#include "engine.h"

screen_position show_play_screen(sfRenderWindow* window, int player_count, char player_name[4][50])
{
    int show_building_intro = 1;
    char filename[100];
    wchar_t intro_text[100] = {0};
    wchar_t status_string[100] = {0};
    wchar_t winner_string[100] = {0};

    sfFont* font;
    SpriteHolder *player_sprites[PLAYER_NUM], *player_selected_sprites[PLAYER_NUM], *player_pos_sprites[PLAYER_NUM];
    SpriteHolder *map_sprite, *button_sprites, *dice_sprites[DICE_NUM];
    sfMusic* music;
    sfText *status_text, *winner_text;
    sfText *op1, *op2, *op3;
    sfRectangleShape* building_intro_bg;
    sfText *building_intro_text;
    sfEvent event;
    sfFloatRect bounds;
    float SCRWIDTH = sfVideoMode_getDesktopMode().width;
    float SCRHEIGHT = sfVideoMode_getDesktopMode().height;
    screen_position destination = NoneScreen;
    
    font = sfFont_createFromFile("resource/font/mingliu.ttc");
    // init engine
    Game_engine game_engine;
    create_engine(&game_engine, player_count, player_name, font);
    
    /* ================= create resources ================== */
    // player relatec
    for (int i = 0; i < PLAYER_NUM; i++) {
        // load image
        sprintf(filename, "resource/pic/chess%d.png", i+1);
        player_pos_sprites[i] = create_sprite_holder(filename);
        sprintf(filename, "resource/pic/StatusBar%d.png", i+1);
        player_sprites[i] = create_sprite_holder(filename);
        sprintf(filename, "resource/pic/StatusBar%d_glow.png", i+1);
        player_selected_sprites[i] = create_sprite_holder(filename);
        
        int deltaX = 0, deltaY = 0;
        // unselected
        bounds = sfSprite_getLocalBounds(player_sprites[i]->sprite);
        float width = (SCRWIDTH - SCRHEIGHT) / 2;
        float height = width / bounds.width * bounds.height;
        if (i % 2 == 1)
            deltaX = width;
        if (i > 1)
            deltaY = SCRHEIGHT - height;
        sfSprite_setScale(player_sprites[i]->sprite, (sfVector2f){ width / bounds.width, height / bounds.height });
        putSprite(player_sprites[i], deltaX + SCRHEIGHT + width / 2, deltaY + height / 2);
        // selected
        bounds = sfSprite_getLocalBounds(player_selected_sprites[i]->sprite);
        width = (SCRWIDTH - SCRHEIGHT) / 2;
        height = width / bounds.width * bounds.height;
        sfSprite_setScale(player_selected_sprites[i]->sprite, (sfVector2f){ width / bounds.width, height / bounds.height });
        putSprite(player_selected_sprites[i], deltaX + SCRHEIGHT + width / 2, deltaY + height / 2);
        // position
        bounds = sfSprite_getLocalBounds(player_pos_sprites[i]->sprite);
        int pos_size = game_engine.buildings[1].width * 0.6 * SCRHEIGHT;
        sfSprite_setScale(player_pos_sprites[i]->sprite, (sfVector2f){ pos_size / bounds.width, pos_size / bounds.height });

        // names
        putText(game_engine.player[i].name, deltaX + SCRHEIGHT + width / 2, deltaY + height * 0.25);
        putText(game_engine.player[i].info_text, deltaX + SCRHEIGHT + width / 3 * 2, deltaY + height * 0.45);
    }
    // dice
    for (int i = 0; i < DICE_NUM; i++) {
        sprintf(filename, "resource/pic/dice%d.png", i+1);
        dice_sprites[i] = create_sprite_holder(filename);
        putSprite(dice_sprites[i], SCRHEIGHT/2.0f - 200, SCRHEIGHT / 2 + 200);
        sfSprite_setScale(dice_sprites[i]->sprite, (sfVector2f){ 100 / sfSprite_getLocalBounds(dice_sprites[i]->sprite).width, 100 / sfSprite_getLocalBounds(dice_sprites[i]->sprite).height });
    }
    // map
    map_sprite = create_sprite_holder("resource/pic/ccu_map.png");
    sfSprite_setScale(map_sprite->sprite, (sfVector2f){ SCRHEIGHT / sfSprite_getLocalBounds(map_sprite->sprite).width, SCRHEIGHT / sfSprite_getLocalBounds(map_sprite->sprite).height });

    // action buttons
    button_sprites = create_sprite_holder("resource/pic/button.png");
    sfSprite_setScale(button_sprites->sprite, (sfVector2f){ 130 / sfSprite_getLocalBounds(button_sprites->sprite).width, 70 / sfSprite_getLocalBounds(button_sprites->sprite).height });
    op1 = createWideText(L"??²éª°å­?", 25, sfWhite, font);
    op2 = createWideText(L"çµ??????????", 25, sfWhite, font);
    op3 = createWideText(L"è³¼è²·/??????", 25, sfWhite, font);
    putText(op1, SCRHEIGHT/2.0f, SCRHEIGHT / 2 - 100.0f + 0 * 100);
    putText(op2, SCRHEIGHT/2.0f, SCRHEIGHT / 2 - 100.0f + 1 * 100);
    putText(op3, SCRHEIGHT/2.0f, SCRHEIGHT / 2 - 100.0f + 2 * 100);

    // building info textbox
    building_intro_bg = sfRectangleShape_create();
    sfRectangleShape_setFillColor(building_intro_bg, sfWhite);
    building_intro_text = createWideText(L"", 25, sfBlack, font);
    // game status
    status_text = createWideText(L"", 25, sfWhite, font);
    putText(status_text, SCRHEIGHT + (SCRWIDTH - SCRHEIGHT) / 2, SCRHEIGHT / 2);
    // winner text
    winner_text = createWideText(L"", 150, sfYellow, font);

    /* Load a music to play */
    music = sfMusic_createFromFile("resource/sound/bgm.wav");
    sfMusic_play(music);
    sfMusic_setVolume(music, 10);
    sfMusic_setLoop(music, 30);

    /* ================= game loop ================== */
    while (sfRenderWindow_isOpen(window) && destination == NoneScreen)
    {
        /* Process events */
        sfVector2i mouse_position = sfMouse_getPosition((sfWindow*)window);
        while (sfRenderWindow_pollEvent(window, &event))
        {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);
            if (event.type == sfEvtMouseButtonPressed) {
                // player action
                if (game_engine.player[game_engine.currentPlayer].isPlayer) {
                    if(isMouseInsideText(op1, mouse_position)) {
                        if(game_engine.state == WaitPlayerDice)
                            do_dice(&game_engine, status_string);
                    } else if (isMouseInsideText(op2, mouse_position)) {
                        if(game_engine.state == WaitPlayerFinish || game_engine.state == WaitPlayerBuy)
                            game_engine.state = TurnPlayer;
                    } else if (isMouseInsideText(op3, mouse_position)) {
                        if(game_engine.state == WaitPlayerBuy) {
                            buy_player(&game_engine, status_string);
                        }
                    }
                }
            }
        }
        /* game state */
        show_building_intro = -1;
        for(int i = 0; i < BUILDING_NUM; i++) {
            if (mouse_position.x >= SCRHEIGHT * game_engine.buildings[i].x && mouse_position.x <= (SCRHEIGHT * (game_engine.buildings[i].x + game_engine.buildings[i].width)))
                if (mouse_position.y >= SCRHEIGHT * game_engine.buildings[i].y && mouse_position.y <= (SCRHEIGHT * (game_engine.buildings[i].y + game_engine.buildings[i].height)))
                    show_building_intro = i;
        }

        handle_state(&game_engine, status_string, &destination);

        /* Clear the screen */
        sfRenderWindow_clear(window, sfBlack);

        /* Draw the sprite */
        // map
        sfRenderWindow_drawSprite(window, map_sprite->sprite, NULL);
        // user action buttons
        for( int i = 0; i < 3; i++) {
            putSprite(button_sprites, SCRHEIGHT/2.0f, SCRHEIGHT / 2 - 100.0f + i * 100);
            sfRenderWindow_drawSprite(window, button_sprites->sprite, NULL);
        }
        sfText_setFillColor(op1, sfWhite);
        sfText_setFillColor(op2, sfWhite);
        sfText_setFillColor(op3, sfWhite);

        bounds = sfText_getGlobalBounds(op1);
        if (sfFloatRect_contains(&bounds, mouse_position.x, mouse_position.y))
            sfText_setFillColor(op1, sfBlack);
        bounds = sfText_getGlobalBounds(op2);
        if (sfFloatRect_contains(&bounds, mouse_position.x, mouse_position.y))
            sfText_setFillColor(op2, sfBlack);
        bounds = sfText_getGlobalBounds(op3);
        if (sfFloatRect_contains(&bounds, mouse_position.x, mouse_position.y))
            sfText_setFillColor(op3, sfBlack);
        
        sfRenderWindow_drawText(window, op1, NULL);
        sfRenderWindow_drawText(window, op2, NULL);
        sfRenderWindow_drawText(window, op3, NULL);
        //dice
        if(game_engine.state == WaitAnim && game_engine.next_state == DiceMove) {
            sfRenderWindow_drawSprite(window, dice_sprites[rand() % DICE_NUM]->sprite, NULL);
        } else
            sfRenderWindow_drawSprite(window, dice_sprites[game_engine.currentDice]->sprite, NULL);

        // player info
        for (int i = 0; i < PLAYER_NUM; i++) {
            if(game_engine.currentPlayer == i) {
                sfRenderWindow_drawSprite(window, player_selected_sprites[i]->sprite, NULL);
            } else {
                sfRenderWindow_drawSprite(window, player_sprites[i]->sprite, NULL);
            }
            int deltaX = 0, deltaY = 0;
            int x = game_engine.buildings[game_engine.player[i].position].x * SCRHEIGHT;
            int y = game_engine.buildings[game_engine.player[i].position].y * SCRHEIGHT;
            if (i % 2 == 1)
                deltaX += game_engine.buildings[game_engine.player[i].position].width * SCRHEIGHT * 0.8;
            else
                deltaX += game_engine.buildings[game_engine.player[i].position].width * SCRHEIGHT * 0.2;
            if (i > 1)
                deltaY += game_engine.buildings[game_engine.player[i].position].height * SCRHEIGHT * 0.8;
            else
                deltaY += game_engine.buildings[game_engine.player[i].position].height * SCRHEIGHT * 0.2;
            putSprite(player_pos_sprites[i], x + deltaX, y + deltaY);
            sfRenderWindow_drawSprite(window, player_pos_sprites[i]->sprite, NULL);

            sprintf(filename, "%d\n\n%d", game_engine.player[i].money, game_engine.player[i].ownedBuildingCount);
            sfText_setString(game_engine.player[i].info_text, filename);
            sfRenderWindow_drawText(window, game_engine.player[i].name, NULL);
            sfRenderWindow_drawText(window, game_engine.player[i].info_text, NULL);
        }

        // building intro box
        if(show_building_intro != -1) {
            getBuildingIntro(&(game_engine.buildings[show_building_intro]), intro_text, game_engine.player);
            setWideText(building_intro_text, intro_text);

            int padding = 10;
            bounds = sfText_getGlobalBounds(building_intro_text);
            putText(building_intro_text, SCRHEIGHT/2.0f + 200, SCRHEIGHT / 2 - 200);
            sfRectangleShape_setSize(building_intro_bg, (sfVector2f){bounds.width + padding * 2, bounds.height + padding * 2});
            putRectangleShapeBox(building_intro_bg, SCRHEIGHT/2.0f + 200, SCRHEIGHT / 2 - 200);
            
            sfRenderWindow_drawRectangleShape(window, building_intro_bg, NULL);
            sfRenderWindow_drawText(window, building_intro_text, NULL);
        }
        // game status box
        putText(status_text, SCRHEIGHT + (SCRWIDTH - SCRHEIGHT) / 2, SCRHEIGHT / 2);
        setWideText(status_text, status_string);
        sfRenderWindow_drawText(window, status_text, NULL);

        // winner text
        if(game_engine.state == GameEnd) {
            size_t i = 0;
            const sfUint32* user_name = sfText_getUnicodeString(game_engine.player[game_engine.winner].name);
            while(user_name[i])
                winner_string[i++] = (wchar_t)user_name[i];
            winner_string[i++] = '\0';
            wcscat(winner_string, L" WIN!!");
            setWideText(winner_text, winner_string);

            putText(winner_text, SCRWIDTH / 2, SCRHEIGHT / 2);
            sfRenderWindow_drawText(window, winner_text, NULL);   
        }

        sfRenderWindow_display(window);
    }
    
    /* Cleanup resources */
    sfMusic_stop(music);
    sfFont_destroy(font);
    for (int i = 0; i < PLAYER_NUM; i++) {
        sprite_holder_destroy(player_sprites[i]);
        sprite_holder_destroy(player_selected_sprites[i]);
        sprite_holder_destroy(player_pos_sprites[i]);
    }
    for (int i = 0; i < DICE_NUM; i++) {
        sprite_holder_destroy(dice_sprites[i]);
    }
    sprite_holder_destroy(map_sprite);
    sprite_holder_destroy(button_sprites);
    sfText_destroy(op1);
    sfText_destroy(op2);
    sfText_destroy(op3);
    sfText_destroy(status_text);
    sfText_destroy(winner_text);
    sfText_destroy(building_intro_text);
    sfRectangleShape_destroy(building_intro_bg);
    sfMusic_destroy(music);
    
    destroy_engine(&game_engine);

    return destination;
}

/*THIS CODE IS MY OWN WORK. IT WAS WRITTEN WITHOUT CONSULTING CODE WRITTEN BY OTHER STUDENTS OR ANY ONE WHO IS NOT AWARE OF MY REFERENCE. 410125034 ³¯«\Ê¹ 408520059 ªL¸a®Ë*/