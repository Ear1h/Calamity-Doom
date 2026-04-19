//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 1993-2008 Raven Software
// Copyright(C) 2005-2014 Simon Howard
// Copyright(C) 2026 Vladislav Putintsev (Ear1h)
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
//
// DESCRIPTION:
//	Multiple state switches
// 
//

/*
 Имплементация Анимированных переключателей
 Протестировав на Гоззе, понятно, что свитчи активируют дверь сразу, а не по мере окончания анимации, поэтому сложность внедрении отпадает.
 
 Текущее ТЗ:
	 Создание Lump ANIMDEFS
	 Чтение Lump ANIMDEFS. Буду брать за основу MUSINFO из Risen3d и ANIMDEFS из Hexen
	 Синтаксис должен быть следующим (почти идентичен гоззовской):
	 switch <base texture> on [activation sequence] tics off [deactivation sequence] tics
	 1. Каждое чтение должно начинаться с заголовка switch
	 2. Начиная с On мы вписываем одну текстуру. Tics для задержки.
	 3. Затем циклично проходим по всем текстурам до off состояния.
	 4. Off состояние действует также, как и on.

	 Единственная потенциальная сложность - как сохранить данное состояние текстуры во время save/load. 
	 Скорее всего нужно просмотреть анимированные двери из Страйфа, поскольку они работают по такому же принципу.

	 Ну и наконец, нужно внедрить в список существующих свитчей.
*/

#include <stdio.h>

#include "i_system.h"
#include "deh_main.h"
#include "doomdef.h"
#include "p_local.h"
#include "i_swap.h" // [crispy] SHORT()
#include "w_wad.h" // [crispy] W_CheckNumForName()
#include "z_zone.h" // [crispy] PU_STATIC

#include "g_game.h"
#include "m_random.h"

#include "s_sound.h"

// Data.
#include "sounds.h"

// State.
#include "doomstat.h"
#include "r_state.h"
#include "s_musinfo.h"
#include "m_scanner.h"
#include "p_spec.h"
#include "p_anim.h"
#include "m_array.h"
#include "p_animdefs.h"

static hanimDef_t *AnimDefs = NULL;
static hframeDef_t *FrameDefs = NULL;

static void P_HexenInitAnimations(p_animdefs_t *animdef)
{
    hanimDef_t hanimdef = {0};

    int fd = 0;

	hanimdef.type = animdef->type;
    if (hanimdef.type == 0)
	{
        if (W_CheckNumForName(animdef->startpic) == -1)
		{
			return;
		}

		hanimdef.index = R_FlatNumForName(animdef->startpic);
	}

	else
    {
        if (R_CheckTextureNumForName(animdef->startpic) == -1)
		{
			return;
		}

		hanimdef.index = R_TextureNumForName(animdef->startpic);
	}

	hanimdef.startFrameDef = array_size(FrameDefs);
    
	for (int i = 0; i < array_size(animdef->pic); i++)
	{
        hframeDef_t Framedef = {0};
        Framedef.index = R_TextureNumForName(animdef->pic[i].name);
        Framedef.tics = animdef->pic[i].mintic;
        if (Framedef.tics <= 0)
            Framedef.tics = 1;
        

        array_push(FrameDefs, Framedef);
	}

    
	hanimdef.endFrameDef = array_size(FrameDefs) - 1;
    hanimdef.currentFrameDef = hanimdef.endFrameDef;
    hanimdef.tics = 1; 

    array_push(AnimDefs, hanimdef);
}

void P_InitHexenAnims(void)
{
    for (int i = 0; i < array_size(animations); i++)
    {
        if (array_size(animations->pic) < 1)
            continue;
        P_HexenInitAnimations(&animations[i]);
    }
}

static void P_Hexen(hanimDef_t *had)
{
    if (!--had->tics)
    {
        if (had->currentFrameDef == had->endFrameDef)
        {
            had->currentFrameDef = had->startFrameDef;
        }
        else
        {
            had->currentFrameDef++;
        }

        const hframeDef_t *hfd = &FrameDefs[had->currentFrameDef];
        had->tics = FrameDefs[had->currentFrameDef].tics;
        if (had->type == 0)
        {
            flattranslation[had->index] = FrameDefs[had->currentFrameDef].index;
        }
        else
        { // Texture
            texturetranslation[had->index] =
                FrameDefs[had->currentFrameDef].index;
        }
    }


}
void P_AnimateSurfaces(void)
{
    for (int i = 0; i < array_size(AnimDefs); i++)
	{
        P_Hexen(&AnimDefs[i]);
	}
}
