/*
 * Persistence on watch
*/

#include <pebble.h>

#include "persistence.h"
#include "state.h"


static inline int read_persistent_state_int(int persistentKey, int defaultValue);
static inline bool read_persistent_state_bool(int persistentKey, bool defaultValue);

void read_persistent_state() {
  // runtime state
  life_opponent      = read_persistent_state_int(PERSISTENT_KEY_LIFE_OPPONENT,      LIFE_DEFAULT);
  life_player        = read_persistent_state_int(PERSISTENT_KEY_LIFE_PLAYER,        LIFE_DEFAULT);
  games_won_opponent = read_persistent_state_int(PERSISTENT_KEY_GAMES_WON_OPPONENT, GAMES_SCORE_DEFAULT);
  games_won_player   = read_persistent_state_int(PERSISTENT_KEY_GAMES_WON_PLAYER,   GAMES_SCORE_DEFAULT);
  games_draw         = read_persistent_state_int(PERSISTENT_KEY_GAMES_DRAW,         GAMES_SCORE_DEFAULT);
  life_step          = read_persistent_state_int(PERSISTENT_KEY_LIFE_STEP,          LIFE_STEP_DEFAULT);
  game_continues_on_purpose = read_persistent_state_bool(PERSISTENT_KEY_GAME_CONTINUES_ON_PURPOSE, false);
  match_start_time = persist_exists(PERSISTENT_KEY_MATCH_START_TIME) ? persist_read_int(PERSISTENT_KEY_MATCH_START_TIME) : time(NULL);

  // config
}
void safe_persistent_state() {
  persist_write_int(PERSISTENT_KEY_LIFE_OPPONENT,      life_opponent);
  persist_write_int(PERSISTENT_KEY_LIFE_PLAYER,        life_player);
  persist_write_int(PERSISTENT_KEY_GAMES_WON_OPPONENT, games_won_opponent);
  persist_write_int(PERSISTENT_KEY_GAMES_WON_PLAYER,   games_won_player);
  persist_write_int(PERSISTENT_KEY_GAMES_DRAW,         games_draw);
  persist_write_int(PERSISTENT_KEY_LIFE_STEP,          life_step);

  persist_write_bool(PERSISTENT_KEY_GAME_CONTINUES_ON_PURPOSE, game_continues_on_purpose);

  persist_write_int(PERSISTENT_KEY_MATCH_START_TIME, match_start_time);
}

static inline int read_persistent_state_int(int persistentKey, int defaultValue) {
  return persist_exists(persistentKey) ? persist_read_int(persistentKey) : defaultValue;
}
static inline bool read_persistent_state_bool(int persistentKey, bool defaultValue) {
  return persist_exists(persistentKey) ? persist_read_bool(persistentKey) : defaultValue;
}
