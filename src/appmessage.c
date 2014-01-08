#include <pebble.h>

#include "appmessage.h"
#include "config.h"


// See dict_calc_buffer_size_from_tuplets(), dict_calc_buffer_size()
#define APP_MESSAGE_DICTIONARY_HEADER_SIZE 1
#define APP_MESSAGE_TUPLET_HEADER_SIZE 7
#define APP_MESSAGE_VALUE_SIZE 4 // all int32_t
#define APP_MESSAGE_INBOX_SIZE (APP_MESSAGE_DICTIONARY_HEADER_SIZE + (APP_MESSAGE_TUPLET_HEADER_SIZE + APP_MESSAGE_VALUE_SIZE) * NUM_CONFIG_KEYS)
#define APP_MESSAGE_OUTBOX_SIZE (APP_MESSAGE_DICTIONARY_HEADER_SIZE + (APP_MESSAGE_TUPLET_HEADER_SIZE + APP_MESSAGE_VALUE_SIZE) * 1)


static void handle_appmessage_receive(DictionaryIterator *received, void *context);


void init_app_message() {
  app_message_register_inbox_received(&handle_appmessage_receive);
  app_message_open(APP_MESSAGE_INBOX_SIZE, APP_MESSAGE_OUTBOX_SIZE);
}

static void handle_appmessage_receive(DictionaryIterator *received, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Received AppMessage");
  handle_config_received_via_appmessage(received);
}
