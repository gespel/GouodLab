/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "GouodLib/StepSequencer.h"
#include "GouodLib/Synths.h"
#include <memory>
#include "GouodLib/CommunicationServer.h"
#include "GouodLib/Instruments.h"
#include "GouodLib/MidiEventParser.h"
#include "GouodLib/Instruments/LichtenKick.h"

//==============================================================================
/**
*/
class GouodLabAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    GouodLabAudioProcessor();
    ~GouodLabAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    std::unique_ptr<FMSynth> fs;
    //std::unique_ptr<CommunicationServer> cs;
    std::unique_ptr<StrangeOrgan> o;
    std::unique_ptr<StrangeOrgan> o2;
    StepSequencer *ss;
    //ServerThread st;
    GKick *gk;
    GHat *gh;
    int counter = 0;
    SineSynth *a;

    SineSynth *bs;
    PadDrone *pd;
    MegaDrone *md;
    
    MidiEventParser *mp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GouodLabAudioProcessor)
};
