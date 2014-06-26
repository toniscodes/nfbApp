#include "../include/AudioChannel.h"

using namespace std;
using namespace plrCommon;

AudioChannel::AudioChannel()
{
    //ctor
    init();
}

AudioChannel::AudioChannel(string samplePath, float volumeP)
{
    //ctor
    init();
    loadSample(samplePath,volumeP);
}

AudioChannel::~AudioChannel()
{
    //dtor
}


void AudioChannel::loadTone(double freq, float volumeF) {
    volumeFactor = volumeF;
    if (loaded)
        audioStream->stop();
    carrierFrequency = freq;
    audioStream  = OpenSound (device, CreateTone(freq), true);
    if (!audioStream)
    {
        playerLog("Failed to load stream! \n");
    }
    else
    {
        audioStream->setRepeat(true);
        audioStream->setVolume(0.0d);
        loaded = true;
    }
}

// Binaural beat is played in separate speakers.
void AudioChannel::loadBinauralBeat(double carrierFreq, double binauralFreq, float volumeF) {
   loadDoubleTone(carrierFreq,binauralFreq,volumeF,-1.0d,1.0d);
}

// Monaural beat is rendered to one channel from two tones.
void AudioChannel::loadMonauralBeat(double carrierFreq, double monauralFreq, float volumeF) {
   loadDoubleTone(carrierFreq,monauralFreq,volumeF,0.0d,0.0d);
}
void AudioChannel::loadDoubleTone(double carrierFreq, double differeceFreq, double volumeF, double pan1, double pan2) {

    // Stop old sound If there was any.
    if (loaded) {
        audioStream->stop();
        if (differenceFrequency>-1.0d)
            audioStream2->stop();
    }

    // Set volume and grab the frequencies.
    volumeFactor      = volumeF;
    differenceFrequency = differeceFreq;
    carrierFrequency  = carrierFreq;

    // Frequencies cannot be lower than zero.
    if (carrierFrequency-differeceFreq<0)
        carrierFrequency = differeceFreq;

    // Audiostream 1 and 2 represetngin different tones of binaural beat.
    audioStream  = OpenSound (device, CreateTone(carrierFrequency), true);
    audioStream2 = OpenSound (device, CreateTone(carrierFrequency-differeceFreq), true);

    if (!audioStream || !audioStream2)
    {
        playerLog("Failed to load stream for binaural beat! \n");
    }
    else
    {
        audioStream->setRepeat(true);
        audioStream->setVolume(0.0d);
        audioStream->setPan(pan1);
        audioStream2->setRepeat(true);
        audioStream2->setVolume(0.0d);
        audioStream2->setPan(pan2);
        loaded = true;
    }
}
void AudioChannel::loadSample(string samplePath, float volumeF)
{
    volumeFactor = volumeF;
    if (loaded)
        audioStream->stop();

    path p = path(samplePath);
    filenameStem = p.stem().string();
    audioPath = samplePath;
    string relativeAudioSrcPathStr = "";
    if (isValidAudioFile(p)) {
        path getcwd = boost::filesystem::current_path();
        relativeAudioSrcPathStr = (naive_uncomplete(p, getcwd)).string(); // Use the naive method to find relative path..

        string properPath = "";
        if ( boost::filesystem::exists( samplePath.c_str() ) )
        {
            //playerLog("Found sample audio file from absolute path!");
            properPath = samplePath.c_str();
        }
        else if ( boost::filesystem::exists( relativeAudioSrcPathStr.c_str() ) )
        {
            //playerLog("Found sample audio file from relative path!.");
            properPath = relativeAudioSrcPathStr.c_str();
        }
        if (properPath!="")
        {
            //playerLog("Opened path: " + properPath + "\n");
            audioSample = OpenSampleSource ( properPath.c_str(), FF_AUTODETECT);
        }
        else
        {
            playerLog("Couldn't found audio file from relative or absolute path: " + relativeAudioSrcPathStr);
        }
    }


    audioStream = OpenSound (device, audioSample, true); // streaming vika.--

    if (!audioStream)
    {
        playerLog("Failed to load stream! " + samplePath + " \n");
    }
    else
    {
        audioStream->setRepeat(true);
        audioStream->setVolume(0); // Volume is set to zero in the start...
        loaded = true;
    }

}

// Comparing If this is asked audio file :)
string AudioChannel::getFileNameStem() {
    return filenameStem;
}

void AudioChannel::init()
{
    loaded=false;
    carrierFrequency=-1.0d;
    differenceFrequency=-1.0d;
    played=0;
    fadeOut = false;
}

// Play audio once without fade in.
void AudioChannel::playOnce(float volP)
{
    audioStream->setRepeat(false);
    fadeIn = 1.0d;
    setVolume(volP);
    play();
}

// Start playing audio channel with fade in.
void AudioChannel::play()
{
    if (loaded) {
        played++;
        fadeIn = 0.0d;
        audioStream->play();
        if (differenceFrequency>-1.0d)
            audioStream2->play();
        fadeOut = false;
    }
}

void AudioChannel::refresh()
{
    if (loaded) {
        if (fadeOut) {
            setVolume(volume-deltaTime*0.05f);
            if (volume<=0.0f)
                stop();
        }

        // Update volume also to have the ability to control single audio volumes via volume rollbutton.
        updateVolume();

    }
}

void AudioChannel::stopByFadingOut()
{
    if (loaded) {
        fadeOut=true;
    }
}

void AudioChannel::stop()
{
    if (loaded)
    {
        audioStream->stop();
        if (differenceFrequency>-1.0d)
            audioStream2->stop();
    }
}

void AudioChannel::setVolume(float volP)
{
    if (volP<0.0f)
        volP = 0.0f;
    if (volP>1.0f)
        volP = 1.0f;
    volume = volP;
    updateVolume();
}

void AudioChannel::updateVolume() {
    if (loaded)
    {
        fadeIn += deltaTime;
        if (fadeIn<0.0d)
            fadeIn = 0.0d;
        if (fadeIn>1.0d)
            fadeIn = 1.0d;

        if (volumeFactor>0)
        {
            audioStream->setVolume(volume*volumeFactor*guiVolumeRollButton.getValue()*fadeIn); // rollbutton defines the main volume of app anyway..
            if (differenceFrequency>-1.0d)
                audioStream2->setVolume(volume*volumeFactor*guiVolumeRollButton.getValue()*fadeIn);
        } else
        { // Complement IF it's under zero. This is for making negative training. To make volume go opposite. ie. training for less beta to achieve relaxation.
            audioStream->setVolume(-(1.0f-volume)*volumeFactor*guiVolumeRollButton.getValue()*fadeIn);
            if (differenceFrequency>-1.0d)
                audioStream2->setVolume(-(1.0f-volume)*volumeFactor*guiVolumeRollButton.getValue()*fadeIn);
        }
    }
}
