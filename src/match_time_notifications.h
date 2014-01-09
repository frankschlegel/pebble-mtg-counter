void match_time_notifications_init();
void match_time_notifications_deinit();

void match_time_notifications_end_enable(time_t match_started_time, int match_duration);
void match_time_notifications_end_disable();

void match_time_notifications_before_end_enable(time_t match_started_time, int match_duration, int before_match_end_time);
void match_time_notifications_before_end_disable();
