#include <pebble.h>


#define LIFE_DEFAULT 20


static Window* window;

static TextLayer* text_layer_life_opponent;
static TextLayer* text_layer_life_player;


static int life_opponent = LIFE_DEFAULT;
static int life_player = LIFE_DEFAULT;


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


static void select_click_handler(ClickRecognizerRef recognizer, void* context) {
  //TODO
}

static void up_click_handler(ClickRecognizerRef recognizer, void* context) {
  //TODO
}

static void down_click_handler(ClickRecognizerRef recognizer, void* context) {
  //TODO
}

static void click_config_provider(void* context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window* window) {
  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer_life_opponent = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 60 } });
  text_layer_set_text_alignment(text_layer_life_opponent, GTextAlignmentCenter);
  text_layer_set_font(text_layer_life_opponent, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  update_opponent_life_counter();
  layer_add_child(window_layer, text_layer_get_layer(text_layer_life_opponent));

  text_layer_life_player = text_layer_create((GRect) { .origin = { 0, 80 }, .size = { bounds.size.w, 60 } });
  text_layer_set_text_alignment(text_layer_life_player, GTextAlignmentCenter);
  text_layer_set_font(text_layer_life_player, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  update_player_life_counter();
  layer_add_child(window_layer, text_layer_get_layer(text_layer_life_player));
}

static void window_unload(Window* window) {
  text_layer_destroy(text_layer_life_opponent);
  text_layer_destroy(text_layer_life_player);
}

static void init(void) {
  window = window_create();

  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  window_stack_push(window, true /*animated*/);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
