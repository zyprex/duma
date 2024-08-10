#include "defs.h"

int page_part = 0;
int page_part_max = 0;

static char flash_info[FLASH_INFO_LEN];
/* get values */
static RECT rect; /* client rect */
static int cW; /* client width */
static int cH; /* client height */
static int aW; /* image actual width */
static int aH; /* image actual  height */
/* calcualted values */
static int iW; /* image display width */
static int iH; /* image display height */

#define MAP_TRANSFORM_VALUES \
  int ox = offset_x_coord; \
  int oy = offset_y_coord; \
  int mg_x = trim_margin_left; \
  int mg_y = trim_margin_top; \
  int mg_w = trim_margin_left + trim_margin_right; \
  int mg_h = trim_margin_top + trim_margin_bottom; \
/* apply zoom factor */ \
  if (zoom_factor > 1.0f || zoom_factor < 1.0f) { \
    iW *= zoom_factor; \
    iH *= zoom_factor; \
  }

static void rotate_dc(HDC hdc, int degree, int x, int y) {
  SetGraphicsMode(hdc, GM_ADVANCED);
  XFORM xform;
  if (degree == 0) {
    xform.eM11 = 1.0f;
    xform.eM12 = 0.0f;
    xform.eM21 = 0.0f;
    xform.eM22 = 1.0f;
    xform.eDx = 0.0f;
    xform.eDy = 0.0f;
  } else {
    double ra = (double) degree / 180 * 3.1415926;
    xform.eM11 = cos(ra);
    xform.eM12 = sin(ra);
    xform.eM21 = -sin(ra);
    xform.eM22 = cos(ra);
    xform.eDx = x-cos(ra)*x + sin(ra)*y;
    xform.eDy = y-cos(ra)*y-sin(ra)*x;
  }
  SetWorldTransform(hdc, &xform);
}

static void draw_select_area(HDC hdc) {
  HDC hdc_select = CreateCompatibleDC(hdc);

  int select_w = select_rect.right - select_rect.left;
  int select_h = select_rect.bottom - select_rect.top;

  HBITMAP hbmp_select = CreateCompatibleBitmap(
      hdc, select_w, select_h);
  SelectObject(hdc_select, hbmp_select);
  BitBlt(hdc_select, 0, 0,
      select_w, select_h,
      hdc, select_rect.left, select_rect.top,
      SRCCOPY);

  SetStretchBltMode(hdc, HALFTONE);

  int center_x = 0;
  int center_y = 0;
  /* fit to client height */
  iH = cH;
  iW = iH * select_w / select_h;
  center_x = (cW-iW)/2;
  if (iW > cW) {
    /* fit to client width */
    iW = cW;
    iH = iW * select_h / select_w;
    center_y = (cH-iH)/2;
    center_x = 0;
  }

  StretchBlt(hdc, center_x, center_y, iW, iH,
      hdc_select, 0, 0, select_w, select_h, SRCCOPY);

  DeleteDC(hdc_select);
  DeleteObject(hbmp_select);
}

static void draw_page_center(HDC hdc_src, HDC hdc_dest) {
  int center_x = 0;
  int center_y = 0;
  /* fit to client height */
  iH = cH;
  iW = iH * aW / aH;
  center_x = (cW-iW)/2;
  if (iW > cW) {
    /* fit to client width */
    iW = cW;
    iH = iW * aH / aW;
    center_y = (cH-iH)/2;
    center_x = 0;
  }
  MAP_TRANSFORM_VALUES;
  StretchBlt(hdc_src, 0+ox+center_x, 0+oy+center_y, iW, iH,
      hdc_dest, 0+mg_x, 0+mg_y, aW-mg_w, aH-mg_h, SRCCOPY);
}

static void draw_part(HDC hdc_src, HDC hdc_dest, int n) {
  MAP_TRANSFORM_VALUES;
  if (n%2!=0) oy += overlap_gap; // use overlap !
  StretchBlt(hdc_src, ((n%2==0) ? 0 : iW)+ox, 0-n*cH+oy, iW, iH,
      hdc_dest, 0+mg_x, 0+mg_y, aW-mg_w, aH-mg_h, SRCCOPY);
}

static void draw_part_2(HDC hdc_src, HDC hdc_dest, int n) {
  draw_part(hdc_src, hdc_dest, 2*n);
  draw_part(hdc_src, hdc_dest, 2*n+1);

}
static void draw_part_h(HDC hdc_src, HDC hdc_dest, int n) {
  MAP_TRANSFORM_VALUES;
  if (n%2!=0) ox += overlap_gap; // use overlap !
  StretchBlt(hdc_src, 0-n*cW+ox, ((n%2==0) ? 0 : iH)+oy, iW, iH,
      hdc_dest, 0+mg_x, 0+mg_y, aW-mg_w, aH-mg_h, SRCCOPY);
}

static void draw_part_h_2(HDC hdc_src, HDC hdc_dest, int n) {
  draw_part_h(hdc_src, hdc_dest, 2*n);
  draw_part_h(hdc_src, hdc_dest, 2*n+1);
}

void update_title(HWND hwnd) {
  char title[MAX_PATH * 3];
  char prefix_number_str[20] = {0};
  if (prefix_number != 0) {
    sprintf(prefix_number_str, " ...%d", prefix_number);
  }
  char page_part_str[40] = {0};
  if (page_part_max > 0) {
    sprintf(page_part_str, " (%d/%d)", page_part + 1, page_part_max + 1);
  }
  sprintf(title, APP_TITLE " -- %d/%d%s%s",
      openfile_idx, openfile_idx_max,
      page_part_str, prefix_number_str);

  SetWindowText(hwnd, title);
}

void set_flash_info(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vsprintf(flash_info, fmt, ap);
  va_end(ap);
}

void display_flash_info(HWND hwnd, HDC hdc) {
  if (flash_info[0] != '\0') {
    prompt_text(hdc, flash_info, RGB(0, 255, 0));
    flash_info[0] = '\0';
    start_flash_info_timer(hwnd, 2000);
  }
}

void display_file_info(HDC hdc) {
  if (show_file_info) {
    char text[500];
    snprintf(text, 500, "%s\\%s\n%d/%d"
        , openfile_dir, openfile_name, openfile_idx, openfile_idx_max);
    SelectObject(hdc, hFont);
    SetBkMode(hdc, TRANSPARENT);

    RECT rect_shadow;
    CopyRect(&rect_shadow, &rect);
    rect_shadow.left = rect_shadow.left + 1;
    rect_shadow.top = rect_shadow.top + 1;
    rect_shadow.right = rect_shadow.right + 1;
    rect_shadow.bottom = rect_shadow.bottom + 1;

    SetTextColor(hdc, RGB(0, 0, 0));
    DrawText(hdc, text, -1, &rect_shadow, DT_WORDBREAK | DT_NOCLIP | DT_NOPREFIX);

    SetTextColor(hdc, RGB(255, 0, 255));
    DrawText(hdc, text, -1, &rect, DT_WORDBREAK | DT_NOCLIP | DT_NOPREFIX);
  }
}

void prompt_text(HDC hdc, char* text, COLORREF color) {
  SelectObject(hdc, hFont);
  SetBkMode(hdc, TRANSPARENT);
  SetTextColor(hdc, color);
  /* DrawText(hdc, text, -1, &rect, DT_NOCLIP | DT_CENTER | DT_VCENTER | DT_SINGLELINE); */

  RECT rect_win, rect_bot;
  CopyRect(&rect_win, &rect);
  CopyRect(&rect_bot, &rect);
  int text_h = DrawText(hdc, text, -1, &rect_win, DT_CALCRECT | DT_WORDBREAK | DT_NOCLIP | DT_NOPREFIX);
  rect_bot.top = rect_bot.bottom - text_h;

  FillRect(hdc, &rect_bot, CreateSolidBrush(RGB(0,0,0)));
  DrawText(hdc, text, -1, &rect_bot, DT_RIGHT | DT_WORDBREAK | DT_NOCLIP | DT_NOPREFIX);

}


void painter(HWND hwnd) {

  GetClientRect(hwnd, &rect);

  cW = rect.right;
  cH = rect.bottom;

  PAINTSTRUCT ps;
  HDC hdc = BeginPaint(hwnd, &ps);

  /* get full path of open file */
  char path[MAX_PATH];
  get_openfile_path(path);
  printf("Use path:%s\n", path);

  if (strlen(path) < 8) {
    prompt_text(hdc, "See 'HELP.md'", RGB(0, 255, 0));
    goto painter_end;
  }
  update_title(hwnd);

  /* decompress the open file */
  if (jpeg_px_data == NULL) {
    if (jpeg_loader(path, &aW, &aH) == NULL) {
      prompt_text(hdc, "Maybe Invalid Path ?", RGB(255, 0, 0));
      goto painter_end;
    }
  }
  HBITMAP hbmp_jpeg = CreateBitmap(aW, aH, 32, 1, jpeg_px_data);

  /* prepare the jpeg file as bmp */
  HDC hdc_jpeg = CreateCompatibleDC(hdc);
  SelectObject(hdc_jpeg, hbmp_jpeg);

  /* prepare temp bmp */
  HDC hdc_temp = CreateCompatibleDC(hdc);
  HBITMAP hbmp_temp;
  hbmp_temp = CreateCompatibleBitmap(hdc, cW, cH);
  SelectObject(hdc_temp, hbmp_temp);

  SetStretchBltMode(hdc_temp, HALFTONE);

  if (rotation_degree > 0) {
    rotate_dc(hdc_temp, rotation_degree, cW/2, cH/2);
  }

  if (aW <= aH) {
    /* try fit to 1/2 screen width */
    iW = cW / 2;
    iH = iW * aH / aW;
    if (iH <= cH) {
      draw_page_center(hdc_temp, hdc_jpeg);
    }
    else {
      page_part_max = (iH / cH)/2;
      update_title(hwnd);
      draw_part_2(hdc_temp, hdc_jpeg, page_part);
    }
  }
  else {
    /* try fit to 1/2 screen height */
    iH = cH / 2;
    iW = iH * aW / aH;
    if (iW <= cW) {
      draw_page_center(hdc_temp, hdc_jpeg);
    }
    else {
      page_part_max = (iW / cW)/2;
      update_title(hwnd);
      draw_part_h_2(hdc_temp, hdc_jpeg, page_part);
    }
  }
  if (rotation_degree > 0) {
    rotate_dc(hdc_temp, 0, 0, 0);
  }

  // draw main image
  BitBlt(hdc, 0, 0, cW, cH, hdc_temp, 0, 0, SRCCOPY);

  // draw select frame
  if(!IsRectEmpty(&select_rect) && select_mode == SELECT_MODE_OFF) {
    FrameRect(hdc, &select_rect, CreateSolidBrush(RGB(255,0,0)));
  }

  // draw select area
  if(select_mode) {
    draw_select_area(hdc);
  }

  display_flash_info(hwnd, hdc);
  display_file_info(hdc);

  DeleteObject(hbmp_jpeg);
  DeleteDC(hdc_jpeg);
  DeleteObject(hbmp_temp);
  DeleteDC(hdc_temp);

painter_end:
  EndPaint(hwnd, &ps);
}

float get_scale_ratio() {
  return cW == iW ? (cW * 1.0f / aW) :
    (cH * 1.0f / aH);
}

