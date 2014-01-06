#include <pebble.h>

typedef void (*DecisionScreenCallback)();

void show_decision_screen(char* text, DecisionScreenCallback confirm_callback, DecisionScreenCallback cancel_callback);
void destroy_decision_screen();
