#include <pebble.h>

#include "match_time_notifications.h"


static AppTimer* match_end_timer;
static AppTimer* before_match_end_timer;


static void match_time_notifications_on_end_timer(void* unused);
static void match_time_notifications_on_before_end_timer(void* unused);


void match_time_notifications_init() {
}
void match_time_notifications_deinit() {
  match_time_notifications_end_disable();
  match_time_notifications_before_end_disable();
}

void match_time_notifications_end_enable(time_t match_started_time, int match_duration)
{
  time_t current_time = time(NULL);
  time_t timer_end_time = match_started_time + match_duration*60;
  if (current_time < timer_end_time) {
    uint32_t timeout = (timer_end_time - current_time)*1000; // ms
    if (match_end_timer) {
      app_timer_reschedule(match_end_timer, timeout);
    } else {
      match_end_timer = app_timer_register(timeout, match_time_notifications_on_end_timer, NULL);
    }
  } else if (current_time == timer_end_time) {
    match_time_notifications_end_disable();
    match_time_notifications_on_end_timer(NULL);
  }
}
void match_time_notifications_end_disable()
{
  if (match_end_timer) {
    app_timer_cancel(match_end_timer);
    match_end_timer = NULL;
  }
}

void match_time_notifications_before_end_enable(time_t match_started_time, int match_duration, int before_match_end_time)
{
  time_t current_time = time(NULL);
  time_t timer_end_time = match_started_time + (match_duration - before_match_end_time) * 60;
  if (current_time < timer_end_time) {
    uint32_t timeout = (timer_end_time - current_time)*1000; // ms
    if (before_match_end_timer) {
      app_timer_reschedule(before_match_end_timer, timeout);
    } else {
      before_match_end_timer = app_timer_register(timeout, match_time_notifications_on_before_end_timer, NULL);
    }
  } else if (current_time == timer_end_time) {
    match_time_notifications_before_end_disable();
    match_time_notifications_on_before_end_timer(NULL);
  }
}
void match_time_notifications_before_end_disable()
{
  if (before_match_end_timer) {
    app_timer_cancel(before_match_end_timer);
    before_match_end_timer = NULL;
  }
}


static void match_time_notifications_on_end_timer(void* unused)
{
  vibes_double_pulse();
  match_end_timer = NULL;
}

static void match_time_notifications_on_before_end_timer(void* unused)
{
  vibes_short_pulse();
  before_match_end_timer = NULL;
}
