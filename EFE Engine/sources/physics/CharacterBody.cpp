#include "physics/CharacterBody.h"

#include "physics/CollideShape.h"
#include "physics/PhysicsWorld.h"
#include "physics/PhysicsBody.h"
#include "system/LowLevelSystem.h"
#include "scene/Camera3D.h"
#include "math/Math.h"

#include "game/Game.h"
#include "scene/Scene.h"
#include "scene/World3D.h"
#include "scene/PortalContainer.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// ENGINE MAIN TYPES
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iCharacterBody::iCharacterBody(const tString &a_sName, iPhysicsWorld *a_pWorld, const cVector3f a_vSize)
	{
		m_sName = a_sName;
		
		m_pWorld = a_pWorld;

		m_vSize = a_vSize;

		m_bActive = true;

		m_bCollideCharacter = true;

		m_bTestCollision = true;

		float fRadius = cMath::Max(a_vSize.x, a_vSize.z)*0.5f;
		cMatrixf mtxOffset = cMath::MatrixRotateZ(kPi2f);
		iCollideShape *pCollider = NULL;

		if (fabs(fRadius*2.0f - a_vSize.y)< 0.01)
			pCollider = m_pWorld->CreateSphereShape(fRadius, NULL);
		else
			pCollider = m_pWorld->CreateCylinderShape(fRadius, a_vSize.y, &mtxOffset);

		m_pBody = m_pWorld->CreateBody(a_sName, pCollider);
		m_pBody->SetMass(0);
		m_pBody->SetGravity(false);
		m_pBody->SetIsCharacter(true);
		m_pBody->SetCharacterBody(this);
	
		m_vExtraBodies.push_back(m_pBody);

		m_fYaw = 0;
		m_fPitch = 0;

		for (int i=0; i<eCharDir_LastEnum; i++)
		{
			m_fMaxPosMoveSpeed[i] = 10;
			m_fMaxNegMoveSpeed[i] = -10;
			m_fMoveSpeed[i] = 0;
			m_fMoveAcc[i] = 20;
			m_fMoveDeacc[i] = 20;
			m_bMoving[i] = false;
		}

		m_vForce = cVector3f(0,0,0);
		m_vVelocity = cVector3f(0,0,0);

		m_bGravityActive = true;

		m_fMaxGravitySpeed = 30.0f;

		m_bEnableNearbyBodies = false;

		m_pCamera = NULL;
		m_vCameraPosAdd = cVector3f(0,0,0);

		m_pEntity = NULL;
		m_mtxEntityOffset = cMatrixf::Identity;
		m_mtxEntityPostOffset = cMatrixf::Identity;

		m_pUserData = NULL;

		m_lCameraSmoothPosNum = 0;
		m_lEntitySmoothPosNum = 0;

		m_fMaxStepHeight = m_vSize.y*0.2f;
		m_fStepClimbSpeed = 1.0f;
		m_fClimbForwardMul = 1.0f;
		m_fClimbHeightAdd = 0.01f;
		m_bClimbing = false;
		m_bAccurateClimbing = false;

		m_fCheckStepClimbCount = 0;
		m_fCheckStepClimbInterval = 1/20.0f;

		m_fGroundFriction = 0.1f;
		m_fAirFriction = 0.01f;

		m_pRayCallback = efeNew(cCharacterBodyRay, ());
		m_pCallbackCollideGravity = efeNew(cCharacterBodyCollideGravity, ());
		m_pCallbackCollidePush = efeNew(cCharacterBodyCollidePush, ());

		m_pCallbackCollideGravity->m_pCharBody = this;
		m_pCallbackCollidePush->m_pCharBody = this;

		m_fMass = 1;

		m_fMaxPushMass = 0;
		m_fPushForce = 0;
		m_bPushIn2D = true;

		m_pCallback = NULL;

		m_pAttachedBody = NULL;
		m_bAttachmentJustAdded = true;

		m_bCustomGravity = false;
		m_vCustomGravity = cVector3f(0, 9.8f, 0);
	}

	//--------------------------------------------------------------

	iCharacterBody::~iCharacterBody()
	{
		for (size_t i=0; i<m_vExtraBodies.size(); i++)
			m_pWorld->DestroyBody(m_vExtraBodies[i]);

		efeDelete(m_pRayCallback);
		efeDelete(m_pCallbackCollideGravity);
		efeDelete(m_pCallbackCollidePush);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// RAY CALLBACK
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cCharacterBodyRay::cCharacterBodyRay()
	{
	}

	//--------------------------------------------------------------

	void cCharacterBodyRay::Clear()
	{
		m_fMinDist = 10000.0f;
		m_bCollide = false;
	}

	//--------------------------------------------------------------

	bool cCharacterBodyRay::OnIntersect(iPhysicsBody *a_pBody, cPhysicsRayParams *a_pParams)
	{
		if (a_pBody->IsCharacter()==false && a_pBody->GetCollideCharacter() &&
			a_pParams->m_fDist < m_fMinDist)
		{
			m_fMinDist = a_pParams->m_fDist;
			m_bCollide = true;
		}

		return true;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// COLLIDE GRAVITY CALLBACK
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cCharacterBodyCollideGravity::cCharacterBodyCollideGravity()
	{
		m_pCharBody = NULL;
	}

	//--------------------------------------------------------------

	void cCharacterBodyCollideGravity::OnCollision(iPhysicsBody *a_pBody, cCollideData *a_pCollideData)
	{
		if (a_pBody->GetCanAttachCharacter())
		{
			if (m_pCharBody->GetAttachedBody()==NULL)
				m_pCharBody->SetAttachedBody(a_pBody);
		}

		if (a_pBody->GetMass()==0 || a_pBody->GetPushedByCharacterGravity()==false) return;

		bool bPushDown = false;
		cVector3f vPoint(0,0,0);
		float fNumPoints = 0;

		for (int i=0; i<a_pCollideData->m_lNumOfPoints; i++)
		{
			cCollidePoint &point = a_pCollideData->m_vContactPoints[i];
			if (point.m_vNormal.y > 0.001f)
			{
				bPushDown = true;
				fNumPoints += 1;
				vPoint += point.m_vPoint;
			}
		}

		if (bPushDown)
		{
			float fForceAdd = 0;

			vPoint = vPoint / fNumPoints;
			a_pBody->AddForceAtPosition(cVector3f(0,m_pCharBody->GetMass() * -9.8f + fForceAdd,0), vPoint);
		}

		if (m_pCharBody->m_pCallback)
			m_pCharBody->m_pCallback->OnGravityCollide(m_pCharBody, a_pBody, a_pCollideData);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// COLLIDE PUSH CALLBACK
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cCharacterBodyCollidePush::cCharacterBodyCollidePush()
	{
		m_pCharBody = NULL;
	}

	//--------------------------------------------------------------

	void cCharacterBodyCollidePush::OnCollision(iPhysicsBody *a_pBody, cCollideData *a_pCollideData)
	{
		if (m_pCharBody->GetMoveSpeed(eCharDir_Forward)==0 &&
			m_pCharBody->GetMoveSpeed(eCharDir_Right)==0) return;

		if (a_pBody->GetMass()==0 || a_pBody->GetMass() > m_pCharBody->GetMaxPushMass()) return;

		bool bPush = false;
		cVector3f vPoint(0,0,0);
		float fNumPoints = 0;

		for (int i=0; i<a_pCollideData->m_lNumOfPoints; i++)
		{
			cCollidePoint &point = a_pCollideData->m_vContactPoints[i];

			bPush = true;
			fNumPoints+=1;
			vPoint += point.m_vPoint;
		}

		if (bPush)
		{
			vPoint = vPoint / fNumPoints;

			if (m_pCharBody->GetPushIn2D())
			{
				cVector3f vDir = a_pBody->GetWorldPosition() - m_pCharBody->GetPosition();
				vDir.y = 0; vDir.Normalize();

				a_pBody->AddForceAtPosition(vDir * m_pCharBody->GetPushForce(), vPoint);
			}
			else
			{
				cVector3f vDir = cMath::Vector3Normalize(a_pBody->GetWorldPosition() - m_pCharBody->GetPosition());

				a_pBody->AddForceAtPosition(vDir * m_pCharBody->GetPushForce(), vPoint);
			}
		}
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	float iCharacterBody::GetMass()
	{
		return m_fMass;
	}

	void iCharacterBody::SetMass(float a_fMass)
	{
		m_fMass = a_fMass;
	}

	//--------------------------------------------------------------

	void iCharacterBody::SetActive(bool a_bX)
	{
		if (m_bActive == a_bX) return;

		m_bActive = a_bX;

		for (size_t i=0; i<m_vExtraBodies.size(); ++i)
		{
			if (m_vExtraBodies[i])
				m_vExtraBodies[i]->SetActive(m_bActive);
		}
	}

	//--------------------------------------------------------------

	void iCharacterBody::SetCollideCharacter(bool a_bX)
	{
		if (m_bCollideCharacter == a_bX) return;

		m_bCollideCharacter = a_bX;

		for (size_t i=0; i<m_vExtraBodies.size(); ++i)
		{
			if (m_vExtraBodies[i])
				m_vExtraBodies[i]->SetCollideCharacter(m_bCollideCharacter);
		}
	}

	//--------------------------------------------------------------

	cVector3f iCharacterBody::GetSize()
	{
		return m_vSize;
	}

	//--------------------------------------------------------------

	void iCharacterBody::SetMaxPositiveMoveSpeed(eCharDir a_Dir, float a_fX)
	{
		m_fMaxPosMoveSpeed[a_Dir] = a_fX;
	}
	float iCharacterBody::GetMaxPositiveMoveSpeed(eCharDir a_Dir)
	{
		return m_fMaxPosMoveSpeed[a_Dir];
	}
	void iCharacterBody::SetMaxNegativeMoveSpeed(eCharDir a_Dir, float a_fX)
	{
		m_fMaxNegMoveSpeed[a_Dir] = a_fX;
	}
	float iCharacterBody::GetMaxNegativeSpeed(eCharDir a_Dir)
	{
		return m_fMaxNegMoveSpeed[a_Dir];
	}

	//--------------------------------------------------------------

	void iCharacterBody::SetMoveSpeed(eCharDir a_Dir, float a_fX)
	{
		m_fMoveSpeed[a_Dir] = a_fX;
	}
	float iCharacterBody::GetMoveSpeed(eCharDir a_Dir)
	{
		return m_fMoveSpeed[a_Dir];
	}

	void iCharacterBody::SetMoveAcc(eCharDir a_Dir, float a_fX)
	{
		m_fMoveAcc[a_Dir] = a_fX;
	}
	float iCharacterBody::GetMoveAcc(eCharDir a_Dir)
	{
		return m_fMoveAcc[a_Dir];
	}

	void iCharacterBody::SetMoveDeacc(eCharDir a_Dir, float a_fX)
	{
		m_fMoveDeacc[a_Dir] = a_fX;
	}
	float iCharacterBody::GetMoveDeacc(eCharDir a_Dir)
	{
		return m_fMoveDeacc[a_Dir];
	}

	//--------------------------------------------------------------

	cVector3f iCharacterBody::GetVelocity(float a_fFrameTime)
	{
		if (a_fFrameTime <= 0) return 0;

		return (m_vPosition - m_vLastPosition) / a_fFrameTime;
	}

	//--------------------------------------------------------------

	void iCharacterBody::SetPosition(const cVector3f &a_vPos, bool a_bSmooth)
	{
		m_vForce = 0;
		m_vVelocity = 0;
		m_vLastPosition = a_vPos;
		m_vPosition = a_vPos;
		m_pBody->SetPosition(a_vPos);

		if (!a_bSmooth)m_lstCameraPos.clear();
	}
	const cVector3f &iCharacterBody::GetPosition()
	{
		return m_vPosition;
	}
	const cVector3f &iCharacterBody::GetLastPosition()
	{
		return m_vLastPosition;
	}

	//--------------------------------------------------------------

	void iCharacterBody::SetFeetPosition(const cVector3f &a_vPos, bool a_bSmooth)
	{
		SetPosition(a_vPos + cVector3f(0, m_pBody->GetShape()->GetSize().y/2.0, 0), a_bSmooth);
	}

	const cVector3f &iCharacterBody::GetFeetPosition()
	{
		return m_vPosition - cVector3f(0, m_pBody->GetShape()->GetSize().y/2, 0);
	}

	//--------------------------------------------------------------

	void iCharacterBody::SetYaw(float a_fX)
	{
		m_fYaw = a_fX;
	}

	void iCharacterBody::AddYaw(float a_fX)
	{
		m_fYaw += a_fX;
	}

	float iCharacterBody::GetYaw()
	{
		return m_fYaw;
	}

	void iCharacterBody::SetPitch(float a_fX)
	{
		m_fPitch = a_fX;
	}

	void iCharacterBody::AddPitch(float a_fX)
	{
		m_fPitch += a_fX;
	}

	float iCharacterBody::GetPitch()
	{
		return m_fPitch;
	}

	//--------------------------------------------------------------

	cVector3f iCharacterBody::GetForward()
	{
		return m_mtxMove.GetForward()*-1.0f;
	}
	cVector3f iCharacterBody::GetRight()
	{
		return m_mtxMove.GetRight();
	}
	cVector3f iCharacterBody::GetUp()
	{
		return m_mtxMove.GetUp();
	}

	//--------------------------------------------------------------

	cMatrixf &iCharacterBody::GetMoveMatrix()
	{
		return m_mtxMove;
	}

	//--------------------------------------------------------------

	void iCharacterBody::SetGravityActive(bool a_bX)
	{
		m_bGravityActive = a_bX;
	}

	bool iCharacterBody::GravityIsActive()
	{
		return m_bGravityActive;
	}

	void iCharacterBody::SetMaxGravitySpeed(float a_fX)
	{
		m_fMaxGravitySpeed = a_fX;
	}
	float iCharacterBody::GetMaxGravitySpeed()
	{
		return m_fMaxGravitySpeed;
	}

	//--------------------------------------------------------------

	bool iCharacterBody::GetCustomGravityActive()
	{
		return m_bCustomGravity;
	}

	void iCharacterBody::SetCustomGravityActive(bool a_bX)
	{
		m_bCustomGravity = a_bX;
	}

	void iCharacterBody::SetCustomGravity(const cVector3f &a_vCustomGravity)
	{
		m_vCustomGravity = a_vCustomGravity;
	}

	cVector3f iCharacterBody::GetCustomGravity()
	{
		return m_vCustomGravity;
	}

	//--------------------------------------------------------------

	int iCharacterBody::AddExtraSize(const cVector3f &a_vSize)
	{
		float fRadius = cMath::Max(a_vSize.x, a_vSize.z)*0.5f;
		cMatrixf mtxOffset = cMath::MatrixRotateZ(kPi2f);
		iCollideShape *pCollider = m_pWorld->CreateCapsuleShape(fRadius, a_vSize.y, &mtxOffset);

		iPhysicsBody *pBody = m_pWorld->CreateBody(m_sName, pCollider);
		pBody->SetMass(0);
		pBody->SetGravity(false);
		pBody->SetIsCharacter(true);
		pBody->SetActive(false);
		pBody->SetCharacterBody(this);

		m_vExtraBodies.push_back(pBody);

		return (int)m_vExtraBodies.size() - 1;
	}

	//--------------------------------------------------------------

	void iCharacterBody::SetActiveSize(int a_lNum)
	{
		m_pBody->SetActive(false);
		m_pBody = m_vExtraBodies[a_lNum];
		m_pBody->SetActive(true);

		m_vSize.y = m_pBody->GetShape()->GetHeight();
		m_vSize.x = m_pBody->GetShape()->GetRadius() * 2;
		m_vSize.z = m_pBody->GetShape()->GetRadius() * 2;
	}

	//--------------------------------------------------------------

	void iCharacterBody::SetForce(const cVector3f &a_vForce)
	{
		m_vForce = a_vForce;
	}

	void iCharacterBody::AddForce(const cVector3f &a_vForce)
	{
		m_vForce += a_vForce;
	}

	cVector3f iCharacterBody::GetForce()
	{
		return m_vForce;
	}

	//--------------------------------------------------------------

	void iCharacterBody::Move(eCharDir a_Dir, float a_fMul, float a_fTimeStep)
	{
		m_fMoveSpeed[a_Dir] += m_fMoveAcc[a_Dir]*a_fMul*a_fTimeStep;

		m_bMoving[a_Dir] = true;

		m_fMoveSpeed[a_Dir] = cMath::Clamp(m_fMoveSpeed[a_Dir],m_fMaxNegMoveSpeed[a_Dir],m_fMaxPosMoveSpeed[a_Dir]);
	}

	//--------------------------------------------------------------

	void iCharacterBody::Update(float a_fTimeStep)
	{
		if (m_bActive==false) return;

		iPhysicsBody *pLastAttached = m_pAttachedBody;
		SetAttachedBody(NULL);

		//Update the move matrix
		UpdateMoveMatrix();

		m_vLastPosition = m_vPosition;

		//Ground and air friction
		float fFriction = m_bOnGround ? m_fGroundFriction : m_fAirFriction;

		cVector3f vVelXZ(m_vVelocity.x, 0, m_vVelocity.z);
		float fSpeed = vVelXZ.Length();
		vVelXZ.Normalize();

		fSpeed -= fFriction * a_fTimeStep;
		if (fSpeed<0) fSpeed=0;

		m_vVelocity.x = vVelXZ.x * fSpeed;
		m_vVelocity.z = vVelXZ.z * fSpeed;

		for (int i=0; i<eCharDir_LastEnum; i++)
		{
			if (m_bMoving[i]==false)
			{
				if (m_fMoveSpeed[i]>0)
				{
					m_fMoveSpeed[i] -= m_fMoveDeacc[i] * a_fTimeStep;
					if (m_fMoveSpeed[i]<0) m_fMoveSpeed[i] = 0;
				}
				else
				{
					m_fMoveSpeed[i] += m_fMoveDeacc[i] * a_fTimeStep;
					if (m_fMoveSpeed[i]>0) m_fMoveSpeed[i] = 0;
				}
			}
		else
			m_bMoving[i] = false;
		}

		if (m_bClimbing)	m_fCheckStepClimbCount = 0;
		else				m_fCheckStepClimbCount -= a_fTimeStep;

		m_bClimbing = false;

		cVector3f vPosAdd(0,0,0);

		cVector3f vForward = GetMoveMatrix().GetForward() * -1.0f;
		cVector3f vRight = GetMoveMatrix().GetRight();

		vPosAdd += vForward * m_fMoveSpeed[eCharDir_Forward] * a_fTimeStep;
		vPosAdd += vRight * m_fMoveSpeed[eCharDir_Right] * a_fTimeStep;

		float fMaxStep =	m_fMoveSpeed[eCharDir_Forward] >= 0 ?
							m_fMaxPosMoveSpeed[eCharDir_Forward] :
							m_fMaxNegMoveSpeed[eCharDir_Forward];
		fMaxStep *= a_fTimeStep;
		fMaxStep = cMath::Abs(fMaxStep);

		float fStepLength = vPosAdd.Length();
		if (fStepLength > fMaxStep)
		{
			vPosAdd = vPosAdd / fStepLength;
			vPosAdd = vPosAdd * fMaxStep;
		}

		m_vPosition += vPosAdd;

		//Set the postition of the body
		m_pBody->SetPosition(m_vPosition);

		//Enable objects around the character
		if (m_vLastPosition != m_vPosition)
		{
			cBoundingVolume largeBV = *m_pBody->GetBV();
			largeBV.SetSize(largeBV.GetSize() * 1.02f);

			m_pWorld->EnableBodiesInBV(&largeBV, true);
		}

		if (m_bTestCollision == false)
		{
			UpdateCamera();
			UpdateEntity();

			m_vVelocity = 0;

			return;
		}

		cVector3f vNewPos;
		if (m_vLastPosition.x != m_vPosition.x || m_vLastPosition.z != m_vPosition.z)
		{
			m_pWorld->CheckShapeWorldCollision(&vNewPos, m_pBody->GetShape(), cMath::MatrixTranslate(m_vPosition), m_pBody,
				false, true, m_pCallbackCollidePush, m_bCollideCharacter);
		}
		else
		{
			vNewPos = m_vPosition;
		}

		if (m_vPosition.x != vNewPos.x || m_vPosition.z != vNewPos.z ||
			(m_bGravityActive == false && m_vPosition.y != vNewPos.y))
		{
			m_vPosition = vNewPos;

			if (m_fCheckStepClimbCount <= 0)
			{
				//Send a ray in front of the player
				cVector3f vShapeSize = m_pBody->GetShape()->GetSize();
				float fRadius = m_pBody->GetShape()->GetRadius();
				float fForwardAdd = vPosAdd.Length();

				//The direction of the movement
				cVector3f vMoveDir = cMath::Vector3Normalize(vPosAdd);

				cVector3f vStepAdd[3];
				cVector3f vStart[3];
				cVector3f vEnd[3];
				bool bCollided[3];
				float fMinDist[3];
				int lNumRays = 1;
				if (m_bAccurateClimbing) lNumRays = 3;

				vStepAdd[0] = vMoveDir*(fRadius+fForwardAdd);

				if (m_bAccurateClimbing)
				{
					cVector3f vRightDir = cMath::MatrixMul(cMath::MatrixRotateY(kPi4f), vMoveDir);
					vStepAdd[1] = (vRightDir*fRadius)+(vMoveDir*fForwardAdd);

					cVector3f vLeftDir = cMath::MatrixMul(cMath::MatrixRotateY(-kPi4f), vMoveDir);
					vStepAdd[2] = (vLeftDir*fRadius)+(vMoveDir*fForwardAdd);
				}

				//Shot the rays
				for (int i=0; i<lNumRays; ++i)
				{
					vStart[i] = m_vPosition + cVector3f(0,vShapeSize.y/2,0) + vStepAdd[i];
					vEnd[i] = vStart[i] - cVector3f(0,vShapeSize.y,0);

					m_pRayCallback->Clear();
					m_pWorld->CastRay(m_pRayCallback, vStart[i], vEnd[i], true, false, false);
					bCollided[i] = m_pRayCallback->m_bCollide;
					fMinDist[i] = m_pRayCallback->m_fMinDist;
				}

				//Check if step can be climbed
				for (int i=0; i<lNumRays; ++i)
				{
					if (bCollided[i] == false) continue;

					float fHeight = vShapeSize.y - fMinDist[i];

					if (fHeight <= m_fMaxStepHeight)
					{
						cVector3f vStepPos = m_vPosition + cVector3f(0,fHeight+m_fClimbHeightAdd,0)+
							(vMoveDir*fForwardAdd*m_fClimbForwardMul);

						m_pWorld->CheckShapeWorldCollision(&vNewPos, m_pBody->GetShape(),
							cMath::MatrixTranslate(vStepPos), m_pBody,
							false, true, NULL, m_bCollideCharacter);

						if (vNewPos == vStepPos)
						{
							m_vPosition.y += m_fStepClimbSpeed * a_fTimeStep;
							m_bClimbing = true;
							break;
						}
					}
				}

				m_fCheckStepClimbCount = m_fCheckStepClimbInterval;
			}
		}

		if (m_bGravityActive == false)
		{
			m_pBody->SetPosition(m_vPosition);

			UpdateCamera();
			UpdateEntity();

			m_vVelocity = 0;

			return;
		}

		cVector3f vBeforeForcePos = m_vPosition;

		m_vVelocity += m_vForce * (a_fTimeStep * (1.0f / m_fMass));
		if (m_bGravityActive && m_bClimbing == false)
		{
			if (m_bCustomGravity)
				m_vVelocity += m_vCustomGravity * a_fTimeStep;
			else
				m_vVelocity += m_pWorld->GetGravity() * a_fTimeStep;

			float fLength = m_vVelocity.Length();
			if (fLength > m_fMaxGravitySpeed)
			{
				m_vVelocity = (m_vVelocity / fLength) * m_fMaxGravitySpeed;
			}
		}

		m_vForce = cVector3f(0,0,0);

		cVector3f vLastVelocity(0,0,0);

		if (m_vVelocity.x != 0 || m_vVelocity.z != 0)
		{
			m_vPosition += cVector3f(m_vVelocity.x, 0, m_vVelocity.z) * a_fTimeStep;

			vNewPos = m_vPosition;
			bool bCollide = m_pWorld->CheckShapeWorldCollision(&vNewPos, m_pBody->GetShape(), cMath::MatrixTranslate(m_vPosition),
				m_pBody, false, true, NULL, m_bCollideCharacter);

			vLastVelocity.x = m_vVelocity.x;
			vLastVelocity.z = m_vVelocity.z;

			m_vVelocity.x = (vNewPos.x - vBeforeForcePos.x) * (1.0f / a_fTimeStep);
			m_vVelocity.z = (vNewPos.z - vBeforeForcePos.z) * (1.0f / a_fTimeStep);

			m_vPosition = vNewPos;
		}

		bool bCollide = false;

		cVector3f vPosBeforeGrav = m_vPosition;
		m_vPosition.y += m_vVelocity.y * a_fTimeStep;

		vNewPos = m_vPosition;
		bCollide = m_pWorld->CheckShapeWorldCollision(&vNewPos, m_pBody->GetShape(), cMath::MatrixTranslate(m_vPosition),
			m_pBody, false, true, m_pCallbackCollideGravity, m_bCollideCharacter);

		//Set new velocity depending on collisions
		vLastVelocity.y = m_vVelocity.y;

		if (m_bClimbing)
		{
			if (m_vVelocity.y < 0)
				m_vVelocity.y = 0;
		}
		else
		{
			m_vVelocity.y = (vNewPos.y - vPosBeforeGrav.y) * (1.0/a_fTimeStep);

			if (vLastVelocity.y < 0 && m_vVelocity.y > 0) m_vVelocity.y = 0;
			if (vLastVelocity.y > 0 && m_vVelocity.y < 0) m_vVelocity.y = 0;
		}

		m_vPosition = vNewPos;

		//Determine if character is on the ground
		if (m_bClimbing)
		{
			m_bOnGround = true;
		}
		else
		{
			if (bCollide && vLastVelocity.y <= 0 && std::abs(m_vVelocity.y) < (std::abs(vLastVelocity.y)-0.001f))
			{
				if (m_bOnGround == false)
				{
					if (m_pCallback) m_pCallback->OnHitGround(this, vLastVelocity);
				}

				m_bOnGround = true;
			}
			else
			{
				m_bOnGround = false;
			}
		}

		if (m_bOnGround && m_vVelocity.y < 0 && m_vVelocity.y > -0.15)
			m_vPosition = vPosBeforeGrav;

		UpdateCamera();
		UpdateEntity();

		if (pLastAttached != m_pAttachedBody) m_bAttachmentJustAdded = true;
		UpdateAttachment();

		//Enable close by objects
		if (m_bEnableNearbyBodies)
		{
			cWorld3D *pWorld = m_pWorld->GetWorld3D();
			cPortalContainerEntityIterator bodyIt = pWorld->GetPortalContainer()->GetEntityIterator(m_pBody->GetBoundingVolume());
			
			cBoundingVolume bv = *m_pBody->GetBV();
			bv.SetLocalMinMax(bv.GetLocalMin() - cVector3f(0.03f,0.03f,0.03f),bv.GetLocalMax() + cVector3f(0.03f,0.03f,0.03f));
			float fHeadY = m_pBody->GetWorldPosition().y + m_pBody->GetShape()->GetSize().y/2 - 0.01f;

			while (bodyIt.HasNext())
			{
				iPhysicsBody *pBody = static_cast<iPhysicsBody*>(bodyIt.Next());

				if (pBody->IsActive() &&
					fHeadY <= pBody->GetLocalPosition().y &&
					pBody->GetEnabled()==false &&
					cMath::CheckCollisionBV(*pBody->GetBV(), bv))
				{
					pBody->SetEnabled(true);
				}
			}

		}
	}

	//--------------------------------------------------------------

	void iCharacterBody::SetCamera(cCamera3D *a_Cam)
	{
		m_pCamera = a_Cam;
	}

	cCamera3D *iCharacterBody::GetCamera()
	{
		return m_pCamera;
	}

	void iCharacterBody::SetCameraPosAdd(const cVector3f &a_vAdd)
	{
		m_vCameraPosAdd = a_vAdd;
	}

	cVector3f iCharacterBody::GetCameraPosAdd()
	{
		return m_vCameraPosAdd;
	}

	//--------------------------------------------------------------

	iCollideShape *iCharacterBody::GetShape()
	{
		return m_pBody->GetShape();
	}

	//--------------------------------------------------------------

	void iCharacterBody::SetAttachedBody(iPhysicsBody *a_pBody)
	{
		if (a_pBody == m_pAttachedBody) return;

		if (m_pAttachedBody) m_pAttachedBody->RemoveAttachedCharacter(this);

		m_pAttachedBody = a_pBody;
		if (m_pAttachedBody) m_pAttachedBody->AddAttachedCharacter(this);
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void iCharacterBody::UpdateMoveMatrix()
	{
		m_mtxMove = cMath::MatrixRotateY(-m_fYaw);
		m_mtxMove = cMath::MatrixMul(cMath::MatrixRotateX(-m_fPitch),m_mtxMove);
		m_mtxMove.SetTranslation(m_vPosition * -1);
	}

	//--------------------------------------------------------------

	void iCharacterBody::UpdateCamera()
	{
		if (m_pCamera == NULL) return;

		if (m_lCameraSmoothPosNum <= 0)
		{
			cVector3f vPos = (m_vPosition - cVector3f(0,m_pBody->GetShape()->GetSize().y/2.0f,0)) +
				cVector3f(0,m_vSize.y,0);
			m_pCamera->SetPosition(vPos + m_vCameraPosAdd);
		}
		//Smooth the camera position
		else
		{
			//Add the newest position
			m_lstCameraPos.push_back(m_vPosition);
			if ((int)m_lstCameraPos.size() > m_lCameraSmoothPosNum)
				m_lstCameraPos.erase(m_lstCameraPos.begin());

			float fNum = (float)m_lstCameraPos.size();

			cVector3f vTotalPos(0,0,0);
			tVector3fListIt it = m_lstCameraPos.begin();
			for (; it != m_lstCameraPos.end(); ++it)
				vTotalPos += *it;

			cVector3f vPos = vTotalPos / fNum;
			cVector3f vFirstSize = m_vExtraBodies[0]->GetShape()->GetSize();

			cVector3f vHeadPos = (vPos - cVector3f(0, m_pBody->GetShape()->GetSize().y/2.0f,0)) +
				cVector3f(0,vFirstSize.y,0);

			m_pCamera->SetPosition(vHeadPos + m_vCameraPosAdd);
		}
	}

	//--------------------------------------------------------------

	void iCharacterBody::UpdateEntity()
	{
		if (m_pEntity == NULL) return;

		if (m_lCameraSmoothPosNum <= 0)
		{
			cMatrixf mtxEntity = cMath::MatrixRotateY(m_fYaw);
			mtxEntity.SetTranslation(m_vPosition);

			m_pEntity->SetMatrix(cMath::MatrixMul(mtxEntity, m_mtxEntityOffset));
		}
		else
		{
			m_lstEntityPos.push_back(m_vPosition);

			if ((int)m_lstEntityPos.size() > m_lCameraSmoothPosNum)
				m_lstEntityPos.erase(m_lstEntityPos.begin());

			float fNum = (float)m_lstEntityPos.size();

			cVector3f vTotalPos(0,0,0);
			tVector3fListIt it = m_lstEntityPos.begin();
			for (; it != m_lstEntityPos.end(); ++it)
				vTotalPos += *it;

			cVector3f vPos = vTotalPos / fNum;

			cMatrixf mtxEntity = cMath::MatrixInverse(m_mtxMove);
			mtxEntity.SetTranslation(0);

			mtxEntity = cMath::MatrixMul(m_mtxEntityPostOffset, mtxEntity);
			mtxEntity.SetTranslation(mtxEntity.GetTranslation() + vPos);

			mtxEntity = cMath::MatrixMul(mtxEntity, m_mtxEntityOffset);

			m_pEntity->SetMatrix(mtxEntity);
		}
	}

	//--------------------------------------------------------------

	void iCharacterBody::UpdateAttachment()
	{
		if (m_pAttachedBody==NULL)
		{
			m_bAttachmentJustAdded = true;
			return;
		}

		if (m_bAttachmentJustAdded)
			m_bAttachmentJustAdded = false;
		else
		{
			cVector3f vPosAdd = m_pAttachedBody->GetWorldPosition() - 
				m_matxAttachedPrevMatrix.GetTranslation();

			m_vPosition += vPosAdd;
		}

		m_matxAttachedPrevMatrix = m_pAttachedBody->GetWorldMatrix();
	}
}