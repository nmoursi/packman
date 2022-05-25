#include "../headers/Pacman.h"
//random numbers
#define INTIAL_ROW  23
#define INTIAL_COL  14
Pacman::Pacman(int intialRow, int intialColumn, float size, Board* board) :
	Character( intialRow,  intialColumn,  size,  board),mLives(3),
	mPowerUp(false),mScore(0),powerUpTime(7),extraLife(false)
{
	speedTimer = 250;
	resetPosition();
}

Pacman& Pacman::setPowerUpTime(int t)
{
	powerUpTime = t; return *this;
}

int Pacman::getPowerUpTime() const
{
	return powerUpTime;
}

void Pacman::checkPowerUpTime()
{
	if (mPowerUp && currentTime.getElapsedTime().asSeconds() >= powerUpTime)
	{	setPowerUp(false);
	currentTime.restart();
	}
}

Pacman& Pacman::incrementScore(unsigned int s)
{
	mScore += s;
	if (mScore >= 10000 && !extraLife)
	{
		addLive(1);
		extraLife = true;
	}
	return *this;
}
Pacman& Pacman::setPowerUp(bool p)
{
	mPowerUp = p;
	currentTime.restart();
	notify(); 
	return *this;
}
bool Pacman::getPowerUp()const { return mPowerUp; }

Pacman& Pacman::setLives(unsigned int l) 
{
	mLives = l;
	return *this;
}
unsigned int Pacman::getLives()const { return mLives; }

unsigned int Pacman::getScore()const{return mScore;}

void Pacman::addLive( int l)
{
	mLives += l;
}

void Pacman::resetPosition()
{
	if (mLives <= 0)
	{
		mSprite.setColor(Color::Black);
		mAlive = false;
	}
	
	setTexture("../images/Pacman.png", 2, 4);
	mDirection = STOP;
	mAlive = true;
	mCurrentColumn = INTIAL_COL;
	mCurrentRow = INTIAL_ROW;
	mSprite.setPosition(mPositionOnWindow.x + mCurrentColumn * mSize, mPositionOnWindow.y + mCurrentRow * mSize);
}

void Pacman::die(sf::RenderWindow& w)
{
	Audio::getInstance()->playDeath();
	mAlive = false;
	--mLives;
	setTexture("../images/Death.png", 12, 1);

	sf::Clock timer;
	timer.restart();
	int i = 0;
	while (i < 12) {
		if (timer.getElapsedTime().asMilliseconds() > 150)
		{
			animateDie();
			timer.restart();
			i++;
			w.draw(mSprite);
			w.display();

		}
	}
	
	resetPosition();
}

void Pacman::addObserver(IObserver* observer)
{
	if (observer == nullptr)
		return;
	mObserverList.push_back(observer);

}
void Pacman::removeObserver(IObserver* observer)
{
	if (observer == nullptr)
		return;
	mObserverList.remove(observer);
}
void Pacman::notify()
{
	std::list<IObserver*>::iterator itr= mObserverList.begin();
	for (itr; itr != mObserverList.end(); itr++)
		(*itr)->update(this->mPowerUp);
}



void Pacman::animateMove()
{
	
		if (mTexture == nullptr)
			return;
		float left = (float) mSprite.getTextureRect().left, top = (float)mSprite.getTextureRect().top;
		switch (mDirection)
		{
		case UP: top = 2; break;
		case RIGHT:top = 0; break;
		case LEFT:top = 1; break;
		case DOWN:top = 3; break;
		default:break;

		}

		int i = (int) left / 16;
		++i %= 2;
		mSprite.setTextureRect(sf::IntRect(16 * i, (int) top * 15, 16, 15));

	
}
void Pacman::animateDie()
{
	
	
	int i = (mSprite.getTextureRect().left/mSprite.getTextureRect().width )%12;	
	
	mSprite.setTextureRect(sf::IntRect(16*(++i),0,16,16));
		

}


void Pacman::move()
{
	if (currentTime.getElapsedTime().asMilliseconds() < speedTimer)
		return;
	if (checkDestination(mDirection) == 0)
		return;

	switch (mDirection)
	{
	case UP:
	{
		mCurrentRow--;
		mSprite.move(0, -mSize);
		break;
	}
	case DOWN:
	{
		mCurrentRow++;
		mSprite.move(0, mSize);
		break; }
	case LEFT:
	{
		if (checkDestination(mDirection) == 2) //portal
		{
			mCurrentColumn = mBoard->getBoard()[mCurrentRow].size() - 1;
			updateShape();

		}
		else {
			mCurrentColumn--;
			mSprite.move(-mSize, 0);
		}
		break;
	}
	case RIGHT: {
		if (checkDestination(mDirection) == 2)//portal
		{
			mCurrentColumn = 0;
			updateShape();
		}
		else {
			mCurrentColumn++;
			mSprite.move(mSize, 0);
		}
		break;
	}
	default: return;
	}
	animateMove();
	currentTime.restart();
}