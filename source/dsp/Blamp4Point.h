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
    void processBlock (juce::AudioBuffer<float>& buffer) override;
};

#endif //ANTIALIASEDHARDCLIPPER_BLAMP4POINT_H
