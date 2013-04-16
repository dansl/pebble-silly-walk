#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0xC2, 0x01, 0x53, 0xB1, 0xD7, 0xBD, 0x47, 0xCC, 0x90, 0xD6, 0x02, 0x2B, 0x51, 0xBA, 0x7B, 0x89 }

PBL_APP_INFO(MY_UUID,
             "Silly Walk Simple - No Seconds", "Dansl",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;

BmpContainer background_image_container;

RotBmpPairContainer hour_hand_image_container;
RotBmpPairContainer minute_hand_image_container;
RotBmpPairContainer second_hand_image_container;

void update_watch(PblTm* t){

	//rotbmp_pair_layer_set_angle(&hour_hand_image_container.layer, ((t->tm_hour % 12) * 30) + (t->tm_min/2));
	hour_hand_image_container.layer.white_layer.rotation = TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 30) + (t->tm_min/2)) / 360;
  	hour_hand_image_container.layer.black_layer.rotation = TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 30) + (t->tm_min/2)) / 360;
	hour_hand_image_container.layer.layer.frame.origin.x = (144/2) - (hour_hand_image_container.layer.layer.frame.size.w/2);
	hour_hand_image_container.layer.layer.frame.origin.y = (168/2) - (hour_hand_image_container.layer.layer.frame.size.h/2);
	layer_mark_dirty(&hour_hand_image_container.layer.layer);

	//rotbmp_pair_layer_set_angle(&minute_hand_image_container.layer, t->tm_min * 6);
	minute_hand_image_container.layer.white_layer.rotation = TRIG_MAX_ANGLE * (t->tm_min * 6) / 360;
  	minute_hand_image_container.layer.black_layer.rotation = TRIG_MAX_ANGLE * (t->tm_min * 6) / 360;
	minute_hand_image_container.layer.layer.frame.origin.x = (144/2) - (minute_hand_image_container.layer.layer.frame.size.w/2);
	minute_hand_image_container.layer.layer.frame.origin.y = (168/2) - (minute_hand_image_container.layer.layer.frame.size.h/2);
	layer_mark_dirty(&minute_hand_image_container.layer.layer);

}

// Called once per second
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
	update_watch(t->tick_time);
}


// Handle the start-up of the app
void handle_init(AppContextRef app_ctx) {

	// Create our app's base window
	window_init(&window, "Silly Walk");
	window_stack_push(&window, true);
	window_set_background_color(&window, GColorBlack);

	resource_init_current_app(&APP_RESOURCES);

	// Set up a layer for the static watch face background
	bmp_init_container(RESOURCE_ID_IMAGE_BACKGROUND, &background_image_container);
	layer_add_child(&window.layer, &background_image_container.layer.layer);


	// Set up a layer for the hour hand
	rotbmp_pair_init_container(RESOURCE_ID_IMAGE_HOUR_HAND_WHITE, RESOURCE_ID_IMAGE_HOUR_HAND_BLACK, &hour_hand_image_container);
	rotbmp_pair_layer_set_src_ic(&hour_hand_image_container.layer, GPoint(33, 40));
	layer_add_child(&window.layer, &hour_hand_image_container.layer.layer);


	// Set up a layer for the minute hand
	rotbmp_pair_init_container(RESOURCE_ID_IMAGE_MINUTE_HAND_WHITE, RESOURCE_ID_IMAGE_MINUTE_HAND_BLACK, &minute_hand_image_container);
	rotbmp_pair_layer_set_src_ic(&minute_hand_image_container.layer, GPoint(16, 60));
	layer_add_child(&window.layer, &minute_hand_image_container.layer.layer);


	PblTm t;
	get_time(&t);
	update_watch(&t);

}

void handle_deinit(AppContextRef ctx) {

	bmp_deinit_container(&background_image_container);
	rotbmp_pair_deinit_container(&hour_hand_image_container);
	rotbmp_pair_deinit_container(&minute_hand_image_container);
	rotbmp_pair_deinit_container(&second_hand_image_container);
}


// The main event/run loop for our app
void pbl_main(void *params) {
  PebbleAppHandlers handlers = {

    // Handle app start
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,

    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }

  };
  app_event_loop(params, &handlers);
}