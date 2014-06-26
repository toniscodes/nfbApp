#include "../include/Session.h"
#include "../include/PlrFunctionality.h"

using namespace std;
using namespace plrCommon;

Session::Session()
{
    //ctor
}

Session::~Session()
{
    //dtor
}


void Session::init() {
    currentLevel         = LEVEL_1;
    prizesGot            = 0;
    lastFoundPrizeTime   = al_get_time();
    sessionOn            = false;
    automaticCalibrationCount = 0;

    nfbSessionPoint      = NFB_BEGIN_POINT;

    for (int i=0; i<AMOUNT_OF_CHANNELS; i++)
    {
        calibratedAvarages[i] = 0.0d;
    }

}



void Session::adjustLevelLogic() {

    amountOfPrizePerLevel = int(amountOfPrizesPerLevel.getConvertedValue());

    // Set mode configurations for autocalibration according training type. Calibrate only those that are important.
    bwMeter.setAutoCalibrateSlowBarChannel(CHANNEL_GAMMA,false);
    bwMeter.setAutoCalibrateSlowBarChannel(CHANNEL_THETA,false);
    bwMeter.setAutoCalibrateSlowBarChannel(CHANNEL_ALPHA1,false);

    // Then set it right.
    if (getTrainingType() == GAMMA_MEDITATION_TRAINING) {
        bwMeter.setAutoCalibrateSlowBarChannel(CHANNEL_GAMMA,true);
    }
    if (getTrainingType() == THETA_ALPHA_MEDITATION_TRAINING) {
        bwMeter.setAutoCalibrateSlowBarChannel(CHANNEL_THETA,true);
        bwMeter.setAutoCalibrateSlowBarChannel(CHANNEL_ALPHA1,true);
    }

    // ## Prize & level logic
    // Interval must be met and the threshold to gain prize.
    if (al_get_time() - lastFoundPrizeTime > timeOfNextPrizeMin.getConvertedValue()) {

        // Prize for gamma-meditation If threshold is reached in gammachannel.
        if (bwMeter.thisChannelSlowBarFoundPrize(CHANNEL_GAMMA) &&
            getTrainingType() == GAMMA_MEDITATION_TRAINING) {
                // Play prize audio!
                audioEnvironment.playPrizeAudio(CHANNEL_GAMMA);
                // Increase prize count and check for level change etc.
                increasePrizeCount();

        }

        if (getTrainingType() == THETA_ALPHA_MEDITATION_TRAINING) {

                if (audioEnvironment.alphaVolF>prizeThreshold.getConvertedValue()*0.01d) {
                    audioEnvironment.playPrizeAudio(CHANNEL_ALPHA);
                    // Increase prize count and check for level change etc.
                    increasePrizeCount();
                }

                if (audioEnvironment.thetaVolF>prizeThreshold.getConvertedValue()*0.01d) {
                    audioEnvironment.playPrizeAudio(CHANNEL_THETA);
                    // Increase prize count and check for level change etc.
                    increasePrizeCount();
                }

        }
    }
}

void Session::increasePrizeCount() {
    lastFoundPrizeTime = al_get_time();
    prizesGot++;
    playerLog("Prize! " + intToStr(prizesGot));
    // # New level reached!
    if (prizesGot>=amountOfPrizePerLevel) {
        nextLevel();
    }
}

void Session::nextLevel() {
    // Reset prizes amount again..
    prizesGot=0;
    // Fade out old sounds..
    audioEnvironment.fadeOutCurrentLevelChannels();
    // Determine level change. Purkalla nyt silleen, että ei nollaudu levelilaskuri.
    currentLevel++;
    if (currentLevel>=AMOUNT_OF_LEVELS)
        currentLevel--;
        //currentLevel=LEVEL_1;
    playerLog("Level change to " + intToStr(currentLevel+1));
    // Start playing this new one as now current level is set again.
    audioEnvironment.play();
}

void Session::previousLevel() {
    // Reset prizes amount again..
    prizesGot=0;
    // Fade out old sounds..
    audioEnvironment.fadeOutCurrentLevelChannels();
    // Determine level change.
    currentLevel--;
    if (currentLevel<0)
        currentLevel=AMOUNT_OF_LEVELS-1;
    playerLog("Level change to " + intToStr(currentLevel+1));
    // Start playing this new one as now current level is set again.
    audioEnvironment.play();
}


//! !!!!!!!!!!!!!!!!!!
// Bigger scale logic..


void Session::restart() {

    init();

    nfbSessionPoint==NFB_BEGIN_POINT;

    // Clear bw-table and for calibration etc.
    initializeBrainWaveTable();

    // Reset the brainwave readings of avg (and min and max etc).
    bwMeter.resetNFBSystem();

    // Restart audio environment. Currently not neccessary (used for randomized prize audios to make em zero counted.) This is same as stop otherwise.
    audioEnvironment.restart();
}



// This will start the session with calibration, audio and recording. But it won't start from beginning If it's just stopped.
void Session::start() {

    // Session can be only continued after idle time If sessin time is extended.
    if (nfbSessionPoint==NFB_END_POINT_IDLE_AFTER || nfbSessionPoint==NFB_PAUSED_POINT) {
        // Okay this just continues the session and closes the session in period of same time as the session was originally designed.
        nfbSessionPoint  = NFB_SESSION_POINT;
        // Start recording the actual session again and continue the neurofeedback system.
        bwManager.startRecording();
        audioEnvironment.play();
        // Note that here the problem with playing after the sesson is ended the automatic saving.
    }

    if (nfbSessionPoint==NFB_BEGIN_POINT) {
        playerLog("Actual session started..");
        // Determine point of session.
        nfbSessionPoint = NFB_CALIBRATION_POINT;
        // The recording of calibration is handled in bwMeter now mostly->
        // Here we only clear the table.
        initializeBrainWaveTable();
        // MAke sure audioenvironment with prizes etc is restarted and stopped.
        audioEnvironment.restart();
        // Start calibration.
        bwMeter.startCalibration(true);
    }


}



void Session::refresh()  {

    // Check that the openEEG device is working properly during session If we'r using openEEG. Now just cluefix.
    if (al_get_time() - openEEGDevice.lastPacketsReceivedTime > 10.0d &&
        usingOpenEEG() &&
        nfbSessionPoint==NFB_SESSION_POINT) {
        // If no packet comes to open EEG in 10 seconds stop the session!
        playerLog("No packets from OpenEEGDevice received in 10 seconds. Close, reboot EEG-device and restart to continue.");
        audioEnvironment.playEEGErrorSound();
        autoSaveSession(); // First autosave of after pause it won't work.
        pause();
    }

    if (nfbSessionPoint==NFB_CALIBRATION_POINT) {
        if (!bwMeter.isCalibrating()) {
            // Calibration done. Start session!
            nfbSessionPoint=NFB_SESSION_POINT;
            // Save bwtable of calibration.
            saveBWTable(BWFolder+"baseline.bwr");
            // Render results of baseline
            renderResultsToFile(BWFolder+"baselineRender.txt");
            // Clear bw again after calibration.
            initializeBrainWaveTable();
            // Start recording the actual session.
            bwManager.startRecording();
            // Audioenvironment should be restarted. Now it's time to play it.
            audioEnvironment.stop();
            audioEnvironment.play();
        }
    }
    if (nfbSessionPoint==NFB_SESSION_POINT) {
        sessionOn = true;
        if (bwPositionInSec > sessionLength.getConvertedValue()*60.0d) {
            nfbSessionPoint=NFB_END_POINT; // End of session. Thank you.
            playerLog("Session End.");
        }
        // Level logic. Prizes and level change.
        adjustLevelLogic();
        recTimePoint = bwPositionInSec; // Important. Tells where we were after autosave and load.
    }
    if (nfbSessionPoint==NFB_END_POINT) {
        // Do the ending stuff..
        sessionOn = false;
        endNeurofeedBackSessionInExperiment();
    }
}

void Session::endNeurofeedBackSessionInExperiment() {
    // Play end sound.
    audioEnvironment.playSystemNotification();
    pause();
    // This prevents this from runnig in circles.
    nfbSessionPoint = NFB_END_POINT_IDLE_AFTER;
    // Save the nfb bw data.
    saveBWTable(BWFolder+"session.bwr");
    // Render results of baseline
    renderResultsToFile(BWFolder+"sessionRender.txt");
    // Copy the results and render some statistics.
    saveExperiment();
}

void Session::pause() {
    sessionOn = false;
    // What happens If I pause during calibration?? Don't do it. Or I have to make more modes here.
    audioEnvironment.stop();
    bwManager.stopRecording();
    // Pause..
    nfbSessionPoint=NFB_PAUSED_POINT;
    recordButton.showAndActivate();
    recordPauseButton.hideAndUnactivate();
}


void Session::doThingsAfterAutoLoad()
{
    // Set calibration from last session.
    for (int i=0; i<AMOUNT_OF_CHANNELS; i++)
    {
        bwMeter.setSlowBarAvg(i,calibratedAvarages[i]);
    }
    bwPositionInSec    = recTimePoint; // Continue precisely from last point in time to record etc.
    lastFoundPrizeTime = 0;
    if (recTimePoint<2)
        nfbSessionPoint=NFB_BEGIN_POINT;
    else
        pause(); // Pause session If it was started so we can just continue..
}

void Session::doThingsBeforeAutoSave()
{
    // Save current calibration into autosave.
    for (int i=0; i<AMOUNT_OF_CHANNELS; i++)
    {
        calibratedAvarages[i] = bwMeter.thisChannelSlowBarAvg(i)*100.0d; // The value is 0-1 so let scale it straight to 0-100 so it will go as it should when saving.
    }

    //  rec time is already set always in session point
}

