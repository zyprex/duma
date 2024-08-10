#include "defs.h"

int prefix_number = 0;

int offset_y_coord = 0;
int offset_x_coord = 0;

int trim_margin_left = 0;
int trim_margin_right = 0;
int trim_margin_top = 0;
int trim_margin_bottom = 0;

float zoom_factor = 1.0f;

int overlap_gap = 0;

int rotation_degree = 0;

int show_file_info = 0;

int auto_play_time = 0;

void quit() {
  DeleteObject(hFont);
  jpeg_unloader();
  PostQuitMessage(0);
}

void refresh_new(HWND hwnd) {
  page_part = 0;
  page_part_max = 0;
  jpeg_unloader();
  InvalidateRect(hwnd, NULL, 0);
}

void refresh(HWND hwnd) {
  InvalidateRect(hwnd, NULL, 0);
}

void trim_margin_on(HWND hwnd, int* where) {
  *where = prefix_number;
  prefix_number = 0;
  refresh(hwnd);
}

void trim_margin_all(HWND hwnd) {
  trim_margin_left = prefix_number;
  trim_margin_right = prefix_number;
  trim_margin_top = prefix_number;
  trim_margin_bottom = prefix_number;
  prefix_number = 0;
  refresh(hwnd);
}

void move_offset_on(HWND hwnd, int* coord, int val) {
  *coord += val;
  refresh(hwnd);
}

void move_offset_reset(HWND hwnd) {
  offset_x_coord = 0;
  offset_y_coord = 0;
  refresh(hwnd);
}


void goto_page_n(HWND hwnd) {
  if (prefix_number >= 1 && prefix_number <= openfile_idx_max) {
    dir_files(prefix_number);
    refresh_new(hwnd);
  }
  prefix_number = 0;
  update_title(hwnd);
}

void goto_page_part_begin(HWND hwnd) {
  page_part = 0;
  refresh(hwnd);
  update_title(hwnd);
}

void goto_page_part_end(HWND hwnd) {
  page_part = page_part_max;
  refresh(hwnd);
  update_title(hwnd);
}

void goto_page_part_n(HWND hwnd) {
  if (prefix_number >= 1 && prefix_number <= page_part_max + 1) {
    page_part = prefix_number - 1;
    refresh(hwnd);
  }
  prefix_number = 0;
  update_title(hwnd);
}

int page_next(HWND hwnd) { 
  if (openfile_idx + 1 <= openfile_idx_max) {
    page_part = 0;
    dir_files(openfile_idx + 1);
    refresh_new(hwnd);
    return 1;
  }
  set_flash_info("Last Page");
  refresh(hwnd);
  return 0;
}

int page_prev(HWND hwnd) {
  if (openfile_idx - 1 >= 1) {
    page_part = 0;
    dir_files(openfile_idx - 1);
    refresh_new(hwnd);
    return 1;
  }
  set_flash_info("First Page");
  refresh(hwnd);
  return 0;
  
}

void page_part_next(HWND hwnd) {
  if (page_part < page_part_max) {
    page_part++;
    refresh(hwnd);
  } else {
    page_next(hwnd);
  }
}

void page_part_prev(HWND hwnd) {
  if (page_part > 0) {
    page_part--;
    refresh(hwnd);
  } else {
    page_prev(hwnd);
  }
}

void print_tweaks(char* str) {
  printf("%s%d,%d,%d,%d,%d,%d,%d\n",
    str,
    offset_x_coord,
    offset_y_coord,
    trim_margin_left,
    trim_margin_right,
    trim_margin_top,
    trim_margin_bottom,
    overlap_gap);
}

void save_tweaks() {
  int arr_tweak[7] = {
    offset_x_coord,
    offset_y_coord,
    trim_margin_left,
    trim_margin_right,
    trim_margin_top,
    trim_margin_bottom,
    overlap_gap,
  };

  char data_file[MAX_PATH] = {0};
  get_datadir_path(data_file, APP_TWEAK_DAT);

  FILE* fp = fopen(data_file, "wb");
  if (fp==NULL) {
    printf("save_tweaks: fopen failed\n");
    return;
  }
  int n = 0;
  n = fwrite(&arr_tweak, sizeof(arr_tweak), 1, fp);
  if (n != 1) {
    printf("save_tweaks: write failed\n");
    return;
  }
  fclose(fp);
  print_tweaks("save_tweaks: ");
}

void load_tweaks() {
  int arr_tweak[7];

  char data_file[MAX_PATH] = {0};
  get_datadir_path(data_file, APP_TWEAK_DAT);

  FILE* fp = fopen(data_file, "rb");
  if (fp==NULL) {
    printf("load_tweaks: fopen failed\n");
    return;
  }
  int n = 0;
  n = fread(&arr_tweak, sizeof(arr_tweak), 1, fp);
  if (n != 1) {
    printf("load_tweaks: read failed\n");
    return;
  }
  fclose(fp);

  offset_y_coord = arr_tweak[0];
  offset_x_coord = arr_tweak[1];

  trim_margin_left = arr_tweak[2];
  trim_margin_right = arr_tweak[3];
  trim_margin_top = arr_tweak[4];
  trim_margin_bottom = arr_tweak[5];

  overlap_gap = arr_tweak[6];

  print_tweaks("load_tweaks: ");
}


void save_last_file() {
  char data_file[MAX_PATH] = {0};
  get_datadir_path(data_file, APP_LAST_FILE);

  FILE* fp = fopen(data_file, "r");

  if (fp != NULL) {
    fclose(fp);
    int mid = MessageBox(NULL, "Last file exist, rewrite it ?",
        TEXT(APP_TITLE), MB_OKCANCEL | MB_ICONWARNING | MB_APPLMODAL);
    if (mid != IDOK) {
      return;
    }
  }

  fp = fopen(data_file, "w");
  fprintf(fp, "start duma.exe %s\\%s", openfile_dir, openfile_name);
  fclose(fp);
  printf("save last file\n");
}

void read_last_file(HWND hwnd) {
  char data_file[MAX_PATH] = {0};
  get_datadir_path(data_file, APP_LAST_FILE);

  FILE* fp = fopen(data_file, "r");
  if (fp == NULL) {
    return;
  }
  char last_fname[MAX_PATH] = {0};
  fgets(last_fname, MAX_PATH, fp);
  last_fname[strlen(last_fname)] = '\0';
  printf("Last file load:%s\n", last_fname + strlen("start duma.exe "));
  fclose(fp);
  path_split(last_fname + strlen("start duma.exe "));
  refresh_new(hwnd);
}

int toggle_fullscreen(HWND hwnd, int on) {
  static HWND hwnd_parent = NULL;
  static DWORD dwWindowStyles = 0;
  static RECT rect_window = {0,0,0,0};
  static int is_fullscreen = 0;
  if (on) {
    if (!is_fullscreen) {
       /* window maximum */
      if (IsZoomed(hwnd)) {
        ShowWindow(hwnd, SW_NORMAL);
      }
      dwWindowStyles = GetWindowLong(hwnd, GWL_STYLE);
      hwnd_parent = SetParent(hwnd, NULL);
      SetWindowLong(hwnd, GWL_STYLE,
          (dwWindowStyles
          & (~WS_CHILD)
          & (~WS_BORDER)
          & (~WS_SIZEBOX))
          | WS_POPUP);
      GetWindowRect(hwnd, &rect_window);
      ShowWindow(hwnd, SW_MAXIMIZE);
      /* SetWindowPos(hwnd, HWND_TOP, 0, 0, */
          /* GetSystemMetrics(SM_CXSCREEN), */
          /* GetSystemMetrics(SM_CYSCREEN), */
          /* SWP_DRAWFRAME | SWP_DEFERERASE | SWP_FRAMECHANGED); */
      is_fullscreen = 1;
    }
  } else {
    if (is_fullscreen) {
      SetWindowLong(hwnd, GWL_STYLE, dwWindowStyles);
      SetParent(hwnd, hwnd_parent);
      POINT pt = {rect_window.left, rect_window.top};
      /* ScreenToClient(hwnd, &pt); */
      MoveWindow(hwnd, pt.x, pt.y,
          rect_window.right - rect_window.left,
          rect_window.bottom - rect_window.top, TRUE);
      is_fullscreen = 0;
    }
  }
  refresh(hwnd);
  return is_fullscreen;
}

void zoom_to(HWND hwnd, float* factor, float val) {
  *factor += val;
  refresh(hwnd);
}

void zoom_reset(HWND hwnd) {
  zoom_factor=1.00f;
  refresh(hwnd);
}

void overlap_gap_on(HWND hwnd, int* overlap, int val) {
  *overlap += val;
  refresh(hwnd);
}

void overlap_gap_reset(HWND hwnd) {
  overlap_gap = 0;
  refresh(hwnd);
}

void rotate_to_degree(HWND hwnd, int degree) {
  rotation_degree = degree;
  prefix_number = 0;
  refresh(hwnd);
}

void toggle_file_info(HWND hwnd) {
  show_file_info = !show_file_info;
  refresh(hwnd);
}

void start_auto_play_timer(HWND hwnd, int ms) {
  SetTimer(hwnd, TIMER_ID_AUTO_PLAY, ms, timer_proc);
}

void kill_auto_play_timer(HWND hwnd) {
  KillTimer(hwnd, TIMER_ID_AUTO_PLAY);
}

void start_flash_info_timer(HWND hwnd, int ms) {
  SetTimer(hwnd, TIMER_ID_FLASH_INFO, ms, timer_proc);
}

void kill_flash_info_timer(HWND hwnd) {
  KillTimer(hwnd, TIMER_ID_FLASH_INFO);
}

VOID CALLBACK timer_proc(HWND hwnd, UINT message, UINT_PTR iTimerID, DWORD dwTime) {
  if (iTimerID == TIMER_ID_FLASH_INFO) {
    refresh(hwnd);
    kill_flash_info_timer(hwnd);
  }
  if (iTimerID == TIMER_ID_AUTO_PLAY) {
    int n = page_next(hwnd);
    if (n == 0) {
      kill_auto_play_timer(hwnd);
      return;
    }
    if (auto_play_time != 0) {
      kill_auto_play_timer(hwnd);
      start_auto_play_timer(hwnd, auto_play_time);
    }
  }
}

void toggle_auto_play(HWND hwnd) {
  if (prefix_number > 0) {
    auto_play_time = prefix_number * 1000;
    start_auto_play_timer(hwnd, auto_play_time);
    set_flash_info("Auto play start %d seconds!", prefix_number);
    prefix_number = 0;
  } else {
    kill_auto_play_timer(hwnd);
    set_flash_info("Auto play cancel!");
  }
  refresh(hwnd);
}
