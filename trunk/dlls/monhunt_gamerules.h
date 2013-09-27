//
// Monster_gamerules.h
//

class CMonsterplay : public CHalfLifeMultiplay
{
public:
	CMonsterplay();
	virtual const char *GetGameDescription( void ) { return "Monster Hunt Mode"; }  // this is the game name that gets seen in the server browser
	virtual BOOL IsMonster( void );
	virtual void PlayerSpawn( CBasePlayer *pPlayer );
	virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
	virtual int JasonsStolen( int jason );
};