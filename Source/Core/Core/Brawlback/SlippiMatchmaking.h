#pragma once

#include "Common/CommonTypes.h"
#include "Common/Thread.h"
#include <random>
//#include "Core/Slippi/SlippiNetplay.h"
#include "Netplay/Netplay.h"

#ifndef _WIN32
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include <unordered_map>
#include <vector>

#include "include/json.hpp"
using json = nlohmann::json;

class SlippiMatchmaking
{
  public:

    struct UserInfo
	{
		std::string uid = "";
		std::string playKey = "";
		std::string displayName = "";
		std::string connectCode = "";
		std::string latestVersion = "";
		std::string fileContents = "";

		int port;
	};

	SlippiMatchmaking(UserInfo user);
	~SlippiMatchmaking();

	enum OnlinePlayMode
	{
		RANKED = 0,
		UNRANKED = 1,
		DIRECT = 2,
		TEAMS = 3,
	};

	enum ProcessState
	{
		IDLE,
		INITIALIZING,
		MATCHMAKING,
		OPPONENT_CONNECTING,
		CONNECTION_SUCCESS,
		ERROR_ENCOUNTERED,
	};
    enum class SlippiConnectStatus
	{
		NET_CONNECT_STATUS_UNSET,
		NET_CONNECT_STATUS_INITIATED,
		NET_CONNECT_STATUS_CONNECTED,
		NET_CONNECT_STATUS_FAILED,
		NET_CONNECT_STATUS_DISCONNECTED,
	};


	struct MatchSearchSettings
	{
		OnlinePlayMode mode = OnlinePlayMode::RANKED;
		std::string connectCode = "";
	};

	void FindMatch(MatchSearchSettings settings);
	void MatchmakeThread();
	ProcessState GetMatchmakeState();
	bool IsSearching();
	//std::unique_ptr<SlippiNetplayClient> GetNetplayClient();
	std::string GetErrorMessage();
	int LocalPlayerIndex();
	std::vector<UserInfo> GetPlayerInfo();
	std::string GetPlayerName(u8 port);
	std::vector<u16> GetStages();
	u8 RemotePlayerCount();
	static bool IsFixedRulesMode(OnlinePlayMode mode);
	//static bool IsMexMode(bool isCurrentGameMex, OnlinePlayMode mode);

  protected:
	const std::string MM_HOST_DEV = "mm2.slippi.gg";
	const std::string MM_HOST_PROD = "mm.slippi.gg";
	const u16 MM_PORT = 43113;

	std::string MM_HOST = "";
	//bool isMex = false;

    UserInfo m_user;

	ENetHost *m_client;
	ENetPeer *m_server;

	std::default_random_engine generator;

	bool isMmConnected = false;

	std::thread m_matchmakeThread;

	MatchSearchSettings m_searchSettings;

	ProcessState m_state;
	std::string m_errorMsg = "";

	int m_isSwapAttempt = false;

	int m_hostPort;
	int m_localPlayerIndex;
	std::vector<std::string> m_remoteIps;
	std::vector<UserInfo> m_playerInfo;
	std::vector<u16> m_allowedStages;
	bool m_joinedLobby;
	bool m_isHost;

	//std::unique_ptr<SlippiNetplayClient> m_netplayClient;

	const std::unordered_map<ProcessState, bool> searchingStates = {
	    {ProcessState::INITIALIZING, true},
	    {ProcessState::MATCHMAKING, true},
	    {ProcessState::OPPONENT_CONNECTING, true},
	};

	std::string getSlippiMMHost();
	std::string getMexMMHost();
	std::string getMMHostForSearchMode();

	void disconnectFromServer();
	void terminateMmConnection();
	void sendMessage(json msg);
	int receiveMessage(json &msg, int maxAttempts);

	void sendHolePunchMsg(std::string remoteIp, u16 remotePort, u16 localPort);

	void startMatchmaking();
	void handleMatchmaking();
	void handleConnecting();
};
