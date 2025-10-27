#ifndef GAME_CLIENT_COMPONENTS_COMP_PULSE_SOCKET_REQUEST_H
#define GAME_CLIENT_COMPONENTS_COMP_PULSE_SOCKET_REQUEST_H

#include "base/color.h"

#include <game/client/component.h>

#include <sio_client.h>

#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

class CWebSocket : public CComponent
{
	void SetupSocketListeners();
	void HandleChatMessage(sio::event &Ev);
	void HandleOnlineUpdate(sio::event &Ev);

	std::mutex m_SkinMutex;

public:
	struct SChatMessage
	{
		std::string m_Text;
		ColorRGBA m_Color;
	};
	std::mutex m_MessageMutex;
	std::vector<SChatMessage> m_ChatMessages;

	bool m_IsConnected;

	std::vector<std::string> m_OnlinePlayers;
	std::mutex m_OnlinePlayersMutex;

	std::unordered_set<std::string> m_TypingUsers;
	std::mutex m_TypingMutex;
	bool m_IsTyping = false;
	int64_t m_LastTypeTime = 0;

	void SocketConnect();
	void SocketDisconnect();
	void ConnectAndSetup();
	void SocketListen(const std::string &Name);
	void SendChatMessage(const std::string &Msg);
	bool IsConnected() const;

	void OnOpen();
	void OnClose();

	void AddMessage(const std::string &Msg, ColorRGBA MsgColor = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f));
	std::vector<SChatMessage> GetMessages();
	void SetPlayerSkin(const std::string &SkinName, const std::string &BodyColor, const std::string &FeetColor, bool IsCustomColor);

	void OnInit() override;
	int Sizeof() const override { return sizeof(*this); }
};

#endif
