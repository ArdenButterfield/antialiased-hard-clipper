//
// Created by Arden on 12/3/2025.
//

#ifndef ANTIALIASEDHARDCLIPPER_OVERSAMPLER4TIMES_H
#define ANTIALIASEDHARDCLIPPER_OVERSAMPLER4TIMES_H

#include "HardClipper.h"

class Oversampler4Times : public HardClipper
{
public:
    Oversampler4Times();
    ~Oversampler4Times() override;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void processBlock(juce::AudioBuffer<float>& buffer) override;
private:
    struct ChannelState {
        std::array<float, 16> oversampled;
        std::array<float, 16> oversampledOut;
        int position;
    };

    std::vector<ChannelState> channelStates;

};

#endif //ANTIALIASEDHARDCLIPPER_OVERSAMPLER4TIMES_H
