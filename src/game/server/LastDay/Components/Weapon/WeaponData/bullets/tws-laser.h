#ifndef GAME_SERVER_LASTDAY_BULLETS_LASER_H
#define GAME_SERVER_LASTDAY_BULLETS_LASER_H

#include <game/server/entity.h>

class CTWSLaser : public CEntity
{
public:
	CTWSLaser(CGameWorld *pGameWorld, vec2 Pos, vec2 Direction, float StartEnergy, int Owner);

	void Reset() override;
	void Tick() override;
	void TickPaused() override;
	void Snap(int SnappingClient) override;

protected:
	bool HitCharacter(vec2 From, vec2 To);
	void DoBounce();

private:
	vec2 m_From;
	vec2 m_Dir;
	float m_Energy;
	int m_Bounces;
	int m_EvalTick;
	int m_Owner;
};

#endif
