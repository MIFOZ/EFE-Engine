#ifndef EFE_NEURAL_NETWORK_H
#define EFE_NEURAL_NETWORK_H

#include "system\String.h"
#include <cassert>

namespace efe
{
	struct cNeuronConnection
	{
		double weight;
		double deltaWeight;
	};
	typedef std::vector<cNeuronConnection> tNeuronConnectionVec;

	class ÒNeuron;

	typedef std::vector<ÒNeuron> ÒNetworkLayer;
	typedef std::vector<ÒNetworkLayer> tNetworkLayerVec;

	class ÒNeuron
	{
	public:
		ÒNeuron(unsigned int a_lOutputNum, unsigned int a_lIndex);
		void SetOutputVal(double a_fVal) {m_fOutputVal = a_fVal;}
		double GetOutputVal() const {return m_fOutputVal;}
		void FeedForward(const ÒNetworkLayer &a_PrevLayer);
		void CalcOutputGradients(double a_fTargetVal);
		void CalcHiddenGradients(const ÒNetworkLayer &a_NextLayer);
		void UpdateInputWeights(ÒNetworkLayer &a_PrevLayer);

	private:
		static double s_fEta;   // [0.0..1.0] overall net training rate
		static double s_fAlpha; // [0.0..n] multiplier of last weight change (momentum)

		static double TransferFunction(double a_fX);
		static double TransferFunctionDerivative(double a_fX);
		static double RandomWeight() {return rand() / double(RAND_MAX);}
		double SumDOW(const ÒNetworkLayer &a_NextLayer) const;

		double m_fOutputVal;
		tNeuronConnectionVec m_vOutputWeights;
		unsigned int m_lIndex;
		double m_fGradient;
	};

	// ****************** class cNet ******************
	class cNet
	{
	public:
		cNet(const tUIntVec &a_vTopology);
		void FeedForward(const tFloatVec &a_vInputVals);
		void BackProp(const tFloatVec &a_vTargetVals);
		void GetResults(tFloatVec &a_ResultVals) const;
		double GetRecentAverageError() const {return m_fRecentAverageError;}

	private:
		static double s_fRecentAverageSmoothingFactor;

		tNetworkLayerVec m_vLayers; // m_vLayers[layerNum][neuronNum]
		float m_fError;
		float m_fRecentAverageError;
	};
};

#endif