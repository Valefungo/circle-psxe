/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2013 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include <stdio.h>
#include "noSDL.h"

/* This a stretch blit implementation based on ideas given to me by
   Tomasz Cejner - thanks! :)

   April 27, 2000 - Sam Lantinga
*/


#define DEFINE_COPY_ROW(name, type)			\
static void name(type *src, int src_w, type *dst, int dst_w)	\
{							\
	int i;						\
	int pos, inc;					\
	type pixel = 0;					\
							\
	pos = 0x10000;					\
	inc = (src_w << 16) / dst_w;			\
	for ( i=dst_w; i>0; --i ) {			\
		while ( pos >= 0x10000L ) {		\
			pixel = *src++;			\
			pos -= 0x10000L;		\
		}					\
		*dst++ = pixel;				\
		pos += inc;				\
	}						\
}
/* *INDENT-OFF* */
DEFINE_COPY_ROW(copy_row1, Uint8)
DEFINE_COPY_ROW(copy_row2, Uint16)
DEFINE_COPY_ROW(copy_row4, Uint32)
/* *INDENT-ON* */

/* The ASM code doesn't handle 24-bpp stretch blits */
static void
copy_row3(Uint8 * src, int src_w, Uint8 * dst, int dst_w)
{
    int i;
    int pos, inc;
    Uint8 pixel[3] = { 0, 0, 0 };

    pos = 0x10000;
    inc = (src_w << 16) / dst_w;
    for (i = dst_w; i > 0; --i) {
        while (pos >= 0x10000L) {
            pixel[0] = *src++;
            pixel[1] = *src++;
            pixel[2] = *src++;
            pos -= 0x10000L;
        }
        *dst++ = pixel[0];
        *dst++ = pixel[1];
        *dst++ = pixel[2];
        pos += inc;
    }
}

/* Perform a stretch blit between two surfaces of the same format.
   NOTE:  This function is not safe to call from multiple threads!
*/
int
SDL_SoftStretch(SDL_Surface * src, const SDL_Rect * srcrect,
                SDL_Surface * dst, const SDL_Rect * dstrect)
{
    int pos, inc;
    int dst_maxrow;
    int src_row, dst_row;
    Uint8 *srcp = NULL;
    Uint8 *dstp;
    SDL_Rect full_src;
    SDL_Rect full_dst;

    printf("T: %d %d %d   S: %d %d %d    SRC:   %d %d    %d %d    DST:   %d %d    %d %d\n",
               dst->w, dst->h, dst->pitch,
               src->w, src->h, dst->pitch,
               srcrect->x, srcrect->y, srcrect->w, srcrect->h,
               dstrect->x, dstrect->y, dstrect->w, dstrect->h);

#define FIND_DST_BYTE_PER_PIXEL 4
#define FIND_SRC_BITS_PER_PIXEL 32
#define FIND_DST_BITS_PER_PIXEL 32

    const int bpp = FIND_DST_BYTE_PER_PIXEL;

    if (FIND_SRC_BITS_PER_PIXEL != FIND_DST_BITS_PER_PIXEL) {
        return SDL_FALSE;
    }

    /* Verify the blit rectangles */
    if (srcrect) {
        if ((srcrect->x < 0) || (srcrect->y < 0) ||
            ((srcrect->x + srcrect->w) > src->w) ||
            ((srcrect->y + srcrect->h) > src->h)) {
            return SDL_FALSE;
        }
    } else {
        full_src.x = 0;
        full_src.y = 0;
        full_src.w = src->w;
        full_src.h = src->h;
        srcrect = &full_src;
    }
    if (dstrect) {
        if ((dstrect->x < 0) || (dstrect->y < 0) ||
            ((dstrect->x + dstrect->w) > dst->w) ||
            ((dstrect->y + dstrect->h) > dst->h)) {
            return SDL_FALSE;
        }
    } else {
        full_dst.x = 0;
        full_dst.y = 0;
        full_dst.w = dst->w;
        full_dst.h = dst->h;
        dstrect = &full_dst;
    }

    /* Set up the data... */
    pos = 0x10000;
    inc = (srcrect->h << 16) / dstrect->h;
    src_row = srcrect->y;
    dst_row = dstrect->y;

    /* Perform the stretch blit */
    for (dst_maxrow = dst_row + dstrect->h; dst_row < dst_maxrow; ++dst_row) {
        dstp = (Uint8 *) dst->pixels + (dst_row * dst->pitch)
            + (dstrect->x * bpp);
        while (pos >= 0x10000L) {
            srcp = (Uint8 *) src->pixels + (src_row * src->pitch)
                + (srcrect->x * bpp);
            ++src_row;
            pos -= 0x10000L;
        }
        switch (bpp) {
            case 1:
                copy_row1(srcp, srcrect->w, dstp, dstrect->w);
                break;
            case 2:
                copy_row2((Uint16 *) srcp, srcrect->w,
                          (Uint16 *) dstp, dstrect->w);
                break;
            case 3:
                copy_row3(srcp, srcrect->w, dstp, dstrect->w);
                break;
            case 4:
                copy_row4((Uint32 *) srcp, srcrect->w,
                          (Uint32 *) dstp, dstrect->w);
                break;
        }
        pos += inc;
    }

    return (0);
}

/* vi: set ts=4 sw=4 expandtab: */

