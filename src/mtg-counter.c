#include <pebble.h>
#include "menu.h"


#define LIFE_DEFAULT 20
#define LIFE_STEP_DEFAULT -1
#define REPEATING_CLICK_INTERVAL 500


enum PKEY {
  LIFE_OPPONENT_PKEY,
  LIFE_PLAYER_PKEY,
  LIFE_STEP_PKEY,
  MATCH_START_TIME_PKEY,
};


static Window* main_window;
static Window* menu_window;

static TextLayer* text_layer_life_opponent;
static TextLayer* text_layer_life_player;
static TextLayer* text_layer_match_timer;

static ActionBarLayer* action_bar_layer;
static GBitmap* action_icon_plus;
static GBitmap* action_icon_minus;
static GBitmap* action_icon_toggle;


static int life_opponent = LIFE_DEFAULT;
static int life_player = LIFE_DEFAULT;
static short life_step = LIFE_STEP_DEFAULT;


static time_t match_start_time;


static void read_state() {
  life_opponent = persist_exists(LIFE_OPPONENT_PKEY) ? persist_read_int(LIFE_OPPONENT_PKEY) : LIFE_DEFAULT;
  life_player   = persist_exists(LIFE_PLAYER_PKEY)   ? persist_read_int(LIFE_PLAYER_PKEY)   : LIFE_DEFAULT;
  life_step     = persist_exists(LIFE_STEP_PKEY)     ? persist_read_int(LIFE_STEP_PKEY)     : LIFE_STEP_DEFAULT;

  match_start_time = persist_exists(MATCH_START_TIME_PKEY) ? persist_read_int(MATCH_START_TIME_PKEY) : time(NULL);
}

static void safe_state() {
  persist_write_int(LIFE_OPPONENT_PKEY, life_opponent);
  persist_write_int(LIFE_PLAYER_PKEY, life_player);
  persist_write_int(LIFE_STEP_PKEY, life_step);
  persist_write_int(MATCH_START_TIME_PKEY, match_start_time);
}

static void update_opponent_life_counter() {
  static char text[5];
  snprintf(text, sizeof(text), "%d", life_opponent);
  text_layer_set_text(text_layer_life_opponent, text);
}

static void update_player_life_counter() {
  static char text[5];
  snprintf(text, sizeof(text), "%d", life_player);
  text_layer_set_text(text_layer_life_player, text);
}

static void update_action_bar() {
  GBitmap* icon = life_step < 0 ? action_icon_minus : action_icon_plus;

  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_UP, icon);
  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_DOWN, icon);
}

static void update_match_timer() {
  time_t diff = time(NULL) - match_start_time;

  static char text[9];
  strftime(text, 9, "%H:%M:%S", localtime(&diff));
  text_layer_set_text(text_layer_match_timer, text);
}


static void select_click_handler(ClickRecognizerRef recognizer, void* context) {
  // toggle step direction
  life_step *= -1;
  update_action_bar();
}

static void select_long_click_handler(ClickRecognizerRef recognizer, void* context) {
  // show menu
  window_stack_push(menu_window, true);
}

static void up_repeating_click_handler(ClickRecognizerRef recognizer, void* context) {
  //TODO: cap
  life_opponent += life_step;
  update_opponent_life_counter();
}

static void down_repeating_click_handler(ClickRecognizerRef recognizer, void* context) {
  //TODO: cap
  life_player += life_step;
  update_player_life_counter();
}

static void click_config_provider(void* context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 0, select_long_click_handler, NULL);
  window_single_repeating_click_subscribe(BUTTON_ID_UP,   REPEATING_CLICK_INTERVAL, up_repeating_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, REPEATING_CLICK_INTERVAL, down_repeating_click_handler);
}


static void second_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_match_timer();
}


static void menu_select_game_reset_callback(int index, void *ctx) {
  life_opponent = LIFE_DEFAULT;
  life_player = LIFE_DEFAULT;
  match_start_time = time(NULL);
  update_opponent_life_counter();
  update_player_life_counter();
  update_match_timer();
  // hide menu
  window_stack_pop(true);
}


static void main_window_load(Window* window) {
  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // initialize the action bar
  action_bar_layer = action_bar_layer_create();
  // load the icons
  action_icon_plus = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_PLUS);
  action_icon_minus = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_MINUS);
  action_icon_toggle = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_TOGGLE);
  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_SELECT, action_icon_toggle);
  update_action_bar();
  // associate the action bar with the window
  action_bar_layer_add_to_window(action_bar_layer, window);
  // set the click config provider
  action_bar_layer_set_click_config_provider(action_bar_layer, click_config_provider);

  text_layer_life_opponent = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w - ACTION_BAR_WIDTH, 60 } });
  text_layer_set_text_alignment(text_layer_life_opponent, GTextAlignmentCenter);
  text_layer_set_font(text_layer_life_opponent, fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS));
  update_opponent_life_counter();
  layer_add_child(window_layer, text_layer_get_layer(text_layer_life_opponent));

  text_layer_life_player = text_layer_create((GRect) { .origin = { 0, 80 }, .size = { bounds.size.w - ACTION_BAR_WIDTH, 60 } });
  text_layer_set_text_alignment(text_layer_life_player, GTextAlignmentCenter);
  text_layer_set_font(text_layer_life_player, fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS));
  update_player_life_counter();
  layer_add_child(window_layer, text_layer_get_layer(text_layer_life_player));

  text_layer_match_timer = text_layer_create((GRect) { .origin = { 0, 50 }, .size = { bounds.size.w - ACTION_BAR_WIDTH, 40 } });
  text_layer_set_text_alignment(text_layer_match_timer, GTextAlignmentCenter);
  text_layer_set_font(text_layer_match_timer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  update_match_timer();
  layer_add_child(window_layer, text_layer_get_layer(text_layer_match_timer));
}

static void main_window_unload(Window* window) {
  text_layer_destroy(text_layer_life_opponent);
  text_layer_destroy(text_layer_life_player);
  text_layer_destroy(text_layer_match_timer);
  action_bar_layer_destroy(action_bar_layer);
  // destory the menu when the main window onloads only
  destroy_menu();
  gbitmap_destroy(action_icon_plus);
  gbitmap_destroy(action_icon_minus);
  gbitmap_destroy(action_icon_toggle);
}

static void menu_window_load(Window* window) {
  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  SimpleMenuLayer* menu_layer = init_menu(bounds, window, (MTGCounterMenuSelectionCallbacks) {
    .game_reset = menu_select_game_reset_callback,
  });

  layer_add_child(window_layer, simple_menu_layer_get_layer(menu_layer));
}

static void init(void) {
  read_state();

  main_window = window_create();
  // window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });

  menu_window = window_create();
  window_set_window_handlers(menu_window, (WindowHandlers) {
    .load = menu_window_load,
  });

  window_stack_push(main_window, true /*animated*/);

  // register callback for match timer
  tick_timer_service_subscribe(SECOND_UNIT, second_tick_handler);
}

static void deinit(void) {
  safe_state();

  window_destroy(main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
