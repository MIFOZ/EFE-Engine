#ifndef EFE_GRAPHICSTYPES_H
#define EFE_GRAPHICSTYPES_H

#include <list>
#include <vector>
#include "graphics/Color.h"
#include "math/MathTypes.h"

namespace efe
{
#define MAX_MRTS (8)
#define MAX_VERTEXSTREAMS (8)
#define MAX_TEXTUREUNITS (16)
#define MAX_SAMPLERSTATES (16)
#define MAX_BEZIERINTERPOLATIONITERATIONS (20)
#define kApproximationEpsilonf (0.01f)
#define MAX_NUM_OF_LIGHTS (30)

	enum eMatrix
	{
		eMatrix_Model,
		eMatrix_View,
		eMatrix_ModelView,
		eMatrix_Projection,
		eMatrix_Texture,
		eMatrix_LastEnum
	};

	enum eTileRotation
	{
		eTileRotation_0,
		eTileRotation_90,
		eTileRotation_180,
		eTileRotation_270,
		eTileRotation_LastEnum
	};

	enum ePrimitiveType
	{
		ePrimitiveType_Tri,
		ePrimitiveType_Quad,
		ePrimitiveType_LastEnum
	};

	typedef tFlag tAnimTransformFlag;

	#define eAnimTransformFlag_Translate	(0x00000001)
	#define eAnimTransformFlag_Scale		(0x00000002)
	#define eAnimTransformFlag_Rotate		(0x00000004)

	const tAnimTransformFlag k_vAnimTransformFlags[] = {eAnimTransformFlag_Translate,
		eAnimTransformFlag_Scale, eAnimTransformFlag_Rotate};

	enum eFontAlign
	{
		eFontAlign_Left,
		eFontAlign_Right,
		eFontAlign_Center,
		eFontAlign_LastEnum
	};

	enum eKeyFrameType
	{
		eKeyFrameType_Linear,
		eKeyFrameType_Bezier,
		eKeyFrameType_LastEnum
	};

	class cKeyFrame
	{
	public:
		cVector3f trans;
		cVector3f scale;
		cQuaternion rotation;
		eKeyFrameType type;
		cVector2f outAnchor;
		cVector2f inAnchor;
		float time;
	};

	typedef std::vector<cKeyFrame*> tKeyFramePtrVec;
	typedef tKeyFramePtrVec::iterator tKeyFramePtrVecIt;

	typedef std::vector<cKeyFrame> tKeyFrameVec;
	typedef tKeyFrameVec::iterator tKeyFrameVecIt;

	enum eAnimationEventType
	{
		eAnimationEventType_PlaySound,
		eAnimationEventType_LastEnum
	};

	class cVertex
	{
	public:
		cVertex() : pos(0),tex(0),col(0){}
		cVertex(const cVector3f &a_vPos, const cVector3f &a_vTex, const cColor &a_Col)
		{
			pos = a_vPos;
			tex = a_vTex;
			col = a_Col;
		}

		cVertex(const cVector3f &a_vPos, const cColor &a_Col)
		{
			pos = a_vPos;
			col = a_Col;
		}
		cVector3f pos;
		cVector3f tex;
		cVector3f tan;
		cVector3f norm;
		cColor col;
	};

	typedef std::vector<cVertex> tVertexVec;
	typedef tVertexVec::iterator tVertexVecIt;

};
#endif