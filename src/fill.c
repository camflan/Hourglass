#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include <math.h>


#define MY_UUID { 0xB5, 0xCA, 0xC1, 0x91, 0xA9, 0x9D, 0x40, 0x8D, 0xA2, 0x2D, 0xEE, 0xB1, 0x42, 0xC3, 0xCA, 0x00 }
PBL_APP_INFO(MY_UUID,
             "Hourglass Fill", "Camron Flanders",
             0, 8, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;

Layer display_layer;
Layer inverse_layer;
TextLayer hours_layer;
TextLayer hours_inverse;

void display_layer_update_callback(Layer *me, GContext* ctx) {
    (void)me;
    double lines;

    static char time_text[] = "00";
    char *time_format;

    PblTm t;
    get_time(&t);

    graphics_context_set_fill_color(ctx, GColorBlack);

    time_format = "%I";

    if(clock_is_24h_style()) {
        time_format = "%R";
    }

    string_format_time(time_text, sizeof(time_text), time_format, &t);

    // Kludge to handle lack of non-padded hour format string
    // for twelve hour clock.
    if (!clock_is_24h_style() && (time_text[0] == '0')) {
        memmove(time_text, &time_text[1], sizeof(time_text) - 1);
    }
    

    text_layer_set_text(&hours_layer, time_text);
    text_layer_set_text(&hours_inverse, time_text);

    if(t.tm_min > 0) {
        lines = floor(t.tm_min*2.8);
        GRect sand = GRect(0, 168 - lines, 144, lines);

        graphics_fill_rect(ctx, sand, 0, GCornerNone);
        
        int inverse_height = 168 - lines - 67;
        inverse_height = inverse_height > 0 ? inverse_height : 0;
        
        layer_set_frame(&inverse_layer, GRect(0, 67, 144, inverse_height));
    }
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
    (void)t;
    (void)ctx;

    layer_mark_dirty(&display_layer);
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Hourglass Fill");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorWhite);

  layer_init(&display_layer, window.layer.frame);
  display_layer.update_proc = &display_layer_update_callback;
  layer_add_child(&window.layer, &display_layer);

  GFont hour_font = fonts_get_system_font(FONT_KEY_GOTHAM_42_BOLD);

  text_layer_init(&hours_layer, window.layer.frame);
  text_layer_set_text_color(&hours_layer, GColorWhite);
  text_layer_set_font(&hours_layer, hour_font);
  text_layer_set_background_color(&hours_layer, GColorClear);
  layer_set_frame(&(hours_layer.layer), GRect(0, 67, 144, 84));
  text_layer_set_text_alignment(&hours_layer, GTextAlignmentCenter);
  layer_add_child(&display_layer, &hours_layer.layer);
  
  layer_init(&inverse_layer, GRect(0, 67, 144, 0));
  layer_add_child(&window.layer, &inverse_layer);

  text_layer_init(&hours_inverse, window.layer.frame);
  text_layer_set_text_color(&hours_inverse, GColorBlack);
  text_layer_set_font(&hours_inverse, hour_font);
  text_layer_set_background_color(&hours_inverse, GColorClear);
  layer_set_frame(&(hours_inverse.layer), GRect(0, 0, 144, 84));
  text_layer_set_text_alignment(&hours_inverse, GTextAlignmentCenter);
  layer_add_child(&inverse_layer, &hours_inverse.layer);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    .tick_info = {
        .tick_handler = &handle_minute_tick,
        .tick_units = MINUTE_UNIT
    }
  };

  app_event_loop(params, &handlers);
}
