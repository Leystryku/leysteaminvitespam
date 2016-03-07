
#include "leysteamspam.h"
#include "leynet.h"

#include <mmsystem.h>

#define STEAMWORKS_CLIENT_INTERFACES
#include <stdint.h>
#include "osw/Steamworks.h"
#include "osw/ISteamUser017.h"


CSteamAPILoader g_SteamAPILoader;
// Client handles
HSteamPipe g_hSteamPipe;
HSteamUser g_hSteamUser;
// Versioned interfaces
ISteamClient017 *steamclient = 0;
IClientEngine *clientengine = 0;
ISteamUser017 *steamuser = 0;
IClientAudio *clientaudio = 0;

#ifdef _WIN32
#define _sleep Sleep
#endif

bool InitSteam()
{
	CreateInterfaceFn fnApiInterface = g_SteamAPILoader.Load();


	if (!fnApiInterface)
		return false;

	if (!(steamclient = (ISteamClient017 *)fnApiInterface(STEAMCLIENT_INTERFACE_VERSION_017, NULL)))
		return false;

	if (!(clientengine = (IClientEngine *)fnApiInterface(CLIENTENGINE_INTERFACE_VERSION, NULL)))
		return false;

	if (!(g_hSteamPipe = steamclient->CreateSteamPipe()))
		return false;

	if (!(g_hSteamUser = steamclient->ConnectToGlobalUser(g_hSteamPipe)))
		return false;

	if (!(steamuser = (ISteamUser017 *)steamclient->GetISteamUser(g_hSteamUser, g_hSteamPipe, STEAMUSER_INTERFACE_VERSION_017)))
		return false;

	if (!(clientaudio = (IClientAudio *)clientengine->GetIClientAudio(g_hSteamUser, g_hSteamPipe, CLIENTAUDIO_INTERFACE_VERSION)))
		return false;

	return true;
}

int main(int argc, const char *argv[])
{

	if (!argv[1] || !argv[2] || !argv[3] || !argv[4] || !argv[5] ||!argv[6] ||!argv[7])
	{
		printf("USAGE: (STEAMID64 OR 0) msg connectstr steamstatus delay someip someport\n");
		return 0;
	}
	
	char ipbuf[512];
	strcpy(ipbuf, argv[6]);


	int ip = 0;
	ip = ip | (atoi(strtok(ipbuf, ".")) << 24);
	ip = ip | (atoi(strtok(0, ".")) << 16);
	ip = ip | (atoi(strtok(0, ".")) << 8);
	ip = ip | (atoi(strtok(0, ".")));


	int delay = atoi(argv[5]);

	InitSteam();

	ISteamFriends015 *friends = (ISteamFriends015 *)steamclient->GetISteamFriends(g_hSteamUser, g_hSteamPipe, STEAMFRIENDS_INTERFACE_VERSION_015);

	CSteamID sid;
	sid.SetFromUint64(76561197960279927llu);

	steamuser->AdvertiseGame(sid, ip, atoi(argv[7]));

	bool notarget = atoi(argv[1]) == 0;
	bool nomsg = strlen(argv[2]) < 2;
	bool noinvite = strlen(argv[4]) < 2;

	CSteamID target;

	if ( !notarget )
	{
		target.SetFromUint64((unsigned long long)_atoi64(argv[1]));
	}




	friends->SetRichPresence("status", argv[4]);
	friends->SetRichPresence("connect", argv[3]);

	int i = 0;

	while (true)
	{

		Sleep(delay);

		if (notarget)
		{

			CSteamID noobid = friends->GetFriendByIndex(i, k_EFriendFlagImmediate);

			if(!noinvite)
				friends->InviteUserToGame(noobid, argv[3]);

			if (!nomsg)
			{
				friends->ReplyToFriendMessage(noobid, argv[2]);
			}

			i++;

			if (i == friends->GetFriendCount(k_EFriendFlagImmediate))
			{
				i = 0;

			}
			continue;
		}

		if(!noinvite)
			friends->InviteUserToGame(target, argv[3]);

		if (!nomsg)
		{
			friends->ReplyToFriendMessage(target, argv[2]);
		}
			

	}

	return 1;
}