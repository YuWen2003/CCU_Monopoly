#ifndef MONO_WIDGETS_H
#define MONO_WIDGETS_H

#include <SFML/Graphics.h>

typedef struct TextureHolder {
    int recycle_counter;
    const char * resource;
    sfTexture* texture;
} TextureHolder;

typedef struct SpriteHolder {
    sfSprite* sprite;
    TextureHolder * texture_holder;
} SpriteHolder;


TextureHolder * create_texture_holder(const char * resource);
void texture_holder_destroy(TextureHolder * texture_holder);

SpriteHolder * create_sprite_holder(const char * resource);
void sprite_holder_destroy(SpriteHolder * spirie_holder);

sfText* createText(const char * string, int size, sfColor color, sfFont* font);
sfText* createWideText(wchar_t * string, int size, sfColor color, sfFont* font);
sfRectangleShape* createTextBox(int width, int height, sfColor border_color, sfColor fill_color);
void setWideText(sfText* text, wchar_t * string);
void putText(sfText* text, float x, float y);
void putRectangleShapeBox(sfRectangleShape* textbox, float x, float y);
void putSprite(SpriteHolder* sprite_holder, float x, float y);
int isMouseInsideText(sfText *text, sfVector2i mouse_position);
#endif 