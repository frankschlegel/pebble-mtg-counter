#include <pebble.h>

typedef struct MTGCounterMenuSelectionCallbacks
{
  SimpleMenuLayerSelectCallback game_reset;
} MTGCounterMenuSelectionCallbacks;

void set_menu_callbacks(MTGCounterMenuSelectionCallbacks selection_callbacks);
void show_menu();
void hide_menu();
void destroy_menu();
