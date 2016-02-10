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
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"


#define	HANDGRENADE_PRIMARY_VOLUME		450

enum handgrenade_e {
	HANDGRENADE_IDLE = 0,
	HANDGRENADE_FIDGET,
	HANDGRENADE_THROW1,	// toss
	HANDGRENADE_HOLSTER,
	HANDGRENADE_DRAW
};

#ifndef CLIENT_DLL

#define POKEBALL_AIR_VELOCITY 1800

class CPokeballWorld : public CBaseEntity
{
	void Spawn( void );
	void Precache( void );
	void EXPORT PokeballTouch( CBaseEntity *pOther );

public:
	static CPokeballWorld *PokeballCreate( void );
};

CPokeballWorld *CPokeballWorld::PokeballCreate( void )
{
	CPokeballWorld *pBall = GetClassPtr( (CPokeballWorld *)NULL );
	pBall->pev->classname = MAKE_STRING("pokeball");
	pBall->Spawn();

	return pBall;
}

void CPokeballWorld::Precache( void )
{
	PRECACHE_MODEL("models/w_pokeball.mdl");
	PRECACHE_SOUND("weapons/pokeball_bounce.wav");
}

void CPokeballWorld::Spawn( void )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/w_pokeball.mdl");
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	pev->gravity = 1;

	SetTouch( &CPokeballWorld::PokeballTouch );

	UTIL_SetOrigin( pev, pev->origin );
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));
}

void CPokeballWorld::PokeballTouch( CBaseEntity *pOther )
{
	// don't hit the guy that launched this grenade
	if ( pOther->edict() == pev->owner )
		return;

	Vector vecTestVelocity;
	// pev->avelocity = Vector (300, 300, 300);

	// this is my heuristic for modulating the grenade velocity because grenades dropped purely vertical
	// or thrown very far tend to slow down too quickly for me to always catch just by testing velocity. 
	// trimming the Z velocity a bit seems to help quite a bit.
	vecTestVelocity = pev->velocity; 
	vecTestVelocity.z *= 0.45;

	if (pev->flags & FL_ONGROUND)
	{
		// add a bit of static friction
		pev->velocity = pev->velocity * 0.8;

		pev->sequence = RANDOM_LONG( 1, 1 );
	}
	else
	{
		// play bounce sound
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/pokeball_bounce.wav", 0.5, ATTN_NORM);
	}
	pev->framerate = pev->velocity.Length() / 200.0;
	if (pev->framerate > 1.0)
		pev->framerate = 1;
	else if (pev->framerate < 0.5)
		pev->framerate = 0;
}

LINK_ENTITY_TO_CLASS( wrld_pokeball, CPokeballWorld );
#endif

LINK_ENTITY_TO_CLASS( weapon_pokeball, CPokeBall );


void CPokeBall::Spawn( )
{
	Precache( );
	m_iId = WEAPON_POKEBALL;
	SET_MODEL(ENT(pev), "models/w_pokeball.mdl");

	FallInit();// get ready to fall down.
}


void CPokeBall::Precache( void )
{
	PRECACHE_MODEL("models/w_pokeball.mdl");
	PRECACHE_MODEL("models/v_pokeball.mdl");
	PRECACHE_MODEL("models/p_pokeball.mdl");

	UTIL_PrecacheOther( "wrld_pokeball" );
}

int CPokeBall::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 5;
	p->iPosition = 3;
	p->iId = m_iId = WEAPON_POKEBALL;
	p->iWeight = POKEBALL_WEIGHT;
	p->iFlags = 0;

	return 1;
}


BOOL CPokeBall::Deploy( )
{
	m_flReleaseThrow = -1;
	return DefaultDeploy( "models/v_pokeball.mdl", "models/p_pokeball.mdl", HANDGRENADE_DRAW, "crowbar" );
}

BOOL CPokeBall::CanHolster( void )
{
	// can only holster hand grenades when not primed!
	return ( m_flStartThrow == 0 );
}

void CPokeBall::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;

	SendWeaponAnim( HANDGRENADE_HOLSTER );

	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);
}

void CPokeBall::PrimaryAttack()
{
		Vector angThrow = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;

		if ( angThrow.x < 0 )
			angThrow.x = -10 + angThrow.x * ( ( 90 - 10 ) / 90.0 );
		else
			angThrow.x = -10 + angThrow.x * ( ( 90 + 10 ) / 90.0 );

		float flVel = ( 90 - angThrow.x ) * 4;
		if ( flVel > 500 )
			flVel = 500;

		UTIL_MakeVectors( angThrow );

		Vector vecSrc = m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16;

		Vector vecThrow = gpGlobals->v_forward * flVel + m_pPlayer->pev->velocity;

		Vector anglesAim = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;

		// alway explode 3 seconds after the pin was pulled
		float time = 3.0;

#ifndef CLIENT_DLL
		CPokeballWorld *pBall = CPokeballWorld::PokeballCreate();
		pBall->pev->origin = vecSrc;
		pBall->pev->angles = anglesAim;
		pBall->pev->owner = m_pPlayer->edict();

		pBall->pev->velocity = gpGlobals->v_forward * POKEBALL_AIR_VELOCITY;
#endif

		SendWeaponAnim( HANDGRENADE_THROW1 );

		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
}


void CPokeBall::WeaponIdle( void )
{

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

		int iAnim;
		float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
		if (flRand <= 0.75)
		{
			iAnim = HANDGRENADE_IDLE;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );// how long till we do this again.
		}
		else 
		{
			iAnim = HANDGRENADE_FIDGET;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 75.0 / 30.0;
		}

		SendWeaponAnim( iAnim );
}




