//--------------------------------------------------------------------------
//
// Calamity Doom
// Copyright (C) 2026 Vladislav Putintsev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/
//

#include <string.h>
#include <stdlib.h>
#include "p_animdefs.h"
#include "m_scanner.h"
#include "m_misc.h"
#include "m_array.h"
#include "w_wad.h"
#include "z_zone.h"
#include "r_local.h"

#define ANIMFLAT "FLAT"
#define ANIMTEX   "TEXTURE"
#define ANIMFLATID 0
#define ANIMTEXID 1

/*
* Doom Syntax
    Texture	Optional	BTTS_1  	Range	BTTS_6  	Tics 1
    Flat	Optional	NUKAGE1 	Range	NUKAGE3 	Tics 8

* Hexen Syntax
* flat WATER1
      pic { name WATER1 random 4, 8 }
      pic { name WATER2 random 4, 8 }
      pic { name WATER3 random 4, 8 }
}
*/

p_animdefs_t *animations = NULL;

static boolean ValidateTextureOrFlat(scanner_t* s, p_animdefs_t* anim)
{
   if (!strcasecmp(SC_GetString(s), "Flat"))
   {
        anim->type = ANIMFLATID;
        return true;
   }
   else if (!strcasecmp(SC_GetString(s), "Texture"))
   {
        anim->type = ANIMTEXID;
        return true;
   }
   else
   {
        return false;
   }
}

static void AnimParseProperty(scanner_t *s, p_animdefs_t *anim)
{
    int tics;
    boolean done = false;
    while (!done)
    {
        SC_MustGetToken(s, TK_StringConst);
        anim->endpic = M_StringDuplicate(SC_GetString(s));

        SC_MustGetToken(s, TK_Identifier);

        SC_MustGetToken(s, TK_IntConst);
        tics = SC_GetNumber(s);
        if (tics < 1)
            SC_Error(s, "INFINITE TICS!");

        anim->tics = tics;
        break;
    }
}

static void AnimParsePicProperty(scanner_t *s, p_animdefs_t *anim)
{
    char *name;
    int tics;
    anim->pic = NULL;

    while (1)
    {
        SC_MustGetToken(s, '{');

        SC_MustGetToken(s, TK_Identifier);
        if (strcasecmp("name", SC_GetString(s)))
            SC_Error(s, "Expected 'name'");

        SC_MustGetToken(s, TK_StringConst);
        name = M_StringDuplicate(SC_GetString(s));

        SC_MustGetToken(s, TK_Identifier);
        if (strcasecmp("tics", SC_GetString(s)))
            SC_Error(s, "Expected 'tics'");

        SC_MustGetToken(s, TK_IntConst);
        tics = SC_GetNumber(s);

        if (tics < 1)
            SC_Error(s, "INFINITE TICS!");

        SC_MustGetToken(s, '}');

        animpic_t picparse = {name, tics};
        array_push(anim->pic, picparse);

        if (!SC_CheckToken(s, TK_Identifier))
            break;

        if (strcasecmp("pic", SC_GetString(s)))
        {
            SC_Rewind(s);
            break;
        }
    }

    if (array_size(anim->pic) < 2)
        SC_Error(s, "AnimDef has framecount < 2");
}

static void ParseAnimdefs(scanner_t *s, p_animdefs_t *anim)
{
    SC_MustGetToken(s, TK_Identifier);

    if (!ValidateTextureOrFlat(s, anim))
        SC_Error(s, "Unknown Identifier: Must be Texture or Flat");

    SC_MustGetToken(s, TK_StringConst);
    anim->startpic = M_StringDuplicate(SC_GetString(s));
    
    SC_GetNextRawString(s, true);
    if (!strcasecmp("Range", SC_GetString(s)))
    {
        AnimParseProperty(s, anim);
    }

    else if (!strcasecmp("pic", SC_GetString(s)))
    {
        AnimParsePicProperty(s, anim);
    }
}


static void FreeAnims(p_animdefs_t *anim)
{
    if (anim->startpic)
    {
        free(anim->startpic);
    }
    if (anim->endpic)
    {
        free(anim->endpic);
    }
    memset(anim, 0, sizeof(*anim));
}

void P_ProcessAnimation(void)
{
    int lumpnum = W_CheckNumForName("ANIMDEFS");
    if (lumpnum < 0)
    {
        return;
    }

    p_animdefs_t parsed = {0};
    scanner_t *s = SC_Open("ANIMDEFS", W_CacheLumpNum(lumpnum, PU_CACHE),
                           W_LumpLength(lumpnum));
    while (SC_TokensLeft(s))
    {
        ParseAnimdefs(s, &parsed);
        array_push(animations, parsed);
    }
   
    SC_Close(s);
}