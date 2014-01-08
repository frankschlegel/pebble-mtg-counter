/*
 * runtime state
*/
#define LIFE_DEFAULT 20
#define LIFE_MAX 999
#define LIFE_MIN -999
#define LIFE_STEP_DEFAULT -1
#define GAMES_SCORE_DEFAULT 0
#define GAMES_SCORE_MAX 99

extern int life_opponent;
extern int life_player;
extern uint8_t games_won_opponent;
extern uint8_t games_won_player;
extern uint8_t games_draw;
extern short life_step;
extern bool game_continues_on_purpose; // the player decided not to quit the current game after life reached 0
extern time_t match_start_time;

void reset_game_state();
void reset_match_state();
