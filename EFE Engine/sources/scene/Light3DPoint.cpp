#include "scene/Light3DPoint.h"

#include "graphics/LowLevelGraphics.h"
#include "graphics/Renderer3D.h"
#include "scene/Camera3D.h"
#include "math/Math.h"

#include "scene/World3D.h"
#include "scene/Scene.h"
#include "game/Game.h"

#include "scene/SectorVisibility.h"
#include "scene/PortalContainer.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cLight3DPoint::cLight3DPoint(tString a_sName, cResources *a_pResources) : iLight3D(a_sName, a_pResources)
	{
		m_LightType = eLight3DType_Point;

		UpdateBoundingVolume();
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	cSectorVisibilityContainer *cLight3DPoint::CreateSectorVisibility()
	{
		return m_pWorld3D->GetPortalContainer()->CreateVisibilityFromBV(GetBoundingVolume());
	}

	void cLight3DPoint::UpdateBoundingVolume()
	{
		m_BoundingVolume.SetSize(m_fFarAttenuation*2);
		m_BoundingVolume.SetPosition(GetWorldPosition());
	}

	bool cLight3DPoint::CreateClipRect(cRectl &a_ClipRect, cRenderSettings *a_pRenderSettings, iLowLevelGraphics *a_pLowLevelGraphics)
	{
		return true;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// CREATE OBJECT STUFF
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	//kBeginSerialize(cSaveData_cLight3DPoint,csavedata
}