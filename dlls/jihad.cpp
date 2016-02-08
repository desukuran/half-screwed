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
#include "explode.h"


#define	JIHAD_PRIMARY_VOLUME		450
#define YELL_TIME					1.8

enum jihad_e {
	JIHAD_IDLE = 0,
	JIHAD_SHOOT1,	// bring out
	JIHAD_SHOOT2,	// drop - never used in this
	JIHAD_SHOOT3,	// Punch buttons
};


LINK_ENTITY_TO_CLASS( weapon_jihad, CJihad );


void CJihad::Spawn( )
{
	Precache( );
	m_iId = WEAPON_JIHAD;
	SET_MODEL(ENT(pev), "models/w_jihad.mdl");
	m_iClip = -1;

#ifndef CLIENT_DLL
	pev->dmg = gSkillData.plrDmgJihad;
#endif

	FallInit();// get ready to fall down.
}


void CJihad::Precache( void )
{
	PRECACHE_MODEL("models/w_jihad.mdl");
	PRECACHE_MODEL("models/v_jihad.mdl");
	PRECACHE_MODEL("models/p_jihad.mdl");
	PRECACHE_MODEL("models/w_grenade.mdl");
	PRECACHE_SOUND("weapons/jihad.wav");
	m_usJihad = PRECACHE_EVENT( 1, "events/jihad.sc" );
}

int CJihad::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 4;
	p->iPosition = 4;
	p->iId = m_iId = WEAPON_JIHAD;
	p->iWeight = JIHAD_WEIGHT;
	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;

	return 1;
}


BOOL CJihad::Deploy( )
{
	//m_flReleaseThrow = -1;
	return DefaultDeploy( "models/v_jihad.mdl", "models/p_jihad.mdl", JIHAD_SHOOT1, "tripmine" );
}

void CJihad::Holster( int skiplocal /* = 0 */ )
{
}

void CJihad::PrimaryAttack()
{	
		if (m_fExplodeTime && gpGlobals->time < m_fExplodeTime)
		{
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + .2;
			return;
		}
		else if (m_fExplodeTime && gpGlobals->time > m_fExplodeTime)
		{
		#ifndef CLIENT_DLL
			ExplosionCreate( m_pPlayer->Center(), m_pPlayer->pev->angles, m_pPlayer->edict(), 1080, TRUE ); // BOOM!
		#endif
			return;
		}

		SendWeaponAnim( JIHAD_SHOOT3 );
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/jihad.wav", 1.0, ATTN_NORM);
		m_fExplodeTime = gpGlobals->time + YELL_TIME;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + .1;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + YELL_TIME;
}
void CJihad::WeaponIdle( void )
{
	if (m_fExplodeTime && gpGlobals->time > m_fExplodeTime)
	{
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 50; //Prevent the player from attacking again
		#ifndef CLIENT_DLL
			ExplosionCreate( m_pPlayer->Center(), m_pPlayer->pev->angles, m_pPlayer->edict(), 1080, TRUE ); // BOOM!
		#endif
	}

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.1;

		if (!m_fExplodeTime)
		{
			int iAnim;
			iAnim = JIHAD_IDLE;
			SendWeaponAnim( iAnim );
		}
}
