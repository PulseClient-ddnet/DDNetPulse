#include "socket_request.h"
#include "game/client/gameclient.h"

void CWebSocket::OnInit()
{
    if(g_Config.m_ClCrossChatAutoConnect)
        SocketListen(Client()->PlayerName());
}

void CWebSocket::SocketConnect()
{
    CGameClient *pClient = (CGameClient *)GameClient();
    pClient->m_SocketIOConnected = false;

    pClient->m_SocketIO.set_open_listener([this, pClient]() {
        pClient->m_SocketIOConnected = true;
        dbg_msg("socket.io", "Connected to server");
    });

    pClient->m_SocketIO.set_close_listener([this, pClient](sio::client::close_reason const &) {
        pClient->m_SocketIOConnected = false;
        dbg_msg("socket.io", "Disconnected from server");
    });

    pClient->m_SocketIO.set_fail_listener([this, pClient]() {
        pClient->m_SocketIOConnected = false;
        dbg_msg("socket.io", "Connection failed");
    });

    pClient->m_SocketIO.connect(g_Config.m_ClSocketNameserver);
}

void CWebSocket::SocketDisconnect()
{
    CGameClient *pClient = (CGameClient *)GameClient();
    if(pClient->m_SocketIOConnected)
    {
        pClient->m_SocketIO.close();
        pClient->m_SocketIOConnected = false;
    }
}

void CWebSocket::SocketListen(const std::string &Name)
{
    CGameClient *pClient = (CGameClient *)GameClient();
    std::string player = Name;

    SetupSocketListeners();

    pClient->m_SocketIO.connect(g_Config.m_ClSocketNameserver);
    pClient->m_SocketIO.socket()->emit("nickname", sio::string_message::create(player));
}

void CWebSocket::SetupSocketListeners()
{
    CGameClient *pClient = (CGameClient *)GameClient();

    pClient->m_SocketIO.set_open_listener([&]() { dbg_msg("socket.io", "Connecting to server"); });
    pClient->m_SocketIO.set_close_listener([&](sio::client::close_reason const &) { dbg_msg("socket.io", "Disconnected from server"); });

    pClient->m_SocketIO.socket()->on("chat_message", [&](sio::event &ev) { HandleChatMessage(ev); });
    pClient->m_SocketIO.socket()->on("online_update", [&](sio::event &ev) { HandleOnlineUpdate(ev); });
}

void CWebSocket::HandleChatMessage(sio::event &ev)
{
    auto Data = ev.get_message();
    if(!Data) return;

    if(Data->get_flag() == sio::message::flag_string)
    {
        std::string Msg = Data->get_string();
        dbg_msg("socket.io", "%s", Msg.c_str());

        if(g_Config.m_ClCrossChatInGameChat || IClient::STATE_ONLINE)
        {
            char aBuf[256];
            str_format(aBuf, sizeof(aBuf), "echo ->: %s", Msg.c_str());
            Console()->ExecuteLine(aBuf);
        }

        AddMessage(Msg);
    }
    else if(Data->get_flag() == sio::message::flag_object)
    {
        auto Nickname = Data->get_map()["nickname"]->get_string();
        auto Message = Data->get_map()["message"]->get_string();

        dbg_msg("socket.io", "[%s]: %s", Nickname.c_str(), Message.c_str());

        if(g_Config.m_ClCrossChatInGameChat || IClient::STATE_ONLINE)
        {
            char aBuf[256];
            str_format(aBuf, sizeof(aBuf), "echo -> [%s]: %s", Nickname.c_str(), Message.c_str());
            Console()->ExecuteLine(aBuf);
        }

        AddMessage("[" + Nickname + "]: " + Message);
    }
}

void CWebSocket::HandleOnlineUpdate(sio::event &ev)
{
    auto Data = ev.get_message();
    if(!Data || Data->get_flag() != sio::message::flag_object) return;

    auto UsersArray = Data->get_map()["users"]->get_vector();
    std::vector<std::string> OnlinePlayers;
    for(auto &userMsg : UsersArray)
        if(userMsg->get_flag() == sio::message::flag_string)
            OnlinePlayers.push_back(userMsg->get_string());

    std::lock_guard<std::mutex> lock(m_OnlinePlayersMutex);
    m_OnlinePlayers = OnlinePlayers;
}

void CWebSocket::SendChatMessage(const std::string &Msg)
{
    CGameClient *pClient = (CGameClient *)GameClient();
    if(pClient->m_SocketIOConnected)
        pClient->m_SocketIO.socket()->emit("chat_message", sio::string_message::create(Msg));
}

void CWebSocket::AddMessage(const std::string &Msg)
{
    std::lock_guard<std::mutex> lock(m_MessageMutex);
    m_ChatMessages.push_back(Msg);
    if(m_ChatMessages.size() > 100)
        m_ChatMessages.erase(m_ChatMessages.begin());
}

std::vector<std::string> CWebSocket::GetMessages()
{
    std::lock_guard<std::mutex> lock(m_MessageMutex);
    return m_ChatMessages;
}

void CWebSocket::SocketMessage(const char *pEvent, const sio::message::list pData)
{
    CGameClient *pClient = (CGameClient *)GameClient();
    pClient->SendSocketMessage(pEvent, pData);
}
