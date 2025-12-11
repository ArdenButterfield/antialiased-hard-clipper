//
// Created by Arden on 12/11/2025.
//

#ifndef ANTIALIASEDHARDCLIPPER_BLAMP4POINTCUBIC_H
#define ANTIALIASEDHARDCLIPPER_BLAMP4POINTCUBIC_H

#include "HardClipper.h"

class Blamp4PointCubic : public HardClipper
{
public:
    Blamp4PointCubic() = default;
    ~Blamp4PointCubic() override = default;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void processBlock (juce::AudioBuffer<float>& buffer) override;
    static float findCubicRoot(const std::array<float, 5>& y);
private:
    struct ChannelState
    {
        std::array<float, 5> inputSamples;
        std::array<float, 5> outputSamples;
        int inputSampleIndex = 0;
        int flag = false;
        int prevFlag = false;
    };

    std::array<float, 5> interpolationPoints;

    std::vector<ChannelState> channelStates;
};

#endif //ANTIALIASEDHARDCLIPPER_BLAMP4POINTCUBIC_H
