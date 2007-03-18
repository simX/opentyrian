/*
 * OpenTyrian Classic: A moden cross-platform port of Tyrian
 * Copyright (C) 2007  The OpenTyrian Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "opentyr.h"
#include "newshape.h"

#include "vga256d.h"

JE_word min, max;

SDL_Surface *tempscreenseg = NULL;

JE_shapearraytype *shapearray;

JE_word shapex[maxtable][maximumshape],	/* [1..maxtable,1..maximumshape] */
        shapey[maxtable][maximumshape];	/* [1..maxtable,1..maximumshape] */
JE_word shapesize[maxtable][maximumshape];	/* [1..maxtable,1..maximumshape] */
JE_boolean shapexist[maxtable][maximumshape];	/* [1..maxtable,1..maximumshape] */

JE_byte maxshape[maxtable];	/* [1..maxtable] */

JE_byte mousegrabshape[24*28];	/* [1..24*28] */

JE_byte x;

JE_boolean LoadOverride = FALSE;

/*
  Colors:
  253 : Black
  254 : Jump to next line

   Skip X Pixels
   Draw X pixels of color Y
*/

JE_byte *Shapes6Pointer;


void JE_NewLoadShapesB( JE_byte table, FILE *f )
{
    short tempw;
    short z;

    fread(&tempw, 2, 1, f);
    maxshape[table] = tempw;

    if(!LoadOverride) {
        min = 1;
        max = maxshape[table];
    }

    if(min > 1) {
        for(z = 0; z < min-1; z++) {
            fread(&shapexist[table][z], 1, 1, f);

            if(shapexist[table][z]) {
                fread(&shapex   [table][z], 2, 1, f);
                fread(&shapey   [table][z], 2, 1, f);
                fread(&shapesize[table][z], 2, 1, f);

                (*shapearray)[table][z] = malloc(shapex[table][z]*shapey[table][z]);

                fread((*shapearray)[table][z], shapesize[table][z], 1, f);

                free((*shapearray)[table][z]);
            }
        }
    }

    for(z = min-1; z < max; z++) {
        tempw = z-min+1;
        fread(&shapexist[table][tempw], 1, 1, f);

        if(shapexist[table][tempw]) {
            fread(&shapex   [table][tempw], 2, 1, f);
            fread(&shapey   [table][tempw], 2, 1, f);
            fread(&shapesize[table][tempw], 2, 1, f);

            (*shapearray)[table][tempw] = malloc(shapex[table][tempw]*shapey[table][tempw]);

            fread((*shapearray)[table][tempw], shapesize[table][tempw], 1, f);
        }
    }
}

void JE_NewDrawCShape( JE_byte *shape, JE_word xsize, JE_word ysize )
{
    JE_word x = 2, y = 2;
    JE_word xloop = 0, yloop = 0;
    JE_byte *p; /* shape pointer */
    unsigned char *s;   /* screen pointer, 8-bit specific */

    s = (unsigned char *)tempscreenseg->pixels;
    s += y * tempscreenseg->w + x;

    for(p = shape; yloop < ysize; p++) {
        switch(*p) {
            case 255:   /* p transparent pixels */
                p++;
                s += *p; xloop += *p;
                break;
            case 254:   /* next y */
                s -= xloop; xloop = 0;
                s += tempscreenseg->w; yloop++;
                break;
            case 253:   /* 1 transparent pixel */
                s++; xloop++;
                break;
            default:    /* set a pixel */
                *s = *p;
                s++; xloop++;
                break;
        }
        if(xloop == xsize) {
            s -= xloop; xloop = 0;
            s += tempscreenseg->w; yloop++;
        }
    }

    tempscreenseg = VGAScreen;
}

void JE_NewDrawCShapeNum( JE_byte table, JE_byte shape, JE_word x, JE_word y )
{
    JE_word xsize, ysize;
    JE_word xloop = 0, yloop = 0;
    JE_byte *p; /* shape pointer */
    unsigned char *s;   /* screen pointer, 8-bit specific */

    if((shape > maxshape[table]) || (!shapexist[table][shape]) || (shape == 255)) {
        exit(99);   /* pascalism */
    }

    xsize = shapex[table][shape]; ysize = shapey[table][shape];

    s = (unsigned char *)tempscreenseg->pixels;
    s += y * tempscreenseg->w + x;

    for(p = (*shapearray)[table][shape]; yloop < ysize; p++) {
        switch(*p) {
            case 255:   /* p transparent pixels */
                p++;
                s += *p; xloop += *p;
                break;
            case 254:   /* next y */
                s -= xloop; xloop = 0;
                s += tempscreenseg->w; yloop++;
                break;
            case 253:   /* 1 transparent pixel */
                s++; xloop++;
                break;
            default:    /* set a pixel */
                *s = *p;
                s++; xloop++;
                break;
        }
        if(xloop == xsize) {
            s -= xloop; xloop = 0;
            s += tempscreenseg->w; yloop++;
        }
    }

    tempscreenseg = VGAScreen;
}

void JE_NewPurgeShapes( JE_byte table )
{
    JE_word x;

    if(maxshape[table] > 0) {
        for(x = 0; x < maxshape[table]; x++) {
            if(shapexist[table][x]) {
                free((*shapearray)[table][x]);
                shapexist[table][x] = FALSE;
            }
        }
    }
}

void JE_DrawShapeTypeOne( JE_word x, JE_word y, JE_byte *shape )
{
    JE_word xloop = 0, yloop = 0;
    JE_byte *p = shape; /* shape pointer */
    unsigned char *s;   /* screen pointer, 8-bit specific */

    s = (unsigned char *)VGAScreen->pixels;
    s += y * VGAScreen->w + x;

    for(yloop = 0; yloop < 28; yloop++) {
        for(xloop = 0; xloop < 24; xloop++) {
            *s = *p;
            s++; p++;
        }
        s -= 24;
        s += VGAScreen->w;
    }
}

void JE_GrabShapeTypeOne( JE_word x, JE_word y, JE_byte *shape )
{
    JE_word xloop = 0, yloop = 0;
    JE_byte *p = shape; /* shape pointer */
    unsigned char *s;   /* screen pointer, 8-bit specific */

    s = (unsigned char *)VGAScreen->pixels;
    s += y * VGAScreen->w + x;

    for(yloop = 0; yloop < 28; yloop++) {
        for(xloop = 0; xloop < 24; xloop++) {
            *p = *s;
            s++; p++;
        }
        s -= 24;
        s += VGAScreen->w;
    }
}

void newshape_init( void )
{
    tempscreenseg = VGAScreen;
    for(x = 0; x < maxtable; x++) {
        maxshape[x] = 0;
    }
    shapearray = malloc(sizeof(JE_shapearraytype));
}
