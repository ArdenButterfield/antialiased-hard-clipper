//
// Created by Arden on 12/2/2025.
//

#ifndef ANTIALIASEDHARDCLIPPER_OVERSAMPLER2TIMES_H
#define ANTIALIASEDHARDCLIPPER_OVERSAMPLER2TIMES_H

#include "HardClipper.h"

class Oversampler2Times : public HardClipper
{
public:
    Oversampler2Times ();
    ~Oversampler2Times () override;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void processBlock(juce::AudioBuffer<float>& buffer) override;
private:
    struct ChannelState {
        std::array<float, 4> oversampled;
        std::array<float, 4> oversampledOut;
        int position;
    };

    std::vector<ChannelState> channelStates;
};

#endif //ANTIALIASEDHARDCLIPPER_OVERSAMPLER2TIMES_H
