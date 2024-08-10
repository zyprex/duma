# About

A win32 image viewer made for:
1. very long height/width image
2. webtoon/manga/comic image

It's auto split image to two colums (if height longer than width too much),
or row (if width longer than height too much).

# Build

Use `make` build debug version, `make ok` build release version.

Supported image formats are png,jpg,gif,bmp,webp. If you want add more formats,
see function `check_file_extension` (line 22) in source file `openfile.c`,
add extension as I do, then recompile the program use gcc mingw-w64.

# Quick View

You can adjust the image's display by:
1. zoom in/out
2. move
3. rotation
4. trim margin
5. set overlap gap
6. enlarge an area (use select mode)

Some extra functions:
1. save bookmark as batch script
2. move to next/prev dir
3. auto play
4. fullscreen

For mouse and keymap, see [doc/HELP.md](doc/HELP.md).

