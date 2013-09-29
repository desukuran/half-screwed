/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// monhunt_gamerules.cpp
//
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h"
#include	"monhunt_gamerules.h"
#include	"game.h"
//#include	"mp3.h"

extern DLL_GLOBAL BOOL		g_fGameOver;
extern int gmsgScoreInfo;
extern int gmsgPlayMP3; //AJH - Killars MP3player

CMonsterplay :: CMonsterplay()
{
 //Genuflect
}

BOOL CMonsterplay::IsMonster()
{
 return TRUE;
}

void CMonsterplay::PlayerSpawn( CBasePlayer *pPlayer )
{
	BOOL		addDefault;
	CBaseEntity	*pWeaponEntity = NULL;

	pPlayer->pev->weapons |= (1<<WEAPON_SUIT);
	
	addDefault = TRUE;
	
	//entvars_t *pev = &pEntity->v;

//	CBasePlayer	*pPlayer;

	edict_t *pClient = g_engfuncs.pfnPEntityOfEntIndex( 1 );

	while ( pWeaponEntity = UTIL_FindEntityByClassname( pWeaponEntity, "game_player_equip" ))
	{
		pWeaponEntity->Touch( pPlayer );
		addDefault = FALSE;
	}

	if ( addDefault )
	{
		pPlayer->GiveNamedItem( "weapon_fotn" );
		//pPlayer->GiveAmmo( 15, "gold", GOLDENGUN_MAX_CARRY );
			//SERVER_COMMAND("mp3 play media/jayson.mp3\n");
	}
}

void CMonsterplay::PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
		
}

int CMonsterplay::iKillforMonster()
{
	return 1; //TODO: Filter by monster for different points.
}

int CMonsterplay::JasonsStolen(int jason)
{
return jason; //JASON! I found you!
}