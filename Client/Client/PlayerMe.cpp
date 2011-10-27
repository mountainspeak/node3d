#include "PlayerMe.h"
#include "MainRoot.h"
#include "protocol.h"
#include "World.h"
#include "Audio.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CPlayerMe::CPlayerMe()
{
	m_uAttackTarget = 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CPlayerMe::~CPlayerMe()
{
	// ----
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CPlayerMe::walk(int x, int y)
{
	// ----
	// Clear attack target.
	// ----
	m_uAttackTarget		= 0;
	// ----
	CRole::setTargetCellPos(x, y);
	// ----
	CSMove(m_posCell.x, m_posCell.y, m_Path, m_uTargetDir);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CPlayerMe::playWalkSound()
{
	if(this->getID() == CPlayerMe::getInstance().getID())
	{
		if (CWorld::getInstance().getSceneData())
		{
			unsigned char uTileID = CWorld::getInstance().getSceneData()->getCellTileID(m_posCell.x, m_posCell.y ,0);
			// ----
			if (uTileID==0)
			{
				GetAudio().playSound("Data\\Sound\\pWalk(Grass).wav");
			}
			else
			{
				GetAudio().playSound("Data\\Sound\\pWalk(Soil).wav");
			}
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CPlayerMe::frameMoveRole(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	// ----
	// # If has the attack target, player should walk around the target, then attack the target.
	// ----
	if(m_uAttackTarget > 0)
	{
		CRole * pRole = CWorld::getInstance().getRole(m_uAttackTarget);
		// ----
		if(pRole == NULL)
		{
			m_uAttackTarget = 0;
		}
		else
		{
			float fDistance = (pRole->getPos() - getPos()).length();
			// ----
			if(m_uActionState == STAND && fDistance >= 2.0f)
			{
				// ----
				m_uTargetDir = CWorld::getInstance().getPath(m_posCell.x, m_posCell.y, pRole->getCellPosX(), pRole->getCellPosY(), m_Path);
				// ----
				if(m_Path.size() > 1)
				{
					m_Path.pop_back();
					// ----
					CSMove(m_posCell.x, m_posCell.y, m_Path,m_uTargetDir);
				}
			}
			else if(m_uActionState == STAND)
			{
				unsigned uTargetDir = GetDir(getPos(),pRole->getPos());
				// ----
				CSAttack(0x78, m_uAttackTarget, uTargetDir);
				// ----
				setDir(uTargetDir);
				// ----
				setActionState(HIT1);
				// ----
				m_uAttackTarget = 0;
			}
		}
	}
	// ----
	CRole::frameMoveRole(mWorld, fTime, fElapsedTime);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------