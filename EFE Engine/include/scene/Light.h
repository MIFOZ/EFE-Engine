#ifndef EFE_LIGHT_H
#define EFE_LIGHT_H

#include "graphics/GraphicsTypes.h"

namespace efe
{
	class iLowLevelGraphics;
	class cWorld3D;

	class iLight
	{
	public:
		iLight();

		//Fading
		void FadeTo(const cColor &a_Col, float a_fRadius, float a_fTime);
		bool IsFading();
		cColor GetDestColor(){return m_DestCol;}
		float GetDestRadius(){return m_fDestRadius;}

		//Flickering
		void SetFlickerActive(bool a_bX);
		bool GetFlickerActive(){return m_bFlickering;}

		void SetFlicker(const cColor &a_OffCol, float a_fOffRadius,
						float a_fOnMinLength, float a_fOnMaxLength, const tString &a_sOnSound, const tString &a_sOnPS,
						float a_fOffMinLength, float a_fOffMaxLength, const tString &a_sOffSound, const tString &a_sOffPS,
						bool a_bFade, float a_fOnFadeLength, float a_fOffFadeLength);

		tString GetFlickerOffSound(){return m_sFlickerOffSound;}
		tString GetFlickerOnSound(){return m_sFlickerOnSound;}
		tString GetFlickerOffPS(){return m_sFlickerOffPS;}
		tString GetFlickerOnPS(){return m_sFlickerOnPS;}
		float GetFlickerOnMinLength(){return m_fFlickerOnMinLength;}
		float GetFlickerOffMinLength(){return m_fFlickerOffMinLength;}
		float GetFlickerOnMaxLength(){return m_fFlickerOnMaxLength;}
		float GetFlickerOffMaxLength(){return m_fFlickerOffMaxLength;}
		cColor GetFlickerOffColor(){return m_FlickerOffColor;}
		float GetFlickerOffRadius(){return m_fFlickerOffRadius;}
		bool GetFlickerFade(){return m_bFlickerFade;}
		float GetFlickerOnFadeLength(){return m_fFlickerOnFadeLength;}
		float GetFlickerOffFadeLength(){return m_fFlickerOffFadeLength;}

		cColor GetFlickerOnColor(){return m_FlickerOnColor;}
		float GetFlickerOnRadius(){return m_fFlickerOnRadius;}

		//Properties
		const cColor &GetDiffuseColor(){return m_DiffuseColor;}
		void SetDiffuseColor(cColor a_Color);
		const cColor &GetSpecularColor(){return m_SpecularColor;}
		void SetSpecularColor(cColor a_Color);

		float GetIntensity(){return m_fIntensity;}
		void SetIntensity(float a_fX){m_fIntensity = a_fX;}
		bool GetCastShadows(){return m_bCastShadows;}
		void SetCastShadows(bool a_bX){m_bCastShadows = a_bX;}

		bool GetAffectMAterial(){return m_bAffectMaterial;}
		void SetAffectMaterial(bool a_bX){m_bAffectMaterial = a_bX;}

		float GetFarAttenuation(){return m_fFarAttenuation;}
		float GetNearAttenuation(){return m_fNearAttenuation;}

		virtual void SetFarAttenuation(float a_fX)=0;
		virtual void SetNearAttenuation(float a_fX)=0;

		float GetSourceRadius(){return m_fSourceRadius;}
		void SetSourceRadius(float a_fX){m_fSourceRadius = a_fX;}

		virtual cVector3f GetLightPosition()=0;

		void UpdateLight(float a_fTimeStep);

		void SetWorld3D(cWorld3D *a_pWorld){m_pWorld3D = a_pWorld;}
	protected:
		virtual void OnFlickerOff(){}
		virtual void OnFlickerOn(){}
		virtual void OnSetDuffuse(){}

		cColor m_DiffuseColor;
		cColor m_SpecularColor;
		float m_fIntensity;
		float m_fFarAttenuation;
		float m_fNearAttenuation;
		float m_fSourceRadius;

		bool m_bCastShadows;
		bool m_bAffectMaterial;

		cWorld3D *m_pWorld3D;

		//Fading
		cColor m_ColAdd;
		float m_fRadiusAdd;
		cColor m_DestCol;
		float m_fDestRadius;
		float m_fFadeTime;

		//Flicker
		bool m_bFlickering;
		tString m_sFlickerOffSound;
		tString m_sFlickerOnSound;
		tString m_sFlickerOffPS;
		tString m_sFlickerOnPS;
		float m_fFlickerOnMinLength;
		float m_fFlickerOffMinLength;
		float m_fFlickerOnMaxLength;
		float m_fFlickerOffMaxLength;
		cColor m_FlickerOffColor;
		float m_fFlickerOffRadius;
		bool m_bFlickerFade;
		float m_fFlickerOnFadeLength;
		float m_fFlickerOffFadeLength;

		cColor m_FlickerOnColor;
		float m_fFlickerOnRadius;

		bool m_bFlickerOn;
		float m_fFlickerTime;
		float m_fFlickerStateLength;
	};
};
#endif