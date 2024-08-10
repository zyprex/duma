#pragma once


#define KEY_DOWN(VK) (GetKeyState(VK) & 0x8000)
// #define KEY_ON(VK) (GetKeyState(VK) & 0x1)

extern RECT select_rect;
extern int select_mode;

extern int is_ctrl_down;
extern int is_shift_down;
extern int is_esc_down;

void on_keypress(HWND hwnd, char ch);
void on_keydown(HWND hwnd);
void on_keyup(HWND hwnd);
void on_mouse_move(HWND hwnd, WPARAM wParam, LPARAM lParam);
void on_mouse_lbtn_up(HWND hwnd, WPARAM wParam, LPARAM lParam);
void on_mouse_lbtn(HWND hwnd, WPARAM wParam, LPARAM lParam);
void on_mouse_rbtn(HWND hwnd, WPARAM wParam, LPARAM lParam);
void on_mouse_mbtn(HWND hwnd, WPARAM wParam, LPARAM lParam);
void on_mouse_wheel(HWND hwnd, WPARAM wParam, LPARAM lParam);


