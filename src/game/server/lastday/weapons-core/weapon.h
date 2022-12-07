#ifndef GAME_SERVER_LASTDAY_WEAPON_H
#define GAME_SERVER_LASTDAY_WEAPON_H

#include <game/server/define.h>
#include <base/vmath.h>

class CGameContext;
class CGameWorld;

// Weapon Core

class IWeapon
{
private:
    int m_WeaponID;
    CGameContext *m_pGameServer;
    int m_ShowType;
    int m_FireDelay;
public:
    IWeapon(CGameContext *pGameServer, int WeaponID, int ShowType, int FireDelay);
    CGameContext *GameServer() const;
    CGameWorld *GameWorld() const;
    int GetWeaponID() const;
    int GetShowType() const;
    int GetFireDelay() const;
    virtual void OnFire(int Owner, vec2 Dir, vec2 Pos) = 0;
};

class CWeapon : public IWeapon
{
    virtual void Fire(int Owner, vec2 Dir, vec2 Pos) = 0;
public:
    CWeapon(CGameContext *pGameServer, int WeaponID, int m_ShowType, int FireDelay);
    void OnFire(int Owner, vec2 Dir, vec2 Pos) override;
};

// Weapon Core End


// Weapon system and init
struct WeaponSystem
{
	void InitWeapon(int Number, IWeapon *pWeapon);
	IWeapon *m_aWeapons[NUM_LASTDAY_WEAPONS];
};

struct WeaponInit
{
public:
    void InitWeapons(CGameContext *pGameServer);
};

extern WeaponSystem g_Weapons;
#endif