#include "menu.h"

#define NUM_MENU_SECTIONS 1


enum MTGCounterMenuGameSectionItems {
  MTGCounterMenuGameSectionItemPlayerWins,
  MTGCounterMenuGameSectionItemOpponentWins,
  MTGCounterMenuGameSectionItemDraw,
  MTGCounterMenuGameSectionItemReset,

  MTGCounterMenuGameSectionItemsCount
};

enum MTGCounterMenuSections {
  MTGCounterMenuSectionGame,

  MTGCounterMenuSectionsCount
};

static Window* window;

static SimpleMenuLayer* menu_layer = NULL;

static MTGCounterMenuSelectionCallbacks selection_callbacks;

static SimpleMenuSection menu_sections[MTGCounterMenuSectionsCount];
static SimpleMenuItem game_section_items[MTGCounterMenuGameSectionItemsCount];


static void init_game_section_items() {
  game_section_items[MTGCounterMenuGameSectionItemPlayerWins] = (SimpleMenuItem){
    .title = "I won",
    .icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_WIN),
    .callback= selection_callbacks.player_wins,
  };
  game_section_items[MTGCounterMenuGameSectionItemOpponentWins] = (SimpleMenuItem){
    .title = "I lost",
    .icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_LOSE),
    .callback= selection_callbacks.opponent_wins,
  };
  game_section_items[MTGCounterMenuGameSectionItemDraw] = (SimpleMenuItem){
    .title = "Draw game",
    .icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_DRAW),
    .callback= selection_callbacks.draw,
  };
  game_section_items[MTGCounterMenuGameSectionItemReset] = (SimpleMenuItem){
    .title = "Next match",
    .subtitle = "Resets everything",
    .icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_RESET),
    .callback= selection_callbacks.game_reset,
  };
}

static void init_sections() {
  init_game_section_items();

  menu_sections[MTGCounterMenuSectionGame] = (SimpleMenuSection){
    .num_items = MTGCounterMenuGameSectionItemsCount,
    .items = game_section_items,
  };
}


static void window_load(Window* window) {
  init_sections();

  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  menu_layer = simple_menu_layer_create(bounds, window, menu_sections, MTGCounterMenuSectionsCount, NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(menu_layer));
}

static void window_unload(Window* window) {
  simple_menu_layer_destroy(menu_layer);
}


void set_menu_callbacks(MTGCounterMenuSelectionCallbacks callbacks) {
  selection_callbacks = callbacks;
}

void show_menu() {
  if (window == NULL) {
    window = window_create();
    window_set_window_handlers(window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
    });
  }

  window_stack_push(window, true);
}

void hide_menu() {
  if (window_stack_get_top_window() == window) {
    window_stack_pop(true);
  }
}

void destroy_menu() {
  if (window != NULL) window_destroy(window);
}
