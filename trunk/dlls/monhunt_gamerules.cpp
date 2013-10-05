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
		pPlayer->GiveNamedItem( "weapon_9mmhandgun" );
		pPlayer->GiveNamedItem( "ammo_9mmclip" );
		pPlayer->GiveNamedItem( "weapon_shotgun" );
		pPlayer->GiveNamedItem( "ammo_buckshot" );
		pPlayer->GiveNamedItem( "weapon_9mmAR" );
		pPlayer->GiveNamedItem( "ammo_9mmAR" );
		pPlayer->GiveNamedItem( "ammo_ARgrenades" );
		pPlayer->GiveNamedItem( "weapon_handgrenade" );
		pPlayer->GiveNamedItem( "weapon_tripmine" );
		pPlayer->GiveNamedItem( "weapon_rpg" );
		pPlayer->GiveNamedItem( "ammo_rpgclip" );
		pPlayer->GiveNamedItem( "weapon_satchel" );
		pPlayer->GiveNamedItem( "weapon_snark" );
		pPlayer->GiveNamedItem( "weapon_soda" );
		pPlayer->GiveNamedItem( "weapon_dosh" );
		pPlayer->GiveNamedItem( "weapon_beamkatana" );
		pPlayer->GiveNamedItem( "weapon_ak47" );
		pPlayer->GiveNamedItem( "weapon_bow" );
		pPlayer->GiveNamedItem( "weapon_jason" );
		pPlayer->GiveNamedItem( "weapon_jihad" );
		pPlayer->GiveNamedItem( "weapon_jackal" );
		pPlayer->GiveNamedItem( "weapon_nstar" );
		pPlayer->GiveNamedItem( "weapon_mw2" );
		pPlayer->GiveNamedItem( "weapon_zapper" );
		pPlayer->GiveNamedItem( "weapon_goldengun" );
		pPlayer->GiveNamedItem( "weapon_boombox" );
		pPlayer->GiveNamedItem( "weapon_scientist" );
		pPlayer->GiveNamedItem( "weapon_modman" );
	}
}

void CMonsterplay::PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
		CBaseEntity *pKira = CBaseEntity::Instance(pKiller);

		if (pKira->Classify() != CLASS_PLAYER && pKira->pev->flags & FL_MONSTER)
		{
			char buf[128];

			sprintf(buf, "%s: killed by %s!", STRING(pVictim->pev->netname), STRING(pKira->pev->classname));

			UTIL_SayTextAllHS( buf );
		}
		else
			return;
}

int CMonsterplay::PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget )
{
	if ( !pPlayer || !pTarget || !pTarget->IsPlayer() )
		return GR_NOTTEAMMATE;

	// monster hunt has only teammates
	return GR_TEAMMATE;
}

BOOL CMonsterplay::FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker )
{
	if ( pAttacker && PlayerRelationship( pPlayer, pAttacker ) == GR_TEAMMATE )
	{
		// my teammate hit me.
		if ( (friendlyfire.value == 0) && (pAttacker != pPlayer) )
		{
			// friendly fire is off, and this hit came from someone other than myself,  then don't get hurt
			return FALSE;
		}
	}

	return CHalfLifeMultiplay::FPlayerCanTakeDamage( pPlayer, pAttacker );
}

int CMonsterplay::iKillforMonster(const char *classname)
{
	if ( !strcmp( classname, "Gay Glenn" ) )
		return 1;
	else if ( !strcmp( classname, "Barney" ) )
		return 2;
	else if ( !strcmp( classname, "X-Mas Tree" ) )
		return 5;
	else if ( !strcmp( classname, "Scientist" ) )
		return 1;
	else if ( !strcmp( classname, "Sinistar" ) )
		return 3;
	else if ( !strcmp( classname, "Chris-Chan" ) )
		return 0;
	else if ( !strcmp( classname, "Zombie" ) )
		return 2;
	else
		return 1;
}