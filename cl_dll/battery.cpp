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
// battery.cpp
//
// implementation of CHudBattery class
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

#include <string.h>
#include <stdio.h>

#define HUD_MGS3	0
#define HUD_HOTLINE 1
#define HUD_ZELDA	2

DECLARE_MESSAGE(m_Battery, Battery)

int CHudBattery::Init(void)
{
	m_iBat = 0;
	m_fFade = 0;
	m_iFlags = 0;

	HOOK_MESSAGE(Battery);

	gHUD.AddHudElem(this);

	return 1;
};


int CHudBattery::VidInit(void)
{
	int HUD_suit_empty = gHUD.GetSpriteIndex( "suit_empty" );
	int HUD_suit_full = gHUD.GetSpriteIndex( "suit_full" );

	m_HUD_mgs3suitbar = gHUD.GetSpriteIndex( "mgs3suitbar" );
	m_HUD_mgs3suitdiv = gHUD.GetSpriteIndex( "mgs3suitdiv" );

	m_HUD_zeldaheart = gHUD.GetSpriteIndex("zheart");
	m_HUD_zeldamagic = gHUD.GetSpriteIndex("zmagic");

	m_prc3 = &gHUD.GetSpriteRect(m_HUD_mgs3suitbar);		//Full

	m_iWidth = m_prc3->right - m_prc3->left;		//32 - 160 = -128

	m_SpriteHandle_t1 = m_SpriteHandle_t2 = 0;  // delaying get sprite handles until we know the sprites are loaded
	m_prc1 = &gHUD.GetSpriteRect( HUD_suit_empty );
	m_prc2 = &gHUD.GetSpriteRect( HUD_suit_full );
	m_iHeight = m_prc2->bottom - m_prc1->top;
	m_fFade = 0;
	return 1;
};

int CHudBattery:: MsgFunc_Battery(const char *pszName,  int iSize, void *pbuf )
{
	m_iFlags |= HUD_ACTIVE;

	
	BEGIN_READ( pbuf, iSize );
	int x = READ_SHORT();

	if (x != m_iBat)
	{
		m_fFade = FADE_TIME;
		m_iBat = x;
		m_flBat = ((float)x)/100.0;		//100.0 divided by 100.0 = 1
	}

	return 1;
}


int CHudBattery::Draw(float flTime)
{
	if ( gHUD.m_iHideHUDDisplay & HIDEHUD_HEALTH )
		return 1;

	wrect_t rc;

	rc = *m_prc2;
	rc.top  += m_iHeight * ((float)(100-(min(100,m_iBat))) * 0.01);	// battery can go from 0 to 100 so * 0.01 goes from 0 to 1

	if (!(gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
		return 1;
	
		if (CVAR_GET_FLOAT("hud_game") == HUD_MGS3)
		{
			int iOffset = (m_prc1->bottom - m_prc1->top)/6;

			int MGSX = (gHUD.GetSpriteRect(m_HUD_mgs3suitbar).right - gHUD.GetSpriteRect(m_HUD_mgs3suitbar).left)/10; //Needs 20. It's 104
			int MGSY = ScreenHeight-45; 

			int iOffset2 = m_iWidth * (1.0 - m_flBat);	//32 * (1 - 1) = 0

			if (iOffset2 < m_iWidth)
			{
				rc = *m_prc3;
				rc.left += iOffset2;

				SPR_Set(gHUD.GetSprite(m_HUD_mgs3suitbar), 255, 255, 255 );
				SPR_Draw(0, MGSX+5, MGSY+28, &rc);

				SPR_Set(gHUD.GetSprite(m_HUD_mgs3suitdiv), 255, 255, 255 );
				SPR_DrawHoles(0, MGSX+5, MGSY+28, &gHUD.GetSpriteRect(m_HUD_mgs3suitdiv));
			}
		}
		else if (CVAR_GET_FLOAT("hud_game") == HUD_HOTLINE)
		{
			//HotlineThink();
			int y = gHUD.GetSpriteRect(gHUD.m_HUD_number_0).bottom - gHUD.GetSpriteRect(gHUD.m_HUD_number_0).top;
			gHUD.DrawHudString(5+150, ScreenHeight-(y*2.5), 320, "ARMOR:", CHudHealth::hotline_r, CHudHealth::hotline_g, 255);
			gHUD.DrawHudNumber(5+150, ScreenHeight-(y+y/2), m_iFlags | DHN_3DIGITS, m_iBat, CHudHealth::hotline_r, CHudHealth::hotline_g, 255);
		}
		else if (CVAR_GET_FLOAT("hud_game") == HUD_ZELDA)
		{
			int ZeldaWidth = gHUD.GetSpriteRect(m_HUD_zeldaheart).right - gHUD.GetSpriteRect(m_HUD_zeldaheart).left;
			int MagicWidth = gHUD.GetSpriteRect(m_HUD_zeldamagic).right - gHUD.GetSpriteRect(m_HUD_zeldamagic).left;
			int x = ZeldaWidth*1.1;
			int y = (gHUD.m_iFontHeight*4) + (gHUD.m_iFontHeight / 2);
			int w = ((MagicWidth-(MagicWidth/16))*(m_flBat));

			SPR_Set(gHUD.GetSprite(m_HUD_zeldamagic), 255, 255, 255 );
			SPR_DrawHoles(0, x-(MagicWidth/32), y-(MagicWidth/32), &gHUD.GetSpriteRect(m_HUD_zeldamagic));
			FillRGBA(x, y, w, 16, 0, 0xFF, 0, 255);
		}
		else
		{
			int iOffset = (m_prc1->bottom - m_prc1->top)/6;

			int MGSX = (gHUD.GetSpriteRect(m_HUD_mgs3suitbar).right - gHUD.GetSpriteRect(m_HUD_mgs3suitbar).left)/10; //Needs 20. It's 104
			int MGSY = ScreenHeight-45; 

			int iOffset2 = m_iWidth * (1.0 - m_flBat);	//32 * (1 - 1) = 0

			if (iOffset2 < m_iWidth)
			{
				rc = *m_prc3;
				rc.left += iOffset2;

				SPR_Set(gHUD.GetSprite(m_HUD_mgs3suitbar), 255, 255, 255 );
				SPR_Draw(0, MGSX+5, MGSY+28, &rc);

				SPR_Set(gHUD.GetSprite(m_HUD_mgs3suitdiv), 255, 255, 255 );
				SPR_DrawHoles(0, MGSX+5, MGSY+28, &gHUD.GetSpriteRect(m_HUD_mgs3suitdiv));
			}
			
		}
		return 1;
}