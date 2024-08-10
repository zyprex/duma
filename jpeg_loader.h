#pragma once
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeImage.h"
 /* contain current jpeg per pixels raw data*/
extern COLORREF* jpeg_px_data;
/* decompress jpeg image  */
COLORREF* jpeg_loader(const char* filename, int* w, int* h);
/* free jpeg raw data  */
void jpeg_unloader();

