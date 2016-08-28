#include <pebble.h>
#include "main.h"

static Window *s_main_window;
static Layer *s_background_layer, *s_date_layer, *s_hands_layer;
static TextLayer *s_weekday_label, *s_month_label;
static char s_weekday_buffer[16], s_month_buffer[16];
static BitmapLayer *s_sharingan_layer;
static GPath *s_tick_paths[NUM_CLOCK_TICKS];
static GPath *s_minute_arrow, *s_hour_arrow;

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Methods /////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

void getMonth(char *input, int location, struct tm *tick_time) {		// 0 is before, 1 is after
	char month_current[16];
	strftime(month_current, sizeof(month_current), "%B", tick_time);
 	
	#if defined(PBL_RECT)
		if(strlen(month_current) > 7) {
			strcat(input,"  %b");
		} else {
			strcat(input,"  %B");
		}
	#elif defined(PBL_ROUND)
		if(strlen(month_current) > 5) {
			strcat(input,"  %b");
	 	} else {
			strcat(input,"  %B");
		}
	#endif
}

void getSuffix(char *input, char date_current[5]) {
	if (strncmp(date_current, "01", 2) == 0 || strncmp(date_current, "21", 2) == 0 || strncmp(date_current,"31",2) == 0) { 
		strcat(input,"st");
	} else if (strncmp(date_current, "02", 2) == 0 || strncmp(date_current, "22", 2) == 0) {
		strcat(input,"nd");
	} else if (strncmp(date_current, "03", 2) == 0 || strncmp(date_current, "23", 2) == 0) { 
		strcat(input,"rd");
	} else {
		strcat(input,"th");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Callbacks ///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
// Colours
	Tuple *colour_background_t	= dict_find(iter, MESSAGE_KEY_COLOUR_BACKGROUND);
 	Tuple *colour_hands_t		= dict_find(iter, MESSAGE_KEY_COLOUR_HANDS);
	Tuple *colour_weekday_t		= dict_find(iter, MESSAGE_KEY_COLOUR_WEEKDAY);
	Tuple *colour_month_t		= dict_find(iter, MESSAGE_KEY_COLOUR_MONTH);
// 	Tuple *colour_bluetooth_t	= dict_find(iter, MESSAGE_KEY_COLOUR_BLUETOOTH);
    int colour_background		= colour_background_t->value->int32;
 	int colour_hands			= colour_hands_t->value->int32;
	int colour_weekday			= colour_weekday_t->value->int32;
	int colour_month			= colour_month_t->value->int32;
// 	int colour_bluetooth = colour_bluetooth_t->value->int32;
	persist_write_int(MESSAGE_KEY_COLOUR_BACKGROUND, colour_background);
	persist_write_int(MESSAGE_KEY_COLOUR_HANDS, colour_hands);
	persist_write_int(MESSAGE_KEY_COLOUR_WEEKDAY, colour_weekday);
	persist_write_int(MESSAGE_KEY_COLOUR_MONTH, colour_month);
// 	persist_write_int(MESSAGE_KEY_COLOUR_BLUETOOTH, colour_bluetooth);	

// Date	
	Tuple *toggle_suffix_t = dict_find(iter, MESSAGE_KEY_TOGGLE_SUFFIX);
	int toggle_suffix = toggle_suffix_t->value->int32;
	persist_write_int(MESSAGE_KEY_TOGGLE_SUFFIX, toggle_suffix);
	
// Update
	layer_mark_dirty(s_background_layer); //update background
	layer_mark_dirty(s_hands_layer); //update Hands
	text_layer_set_text_color(s_weekday_label, GColorFromHEX(colour_weekday));	// Weekday
	text_layer_set_text_color(s_month_label, GColorFromHEX(colour_month));		// Month
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Main ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void bg_update_proc(Layer *layer, GContext *ctx) {
	int colour_background = persist_read_int(MESSAGE_KEY_COLOUR_BACKGROUND);
	if(colour_background) {
		graphics_context_set_fill_color(ctx, GColorFromHEX(colour_background));		// Background
		graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
	} else {
		graphics_context_set_fill_color(ctx, GColorBlack);		// Background
		graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
	}
// 	for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
// 		gpath_draw_filled(ctx, s_tick_paths[i]);
// 	}
}

static void hands_update_proc(Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);

	time_t now = time(NULL);
	struct tm *tick_time = localtime(&now);

// Hour
	int colour_background = persist_read_int(MESSAGE_KEY_COLOUR_BACKGROUND);
	int colour_hands = persist_read_int(MESSAGE_KEY_COLOUR_HANDS);
	
	if(colour_background || colour_hands) {
		graphics_context_set_fill_color(ctx, GColorFromHEX(colour_hands));
		graphics_context_set_stroke_color(ctx, GColorFromHEX(colour_background));
	} else {
		graphics_context_set_fill_color(ctx, GColorWhite);
		graphics_context_set_stroke_color(ctx, GColorBlack);
	}
	
	gpath_rotate_to(s_hour_arrow, (TRIG_MAX_ANGLE * (((tick_time->tm_hour % 12) * 6) + (tick_time->tm_min / 10))) / (12 * 6)); // TRIG_MAX_ANGLE * 50 / 60);
	gpath_draw_filled(ctx, s_hour_arrow);
	gpath_draw_outline(ctx, s_hour_arrow);

	gpath_rotate_to(s_minute_arrow, TRIG_MAX_ANGLE * tick_time->tm_min / 60); //8 / 60);
	gpath_draw_filled(ctx, s_minute_arrow);
	gpath_draw_outline(ctx, s_minute_arrow);

// Dot
// 	graphics_context_set_fill_color(ctx, GColorBlack);
// 	graphics_fill_circle(ctx, GPoint(bounds.size.w / 2,bounds.size.h / 2), 11);
}

static void date_update_proc(Layer *layer, GContext *ctx) {
	time_t now = time(NULL);
	struct tm *tick_time = localtime(&now);
	
	static char date_current[16];
	strftime(date_current, 80, "%d%m", tick_time);
	
	int toggle_suffix = persist_read_int(MESSAGE_KEY_TOGGLE_SUFFIX);	

// Weekday
	strftime(s_weekday_buffer, sizeof(s_weekday_buffer), "%A", tick_time);
	text_layer_set_text(s_weekday_label, s_weekday_buffer);

// Month
	char char_suffix[32] = "";
	strcat(char_suffix,"%e");
	if(toggle_suffix == 1) {
		getSuffix(char_suffix, date_current);
	}	
	getMonth(char_suffix, 0, tick_time);
	
	strftime(s_month_buffer, sizeof(s_month_buffer), char_suffix, tick_time);
	text_layer_set_text(s_month_label, s_month_buffer);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////// Other ///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
	layer_mark_dirty(window_get_root_layer(s_main_window));
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	s_background_layer = layer_create(bounds);
	layer_set_update_proc(s_background_layer, bg_update_proc);
	layer_add_child(window_layer, s_background_layer);

	s_date_layer = layer_create(bounds);
	layer_set_update_proc(s_date_layer, date_update_proc);
	layer_add_child(window_layer, s_date_layer);

// Sharingan
	s_sharingan_layer = bitmap_layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
	bitmap_layer_set_bitmap(s_sharingan_layer, gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SHARINGAN_FULL));
	layer_mark_dirty(bitmap_layer_get_layer(s_sharingan_layer));
	#if defined(PBL_COLOR)
		bitmap_layer_set_compositing_mode(s_sharingan_layer, GCompOpSet);	
	#endif
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_sharingan_layer));
	
// Weekday
	s_weekday_label = text_layer_create(GRect(0, 0, bounds.size.w, 25)); // Top
	text_layer_set_text_alignment(s_weekday_label, GTextAlignmentCenter);
	text_layer_set_background_color(s_weekday_label, GColorClear);
	text_layer_set_font(s_weekday_label, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_NARUTO_15)));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weekday_label));
	text_layer_set_text(s_weekday_label, s_weekday_buffer);
	
// Month
	s_month_label = text_layer_create(GRect(0, bounds.size.h - 16, bounds.size.w, 16)); // Bottom
	text_layer_set_text_alignment(s_month_label, GTextAlignmentCenter);
	text_layer_set_background_color(s_month_label, GColorClear);
	text_layer_set_font(s_month_label, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_NARUTO_15)));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_month_label));
	text_layer_set_text(s_month_label, s_month_buffer);
	
// Set Colours
	int colour_background = persist_read_int(MESSAGE_KEY_COLOUR_BACKGROUND);
	int colour_weekday = persist_read_int(MESSAGE_KEY_COLOUR_WEEKDAY);
	int colour_month = persist_read_int(MESSAGE_KEY_COLOUR_MONTH);
	if(colour_background || colour_weekday || colour_month) {
		text_layer_set_text_color(s_weekday_label, GColorFromHEX(colour_weekday));
		text_layer_set_text_color(s_month_label, GColorFromHEX(colour_month));
	} else {
		text_layer_set_text_color(s_weekday_label, GColorWhite);
		text_layer_set_text_color(s_month_label, GColorWhite);
	}
	
// Hands
	s_hands_layer = layer_create(bounds);
	layer_set_update_proc(s_hands_layer, hands_update_proc);
	layer_add_child(window_layer, s_hands_layer);
}

static void window_unload(Window *window) {
	layer_destroy(s_background_layer);
	layer_destroy(s_hands_layer);

	text_layer_destroy(s_weekday_label);
	text_layer_destroy(s_month_label);
}

static void init() {
	s_main_window = window_create();
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	window_stack_push(s_main_window, true);
	
	s_weekday_buffer[0] = '\0';
	s_month_buffer[0] 	= '\0';

// init hand paths
	s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
	s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);

	Layer *window_layer = window_get_root_layer(s_main_window);
	GRect bounds = layer_get_bounds(window_layer);
	GPoint center = grect_center_point(&bounds);
	gpath_move_to(s_minute_arrow, center);
	gpath_move_to(s_hour_arrow, center);
	
	for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
		s_tick_paths[i] = gpath_create(&ANALOG_BG_POINTS[i]);
	}
	
	const int inbox_size = 128;
	const int outbox_size = 128;
	app_message_register_inbox_received(inbox_received_handler);
	app_message_open(inbox_size, outbox_size);
	
	tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
}

static void deinit() {
	gpath_destroy(s_minute_arrow);
	gpath_destroy(s_hour_arrow);
	bitmap_layer_destroy(s_sharingan_layer);

	for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
		gpath_destroy(s_tick_paths[i]);
	}

	tick_timer_service_unsubscribe();
	window_destroy(s_main_window);
}

int main() {
	init();
	app_event_loop();
	deinit();
}
