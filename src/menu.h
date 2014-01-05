#include <pebble.h>

typedef struct MTGCounterMenuSelectionCallbacks
{
  SimpleMenuLayerSelectCallback game_reset;
} MTGCounterMenuSelectionCallbacks;

SimpleMenuLayer* init_menu(GRect bounds, Window* window, MTGCounterMenuSelectionCallbacks selection_callbacks);
void destroy_menu(void);
