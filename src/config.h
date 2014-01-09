/*
 * app configuration (cooperates with Javascript through AppMessage)
*/

// Keys for identifying config options in mtg-counter.js and config.html.
// See /appinfo.json
enum ConfigKey {
  CONFIG_KEY_LIFE_DEFAULT = 0,
  CONFIG_KEY_MATCH_DURATION = 1,
  CONFIG_KEY_MATCH_END_VIBRATION = 2,
  CONFIG_KEY_BEFORE_MATCH_END_VIBRATION = 3,
  CONFIG_KEY_BEFORE_MATCH_END_TIME = 4,
  CONFIG_KEY_SHOW_TIMER = 5,
  CONFIG_KEY_ROTATION_LOCK = 6,
  CONFIG_KEY_INVERT_COLORS = 7,

  NUM_CONFIG_KEYS = 8,
};


#define LIFE_DEFAULT_DEFAULT 20
#define MATCH_DURATION_DEFAULT 50
#define MATCH_END_VIBRATION_DEFAULT 1
#define BEFORE_MATCH_END_VIBRATION_DEFAULT 0
#define BEFORE_MATCH_END_TIME 10
#define SHOW_TIMER_DEFAULT 1
#define ROTATION_LOCK_DEFAULT 1
#define INVERT_COLORS_DEFAULT 0
extern int life_default;
extern int match_duration; // minutes
extern bool match_end_vibration;
extern bool before_match_end_vibration;
extern int before_match_end_time; // minutes
extern bool show_timer;
extern bool rotation_lock;
extern bool invert_colors;

extern bool has_config;


void config_handle_update();
void config_update_match_end_vibration();
void config_update_before_match_end_vibration();

void request_config_via_appmessage();
void handle_config_received_via_appmessage(DictionaryIterator *received);
