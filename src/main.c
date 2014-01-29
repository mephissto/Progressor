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
TextLayer *layer_hour_real;
TextLayer *layer_day_real;
TextLayer *layer_week_real;
TextLayer *layer_month_real;
TextLayer *layer_year_real;

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

char* floatToString(char* buffer, int bufferSize, double number);

char* intToString(char* buffer, int bufferSize, int number);

void handle_init(void)
{
	time_t now = time(NULL);
	t = localtime(&now);
	
	my_window = window_create();
	window_set_fullscreen(my_window, true);
	window_set_background_color(my_window, BACKGROUND_COLOR);
	
	layer_hour	= text_layer_create(GRect(0, 0, 40, 16));
	layer_day	= text_layer_create(GRect(0, 32, 40, 16));
	layer_week	= text_layer_create(GRect(0, 64, 40, 16));
	layer_month	= text_layer_create(GRect(0, 96, 40, 16));
	layer_year	= text_layer_create(GRect(0, 128, 40, 16));
	
	layer_hour_real	= text_layer_create(GRect(40, 0, 64, 16));
	layer_day_real	= text_layer_create(GRect(40, 32, 64, 16));
	layer_week_real	= text_layer_create(GRect(40, 64, 64, 16));
	layer_month_real= text_layer_create(GRect(40, 96, 64, 16));
	layer_year_real	= text_layer_create(GRect(40, 128, 64, 16));
	
	layer_hour_percent	= text_layer_create(GRect(104, 0, 40, 16));
	layer_day_percent	= text_layer_create(GRect(104, 32, 40, 16));
	layer_week_percent	= text_layer_create(GRect(104, 64, 40, 16));
	layer_month_percent	= text_layer_create(GRect(104, 96, 40, 16));
	layer_year_percent	= text_layer_create(GRect(104, 128, 40, 16));
	
	text_layer_set_text_alignment(layer_hour_real, GTextAlignmentCenter);
	text_layer_set_text_alignment(layer_day_real, GTextAlignmentCenter);
	text_layer_set_text_alignment(layer_week_real, GTextAlignmentCenter);
	text_layer_set_text_alignment(layer_month_real, GTextAlignmentCenter);
	text_layer_set_text_alignment(layer_year_real, GTextAlignmentCenter);
	
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
	
	text_layer_set_font(layer_hour_real, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_font(layer_day_real, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_font(layer_week_real, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_font(layer_month_real, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_font(layer_year_real, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	
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
	
	layer_add_child(layer_root, (Layer*) layer_hour_real);
	layer_add_child(layer_root, (Layer*) layer_day_real);
	layer_add_child(layer_root, (Layer*) layer_week_real);
	layer_add_child(layer_root, (Layer*) layer_month_real);
	layer_add_child(layer_root, (Layer*) layer_year_real);
	
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

void handle_deinit(void)
{
	text_layer_destroy(layer_hour);
	text_layer_destroy(layer_day);
	text_layer_destroy(layer_week);
	text_layer_destroy(layer_month);
	text_layer_destroy(layer_year);
	
	text_layer_destroy(layer_hour_real);
	text_layer_destroy(layer_day_real);
	text_layer_destroy(layer_week_real);
	text_layer_destroy(layer_month_real);
	text_layer_destroy(layer_year_real);
	
	text_layer_destroy(layer_hour_percent);
	text_layer_destroy(layer_day_percent);
	text_layer_destroy(layer_week_percent);
	text_layer_destroy(layer_month_percent);
	text_layer_destroy(layer_year_percent);
	
	window_destroy(my_window);
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}

static void handle_time_tick(struct tm* tick_time, TimeUnits units_changed)
{
	static char time_text[] = "00:00";
    char *time_format;
	if (clock_is_24h_style())
		time_format = "%R";
	else 
		time_format = "%I:%M";
	strftime(time_text, sizeof(time_text), time_format, tick_time);
	text_layer_set_text(layer_hour_real, time_text);
	
	static char date_str[6];
	strcpy(date_str, "");
	strcat(date_str, intToString("31", 8, t->tm_mday));
	text_layer_set_text(layer_day_real, date_str);
	
	static char weekday_name[10];
	strcpy(weekday_name, "");
	switch(t->tm_wday)
	{
		case 0:
			strcat(weekday_name, "Sunday");
			break;
		case 1:
			strcat(weekday_name, "Monday");
			break;
		case 2:
			strcat(weekday_name, "Tuesday");
			break;
		case 3:
			strcat(weekday_name, "Wednesday");
			break;
		case 4:
			strcat(weekday_name, "Thursday");
			break;
		case 5:
			strcat(weekday_name, "Friday");
			break;
		case 6:
			strcat(weekday_name, "Saturday");
			break;
		default:
			strcat(weekday_name, "Errorday");
			break;
	}
	text_layer_set_text(layer_week_real, weekday_name);
	static char month_name[10];
	strcpy(month_name, "");
	switch(t->tm_mon)
	{
		case 0:
			strcat(month_name, "January");
			break;
		case 1:
			strcat(month_name, "February");
			break;
		case 2:
			strcat(month_name, "March");
			break;
		case 3:
			strcat(month_name, "April");
			break;
		case 4:
			strcat(month_name, "May");
			break;
		case 5:
			strcat(month_name, "June");
			break;
		case 6:
			strcat(month_name, "July");
			break;
		case 7:
			strcat(month_name, "August");
			break;
		case 8:
			strcat(month_name, "September");
			break;
		case 9:
			strcat(month_name, "October");
			break;
		case 10:
			strcat(month_name, "November");
			break;
		case 11:
			strcat(month_name, "December");
			break;
		default:
			strcat(month_name, "Errorember");
			break;
	}
	text_layer_set_text(layer_month_real, month_name);
		
	static char year_str[8];
	strcpy(year_str, "");
	strcat(year_str, intToString("2000", 8, t->tm_year + 1900));
	text_layer_set_text(layer_year_real, year_str);	
	
	hourDecimal = ((double)(t->tm_min) / (double)60);
	dayDecimal = ((double)((t->tm_hour * 60) + t->tm_min) / (double)1440);
	weekDecimal = ((double)((t->tm_wday * 1440) + (t->tm_hour * 60) + t->tm_min) / (double)10080);
	monthDecimal = ((double)(((t->tm_mday - 1) * 1440) + (t->tm_hour * 60) + t->tm_min) / (double)(daysInMonth(t->tm_mon, t->tm_year) * 1440));
	yearDecimal = ((double)(((t->tm_yday - 1) * 1440) + (t->tm_hour * 60) + t->tm_min) / (double)(isLeapYear(t->tm_year + 1900) ? 366 * 1440: 365 * 1440));
	
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
	//Hour
	static char percentHour[100];
	strcpy(percentHour, "");
	strcat(percentHour, floatToString("100.00%", 7, 100 * hourDecimal));
	strcat(percentHour, "%");
	text_layer_set_text(layer_hour_percent, percentHour);
	
	//Day
	static char percentDay[100];
	strcpy(percentDay, "");
	strcat(percentDay, floatToString("100.00%", 7, 100 * dayDecimal));
	strcat(percentDay, "%");
	text_layer_set_text(layer_day_percent, percentDay);
	
	//Week
	static char percentWeek[100];
	strcpy(percentWeek, "");
	strcat(percentWeek, floatToString("100.00%", 7, 100 * weekDecimal));
	strcat(percentWeek, "%");
	text_layer_set_text(layer_week_percent, percentWeek);
	
	//Month
	static char percentMonth[100];
	strcpy(percentMonth, "");
	strcat(percentMonth, floatToString("100.00%", 7, 100 * monthDecimal));
	strcat(percentMonth, "%");
	text_layer_set_text(layer_month_percent, percentMonth);
	
	//Year
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

char* intToString(char* buffer, int bufferSize, int number)
{
	char decimalBuffer[5];
	
	snprintf(buffer, bufferSize, "%d", (int)number);
	
	return buffer;
}