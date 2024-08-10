#include "defs.h"

static int fullscreen_state = 0;

static int cursor_x = 0;
static int cursor_y = 0;
static int saved_offset_x = 0;
static int saved_offset_y = 0;

RECT select_rect = {0, 0, 0, 0};
int select_mode = SELECT_MODE_OFF;

int is_ctrl_down = 0;
int is_shift_down = 0;
int is_esc_down = 0;
#define MAP_KEY_STATE \
      is_ctrl_down = KEY_DOWN(VK_CONTROL); \
      is_shift_down = KEY_DOWN(VK_SHIFT);  \
      is_esc_down = KEY_DOWN(VK_ESCAPE);

static void fullscreen_switcher(HWND hwnd) {
  fullscreen_state = fullscreen_state == 0 ? 1 : 0;
  toggle_fullscreen(hwnd, fullscreen_state);
}

void on_keypress(HWND hwnd, char ch) {
  if (ch >= '0' && ch <= '9') {
    prefix_number *= 10;
    prefix_number += ch - '0';
    update_title(hwnd);
  }
  switch (ch) {
    case 'o': open_file(hwnd); break;
    case 'n': page_next(hwnd); break;
    case 'p': page_prev(hwnd); break;
    case ' ':
    case '.': page_part_next(hwnd); break;
    case '>': goto_page_part_end(hwnd); break;
    case '\b':
    case ',': page_part_prev(hwnd); break;
    case '<': goto_page_part_begin(hwnd); break;
    case 'g': goto_page_n(hwnd); break;
    case 't': goto_page_part_n(hwnd); break;
    case 'q': quit(); break;
/* trim margin */
    case 'w': trim_margin_on(hwnd, &trim_margin_top); break;
    case 'a': trim_margin_on(hwnd, &trim_margin_left); break;
    case 's': trim_margin_on(hwnd, &trim_margin_bottom); break;
    case 'd': trim_margin_on(hwnd, &trim_margin_right); break;
    case 'e': trim_margin_all(hwnd); break;
/* move offset */
    case 'K': move_offset_on(hwnd, &offset_y_coord, -100);break;
    case 'J': move_offset_on(hwnd, &offset_y_coord, 100); break;
    case 'H': move_offset_on(hwnd, &offset_x_coord, -100);break;
    case 'L': move_offset_on(hwnd, &offset_x_coord, 100); break;
    case 'k': move_offset_on(hwnd, &offset_y_coord, -10);break;
    case 'j': move_offset_on(hwnd, &offset_y_coord, 10); break;
    case 'h': move_offset_on(hwnd, &offset_x_coord, -10);break;
    case 'l': move_offset_on(hwnd, &offset_x_coord, 10); break;
    case 'm': move_offset_reset(hwnd); break;
/* save load offset and trim */
    case 'y':
      save_tweaks();
      break;
    case 'u':
      load_tweaks();
      refresh(hwnd);
      break;
/* save last file */
    case 'b': save_last_file(); break;
    case 'B': read_last_file(hwnd); break;
/* toggle fullscreen */
    case 'f':
      fullscreen_switcher(hwnd);
      break;
/* zoom */
    case '_': zoom_to(hwnd, &zoom_factor, -0.1f); break;
    case '+': zoom_to(hwnd, &zoom_factor, +0.1f); break;
    case '-': zoom_to(hwnd, &zoom_factor, -0.01f); break;
    case '=': zoom_to(hwnd, &zoom_factor, +0.01f); break;
    case 'z': zoom_reset(hwnd); break;
/* set overlap gap*/
    case 'V': overlap_gap_on(hwnd, &overlap_gap, 100);break;
    case 'C': overlap_gap_on(hwnd, &overlap_gap, -100);break;
    case 'v': overlap_gap_on(hwnd, &overlap_gap, 10);break;
    case 'c': overlap_gap_on(hwnd, &overlap_gap, -10);break;
    case 'x': overlap_gap_reset(hwnd);break;
/* rotation */
    case 'r': rotate_to_degree(hwnd, prefix_number); break;

/* next prev dirs */
    case '[': dir_files_np(hwnd, DIR_PREV_FILES); break;
    case ']': dir_files_np(hwnd, DIR_NEXT_FILES); break;
/* show file info */
    case 'i': toggle_file_info(hwnd); break;
/* auto play n seconds */
    case '\\': toggle_auto_play(hwnd); break;
/* reset all can reset */
    case '\t':
       prefix_number = 0;
       trim_margin_all(hwnd);
       move_offset_reset(hwnd);
       zoom_reset(hwnd);
       overlap_gap_reset(hwnd);
       rotate_to_degree(hwnd, 0);
       break;
    default:
       break;
  }
}

static void select_set_mode(HWND hwnd, int mode) {
  if (select_mode == SELECT_MODE_OFF) {
    select_mode = mode;
    refresh(hwnd);
  }
}

static void select_finish(HWND hwnd) {
  if (select_mode == SELECT_MODE_OFF
      || select_mode == SELECT_MODE_LOCK)  {
    return;
  }
  select_mode = 0;
  SetRectEmpty(&select_rect);
  refresh(hwnd);
}

static void select_going(HWND hwnd, LPARAM lParam) {
  if (select_mode != SELECT_MODE_OFF) {
    return;
  }
  int cur_x = LOWORD(lParam);
  int cur_y = HIWORD(lParam);
  SetRect(&select_rect,
      cursor_x < cur_x ? cursor_x : cur_x,
      cursor_y < cur_y ? cursor_y : cur_y,
      cursor_x > cur_x ? cursor_x : cur_x,
      cursor_y > cur_y ? cursor_y : cur_y);
  refresh(hwnd);
}

void on_keydown(HWND hwnd) {
  MAP_KEY_STATE;
  if (is_esc_down) {
    select_mode = SELECT_MODE_ONCE;
    select_finish(hwnd);
  }
  /* sensible  */
  if (KEY_DOWN(VK_HOME)) goto_page_part_begin(hwnd);
  if (KEY_DOWN(VK_END)) goto_page_part_end(hwnd);
  if (KEY_DOWN(VK_NEXT)) page_part_next(hwnd);
  if (KEY_DOWN(VK_PRIOR)) page_part_prev(hwnd);
  if (KEY_DOWN(VK_LEFT)) move_offset_on(hwnd, &offset_x_coord, -10);
  if (KEY_DOWN(VK_RIGHT)) move_offset_on(hwnd, &offset_x_coord, 10);
  if (KEY_DOWN(VK_UP)) move_offset_on(hwnd, &offset_y_coord, -10);
  if (KEY_DOWN(VK_DOWN)) move_offset_on(hwnd, &offset_y_coord, 10);

}

void on_keyup(HWND hwnd) {
  MAP_KEY_STATE;
  if (!is_shift_down && !IsRectEmpty(&select_rect)) {
    select_finish(hwnd);
  }
}

void on_mouse_move(HWND hwnd, WPARAM wParam, LPARAM lParam) {
  /* ctrl + shift + lbutton */
  if (wParam & MK_CONTROL && wParam & MK_SHIFT && wParam & MK_LBUTTON) {
    select_going(hwnd, lParam);
  }
  /* shift + lbutton */
  else if (wParam & MK_SHIFT && wParam & MK_LBUTTON) {
    select_going(hwnd, lParam);
  }
  /* ctrl+ lbutton */
  else if (wParam & MK_CONTROL && wParam & MK_LBUTTON) {
    int cur_x = LOWORD(lParam);
    int cur_y = HIWORD(lParam);
    offset_x_coord = (cur_x - cursor_x) + saved_offset_x;
    offset_y_coord = (cur_y - cursor_y) + saved_offset_y;
    printf("dx:%d, dy:%d, (%d,%d)\n", cur_x - cursor_x, cur_y - cursor_y, offset_x_coord, offset_y_coord);
    refresh(hwnd);
  }
}

void on_mouse_lbtn_up(HWND hwnd, WPARAM wParam, LPARAM lParam) {
  if (wParam & MK_CONTROL && wParam & MK_SHIFT) {
    select_set_mode(hwnd, SELECT_MODE_LOCK);
  }
  else if (wParam & MK_SHIFT) {
    /* draw select_rect on client ... */
    select_set_mode(hwnd, SELECT_MODE_ONCE);
  }
}

void on_mouse_lbtn(HWND hwnd, WPARAM wParam, LPARAM lParam) {
  /* RBUTTON + LBUTTON */
  if (wParam & MK_RBUTTON) {
    fullscreen_switcher(hwnd);
  }
  else if (wParam & MK_SHIFT) {
    cursor_x = LOWORD(lParam);
    cursor_y = HIWORD(lParam);
  }
  else if (wParam & MK_CONTROL) {
    cursor_x = LOWORD(lParam);
    cursor_y = HIWORD(lParam);
    saved_offset_x = offset_x_coord;
    saved_offset_y = offset_y_coord;
  }
  else {
    page_part_next(hwnd);
  }
}
void on_mouse_rbtn(HWND hwnd, WPARAM wParam, LPARAM lParam) {
  if (wParam & MK_CONTROL) {
    move_offset_reset(hwnd);
  } else {
    page_part_prev(hwnd);
  }
}

void on_mouse_mbtn(HWND hwnd, WPARAM wParam, LPARAM lParam) {
  if (wParam & MK_CONTROL) {
    zoom_reset(hwnd);
  } else {
    open_file(hwnd);
  }
}

void on_mouse_wheel(HWND hwnd, WPARAM wParam, LPARAM lParam) {
  short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
  /* ctrl + wheel */
  if (wParam & MK_CONTROL) {
    zoom_to(hwnd, &zoom_factor, zDelta / 120 * 0.01f);
  }
  else {
    if (zDelta < 0) {
      /* wheel down */
      page_next(hwnd);
    } else {
      /* wheel up */
      page_prev(hwnd);
    }
  }
}
