#include <stdlib.h>
#include <wchar.h>
#include "widgets.h"
/* holder creator */
TextureHolder * create_texture_holder(const char * resource) {
    sfTexture* texture;
    texture = sfTexture_createFromFile(resource, NULL);
    if (!texture)
        return 0;    
    TextureHolder * texture_holder;
    texture_holder = malloc(sizeof(TextureHolder));
    texture_holder->texture = texture;
    texture_holder->resource = resource;
    texture_holder->recycle_counter = 1;
    return texture_holder; 
}

SpriteHolder * create_sprite_holder(const char * resource) {
    TextureHolder * texture_holder = create_texture_holder(resource);
    if(!texture_holder)
        return 0;
    sfSprite* sprite;
    sprite = sfSprite_create();
    sfSprite_setTexture(sprite, texture_holder->texture, sfTrue);

    SpriteHolder * spirie_holder;
    spirie_holder = malloc(sizeof(SpriteHolder));
    spirie_holder->sprite = sprite;
    spirie_holder->texture_holder = texture_holder;
    return spirie_holder;
}

/* holder destructor */
void texture_holder_destroy(TextureHolder * texture_holder) {
    if(!texture_holder)
        return;
    if(texture_holder->recycle_counter != 1)
        return;
    texture_holder->recycle_counter = 0;
    sfTexture_destroy(texture_holder->texture);
    free(texture_holder);
}

void sprite_holder_destroy(SpriteHolder * spirie_holder) {
    if(!spirie_holder)
        return;
    texture_holder_destroy(spirie_holder->texture_holder);
    sfSprite_destroy(spirie_holder->sprite);
    free(spirie_holder);
}

/* widget creator */
sfText* createText(const char * string, int size, sfColor color, sfFont* font) {
    sfText* text = sfText_create();
    sfText_setString(text, string);
    sfText_setFont(text, font);
    sfText_setCharacterSize(text, size);
    sfText_setFillColor(text, color);
    return text;
};

sfText* createWideText(wchar_t * string, int size, sfColor color, sfFont* font) {
    sfText* text = sfText_create();
    sfText_setFont(text, font);
    setWideText(text, string);
    sfText_setCharacterSize(text, size);
    sfText_setFillColor(text, color);
    return text;
};

sfRectangleShape* createTextBox(int width, int height, sfColor border_color, sfColor fill_color) {
    sfRectangleShape* textBox = sfRectangleShape_create();
    sfRectangleShape_setSize(textBox, (sfVector2f){width, height});
    sfRectangleShape_setOutlineThickness(textBox, 2);
    sfRectangleShape_setOutlineColor(textBox, border_color);
    sfRectangleShape_setFillColor(textBox, fill_color);
    return textBox;
}

/* widget config */
void setWideText(sfText* text, wchar_t * string) {
    size_t length = wcslen(string);
    sfUint32* unicodeString = (sfUint32*)malloc((length + 1) * sizeof(sfUint32));
    for (size_t i = 0; i < length; i++)
        unicodeString[i] = (sfUint32)string[i];
    unicodeString[length] = '\0';
    sfText_setUnicodeString(text, unicodeString);
    free(unicodeString);
}

void putText(sfText* text, float x, float y) {
    sfFloatRect textRect = sfText_getLocalBounds(text);
    sfVector2f origin = {textRect.left + textRect.width/2.0f,
               textRect.top  + textRect.height/2.0f};
    sfText_setOrigin(text, origin);
    sfVector2f position = {x, y};
    sfText_setPosition(text, position);
}

void putSprite(SpriteHolder* sprite_holder, float x, float y) {
    sfSprite* sprite = sprite_holder->sprite;
    sfFloatRect rect = sfSprite_getLocalBounds(sprite);
    sfVector2f origin = {rect.left + rect.width/2.0f,
               rect.top  + rect.height/2.0f};
    sfSprite_setOrigin(sprite, origin);
    sfVector2f position = {x, y};
    sfSprite_setPosition(sprite, position);
}

void putRectangleShapeBox(sfRectangleShape* textbox, float x, float y) {
    sfFloatRect rect = sfRectangleShape_getLocalBounds(textbox);
    sfVector2f origin = {rect.left + rect.width/2.0f,
               rect.top  + rect.height/2.0f};
    sfRectangleShape_setOrigin(textbox, origin);
    sfRectangleShape_setPosition(textbox, (sfVector2f){x, y});
}

/* func */
int isMouseInsideText(sfText *text, sfVector2i mouse_position) {
    sfFloatRect bounds = sfText_getGlobalBounds(text);
    return sfFloatRect_contains(&bounds, mouse_position.x, mouse_position.y);
}

/*THIS CODE IS MY OWN WORK. IT WAS WRITTEN WITHOUT CONSULTING CODE WRITTEN BY OTHER STUDENTS OR ANY ONE WHO IS NOT AWARE OF MY REFERENCE. 410125034 ³¯«\Ê¹ 408520059 ªL¸a®Ë*/