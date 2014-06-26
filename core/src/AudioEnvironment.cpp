#include "../include/AudioEnvironment.h"
#include "../include/AudioChannel.h"
using namespace std;
using namespace plrCommon;
using namespace audiere;


AudioEnvironment::AudioEnvironment()
{
    //ctor
}

AudioEnvironment::~AudioEnvironment()
{
    //dtor
}


void AudioEnvironment::init() {

    // Initialize AudioEnvironment..
    prizeSounds.clear();

    // t/a-protocol special attributes.
    alphaF = 0.0d;
    thetaF = 0.0d;
    alphaVolF = 0.0d; // Alpha sound. Usually natural sound ie river, windbells, fountain or something similar.
    thetaVolF = 0.0d; // Theta binaural beat (7hz)
    gammaVolF = 0.0d; // Background mask for theta binaural beat (ocean etc.)

    // Initialize all audiochannels.
    for (int t=0; t<AMOUNT_OF_TRAINING_MODES; t++)
    {
        for (int l=0; l<AMOUNT_OF_LEVELS; l++)
        {
            for (int i=0; i<AMOUNT_OF_CHANNELS; i++)
            {
                audioChannel[t][l][i].init();
            }
        }
    }


    // System sounds.
    systemNotification.loadSample("media/sfx/system/systemNotification.mp3",1.0f);
    calibrationSound.loadSample("media/sfx/system/calibration.mp3",1.0f);
    //calibrationSound.loadSample("media/sfx/system/systemNotification.mp3",1.0f);
    eegErrorSound.loadSample("media/sfx/system/eegError.mp3",1.0f);

    // *********************************
    // ** GAMMA MEDITATION CHANNELS    *
    // *********************************
    // This one uses Monaural beat of 40Hz as Gamma-channel.
    // Alpha channel is played along with gamma-channel to make some background mask.
    specificPrizeAudioFileName[CHANNEL_GAMMA] = "tibetan_prayer_ball_low";
    // Level one
    //audioChannel[GAMMA_MEDITATION_TRAINING][LEVEL_1][CHANNEL_THETA].loadSample("media/sfx/level1/oceanwaves.wav",0.0f);
    audioChannel[GAMMA_MEDITATION_TRAINING][LEVEL_1][CHANNEL_ALPHA].loadSample("media/sfx/level1/oceanwaves.wav",0.5f);
    audioChannel[GAMMA_MEDITATION_TRAINING][LEVEL_1][CHANNEL_GAMMA].loadSample("media/sfx/waves/monaural200hz_25ms.wav",0.15f);

    // Rain level
    //audioChannel[GAMMA_MEDITATION_TRAINING][LEVEL_2][CHANNEL_THETA].loadSample("media/sfx/level1/river.mp3",0.0f);
    audioChannel[GAMMA_MEDITATION_TRAINING][LEVEL_2][CHANNEL_ALPHA].loadSample("media/sfx/level1/river.mp3",0.5f);
    audioChannel[GAMMA_MEDITATION_TRAINING][LEVEL_2][CHANNEL_GAMMA].loadSample("media/sfx/waves/monaural200hz_25ms.wav",0.15f);

    // Monks level
    //audioChannel[GAMMA_MEDITATION_TRAINING][LEVEL_3][CHANNEL_THETA].loadSample("media/sfx/level4/fountain.mp3",0.0f);
    audioChannel[GAMMA_MEDITATION_TRAINING][LEVEL_3][CHANNEL_ALPHA].loadSample("media/sfx/level4/fountain.mp3",0.5f);
    audioChannel[GAMMA_MEDITATION_TRAINING][LEVEL_3][CHANNEL_GAMMA].loadSample("media/sfx/waves/monaural200hz_25ms.wav",0.2f);

    // Fountain level
    //audioChannel[GAMMA_MEDITATION_TRAINING][LEVEL_4][CHANNEL_THETA].loadSample("media/sfx/level5/forestspringbirds.wav",0.0f);
    audioChannel[GAMMA_MEDITATION_TRAINING][LEVEL_4][CHANNEL_ALPHA].loadSample("media/sfx/level5/forestspringbirds.wav",0.5f);
    audioChannel[GAMMA_MEDITATION_TRAINING][LEVEL_4][CHANNEL_GAMMA].loadSample("media/sfx/waves/monaural200hz_25ms.wav",0.15f);

    // Windbells
    //audioChannel[GAMMA_MEDITATION_TRAINING][LEVEL_5][CHANNEL_THETA].loadSample("media/sfx/level3/windbells.mp3",0.0f);
    audioChannel[GAMMA_MEDITATION_TRAINING][LEVEL_5][CHANNEL_ALPHA].loadSample("media/sfx/level3/windbells.mp3",0.5f);
    audioChannel[GAMMA_MEDITATION_TRAINING][LEVEL_5][CHANNEL_GAMMA].loadSample("media/sfx/waves/monaural200hz_25ms.wav",0.15f);
    //audioChannel[MEDITATION_TRAINING][LEVEL_5][CHANNEL_GAMMA].loadSample("media/sfx/windbells/ritualdrumsdistant.mp3",0.3f);


    // *********************************
    // ** THETA & ALPHA TRAINING       * basically these are very similar as when it comes to sounds..^
    // *********************************
    // Gamma-channel is played as theta-increase (mask channel) as theta channel is actually reserver for the theta-binauralbeat.
    double binauralCarrierFreq = 120.0d;
    double binauralFreq        = 7.0d;
    double binauralVolume      = 0.30d;
    specificPrizeAudioFileName[CHANNEL_ALPHA] = "tibetan_prayer_ball_high";
    specificPrizeAudioFileName[CHANNEL_THETA] = "tibetan_prayer_ball_low";

    // Level one ocean waves and river
    audioChannel[THETA_ALPHA_MEDITATION_TRAINING][LEVEL_1][CHANNEL_THETA].loadBinauralBeat(binauralCarrierFreq, binauralFreq, binauralVolume);
    audioChannel[THETA_ALPHA_MEDITATION_TRAINING][LEVEL_1][CHANNEL_ALPHA].loadSample("media/sfx/level1/river.mp3",1.0f);
    audioChannel[THETA_ALPHA_MEDITATION_TRAINING][LEVEL_1][CHANNEL_GAMMA].loadSample("media/sfx/level1/oceanwaves.wav",1.0f);

    // Level two
    audioChannel[THETA_ALPHA_MEDITATION_TRAINING][LEVEL_2][CHANNEL_THETA].loadBinauralBeat(binauralCarrierFreq, binauralFreq, binauralVolume);
    audioChannel[THETA_ALPHA_MEDITATION_TRAINING][LEVEL_2][CHANNEL_ALPHA].loadSample("media/sfx/level2/rain.mp3",1.0f);
    audioChannel[THETA_ALPHA_MEDITATION_TRAINING][LEVEL_2][CHANNEL_GAMMA].loadSample("media/sfx/level2/tibetan_singing_bowls.mp3",0.8f);

    // Monks level
    audioChannel[THETA_ALPHA_MEDITATION_TRAINING][LEVEL_3][CHANNEL_THETA].loadBinauralBeat(binauralCarrierFreq, binauralFreq, binauralVolume);
    audioChannel[THETA_ALPHA_MEDITATION_TRAINING][LEVEL_3][CHANNEL_ALPHA].loadSample("media/sfx/level3/windbells.mp3",1.0f);
    audioChannel[THETA_ALPHA_MEDITATION_TRAINING][LEVEL_3][CHANNEL_GAMMA].loadSample("media/sfx/level3/resonance_of_the_gods.mp3",0.8f);

    // Fountain level
    audioChannel[THETA_ALPHA_MEDITATION_TRAINING][LEVEL_4][CHANNEL_THETA].loadBinauralBeat(binauralCarrierFreq, binauralFreq, binauralVolume);
    audioChannel[THETA_ALPHA_MEDITATION_TRAINING][LEVEL_4][CHANNEL_ALPHA].loadSample("media/sfx/level4/fountain.mp3",0.8f);
    audioChannel[THETA_ALPHA_MEDITATION_TRAINING][LEVEL_4][CHANNEL_GAMMA].loadSample("media/sfx/level4/monks.mp3",1.0f);

    // Windbells
    audioChannel[THETA_ALPHA_MEDITATION_TRAINING][LEVEL_5][CHANNEL_THETA].loadBinauralBeat(binauralCarrierFreq, binauralFreq, binauralVolume);
    audioChannel[THETA_ALPHA_MEDITATION_TRAINING][LEVEL_5][CHANNEL_ALPHA].loadSample("media/sfx/level5/forestspringbirds.wav",0.7f);
    audioChannel[THETA_ALPHA_MEDITATION_TRAINING][LEVEL_5][CHANNEL_GAMMA].loadSample("media/sfx/level5/singingbowls.mp3",0.7f);


    // Initialize prize audio.
    playerLog("Initializing prize audio..");
    importPrizeAudioFiles("media/sfx/prizes/");
    playerLog("Audioenvironment loaded.");

}

void AudioEnvironment::playSystemNotification() {
    systemNotification.playOnce(0.8f);
}

void AudioEnvironment::playCalibrationSound() {
    calibrationSound.playOnce(0.8f);
}
void AudioEnvironment::playEEGErrorSound() {
    eegErrorSound.playOnce(0.8f);
}
bool AudioEnvironment::isPlayingCalibrationSound() {
    return calibrationSound.isPlaying();
}

// Idea is to stop and play if neccessary again.. To stop the previous mode sounds and continue new mode sounds.
void AudioEnvironment::modeChanged() {
    stop();
    // If playbutton is visible.. we are stopped. dont' play ten. now it's just record button.
    if (!recordButton.isVisible())
        play();
}


void AudioEnvironment::restart() {
    for (int i=0;i<prizeSounds.size();i++) {
        prizeSounds[i].setPlayed(false);
    }
    stop();
    playerLog("Audioenv restarted..");
}

void AudioEnvironment::addPrizeAudioFile(const char *importPath) {
    AudioChannel prizeAudio(importPath,1.0f);
    prizeSounds.push_back(prizeAudio);  // Copy to vector of prize audios! :)
}

void AudioEnvironment::importPrizeAudioFiles(const char *importPath) {
    path p (importPath);   // p reads clearer than argv[1] in the following code
    try
    {
    if (exists(p))    // does p actually exist?
    {

      // is p a regular file and valid audiofile?
      if (is_regular_file(p) && isValidAudioFile(p)) {
            addPrizeAudioFile(p.string().c_str());
      }

      else if (is_directory(p))      // is p a directory?
      {
            typedef vector<path> vec;             // store paths,
            vec v;                                // so we can sort them later

            copy(directory_iterator(p), directory_iterator(), back_inserter(v));

            sort(v.begin(), v.end());             // sort, since directory iteration
            // is not ordered on some file systems

            for (vec::const_iterator it (v.begin()); it != v.end(); ++it)
            {
                importPrizeAudioFiles((*it).string().c_str()); // Recursion to import all files..
            }
      }
      else
        playerLog("No normal file.. Strange occurence.");
    }
    else
        playerLog("Audio prizes folder doesn't exist!");
    }

    catch (const filesystem_error& ex)
    {
        playerLog("Problem with listing audio files: " + string(ex.what()));
    }
}

void AudioEnvironment::play() {
    for (int i=0; i<AMOUNT_OF_CHANNELS; i++)
    {
        audioChannel[getTrainingType()][getCurrentLevel()][i].play();
    }
    playerLog("Audio Environment started.");
}

void AudioEnvironment::stop() {
    for (int t=0; t<AMOUNT_OF_TRAINING_MODES; t++) {
        for (int l=0; l<AMOUNT_OF_LEVELS; l++) {
            for (int i=0; i<AMOUNT_OF_CHANNELS; i++)
            {
                audioChannel[t][l][i].stop();
            }
        }
    }
    for (int i=0;i<prizeSounds.size();i++) {
        prizeSounds[i].stop();
    }
    playerLog("Audio Environment stopped.");
}

void AudioEnvironment::fadeOutCurrentLevelChannels() {
    for (int i=0; i<AMOUNT_OF_CHANNELS; i++)
    {
        audioChannel[getTrainingType()][getCurrentLevel()][i].stopByFadingOut();
    }
}

/* Plays randomly some prize audio. Tries to find some sound which is not played but If cannot find such audio it will play something.. */
void AudioEnvironment::playPrizeAudio() {
    if (prizeSounds.size()>0) {
        int trials=100;
        int rndIndex = 0;
        for (int i=0;i<trials;i++) {
            rndIndex = rand() % prizeSounds.size();
            if (!prizeSounds[rndIndex].isPlayed()) {
                break;
            }
        }
        prizeSounds[rndIndex].playOnce(0.15f);
    }
}

/* Plays requested prize audio if filename is found.. Used with the channel id. */
void AudioEnvironment::playPrizeAudio(int graphIndx) {
    string requestName = specificPrizeAudioFileName[graphIndx];
    if (prizeSounds.size()>0) {
        for (int i=0;i<prizeSounds.size();i++) {
            if (prizeSounds[i].getFileNameStem()==requestName) {
                prizeSounds[i].playOnce(0.15f);
            }
        }
    }
}







// Refresh AudioEnvironment.. Adjust nfb related events for audio and level logic.
void AudioEnvironment::refresh() {


    // If session is going on then adapt the environment and calculate prizes etc..
    if (isSessionOn())
    {

        // ## Volume adjustment for NFB:

        // ## Gamma-meditation training. Only gamma channel sets all channels' volume. Only alpha and gamma are now used.
        if (getTrainingType() == GAMMA_MEDITATION_TRAINING) {
            float gammaVolFactor = bwMeter.getCurrentEEGChannelSlowBarValueRelativeToMinimumAndMaximum(CHANNEL_GAMMA);
            //float thetaVolFactor = 1.0f;
            float alphaVolFactor = 1.0f;
            audioChannel[getTrainingType()][getCurrentLevel()][CHANNEL_GAMMA].setVolume( gammaVolFactor );
            audioChannel[getTrainingType()][getCurrentLevel()][CHANNEL_ALPHA].setVolume( gammaVolFactor * alphaVolFactor );
            //audioChannel[getTrainingType()][getCurrentLevel()][CHANNEL_THETA].setVolume( gammaVolFactor * thetaVolFactor );
        }

        // ## Theta training for meditative stage. individual sensitive to theta,alpha and gamma = avarage of theta and alpha channel.
        if (getTrainingType() == THETA_ALPHA_MEDITATION_TRAINING) {
            // Relative t/a.
            alphaF = bwMeter.getCurrentEEGChannelSlowBarValueRelativeToMinimumAndMaximum(CHANNEL_ALPHA1);
            thetaF = bwMeter.getCurrentEEGChannelSlowBarValueRelativeToMinimumAndMaximum(CHANNEL_THETA);
            alphaVolF = 0.0d; // Alpha sound. Usually natural sound ie river, windbells, fountain or something similar.
            thetaVolF = 0.0d; // Theta binaural beat (7hz)
            gammaVolF = 0.0d; // Background mask for theta binaural beat (ocean etc.)
            if (thetaF>alphaF) {
                thetaVolF = thetaF-alphaF;
                gammaVolF = thetaVolF;
            }
            if (alphaF>thetaF) {
                alphaVolF = alphaF-thetaF;
            }
            audioChannel[getTrainingType()][getCurrentLevel()][CHANNEL_GAMMA].setVolume( gammaVolF );
            audioChannel[getTrainingType()][getCurrentLevel()][CHANNEL_ALPHA].setVolume( alphaVolF );
            audioChannel[getTrainingType()][getCurrentLevel()][CHANNEL_THETA].setVolume( thetaVolF );
        }


    } // Session on^

    // Refresh all channels.
    for (int t=0; t<AMOUNT_OF_TRAINING_MODES; t++)
        for (int l=0; l<AMOUNT_OF_LEVELS; l++)
            for (int i=0; i<AMOUNT_OF_CHANNELS; i++)
                audioChannel[t][l][i].refresh(); // Refresh all audiochannels.

    systemNotification.refresh();
    calibrationSound.refresh();
    eegErrorSound.refresh();
}
