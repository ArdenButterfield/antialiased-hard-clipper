//
// Created by Arden on 12/2/2025.
//

#include "Blamp2Point.h"

#include "juce_events/native/juce_RunningInUnity.h"

#include <cmath>

Blamp2Point::Blamp2Point()
{
}
Blamp2Point::~Blamp2Point()
{
}
void Blamp2Point::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    channelStates.resize (0);
}
void Blamp2Point::processBlock (juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumChannels() != channelStates.size())
    {
        channelStates.resize (buffer.getNumChannels());
    }

    for (auto channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        auto state = channelStates[channel];
        auto ptr = buffer.getWritePointer (channel);
        for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            if (std::abs(ptr[sample]) >= highThreshold)
            {
                state.flag = true;
                state.y_n = ptr[sample] > 0 ? highThreshold : lowThreshold;
            } else
            {
                state.flag = false;
                state.y_n = ptr[sample];
            }

            if (state.flag != state.flag_previous)
            {
                auto m = ptr[sample] - state.x_n1;
                auto d = ((state.x_n1 > 0 ? highThreshold : lowThreshold) - state.x_n1) / m;
                auto p1 = - (d * d * d) / 6.0f + (d * 2) / 2.0f - d / 2.0f + 1 / 6.0f;
                auto p0 = (d * d * d) / 6;
                state.y_n1 -= state.x_n1 > 0 ? (std::abs(m) * p1) : -(std::abs(m) * p1);
                state.y_n -= state.x_n1 > 0 ? (std::abs(m) * p1) : -(std::abs(m) * p0);
            }

            state.x_n1 = ptr[sample];
            ptr[sample] = state.y_n1;
            state.flag_previous = state.flag;
            state.y_n1 = state.y_n;
        }
    }
}
