#if 0
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h"
#include	"pokemonstadium_gamerules.h"
#include	"game.h"

BOOL CPokemonStadiumplay::IsPokemon( void )
{
	return TRUE;
}

void CPokemonStadiumplay::PlayerSpawn( CBasePlayer *pPlayer )
{
	BOOL		addDefault;
	CBaseEntity	*pWeaponEntity = NULL;

	pPlayer->pev->weapons |= (1<<WEAPON_SUIT);
	
	addDefault = TRUE;

	edict_t *pClient = g_engfuncs.pfnPEntityOfEntIndex( 1 );

	while ( pWeaponEntity = UTIL_FindEntityByClassname( pWeaponEntity, "game_player_equip" ))
	{
		pWeaponEntity->Touch( pPlayer );
		addDefault = FALSE;
	}

	SetBits( pPlayer->pev->flags, FL_NOTARGET );
	SetBits( pPlayer->pev->flags, FL_GODMODE );

	if ( addDefault )
		pPlayer->GiveNamedItem( "weapon_pokeball" );
}

BOOL CPokemonStadiumplay::CanHavePlayerItem(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon)
{
	if (pWeapon->m_iId != WEAPON_POKEBALL)
		return false;

	return CHalfLifeMultiplay::CanHavePlayerItem( pPlayer, pWeapon );
}

BOOL CPokemonStadiumplay::CanHaveAmmo( CBasePlayer *pPlayer, const char *pszAmmoName, int iMaxCarry )
{
	return false;
}

void CPokemonStadiumplay::PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
	return;
}

BOOL CPokemonStadiumplay::FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker )
{
	return FALSE;
}
#endif 0