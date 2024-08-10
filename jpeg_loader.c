#include "jpeg_loader.h"

COLORREF* jpeg_px_data = NULL;

/* FREE_IMAGE_FORMAT fif_current = FIF_JPEG; */

COLORREF* jpeg_loader(const char* filename, int* w, int* h) {

  FreeImage_Initialise(TRUE);
  FIBITMAP* dib;

  FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
  fif = FreeImage_GetFileType(filename, 0);
  if (fif == FIF_UNKNOWN) {
    fif = FreeImage_GetFIFFromFilename(filename);
    if (fif == FIF_UNKNOWN)
      return NULL;
  }

  dib = FreeImage_Load(fif, filename, 0);

image_display_process:
  *h = FreeImage_GetHeight(dib);
  *w = FreeImage_GetWidth(dib);

  FREE_IMAGE_TYPE  image_type = FreeImage_GetImageType(dib);
  unsigned bpp = FreeImage_GetBPP(dib); // 1 4 8 16 24 32
  int bit_per_px  = FreeImage_GetLine(dib) / (*w);
  unsigned x, y;

  jpeg_px_data = (COLORREF*)calloc((*w)*(*h), sizeof(COLORREF));

  if (jpeg_px_data == NULL) {
    printf("jpeg_loader: calloc failed\n");
    return NULL;
  }

  if (image_type == FIT_BITMAP) {
    // blend rgba pixel on pure black background
    printf("bpp:%d\n", bpp);
    if (bpp == 32) {
      // use a user specified background
      RGBQUAD bk = { 0, 0, 0, 255 };
      dib = FreeImage_Composite(dib, FALSE, &bk, NULL);
      dib = FreeImage_ConvertTo24Bits(dib);
      goto image_display_process;
    }
    else if (bpp == 24) {
      for (y = 0; y < *h; ++y) {
        BYTE *bits = FreeImage_GetScanLine(dib, *h - 1 - y);
        for (x = 0; x < *w; ++x) {
          /* int alpha = bits[FI_RGBA_ALPHA]; */
          // BGR RGB
          COLORREF px = RGB(
              bits[FI_RGBA_BLUE],
              bits[FI_RGBA_GREEN],
              bits[FI_RGBA_RED]);
          jpeg_px_data[x + y * (*w)] = px;
          bits+=bit_per_px;
        }
      }
    }
    else {
      dib = FreeImage_ConvertTo24Bits(dib);
      if (dib != NULL) {
        goto image_display_process;
      }
    }
  }

  FreeImage_Unload(dib);
  FreeImage_DeInitialise();

  return jpeg_px_data;
}

void jpeg_unloader() {
  if (jpeg_px_data != NULL) {
    free(jpeg_px_data);
    jpeg_px_data = NULL;
  }
}
