//
// Created by Arden on 12/11/2025.
//

#include "Blamp4PointCubic.h"
#include "boost/math/tools/roots.hpp"
#include "boost/math/interpolators/cubic_b_spline.hpp"

void Blamp4PointCubic::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    channelStates.resize (0);
}

void Blamp4PointCubic::processBlock (juce::AudioBuffer<float>& buffer)
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

            auto x_3prev = state.inputSamples[(state.inputSampleIndex + 1) % state.inputSamples.size()];
            auto x_2prev = state.inputSamples[(state.inputSampleIndex + 2) % state.inputSamples.size()];
            auto x_1prev = state.inputSamples[(state.inputSampleIndex + 3) % state.inputSamples.size()];
            auto x_1after = state.inputSamples[(state.inputSampleIndex + 4) % state.inputSamples.size()];
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
                if (state.flag == 1 || state.prevFlag == 1)
                {
                    interpolationPoints[0] = x_3prev - highThreshold;
                    interpolationPoints[1] = x_2prev - highThreshold;
                    interpolationPoints[2] = x_1prev - highThreshold;
                    interpolationPoints[3] = x_1after - highThreshold;
                    interpolationPoints[4] = x_2after - highThreshold;
                } else
                {
                    interpolationPoints[0] = x_3prev - lowThreshold;
                    interpolationPoints[1] = x_2prev - lowThreshold;
                    interpolationPoints[2] = x_1prev - lowThreshold;
                    interpolationPoints[3] = x_1after - lowThreshold;
                    interpolationPoints[4] = x_2after - lowThreshold;
                }


                boost::math::cubic_b_spline<float> spline(interpolationPoints.data(), interpolationPoints.size(), 0, 1);
                float intersectionPoint, slope;
                if (interpolationPoints[2] == 0)
                {
                    intersectionPoint = 0;
                    slope = spline.prime(2);
                } else if (interpolationPoints[3] == 0)
                {
                    intersectionPoint = 1;
                    slope = spline.prime(3);
                } else
                {
                    std::uintmax_t iterations = 100;
                    auto termination = [](double left, double right) {
                        return std::abs(left-right) < 0.0001;
                    };

                    auto root = boost::math::tools::bisect(spline, 2.0f, 3.0f, termination, iterations).first;
                    intersectionPoint = root - 2;
                    slope = spline.prime(root);
                }


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
                    state.outputSamples[(state.inputSampleIndex + 1) % state.inputSamples.size()] -= std::abs(slope) * p0;
                    state.outputSamples[(state.inputSampleIndex + 2) % state.inputSamples.size()] -= std::abs(slope) * p1;
                    state.outputSamples[(state.inputSampleIndex + 3) % state.inputSamples.size()] -= std::abs(slope) * p2;
                    state.outputSamples[(state.inputSampleIndex + 0) % state.inputSamples.size()] -= std::abs(slope) * p3;
                } else
                {
                    state.outputSamples[(state.inputSampleIndex + 1) % state.inputSamples.size()] += std::abs(slope) * p0;
                    state.outputSamples[(state.inputSampleIndex + 2) % state.inputSamples.size()] += std::abs(slope) * p1;
                    state.outputSamples[(state.inputSampleIndex + 3) % state.inputSamples.size()] += std::abs(slope) * p2;
                    state.outputSamples[(state.inputSampleIndex + 0) % state.inputSamples.size()] += std::abs(slope) * p3;
                }
            }

            ++state.inputSampleIndex;
            state.inputSampleIndex %= state.inputSamples.size();

            ptr[sample] = state.outputSamples[state.inputSampleIndex];

            state.prevFlag = state.flag;
        }
    }
}

float Blamp4PointCubic::findCubicRoot (const std::array<float, 5>& y)
{
    if ((y[2] > 0) != (y[3] > 0))
    {
        boost::math::cubic_b_spline<float> spline(y.data(), y.size(), 0, 1);

        std::uintmax_t iterations = 100;

        auto f_n = [=](float t) { return std::make_pair(spline(t), spline.prime(t)); };
        return boost::math::tools::newton_raphson_iterate(f_n, 2.5f, 2.0f, 3.0f, 20, iterations);
    }
    jassert(false);
    return 0;
}