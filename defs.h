#pragma once

#include <windows.h>
#include <stdio.h>
#include <locale.h>
#include "jpeg_loader.h"
#include "openfile.h"
#include "painter.h"
#include "cmd.h"
#include "event.h"

#define APP_NAME "duma"
#define APP_TITLE "duma"

#define APP_TWEAK_DAT "duma_tweaks.dat"
#define APP_LAST_FILE "duma_last_file.bat"

#define SELECT_MODE_OFF 0
#define SELECT_MODE_ONCE 1
#define SELECT_MODE_LOCK 2

extern HFONT hFont; // global font handle

