#ifndef EfE_PID_CONTROLLER_H
#define EfE_PID_CONTROLLER_H

#include "math/MathTypes.h"

namespace efe
{
	template <class T>
	class cPidController
	{
	public:
		float p, i, d;

		cPidController()
		{
			Reset();
		}
		cPidController(float a_fP, float a_fI, float a_fD, int a_lErrorNum)
		{
			p = a_fP; i = a_fI; d = a_fD;
			SetErrorNum(a_lErrorNum);
			Reset();
		}

		T Output(T a_Error, float a_fTimeStep)
		{
			m_vErrors[m_lErrorNum] = a_Error;
			m_vTimeSteps[m_lErrorNum] = a_fTimeStep;

			integral = 0;
			size_t lCount = m_vErrors.size();
			for (size_t error = 0; error < lCount; ++error)
			{
				integral += m_vErrors[error] * m_vTimeSteps[error];
			}

			derivative = 0.0f;
			if (m_lLastNum >= 0)
				derivative = (m_vErrors[m_lErrorNum] - m_vErrors[m_lLastNum]) / a_fTimeStep;

			m_lLastNum = m_lErrorNum;
			m_lErrorNum++;
			if (m_lErrorNum >= (int)m_vErrors.size()) m_lErrorNum = 0;

			return m_vErrors[m_lLastNum] * p + integral * i + derivative * d;
		}

		void SetErrorNum(int a_lErrorNum)
		{
			m_vErrors.resize(a_lErrorNum);
			m_vTimeSteps.resize(a_lErrorNum, 0);
		}

		void Reset()
		{
			m_lErrorNum = 0;
			m_lLastNum = -1;
			m_vTimeSteps.assign(m_vTimeSteps.size(), 0);

			integral = 0;
			derivative = 0;
		}

		T GetLastError()
		{
			if (m_lLastNum >= 0) return m_vErrors[m_lLastNum];
			return 0;
		}

		T GetLastDerivatrive()
		{
			return derivative;
		}

		T GetLastIntegral()
		{
			return integral;
		}

	private:
		std::vector<T> m_vErrors;
		std::vector<float> m_vTimeSteps;

		T integral, derivative;

		int m_lErrorNum;
		int m_lLastNum;
	};

	typedef cPidController<float> cPidControllerf;
	typedef cPidController<cVector3f> cPidControllerVec3;
};
#endif