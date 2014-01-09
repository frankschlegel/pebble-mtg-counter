#include <pebble.h>

#include "match_timer.h"

#include "state.h"


static TextLayer* layer;

static void second_tick_handler(struct tm *tick_time, TimeUnits units_changed);


void match_timer_init()
{
  layer = text_layer_create((GRect) { .origin = {0, 0}, .size = { 10, 10 } }); // will update frame before adding to window
  text_layer_set_text_alignment(layer, GTextAlignmentCenter);
  text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_background_color(layer, GColorBlack);
  text_layer_set_text_color(layer, GColorWhite);
}
void match_timer_deinit()
{
  tick_timer_service_unsubscribe();
  text_layer_destroy(layer);
}

void match_timer_enable()
{
  layer_set_hidden((Layer*)layer, false);
  match_timer_update();
  tick_timer_service_subscribe(SECOND_UNIT, second_tick_handler);
}
void match_timer_disable()
{
  tick_timer_service_unsubscribe();
  layer_set_hidden((Layer*)layer, true);
}


Layer* match_timer_get_layer() {
  return text_layer_get_layer(layer);
}


static void second_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  match_timer_update();
}

void match_timer_update() {
  time_t diff = time(NULL) - match_start_time;

  static char text[9];
  strftime(text, sizeof(text), "%H:%M:%S", localtime(&diff));
  text_layer_set_text(layer, text);
}
