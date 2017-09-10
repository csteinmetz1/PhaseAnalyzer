/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/**
*/
class PhaseAnalyzerAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    PhaseAnalyzerAudioProcessor();
    ~PhaseAnalyzerAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;
    
    int getNumParameters() override;
    float getParameter(int index) override;
    void setParameter(int index, float newValue) override;
    
    const String getParameterName(int index) override;
    const String getParameterText(int index) override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // My functions
    //==============================================================================
    int gccPHAT(int frame);
    void applyWindow(AudioSampleBuffer& gccData);
    void resetBuffer();
    
    //==============================================================================
    FFT fft[7]  {{6, false}, {7, false}, {8, false}, {9, false}, {10, false}, {11, false}, {12, false}};
    FFT ifft[7] {{6, true},  {7, true},  {8, true},  {9, true},  {10, true},  {11, true},  {12, true}};
    FFT::Complex L_td[4096], R_td[4096], L_fd[4096], R_fd[4096], S_td[4096], S_fd[4096];
    //==============================================================================
    
    enum Parameters
    {
        kframeSize = 0,
        khopSizeDivisor,
        ksilenceThreshold,
        kwindowType,
        kNumParmeters
    };
    
    int sampleDelay;
    int frameSize;
    float accuracy;
    bool active;
    bool delayedChannel;
    float pathLength;
    float latency;
    double sampleRate_;
    double speedOfSound;
    
    int hopSizeDivisor;
    int hopSize;
    int rmsLevel;
    float silenceThreshold;
    int windowType;
    
    int numCalculations;
    int frameSizeIndex;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PhaseAnalyzerAudioProcessor)
    AudioSampleBuffer analysisBuffer_, gccBuffer;
    int analysisBufferLength_, gccBufferLength;
    int samplesAdded;
};
