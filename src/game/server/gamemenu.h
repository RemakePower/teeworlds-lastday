#ifndef GAME_SERVER_GAMEMENU_H
#define GAME_SERVER_GAMEMENU_H

#include <base/tl/array.h>
#include <string>

class CMenu
{
	class CGameContext *m_pGameServer;
	CGameContext *GameServer() const { return m_pGameServer; }

	const char* Localize(const char* pText) const;
    char m_aLanguageCode[16];

    array<std::string> m_DataTemp;
    
    void GetData(int Page);
public:
    CMenu(CGameContext *pGameServer);

	typedef void (*MenuCallback)(int ClientID, void *pUserData);
	
    void Register(const char *pName, int Pages, MenuCallback pfnFunc, void *pUser, bool CloseMenu);

    void ShowMenu(int ClientID, int Line);
    void UseOptions(int ClientID);

private:
    class COptions
	{
	public:
		const char *m_pName;
		int m_Page;
		MenuCallback m_pfnCallback;
		void *m_pUserData;
		bool m_CloseMenu;
	};

	array<COptions*> m_apOptions;

	int FindOption(const char *pName, int Pages);
};

#endif