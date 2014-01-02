#include <pebble.h>


#define LIFE_DEFAULT 20
#define LIFE_STEP_DEFAULT -1
#define REPEATING_CLICK_INTERVAL 500


static Window* window;

static TextLayer* text_layer_life_opponent;
static TextLayer* text_layer_life_player;

static ActionBarLayer* action_bar_layer;
static GBitmap* action_icon_plus;
static GBitmap* action_icon_minus;


static int life_opponent = LIFE_DEFAULT;
static int life_player = LIFE_DEFAULT;

static short life_step = LIFE_STEP_DEFAULT;


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


static void select_click_handler(ClickRecognizerRef recognizer, void* context) {
  // toggle step direction
  life_step *= -1;
  update_action_bar();
}

static void up_repeating_click_handler(ClickRecognizerRef recognizer, void* context) {
  life_opponent += life_step;
  update_opponent_life_counter();
}

static void down_repeating_click_handler(ClickRecognizerRef recognizer, void* context) {
  life_player += life_step;
  update_player_life_counter();
}

static void click_config_provider(void* context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_UP,   REPEATING_CLICK_INTERVAL, up_repeating_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, REPEATING_CLICK_INTERVAL, down_repeating_click_handler);
}

static void window_load(Window* window) {
  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // initialize the action bar
  action_bar_layer = action_bar_layer_create();
  // load the icons
  action_icon_plus = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_PLUS);
  action_icon_minus = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_MINUS);
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
}

static void window_unload(Window* window) {
  text_layer_destroy(text_layer_life_opponent);
  text_layer_destroy(text_layer_life_player);
}

static void init(void) {
  window = window_create();

  // window_set_click_config_provider(window, click_config_provider);
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
