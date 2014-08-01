#include "scene/Light.h"

#include "system/LowLevelSystem.h"
#include "math/Math.h"

#include "scene/World3D.h"
//#include "scene/SoundEintity.h"

namespace efe
{
	//////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	iLight::iLight() : m_DiffuseColor(0,0,0,0), m_SpecularColor(0,0,0,0), m_fIntensity(1),
		m_bCastShadows(false), m_fFarAttenuation(0), m_fNearAttenuation(0),
		m_fSourceRadius(10), m_bAffectMaterial(true)
	{
		m_fFadeTime = 0;

		m_bFlickering = false;

		m_pWorld3D = NULL;

		m_fFlickerStateLength = 0;

		m_fFadeTime = 0;
	}

	//--------------------------------------------------------------

	//////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////

	//--------------------------------------------------------------

	void iLight::SetDiffuseColor(cColor a_Color)
	{
		m_DiffuseColor = a_Color;

		OnSetDuffuse();
	}

	//--------------------------------------------------------------

	void iLight::UpdateLight(float a_fTimeStep)
	{
		// Fade
		if (m_fFadeTime > 0)
		{
			m_fFarAttenuation += m_fRadiusAdd*a_fTimeStep;
			m_DiffuseColor.r += m_ColAdd.r;
			m_DiffuseColor.g += m_ColAdd.g;
			m_DiffuseColor.b += m_ColAdd.b;
			m_DiffuseColor.a += m_ColAdd.a;
			SetDiffuseColor(m_DiffuseColor);

			m_fFadeTime -= a_fTimeStep;

			if (m_fFadeTime <= 0)
			{
				m_fFadeTime = 0;
				SetDiffuseColor(m_DestCol);
				m_fFarAttenuation = m_fDestRadius;
			}
		}

		// Flickering
		if (m_bFlickering && m_fFadeTime<=0)
		{
			//On
			if (m_bFlickerOn)
			{
				if (m_fFlickerTime >= m_fFlickerStateLength)
				{
					m_bFlickerOn = false;
					if (!m_bFlickerFade)
					{
						SetDiffuseColor(m_FlickerOffColor);
						m_fFarAttenuation = m_fFlickerOffRadius;
					}
					else
						FadeTo(m_FlickerOffColor, m_fFlickerOffRadius, m_fFlickerOffFadeLength);

					if (m_sFlickerOffSound != "")
					{
						
					}

					OnFlickerOff();

					m_fFlickerTime = 0;
					m_fFlickerStateLength = cMath::RandRectf(m_fFlickerOffMinLength, m_fFlickerOffMaxLength);
				}
			}

			//Off
			else
			{
				if (m_fFlickerTime >= m_fFlickerStateLength)
				{
					m_bFlickerOn = true;
					if (!m_bFlickerFade)
					{
						SetDiffuseColor(m_FlickerOnColor);
						m_fFarAttenuation = m_fFlickerOnRadius;
					}
					else
						FadeTo(m_FlickerOnColor, m_fFlickerOnRadius, m_fFlickerOnFadeLength);

					OnFlickerOn();

					m_fFlickerTime = 0;
					m_fFlickerStateLength = cMath::RandRectf(m_fFlickerOnMinLength, m_fFlickerOnMaxLength);
				}
			}

			m_fFlickerTime += a_fTimeStep;
		}
	}

	//--------------------------------------------------------------

	void iLight::FadeTo(const cColor &a_Col, float a_fRadius, float a_fTime)
	{
		if (a_fTime <= 0) a_fTime = 0.0001f;

		m_fFadeTime = a_fTime;

		m_ColAdd.r = (a_Col.r - m_DiffuseColor.r) / a_fTime;
		m_ColAdd.g = (a_Col.g - m_DiffuseColor.g) / a_fTime;
		m_ColAdd.b = (a_Col.b - m_DiffuseColor.b) / a_fTime;
		m_ColAdd.a = (a_Col.a - m_DiffuseColor.a) / a_fTime;

		m_fRadiusAdd = (a_fRadius - m_fFarAttenuation) / a_fTime;

		m_fDestRadius = a_fRadius;
		m_DestCol = a_Col;
	}

	//--------------------------------------------------------------

	bool iLight::IsFading()
	{
		return m_fFadeTime != 0;
	}

	//--------------------------------------------------------------

	void iLight::SetFlickerActive(bool a_bX)
	{
		m_bFlickering = a_bX;
	}

	void iLight::SetFlicker(const cColor &a_OffCol, float a_fOffRadius,
						float a_fOnMinLength, float a_fOnMaxLength, const tString &a_sOnSound, const tString &a_sOnPS,
						float a_fOffMinLength, float a_fOffMaxLength, const tString &a_sOffSound, const tString &a_sOffPS,
						bool a_bFade, float a_fOnFadeLength, float a_fOffFadeLength)
	{
		m_FlickerOffColor = a_OffCol;
		m_fFlickerOffRadius = a_fOffRadius;

		m_fFlickerOnMinLength = a_fOnMinLength;
		m_fFlickerOnMaxLength = a_fOnMaxLength;
		m_sFlickerOnSound = a_sOnSound;
		m_sFlickerOnPS = a_sOnPS;

		m_fFlickerOffMinLength = a_fOffMinLength;
		m_fFlickerOffMaxLength = a_fOffMaxLength;
		m_sFlickerOffSound = a_sOffSound;
		m_sFlickerOffPS = a_sOffPS;

		m_bFlickerFade = a_bFade;

		m_fFlickerOnFadeLength = a_fOnFadeLength;
		m_fFlickerOffFadeLength = a_fOffFadeLength;

		m_FlickerOnColor = m_DiffuseColor;
		m_fFlickerOnRadius = m_fFarAttenuation;

		m_bFlickerOn = true;
		m_fFlickerTime = 0;

		m_fFadeTime = 0;

		m_fFlickerStateLength = cMath::RandRectf(m_fFlickerOnMinLength, m_fFlickerOnMaxLength);
	}
}