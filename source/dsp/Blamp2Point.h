//
// Created by Arden on 12/2/2025.
//

#ifndef ANTIALIASEDHARDCLIPPER_BLAMP2POINT_H
#define ANTIALIASEDHARDCLIPPER_BLAMP2POINT_H

#include "HardClipper.h"
#include <vector>

class Blamp2Point : public HardClipper
{
public:
    Blamp2Point();
    ~Blamp2Point() override;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void processBlock (juce::AudioBuffer<float>& buffer) override;
private:
    struct ChannelState
    {
        bool flag = false;
        bool flag_previous = false;
        float y_n = 0;
        float y_n1 = 0;
        float x_n1 = 0;
    };

    std::vector<ChannelState> channelStates;
};

#endif //ANTIALIASEDHARDCLIPPER_BLAMP2POINT_H
