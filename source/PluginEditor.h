#pragma once

#include "BinaryData.h"
#include "PluginProcessor.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "melatonin_inspector/melatonin_inspector.h"

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor,
            public juce::ComboBox::Listener,
            public juce::Slider::Listener,
            public juce::Timer
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged(juce::Slider*) override;
    void sliderDragEnded(juce::Slider*) override;
    void sliderDragStarted(juce::Slider*) override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

    void timerCallback() override;

    juce::Slider thresholdSlider;
    juce::Label thresholdLabel;
    juce::ComboBox clippingType;
    juce::Label clippingLabel;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;
    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect the UI" };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
