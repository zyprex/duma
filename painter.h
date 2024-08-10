#pragma once

#include <math.h>
#include <stdarg.h>

#define FLASH_INFO_LEN 300

extern int page_part; // current split page
extern int page_part_max; // split max page count

void update_title(HWND hwnd);
void set_flash_info(const char* fmt, ...);
void display_flash_info(HWND hwnd, HDC hdc);
void display_file_info(HDC hdc);
void prompt_text(HDC hdc, char* text, COLORREF color);
void painter(HWND hwnd);
float get_scale_ratio();
