#include <pebble.h>

#include "state.h"
#include "config.h"
#include "persistence.h"
#include "appmessage.h"

#include "decision_screen.h"
#include "menu.h"
#include "score_layer.h"


#define REPEATING_CLICK_INTERVAL 500
#define ORIENTATION_CHECK_TIMER_UPDATE_INTERVAL 500
#define Y_THRESHOLD_ORIENTATION_UPSIDE_DOWN 600
#define Y_THRESHOLD_ORIENTATION_NORMAL 500

// timer
static AppTimer* timer_orientation_check;

// UI
static Window* main_window;

static Layer* background_layer;
static Layer* action_bar_background_layer;
static Layer* game_score_opponent_background_layer;
static Layer* game_score_player_background_layer;
static Layer* game_score_draw_background_layer;

static TextLayer* text_layer_life_opponent;
static TextLayer* text_layer_life_player;
static TextLayer* text_layer_games_won_opponent;
static TextLayer* text_layer_games_won_player;
static TextLayer* text_layer_games_draw;
static TextLayer* text_layer_match_timer;

static ScoreLayer* score_layer_life_opponent;
static ScoreLayer* score_layer_life_player;

static ActionBarLayer* action_bar_layer;
static GBitmap* action_icon_plus;
static GBitmap* action_icon_minus;
static GBitmap* action_icon_toggle;


void update_opponent_life_counter() {
  score_layer_set_score(score_layer_life_opponent, life_opponent);
}
void update_player_life_counter() {
  score_layer_set_score(score_layer_life_player, life_player);
}


void update_games_won_counter_opponent() {
  static char text[3];
  snprintf(text, sizeof(text), "%d", games_won_opponent);
  text_layer_set_text(text_layer_games_won_opponent, text);
}
void update_games_won_counter_player() {
  static char text[3];
  snprintf(text, sizeof(text), "%d", games_won_player);
  text_layer_set_text(text_layer_games_won_player, text);
}
void update_games_draw_counter() {
  static char text[3];
  snprintf(text, sizeof(text), "%d", games_draw);
  text_layer_set_text(text_layer_games_draw, text);
}

void update_action_bar() {
  GBitmap* icon = life_step < 0 ? action_icon_minus : action_icon_plus;

  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_UP, icon);
  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_DOWN, icon);
}

void update_match_timer() {
  time_t diff = time(NULL) - match_start_time;

  static char text[9];
  strftime(text, sizeof(text), "%H:%M:%S", localtime(&diff));
  text_layer_set_text(text_layer_match_timer, text);
}

static void opponent_wins() {
  // cap
  games_won_opponent = (games_won_opponent == GAMES_SCORE_MAX) ? GAMES_SCORE_MAX : games_won_opponent + 1;
  update_games_won_counter_opponent();
  reset_game_state();
}
static void player_wins() {
  // cap
  games_won_player = (games_won_player == GAMES_SCORE_MAX) ? GAMES_SCORE_MAX : games_won_player + 1;
  update_games_won_counter_player();
  reset_game_state();
}
static void draw() {
  // cap
  games_draw = (games_draw == GAMES_SCORE_MAX) ? GAMES_SCORE_MAX : games_draw + 1;
  update_games_draw_counter();
  reset_game_state();
}

static void continue_game_on_purpose() {
  game_continues_on_purpose = true;
}


static void select_click_handler(ClickRecognizerRef recognizer, void* context) {
  // toggle step direction
  life_step *= -1;
  update_action_bar();
}
static void select_long_click_handler(ClickRecognizerRef recognizer, void* context) {
  show_menu();
}
static void up_repeating_click_handler(ClickRecognizerRef recognizer, void* context) {
  life_opponent += life_step;
  // cap
  if (life_opponent > LIFE_MAX) life_opponent = LIFE_MAX;
  if (life_opponent < LIFE_MIN) life_opponent = LIFE_MIN;

  update_opponent_life_counter();
  if (life_opponent == 0 && !game_continues_on_purpose)
  {
    show_decision_screen("You win, congratulation! Start the next game?", player_wins, continue_game_on_purpose);
  }
}
static void down_repeating_click_handler(ClickRecognizerRef recognizer, void* context) {
  life_player += life_step;
  // cap
  if (life_player > LIFE_MAX) life_player = LIFE_MAX;
  if (life_player < LIFE_MIN) life_player = LIFE_MIN;

  update_player_life_counter();
  if (life_player == 0 && !game_continues_on_purpose)
  {
    show_decision_screen("Your opponent wins. Start the next game?", opponent_wins, continue_game_on_purpose);
  }
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


static void check_orientation(void *data) {
  AccelData accel_data = (AccelData) { .x = 0, .y = 0, .z = 0 };
  accel_service_peek(&accel_data);

  ScoreLayerOrientation current_orientation = score_layer_get_orientation(score_layer_life_opponent);

  if (current_orientation == ScoreLayerOrientationNormal && accel_data.y > Y_THRESHOLD_ORIENTATION_UPSIDE_DOWN) {
    score_layer_set_orientation(score_layer_life_opponent, ScoreLayerOrientationUpsideDown);
    score_layer_set_orientation(score_layer_life_player, ScoreLayerOrientationUpsideDown);
  } else if (current_orientation == ScoreLayerOrientationUpsideDown && accel_data.y < Y_THRESHOLD_ORIENTATION_NORMAL) {
    score_layer_set_orientation(score_layer_life_opponent, ScoreLayerOrientationNormal);
    score_layer_set_orientation(score_layer_life_player, ScoreLayerOrientationNormal);
  }

  // restart timer
  timer_orientation_check = app_timer_register(ORIENTATION_CHECK_TIMER_UPDATE_INTERVAL, check_orientation, NULL);
}


static void menu_select_game_reset_callback(int index, void *ctx) {
  reset_match_state();
  hide_menu();
}
static void menu_select_opponent_wins_callback(int index, void *ctx) {
  opponent_wins();
  hide_menu();
}
static void menu_select_player_wins_callback(int index, void *ctx) {
  player_wins();
  hide_menu();
}
static void menu_select_draw_callback(int index, void *ctx) {
  draw();
  hide_menu();
}


static void background_layer_update(Layer* me, GContext* ctx) {
  GRect frame = layer_get_frame(me);
  frame.origin = (GPoint){ .x = 0, .y = 0 };

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, frame, 0, GCornerNone);
}

static void game_score_background_layer_update(Layer* me, GContext* ctx) {
  GRect frame = layer_get_frame(me);
  frame.origin = (GPoint){ .x = 0, .y = 0 };

  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_round_rect(ctx, frame, 3);
}

static void action_bar_background_layer_update(Layer* me, GContext* ctx) {
  GRect frame = layer_get_frame(me);
  frame.origin = (GPoint){ .x = 0, .y = 0 };

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, frame, 4, GCornersLeft);
}


static void main_window_load(Window* window) {
  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  int center_x = bounds.size.w / 2;

  // init background
  background_layer = layer_create(bounds);
  layer_set_update_proc(background_layer, background_layer_update);
  layer_add_child(window_layer, background_layer);

  // load the action bar icons
  // the action bar itself is created on appear
  action_icon_plus = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_PLUS);
  action_icon_minus = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_MINUS);
  action_icon_toggle = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_TOGGLE);

  // init action bar background layer
  action_bar_background_layer = layer_create((GRect) {
    .origin = { bounds.size.w - ACTION_BAR_WIDTH - 1, 2 },
    .size = { ACTION_BAR_WIDTH + 1, bounds.size.h - 4 },
  });
  layer_set_update_proc(action_bar_background_layer, action_bar_background_layer_update);
  layer_add_child(window_layer, action_bar_background_layer);

  // init life score layers
  int score_width = 80;
  GRect score_layer_life_opponent_frame = (GRect) {
    .origin = { center_x - score_width / 2, 8 }, 
    .size = { score_width, 44 }
  };
  score_layer_life_opponent = score_layer_create(score_layer_life_opponent_frame , 3);
  score_layer_set_corner_mask(score_layer_life_opponent, GCornersRight | GCornerTopLeft);
  update_opponent_life_counter();
  layer_add_child(window_layer, score_layer_get_layer(score_layer_life_opponent));

  GRect score_layer_life_player_frame = score_layer_life_opponent_frame;
  score_layer_life_player_frame.origin.y = 101;
  score_layer_life_player = score_layer_create(score_layer_life_player_frame, 3);
  score_layer_set_corner_mask(score_layer_life_player, GCornersRight | GCornerBottomLeft);
  update_player_life_counter();
  layer_add_child(window_layer, score_layer_get_layer(score_layer_life_player));

  // init timer layer
  int timer_width = 80;
  text_layer_match_timer = text_layer_create((GRect) { .origin = {center_x - timer_width / 2, 59}, .size = { timer_width, 24 } });
  text_layer_set_text_alignment(text_layer_match_timer, GTextAlignmentCenter);
  text_layer_set_font(text_layer_match_timer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_background_color(text_layer_match_timer, GColorBlack);
  text_layer_set_text_color(text_layer_match_timer, GColorWhite);
  update_match_timer();
  layer_add_child(window_layer, text_layer_get_layer(text_layer_match_timer));

  // init game score layers
  GRect game_score_opponent_frame = (GRect) {
    .origin = {
      score_layer_life_opponent_frame.origin.x - 20, 
      score_layer_life_opponent_frame.origin.y + score_layer_life_opponent_frame.size.h - 26
    },
    .size = {23, 26},
  };
  game_score_opponent_background_layer = layer_create(game_score_opponent_frame);
  layer_set_update_proc(game_score_opponent_background_layer, game_score_background_layer_update);
  layer_add_child(window_layer, game_score_opponent_background_layer);

  text_layer_games_won_opponent = text_layer_create((GRect) { .origin = {2, 1}, .size = {16, 18} });
  text_layer_set_text_alignment(text_layer_games_won_opponent, GTextAlignmentCenter);
  text_layer_set_font(text_layer_games_won_opponent, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_background_color(text_layer_games_won_opponent, GColorBlack);
  text_layer_set_text_color(text_layer_games_won_opponent, GColorWhite);
  update_games_won_counter_opponent();
  layer_add_child(game_score_opponent_background_layer, text_layer_get_layer(text_layer_games_won_opponent));

  GRect game_score_player_frame = game_score_opponent_frame;
  game_score_player_frame.origin.y = score_layer_life_player_frame.origin.y;

  game_score_player_background_layer = layer_create(game_score_player_frame);
  layer_set_update_proc(game_score_player_background_layer, game_score_background_layer_update);
  layer_add_child(window_layer, game_score_player_background_layer);

  text_layer_games_won_player = text_layer_create((GRect) { .origin = {2, 1}, .size = {16, 18} });
  text_layer_set_text_alignment(text_layer_games_won_player, GTextAlignmentCenter);
  text_layer_set_font(text_layer_games_won_player, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_background_color(text_layer_games_won_player, GColorBlack);
  text_layer_set_text_color(text_layer_games_won_player, GColorWhite);
  update_games_won_counter_player();
  layer_add_child(game_score_player_background_layer, text_layer_get_layer(text_layer_games_won_player));

  GRect game_score_draw_frame = game_score_opponent_frame;
  game_score_draw_frame.origin.y = game_score_player_frame.origin.y - (game_score_player_frame.origin.y - game_score_opponent_frame.origin.y) / 2;
  game_score_draw_frame.size.w = 20;

  game_score_draw_background_layer = layer_create(game_score_draw_frame);
  layer_set_update_proc(game_score_draw_background_layer, game_score_background_layer_update);
  layer_add_child(window_layer, game_score_draw_background_layer);

  text_layer_games_draw = text_layer_create((GRect) { .origin = {2, 1}, .size = {16, 18} });
  text_layer_set_text_alignment(text_layer_games_draw, GTextAlignmentCenter);
  text_layer_set_font(text_layer_games_draw, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_background_color(text_layer_games_draw, GColorBlack);
  text_layer_set_text_color(text_layer_games_draw, GColorWhite);
  update_games_draw_counter();
  layer_add_child(game_score_draw_background_layer, text_layer_get_layer(text_layer_games_draw));
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

static void main_window_appear(Window* window) {
  //XXX: This is a hack to prevent the action bar from remaining in the highlighted state
  //     after pushing a new window to the stack.
  //     http://forums.getpebble.com/discussion/8604

  // destroy the old action bar
  if (action_bar_layer != NULL) {
    action_bar_layer_remove_from_window(action_bar_layer);
    action_bar_layer_destroy(action_bar_layer);
  }

  // initialize the action bar
  action_bar_layer = action_bar_layer_create();
  action_bar_layer_set_background_color(action_bar_layer, GColorBlack);
  // set the icons
  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_SELECT, action_icon_toggle);
  update_action_bar();
  // associate the action bar with the window
  action_bar_layer_add_to_window(action_bar_layer, window);
  // set the click config provider
  action_bar_layer_set_click_config_provider(action_bar_layer, click_config_provider);
}


static void init(void) {
  // restore persistent state
  read_persistent_state();

  // register menu callbacks
  set_menu_callbacks((MTGCounterMenuSelectionCallbacks) {
    .game_reset = menu_select_game_reset_callback,
    .opponent_wins = menu_select_opponent_wins_callback,
    .player_wins = menu_select_player_wins_callback,
    .draw = menu_select_draw_callback,
  });

  // init main window
  main_window = window_create();
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
    .appear = main_window_appear,
  });
  window_set_status_bar_icon(main_window, gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STATUS_BAR_ICON));
  window_stack_push(main_window, true /*animated*/);

  // register callback for match timer
  tick_timer_service_subscribe(SECOND_UNIT, second_tick_handler);

  // subscribe to accelerometer updates
  accel_data_service_subscribe(0, NULL);
  timer_orientation_check = app_timer_register(ORIENTATION_CHECK_TIMER_UPDATE_INTERVAL, check_orientation, NULL);

  // initialize app message and request config (unless read from persistent storage)
  init_app_message();
  if (!has_config) request_config_via_appmessage();
}

static void deinit(void) {
  safe_persistent_state();

  accel_data_service_unsubscribe();

  destroy_menu();
  destroy_decision_screen();

  window_destroy(main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
