#ifndef GAME_CLIENT_COMPONENTS_COMP_PULSE_SOCKET_MODULES_SOCKET_CHAT_H
#define GAME_CLIENT_COMPONENTS_COMP_PULSE_SOCKET_MODULES_SOCKET_CHAT_H

#include "../socket_component.h"
#include "base/color.h"

#include <generated/protocol.h>

#include <sio_client.h>

#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

class CWebSocketChat : public CWebSocketComponent
{
public:

	struct SPlayerSkin
	{
		std::string m_Name;
		std::string m_BackupName;
		bool m_CustomColors = false;
		int m_FeetColor = 0;
		int m_BodyColor = 0;
		int m_Emote = EMOTE_NORMAL;
		ColorRGBA m_ColorFeet = ColorRGBA();
		ColorRGBA m_ColorBody = ColorRGBA();

		bool IsValid() const { return !m_Name.empty(); }
	};
	std::unordered_map<std::string, SPlayerSkin> m_PlayerSkins;
	std::mutex m_PlayerSkinsMutex;

	struct SChatMessage
	{
		std::string m_Text;
		std::string m_Username;
		SPlayerSkin m_Skin;
		ColorRGBA m_Color;
	};
	std::vector<SChatMessage> GetMessages();

	struct SOnlinePlayer
	{
		std::string m_Name;
		SPlayerSkin m_Skin;
	};





	//sio::client m_Socket;

	void HandleChatMessage(sio::event &Ev);
	void SendChatMessage(const std::string &Msg) const;
	void AddMessage(const std::string &Msg,
		ColorRGBA MsgColor = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f),
		std::string MsgUsername = "",
		const SPlayerSkin *MsgSkin = nullptr);

	void HandleOnlineUpdate(sio::event &Ev);


	std::mutex m_MessageMutex;
	std::vector<SChatMessage> m_ChatMessages;

	std::vector<SOnlinePlayer> m_OnlinePlayers;
	std::mutex m_OnlinePlayersMutex;

	std::unordered_set<std::string> m_TypingUsers;
	std::mutex m_TypingMutex;
	bool m_IsTyping = false;
	int64_t m_LastTypeTime = 0;



	void ListenerInit() override;


	void HandleTypingStart(sio::event &ev);
	void HandleTypingStop(sio::event &ev);
	void HandleSkinUpdate(sio::event &ev);

	void SendTypingState(bool State);

};

#endif
