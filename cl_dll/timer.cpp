//========= Copyright © 2011, Half-Screwed Team, Released under the "Do whatever you want" license. ============//
//																									  //
// Purpose:																							  //
//																									  //
// $NoKeywords: $																					  //
//====================================================================================================//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>
#include "mp3.h"

float g_iRoundtime;
float g_iRoundTotal;
int g_iTimeLeft;

DECLARE_MESSAGE(m_Timer, Timer)
int CHudTimer::Init(void)
{
	HOOK_MESSAGE( Timer ); 
	gHUD.AddHudElem(this); 
	return 1; 
};

int CHudTimer::VidInit(void)
{
	gHUD.m_iRoundtime = -1;
	g_hud_timerbg = gHUD.GetSpriteIndex( "nestimerbg" );
	g_hud_timercolon = gHUD.GetSpriteIndex( "nestime" );
 	return 1;
};

int CHudTimer::MsgFunc_Timer( const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	int x = READ_LONG();
	g_iRoundtime = gHUD.m_flTime + x;
	g_iRoundTotal = x;
	g_iTimeLeft = floor(g_iRoundtime - gHUD.m_flTime);
	gHUD.m_iRoundtime = x;

	m_iFlags |= HUD_ACTIVE;
 	return 1;
}

int CHudTimer::Draw( float flTime )
{	
	g_iTimeLeft = floor(g_iRoundtime - gHUD.m_flTime);

	if (g_iTimeLeft <= 0)
		return 1;

		if (CVAR_GET_FLOAT("hud_timer") == 0)
			return 1;

		//Draw BG
		SPR_Set(gHUD.GetSprite(g_hud_timerbg), 255, 255, 255 );
		SPR_Draw(0, (ScreenWidth/2-69), 0, &gHUD.GetSpriteRect(g_hud_timerbg));

		//Draw the numbers and colon
		gHUD.DrawHudNumberNES((ScreenWidth/2)-44, 20, DHN_2DIGITS | DHN_PREZERO, g_iTimeLeft/60, 255, 255, 255); 
		SPR_Set(gHUD.GetSprite(g_hud_timercolon), 255, 255, 255 );
		SPR_Draw(0, (ScreenWidth/2)-4, 20, &gHUD.GetSpriteRect(g_hud_timercolon));							
		gHUD.DrawHudNumberNES((ScreenWidth/2)+6, 20, DHN_2DIGITS | DHN_PREZERO, g_iTimeLeft%60, 255, 255, 255);  

	return 1;
}