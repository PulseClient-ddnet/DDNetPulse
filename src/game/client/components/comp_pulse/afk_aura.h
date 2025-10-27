#ifndef GAME_CLIENT_COMPONENTS_AFK_AURA_H
#define GAME_CLIENT_COMPONENTS_AFK_AURA_H

#include <base/vmath.h>

#include <game/client/component.h>

class CAfkAura : public CComponent
{
	float m_LastMovementTime = 0.0f;
	vec2 m_LastPosition = vec2(0, 0);
	vec2 m_AuraPosition = vec2(0, 0);
	bool m_AuraActive = false;
	bool m_AuraParticlesCreated = false;
	float m_FadeOutStartTime;

public:
	virtual void OnRender() override;

	virtual int Sizeof() const override { return sizeof(*this); }
	virtual void OnInit() override;
};

#endif
