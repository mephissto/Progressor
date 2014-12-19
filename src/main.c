#include <pebble.h>
	
#define BACKGROUND_COLOR GColorWhite
#define FOREGROUND_COLOR GColorBlack
#define ACTUAL_ALIGN GTextAlignmentCenter
#define AM_PM_ALIGN GTextAlignmentRight
#define FONT_TINY RESOURCE_ID_FONT_NUMBERS_8		//use sparingly
#define FONT_SMALL RESOURCE_ID_FONT_NUMBERS_18
#define FONT_LARGE RESOURCE_ID_FONT_NUMBERS_40
	
Window *my_window;

//updating progression bars
InverterLayer *layer_hour;
InverterLayer *layer_day;
InverterLayer *layer_week;
InverterLayer *layer_month;
InverterLayer *layer_year;

//actual time values
TextLayer *layer_hour_actual;
TextLayer *layer_day_actual;
TextLayer *layer_week_actual;
TextLayer *layer_month_actual;
TextLayer *layer_year_actual;
TextLayer *layer_am_pm;

static Layer *layer_bar;
Layer *layer_root;

static struct tm *t;

//value percentages
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

char* intToString(char* buffer, int bufferSize, int number);

void handle_init(void)
{
	time_t now = time(NULL);
	t = localtime(&now);
	
	my_window = window_create();
	window_set_fullscreen(my_window, true);
	window_set_background_color(my_window, BACKGROUND_COLOR);
	
	//set progress bar locations
	layer_hour = inverter_layer_create(GRect(0, 61, 144, 47));
	layer_day = inverter_layer_create(GRect(0, 31, 144, 30));
	layer_week = inverter_layer_create(GRect(0, 109, 144, 30));
	layer_month = inverter_layer_create(GRect(0, 0, 144, 30));
	layer_year = inverter_layer_create(GRect(0, 139, 144, 30));
	
	//set actual value locations
	layer_hour_actual	= text_layer_create(GRect(0, 58, 144, 50));
	layer_day_actual	= text_layer_create(GRect(0, 32, 144, 30));
	layer_week_actual	= text_layer_create(GRect(0, 112, 144, 30));
	layer_month_actual	= text_layer_create(GRect(0, 2, 144, 30));
	layer_year_actual	= text_layer_create(GRect(0, 142, 144, 30));
	layer_am_pm			= text_layer_create(GRect(122, 99, 20, 15));
	
	text_layer_set_background_color(layer_hour_actual, GColorClear);
	text_layer_set_background_color(layer_day_actual, GColorClear);
	text_layer_set_background_color(layer_week_actual, GColorClear);
	text_layer_set_background_color(layer_month_actual, GColorClear);
	text_layer_set_background_color(layer_year_actual, GColorClear);
	text_layer_set_background_color(layer_am_pm, GColorClear);
	
	text_layer_set_text_color(layer_hour_actual, FOREGROUND_COLOR);
	text_layer_set_text_color(layer_day_actual, FOREGROUND_COLOR);
	text_layer_set_text_color(layer_week_actual, FOREGROUND_COLOR);
	text_layer_set_text_color(layer_month_actual, FOREGROUND_COLOR);
	text_layer_set_text_color(layer_year_actual, FOREGROUND_COLOR);
	text_layer_set_text_color(layer_am_pm, FOREGROUND_COLOR);
	
	//load fonts
	text_layer_set_font(layer_hour_actual, fonts_load_custom_font(resource_get_handle(FONT_LARGE)));
	text_layer_set_font(layer_day_actual, fonts_load_custom_font(resource_get_handle(FONT_SMALL)));
	text_layer_set_font(layer_week_actual, fonts_load_custom_font(resource_get_handle(FONT_SMALL)));
	text_layer_set_font(layer_month_actual, fonts_load_custom_font(resource_get_handle(FONT_SMALL)));
	text_layer_set_font(layer_year_actual, fonts_load_custom_font(resource_get_handle(FONT_SMALL)));
	text_layer_set_font(layer_am_pm, fonts_load_custom_font(resource_get_handle(FONT_TINY)));
	
	text_layer_set_text_alignment(layer_hour_actual, ACTUAL_ALIGN);
	text_layer_set_text_alignment(layer_day_actual, ACTUAL_ALIGN);
	text_layer_set_text_alignment(layer_week_actual, ACTUAL_ALIGN);
	text_layer_set_text_alignment(layer_month_actual, ACTUAL_ALIGN);
	text_layer_set_text_alignment(layer_year_actual, ACTUAL_ALIGN);
	text_layer_set_text_alignment(layer_am_pm, AM_PM_ALIGN);
	
	layer_root = window_get_root_layer(my_window);
	layer_bar = layer_create(layer_get_frame(layer_root));
	layer_add_child(layer_root, layer_bar);
	
	layer_add_child(layer_root, (Layer*) layer_hour_actual);
	layer_add_child(layer_root, (Layer*) layer_day_actual);
	layer_add_child(layer_root, (Layer*) layer_week_actual);
	layer_add_child(layer_root, (Layer*) layer_month_actual);
	layer_add_child(layer_root, (Layer*) layer_year_actual);
	layer_add_child(layer_root, (Layer*) layer_am_pm);
	
	layer_add_child(layer_root, (Layer*) layer_hour);
	layer_add_child(layer_root, (Layer*) layer_day);
	layer_add_child(layer_root, (Layer*) layer_week);
	layer_add_child(layer_root, (Layer*) layer_month);
	layer_add_child(layer_root, (Layer*) layer_year);
	
	//update values every minute
	tick_timer_service_subscribe(MINUTE_UNIT, &handle_time_tick);
	
	handle_time_tick(t, MINUTE_UNIT);
	
	//update screen
	layer_set_update_proc(layer_bar, bar_layer_draw);
	layer_mark_dirty(layer_bar);
	
	window_stack_push(my_window, true);
}

void handle_deinit(void)
{
	inverter_layer_destroy(layer_hour);
	inverter_layer_destroy(layer_day);
	inverter_layer_destroy(layer_week);
	inverter_layer_destroy(layer_month);
	inverter_layer_destroy(layer_year);
	
	text_layer_destroy(layer_hour_actual);
	text_layer_destroy(layer_day_actual);
	text_layer_destroy(layer_week_actual);
	text_layer_destroy(layer_month_actual);
	text_layer_destroy(layer_year_actual);
	
	layer_destroy(layer_bar);
	
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
	
	//update actual time value
	if (clock_is_24h_style())
	{
		time_format = "%R";
		text_layer_set_text(layer_am_pm, "");
	}
	else
	{
		static char ampm_text[] = "";
		strftime(ampm_text, sizeof(ampm_text), "%p", tick_time);
		
		if(t->tm_hour < 12)
			strncpy(ampm_text, "am", sizeof("am"));
		else if(t->tm_hour >= 12)
			strncpy(ampm_text, "pm", sizeof("pm"));
		else
			strncpy(ampm_text, "er", sizeof("er"));

		text_layer_set_text(layer_am_pm, ampm_text);
		
		time_format = "%I:%M";
	}
	strftime(time_text, sizeof(time_text), time_format, tick_time);
	text_layer_set_text(layer_hour_actual, time_text);
	
	//update actual date value
	static char date_str[6];
	strcpy(date_str, "");
	strcat(date_str, intToString("31", 8, t->tm_mday));
	text_layer_set_text(layer_day_actual, date_str);
	
	//update actual weekday value
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
	text_layer_set_text(layer_week_actual, weekday_name);
	
	//update actual month value
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
	text_layer_set_text(layer_month_actual, month_name);
	
	//update actual year value
	static char year_str[8];
	strcpy(year_str, "");
	strcat(year_str, intToString("2000", 8, t->tm_year + 1900));
	text_layer_set_text(layer_year_actual, year_str);	
	
	//caculate exact percentages
	hourDecimal = ((double)(t->tm_min) / (double)60);
	dayDecimal = ((double)((t->tm_hour * 60) + t->tm_min) / (double)1440);
	weekDecimal = ((double)((t->tm_wday * 1440) + (t->tm_hour * 60) + t->tm_min) / (double)10080);
	monthDecimal = ((double)(((t->tm_mday - 1) * 1440) + (t->tm_hour * 60) + t->tm_min) / (double)(daysInMonth(t->tm_mon, t->tm_year) * 1440));
	yearDecimal = ((double)(((t->tm_yday - 1) * 1440) + (t->tm_hour * 60) + t->tm_min) / (double)(isLeapYear(t->tm_year + 1900) ? 366 * 1440: 365 * 1440));

	layer_mark_dirty(layer_bar);
}

static void bar_layer_draw(Layer *layer, GContext *ctx)
{
	graphics_context_set_stroke_color(ctx, FOREGROUND_COLOR);
	graphics_context_set_text_color(ctx, FOREGROUND_COLOR);
	
	//Hour
	graphics_draw_rect(ctx, GRect(0, 60, 144, 50));
	layer_set_frame(inverter_layer_get_layer(layer_hour), GRect(1, 61, 142 * hourDecimal, 48));
	
	//Day
	graphics_draw_rect(ctx, GRect(0, 30, 144, 30));
	layer_set_frame(inverter_layer_get_layer(layer_day), GRect(1, 31, 142 * dayDecimal, 28));
	
	//Week
	graphics_draw_rect(ctx, GRect(0, 110, 144, 30));
	layer_set_frame(inverter_layer_get_layer(layer_week), GRect(1, 111, 142 * weekDecimal, 28));
	
	//Month
	graphics_draw_rect(ctx, GRect(0, 0, 144, 30));
	layer_set_frame(inverter_layer_get_layer(layer_month), GRect(1, 1, 142 * monthDecimal, 28));
	
	//Year
	graphics_draw_rect(ctx, GRect(0, 140, 144, 28));
	layer_set_frame(inverter_layer_get_layer(layer_year), GRect(1, 141, 142 * yearDecimal, 26));	
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

//returns true if given year is a leap year
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

char* intToString(char* buffer, int bufferSize, int number)
{
	char decimalBuffer[5];
	
	snprintf(buffer, bufferSize, "%d", (int)number);
	
	return buffer;
}