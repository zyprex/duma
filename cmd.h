#pragma once

extern int prefix_number;

extern int offset_y_coord;
extern int offset_x_coord;

extern int trim_margin_left;
extern int trim_margin_right;
extern int trim_margin_top;
extern int trim_margin_bottom;

extern float zoom_factor;

/* overlap only affect dual split pages */
extern int overlap_gap;

extern int rotation_degree;

extern int show_file_info;

extern int auto_play_time;

void quit();
void refresh_new(HWND hwnd);
void refresh(HWND hwnd);
void trim_margin_on(HWND hwnd, int* where);
void trim_margin_all(HWND hwnd);
void move_offset_on(HWND hwnd, int* coord, int val);
void move_offset_reset(HWND hwnd);
void goto_page_part_begin(HWND hwnd);
void goto_page_part_end(HWND hwnd);
void goto_page_n(HWND hwnd);
void goto_page_part_n(HWND hwnd);
int page_next(HWND hwnd);
int page_prev(HWND hwnd);
void page_part_next(HWND hwnd);
void page_part_prev(HWND hwnd);
void save_tweaks();
void load_tweaks();
void save_last_file();
void read_last_file(HWND hwnd);
int toggle_fullscreen(HWND hwnd, int on);
void zoom_to(HWND hwnd, float* factor, float val);
void zoom_reset(HWND hwnd);
void overlap_gap_on(HWND hwnd, int* overlap, int val);
void overlap_gap_reset(HWND hwnd);
void rotate_to_degree(HWND hwnd, int degree);
void toggle_file_info(HWND hwnd);


#define TIMER_ID_FLASH_INFO 0 /* timer id for flash info */
#define TIMER_ID_AUTO_PLAY 1 /* timer id for auto next page */

void start_auto_play_timer(HWND hwnd, int ms);
void kill_auto_play_timer(HWND hwnd);
void start_flash_info_timer(HWND hwnd, int ms);
void kill_flash_info_timer(HWND hwnd);
VOID CALLBACK timer_proc(HWND hwnd, UINT message, UINT_PTR iTimerID, DWORD dwTime);
void toggle_auto_play(HWND hwnd);
