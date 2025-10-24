#ifndef GAME_CLIENT_COMPONENTS_COMP_PULSE_SOCKET_REQUEST_H
#define GAME_CLIENT_COMPONENTS_COMP_PULSE_SOCKET_REQUEST_H

#include <game/client/component.h>
#include <sio_client.h>
#include <mutex>
#include <vector>
#include <string>

class CWebSocket : public CComponent
{
	void SocketConnect();
	void SetupSocketListeners();
	void HandleChatMessage(sio::event &ev);
	void HandleOnlineUpdate(sio::event &ev);

	std::vector<std::string> m_ChatMessages;
	std::mutex m_MessageMutex;

public:
	std::vector<std::string> m_OnlinePlayers;
	std::mutex m_OnlinePlayersMutex;

	void SocketDisconnect();
	void SocketListen(const std::string &name);
	void SocketMessage(const char *pEvent, const sio::message::list pData);
	void SendChatMessage(const std::string &Msg);

	void AddMessage(const std::string &Msg);
	std::vector<std::string> GetMessages();

	virtual void OnInit() override;
	virtual int Sizeof() const override { return sizeof(*this); }
};

#endif
