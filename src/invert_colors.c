#include <pebble.h>

#include "invert_colors.h"


static InverterLayer* layer;


void invert_colors_init() {
  layer = inverter_layer_create((GRect) { .origin = { 0, 0 }, .size = { 100, 100 } }); // will update frame before adding to window
}
void invert_colors_deinit() {
  layer_destroy((Layer*)layer);
}

void invert_colors_enable() {
  layer_set_hidden((Layer*)layer, false);
}
void invert_colors_disable() {
  layer_set_hidden((Layer*)layer, true);
}

Layer* invert_colors_get_layer() {
  return inverter_layer_get_layer(layer);
}