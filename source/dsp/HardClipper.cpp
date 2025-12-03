//
// Created by Arden on 12/2/2025.
//

#include "HardClipper.h"

HardClipper::HardClipper() : lowThreshold (-1), highThreshold (1)
{
}
void HardClipper::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{

}

void HardClipper::processBlock (juce::AudioBuffer<float>& buffer)
{
    for (auto channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto ptr = buffer.getWritePointer (channel);
        for (auto i = 0; i < buffer.getNumSamples(); ++i)
        {
            ptr[i] = std::min(std::max(lowThreshold, ptr[i]), highThreshold);
        }
    }
}

void HardClipper::releaseResources()
{

}
void HardClipper::setThreshold (float t)
{
    lowThreshold = -t;
    highThreshold = t;
}
