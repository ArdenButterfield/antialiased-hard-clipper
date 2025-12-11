#include "catch2/internal/catch_stdstreams.hpp"
#include "helpers/test_helpers.h"
#include <PluginProcessor.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <juce_audio_formats/juce_audio_formats.h>

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

void generateSineTone(juce::AudioBuffer<float>& buffer, float frequency)
{
    auto angleDelta = frequency * juce::MathConstants<float>::twoPi / 44100.0f;
    auto angle = 0.f;
    for (int s = 0; s < buffer.getNumSamples(); ++s)
    {
        angle += angleDelta;
        while (angle >= juce::MathConstants<float>::twoPi)
        {
            angle -= juce::MathConstants<float>::twoPi;
        }
        auto x = cos(angle);
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            buffer.setSample (channel, s, x);
        }
    }
}

void writeToFile(juce::AudioBuffer<float>& buffer, juce::File& file)
{
    juce::WavAudioFormat format;
    std::unique_ptr<juce::AudioFormatWriter> writer;
    std::unique_ptr<juce::OutputStream> fileStream = std::make_unique<juce::FileOutputStream>(file);
    writer = format.createWriterFor (fileStream, juce::AudioFormatWriterOptions ()
        .withSampleRate (44100).withNumChannels (buffer.getNumChannels()).withBitsPerSample (24));
    if (writer != nullptr)
    {
        writer->writeFromAudioSampleBuffer (buffer, 0, buffer.getNumSamples());
    } else
    {
        std::cerr << "Could not open output file" << std::endl;
    }
}

#if false
TEST_CASE ("Write outputs for graphs", "[writes]")
{
    auto naiveClipper = HardClipper();
    auto oversampler2x = Oversampler2Times();
    auto oversampler4x = Oversampler4Times();
    auto blamp2x = Blamp2Point();

    std::vector<juce::String> names = {"naive", "oversampler2x", "oversampler4x", "blamp2x"};
    std::vector<HardClipper*> clippers = {&naiveClipper, &oversampler2x, &oversampler4x, &blamp2x};
    auto buf = juce::AudioBuffer<float>(1, 65536 /* 2 ** 16 */);

    for (int frequency = 50; frequency <= 20000; frequency += 50)
    {
        std::cout << frequency << std::endl;
        for (int clipper = 0; clipper < clippers.size(); ++clipper)
        {
            for (float thresh = 1.0f; thresh >= 0.01f; thresh /= 2.f)
            {
                generateSineTone (buf, frequency);
                clippers[clipper]->prepareToPlay (buf.getNumSamples(), 44100);
                clippers[clipper]->setThreshold (thresh);
                clippers[clipper]->processBlock (buf);
                auto outFile = juce::File(juce::File::getCurrentWorkingDirectory().getParentDirectory().getChildFile (
                    "Analysis-and-graphs/outfiles/" + names[clipper] + "_" + juce::String(frequency) + "_" + juce::String(thresh) + "_thresh.wav"));
                writeToFile (buf, outFile);
            }
        }
    }

}
#endif

TEST_CASE ("Oversample stuff", "[ovs]")
{
    auto buffer = juce::AudioBuffer<float>(1, 100);
    for (auto sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        buffer.setSample (0, sample, sample % 20 < 10 ? 1.0f : 0.0f);
    }

    auto clipper = Oversampler2Times();
    clipper.prepareToPlay (100, 44100);
    clipper.setThreshold (0.5f);
    clipper.processBlock (buffer);
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

TEST_CASE ("cubic", "[cubic]")
{
    std::array<float, 5> y = {-1,-2, 2, 1,4};
    auto root = Blamp4Point::findCubicRoot (y);
    REQUIRE (root > 1);
    REQUIRE (root < 2);
}

#ifdef PAMPLEJUCE_IPP
    #include <ipp.h>

TEST_CASE ("IPP version", "[ipp]")
{
    CHECK_THAT (ippsGetLibVersion()->Version, Catch::Matchers::Equals ("2022.2.0 (r0x42db1a66)"));
}
#endif
