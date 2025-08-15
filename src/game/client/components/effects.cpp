/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

#include <engine/demo.h>

#include <engine/shared/config.h>

#include <game/generated/client_data.h>

#include <game/client/components/damageind.h>
#include <game/client/components/flow.h>
#include <game/client/components/particles.h>
#include <game/client/components/sounds.h>
#include <game/client/gameclient.h>

#include "effects.h"

CEffects::CEffects()
{
	m_Add5hz = false;
	m_Add50hz = false;
	m_Add100hz = false;
}

void CEffects::AirJump(vec2 Pos, float Alpha)
{
	if(!g_Config.m_ClJumpEffect)
	{
		if(g_Config.m_SndGame)
			GameClient()->m_Sounds.PlayAt(CSounds::CHN_WORLD, SOUND_PLAYER_AIRJUMP, 1.0f, Pos);
		return;
	}

	// Check focus mode settings
	if(g_Config.m_ClFocusMode && g_Config.m_ClFocusModeHideEffects)
	{
		if(g_Config.m_SndGame)
			GameClient()->m_Sounds.PlayAt(CSounds::CHN_WORLD, SOUND_PLAYER_AIRJUMP, 1.0f, Pos);
		return;
	}

	CParticle p;
	p.SetDefault();
	p.m_Spr = SPRITE_PART_AIRJUMP;
	p.m_Pos = Pos + vec2(-6.0f, 16.0f);
	p.m_Vel = vec2(0.0f, -200.0f);
	p.m_LifeSpan = 0.5f;
	p.m_StartSize = 48.0f;
	p.m_EndSize = 0.0f;
	p.m_Rot = random_angle();
	p.m_Rotspeed = pi * 2.0f;
	p.m_Gravity = 500.0f;
	p.m_Friction = 0.7f;
	p.m_FlowAffected = 0.0f;
	p.m_Color.a = Alpha;
	p.m_StartAlpha = Alpha;
	GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);

	p.m_Pos = Pos + vec2(6.0f, 16.0f);
	GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);

	if(g_Config.m_SndGame)
		GameClient()->m_Sounds.PlayAt(CSounds::CHN_WORLD, SOUND_PLAYER_AIRJUMP, 1.0f, Pos);
}

void CEffects::DamageIndicator(vec2 Pos, vec2 Dir, float Alpha) { GameClient()->m_DamageInd.Create(Pos, Dir, Alpha); }

void CEffects::PowerupShine(vec2 Pos, vec2 Size, float Alpha)
{
	if(!m_Add50hz)
		return;

	CParticle p;
	p.SetDefault();
	p.m_Spr = SPRITE_PART_SLICE;
	p.m_Pos = Pos + vec2(random_float(-0.5f, 0.5f), random_float(-0.5f, 0.5f)) * Size;
	p.m_Vel = vec2(0.0f, 0.0f);
	p.m_LifeSpan = 0.5f;
	p.m_StartSize = 16.0f;
	p.m_EndSize = 0.0f;
	p.m_Rot = random_angle();
	p.m_Rotspeed = pi * 2.0f;
	p.m_Gravity = 500.0f;
	p.m_Friction = 0.9f;
	p.m_FlowAffected = 0.0f;
	p.m_Color.a = Alpha;
	p.m_StartAlpha = Alpha;
	GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
}

void CEffects::FreezingFlakes(vec2 Pos, vec2 Size, float Alpha)
{
	if(g_Config.m_ClFreezeSnowFlakes == 0)
		return;

	// Check focus mode settings
	if(g_Config.m_ClFocusMode && g_Config.m_ClFocusModeHideEffects)
		return;

	if(!m_Add5hz)
		return;

	CParticle p;
	p.SetDefault();
	p.m_Spr = SPRITE_PART_SNOWFLAKE;
	p.m_Pos = Pos + vec2(random_float(-0.5f, 0.5f), random_float(-0.5f, 0.5f)) * Size;
	p.m_Vel = vec2(0.0f, 0.0f);
	p.m_LifeSpan = 1.5f;
	p.m_StartSize = random_float(0.5f, 1.5f) * 16.0f;
	p.m_EndSize = p.m_StartSize * 0.5f;
	p.m_UseAlphaFading = true;
	p.m_StartAlpha = 1.0f;
	p.m_EndAlpha = 0.0f;
	p.m_Rot = random_angle();
	p.m_Rotspeed = pi;
	p.m_Gravity = random_float(250.0f);
	p.m_Friction = 0.9f;
	p.m_FlowAffected = 0.0f;
	p.m_Collides = false;
	p.m_Color.a = Alpha;
	p.m_StartAlpha = Alpha;
	GameClient()->m_Particles.Add(CParticles::GROUP_EXTRA, &p);
}

void CEffects::SparkleTrail(vec2 Pos, float Alpha)
{
	// Add sparkle trail
	if(!m_Add50hz)
		return;

	CParticle p;
	p.SetDefault();
	p.m_Spr = SPRITE_PART_SPARKLE;
	p.m_Pos = Pos + random_direction() * random_float(40.0f);
	p.m_Vel = vec2(0.0f, 0.0f);
	p.m_LifeSpan = 0.5f;
	p.m_StartSize = 0.0f;
	p.m_EndSize = random_float(20.0f, 30.0f);
	p.m_UseAlphaFading = true;
	p.m_StartAlpha = Alpha;
	p.m_EndAlpha = std::min(0.2f, Alpha);
	p.m_Collides = false;
	GameClient()->m_Particles.Add(CParticles::GROUP_TRAIL_EXTRA, &p);
}

void CEffects::SmokeTrail(vec2 Pos, vec2 Vel, float Alpha, float TimePassed)
{
	if(!m_Add50hz && TimePassed < 0.001f)
		return;

	CParticle p;
	p.SetDefault();
	p.m_Spr = SPRITE_PART_SMOKE;
	p.m_Pos = Pos;
	p.m_Vel = Vel + random_direction() * 50.0f;
	p.m_LifeSpan = random_float(0.5f, 1.0f);
	p.m_StartSize = random_float(12.0f, 20.0f);
	p.m_EndSize = 0.0f;
	p.m_Friction = 0.7f;
	p.m_Gravity = random_float(-500.0f);
	p.m_Color.a = Alpha;
	p.m_StartAlpha = Alpha;
	GameClient()->m_Particles.Add(CParticles::GROUP_PROJECTILE_TRAIL, &p, TimePassed);
}

void CEffects::SkidTrail(vec2 Pos, vec2 Vel, int Direction, float Alpha)
{
	if(m_Add100hz)
	{
		CParticle p;
		p.SetDefault();
		p.m_Spr = SPRITE_PART_SMOKE;
		p.m_Pos = Pos + vec2(-Direction * 6.0f, 12.0f);
		p.m_Vel = vec2(-Direction * 100.0f * length(Vel), -50.0f) + random_direction() * 50.0f;
		p.m_LifeSpan = random_float(0.5f, 1.0f);
		p.m_StartSize = random_float(24.0f, 36.0f);
		p.m_EndSize = 0.0f;
		p.m_Friction = 0.7f;
		p.m_Gravity = random_float(-500.0f);
		p.m_Color = ColorRGBA(0.75f, 0.75f, 0.75f, Alpha);
		p.m_StartAlpha = Alpha;
		GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
	}
	if(g_Config.m_SndGame)
	{
		int64_t Now = time();
		if(Now - m_SkidSoundTimer > time_freq() / 10)
		{
			m_SkidSoundTimer = Now;
			GameClient()->m_Sounds.PlayAt(CSounds::CHN_WORLD, SOUND_PLAYER_SKID, 1.0f, Pos);
		}
	}
}

void CEffects::BulletTrail(vec2 Pos, float Alpha, float TimePassed)
{
	if(!m_Add100hz && TimePassed < 0.001f)
		return;

	CParticle p;
	p.SetDefault();
	p.m_Spr = SPRITE_PART_BALL;
	p.m_Pos = Pos;
	p.m_LifeSpan = random_float(0.25f, 0.5f);
	p.m_StartSize = 8.0f;
	p.m_EndSize = 0.0f;
	p.m_Friction = 0.7f;
	p.m_Color.a *= Alpha;
	p.m_StartAlpha = Alpha;
	GameClient()->m_Particles.Add(CParticles::GROUP_PROJECTILE_TRAIL, &p, TimePassed);
}

void CEffects::PlayerSpawn(vec2 Pos, float Alpha)
{
	for(int i = 0; i < 32; i++)
	{
		CParticle p;
		p.SetDefault();
		p.m_Spr = SPRITE_PART_SHELL;
		p.m_Pos = Pos;
		p.m_Vel = random_direction() * (std::pow(random_float(), 3) * 600.0f);
		p.m_LifeSpan = random_float(0.3f, 0.6f);
		p.m_StartSize = random_float(64.0f, 96.0f);
		p.m_EndSize = 0.0f;
		p.m_Rot = random_angle();
		p.m_Rotspeed = random_float();
		p.m_Gravity = random_float(-400.0f);
		p.m_Friction = 0.7f;
		p.m_Color = ColorRGBA(0xb5 / 255.0f, 0x50 / 255.0f, 0xcb / 255.0f, Alpha);
		p.m_StartAlpha = Alpha;
		GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
	}
	if(g_Config.m_SndGame)
		GameClient()->m_Sounds.PlayAt(CSounds::CHN_WORLD, SOUND_PLAYER_SPAWN, 1.0f, Pos);
}

void CEffects::PlayerDeath(vec2 Pos, int ClientId, float Alpha)
{
	ColorRGBA BloodColor(1.0f, 1.0f, 1.0f);

	if(ClientId >= 0)
	{
		// Use m_RenderInfo.m_CustomColoredSkin instead of m_UseCustomColor
		// m_UseCustomColor says if the player's skin has a custom color (value sent from the client side)

		// m_RenderInfo.m_CustomColoredSkin Defines if in the context of the game the color is being customized,
		// Using this value if the game is teams (red and blue), this value will be true even if the skin is with the normal color.
		// And will use the team body color to create player death effect instead of tee color
		if(GameClient()->Client()->IsSixup())
		{
			if(GameClient()->m_aClients[ClientId].m_RenderInfo.m_aSixup[g_Config.m_ClDummy].m_aUseCustomColors[protocol7::SKINPART_BODY]) { BloodColor = GameClient()->m_aClients[ClientId].m_RenderInfo.m_aSixup[g_Config.m_ClDummy].m_aColors[protocol7::SKINPART_BODY]; }
			else { BloodColor = GameClient()->m_aClients[ClientId].m_RenderInfo.m_aSixup[g_Config.m_ClDummy].m_BloodColor; }
		}
		else
		{
			if(GameClient()->m_aClients[ClientId].m_RenderInfo.m_CustomColoredSkin) { BloodColor = GameClient()->m_aClients[ClientId].m_RenderInfo.m_ColorBody; }
			else { BloodColor = GameClient()->m_aClients[ClientId].m_RenderInfo.m_BloodColor; }
		}
	}

	for(int i = 0; i < 64; i++)
	{
		CParticle p;
		p.SetDefault();
		p.m_Spr = SPRITE_PART_SPLAT01 + (rand() % 3);
		p.m_Pos = Pos;
		p.m_Vel = random_direction() * (random_float(0.1f, 1.1f) * 900.0f);
		p.m_LifeSpan = random_float(0.3f, 0.6f);
		p.m_StartSize = random_float(24.0f, 40.0f);
		p.m_EndSize = 0.0f;
		p.m_Rot = random_angle();
		p.m_Rotspeed = random_float(-0.5f, 0.5f) * pi;
		p.m_Gravity = 800.0f;
		p.m_Friction = 0.8f;
		ColorRGBA c = BloodColor.v4() * random_float(0.75f, 1.0f);
		p.m_Color = ColorRGBA(c.r, c.g, c.b, 0.75f * Alpha);
		p.m_StartAlpha = Alpha;
		GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
	}
}

void CEffects::Confetti(vec2 Pos, float Alpha)
{
	ColorRGBA Red(1.0f, 0.4f, 0.4f);
	ColorRGBA Green(0.4f, 1.0f, 0.4f);
	ColorRGBA Blue(0.4f, 0.4f, 1.0f);
	ColorRGBA Yellow(1.0f, 1.0f, 0.4f);
	ColorRGBA Cyan(0.4f, 1.0f, 1.0f);
	ColorRGBA Magenta(1.0f, 0.4f, 1.0f);

	ColorRGBA aConfettiColors[] = {Red, Green, Blue, Yellow, Cyan, Magenta};

	// powerful confettis
	for(int i = 0; i < 32; i++)
	{
		CParticle p;
		p.SetDefault();
		p.m_Spr = SPRITE_PART_SPLAT01 + (rand() % 3);
		p.m_Pos = Pos;
		p.m_Vel = direction(-0.5f * pi + random_float(-0.2f, 0.2f)) * random_float(0.01f, 1.0f) * 2000.0f;
		p.m_LifeSpan = random_float(1.0f, 1.2f);
		p.m_StartSize = random_float(12.0f, 24.0f);
		p.m_EndSize = 0.0f;
		p.m_Rot = random_angle();
		p.m_Rotspeed = random_float(-0.5f, 0.5f) * pi;
		p.m_Gravity = -700.0f;
		p.m_Friction = 0.6f;
		ColorRGBA c = aConfettiColors[(rand() % std::size(aConfettiColors))];
		p.m_Color = c.WithMultipliedAlpha(0.75f * Alpha);
		p.m_StartAlpha = Alpha;
		GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
	}

	// broader confettis
	for(int i = 0; i < 32; i++)
	{
		CParticle p;
		p.SetDefault();
		p.m_Spr = SPRITE_PART_SPLAT01 + (rand() % 3);
		p.m_Pos = Pos;
		p.m_Vel = direction(-0.5f * pi + random_float(-0.8f, 0.8f)) * random_float(0.01f, 1.0f) * 1500.0f;
		p.m_LifeSpan = random_float(0.8f, 1.0f);
		p.m_StartSize = random_float(12.0f, 24.0f);
		p.m_EndSize = 0.0f;
		p.m_Rot = random_angle();
		p.m_Rotspeed = random_float(-0.5f, 0.5f) * pi;
		p.m_Gravity = -700.0f;
		p.m_Friction = 0.6f;
		ColorRGBA c = aConfettiColors[(rand() % std::size(aConfettiColors))];
		p.m_Color = c.WithMultipliedAlpha(0.75f * Alpha);
		p.m_StartAlpha = Alpha;
		GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
	}
}

//Pulse
void CEffects::PlayerTrail(vec2 Pos, float Alpha)
{
	if(!m_Add100hz)
		return;

	// Check focus mode settings
	if(g_Config.m_ClFocusMode && g_Config.m_ClFocusModeHideEffects)
		return;

	int Style = g_Config.m_ClTrailStyle;

	if(Style == 1)
	{
		CParticle p;
		p.SetDefault();
		p.m_Spr = SPRITE_PART_SPARKLE;
		p.m_Pos = Pos + random_direction() * random_float(0.0f, 20.0f);
		p.m_Vel = vec2(0, 0);
		p.m_LifeSpan = 1.5f;
		p.m_StartSize = random_float(42.0f, 35.0f);
		p.m_EndSize = 0;
		p.m_UseAlphaFading = true;
		p.m_StartAlpha = Alpha;
		p.m_EndAlpha = 0.0f;
		p.m_Color = ColorRGBA(0.0f, 0.8f, 1.0f, Alpha);
		GameClient()->m_Particles.Add(CParticles::GROUP_TRAIL_EXTRA, &p);
	}
	else if(Style == 2)
	{
		for(int i = 0; i < 12; i++)
		{
			CParticle p;
			p.SetDefault();
			p.m_Spr = SPRITE_PART_SPARKLE;
			float Angle = (i / 12.0f) * 2 * pi;
			vec2 Offset = vec2(std::cos(Angle), std::sin(Angle)) * 32.0f;
			p.m_Pos = Pos + Offset;
			p.m_Vel = vec2(0, 0);
			p.m_LifeSpan = 0.1f;
			p.m_StartSize = random_float(8.0f, 12.0f);
			p.m_EndSize = p.m_StartSize;
			p.m_UseAlphaFading = false;
			p.m_StartAlpha = Alpha;
			p.m_EndAlpha = Alpha;
			p.m_Color = ColorRGBA(1.0f, 0.8f, 0.0f, Alpha);
			GameClient()->m_Particles.Add(CParticles::GROUP_TRAIL_EXTRA, &p);
		}
	}
	else if(Style == 3)
	{
		// Rainbow pulsing trail
		static float s_Time = 0.0f;
		s_Time += 0.01f; // Adjust speed of color change

		// Create a ring of particles
		for(int i = 0; i < 16; i++)
		{
			CParticle p;
			p.SetDefault();
			p.m_Spr = SPRITE_PART_SPARKLE;

			// Calculate position in a circle
			float Angle = (i / 16.0f) * 2 * pi;
			vec2 Offset = vec2(std::cos(Angle), std::sin(Angle)) * 24.0f;
			p.m_Pos = Pos + Offset;
			p.m_Vel = vec2(0, 0);

			// Calculate rainbow color using HSVA
			float Hue = (s_Time + i / 16.0f) - floor(s_Time + i / 16.0f); // Keep hue in [0,1]
			float Pulse = (std::sin(s_Time * 2.0f) + 1.0f) * 0.5f; // Pulse between 0 and 1
			ColorHSVA HSVA(Hue, 1.0f, 1.0f, Alpha * (0.5f + Pulse * 0.5f));
			p.m_Color = color_cast<ColorRGBA>(HSVA);

			p.m_LifeSpan = 0.2f;
			p.m_StartSize = random_float(12.0f, 16.0f);
			p.m_EndSize = p.m_StartSize * 0.5f;
			p.m_UseAlphaFading = true;
			p.m_StartAlpha = Alpha;
			p.m_EndAlpha = 0.0f;
			GameClient()->m_Particles.Add(CParticles::GROUP_TRAIL_EXTRA, &p);
		}
	}
}

void CEffects::AfkAura(vec2 Position, float Alpha, vec4 BodyColor)
{
	const int NUM_SPARKLES = g_Config.m_ClToggleAuraParticlesNum;
	const float SPARKLE_SIZE = 6.0f;
	const float AURA_RADIUS = g_Config.m_ClToggleAuraRadius;
	const float VERTICAL_OFFSET = -17.0f;

	float Time = Client()->LocalTime();
	vec2 BasePosition = Position + vec2(0, VERTICAL_OFFSET);

	if(g_Config.m_ClIdleAuraType == 1)
	{
		for(int i = 0; i < NUM_SPARKLES; i++)
		{
			float BaseAngle = (float)i / NUM_SPARKLES * 2.0f * pi;

			float RotationSpeed = 0.2f + 0.05f * i;
			float Angle = BaseAngle + Time * RotationSpeed;

			float RadiusPulse = AURA_RADIUS * (0.9f + 0.1f * std::sin(Time * 2.0f + i));

			vec2 SparklePos = BasePosition + vec2(
				                  std::cos(Angle) * RadiusPulse,
				                  std::sin(Angle) * RadiusPulse
				                  );

			float SparkleAlpha = 0.4f + 0.3f * std::sin(Time * 3.0f + i);

			float ColorPhase = Time * 0.5f + i * 0.3f;
			float R = BodyColor.r * (0.8f + 0.2f * std::sin(ColorPhase));
			float G = BodyColor.g * (0.8f + 0.2f * std::sin(ColorPhase + 1.0f));
			float B = BodyColor.b * (0.8f + 0.2f * std::sin(ColorPhase + 2.0f));

			CParticle p;
			p.SetDefault();
			p.m_Spr = SPRITE_PART_SPARKLE;
			p.m_Pos = SparklePos;
			p.m_Vel = vec2(0.0f, 0.0f);
			p.m_LifeSpan = 0.05f;
			p.m_StartSize = SPARKLE_SIZE * 1.2f;
			p.m_EndSize = SPARKLE_SIZE * 0.8f;
			p.m_UseAlphaFading = true;
			p.m_StartAlpha = SparkleAlpha * Alpha;
			p.m_EndAlpha = 0.0f;
			p.m_Rot = Angle;
			p.m_Rotspeed = 0.0f;
			p.m_Gravity = 0.0f;
			p.m_Friction = 1.0f;
			p.m_FlowAffected = 0.0f;
			p.m_Collides = false;
			p.m_Color = ColorRGBA(
				std::clamp(R, 0.0f, 1.0f),
				std::clamp(G, 0.0f, 1.0f),
				std::clamp(B, 0.0f, 1.0f),
				1.0f
				);

			GameClient()->m_Particles.Add(CParticles::GROUP_EXTRA, &p);
		}
	}
	if(g_Config.m_ClIdleAuraType == 2)
	{
		for(int i = 0; i < NUM_SPARKLES; i++)
		{
			float BaseAngle = (float)i / NUM_SPARKLES * 2.0f * pi;

			float RotationSpeed = 0.05f + 0.02f * (float)(i % 3);
			float Angle = BaseAngle + Time * RotationSpeed;

			float RadiusPulse = AURA_RADIUS * (0.85f + 0.15f * std::sin(Time * 1.0f + i));

			float OffsetX = std::sin(Time * 0.5f + i) * random_float(1.0f, 3.0f);
			float OffsetY = std::cos(Time * 0.5f + i) * random_float(1.0f, 3.0f);

			vec2 SparklePos = BasePosition + vec2(
				                  std::cos(Angle) * RadiusPulse + OffsetX,
				                  std::sin(Angle) * RadiusPulse + OffsetY
				                  );

			float SparkleAlpha = 0.3f + 0.3f * std::sin(Time * 0.8f + i);

			float ColorPhase = Time * 0.3f + i * 0.5f;
			float R = BodyColor.r * (0.85f + 0.15f * std::sin(ColorPhase));
			float G = BodyColor.g * (0.85f + 0.15f * std::sin(ColorPhase + 1.0f));
			float B = BodyColor.b * (0.85f + 0.15f * std::sin(ColorPhase + 2.0f));

			CParticle p;
			p.SetDefault();
			p.m_Spr = SPRITE_PART_SPARKLE;
			p.m_Pos = SparklePos;
			p.m_Vel = vec2(0.0f, 0.0f);
			p.m_LifeSpan = 0.05f;
			p.m_StartSize = SPARKLE_SIZE * random_float(1.0f, 1.2f);
			p.m_EndSize = SPARKLE_SIZE * 0.8f;
			p.m_UseAlphaFading = true;
			p.m_StartAlpha = SparkleAlpha * Alpha;
			p.m_EndAlpha = 0.0f;
			p.m_Rot = Angle;
			p.m_Rotspeed = random_float(-0.2f, 0.2f);
			p.m_Gravity = 0.0f;
			p.m_Friction = 1.0f;
			p.m_FlowAffected = 0.0f;
			p.m_Collides = false;
			p.m_Color = ColorRGBA(
				std::clamp(R, 0.0f, 1.0f),
				std::clamp(G, 0.0f, 1.0f),
				std::clamp(B, 0.0f, 1.0f),
				1.0f
				);

			GameClient()->m_Particles.Add(CParticles::GROUP_EXTRA, &p);
		}
	}
	if(g_Config.m_ClIdleAuraType == 3)
	{
		for(int i = 0; i < NUM_SPARKLES; i++)
		{
			float BaseAngle = (float)i / NUM_SPARKLES * 2.0f * pi;
			float RotationSpeed = 0.1f + (float)(i % 4) * 0.15f;

			float AngleNoise = std::sin(Time * 0.3f + i * 0.7f) * 0.5f +
			                   std::cos(Time * 0.9f + i * 1.1f) * 0.2f;
			float Angle = BaseAngle + Time * RotationSpeed + AngleNoise;

			float VerticalWave = std::sin(Time * 0.8f + i * 0.5f) * 8.0f +
			                     std::cos(Time * 0.4f + i * 0.3f) * 4.0f +
			                     std::sin(Time * 1.2f + i * 0.2f) * 2.0f +
			                     random_float(-2.0f, 2.0f);

			float RadiusVariation = 1.0f +
			                        std::sin(Time * 0.6f + i * 0.4f) * 0.2f +
			                        std::cos(Time * 0.3f + i * 0.6f) * 0.1f +
			                        random_float(-0.1f, 0.1f);
			float CurrentRadius = AURA_RADIUS * RadiusVariation;

			vec2 SparklePos = BasePosition + vec2(
				                  std::cos(Angle) * CurrentRadius + random_float(-1.5f, 1.5f),
				                  std::sin(Angle) * CurrentRadius + VerticalWave + random_float(-1.5f, 1.5f)
				                  );

			float SparkleAlpha = 0.4f + 0.3f * std::sin(Time * 1.2f + i * 0.4f) +
			                     0.2f * std::cos(Time * 0.8f + i * 0.6f);

			float ColorPhase = Time * 0.3f + i * 0.2f;
			float ColorVariation = 0.15f * std::sin(ColorPhase);
			float ColorVariation2 = 0.1f * std::cos(ColorPhase * 1.5f);

			float R = BodyColor.r + ColorVariation;
			float G = BodyColor.g + ColorVariation2;
			float B = BodyColor.b + ColorVariation * 0.5f;

			float ColorPulse = 0.1f * std::sin(Time * 0.5f + i * 0.3f);
			R += ColorPulse;
			G += ColorPulse * 0.5f;
			B += ColorPulse * 0.3f;

			CParticle p;
			p.SetDefault();
			p.m_Spr = SPRITE_PART_SPARKLE;
			p.m_Pos = SparklePos;
			p.m_Vel = vec2(0.0f, 0.0f);
			p.m_LifeSpan = 0.05f;

			float SizeNoise = SPARKLE_SIZE * (0.8f + 0.4f * std::sin(Time + i));
			p.m_StartSize = SizeNoise;
			p.m_EndSize = SizeNoise * 0.5f;

			p.m_UseAlphaFading = true;

			float RandomAlphaMul = random_float(0.5f, 1.0f);
			p.m_StartAlpha = SparkleAlpha * Alpha * RandomAlphaMul;
			p.m_EndAlpha = 0.0f;

			p.m_Rot = Time * 2.0f + i * 0.5f +
			          std::sin(Time * 0.7f + i * 0.4f) * 0.5f;
			p.m_Rotspeed = random_float(-1.0f, 1.0f);
			p.m_Gravity = 0.0f;
			p.m_Friction = 1.0f;
			p.m_FlowAffected = 0.0f;
			p.m_Collides = false;
			p.m_Color = ColorRGBA(
				std::clamp(R, 0.0f, 1.0f),
				std::clamp(G, 0.0f, 1.0f),
				std::clamp(B, 0.0f, 1.0f),
				1.0f
				);

			GameClient()->m_Particles.Add(CParticles::GROUP_EXTRA, &p);
		}
		if(g_Config.m_ClIdleAuraType == 5)
		{
			for(int i = 0; i < NUM_SPARKLES; i++)
			{
				float BaseAngle = (float)i / NUM_SPARKLES * 2.0f * pi;
				float SpiralSpeed = 0.05f + 0.01f * (i % 5);
				float Angle = BaseAngle + Time * SpiralSpeed;

				float RadiusPulse = AURA_RADIUS * (0.5f + 0.5f * std::sin(Time * 0.8f + i * 0.5f));

				float VerticalWave = std::sin(Time * 0.5f + i * 0.7f) * 5.0f;

				float OffsetX = std::cos(Time * 0.3f + i) * random_float(0.5f, 1.5f);
				float OffsetY = std::sin(Time * 0.3f + i) * random_float(0.5f, 1.5f);

				vec2 SparklePos = BasePosition + vec2(
					                  std::cos(Angle) * RadiusPulse + OffsetX,
					                  std::sin(Angle) * RadiusPulse + VerticalWave + OffsetY
					                  );

				float SparkleAlpha = 0.3f + 0.4f * std::sin(Time * 0.7f + i);

				float ColorPhase = Time * 0.4f + i * 0.6f;
				float R = BodyColor.r * (0.7f + 0.3f * std::sin(ColorPhase));
				float G = BodyColor.g * (0.7f + 0.3f * std::sin(ColorPhase + 1.0f));
				float B = BodyColor.b * (0.7f + 0.3f * std::sin(ColorPhase + 2.0f));

				CParticle p;
				p.SetDefault();
				p.m_Spr = SPRITE_PART_SPARKLE;
				p.m_Pos = SparklePos;
				p.m_Vel = vec2(0.0f, 0.0f);
				p.m_LifeSpan = 0.05f;
				p.m_StartSize = SPARKLE_SIZE * random_float(1.0f, 1.3f);
				p.m_EndSize = SPARKLE_SIZE * 0.8f;
				p.m_UseAlphaFading = true;
				p.m_StartAlpha = SparkleAlpha * Alpha;
				p.m_EndAlpha = 0.0f;
				p.m_Rot = Angle;
				p.m_Rotspeed = random_float(-0.1f, 0.1f);
				p.m_Gravity = 0.0f;
				p.m_Friction = 1.0f;
				p.m_FlowAffected = 0.0f;
				p.m_Collides = false;
				p.m_Color = ColorRGBA(
					std::clamp(R, 0.0f, 1.0f),
					std::clamp(G, 0.0f, 1.0f),
					std::clamp(B, 0.0f, 1.0f),
					1.0f
					);

				GameClient()->m_Particles.Add(CParticles::GROUP_EXTRA, &p);
			}
		}
	}
	if(g_Config.m_ClIdleAuraType == 4)
	{
		for(int i = 0; i < NUM_SPARKLES; i++)
		{
			float BaseAngle = (float)i / NUM_SPARKLES * 2.0f * pi;
			float SpiralSpeed = 0.03f + 0.01f * (i % 5);
			float Angle = BaseAngle + Time * SpiralSpeed;

			float RadiusPulse = AURA_RADIUS * (0.5f + 0.5f * std::sin(Time * 0.5f + i * 0.3f));
			float VerticalWave = std::sin(Time * 0.4f + i * 0.7f) * 5.0f;

			float OffsetX = std::cos(Time * 0.2f + i) * random_float(0.2f, 0.8f);
			float OffsetY = std::sin(Time * 0.2f + i) * random_float(0.2f, 0.8f);

			vec2 SparklePos = BasePosition + vec2(
				                  std::cos(Angle) * RadiusPulse + OffsetX,
				                  std::sin(Angle) * RadiusPulse + VerticalWave + OffsetY
				                  );

			float MinRadius = 5.0f;
			float DistFactor = std::max(RadiusPulse, MinRadius) / AURA_RADIUS;
			float SparkleAlpha = 0.3f + 0.4f * std::sin(Time * 0.7f + i);
			SparkleAlpha *= DistFactor * (0.8f + 0.2f * std::sin(Time * 1.5f + i));

			// цвет игрока с мягкой пульсацией
			float ColorPhase = Time * 0.4f + i * 0.6f;
			float R = BodyColor.r * (0.7f + 0.3f * std::sin(ColorPhase));
			float G = BodyColor.g * (0.7f + 0.3f * std::sin(ColorPhase + 1.0f));
			float B = BodyColor.b * (0.7f + 0.3f * std::sin(ColorPhase + 2.0f));

			CParticle p;
			p.SetDefault();
			p.m_Spr = SPRITE_PART_SPARKLE;
			p.m_Pos = SparklePos;
			p.m_Vel = vec2(0.0f, 0.0f);
			p.m_LifeSpan = 0.05f;
			p.m_StartSize = SPARKLE_SIZE * random_float(1.0f, 1.3f);
			p.m_EndSize = SPARKLE_SIZE * 0.8f;
			p.m_UseAlphaFading = true;
			p.m_StartAlpha = SparkleAlpha * Alpha;
			p.m_EndAlpha = 0.0f;
			p.m_Rot = Angle;
			p.m_Rotspeed = random_float(-0.05f, 0.05f);
			p.m_Gravity = 0.0f;
			p.m_Friction = 1.0f;
			p.m_FlowAffected = 0.0f;
			p.m_Collides = false;
			p.m_Color = ColorRGBA(
				std::clamp(R, 0.0f, 1.0f),
				std::clamp(G, 0.0f, 1.0f),
				std::clamp(B, 0.0f, 1.0f),
				1.0f
				);

			GameClient()->m_Particles.Add(CParticles::GROUP_EXTRA, &p);
		}
	}
	if(g_Config.m_ClIdleAuraType == 5)
	{
		const float SPIRAL_RADIUS = AURA_RADIUS;
		const float SPIRAL_TURNS = 3.0f; // количество витков
		const float SPIRAL_HEIGHT = 40.0f; // высота спирали
		const float SPEED = 0.5f; // скорость вращения

		for(int i = 0; i < NUM_SPARKLES; i++)
		{
			float t = (float)i / NUM_SPARKLES; // параметр вдоль спирали

			float angle = t * SPIRAL_TURNS * 2.0f * pi + Time * SPEED;
			float y = (t - 0.5f) * SPIRAL_HEIGHT;

			float angle2 = angle + pi;

			bool secondStrand = (i % 2 != 0);
			float finalAngle = secondStrand ? angle2 : angle;

			float radius = SPIRAL_RADIUS * (0.8f + 0.2f * std::sin(Time + t * 5.0f));

			vec2 SparklePos = BasePosition + vec2(
				                  std::cos(finalAngle) * radius,
				                  std::sin(finalAngle) * radius + y
				                  );

			float SparkleAlpha = (0.3f + 0.3f * std::sin(Time * 2.0f + i)) * (radius / SPIRAL_RADIUS);

			float ColorPhase = Time * 0.4f + i * 0.3f;
			float R = BodyColor.r * (0.7f + 0.3f * std::sin(ColorPhase));
			float G = BodyColor.g * (0.7f + 0.3f * std::sin(ColorPhase + 0.5f));
			float B = BodyColor.b * (0.7f + 0.3f * std::sin(ColorPhase + 1.0f));

			CParticle p;
			p.SetDefault();
			p.m_Spr = SPRITE_PART_SPARKLE;
			p.m_Pos = SparklePos;
			p.m_Vel = vec2(0.0f, 0.0f);
			p.m_LifeSpan = 0.05f;
			p.m_StartSize = SPARKLE_SIZE * random_float(1.0f, 1.2f);
			p.m_EndSize = SPARKLE_SIZE * 0.7f;
			p.m_UseAlphaFading = true;
			p.m_StartAlpha = SparkleAlpha * Alpha;
			p.m_EndAlpha = 0.0f;
			p.m_Rot = finalAngle;
			p.m_Rotspeed = random_float(-0.02f, 0.02f);
			p.m_Gravity = 0.0f;
			p.m_Friction = 1.0f;
			p.m_FlowAffected = 0.0f;
			p.m_Collides = false;
			p.m_Color = ColorRGBA(
				std::clamp(R, 0.0f, 1.0f),
				std::clamp(G, 0.0f, 1.0f),
				std::clamp(B, 0.0f, 1.0f),
				1.0f
				);

			GameClient()->m_Particles.Add(CParticles::GROUP_EXTRA, &p);
		}
	}
}

void CEffects::Explosion(vec2 Pos, float Alpha)
{
	// add to flow
	for(int y = -8; y <= 8; y++)
		for(int x = -8; x <= 8; x++)
		{
			if(x == 0 && y == 0)
				continue;

			float a = 1 - (length(vec2(x, y)) / length(vec2(8.0f, 8.0f)));
			GameClient()->m_Flow.Add(Pos + vec2(x, y) * 16.0f, normalize(vec2(x, y)) * 5000.0f * a, 10.0f);
		}

	// add the explosion
	CParticle p;
	p.SetDefault();
	p.m_Spr = SPRITE_PART_EXPL01;
	p.m_Pos = Pos;
	p.m_LifeSpan = 0.4f;
	p.m_StartSize = 150.0f;
	p.m_EndSize = 0.0f;
	p.m_Rot = random_angle();
	p.m_Color.a = Alpha;
	p.m_StartAlpha = Alpha;
	GameClient()->m_Particles.Add(CParticles::GROUP_EXPLOSIONS, &p);

	// Nudge position slightly to edge of closest tile so the
	// smoke doesn't get stuck inside the tile.
	if(Collision()->CheckPoint(Pos))
	{
		const vec2 DistanceToTopLeft = Pos - vec2(round_truncate(Pos.x / 32), round_truncate(Pos.y / 32)) * 32;

		vec2 CheckOffset;
		CheckOffset.x = (DistanceToTopLeft.x > 16.0f ? 32.0f : -1.0f);
		CheckOffset.y = (DistanceToTopLeft.y > 16.0f ? 32.0f : -1.0f);
		CheckOffset -= DistanceToTopLeft;

		for(vec2 Mask : {vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f)})
		{
			const vec2 NewPos = Pos + CheckOffset * Mask;
			if(!Collision()->CheckPoint(NewPos))
			{
				Pos = NewPos;
				break;
			}
		}
	}

	// add the smoke
	for(int i = 0; i < 24; i++)
	{
		p.SetDefault();
		p.m_Spr = SPRITE_PART_SMOKE;
		p.m_Pos = Pos;
		p.m_Vel = random_direction() * (random_float(1.0f, 1.2f) * 1000.0f);
		p.m_LifeSpan = random_float(0.5f, 0.9f);
		p.m_StartSize = random_float(32.0f, 40.0f);
		p.m_EndSize = 0.0f;
		p.m_Gravity = random_float(-800.0f);
		p.m_Friction = 0.4f;
		p.m_Color = mix(vec4(0.75f, 0.75f, 0.75f, 1.0f), vec4(0.5f, 0.5f, 0.5f, 1.0f), random_float());
		p.m_Color.a *= Alpha;
		p.m_StartAlpha = p.m_Color.a;
		GameClient()->m_Particles.Add(CParticles::GROUP_GENERAL, &p);
	}
}

void CEffects::HammerHit(vec2 Pos, float Alpha)
{
	if(g_Config.m_ClHammerHitEffect == 0)
	{
		if(g_Config.m_SndGame && g_Config.m_ClHammerHitEffectSound)
			GameClient()->m_Sounds.PlayAt(CSounds::CHN_WORLD, SOUND_HAMMER_HIT, 1.0f, Pos);
		return;
	}

	// Check focus mode settings
	if(g_Config.m_ClFocusMode && g_Config.m_ClFocusModeHideEffects)
	{
		if(g_Config.m_SndGame && g_Config.m_ClHammerHitEffectSound)
			GameClient()->m_Sounds.PlayAt(CSounds::CHN_WORLD, SOUND_HAMMER_HIT, 1.0f, Pos);
		return;
	}

	// add the explosion
	CParticle p;
	p.SetDefault();
	p.m_Spr = SPRITE_PART_HIT01;
	p.m_Pos = Pos;
	p.m_LifeSpan = 0.3f;
	p.m_StartSize = 120.0f;
	p.m_EndSize = 0.0f;
	p.m_Rot = random_angle();
	p.m_Color.a = Alpha;
	p.m_StartAlpha = Alpha;
	GameClient()->m_Particles.Add(CParticles::GROUP_EXPLOSIONS, &p);
	if(g_Config.m_SndGame)
		GameClient()->m_Sounds.PlayAt(CSounds::CHN_WORLD, SOUND_HAMMER_HIT, 1.0f, Pos);
}

void CEffects::OnRender()
{
	float Speed = 1.0f;
	if(Client()->State() == IClient::STATE_DEMOPLAYBACK)
		Speed = DemoPlayer()->BaseInfo()->m_Speed;

	const int64_t Now = time();
	auto FUpdateClock = [&](bool &Add, int64_t &LastUpdate, int Frequency) {
		Add = Now - LastUpdate > time_freq() / ((float)Frequency * Speed);
		if(Add)
			LastUpdate = Now;
	};
	FUpdateClock(m_Add5hz, m_LastUpdate5hz, 5);
	FUpdateClock(m_Add50hz, m_LastUpdate50hz, 50);
	FUpdateClock(m_Add100hz, m_LastUpdate100hz, 100);

	if(m_Add50hz)
		GameClient()->m_Flow.Update();
}