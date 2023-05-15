// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// $Log:$
//
// DESCRIPTION:
//	The status bar widget code.
//
//-----------------------------------------------------------------------------

#include <ctype.h>

#include "doomdef.h"

#include "z_zone.h"
#include "v_video.h"

#include "m_swap.h"

#include "i_system.h"

#include "w_wad.h"

#include "st_stuff.h"
#include "st_lib.h"
#include "r_local.h"


// in AM_map.c
extern boolean		automapactive;

//
// Hack display negative frags.
//  Loads and store the stminus lump.
//
patch_t*		sttminus;

void STlib_init(void)
{
    sttminus = (patch_t *) W_CacheLumpName("STTMINUS", PU_STATIC);
}


// ?
void
STlib_initNum
( st_number_t*		n,
  int			x,
  int			y,
  patch_t**		pl,
  int*			num,
  boolean*		on,
  int			width )
{
    n->x	= x;
    n->y	= y;
    n->oldnum	= 0;
    n->width	= width;
    n->num	= num;
    n->on	= on;
    n->p	= pl;
}


//
// A fairly efficient way to draw a number
//  based on differences from the old number.
// Note: worth the trouble?
//
void STlib_drawNum(st_number_t* n, boolean refresh)
{
    int numdigits = n->width;
    int num = *n->num;

    refresh = false;

    int w = SHORT(n->p[0]->width);
#if 0
    int h = SHORT(n->p[0]->height);
#endif
    int x = n->x;
    const int y = n->y;

    int neg;

    n->oldnum = *n->num;

    neg = num < 0;

    if (neg)
    {
        if (numdigits == 2 && num < -9)
        {
            num = -9;
        }
        else if (numdigits == 3 && num < -99)
        {
            num = -99;
        }
        num = -num;
    }

    // clear the area
    x = n->x - numdigits*w;

#ifdef RANGECHECK
    unsigned int c = y - ST_Y;

    if (c > y)
    {
        I_Error("STlib_drawNum: n->y - ST_Y < 0; %d - %d < 0", y, ST_Y);
    }
#endif

    // changes from not using 8bpp framebuffers, no background to copy from
#if 0
    V_CopyRect(x, y - ST_Y, 0, w*numdigits, h, x, y, 5);
#endif

    // if non-number, do not draw it
    if (num == 1994)
        return;

    x = n->x;

    // in the special case of 0, you draw 0
    if (!num)
        V_DrawPatch(x - w, y, FG, n->p[ 0 ]);

    // draw the new number
    while (num && numdigits--)
    {
        x -= w;
        V_DrawPatch(x, y, FG, n->p[ num % 10 ]);
        num /= 10;
    }

    // draw a minus sign if necessary
    if (neg)
    {
        V_DrawPatch(x - 8, y, FG, sttminus);
    }
}


//
void STlib_updateNum(st_number_t* n, boolean refresh)
{
    if (*n->on)
    {
        STlib_drawNum(n, refresh);
    }
}


//
void
STlib_initPercent
( st_percent_t*		p,
  int			x,
  int			y,
  patch_t**		pl,
  int*			num,
  boolean*		on,
  patch_t*		percent )
{
    STlib_initNum(&p->n, x, y, pl, num, on, 3);
    p->p = percent;
}




void
STlib_updatePercent
( st_percent_t*		per,
  int			refresh )
{
    if (refresh && *per->n.on)
    {
        V_DrawPatch(per->n.x, per->n.y, FG, per->p);
    }

    STlib_updateNum(&per->n, refresh);
}



void
STlib_initMultIcon
( st_multicon_t*	i,
  int			x,
  int			y,
  patch_t**		il,
  int*			inum,
  boolean*		on )
{
    i->x	= x;
    i->y	= y;
    i->oldinum 	= -1;
    i->inum	= inum;
    i->on	= on;
    i->p	= il;
}


void
STlib_updateMultIcon
( st_multicon_t*	mi,
  boolean		refresh )
{
    if (*mi->on && (mi->oldinum != *mi->inum || refresh) && (*mi->inum!=-1))
    {
        V_DrawPatch(mi->x, mi->y, FG, mi->p[*mi->inum]);
        mi->oldinum = *mi->inum;
    }
}


void STlib_initBinIcon
( st_binicon_t*		b,
  int			x,
  int			y,
  patch_t*		i,
  boolean*		val,
  boolean*		on )
{
    b->x	= x;
    b->y	= y;
    b->oldval	= 0;
    b->val	= val;
    b->on	= on;
    b->p	= i;
}


void STlib_updateBinIcon(st_binicon_t* bi, boolean refresh)
{
    if (*bi->on && (bi->oldval != *bi->val || refresh))
    {
        if (*bi->val)
        {
            V_DrawPatch(bi->x, bi->y, FG, bi->p);
        }
        bi->oldval = *bi->val;
    }
}
