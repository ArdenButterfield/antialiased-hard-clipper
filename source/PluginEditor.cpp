#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

    addAndMakeVisible (inspectButton);

    // this chunk of code instantiates and opens the melatonin inspector
    inspectButton.onClick = [&] {
        if (!inspector)
        {
            inspector = std::make_unique<melatonin::Inspector> (*this);
            inspector->onClose = [this]() { inspector.reset(); };
        }

        inspector->setVisible (true);
    };

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    thresholdLabel.setText ("Threshold", juce::dontSendNotification);
    clippingLabel.setText ("Clipping type", juce::dontSendNotification);
    thresholdSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    addAndMakeVisible (clippingLabel);
    addAndMakeVisible (thresholdLabel);
    addAndMakeVisible (clippingType);
    addAndMakeVisible (thresholdSlider);

    clippingType.addItemList (processorRef.clipperType->choices, 1);
    thresholdSlider.setRange (processorRef.threshold->range.start, processorRef.threshold->range.end);

    startTimerHz (1);

    clippingType.addListener (this);
    thresholdSlider.addListener (this);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::resized()
{
    inspectButton.setBounds ({0,0,150,50});
    clippingLabel.setBounds ({0,60,150,50});
    clippingType.setBounds ({0,120,150,50});
    thresholdLabel.setBounds ({150,0,150,50});
    thresholdSlider.setBounds ({150,50,150,150});
}
void PluginEditor::sliderValueChanged (juce::Slider*)
{
    *processorRef.threshold = thresholdSlider.getValue();
}
void PluginEditor::sliderDragEnded (juce::Slider* slider)
{
}

void PluginEditor::sliderDragStarted (juce::Slider* slider)
{
}

void PluginEditor::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    std::cout << "comboboxchanged " <<comboBoxThatHasChanged->getSelectedId() << std::endl;
    *processorRef.clipperType = clippingType.getSelectedId() - 1;
}

void PluginEditor::timerCallback()
{
    thresholdSlider.setValue (thresholdSlider.getValue());
    clippingType.setSelectedId (*processorRef.clipperType + 1);
}
