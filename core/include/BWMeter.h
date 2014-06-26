//#include "PlayerCommon.h"
#include "GUI/GUIElement.h"
#include "ImageLibrary.h"

#ifndef BWMETER_H
#define BWMETER_H

// These are used for getting values to basic graph
enum BW_CHANNELS {  CHANNEL_DELTA,
                       CHANNEL_THETA,
                       CHANNEL_ALPHA,
                       CHANNEL_ALPHA1,
                       CHANNEL_ALPHA2,
                       CHANNEL_BETA,
                       CHANNEL_BETA1,
                       CHANNEL_BETA2,
                       CHANNEL_GAMMA,
                       CHANNEL_GAMMA1,
                       CHANNEL_GAMMA2,
                       CHANNEL_AVG,
                       CHANNEL_ATT,
                       CHANNEL_MED,
                       CHANNEL_POWER_AVG,
                    AMOUNT_OF_CHANNELS
                  }; // Notice that the last one tells the amount of these things..



/**
 * \brief BWMeter.h ...:::Brainwave Meter As Virtual Input Device:::...
*
* This class is supposed to be a virtual offerer for all BCI data that is brought into the application. It acts as an 'interface'.
* In other words all BCI data is brought and processed first here. This can be safely used in every part of the application to gather
* realible data.
*
* author: tonis @ 2013
*
*/
class SlowBar {
   public:
    SlowBar();
    double getCurrentValue() {
        return currentValue;
    }
    double getMaximumValue() {
        return maximum;
    }
    double getMinimumValue() {
        return minimum;
    }
    bool getFoundPrize() {
        return foundPrize;
    }
    double getAvg() {
        return avgValue;
    }
    void setAvarage(double valP) {
        avgDestValue = valP;
    }
    void startCalibration() {
        searchForAvg=true;
        avgSum=0;
        avgValue=0;
        avgDestValue=0;
        avgAddedCount=0;
    }
    void stopCalibration() {
        searchForAvg=false;
        avgDestValue = avgSum/((double)avgAddedCount);
        //avgValue = avgSum/((double)avgAddedCount);
    }

    void setAutoCalibrate(bool aCalibrate);
    void refresh(double goToV);
    void reset(int typeP);
    void reset();

private:
    int    avgAddedCount;
    int    currentAvarageCount;
    int    type;
    double goToValue;
    double currentValue;
    double speed;
    double maxSpeed;
    double maximum;
    double minimum;
    double avgSum;
    double avgValue;
    double avgDestValue;
    double currentAvarage;
    double currentAvarageSum;
    double lastTimeCurrentAvgCalc;
    bool   searchForAvg;
    bool   foundPrize;
    bool   autoCalibrate;
};


class BWMeter : public GUIElement
{
public:
    BWMeter();
    BWMeter(float px, float py, string toolTipText);
    void reset();
    void resetNFBSystem();
    void refresh();
    void draw();
    void init();
    bool EEGDeviceIsWorking();
    void multiplySlowBarMaximums(double multiplier);
    void multiplySlowBarMinimums(double multiplier);
    double  channelCurrentValue[AMOUNT_OF_CHANNELS];
    SlowBar channelSlowBar[AMOUNT_OF_CHANNELS];

    double getCurrentEEGChannelValue(int graphChannelId);
    void  updateChannelData(int graphId, double valToUpdate);

    double getCurrentEEGChannelSlowBarValue(int graphChannelId);
    double getCurrentEEGChannelSlowBarValueRelativeToMaximum(int graphChannelId);
    double getCurrentEEGChannelSlowBarValueRelativeToMinimumAndMaximum(int graphChannelId);
    bool   thisChannelSlowBarFoundPrize(int graphChannelId);
    double thisChannelSlowBarMaximum(int graphChannelId);
    double thisChannelSlowBarMinimum(int graphChannelId);
    double thisChannelSlowBarAvg(int graphChannelId);
    void setAutoCalibrateSlowBarChannel(int graphChannelId, bool valP);
    void setSlowBarAvg(int graphChannelId, double valP);

    void startVolumeCalibration();
    void startEEGCalibration();
    void startCalibration(bool recordEEGForCalib);

    bool isCalibrating() {
        if (calibrationStartTimeForEEG>0 || calibrationStartTimeForVolume>0)
            return true;
        else
            return false;
    }

    virtual ~BWMeter();
protected:
private:
    float x,y;
    vector<double> meterValues;
    double rawMeterMultipliesBWMeter;
    double calibrationStartTimeForVolume;
    double calibrationStartTimeForEEG;
    double timeFromLastUpdate;
    double lastBWUpdate;
    bool   recordEEGForCalibration;
    int    METER_SAMPLE_SIZE;
    ALLEGRO_BITMAP *bwMeterBG;
    ALLEGRO_BITMAP *circleImg;
    ALLEGRO_BITMAP *mindWaveImg;

};

#endif // BWMETER_H
