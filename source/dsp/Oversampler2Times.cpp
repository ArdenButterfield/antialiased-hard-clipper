//
// Created by Arden on 12/2/2025.
//

#include "Oversampler2Times.h"

#include "juce_events/native/juce_RunningInUnity.h"

Oversampler2Times::Oversampler2Times()
{

}
Oversampler2Times::~Oversampler2Times()
{
}
void Oversampler2Times::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    channelStates.resize (0);
}
void Oversampler2Times::processBlock (juce::AudioBuffer<float>& buffer)
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

            auto pos = state.position;
            auto prev_pos = (state.position + 3) % 4;
            auto prev_prev_pos = (state.position + 2) % 4;
            auto next_pos = (state.position + 1) % 4;

            state.oversampled[state.position] = in * 1.0f;
            state.oversampled[prev_pos] += in * 0.5f;
            state.oversampled[next_pos] = in * 0.5f;

            state.oversampledOut[pos] = std::min(std::max(lowThreshold, state.oversampled[pos]), highThreshold);
            state.oversampledOut[prev_pos] = std::min(std::max(lowThreshold, state.oversampled[prev_pos]), highThreshold);

            ptr[s] =
                state.oversampledOut[prev_prev_pos] * 0.25f +
                state.oversampledOut[prev_pos] * 0.5f +
                state.oversampledOut[pos] * 0.25f;

            state.position = (state.position + 2) % 4;
        }
    }
}
