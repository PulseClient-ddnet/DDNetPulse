#include "afk_aura.h"
#include <game/client/gameclient.h>
#include <game/client/components/effects.h>


void CAfkAura::OnInit() {}

void CAfkAura::OnRender()
{
	if(Client()->State() != IClient::STATE_ONLINE && Client()->State() != IClient::STATE_DEMOPLAYBACK)
		return;

	int ClientId = GameClient()->m_Snap.m_LocalClientId;
	if(ClientId < 0)
		return;

	const CGameClient::CSnapState::CCharacterInfo &CharacterInfo = GameClient()->m_Snap.m_aCharacters[ClientId];
	if(!CharacterInfo.m_Active)
		return;

	vec2 Position = GameClient()->m_aClients[ClientId].m_RenderPos;
	float CurrentTime = Client()->LocalTime();

	const float MovementThreshold = 1.0f;
	const float IdleDelay = g_Config.m_ClPlayerIdleAuraTimer;
	const float FadeInTime = 2.0f;
	const float PostFadeTime = 2.5f;

	bool IsMoving = length(Position - m_LastPosition) > MovementThreshold;
	m_LastPosition = Position;

	if(IsMoving)
	{
		if(m_AuraActive)
		{
			m_AuraActive = false;
			m_FadeOutStartTime = CurrentTime;
		}
		m_LastMovementTime = CurrentTime;
	}
	else if(!m_AuraActive && CurrentTime - m_LastMovementTime >= IdleDelay)
	{
		m_AuraActive = true;
		m_AuraPosition = Position;
	}

	float Alpha = 0.0f;
	bool RenderAura = false;

	if(m_AuraActive && g_Config.m_ClPlayerIdleAura)
	{
		float TimeSinceIdle = CurrentTime - m_LastMovementTime - IdleDelay;
		float FadeInProgress = TimeSinceIdle / FadeInTime;
		Alpha = std::clamp(FadeInProgress, 0.0f, 1.0f);
		RenderAura = true;
	}
	else if(!m_AuraActive && CurrentTime - m_FadeOutStartTime < PostFadeTime)
	{
		float TimeSinceFadeOut = CurrentTime - m_FadeOutStartTime;
		Alpha = 1.0f - std::clamp(TimeSinceFadeOut / PostFadeTime, 0.0f, 1.0f);
		RenderAura = true;
	}

	if(RenderAura && g_Config.m_ClPlayerIdleAura)
	{
		vec4 BodyColor = vec4(
			GameClient()->m_aClients[ClientId].m_RenderInfo.m_ColorBody.r,
			GameClient()->m_aClients[ClientId].m_RenderInfo.m_ColorBody.g,
			GameClient()->m_aClients[ClientId].m_RenderInfo.m_ColorBody.b,
			1.0f
			);

		GameClient()->m_Effects.AfkAura(m_AuraPosition, Alpha, BodyColor);
	}
}