#ifndef EFE_MATRIX_H
#define EFE_MATRIX_H

#include <math.h>
#include "math/Vector3.h"

namespace efe
{
	template <class T> class cMatrix
	{
	public:
		union
		{
			T m[4][4];
			T v[16];
		};

		inline cMatrix()
		{
		}

		inline cMatrix(
			T a_00, T a_01, T a_02, T a_03,
			T a_10, T a_11, T a_12, T a_13,
			T a_20, T a_21, T a_22, T a_23,
			T a_30, T a_31, T a_32, T a_33)
		{
			m[0][0] = a_00;
			m[0][1] = a_01;
			m[0][2] = a_02;
			m[0][3] = a_03;

			m[1][0] = a_10;
			m[1][1] = a_11;
			m[1][2] = a_12;
			m[1][3] = a_13;

			m[2][0] = a_20;
			m[2][1] = a_21;
			m[2][2] = a_22;
			m[2][3] = a_23;

			m[3][0] = a_30;
			m[3][1] = a_31;
			m[3][2] = a_32;
			m[3][3] = a_33;
		}

		static const cMatrix<T> Identity;
		static const cMatrix<T> Zero;

		//--------------------------------------------------------------

		inline bool operator==(const cMatrix<T> &a_Mtx) const
		{
			if (m[0][0] == a_Mtx.m[0][0] &&
				m[0][1] == a_Mtx.m[0][1] &&
				m[0][2] == a_Mtx.m[0][2] &&
				m[0][3] == a_Mtx.m[0][3] &&
				m[1][0] == a_Mtx.m[1][0] &&
				m[1][1] == a_Mtx.m[1][1] &&
				m[1][2] == a_Mtx.m[1][2] &&
				m[1][3] == a_Mtx.m[1][3] &&
				m[2][0] == a_Mtx.m[2][0] &&
				m[2][1] == a_Mtx.m[2][1] &&
				m[2][2] == a_Mtx.m[2][2] &&
				m[2][3] == a_Mtx.m[2][3] &&
				m[3][0] == a_Mtx.m[3][0] &&
				m[3][1] == a_Mtx.m[3][1] &&
				m[3][2] == a_Mtx.m[3][2] &&
				m[3][3] == a_Mtx.m[3][3])
				return true;

			return false;
		}

		//--------------------------------------------------------------

		inline void FromTranspose(const T *pA)
		{
			m[0][0] = pA[0];
			m[1][0] = pA[1];
			m[2][0] = pA[2];
			m[3][0] = pA[3];

			m[0][1] = pA[4];
			m[1][1] = pA[5];
			m[2][1] = pA[6];
			m[3][1] = pA[7];

			m[0][2] = pA[8];
			m[1][2] = pA[9];
			m[2][2] = pA[10];
			m[3][2] = pA[11];

			m[0][3] = pA[12];
			m[1][3] = pA[13];
			m[2][3] = pA[14];
			m[3][3] = pA[15];
		}

		//--------------------------------------------------------------

		inline cVector3<T> GetRight() const
		{
			return cVector3<T>(m[0][0], m[0][1], m[0][2]);
		}

		inline void SetRight(const cVector3<T> &a_vVec)
		{
			m[0][0] = a_vVec.x;
			m[0][1] = a_vVec.y;
			m[0][2] = a_vVec.z;
		}

		//--------------------------------------------------------------

		inline cVector3<T> GetUp() const
		{
			return cVector3<T>(m[1][0], m[1][1], m[1][2]);
		}

		inline void SetUp(const cVector3<T> &a_vVec)
		{
			m[1][0] = a_vVec.x;
			m[1][1] = a_vVec.y;
			m[1][2] = a_vVec.z;
		}

		//--------------------------------------------------------------

		inline cVector3<T> GetForward() const
		{
			return cVector3<T>(m[2][0], m[2][1], m[2][2]);
		}

		inline void SetForward(const cVector3<T> &a_vVec)
		{
			m[2][0] = a_vVec.x;
			m[2][1] = a_vVec.y;
			m[2][2] = a_vVec.z;
		}

		//--------------------------------------------------------------

		inline cVector3<T> GetTranslation() const
		{
			return cVector3<T>(m[0][3], m[1][3], m[2][3]);
		}

		inline void SetTranslation(const cVector3<T> &a_vTrans)
		{
			m[0][3] = a_vTrans.x;
			m[1][3] = a_vTrans.y;
			m[2][3] = a_vTrans.z;
		}

		inline cMatrix<T> GetRotation() const
		{
			return cMatrix<T>(	m[0][0], m[0][1], m[0][2], 0,
								m[1][0], m[1][1], m[1][2], 0,
								m[2][0], m[2][1], m[2][2], 0,
								0,		 0,		  0,	   1);
		}

		inline cMatrix<T> GetTranspose()const
		{
			return cMatrix<T>(m[0][0], m[1][0], m[2][0], m[3][0],
							m[0][1], m[1][1], m[2][1], m[3][1],
							m[0][2], m[1][2], m[2][2], m[3][2],
							m[0][3], m[1][3], m[2][3], m[3][3]);
		}
	};
};
#endif