#include <pebble.h>

#include "config.h"
#include "appmessage.h"
#include "state.h"
#include "ui.h"


int life_default = LIFE_DEFAULT_DEFAULT;
int match_duration = MATCH_DURATION_DEFAULT;
bool match_end_vibration = MATCH_END_VIBRATION_DEFAULT;
bool before_match_end_vibration = BEFORE_MATCH_END_VIBRATION_DEFAULT;
int before_match_end_time = BEFORE_MATCH_END_TIME;
bool show_timer = SHOW_TIMER_DEFAULT;
bool rotation_lock = ROTATION_LOCK_DEFAULT;
bool invert_colors = INVERT_COLORS_DEFAULT;

bool has_config = false; // prevents saving default config in JavaScript. See deinit()


void request_config_via_appmessage()
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Requesting config");
  
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  if (!iter) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Cannot write to AppMessage outbox");
    return;
  }
  
  Tuplet request_tuple = TupletInteger(APP_MESSAGE_KEY_REQUEST_CONFIG, 1);
  dict_write_tuplet(iter, &request_tuple);
  dict_write_end(iter);
  
  app_message_outbox_send();
}

void handle_config_received_via_appmessage(DictionaryIterator *received)
{
  bool will_reset_life = life_opponent == life_default && life_player == life_default;

  Tuple *tuple = dict_read_first(received);
  while (tuple) {
    int32_t value = tuple->value->int32;
    switch (tuple->key) {
      case CONFIG_KEY_LIFE_DEFAULT: life_default = value; break;
      case CONFIG_KEY_MATCH_DURATION: match_duration = value; break;
      case CONFIG_KEY_MATCH_END_VIBRATION: match_end_vibration = value; break;
      case CONFIG_KEY_BEFORE_MATCH_END_VIBRATION: before_match_end_vibration = value; break;
      case CONFIG_KEY_BEFORE_MATCH_END_TIME: before_match_end_time = value; break;
      case CONFIG_KEY_SHOW_TIMER: show_timer = value; break;
      case CONFIG_KEY_ROTATION_LOCK: rotation_lock = value; break;
      case CONFIG_KEY_INVERT_COLORS: invert_colors = value; break;
    }
    tuple = dict_read_next(received);
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Received config");
  has_config = true;

  if (will_reset_life) {
    // No scores have been changed yet
    // --> set initial life counts to life_default just received from config
    life_player = life_default;
    life_opponent = life_default;
    update_opponent_life_counter();
    update_player_life_counter();
  }

  // TODO update UI, timers, ...
}