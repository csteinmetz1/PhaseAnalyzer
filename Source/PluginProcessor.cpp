/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>
#include <unordered_map>
using namespace std;



//==============================================================================
PhaseAnalyzerAudioProcessor::PhaseAnalyzerAudioProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
     AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ), analysisBuffer_(2, 1)
#endif
{
    sampleDelay    = 0 ;   // delay in samples      ( 0-2048)
    frameSize      = 512; // analysis frame size   (64-4096)
    accuracy       = 0;  // delay est. accuracy ( 0-100%)
    framesAnalyzed = 0;    // is GCC analysis occuring
    delayedChannel = 0;    // 0 - right delayed 1 - left delayed
    pathLength     = 0;    // path length difference in cm
    latency        = 0;    // delay time in ms
    
    hopSize           = 0;
    hopSizeDivisor    = 4;
    rmsLevel          = 0;
    silenceThreshold  = 0.05; // change this to dB
    windowType        = 1; // 1 - Hann, 2 - Blackman-Harris, 3 - Hamming
    
    analysisBufferLength_  = 4096;
    numCalculations        = 0;
    frameSizeIndex         = 3;
    
    speedOfSound = 346.13; // speed of sound in m/s
    temp         = 20;     // ambient temperature in deg C
    
    delayCorrection = false;
    wptr = 0;
    rptr = 0;
}

PhaseAnalyzerAudioProcessor::~PhaseAnalyzerAudioProcessor()
{
    /*
    for (int i = 0; i < 8; i++)
    {
        fft[i].~FFT();
        ifft[i].~FFT();
    }
     */
}

//==============================================================================
const String PhaseAnalyzerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int PhaseAnalyzerAudioProcessor::getNumParameters()
{
    return kNumParmeters;
}

float PhaseAnalyzerAudioProcessor::getParameter(int index)
{
    switch (index) {
        case kframeSize:    return frameSize;
            break;
            
        default:            return 0.0f;
            break;
    }
}

void PhaseAnalyzerAudioProcessor::setParameter(int index, float newValue)
{
    switch (index)
    {
        case kframeSize:
            frameSize = (int) newValue;
            resetBuffer();
            sampleDelay = 0;
            switch (frameSize) {
                case 64:
                    frameSizeIndex = 0;
                    break;
                case 128:
                    frameSizeIndex = 1;
                    break;
                case 256:
                    frameSizeIndex = 2;
                    break;
                case 512:
                    frameSizeIndex = 3;
                    break;
                case 1024:
                    frameSizeIndex = 4;
                    break;
                case 2048:
                    frameSizeIndex = 5;
                    break;
                default:
                    break;
            }
            break;
        case khopSizeDivisor:
            hopSizeDivisor = (int) newValue;
            resetBuffer();
            break;
        case ksilenceThreshold:
            silenceThreshold = newValue;
            break;
        case kwindowType:
            windowType = (int) newValue;
            break;
        case kdelayCorrection:
            delayCorrection = (bool) newValue;
            break;
        case ktemperature:
            temp = newValue;
            break;
        default:
            break;
       
    }
}

const String PhaseAnalyzerAudioProcessor::getParameterName(int index)
{
    switch (index)
    {
        case kframeSize:    return "frame size";
            break;
        default:            break;
    }
    
    return String::empty;
}

const String PhaseAnalyzerAudioProcessor::getParameterText(int index)
{
    return String (getParameter (index), 2);
}


bool PhaseAnalyzerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PhaseAnalyzerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double PhaseAnalyzerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PhaseAnalyzerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PhaseAnalyzerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PhaseAnalyzerAudioProcessor::setCurrentProgram (int index)
{
}

const String PhaseAnalyzerAudioProcessor::getProgramName (int index)
{
    return {};
}

void PhaseAnalyzerAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void PhaseAnalyzerAudioProcessor::prepareToPlay (double sampleRate_, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    samplesAdded = 0;
    analysisBuffer_.setSize(2, analysisBufferLength_);
    analysisBuffer_.clear();
    gccBuffer.setSize(4, analysisBufferLength_);
    gccBuffer.clear();
    delayBuffer.setSize(2, analysisBufferLength_);
    delayBuffer.clear();
    sampleRate = sampleRate_;
}

void PhaseAnalyzerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PhaseAnalyzerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PhaseAnalyzerAudioProcessor::resetBuffer()
{
    analysisBuffer_.clear();
    gccBuffer.clear();
    samplesAdded = 0;
}

// mode code
std::pair<int, int> mode(int data[], unsigned int size) {
    std::unordered_map<int, int> counts;

    for (int i=0; i<size; i++) {
        if (!counts.insert({data[i],1}).second){
            counts[data[i]]++;
        }
    }
    int largestCount = 0, currentMode = 0;
    for(auto keyVal: counts){
        if(largestCount < keyVal.second){
            currentMode = keyVal.first;
            largestCount = keyVal.second;
        }
    }
    
    std::pair<int, int> modeData;
    
    modeData.first = currentMode;
    modeData.second = largestCount;
    
    return modeData;
}

int PhaseAnalyzerAudioProcessor::gccPHAT(int frame)
{
    int gccIndex;
   
    float* L      = gccBuffer.getWritePointer(0);
    float* R      = gccBuffer.getWritePointer(1);
    float* S      = gccBuffer.getWritePointer(2);
    float* absofS = gccBuffer.getWritePointer(3);

    gccIndex = 0;
    
    const float* L_analysisData = analysisBuffer_.getReadPointer(0);
    for (int i = (frame * hopSize); i < (frame * hopSize) + frameSize; i++){
        L[gccIndex] = L_analysisData[i];
        gccIndex++;
    }
    
    gccIndex = 0;
    
    const float* R_analysisData = analysisBuffer_.getReadPointer(1);
    for (int i = (frame * hopSize); i < (frame * hopSize) + frameSize; i++){
        R[gccIndex] = R_analysisData[i];
        gccIndex++;
    }
    
    int frameDelay = 0;
    
    if (gccBuffer.getRMSLevel(0, 0, frameSize) > silenceThreshold)
    {
        applyWindow(gccBuffer);
        
        // copy frameSize samples into Complex Time Domain buffers
        const float* L_gccData = gccBuffer.getReadPointer(0);
        for (int i = 0; i < frameSize; i++){
            L_td[i].r = L_gccData[i];
            L_td[i].i = 0;
 
        }

        const float* R_gccData = gccBuffer.getReadPointer(1);
        for (int i = 0; i < frameSize; i++){
            R_td[i].r = R_gccData[i];
            R_td[i].i = 0;
        }
        
        fft[frameSizeIndex].perform(L_td, L_fd);
        fft[frameSizeIndex].perform(R_td, R_fd);
        
      
        for (int i = 0; i < frameSize; i++)
        {
            //cout << sqrt(powf(L_fd[i].r, 2) + powf(L_fd[i].i, 2)) << endl;
            
            S_fd[i].r = (L_fd[i].r * R_fd[i].r) + (L_fd[i].i * R_fd[i].i);
            //cout << "L fd " << i << ": " << L_fd[i].r << endl;
            
            S_fd[i].i = (L_fd[i].i * R_fd[i].r) - (L_fd[i].r * R_fd[i].i);
            
            absofS[i] = sqrt(powf(S_fd[i].r, 2) + powf(S_fd[i].i, 2));
            
            S_fd[i].r = S_fd[i].r / absofS[i];
            S_fd[i].i = S_fd[i].i / absofS[i];

        }
        
        ifft[frameSizeIndex].perform(S_fd, S_td);    
        
        //cout << "start --------" << endl;
        
        for (int i = 0; i < frameSize; i++){
            S[i] = S_td[i].r;
            //S[i] = sqrt(powf(S_td[i].r, 2) + powf(S_td[i].i, 2));
            //cout << S[i] << endl;
        }
        
        //cout << "end ----------" << endl;
        
        // find max
        frameDelay = int(max_element(S, S + frameSize) - S);
       
        if (frameDelay > frameSize/2) {
            frameDelay = frameSize - frameDelay;
            delayedChannel = 0;
        }
        else {delayedChannel = 1;}
    }
    else {frameDelay = -1;} // return -1 if the RMS level is below the threshold
   
    return frameDelay;
}

void PhaseAnalyzerAudioProcessor::applyWindow(AudioSampleBuffer& gccBuffer)
{
    
    double multiplier;
    
    for (int channel = 0; channel < 2; channel++)
    {
        float* gccData = gccBuffer.getWritePointer(channel);
        for(int i = 0; i < frameSize; i++)
        {
            // this may be optimized by generating these vectors of difference lenghts on start up
            switch (windowType) {
                case 1: // Hann
                    multiplier = 0.5 * (1 - cos((2 * double_Pi * i)/(frameSize - 1)));
                    break;
                case 2: // Blackman-Harris
                    multiplier = 0.35875 - 0.48839 * cos((2 * double_Pi * i)/(frameSize - 1)) + 0.14128* cos((4 * double_Pi * i)/(frameSize - 1)) - 0.01168 * cos((6 * double_Pi * i)/(frameSize - 1));
                    break;
                case 3: // Hamming
                    multiplier = 0.54 - 0.46 * cos((2 * double_Pi * i)/frameSize);
                    break;
                case 4: // Rectangular
                    multiplier = 1;
                    break;
            }
            gccData[i] = gccData[i] * multiplier;
        }
    }
    
}

void PhaseAnalyzerAudioProcessor::correctDelay(AudioSampleBuffer& buffer)
{
    const int numSamples = buffer.getNumSamples();
    
    rptr = wptr - sampleDelay;
    
    if (rptr < 0){
        rptr = analysisBufferLength_ - 1 - sampleDelay;
    }
    
    // right delayed  - delay left channel
    if (delayedChannel == 0){
        
        float* left = buffer.getWritePointer(0);
        const float* delay = delayBuffer.getReadPointer(0);
        
        for (int i = 0; i < numSamples; i++){
            left[i] = delay[rptr];
            rptr++;
            if (rptr >= analysisBufferLength_){
                rptr = 0;
            }
        }
    }
    // left delayed  - delay right channel
    if (delayedChannel == 1){

        float* right = buffer.getWritePointer(1);
        const float* delay = delayBuffer.getReadPointer(1);
        
        for (int i = 0; i < numSamples; i++){
            right[i] = delay[rptr];
            rptr++;
            if (rptr >= analysisBufferLength_){
                rptr = 0;
            }
        }
    }
}

void PhaseAnalyzerAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();
    
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, numSamples);
    
    // Check if analysisBuffer has been filled, if yes run GCC-PHAT
    if (samplesAdded == analysisBufferLength_){
        
        // run GCC-PHAT here
        int f = frameSize;
        int d = hopSizeDivisor;
        
        hopSize = f/d;
        
        // number of frames to operate on
        int numFrames = (analysisBufferLength_ - frameSize)/hopSize;
        
        // array to hold delay estimations
        int frameDelay[numFrames];
        
        // iterator for frameDelay array (number of frames analyzed)
        int i = 0;

        for (int frame = 0; frame < numFrames; frame++)
        {
            int delay = gccPHAT(frame);
            if (delay != -1) {
                frameDelay[i] = delay;
                //cout << "delay" << i << " | " << frameDelay[i] << endl;
                i++;
            }
        }
        
        std::pair<int, int> analysis = mode(frameDelay, i);
        
        // only update GUI if more than one frame was above silence threshold
        if (i > 0) {
            sampleDelay = analysis.first;
            latency = ((double(sampleDelay)/sampleRate)) * 1000.0;
            speedOfSound = 331.3 * (sqrt(1 + (temp/273.15)));
            pathLength = (latency * speedOfSound)/10.0;
            
            int count = 0;
            int val = 0;
            
            for (int p = 0; p < i; p++){
                val = frameDelay[p];
                if (val == sampleDelay || val == sampleDelay - 1 || val == sampleDelay + 1){
                    count++;
                }
            }
            accuracy = floor((float(count) / float(i))*100);
        }
        resetBuffer();
    }
    
   
    // Add each channel of current block of samples into analysisBuffer
    for (int ch = 0; ch < totalNumInputChannels; ++ch)
    {
        analysisBuffer_.addFrom(ch, samplesAdded, buffer, ch, 0, numSamples);
    }
    
    // increment the number of audio samples added
    samplesAdded = samplesAdded + numSamples;
    
    // Save current samples before correcting delay
    /////////////////////////////////////////////
    int wptr_[2];
    wptr_[0] = wptr;
    wptr_[1] = wptr;
    
    for (int ch = 0; ch < totalNumInputChannels; ++ch) {
        const float* input = buffer.getReadPointer(ch);
        float* delay = delayBuffer.getWritePointer(ch);
    
        for (int i = 0; i < numSamples; i++){
            delay[wptr_[ch] + i] = input[i];
        }
    }
    //////////////////////////////////////////
    
    // Correct delay if toggle button checked
    if (delayCorrection == true && sampleDelay != 0) {correctDelay(buffer);}
    
    wptr = wptr + numSamples;
    
    if (wptr >= analysisBufferLength_){
        wptr = 0;
    }
    
}

// Eventually would like to be able to force meter updates anywhere in code
/*
void PhaseAnalyzerAudioProcessor::updateMeters(){
    
    int count = 0; // count number of frames within 1 sample delay
    int val = 0; // current sample delay
    
    for (int p = 0; p < i; p++){
        val = frameDelay[p];
        if (val == sampleDelay || val == sampleDelay - 1 || val == sampleDelay + 1){
            count++;
        }
    }
    
    latency = ((double(sampleDelay)/sampleRate)) * 1000.0;
    speedOfSound = 331.3 * (sqrt(1 + (temp/273.15)));
    pathLength = (latency * speedOfSound)/10.0;
    accuracy = floor((float(count) / float(i))*100);
    framesAnalyzed = i;
    
}
*/

//==============================================================================
bool PhaseAnalyzerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* PhaseAnalyzerAudioProcessor::createEditor()
{
    return new PhaseAnalyzerAudioProcessorEditor (*this);
}

//==============================================================================
void PhaseAnalyzerAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    XmlElement xml("PLUGINSETTINGS");
    
    xml.setAttribute("sampleDelay", sampleDelay);
    xml.setAttribute("frameSize", frameSize);
    xml.setAttribute("accuracy", accuracy);
    xml.setAttribute("framesAnalyzed", framesAnalyzed);
    xml.setAttribute("delayedChannel", delayedChannel);
    xml.setAttribute("pathLength", pathLength);
    xml.setAttribute("latency", latency);
    xml.setAttribute("hopSizeDivisor", hopSizeDivisor);
    xml.setAttribute("windowType", windowType);
    xml.setAttribute("delayCorrection", delayCorrection);
    xml.setAttribute("temp", temp);
    ;
    copyXmlToBinary(xml, destData);
}

void PhaseAnalyzerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary(data, sizeInBytes));
    
    if(xmlState != 0)
    {
        if(xmlState->hasTagName("PLUGINSETTINGS"))
        {
            sampleDelay    = xmlState->getIntAttribute("sampleDelay", sampleDelay);
            frameSize      = xmlState->getIntAttribute("frameSize", frameSize);
            accuracy       = xmlState->getIntAttribute("accuracy", accuracy);
            framesAnalyzed = xmlState->getBoolAttribute("framesAnalyzed", framesAnalyzed);
            delayedChannel = xmlState->getBoolAttribute("delayedChannel", delayedChannel);
            pathLength     = (float)xmlState->getDoubleAttribute("pathLength", pathLength);
            latency        = (float)xmlState->getDoubleAttribute("latency", latency);
            hopSizeDivisor = xmlState->getIntAttribute("hopSizeDivisor", hopSizeDivisor);
            windowType     = xmlState->getIntAttribute("windowType", windowType);
            delayCorrection = xmlState->getBoolAttribute("delayCorrection", delayCorrection);
            temp            = (float)xmlState->getDoubleAttribute("temp", temp);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PhaseAnalyzerAudioProcessor();
}
