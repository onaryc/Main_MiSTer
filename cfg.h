// cfg.h
// 2015, rok.krajnc@gmail.com
// 2017+, Sorgelig

#ifndef __CFG_H__
#define __CFG_H__

#include <inttypes.h>

//// type definitions ////
typedef struct {
	uint32_t keyrah_mode;
	uint8_t forced_scandoubler;
	uint8_t key_menu_as_rgui;
	uint8_t reset_combo;
	uint8_t ypbpr;
	uint8_t csync;
	uint8_t vga_scaler;
	uint8_t vga_sog;
	uint8_t hdmi_audio_96k;
	uint8_t dvi;
	uint8_t hdmi_limited;
	uint8_t direct_video;
	uint8_t video_info;
	uint8_t refresh_min;
	uint8_t refresh_max;
	uint8_t controller_info;
	uint8_t vsync_adjust;
	uint8_t kbd_nomouse;
	uint8_t mouse_throttle;
	uint8_t bootscreen;
	uint8_t vscale_mode;
	uint16_t vscale_border;
	uint8_t rbf_hide_datecode;
	uint8_t menu_pal;
	int16_t bootcore_timeout;
	uint8_t fb_size;
	uint8_t fb_terminal;
	uint8_t osd_rotate;
	uint16_t osd_timeout;
	uint8_t gamepad_defaults;
	uint8_t recents;
	uint16_t jamma_vid;
	uint16_t jamma_pid;
	uint16_t no_merge_vid;
	uint16_t no_merge_pid;
	uint8_t sniper_mode;
	uint8_t browse_expand;
	uint8_t logo;
	char bootcore[256];
	char video_conf[1024];
	char video_conf_pal[1024];
	char video_conf_ntsc[1024];
	char font[1024];
	char shared_folder[1024];
	char custom_aspect_ratio[2][16];
} cfg_t;

typedef enum
{
	UINT8 = 0, INT8, UINT16, INT16, UINT32, INT32, FLOAT, STRING
} ini_vartypes_t;

typedef enum
{
	VIDEO_DIGITAL_IDX = 0, VIDEO_ANALOG_IDX, UI_IDX, AUDIO_IDX, INPUT_IDX, 
	STORAGE_IDX, NETWORK_IDX, UPDATE_IDX, MISC_IDX
} ini_cfgcat_idx_t;

typedef struct 
{
	const char* name;
	ini_cfgcat_idx_t id;
} ini_cfgcat_t;

typedef struct
{
	const char* name;
	void* var;
	ini_vartypes_t type;
	int min;
	int max;
	unsigned int category = 0;
	int dismenu = 0;
	const char ** prettyp_values;
	const char * prettyp_name;
} ini_var_t;

extern cfg_t cfg;

//// functions ////
void cfg_parse();
const char* cfg_get_name(uint8_t alt);

void cfgvar_to_str (char * res, unsigned int i);
void cfgval_to_str (char * res, unsigned int i);
int display_var (unsigned int i, unsigned int category);
unsigned int get_nvars ();
char * get_cfgcat(unsigned int i);
unsigned int get_ncfgcats();

#endif // __CFG_H__
