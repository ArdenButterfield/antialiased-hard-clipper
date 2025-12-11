#pragma once

#include "dsp/Blamp4PointCubic.h"

#include <juce_audio_processors/juce_audio_processors.h>

#if (MSVC)
#include "ipps.h"
#endif

#include "dsp/HardClipper.h"
#include "dsp/Blamp2Point.h"
#include "dsp/Blamp4Point.h"
#include "dsp/Oversampler2Times.h"
#include "dsp/Oversampler4Times.h"

class PluginProcessor : public juce::AudioProcessor, public juce::AudioProcessorParameter::Listener
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioParameterFloat* threshold;
    juce::AudioParameterChoice* clipperType;

private:
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;
    void parameterValueChanged(int parameterIndex, float newValue) override;

    bool clipperChanged;
    bool thresholdChanged;

    HardClipper hardClipper;
    Blamp2Point blamp2Point;
    Blamp4Point blamp4Point;
    Blamp4PointCubic blamp4PointCubic;
    Oversampler2Times oversampler2Times;
    Oversampler4Times oversampler4Times;

    std::vector<HardClipper*> clippers {
        &hardClipper,
        &blamp2Point,
        &blamp4Point,
        &blamp4PointCubic,
        &oversampler2Times,
        &oversampler4Times};

    HardClipper* activeClipper;

    double fs{};
    double samplesPerBlock{};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
