//
// Created by Arden on 12/3/2025.
//

#include "Oversampler4Times.h"

Oversampler4Times::Oversampler4Times()
{

}

Oversampler4Times::~Oversampler4Times()
{

}
void Oversampler4Times::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    channelStates.resize (0);
}
void Oversampler4Times::processBlock (juce::AudioBuffer<float>& buffer)
{
    if (channelStates.size() != buffer.getNumChannels())
    {
        channelStates.resize (buffer.getNumChannels());
        for (auto& s : channelStates)
        {
            s.oversampled.fill (0.0);
            s.oversampledOut.fill (0.0);
            s.position = 0;
        }
    }

    for (auto channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto ptr = buffer.getWritePointer (channel);
        auto& state = channelStates[channel];
        for (auto s = 0; s < buffer.getNumSamples(); ++s)
        {
            auto in = ptr[s];
            state.oversampled[(state.position + 13) % 16] += in * 0.25f;
            state.oversampled[(state.position + 14) % 16] += in * 0.5f;
            state.oversampled[(state.position + 15) % 16] += in * 0.75f;
            state.oversampled[state.position] = in * 1.0f;
            state.oversampled[(state.position + 1) % 16] = in * 0.75f;
            state.oversampled[(state.position + 2) % 16] = in * 0.5f;
            state.oversampled[(state.position + 3) % 16] = in * 0.25f;

            state.oversampledOut[state.position] = std::min(std::max(lowThreshold, state.oversampled[state.position]), highThreshold);
            state.oversampledOut[(state.position + 15) % 16] = std::min(std::max(lowThreshold, state.oversampled[(state.position + 7) % 16]), highThreshold);
            state.oversampledOut[(state.position + 14) % 16] = std::min(std::max(lowThreshold, state.oversampled[(state.position + 6) % 16]), highThreshold);
            state.oversampledOut[(state.position + 13) % 16] = std::min(std::max(lowThreshold, state.oversampled[(state.position + 5) % 16]), highThreshold);

            ptr[s] =
                state.oversampledOut[state.position] * 0.25f/4 +
                state.oversampledOut[(state.position + 15) % 16] * 0.5f/4 +
                state.oversampledOut[(state.position + 14) % 16] * 0.75f/4 +
                state.oversampledOut[(state.position + 13) % 16] * 1.0f/4 +
                state.oversampledOut[(state.position + 12) % 16] * 0.75f/4 +
                state.oversampledOut[(state.position + 11) % 16] * 0.5f/4 +
                state.oversampledOut[(state.position + 10) % 16] * 0.5f/4;

            state.position = (state.position + 4) % 16;
        }
    }
}

