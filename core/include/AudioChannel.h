#include "PlayerCommon.h"
#ifndef AUDIOCHANNEL_H
#define AUDIOCHANNEL_H

using namespace audiere;

class AudioChannel
{
    public:
        AudioChannel();
        AudioChannel(string samplePath, float volumeP);
        void loadSample(string samplePath, float volumeF);
        void loadTone(double freq, float volumeF);
        void loadBinauralBeat(double carrierFreq, double binauralFreq, float volumeF);
        void loadMonauralBeat(double carrierFreq, double monauralFreq, float volumeF);
        void loadDoubleTone(double carrierFreq, double differeceFreq, double volumeF, double pan1, double pan2);
        void updateVolume();
        void play();
        void stop();
        void refresh();
        void setVolume(float vol);
        void stopByFadingOut();
        void init();
        void playOnce(float volP);
        string getFileNameStem();
        bool isPlayed() {
            if (played>0)
                return true;
        }
        void setPlayed(bool val) {
            if (val)
                played=true;
            else
                played=false;
        }
        bool isPlaying() {
            return audioStream->isPlaying();
        }
        virtual ~AudioChannel();
        double lastThreadFrameCountTime;
        int threadFrames;
        int threadFPS;
        double carrierFrequency;
        double differenceFrequency;
        bool ready;
    protected:
    private:
        bool loaded;
        float volume;
        string audioPath;
        string filenameStem;
        double fadeIn;
        bool fadeOut;
        int played;
        float volumeFactor;
        SampleSourcePtr audioSample;
        OutputStreamPtr audioStream;
        OutputStreamPtr audioStream2;

};

#endif // AUDIOCHANNEL_H
