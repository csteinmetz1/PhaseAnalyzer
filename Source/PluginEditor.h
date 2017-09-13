/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class PhaseAnalyzerAudioProcessorEditor  : public AudioProcessorEditor,
                                           public Timer,
                                           private ComboBox::Listener,
                                           private Slider::Listener,
                                           private Button::Listener,
                                           private TextEditor::Listener
{
public:
    PhaseAnalyzerAudioProcessorEditor (PhaseAnalyzerAudioProcessor&);
    ~PhaseAnalyzerAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void comboBoxChanged(ComboBox*) override;
    void sliderValueChanged(Slider*) override;
    void buttonClicked(Button*) override;
    void textEditorReturnKeyPressed(TextEditor&) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PhaseAnalyzerAudioProcessor& processor;
    
    ToggleButton delayCorrectionToggleButton_;
    
    TextEditor sampleDelayTextEditor_, accuracyTextEditor_, framesAnalyzedTextEditor_, delayedChannelTextEditor_,
               pathLengthTextEditor_, latencyTextEditor_, tempTextEditor_;
    
    Label sampleDelayLabel_, frameSizeLabel_, accuracyLabel_, framesAnalyzedLabel_, delayedChannelLabel_,
          pathLengthLabel_, latencyLabel_, hopSizeDivisorLabel_, silenceThresholdLabel_, windowTypeLabel_,
          delayCorrectionLabel_, tempLabel_;
    
    ComboBox frameSizeComboBox_, hopSizeDivisorComboBox_, windowTypeComboBox_;
    
    Slider silenceThresholdSlider_;
        
    String accuracyString, framesAnalyzedString, delayedChannelString;
    
    Font largeParameterFont_, smallParameterFont_;
    
    StringArray frameSizeOptions, hopSizeDivisorOptions, windowTypeOptions;
    
    PhaseAnalyzerAudioProcessor* getProcessor() const
    {
        return static_cast <PhaseAnalyzerAudioProcessor*> (getAudioProcessor());
    }
    
   
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhaseAnalyzerAudioProcessorEditor)
};
