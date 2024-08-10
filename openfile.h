#pragma once

#define DIR_FILES_IDX_UPDATE -2
#define DIR_FILES_IDX_MAX_UPDATE -1

#define DIR_PREV_FILES 0
#define DIR_NEXT_FILES 1

#define DIR_NOT_FIND 0
#define DIR_NEXT_NOFILE 1
#define DIR_PREV_NOFILE 2
#define DIR_FIND_CURRENT 3

extern int openfile_idx;
extern int openfile_idx_max;
extern char openfile_dir[MAX_PATH];
extern char openfile_name[MAX_PATH];

void get_datadir();
void get_datadir_path(char* buf, const char* name);
void get_openfile_path(char* buf);
void path_split(char* path);
void open_file(HWND hwnd);
void open_cmdargs_file(char* cmdline);
void open_drag_file(HWND hwnd, WPARAM wParam);
int dir_files(int idx);
int dir_files_np(HWND hwnd, int next_or_prev);
