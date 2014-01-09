#include <pebble.h>

#include "state.h"
#include "config.h"

#include "ui.h"
#include "match_timer.h"


/*
 * runtime state
*/

int life_opponent = LIFE_DEFAULT_DEFAULT;
int life_player = LIFE_DEFAULT_DEFAULT;
uint8_t games_won_opponent = GAMES_SCORE_DEFAULT;
uint8_t games_won_player = GAMES_SCORE_DEFAULT;
uint8_t games_draw = GAMES_SCORE_DEFAULT;
short life_step = LIFE_STEP_DEFAULT;
bool game_continues_on_purpose = false; // the player decided not to quit the current game after life reached 0
time_t match_start_time;

void reset_game_state() {
  life_opponent = life_default;
  life_player = life_default;
  game_continues_on_purpose = false;
  update_opponent_life_counter();
  update_player_life_counter();
}
void reset_match_state() {
  reset_game_state();
  games_won_opponent = GAMES_SCORE_DEFAULT;
  games_won_player = GAMES_SCORE_DEFAULT;
  match_start_time = time(NULL);
  update_games_won_counter_opponent();
  update_games_won_counter_player();
  match_timer_update();
}
