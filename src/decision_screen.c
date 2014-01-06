#include "decision_screen.h"

#define WINDOW_MARGIN 3

static Window* window;

static TextLayer* text_layer_message;

static ActionBarLayer* action_bar_layer;
static GBitmap* action_icon_confirm;
static GBitmap* action_icon_cancel;

static char* message;

static DecisionScreenCallback confirm_callback;
static DecisionScreenCallback cancel_callback;


static void cancel_click_handler(ClickRecognizerRef recognizer, void* context) {
  // dismiss window
  window_stack_pop(true);

  if (cancel_callback != NULL) cancel_callback();
}

static void confirm_click_handler(ClickRecognizerRef recognizer, void* context) {
  // dismiss window
  window_stack_pop(true);
  
  if (confirm_callback != NULL) confirm_callback();
}

static void click_config_provider(void* context) {
  window_single_click_subscribe(BUTTON_ID_UP,   cancel_click_handler);
  window_single_click_subscribe(BUTTON_ID_BACK, cancel_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, confirm_click_handler);
}


static void update_message() {
  if (text_layer_message != NULL) text_layer_set_text(text_layer_message, message);
}


static void window_load(Window* window) {
  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);


  action_bar_layer = action_bar_layer_create();

  action_icon_confirm = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_CONFIRM);
  action_icon_cancel  = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_CANCEL);
  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_DOWN, action_icon_confirm);
  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_UP,   action_icon_cancel);

  action_bar_layer_add_to_window(action_bar_layer, window);

  action_bar_layer_set_click_config_provider(action_bar_layer, click_config_provider);


  text_layer_message = text_layer_create((GRect) { 
    .origin = { WINDOW_MARGIN, WINDOW_MARGIN }, 
    .size = { bounds.size.w - ACTION_BAR_WIDTH - 2 * WINDOW_MARGIN, bounds.size.h - 2 * WINDOW_MARGIN } 
  });
  text_layer_set_text_alignment(text_layer_message, GTextAlignmentLeft);
  text_layer_set_font(text_layer_message, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  update_message();
  layer_add_child(window_layer, text_layer_get_layer(text_layer_message));
}

static void window_unload(Window* window) {
  action_bar_layer_destroy(action_bar_layer);
  gbitmap_destroy(action_icon_confirm);
  gbitmap_destroy(action_icon_cancel);
}


void show_decision_screen(char* text, DecisionScreenCallback confirm, DecisionScreenCallback cancel) {
  // init window on first call
  if (window == NULL) {
    window = window_create();
    window_set_window_handlers(window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
    });
  }

  message = text;
  update_message();
  confirm_callback = confirm;
  cancel_callback = cancel;

  window_stack_push(window, true);
}
