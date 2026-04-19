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
#ifndef __P_ANIM_H__
#define __P_ANIM_H__

typedef struct
{
    int index;
    int tics;
} hframeDef_t;

typedef struct
{
    int type;
    int index;
    int tics;
    int currentFrameDef;
    int startFrameDef;
    int endFrameDef;
    int frameCount;
} hanimDef_t;

/*
typedef enum
{
    type_flat,
    type_texture
} e_animtype_t;

typedef struct
{
    char *name;
    int tic;
} animpic_t;

typedef struct
{
    e_animtype_t type;
    char *startpic;
    char *endpic;
    int tics;

    animpic_t *pic;
} p_animdefs_t;
*/

extern void P_AnimateSurfaces(void);
//void P_ResetAnimatedSurfaces(void);
void P_InitHexenAnims(void);

#endif

// EOF