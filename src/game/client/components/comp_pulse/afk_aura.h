#ifndef GAME_CLIENT_COMPONENTS_AFK_AURA_H
#define GAME_CLIENT_COMPONENTS_AFK_AURA_H

#include <game/client/component.h>
#include <base/vmath.h>

class CAfkAura : public CComponent
{
public:
	CAfkAura();

	virtual void OnRender() override;

	virtual int Sizeof() const override { return sizeof(*this); }
	virtual void OnInit() override;


private:
	float m_LastMovementTime;
	vec2 m_LastPosition;
	vec2 m_AuraPosition;
	bool m_AuraActive;
	bool m_AuraParticlesCreated;
	float m_FadeOutStartTime;
};

#endif 