#include "stdafx.h"
#include "GameObjects.h"
#include "Pong.h"
#include "eiInput.h"
#include "Constants.h"


extern PongApp theApp;


GameObject::GameObject(D3DModel* m)
	:	model(m),
		x(0),
		y(0),
		z(0)
{
}

void GameObject::Render()
{
	assert(model);

	model->SetLocation( x, y, z );
	model->Render();
}

Ball::Ball(D3DModel* model)
	:	GameObject(model),
		incX(IncX),
		incY(IncY)
{
	SetXYZ( 0.0f, 0.0f, 1.0f );
}

void Ball::Update()
{
	assert(player[0] && player[1]);

	float x = GetX();
	float y = GetY();

	// update position
	x += incX;
	y += incY;

	// check for collision with wall, and bounce
	if (x > LimitX || x < -LimitX)
	{
		incX = -incX;
		x += incX;

		Player* p = GetPlayerOnSide( incY );
		p->BallReversal( true );
		GetOtherPlayer( p )->BallReversal( false );
	}

	// check for collision with paddle
	float absBallY = static_cast<float>(fabs( y ));
	float absIncY = static_cast<float>( fabs(incY) );
	if (absBallY > PaddlePos-Padding && absBallY <= PaddlePos-Padding+absIncY*2)
	{
		// ball is in the right place horizontally
		// now check if it's not colliding with the
		// paddle whose side the ball is currently exiting
		Player* p = GetPlayerOnSide( incY );
		if (SameSide( p->GetY(), y ))
		{
			// now check vertical alignment
			float px = p->GetX();
			if (x > px-PaddleWidth && x < px+PaddleWidth)
			{
				// it's a hit! -- now ricochet off the paddle
				incX = (x - px) / 4.0f;

				incY = -incY;
				y += incY;

				p->BallReversal( false );
				GetOtherPlayer( p )->BallReversal( true );
			}
		}
	}

	// check for collision with bottom or top of window
	if (y > LimitY || y < -LimitY)
	{
		// yup -- retreat to previous position
		incY = -incY;
		y += incY;

		// this constitutes a score,
		// notify both players
		// (the AI player "gets angry" when the 
		//  human scores, so notify both players)
		
		// 
		Player* p = GetPlayerOnSide( y );
		p->PointScored( false );
		p->BallReversal( false );

		Player* op = GetOtherPlayer( p );
		op->PointScored( true );
		op->BallReversal( true );
	}

	SetX( x );
	SetY( y );
}

void Ball::SetPlayers(Player* human, Player* ai)
{
	assert(human);
	assert(ai);

	player[0] = human;
	player[1] = ai;
	
	// the ball starts in the direction of the AI paddle,
	// so notify the ai player to prompt a reaction
	ai->BallReversal( true );
}

Player* Ball::GetPlayerOnSide(float i)
{
	if (i > 0.0f)
	{
		if (player[0]->GetY() > 0.0f)
			return player[0];
		else
			return player[1];
	}
	else
		if (player[0]->GetY() > 0.0f)
			return player[1];

	return player[0];
}

Player* Ball::GetOtherPlayer(const Player* p)
{
	if (player[0] == p)
		return player[1];
	return player[0];
}

bool Ball::SameSide(float y, float yy)
{
	return (y > 0.0f && yy > 0) || (y < 0.0f && yy < 0);
}

void Ball::IncSpeed()
{
	// increase ball speed, meaning differes depending on direction
	if (incY > 0)
		incY++;
	else
		incY--;
}

Player::Player(D3DModel* model)
	:	GameObject(model),
		score(0),
		ball(0)
{
}

void Player::Render()
{
	GameObject::Render();	// render the player model first

	// then render the player's score
	CD3DFont* font = theApp.GetBigFont();
	assert(font);

	char str[10];
	sprintf(str, "%d", score);

	if (GetY() > 0.0f)
		font->DrawText( 30, 40, 0x88ffff00, str, 0L );
	else
		font->DrawText( 30, 380, 0x88ffff00, str, 0L );

}

void Player::PointScored(bool myScore)
{
	if (myScore)
		score++; 
	else
		IncreaseAnger();
}

HumanPlayer::HumanPlayer(D3DModel* model)
	:	Player(model),
		controllerX(0)
{
	SetXYZ( 0.0f, -PaddlePos, 0.0f );
}


void HumanPlayer::Update()
{
	eiPointer* mouse = theApp.GetMouse();

	int curCtrlX = controllerX;

	DIDEVICEOBJECTDATA event;
	while (mouse->GetEvent( event ))
	{
		switch (event.dwOfs)
		{
		case DIMOFS_X:
			controllerX += event.dwData;
			break;
		}
	}

	float newX = controllerX * 2.0f;
	if (newX > LimitX+100.0f || newX < -LimitX-100.0f)
		controllerX = curCtrlX;
	else
		SetX( newX );
}

ArtificialPlayer::ArtificialPlayer(D3DModel* model)
	:	Player(model),
		state(STATE_SERVING),
		speed(InitAISpeed),
		targetX(-200.0f)															    
{
	SetXYZ( 0.0f, PaddlePos, 0.0f );
}

void ArtificialPlayer::BallReversal(bool oncomming)
{
	if (oncomming)
	{
		state = STATE_REACTING;
		CalculateTarget();
	}
	else
		state = STATE_SERVING;
}

void ArtificialPlayer::Update()
{
	// ball is headed away -- do nothing
	if (state == STATE_SERVING)
		return;

	// try to move to 'targetX', moving at 'speed'
	float curX = GetX();
	if (curX != targetX)
	{
		if (curX > targetX)
		{
			if (curX - speed <= targetX)
				SetX( targetX );
			else
				SetX( curX - speed );
		}
		else
		{
			if (curX + speed >= targetX)
				SetX( targetX );
			else
				SetX( curX + speed );
		}
	}
}

void ArtificialPlayer::CalculateTarget()
{
	Ball* b = GetBall();
	assert(b);

	float bx = b->GetX();
	float by = b->GetY();
	float bxi = b->GetXinc();
	float byi = b->GetYinc();

	float paddlePos = GetY();

	float distY = static_cast<float>(fabs(paddlePos - by));	// vertial distance from ball to paddle
	float distYInc = static_cast<float>(fabs(distY / byi));	// steps before interception
	float distX = distYInc * bxi;			// horitonal distance we must move to intercept
	int r = (rand()%(AIRandSlop*2)) - AIRandSlop;	// random "slop" 

	targetX = bx + distX + r;			// where we want to be to intercept ball
}

// gets called when the other player scores
void ArtificialPlayer::IncreaseAnger()
{
	// 
	speed += IncAISpeed;		// boost my speed
	GetBall()->IncSpeed();		// boost ball speed
}


