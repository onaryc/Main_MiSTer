// cfg.c
// 2015, rok.krajnc@gmail.com
// 2017+, Sorgelig

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include "cfg.h"
#include "debug.h"
#include "file_io.h"
#include "user_io.h"

cfg_t cfg;

//typedef enum
//{
	//VIDEODIGITAL = 0, VIDEODIGITAL, UI, AUDIO, INPUT, MISC
//} ini_varcattypes_t;

const ini_cfgcat_t ini_cfgcats[] =
{
	{ "Video - Digital", VIDEO_DIGITAL_IDX },
	{ "Video - Analog", VIDEO_DIGITAL_IDX },
	{ "UI", UI_IDX },
	{ "Audio", AUDIO_IDX },
	{ "Input", INPUT_IDX },
	{ "Storage", STORAGE_IDX },
	{ "Network", NETWORK_IDX },
	{ "Update", UPDATE_IDX },
	{ "Misc Settings", MISC_IDX },
};

const unsigned int ncfgcats = (unsigned int)(sizeof(ini_cfgcats) / sizeof(ini_cfgcat_t));

const char* config_hdmi_audio_msg[] = { "48kHz/16-bit", "96kHz/16-bit" };
const char* config_reset_combo_msg[] = { "LCTL+LALT+RALT","LCTL+LGUI+RGUI","LCTL+LALT+DEL" };
const char* config_on_off_msg[] = { "Off", "On" };
const char* config_autoboot_core_msg[] = { "Menu", "Last" };
//const char* config_autoboot_timeout_msg[] = { "10", "15", "20", "25", "30" };
const char* config_display_vsync_msg[] = { "Compatible", "Balanced", "Low Latency" };
const char* config_hdmi_range_msg[] = { "Full(0-255)", "Limited(16-255)", "Limited(16-235)" };
const char* config_vertical_scale_msg[] = { "Fit", "Integer", "1/2 Step", "1/4 Step" };
const char* config_framebuffer_scale_msg[] = { "Auto", "Full", "Half", "Quarter" };
//const char* config_separate_ntsc_pal_msg[] = { "      Off","       On" };
const char* config_menu_mode_msg[] = { "NTSC", "PAL" };
const char* config_analog_display_mode_msg[] = {"Native(240p)", "Scaled(HDMI)" };
const char* config_scandoubler_msg[] = { "Off","On (240p->480p)" };
const char* config_color_mode_msg[] = { "RGB", "YPbPr" };
const char* config_sync_mode_msg[] = { "Off", "On Hsync" };
//const char* config_sync_mode_msg[] = { "Separate (RGBHV)", "Composite (RGBS)", "Green (RGsB/YPbPr)" };
const char* config_video_mode_msg[] = { "1280x720@60", "1024x768@60", "720x480@60", "720x576@50", "1280x1024@60", "800x600@60", "640x480@60", "1280x720@50",
	"1920x1080@60", "1920x1080@50", "1366x768@60", "1024x600@60", "1920x1440@60", "2048x1536@60" };
const char* config_osd_rotate_msg[] = { "No", "Right (+90)", "Left (-90)" };
const char* config_sniper_mode_msg[] = { "Slow", "Swapped" };

const ini_var_t ini_vars[] =
{
	{ "YPBPR", (void*)(&(cfg.ypbpr)), UINT8, 0, 1, 1, 1, config_color_mode_msg, "Color Mode" },
	{ "COMPOSITE_SYNC", (void*)(&(cfg.csync)), UINT8, 0, 1, 1, 1, config_sync_mode_msg, "Sync" },
	{ "FORCED_SCANDOUBLER", (void*)(&(cfg.forced_scandoubler)), UINT8, 0, 1, 1, 1, config_scandoubler_msg, "Scandoubler" },
	{ "VGA_SCALER", (void*)(&(cfg.vga_scaler)), UINT8, 0, 1, 1, 1, config_analog_display_mode_msg, "Display Mode" },
	{ "VGA_SOG", (void*)(&(cfg.vga_sog)), UINT8, 0, 1, 1, 1, {}, "" },
	{ "KEYRAH_MODE", (void*)(&(cfg.keyrah_mode)), UINT32, 0, (int)0xFFFFFFFF, 4, 0, {}, ""},
	{ "RESET_COMBO", (void*)(&(cfg.reset_combo)), UINT8, 0, 3, 8, 1, config_reset_combo_msg, "" },
	{ "KEY_MENU_AS_RGUI", (void*)(&(cfg.key_menu_as_rgui)), UINT8, 0, 1, 4, 1, config_on_off_msg, "Key Menu As RGUI (Minimig)" },
	{ "VIDEO_MODE", (void*)(cfg.video_conf), STRING, 0, sizeof(cfg.video_conf)-1 , 0, 1, config_video_mode_msg, "Display Mode" },
	{ "VIDEO_MODE_PAL", (void*)(cfg.video_conf_pal), STRING, 0, sizeof(cfg.video_conf_pal) - 1 , 0, 1, config_video_mode_msg, "PAL Mode" },
	{ "VIDEO_MODE_NTSC", (void*)(cfg.video_conf_ntsc), STRING, 0, sizeof(cfg.video_conf_ntsc) - 1, 0, 1, config_video_mode_msg, "NTSC Mode" },
	{ "VIDEO_INFO", (void*)(&(cfg.video_info)), UINT8, 0, 10, 0, 1, {}, "" },
	{ "VSYNC_ADJUST", (void*)(&(cfg.vsync_adjust)), UINT8, 0, 2, 0, 1, config_display_vsync_msg, "Vertical Sync" },
	{ "HDMI_AUDIO_96K", (void*)(&(cfg.hdmi_audio_96k)), UINT8, 0, 1, 3, 1, config_hdmi_audio_msg, "Hdmi Audio" },
	{ "DVI_MODE", (void*)(&(cfg.dvi)), UINT8, 0, 1, 0, 1, config_on_off_msg, "" },
	{ "HDMI_LIMITED", (void*)(&(cfg.hdmi_limited)), UINT8, 0, 2, 0, 1, config_hdmi_range_msg, "HDMI Range" },
	{ "KBD_NOMOUSE", (void*)(&(cfg.kbd_nomouse)), UINT8, 0, 1, 4, 1, {}, "" },
	{ "MOUSE_THROTTLE", (void*)(&(cfg.mouse_throttle)), UINT8, 1, 100, 4, 0, {}, "" },
	{ "BOOTSCREEN", (void*)(&(cfg.bootscreen)), UINT8, 0, 1, 8, 1, config_on_off_msg, "" },
	{ "VSCALE_MODE", (void*)(&(cfg.vscale_mode)), UINT8, 0, 3, 0, 1, config_vertical_scale_msg, "Vertical Scale" },
	{ "VSCALE_BORDER", (void*)(&(cfg.vscale_border)), UINT16, 0, 399, 0, 0, {}, "" },
	{ "RBF_HIDE_DATECODE", (void*)(&(cfg.rbf_hide_datecode)), UINT8, 0, 1, 2, 1, config_on_off_msg, "Hide Core Datecode" },
	{ "MENU_PAL", (void*)(&(cfg.menu_pal)), UINT8, 0, 1, 2, 1, config_menu_mode_msg, "Menu Mode" },
	{ "BOOTCORE", (void*)(&(cfg.bootcore)), STRING, 0, sizeof(cfg.bootcore) - 1, 8, 0, config_autoboot_core_msg, "" },
	{ "BOOTCORE_TIMEOUT", (void*)(&(cfg.bootcore_timeout)), INT16, 10, 30, 8, 0, {}, "" },
	{ "FONT", (void*)(&(cfg.font)), STRING, 0, sizeof(cfg.font) - 1, 2, 1, {}, "" },
	{ "FB_SIZE", (void*)(&(cfg.fb_size)), UINT8, 0, 4, 0, 0, config_framebuffer_scale_msg, "Framebuffer Scale" },
	{ "FB_TERMINAL", (void*)(&(cfg.fb_terminal)), UINT8, 0, 1, 0, 0, config_on_off_msg, "Framebuffer Terminal" },
	{ "OSD_TIMEOUT", (void*)(&(cfg.osd_timeout)), INT16, 5, 3600, 2, 1, {}, "" },
	{ "DIRECT_VIDEO", (void*)(&(cfg.direct_video)), UINT8, 0, 1, 1, 1, config_on_off_msg, "240p through HDMI" },
	{ "OSD_ROTATE", (void*)(&(cfg.osd_rotate)), UINT8, 0, 2, 2, 1, config_osd_rotate_msg, "" },
	{ "GAMEPAD_DEFAULTS", (void*)(&(cfg.gamepad_defaults)), UINT8, 0, 1, 4, 1, {}, "" },
	{ "RECENTS", (void*)(&(cfg.recents)), UINT8, 0, 1, 2, 1, config_on_off_msg, "" },
	{ "CONTROLLER_INFO", (void*)(&(cfg.controller_info)), UINT8, 0, 10, 4, 1, {}, "" },
	{ "REFRESH_MIN", (void*)(&(cfg.refresh_min)), UINT8, 0, 150, 0, 0, {}, "" },
	{ "REFRESH_MAX", (void*)(&(cfg.refresh_max)), UINT8, 0, 150, 0, 0, {}, ""  },
	{ "JAMMA_VID", (void*)(&(cfg.jamma_vid)), UINT16, 0, 0xFFFF, 4, 0, {}, ""  },
	{ "JAMMA_PID", (void*)(&(cfg.jamma_pid)), UINT16, 0, 0xFFFF, 4, 0, {}, ""  },
	{ "SNIPER_MODE", (void*)(&(cfg.sniper_mode)), UINT8, 0, 1, 4, 1, config_sniper_mode_msg, "" },
	{ "BROWSE_EXPAND", (void*)(&(cfg.browse_expand)), UINT8, 0, 1, 2, 1, config_on_off_msg, "2nd line for filename" },
	{ "LOGO", (void*)(&(cfg.logo)), UINT8, 0, 1, 2, 1, config_on_off_msg, "MiSTer Logo In Menu" },
	{ "SHARED_FOLDER", (void*)(&(cfg.shared_folder)), STRING, 0, sizeof(cfg.shared_folder) - 1, 5, 0, {}, "" },
	{ "NO_MERGE_VID", (void*)(&(cfg.no_merge_vid)), UINT16, 0, 0xFFFF, 4, 0, {}, "" },
	{ "NO_MERGE_PID", (void*)(&(cfg.no_merge_pid)), UINT16, 0, 0xFFFF, 4, 0, {}, "" },
	{ "CUSTOM_ASPECT_RATIO_1", (void*)(&(cfg.custom_aspect_ratio[0])), STRING, 0, sizeof(cfg.custom_aspect_ratio[0]) - 1, 0, 0, {}, "" },
	{ "CUSTOM_ASPECT_RATIO_2", (void*)(&(cfg.custom_aspect_ratio[1])), STRING, 0, sizeof(cfg.custom_aspect_ratio[1]) - 1, 0, 0, {}, "" },
};

const unsigned int nvars = (unsigned int)(sizeof(ini_vars) / sizeof(ini_var_t));

#define INI_EOT                 4 // End-Of-Transmission

#define INI_LINE_SIZE           256

#define INI_SECTION_START       '['
#define INI_SECTION_END         ']'
#define INI_SECTION_INVALID_ID  0

#define CHAR_IS_NUM(c)          (((c) >= '0') && ((c) <= '9'))
#define CHAR_IS_ALPHA_LOWER(c)  (((c) >= 'a') && ((c) <= 'z'))
#define CHAR_IS_ALPHA_UPPER(c)  (((c) >= 'A') && ((c) <= 'Z'))
#define CHAR_IS_ALPHANUM(c)     (CHAR_IS_ALPHA_LOWER(c) || CHAR_IS_ALPHA_UPPER(c) || CHAR_IS_NUM(c))
#define CHAR_IS_SPECIAL(c)      (((c) == '[') || ((c) == ']') || ((c) == '(') || ((c) == ')') || \
                                 ((c) == '-') || ((c) == '+') || ((c) == '/') || ((c) == '=') || \
                                 ((c) == '#') || ((c) == '$') || ((c) == '@') || ((c) == '_') || \
                                 ((c) == ',') || ((c) == '.') || ((c) == '!') || ((c) == '*') || \
                                 ((c) == ':'))

#define CHAR_IS_VALID(c)        (CHAR_IS_ALPHANUM(c) || CHAR_IS_SPECIAL(c))
#define CHAR_IS_SPACE(c)        (((c) == ' ') || ((c) == '\t'))
#define CHAR_IS_LINEEND(c)      (((c) == '\n'))
#define CHAR_IS_COMMENT(c)      (((c) == ';'))
#define CHAR_IS_QUOTE(c)        (((c) == '"'))


fileTYPE ini_file;

int ini_pt = 0;
static char ini_getch()
{
	static uint8_t buf[512];
	if (!(ini_pt & 0x1ff)) FileReadSec(&ini_file, buf);
	if (ini_pt >= ini_file.size) return 0;
	return buf[(ini_pt++) & 0x1ff];
}

static int ini_getline(char* line)
{
	char c, ignore = 0, skip = 1;
	int i = 0;

	while ((c = ini_getch()))
	{
		if (!CHAR_IS_SPACE(c)) skip = 0;
		if (i >= (INI_LINE_SIZE - 1) || CHAR_IS_COMMENT(c)) ignore = 1;

		if (CHAR_IS_LINEEND(c)) break;
		if (CHAR_IS_VALID(c) && !ignore && !skip) line[i++] = c;
	}
	line[i] = '\0';
	return c == 0 ? INI_EOT : 0;
}

static int ini_get_section(char* buf)
{
	int i = 0;

	// get section start marker
	if (buf[0] != INI_SECTION_START)
	{
		return INI_SECTION_INVALID_ID;
	}
	else buf++;

	int wc_pos = -1;

	// get section stop marker
	while (1)
	{
		if (buf[i] == INI_SECTION_END)
		{
			buf[i] = '\0';
			break;
		}

		if (buf[i] == '*') wc_pos = i;

		i++;
		if (i >= INI_LINE_SIZE) return INI_SECTION_INVALID_ID;
	}

	// convert to uppercase
	for (i = 0; i < INI_LINE_SIZE; i++)
	{
		if (!buf[i]) break;
		else buf[i] = toupper(buf[i]);
	}

	if (!strcasecmp(buf, "MiSTer") || ((wc_pos >= 0) ? !strncasecmp(buf, user_io_get_core_name_ex(), wc_pos) : !strcasecmp(buf, user_io_get_core_name_ex())))
	{
		ini_parser_debugf("Got SECTION '%s'", buf);
		return 1;
	}

	return INI_SECTION_INVALID_ID;
}

static void ini_parse_var(char* buf)
{
	int i = 0, j = 0;
	int var_id = -1;

	// find var
	while (1)
	{
		if (buf[i] == '=')
		{
			buf[i] = '\0';
			break;
		}
		else if (buf[i] == '\0') return;
		i++;
	}

	// convert to uppercase
	for (j = 0; j <= i; j++)
	{
		if (!buf[j]) break;
		else buf[j] = toupper(buf[j]);
	}

	// parse var
	for (j = 0; j < (int)nvars; j++)
	{
		if (!strcasecmp(buf, ini_vars[j].name)) var_id = j;
	}

	// get data
	if (var_id != -1)
	{
		ini_parser_debugf("Got VAR '%s' with VALUE %s", buf, &(buf[i + 1]));
		i++;
		switch (ini_vars[var_id].type)
		{
		case UINT8:
			*(uint8_t*)(ini_vars[var_id].var) = strtoul(&(buf[i]), NULL, 0);
			if (*(uint8_t*)(ini_vars[var_id].var) > ini_vars[var_id].max) *(uint8_t*)(ini_vars[var_id].var) = ini_vars[var_id].max;
			if (*(uint8_t*)(ini_vars[var_id].var) < ini_vars[var_id].min) *(uint8_t*)(ini_vars[var_id].var) = ini_vars[var_id].min;
			break;
		case INT8:
			*(int8_t*)(ini_vars[var_id].var) = strtol(&(buf[i]), NULL, 0);
			if (*(int8_t*)(ini_vars[var_id].var) > ini_vars[var_id].max) *(int8_t*)(ini_vars[var_id].var) = ini_vars[var_id].max;
			if (*(int8_t*)(ini_vars[var_id].var) < ini_vars[var_id].min) *(int8_t*)(ini_vars[var_id].var) = ini_vars[var_id].min;
			break;
		case UINT16:
			*(uint16_t*)(ini_vars[var_id].var) = strtoul(&(buf[i]), NULL, 0);
			if (*(uint16_t*)(ini_vars[var_id].var) > ini_vars[var_id].max) *(uint16_t*)(ini_vars[var_id].var) = ini_vars[var_id].max;
			if (*(uint16_t*)(ini_vars[var_id].var) < ini_vars[var_id].min) *(uint16_t*)(ini_vars[var_id].var) = ini_vars[var_id].min;
			break;
		case INT16:
			*(int16_t*)(ini_vars[var_id].var) = strtol(&(buf[i]), NULL, 0);
			if (*(int16_t*)(ini_vars[var_id].var) > ini_vars[var_id].max) *(int16_t*)(ini_vars[var_id].var) = ini_vars[var_id].max;
			if (*(int16_t*)(ini_vars[var_id].var) < ini_vars[var_id].min) *(int16_t*)(ini_vars[var_id].var) = ini_vars[var_id].min;
			break;
		case UINT32:
			*(uint32_t*)(ini_vars[var_id].var) = strtoul(&(buf[i]), NULL, 0);
			if (*(uint32_t*)(ini_vars[var_id].var) > (uint32_t)ini_vars[var_id].max) *(uint32_t*)(ini_vars[var_id].var) = ini_vars[var_id].max;
			if (*(uint32_t*)(ini_vars[var_id].var) < (uint32_t)ini_vars[var_id].min) *(uint32_t*)(ini_vars[var_id].var) = ini_vars[var_id].min;
			break;
		case INT32:
			*(int32_t*)(ini_vars[var_id].var) = strtol(&(buf[i]), NULL, 0);
			if (*(int32_t*)(ini_vars[var_id].var) > ini_vars[var_id].max) *(int32_t*)(ini_vars[var_id].var) = ini_vars[var_id].max;
			if (*(int32_t*)(ini_vars[var_id].var) < ini_vars[var_id].min) *(int32_t*)(ini_vars[var_id].var) = ini_vars[var_id].min;
			break;
		case FLOAT:
			*(float*)(ini_vars[var_id].var) = strtof(&(buf[i]), NULL);
			if (*(float*)(ini_vars[var_id].var) > ini_vars[var_id].max) *(float*)(ini_vars[var_id].var) = ini_vars[var_id].max;
			if (*(float*)(ini_vars[var_id].var) < ini_vars[var_id].min) *(float*)(ini_vars[var_id].var) = ini_vars[var_id].min;
			break;
		case STRING:
			memset(ini_vars[var_id].var, 0, ini_vars[var_id].max);
			strncpy((char*)(ini_vars[var_id].var), &(buf[i]), ini_vars[var_id].max);
			break;
		}
	}
}

static void ini_parse(int alt)
{
	char line[INI_LINE_SIZE] = { 0 };
	int section = INI_SECTION_INVALID_ID;
	int line_status;

	ini_parser_debugf("Start INI parser for core \"%s\".", user_io_get_core_name_ex());

	memset(&ini_file, 0, sizeof(ini_file));

	const char *name = cfg_get_name(alt);
	if (!FileOpen(&ini_file, name))	return;

	ini_parser_debugf("Opened file %s with size %llu bytes.", name, ini_file.size);

	ini_pt = 0;

	// parse ini
	while (1)
	{
		// get line
		line_status = ini_getline(line);
		ini_parser_debugf("line(%d): \"%s\".", line_status, line);

		if (line[0] == INI_SECTION_START)
		{
			// if first char in line is INI_SECTION_START, get section
			section = ini_get_section(line);
		}
		else if(section)
		{
			// otherwise this is a variable, get it
			ini_parse_var(line);
		}

		// if end of file, stop
		if (line_status == INI_EOT) break;
	}

	FileClose(&ini_file);
}

const char* cfg_get_name(uint8_t alt)
{
	static char name[64];
	strcpy(name, "MiSTer.ini");

	if (alt == 1)
	{
		strcpy(name, "MiSTer_alt_1.ini");
		if (FileExists(name)) return name;
		return "MiSTer_alt.ini";
	}

	if (alt && alt < 4) sprintf(name, "MiSTer_alt_%d.ini", alt);
	return name;
}

void cfg_parse()
{
	memset(&cfg, 0, sizeof(cfg));
	cfg.bootscreen = 1;
	cfg.fb_terminal = 1;
	cfg.controller_info = 6;
	cfg.browse_expand = 1;
	cfg.logo = 1;
	ini_parse(altcfg());
}

unsigned int get_var_cat (unsigned int i) {
	return ini_vars[i].category;
}

int get_var_dismenu (unsigned int i) {
	return ini_vars[i].dismenu;
}

int display_var (unsigned int i, unsigned int category) {
	if ((ini_vars[i].category == category) && (ini_vars[i].dismenu)) {
		return 1;
	} else {
		return 0;
	}
}

void cfgvar_to_str (char * res, unsigned int i) {
	if (strcmp(ini_vars[i].prettyp_name, "")) { // prettier name for the variable 
		strcpy(res, ini_vars[i].prettyp_name);
	} else { // no pretty name
		strcpy(res, ini_vars[i].name);
		
		// format the variable name
		*res = toupper(*res);
		for(char *i=res+1; *i; i++)
		{
			if (*i == '_') *i = ' ';
			else *i = tolower(*i);
		}
	}
}

void cfgval_to_str (char * res, unsigned int i) {
	if (ini_vars[i].prettyp_values != NULL) { // pretty value for the variable
		char* endc;
		long tmpValue;
		
		switch (ini_vars[i].type) {
			case UINT8:
				strcpy(res, ini_vars[i].prettyp_values[*(uint8_t*)ini_vars[i].var]);
				break;
			case INT8:
				strcpy(res, ini_vars[i].prettyp_values[*(int8_t*)ini_vars[i].var]);
				break;
			case UINT16:
				strcpy(res, ini_vars[i].prettyp_values[*(uint16_t*)ini_vars[i].var]);
				break;
			case INT16:
				strcpy(res, ini_vars[i].prettyp_values[*(int16_t*)ini_vars[i].var]);
				break;
			case UINT32:
				strcpy(res, ini_vars[i].prettyp_values[*(uint32_t*)ini_vars[i].var]);
				break;
			case INT32:
				strcpy(res, ini_vars[i].prettyp_values[*(int32_t*)ini_vars[i].var]);
				break;
			//case FLOAT: // not sure it make sense, float index
				//strcpy(res, ini_vars[i].prettyp_values[*(float*)ini_vars[i].var]);
				//break;
			case STRING: // var can contain string representing mixed string and numeric values (i.e. video mode)
				tmpValue = strtol((char *)ini_vars[i].var, &endc, 10);
				if (*endc) { // not a numeric value
					strcpy(res, (char *)ini_vars[i].var);
				} else { // numeric value
					strcpy(res, ini_vars[i].prettyp_values[tmpValue]);
				}
				break;
			default:
				strcpy(res, ini_vars[i].prettyp_values[*(uint8_t*)ini_vars[i].var]);
				break;
		}
	} else { // display the raw value
		switch (ini_vars[i].type) {
			case UINT8:
				sprintf(res, "%u", *(uint8_t*)ini_vars[i].var);
				break;
			case INT8:
				sprintf(res, "%i", *(int8_t*)ini_vars[i].var);
				break;
			case UINT16:
				sprintf(res, "%u", *(uint16_t*)ini_vars[i].var);
				break;
			case INT16:
				sprintf(res, "%i", *(int16_t*)ini_vars[i].var);
				break;
			case UINT32:
				sprintf(res, "%u", *(uint32_t*)ini_vars[i].var);
				break;
			case INT32:
				sprintf(res, "%i", *(int32_t*)ini_vars[i].var);
				break;
			case FLOAT:
				sprintf(res, "%f", *(float*)ini_vars[i].var);
				break;
			case STRING:
				strcpy(res, (char *)ini_vars[i].var);
				break;
		}
	}
}

unsigned int get_nvars () {
	return nvars;
}

char * get_cfgcat(unsigned int i) {
	return (char *)ini_cfgcats[i].name;
}

unsigned int get_ncfgcats() {
	return ncfgcats;
}
