#include "defs.h"

HFONT hFont;

LRESULT CALLBACK WndProc(HWND hwnd, UINT wm, WPARAM wParam, LPARAM lParam) {
  switch (wm) {
    case WM_CREATE:
      {
        LOGFONT log_font;
        ZeroMemory(&log_font, sizeof(LOGFONT));
        log_font.lfHeight = 20;
        log_font.lfWeight = FW_BOLD;
        sprintf(log_font.lfFaceName, "Segoe UI");
        hFont = CreateFontIndirect(&log_font);
        /* enable drag files */
        DragAcceptFiles(hwnd, TRUE);
        break;
      }
    case WM_DROPFILES:
      open_drag_file(hwnd, wParam);
      break;
    case WM_DESTROY:
      KillTimer(hwnd, TIMER_ID_FLASH_INFO);
      quit();
      break;
    case WM_PAINT:
      painter(hwnd);
      break;
    case WM_CHAR:
      on_keypress(hwnd, (char)wParam);
      break;
    case WM_KEYDOWN:
      on_keydown(hwnd);
      break;
    case WM_KEYUP:
      on_keyup(hwnd);
      break;
    case WM_MOUSEMOVE:
      on_mouse_move(hwnd, wParam, lParam);
      break;
    case WM_LBUTTONUP:
      on_mouse_lbtn_up(hwnd, wParam, lParam);
      break;
    case WM_LBUTTONDOWN:
      on_mouse_lbtn(hwnd, wParam, lParam);
      break;
    case WM_RBUTTONDOWN:
      on_mouse_rbtn(hwnd, wParam, lParam);
      break;
    case WM_MBUTTONDOWN:
      on_mouse_mbtn(hwnd, wParam, lParam);
      break;
    case WM_MOUSEWHEEL:
      on_mouse_wheel(hwnd, wParam, lParam);
      break;
    default:
      return DefWindowProc(hwnd, wm, wParam, lParam);
  }
  return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow) {

#if defined(DEBUG)
  system("CHCP 65001");
#endif
  /* setlocale(LC_ALL, ""); */
  printf("Locale:%s\n", setlocale(LC_ALL, ""));

  get_datadir();

  // use cmdargs open file
  open_cmdargs_file(szCmdLine);

  LPCSTR szAppName = TEXT(APP_NAME);
  LPCSTR szTitle   = TEXT(APP_TITLE);

  HWND hwnd;
  MSG msg;

  const WNDCLASS wc = {
    .style         = CS_HREDRAW | CS_VREDRAW,
    .lpfnWndProc   = WndProc,
    .cbClsExtra    = 0,
    .cbWndExtra    = 0,
    .hInstance     = hInstance,
    .hIcon         = LoadImage(hInstance, "duma.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE),
    /* .hIcon         = LoadIcon(NULL, IDI_APPLICATION), */
    .hCursor       = LoadCursor(NULL, IDC_ARROW),
    .hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH),
    .lpszMenuName  = NULL,
    .lpszClassName = szAppName
  };

  if (!RegisterClass(&wc)) return 0;


  hwnd = CreateWindow(szAppName, szTitle,
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      NULL,
      NULL,
      hInstance,
      NULL);

  ShowWindow(hwnd, iCmdShow);

  UpdateWindow(hwnd);

  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return msg.wParam;
}

