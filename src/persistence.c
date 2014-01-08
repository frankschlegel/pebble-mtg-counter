/*
 * Persistence on watch
*/

#include <pebble.h>

#include "persistence.h"
#include "state.h"
#include "config.h"


static inline bool read_persistent_state_int(int persistent_key, int* out);
static inline bool read_persistent_state_bool(int persistent_key, bool* out);
static inline int  read_persistent_state_int_default(int persistent_key, int default_value);
static inline bool read_persistent_state_bool_default(int persistent_key, bool default_value);


void read_persistent_state() {
  // config
  has_config = false;
  // has_config = read_persistent_state_int (PERSISTENT_KEY_CONFIG_BASE+CONFIG_KEY_LIFE_DEFAULT, &life_default) && has_config;
  // has_config = read_persistent_state_int (PERSISTENT_KEY_CONFIG_BASE+CONFIG_KEY_MATCH_DURATION, &match_duration) && has_config;
  // has_config = read_persistent_state_bool(PERSISTENT_KEY_CONFIG_BASE+CONFIG_KEY_MATCH_END_VIBRATION, &match_end_vibration) && has_config;
  // has_config = read_persistent_state_bool(PERSISTENT_KEY_CONFIG_BASE+CONFIG_KEY_BEFORE_MATCH_END_VIBRATION, &before_match_end_vibration) && has_config;
  // has_config = read_persistent_state_int (PERSISTENT_KEY_CONFIG_BASE+CONFIG_KEY_BEFORE_MATCH_END_TIME, &before_match_end_time) && has_config;
  // has_config = read_persistent_state_bool(PERSISTENT_KEY_CONFIG_BASE+CONFIG_KEY_SHOW_TIMER, &show_timer) && has_config;
  // has_config = read_persistent_state_bool(PERSISTENT_KEY_CONFIG_BASE+CONFIG_KEY_ROTATION_LOCK, &rotation_lock) && has_config;
  // has_config = read_persistent_state_bool(PERSISTENT_KEY_CONFIG_BASE+CONFIG_KEY_INVERT_COLORS, &invert_colors) && has_config;
  // if (has_config) APP_LOG(APP_LOG_LEVEL_DEBUG, "Loaded config");

  // runtime state
  life_opponent = read_persistent_state_int_default(PERSISTENT_KEY_LIFE_OPPONENT, life_default);
  life_player = read_persistent_state_int_default(PERSISTENT_KEY_LIFE_PLAYER, life_default);
  games_won_opponent = read_persistent_state_int_default(PERSISTENT_KEY_GAMES_WON_OPPONENT, GAMES_SCORE_DEFAULT);
  games_won_player = read_persistent_state_int_default(PERSISTENT_KEY_GAMES_WON_PLAYER, GAMES_SCORE_DEFAULT);
  games_draw = read_persistent_state_int_default(PERSISTENT_KEY_GAMES_DRAW, GAMES_SCORE_DEFAULT);
  life_step = read_persistent_state_int_default(PERSISTENT_KEY_LIFE_STEP, LIFE_STEP_DEFAULT);
  game_continues_on_purpose = read_persistent_state_bool_default(PERSISTENT_KEY_GAME_CONTINUES_ON_PURPOSE, false);
  match_start_time = read_persistent_state_int_default(PERSISTENT_KEY_MATCH_START_TIME, time(NULL));
}
void safe_persistent_state() {
  // runtime state
  persist_write_int (PERSISTENT_KEY_LIFE_OPPONENT,      life_opponent);
  persist_write_int (PERSISTENT_KEY_LIFE_PLAYER,        life_player);
  persist_write_int (PERSISTENT_KEY_GAMES_WON_OPPONENT, games_won_opponent);
  persist_write_int (PERSISTENT_KEY_GAMES_WON_PLAYER,   games_won_player);
  persist_write_int (PERSISTENT_KEY_GAMES_DRAW,         games_draw);
  persist_write_int (PERSISTENT_KEY_LIFE_STEP,          life_step);
  persist_write_bool(PERSISTENT_KEY_GAME_CONTINUES_ON_PURPOSE, game_continues_on_purpose);
  persist_write_int (PERSISTENT_KEY_MATCH_START_TIME,   match_start_time);

  // config
  persist_write_int (PERSISTENT_KEY_CONFIG_BASE+CONFIG_KEY_LIFE_DEFAULT, life_default);
  persist_write_int (PERSISTENT_KEY_CONFIG_BASE+CONFIG_KEY_MATCH_DURATION, match_duration);
  persist_write_bool(PERSISTENT_KEY_CONFIG_BASE+CONFIG_KEY_MATCH_END_VIBRATION, match_end_vibration);
  persist_write_bool(PERSISTENT_KEY_CONFIG_BASE+CONFIG_KEY_BEFORE_MATCH_END_VIBRATION, before_match_end_vibration);
  persist_write_int (PERSISTENT_KEY_CONFIG_BASE+CONFIG_KEY_BEFORE_MATCH_END_TIME, before_match_end_time);
  persist_write_bool(PERSISTENT_KEY_CONFIG_BASE+CONFIG_KEY_SHOW_TIMER, show_timer);
  persist_write_bool(PERSISTENT_KEY_CONFIG_BASE+CONFIG_KEY_ROTATION_LOCK, rotation_lock);
  persist_write_bool(PERSISTENT_KEY_CONFIG_BASE+CONFIG_KEY_INVERT_COLORS, invert_colors);
}

static inline bool read_persistent_state_int(int persistent_key, int* out) {
  if (persist_exists(persistent_key)) {
    *out = persist_read_int(persistent_key);
    return true;
  } else {
    return false;
  }
}
static inline bool read_persistent_state_bool(int persistent_key, bool* out) {
  if (persist_exists(persistent_key)) {
    *out = persist_read_bool(persistent_key);
    return true;
  } else {
    return false;
  }
}

static inline int read_persistent_state_int_default(int persistent_key, int default_value) {
  return persist_exists(persistent_key) ? persist_read_int(persistent_key) : default_value;
}
static inline bool read_persistent_state_bool_default(int persistent_key, bool default_value) {
  return persist_exists(persistent_key) ? persist_read_bool(persistent_key) : default_value;
}
