#include "menu.h"

#define NUM_MENU_SECTIONS 1


enum MTGCounterMenuGameSectionItems {
  MTGCounterMenuGameSectionItemReset,

  MTGCounterMenuGameSectionItemsCount
};

enum MTGCounterMenuSections {
  MTGCounterMenuSectionGame,

  MTGCounterMenuSectionsCount
};

static SimpleMenuLayer* menu_layer = NULL;

static MTGCounterMenuSelectionCallbacks selection_callbacks;

static SimpleMenuSection menu_sections[MTGCounterMenuSectionsCount];
static SimpleMenuItem game_section_items[MTGCounterMenuGameSectionItemsCount];


static void init_game_section_items(void) {
  game_section_items[MTGCounterMenuGameSectionItemReset] = (SimpleMenuItem){
    .title = "Reset",
    .callback= selection_callbacks.game_reset,
  };
}


static void init_sections(void) {
  init_game_section_items();

  menu_sections[MTGCounterMenuSectionGame] = (SimpleMenuSection){
    .num_items = MTGCounterMenuGameSectionItemsCount,
    .items = game_section_items,
  };
}


SimpleMenuLayer* init_menu(GRect bounds, Window* window, MTGCounterMenuSelectionCallbacks callbacks) {
  // check if already initialized
  if (menu_layer == NULL) {
    selection_callbacks = callbacks;
    init_sections();

    menu_layer = simple_menu_layer_create(bounds, window, menu_sections, MTGCounterMenuSectionsCount, NULL);
  }

  return menu_layer;
}

void destroy_menu(void) {
  if (menu_layer != NULL) simple_menu_layer_destroy(menu_layer);
  menu_layer = NULL;
}
