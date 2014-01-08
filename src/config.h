/*
 * app configuration (cooperates with Javascript through AppMessage)
*/

// Keys for identifying config options in mtg-counter.js and config.html.
// See /appinfo.json
enum ConfigKey {
  CONFIG_KEY_LIFE_POINTS = 0,
  CONFIG_KEY_MATCH_DURATION = 1,
  CONFIG_KEY_MATCH_END_VIBRATION = 2,
  CONFIG_KEY_BEFORE_MATCH_END_VIBRATION = 3,
  CONFIG_KEY_BEFORE_MATCH_END_TIME = 4,
  CONFIG_KEY_SHOW_TIMER = 5,
  CONFIG_KEY_ROTATION_LOCK = 6,
  CONFIG_KEY_INVERT_COLORS = 7,
};
