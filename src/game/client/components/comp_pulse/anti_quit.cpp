/* (c) DDNetPulse contributors. See licence.txt in the root of the distribution for more information. */

#include "anti_quit.h"

#include <engine/client.h>

#include <game/client/gameclient.h>

void CAntiQuit::OnInterfacesInit(CGameClient *pClient)
{
	CComponent::OnInterfacesInit(pClient);
}

void CAntiQuit::Request()
{
	if(g_Config.m_ClAntiRQ)
	{
		if(m_PopupRequested)
			return;

		m_PopupRequested = true;

		if(Client()->State() == IClient::STATE_ONLINE)
		{
			GameClient()->m_Menus.SetActive(true);
			GameClient()->m_Menus.SetMenuPage(CMenus::PAGE_GAME); //ddnet fix pages
		}
		else
		{
			GameClient()->m_Menus.SetShowStart(true);
			GameClient()->m_Menus.SetMenuPage(CMenus::PAGE_NEWS);
		}

		GameClient()->m_Menus.ShowQuitPopup();
	}
}

void CAntiQuit::OnUpdate()
{
	if(!m_PopupRequested)
		return;

	if(GameClient()->m_Menus.m_Popup != CMenus::POPUP_QUIT)
	{
		m_PopupRequested = false;
	}
}
