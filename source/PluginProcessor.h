#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#if (MSVC)
#include "ipps.h"
#endif

#include "dsp/HardClipper.h"
#include "dsp/Blamp2Point.h"
#include "dsp/Blamp4Point.h"

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


    std::vector<HardClipper*> clippers {&hardClipper, &blamp2Point, &blamp4Point};
    HardClipper* activeClipper;

    double fs{};
    double samplesPerBlock{};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
