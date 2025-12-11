//
// Created by Arden on 12/2/2025.
//

#ifndef ANTIALIASEDHARDCLIPPER_BLAMP4POINT_H
#define ANTIALIASEDHARDCLIPPER_BLAMP4POINT_H

#include "HardClipper.h"

class Blamp4Point : public HardClipper
{
public:
    Blamp4Point() = default;
    ~Blamp4Point() override = default;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void processBlock (juce::AudioBuffer<float>& buffer) override;
    static float findCubicRoot(const std::array<float, 5>& y);
private:
    struct ChannelState
    {
        std::array<float, 4> inputSamples;
        std::array<float, 4> outputSamples;
        int inputSampleIndex = 0;
        int flag = false;
        int prevFlag = false;
    };

    std::vector<ChannelState> channelStates;
};


#endif //ANTIALIASEDHARDCLIPPER_BLAMP4POINT_H
