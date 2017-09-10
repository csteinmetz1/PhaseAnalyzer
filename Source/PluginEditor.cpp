/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
PhaseAnalyzerAudioProcessorEditor::PhaseAnalyzerAudioProcessorEditor (PhaseAnalyzerAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p),
      sampleDelayLabel_("", "Sample Delay"),
      frameSizeLabel_("", "Frame Size"),
      accuracyLabel_("", "Accuracy"),
      framesAnalyzedLabel_("", "Frames Analyzed"),
      delayedChannelLabel_("", "Delayed Channel"),
      pathLengthLabel_("", "Path Length (cm)"),
      latencyLabel_("", "Latency (ms)"),
      hopSizeDivisorLabel_("", "Hop Size Divisor"),
      silenceThresholdLabel_("", "Silence Threshold"),
      windowTypeLabel_("", "Window Type")

{
    frameSizeOptions = {"64", "128", "256", "512", "1024", "2048"};
    hopSizeDivisorOptions = {"1", "2", "4", "8", "16"};
    windowTypeOptions = {"Hann", "Blackman-Harris", "Hamming", "Rectangular"};
    
    largeParameterFont_.setHeight(45.0);
    smallParameterFont_.setHeight(15.0);
    
    addAndMakeVisible(&sampleDelayTextEditor_);
    sampleDelayTextEditor_.setReadOnly(true);
    sampleDelayTextEditor_.setFont(largeParameterFont_);
    sampleDelayTextEditor_.setText("0", false);
    
    addAndMakeVisible(&frameSizeComboBox_);
    frameSizeComboBox_.setEditableText(false);
    frameSizeComboBox_.addItemList(frameSizeOptions, 1);
    frameSizeComboBox_.addListener(this);
    
    addAndMakeVisible(&accuracyTextEditor_);
    accuracyTextEditor_.setReadOnly(true);
    accuracyTextEditor_.setFont(Font (15.0f));
    accuracyTextEditor_.setText("0%", false);
    
    addAndMakeVisible(&framesAnalyzedTextEditor_);
    framesAnalyzedTextEditor_.setReadOnly(true);
    framesAnalyzedTextEditor_.setFont(smallParameterFont_);
    framesAnalyzedTextEditor_.setText("No", false);
    
    addAndMakeVisible(&delayedChannelTextEditor_);
    delayedChannelTextEditor_.setReadOnly(true);
    delayedChannelTextEditor_.setFont(smallParameterFont_);
    delayedChannelTextEditor_.setText("None");
    
    addAndMakeVisible(&pathLengthTextEditor_);
    pathLengthTextEditor_.setReadOnly(true);
    pathLengthTextEditor_.setFont(smallParameterFont_);
    pathLengthTextEditor_.setText("0");
    
    addAndMakeVisible(&latencyTextEditor_);
    latencyTextEditor_.setReadOnly(true);
    latencyTextEditor_.setFont(smallParameterFont_);
    latencyTextEditor_.setText("0");
    
    addAndMakeVisible(&hopSizeDivisorComboBox_);
    hopSizeDivisorComboBox_.setEditableText(false);
    hopSizeDivisorComboBox_.addItemList(hopSizeDivisorOptions, 1);
    hopSizeDivisorComboBox_.addListener(this);
    
    addAndMakeVisible(&silenceThresholdSlider_);
    silenceThresholdSlider_.setSliderStyle(Slider::LinearHorizontal);
    silenceThresholdSlider_.addListener(this);
    silenceThresholdSlider_.setRange(0.000, 0.1, 0.01);
    silenceThresholdSlider_.setTextBoxStyle(Slider::TextEntryBoxPosition(1), true, 50, 25);
    
    addAndMakeVisible(&windowTypeComboBox_);
    windowTypeComboBox_.setEditableText(false);
    windowTypeComboBox_.addItemList(windowTypeOptions, 1);
    windowTypeComboBox_.addListener(this);
    
    
    sampleDelayLabel_.attachToComponent(&sampleDelayTextEditor_, false);
    sampleDelayLabel_.setFont(Font (15.0f));
    
    frameSizeLabel_.attachToComponent(&frameSizeComboBox_, false);
    frameSizeLabel_.setFont(Font (15.0f));
    
    accuracyLabel_.attachToComponent(&accuracyTextEditor_, true);
    accuracyLabel_.setFont(Font (15.0f));
    
    framesAnalyzedLabel_.attachToComponent(&framesAnalyzedTextEditor_, true);
    framesAnalyzedLabel_.setFont(Font (15.0f));
    
    delayedChannelLabel_.attachToComponent(&delayedChannelTextEditor_, false);
    delayedChannelLabel_.setFont(Font (15.0f));

    pathLengthLabel_.attachToComponent(&pathLengthTextEditor_, false);
    pathLengthLabel_.setFont(Font (15.0f));
    
    latencyLabel_.attachToComponent(&latencyTextEditor_, false);
    latencyLabel_.setFont(Font (15.0f));
    
    hopSizeDivisorLabel_.attachToComponent(&hopSizeDivisorComboBox_, false);
    hopSizeDivisorLabel_.setFont(Font (15.0f));
    
    silenceThresholdLabel_.attachToComponent(&silenceThresholdSlider_, true);
    silenceThresholdLabel_.setFont(Font (15.0f));
    
    windowTypeLabel_.attachToComponent(&windowTypeComboBox_, false);
    windowTypeLabel_.setFont(Font (15.0f));
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 450);
    
    startTimer(50);
}

PhaseAnalyzerAudioProcessorEditor::~PhaseAnalyzerAudioProcessorEditor()
{
}

//==============================================================================
void PhaseAnalyzerAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (25.0f);
    g.drawSingleLineText ("GCC-PHAT Phase Analyzer", 25, 40, Justification::left);
    //g.setFont (15.0f);
    //g.drawSingleLineText("Christian Steinmetz", 25, 40, Justification::left);
    //g.setFont (12.0f);
    //g.drawSingleLineText("Clemson University", 25, 45, Justification::left);

}

void PhaseAnalyzerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    sampleDelayTextEditor_.setBounds(195, 150, 105, 40);
    frameSizeComboBox_.setBounds(50, 160, 100, 25);
    accuracyTextEditor_.setBounds(400, 160, 50, 25);
    framesAnalyzedTextEditor_.setBounds(400, 385, 45, 25);
    delayedChannelTextEditor_.setBounds(50, 315, 100, 25);
    pathLengthTextEditor_.setBounds(200, 315, 100, 25);
    latencyTextEditor_.setBounds(350, 315, 100, 25);
    hopSizeDivisorComboBox_.setBounds(50, 100, 100, 25);
    silenceThresholdSlider_.setBounds(160, 385, 150, 25);
    windowTypeComboBox_.setBounds(50, 220, 100, 25);
}

void PhaseAnalyzerAudioProcessorEditor::timerCallback()
{
    PhaseAnalyzerAudioProcessor* ourProcessor = getProcessor();
    
    if      ((int)(ourProcessor->frameSize) == 64)   {frameSizeComboBox_.setSelectedId(1);}
    else if ((int)(ourProcessor->frameSize) == 128)  {frameSizeComboBox_.setSelectedId(2);}
    else if ((int)(ourProcessor->frameSize) == 256)  {frameSizeComboBox_.setSelectedId(3);}
    else if ((int)(ourProcessor->frameSize) == 512)  {frameSizeComboBox_.setSelectedId(4);}
    else if ((int)(ourProcessor->frameSize) == 1024) {frameSizeComboBox_.setSelectedId(5);}
    else if ((int)(ourProcessor->frameSize) == 2048) {frameSizeComboBox_.setSelectedId(6);}

    
    if      ((int)(ourProcessor->hopSizeDivisor) == 1) {hopSizeDivisorComboBox_.setSelectedId(1);}
    else if ((int)(ourProcessor->hopSizeDivisor) == 2) {hopSizeDivisorComboBox_.setSelectedId(2);}
    else if ((int)(ourProcessor->hopSizeDivisor) == 4) {hopSizeDivisorComboBox_.setSelectedId(3);}
    else if ((int)(ourProcessor->hopSizeDivisor) == 8) {hopSizeDivisorComboBox_.setSelectedId(4);}
    else                                               {hopSizeDivisorComboBox_.setSelectedId(5);}

    
    if      ((int)(ourProcessor->windowType) == 1) {windowTypeComboBox_.setSelectedId(1);}
    else if ((int)(ourProcessor->windowType) == 2) {windowTypeComboBox_.setSelectedId(2);}
    else if ((int)(ourProcessor->windowType) == 3) {windowTypeComboBox_.setSelectedId(3);}
    else if ((int)(ourProcessor->windowType) == 4) {windowTypeComboBox_.setSelectedId(4);}
    
    accuracyString = (String(ourProcessor->accuracy).operator+=('%'));
    framesAnalyzedString  = (String(ourProcessor->framesAnalyzed));
    if (ourProcessor->delayedChannel) {delayedChannelString = "Left";} else {delayedChannelString = "Right";}
    
    sampleDelayTextEditor_.setText(String(ourProcessor->sampleDelay).paddedLeft(juce_wchar(' '), 4), false);
    accuracyTextEditor_.setText(accuracyString.paddedLeft(juce_wchar(' '), 3), false);
    framesAnalyzedTextEditor_.setText(framesAnalyzedString.paddedLeft(juce_wchar(' '), 4), false);
    delayedChannelTextEditor_.setText(delayedChannelString.paddedLeft(' ', 12));
    pathLengthTextEditor_.setText(String(ourProcessor->pathLength).paddedLeft(' ', 12));
    latencyTextEditor_.setText(String(ourProcessor->latency).paddedLeft(' ', 12));
    silenceThresholdSlider_.setValue(ourProcessor->silenceThreshold, dontSendNotification);
}

void PhaseAnalyzerAudioProcessorEditor::comboBoxChanged(ComboBox* comboBox)
{
    if (comboBox == &frameSizeComboBox_)
    {
        getProcessor()->setParameterNotifyingHost(PhaseAnalyzerAudioProcessor::kframeSize,
                                                  (String(frameSizeComboBox_.getText()).getFloatValue()));
    }
    
    else if (comboBox == &hopSizeDivisorComboBox_)
    {
        getProcessor()->setParameterNotifyingHost(PhaseAnalyzerAudioProcessor::khopSizeDivisor,
                                                  (String(hopSizeDivisorComboBox_.getText()).getFloatValue()));
    }
    
    else if (comboBox == &windowTypeComboBox_)
    {
        getProcessor()->setParameterNotifyingHost(PhaseAnalyzerAudioProcessor::kwindowType,
                                                  (String(windowTypeComboBox_.getSelectedId()).getFloatValue()));
    }
    
}

void PhaseAnalyzerAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    if (slider == &silenceThresholdSlider_);
    {
        getProcessor()->setParameterNotifyingHost(PhaseAnalyzerAudioProcessor::ksilenceThreshold,
                                                  (float)silenceThresholdSlider_.getValue());
    }
}