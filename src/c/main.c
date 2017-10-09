/*
 * Simple Scrolling text layer for lyrics
 */

#include <pebble.h>


#define MIN(a,b)  ((a) < (b) ? (a) : (b))
#define MAX_TEXT_SIZE 2000

static Window *s_window;
static TextLayer *s_info_text_layer;
static TextLayer *s_lyrics_text_layer;
static ScrollLayer *s_scroll_layer;
static char s_info_buffer[128];

char *translate_error(AppMessageResult result) {
  switch (result) {
    case APP_MSG_OK: return "APP_MSG_OK";
    case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
    case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
    case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
    case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
    case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
    case APP_MSG_BUSY: return "APP_MSG_BUSY";
    case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
    case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
    case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
    case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
    case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
    case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
    case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
    default: return "UNKNOWN ERROR";
  }
}

void set_lyrics(char* lyrics) {
  text_layer_set_text(s_lyrics_text_layer, lyrics);
  
  // trim text layer and scroll content to fit text box
  GRect bounds = layer_get_bounds(window_get_root_layer(s_window));
  GSize max_size = text_layer_get_content_size(s_lyrics_text_layer);
  text_layer_set_size(s_lyrics_text_layer, max_size);
  scroll_layer_set_content_size(s_scroll_layer, GSize(bounds.size.w, max_size.h + 4));
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Message received.");
    
  Tuple *tuple = dict_find(iter, MESSAGE_KEY_LYRICS);
  if (tuple) {
    set_lyrics(tuple->value->cstring);
  } else {
    char *track = dict_find(iter, MESSAGE_KEY_TRACK)->value->cstring;
    char *artist = dict_find(iter, MESSAGE_KEY_ARTIST)->value->cstring;
    char *album = dict_find(iter, MESSAGE_KEY_ALBUM)->value->cstring;
    
    snprintf(s_info_buffer, sizeof(s_info_buffer), "%s - %s\n%s", track, artist, album);
    text_layer_set_text(s_info_text_layer, s_info_buffer);
    
    DictionaryIterator *outbox;
    app_message_outbox_begin(&outbox);
    // Send the app message
    dict_write_cstring(outbox, MESSAGE_KEY_ARTIST, artist);
    dict_write_cstring(outbox, MESSAGE_KEY_TRACK, track);
    dict_write_cstring(outbox, MESSAGE_KEY_ALBUM, album);    
    app_message_outbox_send();
  }
  
}

static void inbox_dropped_handler(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Dropped message! Reason given: %s", translate_error(reason));
}

static void outbox_sent_handler(DictionaryIterator *iter, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Message sent.");
}

static void outbox_failed_handler(DictionaryIterator *iter, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to send message. Reason = %s", translate_error(reason));
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  
  // setup scroll layer
  GRect bounds = layer_get_bounds(window_layer);
  s_scroll_layer = scroll_layer_create(GRect(0, 32, bounds.size.w, bounds.size.h - 32));
  scroll_layer_set_click_config_onto_window(s_scroll_layer, window);
    
  // setup lyrics text layer
  GRect max_text_bounds = GRect(0, 0, bounds.size.w, MAX_TEXT_SIZE);
  s_lyrics_text_layer = text_layer_create(max_text_bounds);
  
  // setup info text layer
  s_info_text_layer = text_layer_create(GRect(0, 0, bounds.size.w, 30));
  text_layer_set_text(s_info_text_layer, "Loading track info...");
  
  // add the layers
  layer_add_child(window_layer, text_layer_get_layer(s_info_text_layer));
  scroll_layer_add_child(s_scroll_layer, text_layer_get_layer(s_lyrics_text_layer));
  layer_add_child(window_layer, scroll_layer_get_layer(s_scroll_layer));
  
  // hook up app messaging
  app_message_register_inbox_received(inbox_received_handler);
  app_message_register_inbox_dropped(inbox_dropped_handler);
  app_message_register_outbox_sent(outbox_sent_handler);
  app_message_register_outbox_failed(outbox_failed_handler);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Max buffer sizes are %li / %li",
    app_message_inbox_size_maximum(),
    app_message_outbox_size_maximum());
  app_message_open(app_message_inbox_size_maximum(), MIN(512, app_message_outbox_size_maximum()));
}

static void window_unload(Window *window) {
  app_message_deregister_callbacks();
  text_layer_destroy(s_info_text_layer);
  text_layer_destroy(s_lyrics_text_layer);
  scroll_layer_destroy(s_scroll_layer);
}

static void init() {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  
  window_stack_push(s_window, true);
}

static void deinit() {
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}