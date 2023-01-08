/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_VARIABLES_H
#define GAME_VARIABLES_H
#undef GAME_VARIABLES_H // this file will be included several times

MACRO_CONFIG_INT(SvGeneratedMapWidth, sv_generated_map_width, 256, 64, 2000, CFGFLAG_SERVER, "generated map width")
MACRO_CONFIG_INT(SvGeneratedMapHeight, sv_generated_map_height, 128, 64, 2000, CFGFLAG_SERVER, "generated map height")


#endif
