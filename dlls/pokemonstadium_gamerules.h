//
// pokemonstadium_gamerules.h
//
#if 0
class CPokemonStadiumplay : public CHalfLifeMultiplay
{
public:
	virtual const char *GetGameDescription( void ) { return "Pokemon Stadium Mode"; }  // this is the game name that gets seen in the server browser
	virtual BOOL IsPokemon( void );
	virtual void PlayerSpawn( CBasePlayer *pPlayer );
	virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
	virtual BOOL FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker );
	virtual BOOL CanHavePlayerItem( CBasePlayer *pPlayer, CBasePlayerItem *pWeapon );// The player is touching an CBasePlayerItem, do I give it to him?
	virtual BOOL CanHaveAmmo( CBasePlayer *pPlayer, const char *pszAmmoName, int iMaxCarry );// can this player take more of this ammo?
};

#endif 0