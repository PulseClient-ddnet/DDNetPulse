#include "socket_chat.h"

#include "../socket.h"
#include "game/client/gameclient.h"

void CWebSocketChat::ListenerInit()
{
    m_Socket->socket()->on("chat_message", [&](sio::event &ev) { HandleChatMessage(ev); });
    m_Socket->socket()->on("online_update", [&](sio::event &ev) { HandleOnlineUpdate(ev); });
    m_Socket->socket()->on("typing_start", [&](sio::event &ev) { HandleTypingStart(ev); });
    m_Socket->socket()->on("typing_stop", [&](sio::event &ev) { HandleTypingStop(ev); });
    m_Socket->socket()->on("skin_update", [&](sio::event &ev) { HandleSkinUpdate(ev); });

    dbg_msg("test", "%d", *m_IsConnected);
}

void CWebSocketChat::HandleTypingStart(sio::event &ev)
{
    auto Data = ev.get_message();
    if(!Data || Data->get_flag() != sio::message::flag_object)
        return;
    std::string Nickname = Data->get_map()["nickname"]->get_string();

    std::lock_guard<std::mutex> Lock(m_TypingMutex);
    m_TypingUsers.insert(Nickname);
}

void CWebSocketChat::HandleTypingStop(sio::event &ev)
{
    auto Data = ev.get_message();
    if(!Data || Data->get_flag() != sio::message::flag_object)
        return;
    std::string Nickname = Data->get_map()["nickname"]->get_string();

    std::lock_guard<std::mutex> lock(m_TypingMutex);
    m_TypingUsers.erase(Nickname);
}

void CWebSocketChat::HandleSkinUpdate(sio::event &ev)
{
    auto Data = ev.get_message();
    if(!Data || Data->get_flag() != sio::message::flag_object)
        return;

    auto Map = Data->get_map();
    std::string Nickname = Map["nickname"]->get_string();

    SPlayerSkin Skin;
    Skin.m_Name = Map.contains("skin_name") ? Map["skin_name"]->get_string() : "";
    Skin.m_BackupName = "default";
    Skin.m_CustomColors = Map.contains("use_custom_color") ? Map["use_custom_color"]->get_bool() : false;
    Skin.m_BodyColor = Map.contains("body_color") ? std::stoi(Map["body_color"]->get_string()) : 0;
    Skin.m_FeetColor = Map.contains("feet_color") ? std::stoi(Map["feet_color"]->get_string()) : 0;

    std::lock_guard<std::mutex> lock(m_PlayerSkinsMutex);
    m_PlayerSkins[Nickname] = Skin;
}

void CWebSocketChat::HandleChatMessage(sio::event &ev)
{
    auto Data = ev.get_message();
    if(!Data || Data->get_flag() != sio::message::flag_object)
        return;

    auto Map = Data->get_map();
    std::string Nickname = Map["nickname"]->get_string();
    std::string Message = Map["message"]->get_string();

    SPlayerSkin Skin;
    {
        std::lock_guard<std::mutex> lock(m_PlayerSkinsMutex);
        auto it = m_PlayerSkins.find(Nickname);
        if(it != m_PlayerSkins.end())
            Skin = it->second;
    }

    ColorRGBA MsgColor(1.0f, 1.0f, 1.0f, 1.0f);
    if(Map.contains("color"))
    {
        auto c = Map["color"]->get_map();
        MsgColor.r = c["r"] ? (float)c["r"]->get_double() : 1.0f;
        MsgColor.g = c["g"] ? (float)c["g"]->get_double() : 1.0f;
        MsgColor.b = c["b"] ? (float)c["b"]->get_double() : 1.0f;
        MsgColor.a = c["a"] ? (float)c["a"]->get_double() : 1.0f;
    }

    AddMessage(Message, MsgColor, Nickname + ": ", &Skin);

    if(g_Config.m_ClCrossChatDebug)
    {
        char aBuf[128];
        str_format(aBuf, sizeof(aBuf), "[Debug]: Got color: r=%.2f g=%.2f b=%.2f a=%.2f",
                   MsgColor.r, MsgColor.g, MsgColor.b, MsgColor.a);
        AddMessage(aBuf, ColorRGBA(0.0f, 1.0f, 0.0f, 1.0f));
    }
}

void CWebSocketChat::HandleOnlineUpdate(sio::event &ev)
{
	auto Data = ev.get_message();
	if(!Data || Data->get_flag() != sio::message::flag_object)
		return;

	auto UsersArray = Data->get_map()["users"]->get_vector();

	std::vector<SOnlinePlayer> Players;

	for(auto &userMsg : UsersArray)
	{
		if(userMsg->get_flag() != sio::message::flag_string)
			continue;

		std::string Nick = userMsg->get_string();
		SOnlinePlayer Player;
		Player.m_Name = Nick;

		// получить skin из m_PlayerSkins
		{
			std::lock_guard<std::mutex> lock(m_PlayerSkinsMutex);
			auto it = m_PlayerSkins.find(Nick);
			if(it != m_PlayerSkins.end())
				Player.m_Skin = it->second;
		}

		Players.push_back(Player);
	}

	std::lock_guard<std::mutex> lock(m_OnlinePlayersMutex);
	m_OnlinePlayers = Players;
}


void CWebSocketChat::SendChatMessage(const std::string &Msg) const
{
    if(!m_IsConnected)
        return;
    m_Socket->socket()->emit("chat_message", sio::string_message::create(Msg));
}

void CWebSocketChat::AddMessage(const std::string &Msg, ColorRGBA MsgColor, std::string MsgUsername, const SPlayerSkin *MsgSkin)
{
    std::lock_guard<std::mutex> lock(m_MessageMutex);

    SChatMessage MsgStruct;
    MsgStruct.m_Text = Msg;
    MsgStruct.m_Username = MsgUsername;
    MsgStruct.m_Color = MsgColor;

    if(MsgSkin)
        MsgStruct.m_Skin = *MsgSkin;

    m_ChatMessages.push_back(MsgStruct);

    if(m_ChatMessages.size() > 100)
        m_ChatMessages.erase(m_ChatMessages.begin());
}

std::vector<CWebSocketChat::SChatMessage> CWebSocketChat::GetMessages()
{
    std::lock_guard<std::mutex> Lock(m_MessageMutex);
    return m_ChatMessages;
}

void CWebSocketChat::SendTypingState(bool State)
{
    if(!m_IsConnected)
        return;

    if(State)
        m_Socket->socket()->emit("typing_start", sio::string_message::create(g_Config.m_PlayerName));
    else
        m_Socket->socket()->emit("typing_stop", sio::string_message::create(g_Config.m_PlayerName));
}
