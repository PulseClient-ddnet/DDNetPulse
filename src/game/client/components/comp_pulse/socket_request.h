#ifndef GAME_CLIENT_COMPONENTS_COMP_PULSE_SOCKET_REQUEST_H
#define GAME_CLIENT_COMPONENTS_COMP_PULSE_SOCKET_REQUEST_H

#include "base/color.h"

#include <game/client/component.h>

#include <sio_client.h>

#include <mutex>
#include <string>
#include <vector>

class CWebSocket : public CComponent
{
	void SocketConnect();
	void SetupSocketListeners();
	void HandleChatMessage(sio::event &ev);
	void HandleOnlineUpdate(sio::event &ev);



public:
	struct SChatMessage
	{
		std::string m_Text;
		ColorRGBA m_Color;
	};
	std::mutex m_MessageMutex;
protected:
	std::vector<SChatMessage> m_ChatMessages;
public:
	bool m_IsConnected;

	std::vector<std::string> m_OnlinePlayers;
	std::mutex m_OnlinePlayersMutex;

	void SocketDisconnect();
	void SocketListen(const std::string &name);
	void SendChatMessage(const std::string &Msg);
	bool IsConnected() const;

	void AddMessage(const std::string &Msg, ColorRGBA Color = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	std::vector<SChatMessage> GetMessages();

	virtual void OnInit() override;
	virtual int Sizeof() const override { return sizeof(*this); }
};

#endif
