#include "defs.h"

int openfile_idx = 0;
int openfile_idx_max = 0;
char openfile_dir[MAX_PATH];
char openfile_name[MAX_PATH];

char data_dir[MAX_PATH]; // app start path

static void path_build(char* buf, const char* dir, const char* f) {
  snprintf(buf, MAX_PATH, "%s\\%s", dir, f);
}

static void print_file_path() {
  puts("---NEW---");
  printf("|dir |:%s\n", openfile_dir);
  printf("|name|:%s\n", openfile_name);
  printf("|idx |:%d\n", openfile_idx);
  printf("|max |:%d\n", openfile_idx_max);
  puts("---------");
}

static int check_file_extension(char* path, char* ext) {
  char* fext = strrchr(path, '.');
  if (fext == NULL)
    return 0;
  /* same as the `strcasecmp' in POSIX
  * for case-insensitive string comparasion
  */
  return (!_stricmp(fext + 1, ext));
}

static int validate_file_extension(char* path) {
  return
    check_file_extension(path, "bmp") ||
    check_file_extension(path, "gif") ||
    check_file_extension(path, "jpg") ||
    check_file_extension(path, "jpeg") ||
    check_file_extension(path, "png") ||
    check_file_extension(path, "webp");
}

void get_datadir() {
  GetModuleFileName(NULL, data_dir, MAX_PATH);
  char* sp = strrchr(data_dir, '\\');
  data_dir[strlen(data_dir) - strlen(sp)] = '\0';
  printf("data_dir:%s\n", data_dir);
}

void get_datadir_path(char* buf, const char* name) {
  path_build(buf, data_dir, name);
}

void get_openfile_path(char* buf) {
  path_build(buf, openfile_dir, openfile_name);
}

void path_split(char* path) {
  char* sp = strrchr(path, '\\');
  if (sp == NULL) {
    puts("path error?");
    return;
  }
  // update openfile_name
  snprintf(openfile_name, strlen(sp), "%s", sp + 1);
  // update openfile_dir
  snprintf(openfile_dir, strlen(path) - strlen(sp), "%s", path);
  openfile_dir[strlen(path) - strlen(sp)] = '\0';
  // update openfile_idx;
  dir_files(DIR_FILES_IDX_UPDATE);
  // update openfile_idx_max;
  dir_files(DIR_FILES_IDX_MAX_UPDATE);

  // no file name, auto get first
  if (openfile_name[0] == '\0') {
    dir_files(1);
  }

  print_file_path();
}

void open_file(HWND hwnd) {
  OPENFILENAME ofn;
  char fname[260];

  ZeroMemory(&ofn, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = hwnd;
  ofn.lpstrFile = fname;
  ofn.lpstrFile[0] = '\0';
  ofn.nMaxFile = sizeof(fname);
  ofn.lpstrFilter = "JPG(*.JPG)\0*.JP*G\0"
    "PNG(*.PNG)\0*.PNG\0"
    "WEBP(*.WEBP)\0*.WEBP\0"
    "BMP(*.BMP)\0*.BMP\0"
    "GIF(*.GIF)\0*.GIF\0"
    "All(*.*)\0*.*\0";
  ofn.nFilterIndex = 1;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = NULL;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
  if (GetOpenFileName(&ofn) == TRUE) {
    printf("get open file:%s\n", ofn.lpstrFile);
    if (!validate_file_extension(ofn.lpstrFile)) {
      return;
    }
    path_split(ofn.lpstrFile);
    refresh_new(hwnd);
  }
}

void open_cmdargs_file(char* cmdline) {
  // use cmdargs open file
  if (!validate_file_extension(cmdline)) {
    return;
  }
  if (strstr(cmdline, ":\\") == NULL) {
    char path[MAX_PATH];
    char buffer[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, buffer);
    path_build(path, buffer, cmdline);
    path_split(path);
  } else {
    path_split(cmdline);
  }
}

void open_drag_file(HWND hwnd, WPARAM wParam) {
  HDROP hDrop = (HDROP)wParam;
  UINT n = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
  char f[MAX_PATH];
  for (int i = 0; i < n; ++i) {
    DragQueryFile(hDrop, i, f, MAX_PATH);
    if (validate_file_extension(f)) {
      path_split(f);
      refresh_new(hwnd);
      break;
    }
  }
  DragFinish(hDrop);
}

int dir_files(int idx) {
  char dir[MAX_PATH+7];
  snprintf(dir, MAX_PATH+7,"%s\\*.*", openfile_dir);

  WIN32_FIND_DATA ffd;
  HANDLE hFind = FindFirstFile(dir, &ffd);
  if (hFind == INVALID_HANDLE_VALUE) {
    puts("FindFirstFile Error!");
    return -1;
  }
  int count = 1;
  do {
    if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      // skip dirs
    } else {
      // find `openfile_idx'
      if (idx == DIR_FILES_IDX_UPDATE) {
        if (!strcmp(openfile_name, ffd.cFileName)) {
          openfile_idx = count;
          break;
        }
      }
      // find new file
      if (idx == count) {
        printf("find new file[%d]:%s\n", idx, ffd.cFileName);
        char path[MAX_PATH];
        path_build(path, openfile_dir, ffd.cFileName);
        path_split(path);
        openfile_idx = count;
        break;
      }
    }
    if (validate_file_extension(ffd.cFileName)) {
      count++;
    }
  } while(FindNextFile(hFind, &ffd) != 0);
  // find `openfile_idx_max'
  if (idx == DIR_FILES_IDX_MAX_UPDATE) {
    openfile_idx_max = count - 1;
  }
  CloseHandle(hFind);
  return count;
}

static int dir_try_entry(
    const char* parent_dir,
    const char* current_dir,
    const char* try_dir) {

  char origin_parent_dir[MAX_PATH];
  char origin_name[MAX_PATH];

  sprintf(origin_parent_dir, "%s", current_dir);
  sprintf(origin_name, "%s", openfile_name);

  path_build(openfile_dir, parent_dir, try_dir);
  openfile_name[0] = '\0';

  int n = dir_files(1);
  if (n == -1) {
    // restore previous dir & name
    sprintf(openfile_name, "%s", origin_name);
    snprintf(openfile_dir, MAX_PATH, "%s\\%s", parent_dir, origin_parent_dir);
    return 1;
  }
  print_file_path();
  return 0;
}

int dir_files_np(HWND hwnd, int next_or_prev) {

  if (openfile_name[0] == '\0') return 0;
  char parent_dir[MAX_PATH+2];
  char* sp = strrchr(openfile_dir, '\\');

  /* if (sp == NULL) */
  /* strncpy(parent_dir, openfile_dir, strlen(openfile_dir) - strlen(sp)); */
  // in debug this not ok?
  int parent_dir_len = strlen(openfile_dir) - strlen(sp);
  snprintf(parent_dir, parent_dir_len, "%s", openfile_dir);
  parent_dir[parent_dir_len] = '\0';

  char dir[MAX_PATH+7];
  snprintf(dir, MAX_PATH+7, "%s\\*.*", parent_dir);


  WIN32_FIND_DATA ffd;
  HANDLE hFind = FindFirstFile(dir, &ffd);
  if (hFind == INVALID_HANDLE_VALUE) {
    puts("FindFirstFile Error! (dir_files_np)");
    printf("dir: %s\n", dir);
    return -1;
  }
  int find_current= DIR_NOT_FIND;
  char old_dir[MAX_PATH] = {0};
  do {
    if (!strcmp(ffd.cFileName, "."))
      continue;
    if (!strcmp(ffd.cFileName, ".."))
      continue;
    if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
      continue;
    if (find_current == DIR_FIND_CURRENT) {
      if (next_or_prev == DIR_NEXT_FILES) {
        if(!dir_try_entry(parent_dir, sp + 1, ffd.cFileName)) {
          find_current = DIR_NOT_FIND;
          refresh_new(hwnd);
        } else {
          find_current = DIR_NEXT_NOFILE;
        }
      }
      break;
    }
    if (!strcmp(sp + 1, ffd.cFileName)) {
      if (next_or_prev == DIR_PREV_FILES) {
        if (old_dir[0] == '\0') {
          find_current = DIR_PREV_NOFILE;
          break;
        }
        if (!dir_try_entry(parent_dir, sp + 1, old_dir)) {
          find_current = DIR_NOT_FIND;
          refresh_new(hwnd);
        } else {
          find_current = DIR_PREV_NOFILE;
        }
        break;
      }
      find_current = DIR_FIND_CURRENT;
    } else {
      sprintf(old_dir, "%s", ffd.cFileName);
    }
  } while(FindNextFile(hFind, &ffd) != 0);

  printf("find_current: %d\n", find_current);

  if (find_current == DIR_PREV_NOFILE) {
    set_flash_info("First Dir!");
    /* puts("first dir!"); */
    refresh(hwnd);
  }

  if (find_current == DIR_NEXT_NOFILE ||
      find_current == DIR_FIND_CURRENT) {
    set_flash_info("Last Dir!");
    /* puts("last dir!"); */
    refresh(hwnd);
  }

  CloseHandle(hFind);
  return 0;
}
