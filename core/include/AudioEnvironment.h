#include "PlayerCommon.h"
#include "AudioChannel.h"

//using namespace std;

#ifndef AUDIOENVIRONMENT_H
#define AUDIOENVIRONMENT_H


class AudioEnvironment : public GUIElement
{
    public:
        AudioEnvironment();
        AudioEnvironment(string envType);
        AudioChannel systemNotification;
        AudioChannel calibrationSound;
        AudioChannel eegErrorSound;
        vector<AudioChannel> prizeSounds;
        AudioChannel audioChannel[AMOUNT_OF_TRAINING_MODES][AMOUNT_OF_LEVELS][AMOUNT_OF_CHANNELS];
        void playSystemNotification();
        void playCalibrationSound();
        void playEEGErrorSound();
        bool isPlayingCalibrationSound();
        int   profileMaximumForChannel[AMOUNT_OF_CHANNELS];
        float getSensitiveValueOfChannel(int chan);
        void  importPrizeAudioFiles(const char *importPath);
        void  addPrizeAudioFile(const char *importPath);
        void  modeChanged();
        void  nextLevel();
        void  previousLevel();

        string specificPrizeAudioFileName[AMOUNT_OF_CHANNELS];

        void refresh();
        void restart();
        void init();
        void play();
        void stop();
        void fadeOutCurrentLevelChannels();
        void playPrizeAudio();
        void playPrizeAudio(int graphIndex);
        virtual ~AudioEnvironment();
        double alphaF;
        double thetaF;
        double alphaVolF;
        double thetaVolF;
        double gammaVolF;
    protected:
    private:

};

#endif // AUDIOENVIRONMENT_H
