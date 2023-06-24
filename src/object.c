#include <SFML/Graphics.h>
#include <string.h>
#include <wchar.h>
#include "object.h"
#include "widgets.h"
/* building */
void createBuilding(Building *building, wchar_t *name, int is_special, int price, int payment, int can_buy, float x, float y, float width, float height) {
    building->name = name;
    building->is_special = is_special;
    building->price = price;
    building->payment = payment;
    building->room_numer = 0;
    building->can_buy = can_buy;
    building->x = x;
    building->y = y;
    building->height = height;
    building->width = width;
    building->owner = -1;
}

void appendInt(wchar_t * text, int i) {
    char inverse[20] = {0};
    int inverse_len = 0;
    size_t start = wcslen(text);
    if(i == 0) {
        inverse[inverse_len++] = i+48;
    } else {
        while(i > 0) {
            inverse[inverse_len++] = (i % 10) + 48;
            i /= 10;
        }
    }
    while(inverse_len--)
        text[start++] = inverse[inverse_len];
    text[start++] = '\0';
}

void getBuildingIntro(Building *building, wchar_t * intro_text, Player * player_list) {
    if(building->is_special) {
        wcscpy(intro_text, building->name);
    } else {
        wcscpy(intro_text, building->name);
        wcscat(intro_text, L"\n擁有玩家: ");
        if(building->owner == -1)
            wcscat(intro_text, L"無");
        else {
            const sfUint32* user_name = sfText_getUnicodeString(player_list[building->owner].name);
            size_t start = wcslen(intro_text);
            size_t i = 0;
            while(user_name[i])
                intro_text[start++] = (wchar_t)user_name[i++];
            intro_text[start++] = '\0';
        }
        wcscat(intro_text, L"\n租金: ");
        appendInt(intro_text, building->payment);
        wcscat(intro_text, L"\n價格: ");
        appendInt(intro_text, building->price);
        wcscat(intro_text, L"\n有幾間房: ");
        appendInt(intro_text, building->room_numer);
    }
}

/* player */
void createPlayer(Player * player, sfText *name, sfText *info_text, int isPlayer) {
    player->name = name;
    player->isPlayer = isPlayer;
    player->position = 0;
    player->money = 10000;
    player->movable = 1;
    player->ownedBuildingCount = 0;
    player->gameable = 1;
    player->info_text = info_text;
}

/*THIS CODE IS MY OWN WORK. IT WAS WRITTEN WITHOUT CONSULTING CODE WRITTEN BY OTHER STUDENTS OR ANY ONE WHO IS NOT AWARE OF MY REFERENCE. 410125034 陳俞妏 408520059 林筠晏*/