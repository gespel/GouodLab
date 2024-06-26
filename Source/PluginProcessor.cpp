/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GouodLabAudioProcessor::GouodLabAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

GouodLabAudioProcessor::~GouodLabAudioProcessor()
{
}

//==============================================================================
const juce::String GouodLabAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GouodLabAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GouodLabAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GouodLabAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GouodLabAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GouodLabAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GouodLabAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GouodLabAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GouodLabAudioProcessor::getProgramName (int index)
{
    return {};
}

void GouodLabAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GouodLabAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    md = new MegaDrone(55, 500, sampleRate);
    a = new SineSynth(5, sampleRate);
    gk = new GKick(110, sampleRate);
    gh = new GHat(110, sampleRate);
    pd = new PadDrone(220.f, sampleRate);
    //this->cs = std::make_unique<CommunicationServer>();
    //this->ss = new StepSequencer((float)sampleRate, {1.f, 1.f + 5.f/12.f, 1.f + 8.f/12.f, 2.f});
    this->ss = new StepSequencer((float)sampleRate, {1, 3, 2, 0.5, 5});
    this->ss->setSpeed(1);
    this->bs = new SineSynth(0.03, sampleRate);
    this->o = std::make_unique<StrangeOrgan>(440.f, sampleRate);
    this->o2 = std::make_unique<StrangeOrgan>(440.f * (1.f + 5.f/12.f), sampleRate);
    
    //st.startThread();
    fs = std::make_unique<FMSynth>(440.f, sampleRate);
    fs->setModulatorFrequency(200.f);
    this->mp = new MidiEventParser(sampleRate);

}

void GouodLabAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GouodLabAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void GouodLabAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    auto cL = buffer.getWritePointer(0);
    auto cR = buffer.getWritePointer(1);
    //this->mp->handle(&midiMessages);

    for(int sample = 0; sample < buffer.getNumSamples(); sample++) {
        //auto sL = std::get<0>(this->o->getSample()) + std::get<0>(this->o2->getSample())/2;
        //auto sR = std::get<1>(this->o->getSample()) + std::get<1>(this->o2->getSample())/2;
        auto samp = md->getSample();
        
        //auto samp = gk->getSample();
        //auto sL = samp;
        //auto sR = samp;
        auto sL = std::get<0>(samp);
        auto sR = std::get<1>(samp);
        cL[sample] = sL;
        cR[sample] = sR;
        auto f = a->getSample()*1000;
        this->o2->setModulatorFrequency(f);
        this->o->setModulatorFrequency(f);
        auto ssample = this->ss->getSample();
        this->pd->setFrequency(ssample*220);
        //this->o->setFrequency(ssample * 110);
        //this->o2->setFrequency(ssample * 110 * (1.f + 5.f/12.f));
        //fs->setModulatorFrequency(200.f*bs->getSample() + 20);
        counter ++;
        if(counter >= 40000) {
            gk->trigger();
            counter = 0;
        }
        md->setFrequency(ss->getSample() * 110.f);

    }
}

//==============================================================================
bool GouodLabAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GouodLabAudioProcessor::createEditor()
{
    return new GouodLabAudioProcessorEditor (*this);
}

//==============================================================================
void GouodLabAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GouodLabAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GouodLabAudioProcessor();
}
