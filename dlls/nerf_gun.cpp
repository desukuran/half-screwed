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
#if !defined( OEM_BUILD ) && !defined( HLDEMO_BUILD )

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"

#ifndef CLIENT_DLL
#define NERF_AIR_VELOCITY	1200
#define NERF_WATER_VELOCITY	5

// UNDONE: Save/restore this?  Don't forget to set classname and LINK_ENTITY_TO_CLASS()
// 
// OVERLOADS SOME ENTVARS:
//
// speed - the ideal magnitude of my velocity
class CNerfDart : public CBaseEntity
{
	void Spawn( void );
	void Precache( void );
	int  Classify ( void );
	void EXPORT BubbleThink( void );
	void EXPORT WallThink( void );
	void EXPORT DartTouch( CBaseEntity *pOther );
	void EXPORT ExplodeThink( void );

	int m_iTrail;
	float m_flDieTime;
	float m_flStickTime;

public:
	static CNerfDart *DartCreate( void );
};
LINK_ENTITY_TO_CLASS( nerf_dart, CNerfDart );

CNerfDart *CNerfDart::DartCreate( void )
{
	// Create a new entity with CCrossbowBolt private data
	CNerfDart *pDart = GetClassPtr( (CNerfDart *)NULL );
	pDart->pev->classname = MAKE_STRING("dart");
	pDart->Spawn();

	return pDart;
}

void CNerfDart::Spawn( )
{
	Precache( );
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	pev->gravity = 1;
	//pev->velocity = Vector(0, 0, -50);

	SET_MODEL(ENT(pev), "models/nerf_dart.mdl");

	UTIL_SetOrigin( pev, pev->origin );
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));

	SetTouch( &CNerfDart::DartTouch );
	SetThink( &CNerfDart::BubbleThink );
	pev->nextthink = gpGlobals->time + 0.2;
}


void CNerfDart::Precache( )
{
	PRECACHE_MODEL ("models/nerf_dart.mdl");
	PRECACHE_SOUND("weapons/nstar_hitbod.wav");
	PRECACHE_SOUND("weapons/nerf_dartstick.wav");
	PRECACHE_SOUND("weapons/nerf_fire1.wav");
	PRECACHE_SOUND("weapons/nerf_fire2.wav");
	PRECACHE_SOUND("weapons/nerf_bolt.wav");
	PRECACHE_SOUND("fvox/beep.wav");
	m_iTrail = PRECACHE_MODEL("sprites/streak.spr");
}


int	CNerfDart :: Classify ( void )
{
	return	CLASS_NONE;
}

void CNerfDart::DartTouch( CBaseEntity *pOther )
{
	SetTouch( NULL );
	SetThink( NULL );

	if (pOther->pev->takedamage)
	{
		TraceResult tr = UTIL_GetGlobalTrace( );
		entvars_t	*pevOwner;

		pevOwner = VARS( pev->owner );

		// UNDONE: this needs to call TraceAttack instead
		ClearMultiDamage( );

		pOther->TraceAttack(pevOwner, gSkillData.plrDmgStar, pev->velocity.Normalize(), &tr, DMG_BULLET | DMG_NEVERGIB ); 

		ApplyMultiDamage( pev, pevOwner );

		pev->velocity = Vector( 0, 0, 0 );
		// play body "errgh" sound
		EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/nstar_hitbod.wav", 1, ATTN_NORM);

		if ( !g_pGameRules->IsMultiplayer() )
		{
		switch (RANDOM_LONG(1,2)) 
			{
				case 1: 
					Killed( pev, GIB_ALWAYS ); // Victim is lucky to have a heart at this point
					break;
				case 2: 
					Killed( pev, GIB_NEVER ); // Victim is lucky and keeps his organs in his corpse
					break;
			}
		}

		UTIL_Remove( this );
	}
	else
	{
		EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, "weapons/nerf_dartstick.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 98 + RANDOM_LONG(0,7));

		SetThink( &CNerfDart::WallThink );
		pev->nextthink = gpGlobals->time;// this will get changed below if the bolt is allowed to stick in what it hit.
		m_flStickTime = gpGlobals->time + RANDOM_FLOAT( 0, 10 );

		if ( FClassnameIs( pOther->pev, "worldspawn" ) )
		{
			// if what we hit is static architecture, can stay around for a while.
			Vector vecDir = pev->velocity.Normalize( );
			UTIL_SetOrigin( pev, pev->origin - vecDir * 18 );
			pev->angles = UTIL_VecToAngles( vecDir );
			pev->solid = SOLID_NOT;
			pev->movetype = MOVETYPE_FLY;
			pev->velocity = Vector( 0, 0, 0 );
			pev->avelocity.z = 0;
			pev->angles.z = RANDOM_LONG(0,360);
			pev->nextthink = gpGlobals->time + RANDOM_FLOAT( 0, 10 ); //TODO: Change
		}

		/*if (UTIL_PointContents(pev->origin) != CONTENTS_WATER)
		{
			UTIL_Sparks( pev->origin );
		}*/
	}
}

void CNerfDart::BubbleThink( void )
{
	pev->nextthink = gpGlobals->time + 0.1;

	pev->velocity.z -= 15;

	if (pev->waterlevel == 0)
		return;

	UTIL_BubbleTrail( pev->origin - pev->velocity * 0.1, pev->origin, 20 );
}

void CNerfDart::WallThink( void )
{
	pev->nextthink = gpGlobals->time + 0.1;

	if (gpGlobals->time > m_flStickTime)
	{
		if (!m_flDieTime)
			m_flDieTime = gpGlobals->time + 10;

		pev->velocity.z -= 30;
	}

	if (gpGlobals->time > m_flDieTime)
		SetThink( &CBaseEntity::SUB_Remove );
}

void CNerfDart::ExplodeThink( void )
{
	int iContents = UTIL_PointContents ( pev->origin );
	int iScale;
	
	pev->dmg = 40;
	iScale = 10;

	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION);		
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		if (iContents != CONTENTS_WATER)
		{
			WRITE_SHORT( g_sModelIndexFireball );
		}
		else
		{
			WRITE_SHORT( g_sModelIndexWExplosion );
		}
		WRITE_BYTE( iScale  ); // scale * 10
		WRITE_BYTE( 15  ); // framerate
		WRITE_BYTE( TE_EXPLFLAG_NONE );
	MESSAGE_END();

	entvars_t *pevOwner;

	if ( pev->owner )
		pevOwner = VARS( pev->owner );
	else
		pevOwner = NULL;

	pev->owner = NULL; // can't traceline attack owner if this is set

	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, 128, CLASS_NONE, DMG_BLAST | DMG_ALWAYSGIB );

	UTIL_Remove(this);
}
#endif

enum nerf_e { // Do this!@
	NERF_IDLE = 0,	// full
	NERF_FIRE,		// full
	NERF_DRAW,		// full
};

LINK_ENTITY_TO_CLASS( weapon_nerfgun, CNerfGun );

void CNerfGun::Spawn( )
{
	Precache( );
	m_iId = WEAPON_NERFGUN;
	SET_MODEL(ENT(pev), "models/w_nerfgun.mdl");

	m_iDefaultAmmo = NSTAR_DEFAULT_GIVE;

	FallInit();// get ready to fall down.
}

int CNerfGun::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

void CNerfGun::Precache( void )
{
	PRECACHE_MODEL("models/w_nerfgun.mdl");
	PRECACHE_MODEL("models/v_nerfgun.mdl");
	PRECACHE_MODEL("models/p_nerfgun.mdl");

	UTIL_PrecacheOther( "nerf_dart" );

	m_usNerfGun = PRECACHE_EVENT( 1, "events/nerfgun.sc" );
}


int CNerfGun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "darts";
	p->iMaxAmmo1 = NSTAR_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = NSTAR_MAX_CLIP;
	p->iSlot = 5;
	p->iPosition = 2;
	p->iId = WEAPON_NERFGUN;
	p->iFlags = 0;
	p->iWeight = NSTAR_WEIGHT;
	return 1;
}


BOOL CNerfGun::Deploy( )
{
	return DefaultDeploy( "models/v_nerfgun.mdl", "models/p_nerfgun.mdl", NERF_IDLE, "mp5" );
}

void CNerfGun::Holster( int skiplocal /* = 0 */ )
{
	if ( m_fInZoom )
	{
		SecondaryAttack( );
	}

	m_fInReload = FALSE;// cancel any reload in progress.
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
}

void CNerfGun::PrimaryAttack( void )
{
	if (m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] > 0)
	{
		FireDart();
	}
}

void CNerfGun::SecondaryAttack()
{
	if ( m_pPlayer->pev->fov != 0 )
	{
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 0; // 0 means reset to default fov
		m_fInZoom = 0;
	}
	else if ( m_pPlayer->pev->fov != 20 )
	{
		m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 20;
		m_fInZoom = 1;
	}
	
	pev->nextthink = UTIL_WeaponTimeBase() + 0.1;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
}

void CNerfGun::FireDart()
{
	TraceResult tr;

	if (m_iClip == 0)
	{
		PlayEmptySound( );
		return;
	}

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;

	//m_iClip--;
	m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ]--;

	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usNerfGun, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0, 0, m_iClip, m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType], 0, 0 );

	m_flPumpTime = gpGlobals->time;	

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector anglesAim = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
	UTIL_MakeVectors( anglesAim );
	
	anglesAim.x		= -anglesAim.x;
	Vector vecSrc	 = m_pPlayer->GetGunPosition( ) - gpGlobals->v_up * 2;
	Vector vecDir	 = gpGlobals->v_forward;

#ifndef CLIENT_DLL
	CNerfDart *pDart = CNerfDart::DartCreate();
	pDart->pev->origin = vecSrc;
	pDart->pev->angles = anglesAim;
	pDart->pev->owner = m_pPlayer->edict();

	if (m_pPlayer->pev->waterlevel == 3)
	{
		pDart->pev->velocity = vecDir * NERF_WATER_VELOCITY;
		pDart->pev->speed = NERF_WATER_VELOCITY;
	}
	else
	{
		pDart->pev->velocity = vecDir * NERF_AIR_VELOCITY;
		pDart->pev->velocity.z -= 15;
		pDart->pev->speed = NERF_AIR_VELOCITY;
	}
	pDart->pev->avelocity.z = 10;
#endif

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.8;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.75;

	if (m_iClip != 0)
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 5.0;
	else
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.75;
}

void CNerfGun::WeaponIdle( void )
{
	m_pPlayer->GetAutoaimVector( AUTOAIM_2DEGREES );  // get the autoaim vector but ignore it;  used for autoaim crosshair in DM

	if ( m_flPumpTime && m_flPumpTime < gpGlobals->time )
	{
		SendWeaponAnim( NERF_DRAW );
		// play pumping sound
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/nerf_bolt.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,0x1f));
		m_flPumpTime = 0;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.8;
		m_fBoltTime = UTIL_WeaponTimeBase() + 1.8;
	}
	
	if ( m_fBoltTime < gpGlobals->time )
		return;

	ResetEmptySound( );
	
	SendWeaponAnim( NERF_IDLE );
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 80.0 / 30.0;
}



class CNerfDartAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_nstar.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_nstar.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		if (pOther->GiveAmmo( 30, "darts", NSTAR_MAX_CARRY ) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( ammo_ndart, CNerfDartAmmo );

#endif