#ifndef GAME_CLIENT_COMPONENTS_PLAYERS_EXTENDED_H
#define GAME_CLIENT_COMPONENTS_PLAYERS_EXTENDED_H

#include <game/client/components/players.h>
#include <game/client/render.h>
#include <game/generated/protocol.h>

class CPlayerExtended : public CPlayers
{
public:
	CPlayerExtended();
	virtual ~CPlayerExtended() {}

	virtual void OnInit() override;
	virtual void OnRender() override;

private:
	void RenderPlayerAura(vec2 Position, float Alpha, vec4 BodyColor);

	float m_LastMovementTime;
	vec2 m_LastPosition;
	float m_LastAlpha;
};

#endif
