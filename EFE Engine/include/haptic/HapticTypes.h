#ifndef EFE_HAPTIC_TYPES_H
#define EFE_HAPTIC_TYPES_H

#include <vector>
#include <list>

namespace efe
{
	enum eHapticShapeType
	{
		eHapticShapeType_Box,
		eHapticShapeType_Sphere,
		eHapticShapeType_Cylinder,
		eHapticShapeType_Capsule,
		eHapticShapeType_Mesh,
		eHapticShapeType_Compound,
		eHapticShapeType_LastEnum
	};

	enum eHapticForceType
	{
		eHapticForceType_Impulse,
		eHapticForceType_Spring,
		eHapticForceType_SineWave,
		eHapticForceType_SawWave,
		eHapticForceType_Viscosity,
		eHapticForceType_LastEnum
	};

	enum eHapticSurfaceType
	{
		eHapticSurfaceType_Simple,
		eHapticSurfaceType_Rough,
		eHapticSurfaceType_Frictional,
		eHapticSurfaceType_Sticky,
		eHapticSurfaceType_LastEnum
	};

	class iHapticShape;
	class iHapticForce;
	class iHapticSurface;

	typedef std::vector<iHapticShape*> tHapticShapeVec;
	typedef tHapticShapeVec::iterator tHapticShapeVecIt;

	typedef std::list<iHapticShape*> tHapticShapeList;
	typedef tHapticShapeList::iterator tHapticShapeListIt;

	typedef std::list<iHapticForce*> tHapticForceList;
	typedef tHapticForceList::iterator tHapticForceListIt;

	typedef std::list<iHapticSurface*> tHapticSurfaceList;
	typedef tHapticSurfaceList::iterator tHapticSurfaceListIt;

	typedef std::map<tString, iHapticSurface*> tHapticSurfaceMap;
	typedef tHapticSurfaceMap::iterator tHapticSurfaceMapIt;

	typedef cSTLIterator<iHapticShape*, tHapticShapeList, tHapticShapeListIt> cHapticShapeIterator;
};

#endif