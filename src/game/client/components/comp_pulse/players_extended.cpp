#include "players_extended.h"
#include <base/math.h>
#include <game/client/gameclient.h>

CPlayerExtended::CPlayerExtended()
{
	m_LastMovementTime = 0.0f;
	m_LastPosition = vec2(0, 0);
	m_LastAlpha = 0.0f;
}

void CPlayerExtended::OnInit()
{
	CPlayers::OnInit();
}

void CPlayerExtended::OnRender()
{
	CPlayers::OnRender();

	if(Client()->State() != IClient::STATE_ONLINE && Client()->State() != IClient::STATE_DEMOPLAYBACK)
		return;

	const CNetObj_Character *pPrevChar = nullptr;
	const CNetObj_Character *pPlayerChar = nullptr;
	int ClientId = GameClient()->m_Snap.m_LocalClientId;

	if(ClientId >= 0)
	{
		const CGameClient::CSnapState::CCharacterInfo &CharacterInfo = GameClient()->m_Snap.m_aCharacters[ClientId];
		if(CharacterInfo.m_Active)
		{
			const CGameClient::CClientData &ClientData = GameClient()->m_aClients[ClientId];
			pPrevChar = &ClientData.m_RenderPrev;
			pPlayerChar = &ClientData.m_RenderCur;

			vec2 Position = ClientData.m_RenderPos;

			float CurrentTime = Client()->LocalTime();
			float MovementThreshold = 0.1f;
			float DistanceMoved = length(Position - m_LastPosition);

			if(DistanceMoved > MovementThreshold)
				m_LastMovementTime = CurrentTime;

			m_LastPosition = Position;

			float TimeSinceLastMovement = CurrentTime - m_LastMovementTime;
			float FadeInTime = g_Config.m_ClPlayerIdleAuraTimer;
			float FadeOutTime = 0.2f;
			float BaseAlpha = 0.0f;

			if(TimeSinceLastMovement >= 2.0f)
			{
				float FadeInProgress = (TimeSinceLastMovement - 2.0f) / FadeInTime;
				BaseAlpha = clamp(FadeInProgress, 0.0f, 1.0f);
			}
			else if(m_LastAlpha > 0.0f)
			{
				float FadeOutProgress = TimeSinceLastMovement / FadeOutTime;
				BaseAlpha = m_LastAlpha * (1.0f - clamp(FadeOutProgress, 0.0f, 1.0f));
			}

			m_LastAlpha = BaseAlpha;
			if(g_Config.m_ClPlayerIdleAura)
			{
				if(BaseAlpha > 0.0f)
					RenderPlayerAura(Position, BaseAlpha, ClientData.m_RenderInfo.m_ColorBody);
			}
		}
	}
}

void CPlayerExtended::RenderPlayerAura(vec2 Position, float Alpha, vec4 BodyColor)
{
	const int NUM_SPARKLES = 12;
	const float SPARKLE_SIZE = 6.0f;
	const float AURA_RADIUS = 30.9f;
	const float VERTICAL_OFFSET = -17.0f;

	Graphics()->TextureSet(GameClient()->m_ExtrasSkin.m_SpriteParticleSparkle);
	Graphics()->BlendNormal();
	Graphics()->QuadsBegin();

	float Time = Client()->LocalTime() * 0.5f;
	vec2 BasePosition = Position + vec2(0, VERTICAL_OFFSET);

	for(int i = 0; i < NUM_SPARKLES; i++)
	{
		float BaseAngle = (float)i / NUM_SPARKLES * 2.0f * pi;
		float RotationSpeed = 0.1f + (float)(i % 4) * 0.15f;
		float RandomOffset = std::sin(Time * 0.3f + i * 0.7f) * 0.5f;
		float Angle = BaseAngle + Time * RotationSpeed + RandomOffset;

		float VerticalOffset = std::sin(Time * 0.8f + i * 0.5f) * 8.0f +
				       std::cos(Time * 0.4f + i * 0.3f) * 4.0f +
				       std::sin(Time * 1.2f + i * 0.2f) * 2.0f;

		float RadiusVariation = 1.0f + std::sin(Time * 0.6f + i * 0.4f) * 0.2f +
					std::cos(Time * 0.3f + i * 0.6f) * 0.1f;
		float CurrentRadius = AURA_RADIUS * RadiusVariation;

		vec2 SparklePos = BasePosition + vec2(std::cos(Angle) * CurrentRadius,
							 std::sin(Angle) * CurrentRadius + VerticalOffset);

		float SparkleAlpha = 0.4f + 0.3f * std::sin(Time * 1.2f + i * 0.4f) +
				     0.2f * std::cos(Time * 0.8f + i * 0.6f);

		float ColorPhase = Time * 0.3f + i * 0.2f;
		float ColorVariation = 0.15f * std::sin(ColorPhase);
		float ColorVariation2 = 0.1f * std::cos(ColorPhase * 1.5f);

		//player's color as base
		float R = BodyColor.r + ColorVariation;
		float G = BodyColor.g + ColorVariation2;
		float B = BodyColor.b + ColorVariation * 0.5f;

		float ColorPulse = 0.1f * std::sin(Time * 0.5f + i * 0.3f);
		R += ColorPulse;
		G += ColorPulse * 0.5f;
		B += ColorPulse * 0.3f;

		float SparkleRotation = Time * 2.0f + i * 0.5f +
					std::sin(Time * 0.7f + i * 0.4f) * 0.5f;

		// half-sparkle
		float OuterGlowSize = SPARKLE_SIZE * 2.0f;
		float OuterGlowAlpha = SparkleAlpha * 0.15f;
		Graphics()->SetColor(R, G, B, Alpha * OuterGlowAlpha);
		IGraphics::CQuadItem OuterGlowQuad(SparklePos.x - OuterGlowSize / 2, SparklePos.y - OuterGlowSize / 2, OuterGlowSize, OuterGlowSize);
		Graphics()->QuadsSetRotation(SparkleRotation * 0.5f);
		Graphics()->QuadsDrawTL(&OuterGlowQuad, 1);

		// sparkle
		Graphics()->SetColor(R, G, B, Alpha * SparkleAlpha);
		IGraphics::CQuadItem Quad(SparklePos.x - SPARKLE_SIZE / 2, SparklePos.y - SPARKLE_SIZE / 2, SPARKLE_SIZE, SPARKLE_SIZE);
		Graphics()->QuadsSetRotation(SparkleRotation);
		Graphics()->QuadsDrawTL(&Quad, 1);

		float GlowSizes[] = {0.8f, 0.6f, 0.4f};
		float GlowAlphas[] = {0.5f, 0.4f, 0.3f};
		float GlowRotations[] = {1.5f, 2.0f, 2.5f};

		for(int j = 0; j < 3; j++)
		{
			float GlowSize = SPARKLE_SIZE * GlowSizes[j];
			float GlowAlpha = SparkleAlpha * GlowAlphas[j];
			Graphics()->SetColor(R, G, B, Alpha * GlowAlpha);
			IGraphics::CQuadItem GlowQuad(SparklePos.x - GlowSize / 2, SparklePos.y - GlowSize / 2, GlowSize, GlowSize);
			Graphics()->QuadsSetRotation(SparkleRotation * GlowRotations[j]);
			Graphics()->QuadsDrawTL(&GlowQuad, 1);
		}
	}

	Graphics()->QuadsEnd();
}
