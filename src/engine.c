#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <time.h>
#include "engine.h"
#include "widgets.h"

/* game params */
#define COM_WAIT_TIME_MICRO 2500000
#define NEXT_NEW_GAME_GAP 13000000
#define CHANCE_CARD_NUM 2
#define FATE_CARD_NUM 2
wchar_t CHANCE_STR[][100] = {L"機會效果:作業被助教選中，加一分，\n去買一箱飲料慶祝:+500", L"在活中前擺攤 \n賣點心:+1000"};
int CHANCE_GAIN[] = {500, 1000};
wchar_t FATE_STR[][100] = {L"命運效果:為了享受系服的\n折價，決定補繳系學會費:-2500", L"積極參與所有抽獎，\n不斷分享抽獎頁面，\n瘋狂填問卷，\n最後成功中頭獎:+1500"};
int FATE_GAIN[] = {-2500, 1500};

/* engine */
void create_engine(Game_engine *game_engine, int player_count, char player_name[4][50], sfFont *font) {
    srand(time(0));
    game_engine->dice_sound = sfMusic_createFromFile("resource/sound/dice.wav");
    game_engine->buy_sound = sfMusic_createFromFile("resource/sound/ding.wav");
    game_engine->win_sound = sfMusic_createFromFile("resource/sound/win.wav");
    sfMusic_setLoop(game_engine->dice_sound, 0);
    sfMusic_setLoop(game_engine->buy_sound, 0);
    sfMusic_setLoop(game_engine->win_sound, 0);

    game_engine->anim_timer = sfClock_create();
    game_engine->buildings = getBuildings();
    
    // init player
    for (int i = 0; i < PLAYER_NUM; i++) {
        sfText *name, *info;
        if (i < player_count)
            name = createText(player_name[i], 25, sfWhite, font);
        else {
            wchar_t com_name[4] = L"電腦";
            com_name[2] = i-player_count+1+L'0';
            com_name[3] = '\0';
            name = createWideText(com_name, 25, sfWhite, font);
        }
        info = createText("", 25, sfWhite, font);
        createPlayer(&(game_engine->player[i]), name, info, i < player_count);
    }
    
    game_engine->currentPlayer = -1;
    game_engine->currentDice = 0;
    game_engine->state = TurnPlayer;
    game_engine->next_state = TurnPlayer;
};

void destroy_engine(Game_engine *game_engine) {
    sfMusic_destroy(game_engine->dice_sound);
    sfMusic_destroy(game_engine->buy_sound);
    sfMusic_destroy(game_engine->win_sound);
    sfClock_destroy(game_engine->anim_timer);
    free(game_engine->buildings);
    for (int i = 0; i < PLAYER_NUM; i++) {
        sfText_destroy(game_engine->player[i].name);
        sfText_destroy(game_engine->player[i].info_text);
    }
}

/* spacial location action */
void do_chance(Game_engine *game_engine, Player * player, wchar_t * status_string) {
    int card_number = (rand() % CHANCE_CARD_NUM);
    wcscpy(status_string, CHANCE_STR[card_number]);
    player->money += CHANCE_GAIN[card_number];
}

void do_fate(Game_engine *game_engine, Player * player, wchar_t * status_string) {
    int card_number = (rand() % FATE_CARD_NUM);
    wcscpy(status_string, FATE_STR[card_number]);
    player->money += FATE_GAIN[card_number];
}

void do_doraemon(Game_engine *game_engine, Player * player, wchar_t * status_string) {
    int door = 0;
    while(game_engine->buildings[door].is_special)
        door = rand() % BUILDING_NUM;
    player->position = door;
    wcscpy(status_string, L"哆啦A夢的任意門效果:\n移到");
    wcscat(status_string, game_engine->buildings[door].name);
}

void do_go_home(Game_engine *game_engine, Player * player, wchar_t * status_string) {
    player->position = 0;
    wcscpy(status_string, L"傳送回原點");
}

void do_traffic(Game_engine *game_engine, Player * player, wchar_t * status_string) {
    int door = 0;
    while(game_engine->buildings[door].is_special) {
        int move_step = (rand() % 6) + 1;
        wchar_t tmp[2] = {0};
        tmp[0] = move_step + 48;
        if(rand() % 2) {
            door = player->position + move_step;
            wcscpy(status_string, L"遇到紅綠燈,闖紅燈\n移動"); 
        }
        else {
            door = player->position - move_step;
            wcscpy(status_string, L"遇到紅綠燈,被喇叭嚇到\n退後"); 
        }
        wcscat(status_string, tmp);
        wcscat(status_string, L"格");
    }
    player->position = door;
}

/* player dice */
void do_dice(Game_engine *game_engine, wchar_t * status_string) {
    game_engine->currentDice = rand() % 6;
    sfMusic_setPlayingOffset(game_engine->dice_sound, sfSeconds(0));
    sfMusic_play(game_engine->dice_sound);
    sfClock_restart(game_engine->anim_timer);

    game_engine->state = WaitAnim;
    game_engine->next_state = DiceMove;
}

void do_move(Game_engine *game_engine, wchar_t * status_string) {
    game_engine->state = WaitPlayerBuy;
    game_engine->next_state = WaitPlayerBuy;
    game_engine->player[game_engine->currentPlayer].position += game_engine->currentDice + 1;
    if(game_engine->player[game_engine->currentPlayer].position >= BUILDING_NUM) {
        game_engine->player[game_engine->currentPlayer].position %= BUILDING_NUM;
    }
    
    // check if building is special
    Player * player = &game_engine->player[game_engine->currentPlayer];
    int buildType = game_engine->buildings[player->position].is_special;
    if(buildType) {
        if(buildType == 2) {
            // chance
            do_chance(game_engine, player, status_string);
        } else if (buildType == 3) {
            // fate
            do_fate(game_engine, player, status_string);
        } else if (buildType == 4) {
            // traffic light
            do_traffic(game_engine, player, status_string);
        } else if (buildType == 5) {
            // doraemon
            do_doraemon(game_engine, player, status_string);
        } else if (buildType == 6) {
            // go home
            do_go_home(game_engine, player, status_string);
        }
        game_engine->state = WaitAnim;
        game_engine->next_state = WaitPlayerBuy;
    }
    int owner = game_engine->buildings[player->position].owner;
    // check if player has to pay building fee
    if((owner != -1) && (owner != game_engine->currentPlayer) && game_engine->player[owner].gameable) {
        // give money
        int payment = game_engine->buildings[player->position].payment * game_engine->buildings[player->position].room_numer;
        player->money -= payment;
        game_engine->player[owner].money += payment;
        wcscpy(status_string, L"支付過路費");
    }

    // check if player is computer to wait
    sfClock_restart(game_engine->anim_timer);
    if(!game_engine->player[game_engine->currentPlayer].isPlayer) {
        game_engine->state = WaitAnim;        
    }

    // check if someone win or loss
    int winner = -1;
    for(int i = 0; i < PLAYER_NUM; i++) {
        if(game_engine->player[i].money < 0) {
            game_engine->player[i].gameable = 0;
        }
        if(game_engine->player[i].money >= 20000)
            winner = i;
        if(game_engine->player[i].ownedBuildingCount >= 15)
            winner = i;
    }
    int gameable = 0;
    if(winner == -1) {
        for(int i = 0; i < PLAYER_NUM; i++)
            if(game_engine->player[i].gameable) {
                gameable += 1;
                winner = i;
            }
        if(gameable > 1)
            winner = -1;
    }
    if(winner != -1) {
        game_engine->winner = winner;
        game_engine->state = GameEnd;
        sfClock_restart(game_engine->anim_timer);
        sfMusic_play(game_engine->win_sound);
    }
}

/* player buy building */
void buy_player(Game_engine *game_engine, wchar_t * status_string) {
    int position = game_engine->player[game_engine->currentPlayer].position;
    if(game_engine->buildings[position].can_buy) {
        if(game_engine->buildings[position].owner == -1) {
            if(game_engine->player[game_engine->currentPlayer].money >= game_engine->buildings[position].price) {
                game_engine->player[game_engine->currentPlayer].money -= game_engine->buildings[position].price;
                game_engine->player[game_engine->currentPlayer].ownedBuildingCount += 1;
                game_engine->buildings[position].owner = game_engine->currentPlayer;
                game_engine->buildings[position].room_numer += 1;
                sfMusic_setPlayingOffset(game_engine->buy_sound, sfSeconds(0.5));
                sfMusic_play(game_engine->buy_sound);
                wcscpy(status_string, game_engine->buildings[position].name);
                wcscat(status_string, L"\n地點購買成功");
            } else {
                wcscpy(status_string, L"\n餘額不足");
            }
        } else if(game_engine->buildings[position].owner == game_engine->currentPlayer) {
            if(game_engine->player[game_engine->currentPlayer].money >= game_engine->buildings[position].price) {
                game_engine->player[game_engine->currentPlayer].money -= game_engine->buildings[position].price;
                game_engine->player[game_engine->currentPlayer].ownedBuildingCount += 1;
                game_engine->buildings[position].room_numer += 1;
                sfMusic_setPlayingOffset(game_engine->buy_sound, sfSeconds(0.5));
                sfMusic_play(game_engine->buy_sound);
                wcscpy(status_string, game_engine->buildings[position].name);
                wcscat(status_string, L"\n地點購買成功");
            } else {
                wcscpy(status_string, L"\n餘額不足");
            }
        } else {
            wcscat(status_string, game_engine->buildings[position].name);
            wcscpy(status_string, L"\n此地點已被購買");
        }
    } else {
        wcscpy(status_string, L"此地點不能購買");
    }
    // perform buying
    game_engine->state = WaitPlayerFinish;
    game_engine->next_state = WaitPlayerFinish;
    sfClock_restart(game_engine->anim_timer);
    if(!game_engine->player[game_engine->currentPlayer].isPlayer) {
        game_engine->state = WaitAnim;
    }
}

/* game loop */
void handle_state(Game_engine *game_engine, wchar_t * status_string, screen_position *destination) {
    switch(game_engine->state) {
            case TurnPlayer:    
                wcscpy(status_string, L"");
                game_engine->currentPlayer = (game_engine->currentPlayer + 1) % PLAYER_NUM;
                while(!game_engine->player[game_engine->currentPlayer].gameable) {
                    game_engine->currentPlayer = (game_engine->currentPlayer + 1) % PLAYER_NUM;
                }
                game_engine->state = WaitPlayerDice;
                break;
            case WaitPlayerDice: //computer turn
                if (!game_engine->player[game_engine->currentPlayer].isPlayer) {
                    do_dice(game_engine, status_string);
                }
                break;
            case DiceMove: 
                do_move(game_engine, status_string);
                break;
            case WaitPlayerBuy: //computer turn
                if (!game_engine->player[game_engine->currentPlayer].isPlayer) {
                    buy_player(game_engine, status_string);
                }
                break;
            case WaitPlayerFinish: //computer turn
                if (!game_engine->player[game_engine->currentPlayer].isPlayer) {
                    game_engine->state = TurnPlayer;
                }
                break;
            case WaitAnim:
                if(sfClock_getElapsedTime(game_engine->anim_timer).microseconds >= COM_WAIT_TIME_MICRO) {
                    game_engine->state = game_engine->next_state;
                }
                break;
            case GameEnd:
                if(sfClock_getElapsedTime(game_engine->anim_timer).microseconds >= NEXT_NEW_GAME_GAP) {
                    *destination = MenuScreen;
                }
                break;
        }
}

Building* getBuildings(sfFont * font) {
    Building* buildings = malloc(BUILDING_NUM * sizeof(Building));
    createBuilding(&buildings[0], L"起點\n經過或停留起點\n，+2000", 1, 0, 0, 0, 0.8570731707317073, 0.8552631578947368, 0.13390243902439006, 0.13390243902439006);
    createBuilding(&buildings[1], L"寧靜湖", 0, 500, 100, 1, 0.7678048780487805, 0.8552631578947368, 0.0892682926829267, 0.13390243902439006);
    createBuilding(&buildings[2], L"噴水池", 0, 1000, 200, 1, 0.6785365853658537, 0.8552631578947368, 0.0892682926829267, 0.13390243902439006);
    createBuilding(&buildings[3], L"抽一張機會卡", 2, 0, 0, 0, 0.5892682926829268, 0.8552631578947368, 0.0892682926829267, 0.13390243902439006);
    createBuilding(&buildings[4], L"鳳凰大道", 0, 1500, 300, 1, 0.5, 0.8552631578947368, 0.0892682926829267, 0.13390243902439006);
    createBuilding(&buildings[5], L"禮堂", 0, 2000, 400, 1, 0.41073170731707315, 0.8552631578947368, 0.0892682926829267, 0.13390243902439006);
    createBuilding(&buildings[6], L"理學院一館", 0, 1500, 300, 1, 0.32146341463414635, 0.8552631578947368, 0.0892682926829267, 0.13390243902439006);
    createBuilding(&buildings[7], L"抽一張命運卡", 3, 0, 0, 0, 0.23219512195121952, 0.8552631578947368, 0.0892682926829267, 0.13390243902439006);
    createBuilding(&buildings[8], L"理學院二館", 0, 1000, 200, 1, 0.14292682926829267, 0.8552631578947368, 0.0892682926829267, 0.13390243902439006);
    createBuilding(&buildings[9], L"紅綠燈闖紅燈\n或被喇叭嚇到\n移動-6到+6格", 4, 0, 0, 0, 0.00975609756097561, 0.8552631578947368, 0.13390243902439006, 0.13390243902439006);
    createBuilding(&buildings[10], L"法學院", 0, 500, 100, 1, 0.00975609756097561, 0.7660818713450293, 0.13390243902439006, 0.0892682926829267);
    createBuilding(&buildings[11], L"抽一張機會卡", 2, 0, 0, 0, 0.00975609756097561, 0.6769005847953217, 0.13390243902439006, 0.0892682926829267);
    createBuilding(&buildings[12], L"管理學院", 0, 1000, 200, 1, 0.00975609756097561, 0.5877192982456141, 0.13390243902439006, 0.0892682926829267);
    createBuilding(&buildings[13], L"共同教室", 0, 1500, 300, 1, 0.00975609756097561, 0.49853801169590645, 0.13390243902439006, 0.0892682926829267);
    createBuilding(&buildings[14], L"活動中心", 0, 2000, 400, 1, 0.00975609756097561, 0.4093567251461988, 0.13390243902439006, 0.0892682926829267);
    createBuilding(&buildings[15], L"社科院", 0, 1500, 300, 1, 0.00975609756097561, 0.3201754385964912, 0.13390243902439006, 0.0892682926829267);
    createBuilding(&buildings[16], L"抽一張命運卡", 3, 0, 0, 0, 0.00975609756097561, 0.2309941520467836, 0.13390243902439006, 0.0892682926829267);
    createBuilding(&buildings[17], L"文學院", 0, 1000, 200, 1, 0.00975609756097561, 0.14181286549707603, 0.13390243902439006, 0.0892682926829267);
    createBuilding(&buildings[18], L"哆啦A夢的任意門\n傳送到任意地方", 5, 0, 0, 0, 0.00975609756097561, 0.007797270955165692, 0.13390243902439006, 0.13390243902439006);
    createBuilding(&buildings[19], L"工學院一館", 0, 1000, 200, 1, 0.14390243902439023, 0.007797270955165692, 0.0892682926829267, 0.13390243902439006);
    createBuilding(&buildings[20], L"抽一張機會卡", 2, 0, 0, 0, 0.23317073170731709, 0.007797270955165692, 0.0892682926829267, 0.13390243902439006);
    createBuilding(&buildings[21], L"工學院二館", 0, 1500, 300, 1, 0.3224390243902439, 0.007797270955165692, 0.0892682926829267, 0.13390243902439006);
    createBuilding(&buildings[22], L"體育館", 0, 2000, 400, 1, 0.4117073170731707, 0.007797270955165692, 0.0892682926829267, 0.13390243902439006);
    createBuilding(&buildings[23], L"創新大樓", 0, 1500, 300, 1, 0.5009756097560976, 0.007797270955165692, 0.0892682926829267, 0.13390243902439006);
    createBuilding(&buildings[24], L"抽一張命運卡", 3, 0, 0, 0, 0.5902439024390244, 0.007797270955165692, 0.0892682926829267, 0.13390243902439006);
    createBuilding(&buildings[25], L"實習工廠", 0, 1000, 200, 1, 0.6795121951219513, 0.007797270955165692, 0.0892682926829267, 0.13390243902439006);
    createBuilding(&buildings[26], L"圖書資訊大樓", 0, 500, 100, 1, 0.7687804878048781, 0.007797270955165692, 0.0892682926829267, 0.13390243902439006);
    createBuilding(&buildings[27], L"等待區\n傳送到原點", 6, 0, 0, 0, 0.8580487804878049, 0.007797270955165692, 0.13390243902439006, 0.13390243902439006);
    createBuilding(&buildings[28], L"學士班宿舍", 0, 500, 100, 1, 0.8580487804878049, 0.14181286549707603, 0.13390243902439006, 0.0892682926829267);
    createBuilding(&buildings[29], L"抽一張機會卡", 2, 0, 0, 0, 0.8580487804878049, 0.2309941520467836, 0.13390243902439006, 0.0892682926829267);
    createBuilding(&buildings[30], L"碩博班宿舍", 0, 1000, 200, 1, 0.8580487804878049, 0.3201754385964912, 0.13390243902439006, 0.0892682926829267);
    createBuilding(&buildings[31], L"學人宿舍", 0, 1500, 300, 1, 0.8580487804878049, 0.4093567251461988, 0.13390243902439006, 0.0892682926829267);
    createBuilding(&buildings[32], L"行政大樓", 0, 2000, 400, 1, 0.8580487804878049, 0.49853801169590645, 0.13390243902439006, 0.0892682926829267);
    createBuilding(&buildings[33], L"教育學院二館", 0, 1500, 300, 1, 0.8580487804878049, 0.5877192982456141, 0.13390243902439006, 0.0892682926829267);
    createBuilding(&buildings[34], L"抽一張命運卡", 3, 0, 0, 0, 0.8580487804878049, 0.6769005847953217, 0.13390243902439006, 0.0892682926829267);
    createBuilding(&buildings[35], L"教育學院一館", 0, 1000, 200, 1, 0.8580487804878049, 0.7660818713450293, 0.13390243902439006, 0.0892682926829267);
    return buildings;
}

/*THIS CODE IS MY OWN WORK. IT WAS WRITTEN WITHOUT CONSULTING CODE WRITTEN BY OTHER STUDENTS OR ANY ONE WHO IS NOT AWARE OF MY REFERENCE. 410125034 陳俞妏 408520059 林筠晏*/