//
// Created by Arden on 12/2/2025.
//

#ifndef ANTIALIASEDHARDCLIPPER_HARDCLIPPER_H
#define ANTIALIASEDHARDCLIPPER_HARDCLIPPER_H

#include "juce_audio_basics/juce_audio_basics.h"

class HardClipper
{
public:
    HardClipper();
    virtual ~HardClipper() = default;
    virtual void prepareToPlay (int samplesPerBlockExpected, double sampleRate);
    virtual void processBlock (juce::AudioBuffer<float>& buffer);
    virtual void releaseResources ();
    void setThreshold (float t);
protected:
    float lowThreshold;
    float highThreshold;
};

#endif //ANTIALIASEDHARDCLIPPER_HARDCLIPPER_H
