#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include <math.h>

#define HOURS_X 0
#define HOURS_Y 60 

#define SHADOW_OFFSET_X 3
#define SHADOW_OFFSET_Y 3

#define OUTLINE_1_OFFSET_X -1
#define OUTLINE_1_OFFSET_Y -1

#define OUTLINE_2_OFFSET_X 1
#define OUTLINE_2_OFFSET_Y -1

#define OUTLINE_3_OFFSET_X -1
#define OUTLINE_3_OFFSET_Y 1

#define MY_UUID { 0xB5, 0xCA, 0xC1, 0x91, 0xA9, 0x9D, 0x40, 0x8D, 0xA2, 0x2D, 0xEE, 0xB1, 0x42, 0xC3, 0xCA, 0x00 }
PBL_APP_INFO(MY_UUID,
             "Hourglass Fill", "Camron Flanders",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;

Layer display_layer;
TextLayer hours_layer;
TextLayer hours_shadow;
TextLayer hours_outline1;
TextLayer hours_outline2;
TextLayer hours_outline3;

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
    text_layer_set_text(&hours_shadow, time_text);
    text_layer_set_text(&hours_outline1, time_text);
    text_layer_set_text(&hours_outline2, time_text);
    text_layer_set_text(&hours_outline3, time_text);

    if(t.tm_min > 0) {
        lines = floor(t.tm_min*2.8);
        GRect sand = GRect(0, 168 - lines, 144, lines);

        graphics_fill_rect(ctx, sand, 0, GCornerNone);
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

  //GFont hour_font = fonts_get_system_font(FONT_KEY_GOTHAM_42_BOLD);

  resource_init_current_app(&APP_RESOURCES);
  GFont hour_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_BEBAS_48));

  text_layer_init(&hours_shadow, window.layer.frame);
  text_layer_set_text_color(&hours_shadow, GColorBlack);
  text_layer_set_font(&hours_shadow, hour_font);
  text_layer_set_background_color(&hours_shadow, GColorClear);
  layer_set_frame(&(hours_shadow.layer), GRect(HOURS_X + SHADOW_OFFSET_X, HOURS_Y + SHADOW_OFFSET_Y, 144, 84));
  text_layer_set_text_alignment(&hours_shadow, GTextAlignmentCenter);
  layer_add_child(&display_layer, &hours_shadow.layer);

  text_layer_init(&hours_outline1, window.layer.frame);
  text_layer_set_text_color(&hours_outline1, GColorBlack);
  text_layer_set_font(&hours_outline1, hour_font);
  text_layer_set_background_color(&hours_outline1, GColorClear);
  layer_set_frame(&(hours_outline1.layer), GRect(HOURS_X + OUTLINE_1_OFFSET_X, HOURS_Y + OUTLINE_1_OFFSET_Y, 144, 84));
  text_layer_set_text_alignment(&hours_outline1, GTextAlignmentCenter);
  layer_add_child(&display_layer, &hours_outline1.layer);

  text_layer_init(&hours_outline2, window.layer.frame);
  text_layer_set_text_color(&hours_outline2, GColorBlack);
  text_layer_set_font(&hours_outline2, hour_font);
  text_layer_set_background_color(&hours_outline2, GColorClear);
  layer_set_frame(&(hours_outline2.layer), GRect(HOURS_X + OUTLINE_2_OFFSET_X, HOURS_Y + OUTLINE_2_OFFSET_Y, 144, 84));
  text_layer_set_text_alignment(&hours_outline2, GTextAlignmentCenter);
  layer_add_child(&display_layer, &hours_outline2.layer);

  text_layer_init(&hours_outline3, window.layer.frame);
  text_layer_set_text_color(&hours_outline3, GColorBlack);
  text_layer_set_font(&hours_outline3, hour_font);
  text_layer_set_background_color(&hours_outline3, GColorClear);
  layer_set_frame(&(hours_outline3.layer), GRect(HOURS_X + OUTLINE_3_OFFSET_X, HOURS_Y + OUTLINE_3_OFFSET_Y, 144, 84));
  text_layer_set_text_alignment(&hours_outline3, GTextAlignmentCenter);
  layer_add_child(&display_layer, &hours_outline3.layer);

  text_layer_init(&hours_layer, window.layer.frame);
  text_layer_set_text_color(&hours_layer, GColorWhite);
  text_layer_set_font(&hours_layer, hour_font);
  text_layer_set_background_color(&hours_layer, GColorClear);
  layer_set_frame(&(hours_layer.layer), GRect(HOURS_X, HOURS_Y, 124, 84));
  text_layer_set_text_alignment(&hours_layer, GTextAlignmentCenter);
  layer_add_child(&display_layer, &hours_layer.layer);
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

