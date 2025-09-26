/* (c) DDNetPulse contributors. See licence.txt in the root of the distribution for more information. */
#ifndef GAME_CLIENT_COMPONENTS_COMP_PULSE_ANTI_QUIT_H
#define GAME_CLIENT_COMPONENTS_COMP_PULSE_ANTI_QUIT_H

#include <game/client/component.h>

class CAntiQuit final : public CComponent
{
public:
    int Sizeof() const override { return sizeof(*this); }

    void OnInterfacesInit(class CGameClient *pClient) override;
    void OnUpdate() override;

    void Request();

private:
    bool m_PopupRequested = false;
};

#endif // GAME_CLIENT_COMPONENTS_COMP_PULSE_ANTI_QUIT_H


