//#if 0
#include <pebble.h>

#include "autorotate.h"
#include "score_layer.h"


#define ORIENTATION_CHECK_TIMER_UPDATE_INTERVAL 500
#define Y_THRESHOLD_ORIENTATION_UPSIDE_DOWN 600
#define Y_THRESHOLD_ORIENTATION_NORMAL 500

static AppTimer* timer_orientation_check;

static void autorotate_on_check_orientation_timer(void* unused);


void autorotate_init() {
  timer_orientation_check = NULL;
}
void autorotate_deinit() {
  autorotate_disable(false /* rotate_to_normal */);
}
void autorotate_enable()
{
  // subscribe to accelerometer updates
  if (!timer_orientation_check) {
    accel_data_service_subscribe(0, NULL);
    timer_orientation_check = app_timer_register(ORIENTATION_CHECK_TIMER_UPDATE_INTERVAL, autorotate_on_check_orientation_timer, NULL);
  }
}
void autorotate_disable(bool rotate_to_normal)
{
  if (timer_orientation_check) {
    app_timer_cancel(timer_orientation_check);
    timer_orientation_check = NULL;
    accel_data_service_unsubscribe();

    if (rotate_to_normal) {
      score_layer_set_orientation_all(ScoreLayerOrientationNormal);
    }
  }
}

static void autorotate_on_check_orientation_timer(void* unused)
{
  // get accelerometer data
  AccelData accel_data = (AccelData) { .x = 0, .y = 0, .z = 0 };
  accel_service_peek(&accel_data);

  // toggle orientation if past threshold
  ScoreLayerOrientation current_orientation = score_layer_get_orientation_all();
  if (current_orientation == ScoreLayerOrientationNormal && accel_data.y > Y_THRESHOLD_ORIENTATION_UPSIDE_DOWN) {
    score_layer_set_orientation_all(ScoreLayerOrientationUpsideDown);
  } else if (current_orientation == ScoreLayerOrientationUpsideDown && accel_data.y < Y_THRESHOLD_ORIENTATION_NORMAL) {
    score_layer_set_orientation_all(ScoreLayerOrientationNormal);
  }

  // restart timer
  timer_orientation_check = app_timer_register(ORIENTATION_CHECK_TIMER_UPDATE_INTERVAL, autorotate_on_check_orientation_timer, NULL);
}

//#endif
