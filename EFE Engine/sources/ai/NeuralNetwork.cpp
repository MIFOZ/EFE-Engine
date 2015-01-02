#include "ai\NeuralNetwork.h"

namespace efe
{
	double ÒNeuron::s_fEta = 0.15;    // overall net learning rate, [0.0..1.0]
	double ÒNeuron::s_fAlpha = 0.5;   // momentum, multiplier of last deltaWeight, [0.0..1.0]

	void ÒNeuron::UpdateInputWeights(ÒNetworkLayer &a_PrevLayer)
	{
		// The weights to be updated are in the Connection container
		// in the neurons in the preceding layer

		for (unsigned n = 0; n < a_PrevLayer.size(); ++n)
		{
			ÒNeuron &Neuron = a_PrevLayer[n];
			double fOldDeltaWeight = Neuron.m_vOutputWeights[m_lIndex].deltaWeight;

			double fNewDeltaWeight =
				// Individual input, magnified by the gradient and train rate:
				s_fEta
				* Neuron.GetOutputVal()
				* m_fGradient
				// Also add momentum = a fraction of the previous delta weight;
				+ s_fAlpha
				* fOldDeltaWeight;

			Neuron.m_vOutputWeights[m_lIndex].deltaWeight = fNewDeltaWeight;
			Neuron.m_vOutputWeights[m_lIndex].weight += fNewDeltaWeight;
		}
	}

	double ÒNeuron::SumDOW(const ÒNetworkLayer &a_NextLayer) const
	{
		double fSum = 0.0;

		// Sum our contributions of the errors at the nodes we feed.

		for (unsigned n = 0; n < a_NextLayer.size() - 1; ++n)
		{
			fSum += m_vOutputWeights[n].weight * a_NextLayer[n].m_fGradient;
		}

		return fSum;
	}

	void ÒNeuron::CalcHiddenGradients(const ÒNetworkLayer &a_NextLayer)
	{
		double fDow = SumDOW(a_NextLayer);
		m_fGradient = fDow * ÒNeuron::TransferFunctionDerivative(m_fOutputVal);
	}

	void ÒNeuron::CalcOutputGradients(double a_fTargetVal)
	{
		double fDelta = a_fTargetVal - m_fOutputVal;
		m_fGradient = fDelta * ÒNeuron::TransferFunctionDerivative(m_fOutputVal);
	}

	double ÒNeuron::TransferFunction(double a_fX)
	{
		// tanh - output range [-1.0..1.0]
		return tanh(a_fX);
	}

	double ÒNeuron::TransferFunctionDerivative(double a_fX)
	{
		// tanh derivative
		return 1.0 - a_fX * a_fX;
	}

	void ÒNeuron::FeedForward(const ÒNetworkLayer &a_PrevLayer)
	{
		double fSum = 0.0;

		// Sum the previous layer's outputs (which are our inputs)
		// Include the bias node from the previous layer.

		for (unsigned int n = 0; n < a_PrevLayer.size(); ++n)
		{
			fSum += a_PrevLayer[n].GetOutputVal() *
				a_PrevLayer[n].m_vOutputWeights[m_lIndex].weight;
		}

		m_fOutputVal = ÒNeuron::TransferFunction(fSum);
	}

	ÒNeuron::ÒNeuron(unsigned int a_lOutputNum, unsigned int a_lIndex)
	{
		for (unsigned int i = 0; i < a_lOutputNum; i++)
		{
			m_vOutputWeights.push_back(cNeuronConnection());
			m_vOutputWeights.back().weight = RandomWeight();
		}

		m_lIndex = a_lIndex;
	}

	double cNet::s_fRecentAverageSmoothingFactor = 100.0; // Number of training samples to average over

	cNet::cNet(const tUIntVec &a_vTopology)
	{
		unsigned numLayers = a_vTopology.size();
		for (unsigned int layerNum = 0; layerNum < numLayers; layerNum++)
		{
			m_vLayers.push_back(ÒNetworkLayer());
			unsigned int lOutputNum = layerNum == a_vTopology.size() - 1 ? 0 : a_vTopology[layerNum + 1];

			// We have a new layer, now fill it with neurons, and
			// add a bias neuron in each layer.
			for (unsigned int neuronNum = 0; neuronNum <= a_vTopology[layerNum]; ++neuronNum)
			{
				m_vLayers.back().push_back(ÒNeuron(lOutputNum, neuronNum));
			}

			// Force the bias node's output to 1.0 (it was the last neuron pushed in this layer):
			m_vLayers.back().back().SetOutputVal(1.0);
		}
	}

	void cNet::GetResults(tFloatVec &a_ResultVals) const
	{
		a_ResultVals.clear();

		for (unsigned int n = 0; n < m_vLayers.back().size() - 1; n++)
		{
			a_ResultVals.push_back(m_vLayers.back()[n].GetOutputVal());
		}
	}

	void cNet::BackProp(const tFloatVec &a_vTargetVals)
	{
		// Calculate overall net error (RMS of output neuron errors)

		ÒNetworkLayer &OutputLayer = m_vLayers.back();
		m_fError = 0.0f;

		for (unsigned int n = 0; n < OutputLayer.size() - 1; n++)
		{
			double fDelta = a_vTargetVals[n] - OutputLayer[n].GetOutputVal();
			m_fError += fDelta * fDelta;
		}
		m_fError /= OutputLayer.size() - 1; // get average error squared
		m_fError = sqrt(m_fError); // RMS

		// Implement a recent average measurement
		m_fRecentAverageError = (m_fRecentAverageError * s_fRecentAverageSmoothingFactor + m_fError)
			/ (s_fRecentAverageSmoothingFactor + 1.0);

		// Calculate output layer gradients
		for (unsigned int n = 0; n < OutputLayer.size() - 1; n++)
		{
			OutputLayer[n].CalcOutputGradients(a_vTargetVals[n]);
		}

		// Calculate hidden layer gradients

		for (unsigned int layerNum = m_vLayers.size() - 2; layerNum > 0; --layerNum)
		{
			ÒNetworkLayer &HiddenLayer = m_vLayers[layerNum];
			ÒNetworkLayer &NextLayer = m_vLayers[layerNum + 1];

			for (unsigned int n = 0; n < HiddenLayer.size(); ++n) {
				HiddenLayer[n].CalcHiddenGradients(NextLayer);
			}
		}

		// For all layers from outputs to first hidden layer,
		// update connection weights
		for (unsigned layerNum = m_vLayers.size() - 1; layerNum > 0; --layerNum)
		{
			ÒNetworkLayer &Layer = m_vLayers[layerNum];
			ÒNetworkLayer &PrevLayer = m_vLayers[layerNum - 1];

			for (unsigned int n = 0; n < Layer.size() - 1; ++n)
			{
				Layer[n].UpdateInputWeights(PrevLayer);
			}
		}
	}

	void cNet::FeedForward(const tFloatVec &a_vInputVals)
	{
		assert(a_vInputVals.size() == m_vLayers[0].size() - 1);

		// Assign (latch) the input values into the input neurons
		for (unsigned int i = 0; i < a_vInputVals.size(); i++)
		{
			m_vLayers[0][i].SetOutputVal(a_vInputVals[i]);
		}

		// forward propagate
		for (unsigned int layerNum = 1; layerNum < m_vLayers.size(); ++layerNum)
		{
			ÒNetworkLayer &PrevLayer = m_vLayers[layerNum - 1];
			for (unsigned int n = 0; n < m_vLayers[layerNum].size() - 1; n++)
			{
				m_vLayers[layerNum][n].FeedForward(PrevLayer);
			}
		}
	}
}