#include "catch2/internal/catch_stdstreams.hpp"
#include "helpers/test_helpers.h"
#include <PluginProcessor.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "dsp/Blamp2Point.h"
#include "dsp/Blamp4Point.h"
#include "dsp/HardClipper.h"
#include "dsp/Oversampler2Times.h"

TEST_CASE ("one is equal to one", "[dummy]")
{
    REQUIRE (1 == 1);
}

TEST_CASE ("Plugin instance", "[instance]")
{
    PluginProcessor testPlugin;

    SECTION ("name")
    {
        CHECK_THAT (testPlugin.getName().toStdString(),
            Catch::Matchers::Equals ("Antialiased Hard Clipper"));
    }
}

TEST_CASE ("Oversample stuff", "[ovs]")
{
    auto buffer = juce::AudioBuffer<float>(1, 100);
    for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        buffer.setSample (0, sample, sample % 20 < 10 ? 1.0f : 0.0f);
        std::cout << buffer.getSample (0, sample) << "\t";
    }
    std::cout << std::endl;

    auto clipper = Oversampler2Times();
    clipper.prepareToPlay (100, 44100);
    clipper.setThreshold (0.5f);
    clipper.processBlock (buffer);
    for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        std::cout << buffer.getSample (0, sample) << "\t";
    }
    std::cout << std::endl;
}

void testForClicks(HardClipper& clipper)
{
    clipper.setThreshold (0.5f);
    auto currentAngle = 0.0f;
    auto angleDelta = 30.0f * 2 * juce::MathConstants<float>::pi / 44100.0f;

    juce::AudioBuffer<float> sampleBuffer;
    const auto bsize = 128;
    clipper.prepareToPlay (bsize, 44100);
    sampleBuffer.setSize (1, bsize);
    auto prev = 0.f;
    for (auto iteration = 0; iteration < std::floor(44100.0f / bsize); ++iteration)
    {
        for (auto s = 0; s < sampleBuffer.getNumSamples(); ++s)
        {
            sampleBuffer.setSample (0, s, std::sin (currentAngle));
            currentAngle += angleDelta;
            while (currentAngle > 2 * juce::MathConstants<float>::pi)
            {
                currentAngle -= 2 * juce::MathConstants<float>::pi;
            }
        }
        clipper.processBlock (sampleBuffer);

        for (auto s = 0; s < sampleBuffer.getNumSamples(); ++s)
        {
            auto current = sampleBuffer.getSample (0, s);
            if (iteration > 1)
            {
                REQUIRE(std::abs(current - prev) < 0.1);
            }
            prev = current;
        }
    }
}

TEST_CASE ("Check naive for pops", "[pops]")
{
    HardClipper hardclipper;
    testForClicks (hardclipper);
}
TEST_CASE ("Check blamp2 for pops", "[popsb2]")
{
    Blamp2Point hardclipper;
    testForClicks (hardclipper);
}
TEST_CASE ("Check oversampled2 for pops", "[popsovs2]")
{
    Oversampler2Times hardclipper;
    testForClicks (hardclipper);
}


#ifdef PAMPLEJUCE_IPP
    #include <ipp.h>

TEST_CASE ("IPP version", "[ipp]")
{
    CHECK_THAT (ippsGetLibVersion()->Version, Catch::Matchers::Equals ("2022.2.0 (r0x42db1a66)"));
}
#endif
