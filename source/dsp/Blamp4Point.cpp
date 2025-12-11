//
// Created by Arden on 12/2/2025.
//

#include "Blamp4Point.h"

#include "boost/math/tools/roots.hpp"

#include <boost/math/interpolators/cubic_b_spline.hpp>


void Blamp4Point::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    channelStates.resize (0);
}
void Blamp4Point::processBlock (juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumChannels() != channelStates.size())
    {
        channelStates.resize (buffer.getNumChannels());
    }

    for (auto channel = 0; channel < buffer.getNumChannels(); channel++)
    {
        auto& state = channelStates[channel];
        auto ptr = buffer.getWritePointer (channel);
        for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            state.inputSamples[state.inputSampleIndex] = ptr[sample];
            state.outputSamples[state.inputSampleIndex] = std::min(std::max(lowThreshold, ptr[sample]), highThreshold);

            auto x_2prev = state.inputSamples[(state.inputSampleIndex + 1) % state.inputSamples.size()];
            auto x_1prev = state.inputSamples[(state.inputSampleIndex + 2) % state.inputSamples.size()];
            auto x_1after = state.inputSamples[(state.inputSampleIndex + 3) % state.inputSamples.size()];
            auto x_2after = state.inputSamples[state.inputSampleIndex];

            if (x_1after >= highThreshold)
            {
                state.flag = 1;
            } else if (x_1after <= lowThreshold)
            {
                state.flag = -1;
            } else
            {
                state.flag = 0;
            }

            if (state.flag != state.prevFlag)
            {
                auto slope = x_1after - x_1prev; // TODO: cubic polynomial fit instead, as described on page 5262. Does it make a difference?
                auto intersectionPoint = (x_1prev > 0) ? (highThreshold - x_1prev) / slope : (lowThreshold - x_1prev) / slope;

                auto d = intersectionPoint;
                auto d2 = d * d;
                auto d3 = d * d2;
                auto d4 = d * d3;
                auto d5 = d * d4;

                auto p0 = d5/120;
                auto p1 = -d5/40 + d4/24 + d3/12 + d2/12 +d/24 + 1/120.f;
                auto p2 = d5/40 - d4/12 + d2/3 - d/2 + 7/30.f;
                auto p3 = -d5/12 + d4/24 - d3/12 + d2/12 - d/24 + 1/120.f;

                if (state.flag == 1 or state.prevFlag == 1)
                {
                    state.outputSamples[state.inputSampleIndex] -= std::abs(slope) * p0;
                    state.outputSamples[(state.inputSampleIndex + 1) % state.inputSamples.size()] -= std::abs(slope) * p1;
                    state.outputSamples[(state.inputSampleIndex + 2) % state.inputSamples.size()] -= std::abs(slope) * p2;
                    state.outputSamples[(state.inputSampleIndex + 3) % state.inputSamples.size()] -= std::abs(slope) * p3;
                } else
                {
                    state.outputSamples[state.inputSampleIndex] += std::abs(slope) * p0;
                    state.outputSamples[(state.inputSampleIndex + 1) % state.inputSamples.size()] += std::abs(slope) * p1;
                    state.outputSamples[(state.inputSampleIndex + 2) % state.inputSamples.size()] += std::abs(slope) * p2;
                    state.outputSamples[(state.inputSampleIndex + 3) % state.inputSamples.size()] += std::abs(slope) * p3;

                }
            }

            ++state.inputSampleIndex;
            state.inputSampleIndex %= state.inputSamples.size();

            ptr[sample] = state.outputSamples[state.inputSampleIndex];

            state.prevFlag = state.flag;
        }
    }
}

float Blamp4Point::findCubicRoot (const std::array<float, 5>& y)
{
    if ((y[1] > 0) != (y[2] > 0))
    {
        boost::math::cubic_b_spline<float> spline(y.data(), y.size(), 0, 1);

        auto termination = [](float left, float right) {
            return abs(left - right) < 1e-6;
        };
        std::uintmax_t iterations = 100;

        auto f_n = [=](float t) { return std::make_pair(spline(t), spline.prime(t)); };
        return boost::math::tools::newton_raphson_iterate(f_n, 1.5f, 1.0f, 2.0f, 20, iterations);
    }
    jassert(false);
}
