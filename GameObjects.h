#pragma once


#include "D3DApplication.h"


class Player;


class GameObject
{
protected:	
	// protected constructor -- for derivation only
	GameObject(D3DModel* m);

public:
	virtual ~GameObject() { }
	virtual void Update() = 0;
	virtual void Render();
	void SetX(float xx)   { x = xx; }
	void SetY(float yy)   { y = yy; }
	void SetZ(float zz)   { z = zz; }
	void SetXYZ(float xx, float yy, float zz)  { x=xx; y=yy; z=zz; }
	float GetX()  { return x; }
	float GetY()  { return y; }
	float GetZ()  { return z; }
	float GetXYZ(float& xx, float& yy, float& zz) { xx=x; yy=y; zz=z; }
	D3DModel* GetModel()  { return model; }
private:
	D3DModel* model;
	float x, y, z;
};




class Ball : public GameObject
{
public:
	Ball(D3DModel* model);
	void SetPlayers(Player* p1, Player* p2);
	float GetXinc()  { return incX; }
	float GetYinc()  { return incY; }
	void IncSpeed();
	float GetSpeed()  { return static_cast<float>(fabs(incY)); }

private:
	void Update();
	Player* GetPlayerOnSide(float);
	Player* GetOtherPlayer(const Player* p);
	bool SameSide(float y, float yy);
private:
	float incX, incY;
	Player* player[2];
};

class Player : public GameObject
{
protected:
	// protected constructor -- for derivation only
	Player(D3DModel* model);

public:
	virtual void Update() = 0;
	virtual void BallReversal(bool oncomming) { }
	virtual void IncreaseAnger() { }
	void SetBall(Ball* b)	{ assert(ball==0); ball = b;	}
	void PointScored(bool myScore);
	int GetScore()			{ return score; }
	Ball* GetBall()			{ return ball; }
	void Render();
private:
	int score;
	Ball* ball;
};


class HumanPlayer : public Player
{
public:
	HumanPlayer(D3DModel* model);
	virtual void Update();
private:
	int controllerX;
};

class ArtificialPlayer : public Player
{
public:

	ArtificialPlayer(D3DModel* model);

private:
	enum PlayerState
	{
		STATE_SERVING,
		STATE_REACTING
	};
	virtual void BallReversal(bool oncomming);
	virtual void Update();
	virtual void IncreaseAnger();
	void CalculateTarget();

private:
	PlayerState state;
	float speed;
	float targetX;
};