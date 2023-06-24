#ifndef MONO_OBJECT_H
#define MONO_OBJECT_H

typedef struct Player Player;

typedef struct Building {
    wchar_t *name;  // 土地之名字
    int is_special;
    wchar_t *info;
    int price;  // 土地之價錢
    int payment;  // 土地之過路費
    int room_numer;
    int can_buy;
    float x, y;
    float width, height;
    int owner;  // 土地的擁有者(初始為沒有) #class傳入
} Building;


typedef struct Player {
    sfText *name, *info_text;
    int isPlayer;
    int position;
    int money;
    int movable;
    int ownedBuildingCount;
    Building ownedBuildings[100];
    int gameable;
} Player;

void createBuilding(Building *building, wchar_t *name, int is_special, int price, int payment, int can_buy, float x, float y, float width, float height);
void getBuildingIntro(Building *building, wchar_t * intro_text, Player * player_list);
void createPlayer(Player * player, sfText *name, sfText *info_text, int isPlayer);


#endif 