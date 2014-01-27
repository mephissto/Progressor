#include <pebble.h>
	
#define BACKGROUND_COLOR GColorWhite
#define FOREGROUND_COLOR GColorBlack
#define CORNER_MASK GCornerNone
#define CORNER_SIZE 0
	
Window *my_window;
TextLayer *layer_hour;
TextLayer *layer_day;
TextLayer *layer_week;
TextLayer *layer_month;
TextLayer *layer_year;
TextLayer *layer_hour_percent;
TextLayer *layer_day_percent;
TextLayer *layer_week_percent;
TextLayer *layer_month_percent;
TextLayer *layer_year_percent;

static Layer *layer_bar;
Layer *layer_root;

static struct tm *t;

static float hourDecimal;
static float dayDecimal;
static float weekDecimal;
static float monthDecimal;
static float yearDecimal;

static void handle_time_tick(struct tm* tick_time, TimeUnits units_changed);

static void bar_layer_draw(Layer *layer, GContext *ctx);
void text_layer_update();

int daysInMonth(int month, int year);
bool isLeapYear(int year);

char *itoa(int num);
char* floatToString(char* buffer, int bufferSize, double number);

void handle_init(void)
{
	time_t now = time(NULL);
	t = localtime(&now);
	
	my_window = window_create();
	window_set_fullscreen(my_window, true);
	window_set_background_color(my_window, BACKGROUND_COLOR);
	
	layer_hour	= text_layer_create(GRect(0, 0, 72, 16));
	layer_day	= text_layer_create(GRect(0, 32, 72, 16));
	layer_week	= text_layer_create(GRect(0, 64, 72, 16));
	layer_month	= text_layer_create(GRect(0, 96, 72, 16));
	layer_year	= text_layer_create(GRect(0, 128, 72, 16));
	
	layer_hour_percent	= text_layer_create(GRect(72, 0, 72, 16));
	layer_day_percent	= text_layer_create(GRect(72, 32, 72, 16));
	layer_week_percent	= text_layer_create(GRect(72, 64, 72, 16));
	layer_month_percent	= text_layer_create(GRect(72, 96, 72, 16));
	layer_year_percent	= text_layer_create(GRect(72, 128, 72, 16));
	
	text_layer_set_text_alignment(layer_hour_percent, GTextAlignmentRight);
	text_layer_set_text_alignment(layer_day_percent, GTextAlignmentRight);
	text_layer_set_text_alignment(layer_week_percent, GTextAlignmentRight);
	text_layer_set_text_alignment(layer_month_percent, GTextAlignmentRight);
	text_layer_set_text_alignment(layer_year_percent, GTextAlignmentRight);
	
	text_layer_set_text(layer_hour,		"Hour:");
	text_layer_set_text(layer_day,		"Day:");
	text_layer_set_text(layer_week,		"Week:");
	text_layer_set_text(layer_month,	"Month:");
	text_layer_set_text(layer_year,		"Year:");
	
	text_layer_set_font(layer_hour, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_font(layer_day, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_font(layer_week, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_font(layer_month, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_font(layer_year, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	
	text_layer_set_font(layer_hour_percent, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_font(layer_day_percent, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_font(layer_week_percent, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_font(layer_month_percent, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_font(layer_year_percent, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	
	layer_root = window_get_root_layer(my_window);
	layer_bar = layer_create(layer_get_frame(layer_root));
	layer_add_child(layer_root, layer_bar);
	
	layer_add_child(layer_root, (Layer*) layer_hour);
	layer_add_child(layer_root, (Layer*) layer_day);
	layer_add_child(layer_root, (Layer*) layer_week);
	layer_add_child(layer_root, (Layer*) layer_month);
	layer_add_child(layer_root, (Layer*) layer_year);
	
	layer_add_child(layer_root, (Layer*) layer_hour_percent);
	layer_add_child(layer_root, (Layer*) layer_day_percent);
	layer_add_child(layer_root, (Layer*) layer_week_percent);
	layer_add_child(layer_root, (Layer*) layer_month_percent);
	layer_add_child(layer_root, (Layer*) layer_year_percent);
	
	
	tick_timer_service_subscribe(MINUTE_UNIT, &handle_time_tick);
	
	handle_time_tick(t, MINUTE_UNIT);
	
	layer_set_update_proc(layer_bar, bar_layer_draw);
	layer_mark_dirty(layer_bar);
	
	window_stack_push(my_window, true);
	
}

void handle_deinit(void) {
	text_layer_destroy(layer_hour);
	text_layer_destroy(layer_day);
	text_layer_destroy(layer_week);
	text_layer_destroy(layer_month);
	text_layer_destroy(layer_year);
	window_destroy(my_window);
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}

static void handle_time_tick(struct tm* tick_time, TimeUnits units_changed)
{
	hourDecimal = ((double)(t->tm_min) / (double)60);
	dayDecimal = ((double)((t->tm_hour * 60) + t->tm_min) / (double)1440);
	weekDecimal = ((double)((t->tm_wday * 1440) + (t->tm_hour * 60) + t->tm_min) / (double)10080);
	monthDecimal = ((double)(((t->tm_mday - 1) * 1440) + (t->tm_hour * 60) + t->tm_min) / (double)(daysInMonth(t->tm_mon, t->tm_year) * 1440));
	yearDecimal = ((double)(((t->tm_yday - 1) * 1440) + (t->tm_hour * 60) + t->tm_min) / (double)(isLeapYear(t->tm_year) ? 366 * 1440: 365 * 1440));
	
	text_layer_update();	
	layer_mark_dirty(layer_bar);
}

static void bar_layer_draw(Layer *layer, GContext *ctx)
{
	graphics_context_set_stroke_color(ctx, FOREGROUND_COLOR);
	graphics_context_set_fill_color(ctx, FOREGROUND_COLOR);
	graphics_context_set_text_color(ctx, FOREGROUND_COLOR);
	
	//Hour
	graphics_draw_rect(ctx, GRect(0, 16, 144, 16));
	graphics_fill_rect(ctx, GRect(0, 16, 144 * hourDecimal, 16), CORNER_SIZE, CORNER_MASK);
	
	//Day
	graphics_draw_rect(ctx, GRect(0, 48, 144, 16));
	graphics_fill_rect(ctx, GRect(0, 48, 144 * dayDecimal, 16), CORNER_SIZE, CORNER_MASK);
	
	//Week
	graphics_draw_rect(ctx, GRect(0, 80, 144, 16));
	graphics_fill_rect(ctx, GRect(0, 80, 144 * weekDecimal, 16), CORNER_SIZE, CORNER_MASK);
	
	//Month
	graphics_draw_rect(ctx, GRect(0, 112, 144, 16));
	graphics_fill_rect(ctx, GRect(0, 112, 144 * monthDecimal, 16), CORNER_SIZE, CORNER_MASK);
	
	//Year
	graphics_draw_rect(ctx, GRect(0, 144, 144, 16));
	graphics_fill_rect(ctx, GRect(0, 144, 144 * yearDecimal, 16), CORNER_SIZE, CORNER_MASK);
}

void text_layer_update()
{
	
	static char percentHour[100];
	strcpy(percentHour, "");
	strcat(percentHour, floatToString("100.00%", 7, 100 * hourDecimal));
	strcat(percentHour, "%");
	text_layer_set_text(layer_hour_percent, percentHour);
	
	static char percentDay[100];
	strcpy(percentDay, "");
	strcat(percentDay, floatToString("100.00%", 7, 100 * dayDecimal));
	strcat(percentDay, "%");
	text_layer_set_text(layer_day_percent, percentDay);
	
	static char percentWeek[100];
	strcpy(percentWeek, "");
	strcat(percentWeek, floatToString("100.00%", 7, 100 * weekDecimal));
	strcat(percentWeek, "%");
	text_layer_set_text(layer_week_percent, percentWeek);
	
	static char percentMonth[100];
	strcpy(percentMonth, "");
	strcat(percentMonth, floatToString("100.00%", 7, 100 * monthDecimal));
	strcat(percentMonth, "%");
	text_layer_set_text(layer_month_percent, percentMonth);
	
	static char percentYear[100];
	strcpy(percentYear, "");
	strcat(percentYear, floatToString("100.00%", 7, 100 * yearDecimal));
	strcat(percentYear, "%");
	text_layer_set_text(layer_year_percent, percentYear);
	
	
}

int daysInMonth(int month, int year)
{
	if(month % 2 == 0)
		return 31;
	else
	{
		if(month == 1)
		{
			if(isLeapYear(year))
				return 29;
			else
				return 28;
		}
		else
			return 30;
	}
}

bool isLeapYear(int year)
{
	if(year % 400 == 0)
		return true;
	else
	{
		if(year % 100 == 0)
			return false;
		else
		{
			if(year % 4 == 0)
				return true;
			else
				return false;
		}
	}
}

char* floatToString(char* buffer, int bufferSize, double number)
{
	char decimalBuffer[5];
	
	snprintf(buffer, bufferSize, "%d", (int)number);
	strcat(buffer, ".");
	
	snprintf(decimalBuffer, 5, "%02d", (int)((double)(number - (int)number) * (double)100));
	strcat(buffer, decimalBuffer);
	
	return buffer;
}