/********************************************************************************
* Meter to show the current state of Brain Computer Interface -device and some  *
* Nice graph of the EEG.                                                        *
*                                                                               *
* I guess this is used for only reading RAW Values.. Later perhaps this will be *
* the interface for all BCI devices before taking data to graphs. So the data   *
* would be handled here a bit already..                                         *
*                                                                               *
* !Update. Now it supports OpenEEG (5.2014)                                     *
*                                                                               *
* Additionally:                                                                 *
* Modified for NFB program now..                                                *
********************************************************************************/
#include "../include/BWMeter.h"
#include "../include/thinkgear.h"
#include "../include/PlayerCommon.h"

using namespace std;
using namespace plrCommon;

// Slow bar holds the information of current state of one channel. It's slow because it's smoothed and not real time accurate absolutely.
// This is why it's good for nfb .. It also captures the maximum values of each channel.
SlowBar::SlowBar()
{
    reset();
}

void SlowBar::refresh(double goToV)
{
    foundPrize=false;
    goToValue = goToV;

    // Calculate the actual bar value from avarage of few calculations.

    currentAvarageCount++;
    currentAvarageSum+=goToV;

    // Update the current avarage during session. It's updated interval of same time of calibration for baseline.
    if (al_get_time()-lastTimeCurrentAvgCalc>(calibrationTime.getConvertedValue()) ) {
        lastTimeCurrentAvgCalc = al_get_time();
        // Autocalibrate only in session.. So it's done in intervals If session is on.
        if (isSessionOn()) {
            currentAvarage=currentAvarageSum/currentAvarageCount;
            currentAvarageCount=0;
            currentAvarageSum=0;
            // When user has got stuck with this channel. The channel will calibrate itself.
            if (currentAvarage < avgValue-minimumPlace.getConvertedValue() && autoCalibrate ) {
                avgDestValue=(currentAvarage+avgValue)/2.0d; // Move the avarage in the middle of respectively last set avarage value and this new one.
                playerLog("Automatic calibration occured (decreased) for channel " + getGraphTheme(type).text);
                session.automaticCalibrationCount++;
            }
            // if (currentAvarage > avgValue + maximumPlace.getConvertedValue() * 1.15d && autoCalibrate ) {
            if (currentAvarage > avgValue + maximumPlace.getConvertedValue() && autoCalibrate ) {
                avgDestValue=(currentAvarage+avgValue)/2.0d; // Move the avarage in the middle of respectively last set avarage value and this new one.
                playerLog("Automatic calibration occured (increased) for channel " + getGraphTheme(type).text);
                session.automaticCalibrationCount++;
            }
        }
    }

    double timeFactor = deltaTime;
    double distToGoTo = getPositiveOfThis(goToValue-currentValue);

    if (goToValue>currentValue)
        speed = distToGoTo*0.1f;
    if (goToValue<currentValue)
        speed = -distToGoTo*0.1f;
    if (speed>maxSpeed)
        speed = maxSpeed;
    if (speed<-maxSpeed)
        speed = -maxSpeed;
        // This could be made more beatiful taking the positive of ^ and replacing some if clauses etc.
    //currentValue += (goToValue-currentValue)*0.1d*deltaTime; ie this could resolve this whole thing <-
    currentValue += speed*timeFactor;
    if (currentValue<0)
        currentValue=0;
    if (currentValue>100)
        currentValue=100;

    // Are we searching for new avarage value? This is for manual calibration.
    if (searchForAvg) {
        avgAddedCount++;
        avgSum+=goToV;
    }

    // Move avarage slowly to the calibration given number not to cause sudden jumps in volume.
    double distToGoToAvg = getPositiveOfThis(avgDestValue-avgValue);
    if (avgValue>avgDestValue)
        avgValue -= deltaTime*distToGoToAvg*0.1d; // Move there in one second.
    if (avgValue<avgDestValue)
        avgValue += deltaTime*distToGoToAvg*0.1d; // Move there in one second.

    // Determine minimum and maximum all the time. If it's changed it will be updated..
    minimum = avgValue-minimumPlace.getConvertedValue();
    if (minimum<0)
        minimum=0;
    maximum = avgValue+maximumPlace.getConvertedValue();
    if (maximum>100)
        minimum=100;

    // Found prize.
    if (currentValue>minimum+( (maximum-minimum) * prizeThreshold.getConvertedValue() * 0.01f ) )
        foundPrize=true;

}

// This is for specifying the type also.
void SlowBar::reset(int typeP) {
    reset();
    type = typeP;
}

void SlowBar::setAutoCalibrate(bool aCalibrate) {
    autoCalibrate = aCalibrate;
}

void SlowBar::reset() {
    type = 0;
    goToValue = 0;
    currentValue = 0;
    speed = 0.0f;
    maxSpeed = 10.0f;
    maximum = 100.0f;
    minimum = 10.0f;
    autoCalibrate = false;
    searchForAvg=false;
    avgSum=0;
    avgAddedCount=0;
    avgValue=0;
    avgDestValue=0;
    foundPrize=false;
    // Search for current avarage value.
    lastTimeCurrentAvgCalc=0;
    currentAvarage=0;
    currentAvarageSum=0;
    currentAvarageCount=0;
}

BWMeter::BWMeter()
{
    //ctor
    reset();
}

BWMeter::BWMeter(float px, float py, string toolTipText)
{
    //ctor
    reset();
    x = px;
    y = py;
    GUIElement::setToolTipArea(0,0,0,0,toolTipText);
}

BWMeter::~BWMeter()
{
    //dtor
}

/*
* Init()
*:::::::::::::
*
* Initializes pictures for the meter and other stuff ie. MindWave and devices where data is imported.
*
*/
void BWMeter::init()
{
    playerLog("Initializing BWMeter..");
    bwMeterBG   = imageLibrary.loadImage("media/gfx/BWMeterBg.png");

    mindWaveImg = imageLibrary.loadImage("media/gfx/MindWave.png");

    circleImg   = imageLibrary.loadImage("media/gfx/circle.png");

    GUIElement::setToolTipAreaOnly(x,y,al_get_bitmap_width(bwMeterBG),al_get_bitmap_height(bwMeterBG));
}


/* Reset Brainwave Meter */
void BWMeter::reset()
{
    playerLog("Resetting BrainWave Meter..");
    METER_SAMPLE_SIZE = 500;
    recordEEGForCalibration=false;
    meterValues.clear();
    lastBWUpdate = 0;
    for (int i=0; i<AMOUNT_OF_CHANNELS; i++)
    {
        channelCurrentValue[i] = 0;
        channelSlowBar[i].reset(i);
    }

    calibrationStartTimeForEEG = -1;
    calibrationStartTimeForVolume = -1;

    rawMeterMultipliesBWMeter = 3; // This tells how many units of data is in one pixel width in raw.. for example 5 means 5 different new y-positions of the graph inside 1 real pixel.
    // This is good. It shows the data more slowly and thicker thand drawing update per pixel....


}


void BWMeter::resetNFBSystem() {
    for (int i=0; i<AMOUNT_OF_CHANNELS; i++)
    {
        channelSlowBar[i].reset(i);
    }
}

/* Returns the newest value taken from device 0-100. */
double BWMeter::getCurrentEEGChannelValue(int graphChannelId)
{
    if (useDebugInput) {
        return getRandomZeroToOne()*100000;
    }
    double chanValueToReturn = channelCurrentValue[graphChannelId];
    if (chanValueToReturn == CHANNEL_EMPTY_INDEX) {
        playerLog("For some reasong the lottery ticket was won.."); // This is to make sure that empty index cannot really never be given from device's input :)
        // If it was the smallest in number.. increase by one and return.. :)
        chanValueToReturn++;
    }
    return float(chanValueToReturn);
}


/* Sets the slow bar avarage ie. after autoload straightly to the correct position. */
void BWMeter::setSlowBarAvg(int graphChannelId, double valP) {
    channelSlowBar[graphChannelId].setAvarage(valP);
}

/* Returns the current newest slow value as scaled to 0 - 1.0f */
double BWMeter::getCurrentEEGChannelSlowBarValue(int graphChannelId)
{
    return channelSlowBar[graphChannelId].getCurrentValue()/100.0f;
}

void BWMeter::setAutoCalibrateSlowBarChannel(int graphChannelId, bool valP) {
    channelSlowBar[graphChannelId].setAutoCalibrate(valP);
}

/*  Return the relative value of minimum to maximum. */
double BWMeter::getCurrentEEGChannelSlowBarValueRelativeToMinimumAndMaximum(int graphChannelId)
{
    if (channelSlowBar[graphChannelId].getMaximumValue()!=0.0f) {
        double wholeDeltaMinToMax = channelSlowBar[graphChannelId].getMaximumValue() - channelSlowBar[graphChannelId].getMinimumValue();
        double currentBetween = channelSlowBar[graphChannelId].getCurrentValue() - channelSlowBar[graphChannelId].getMinimumValue();
        double relVal = (currentBetween/wholeDeltaMinToMax);
        if (relVal<0.0f)
            relVal=0.0f;
        if (relVal>1.0f)
            relVal=1.0f;
        return relVal;
    } else
        return 0.0f; // Prevent division by zero..
}

/* Returns the current newest value as relative to the maximum from 0 - 1.0f */
double BWMeter::getCurrentEEGChannelSlowBarValueRelativeToMaximum(int graphChannelId)
{
    if (channelSlowBar[graphChannelId].getMaximumValue()!=0.0f) {
        //playerLog("cur: " + desToStr(channelSlowBar[graphChannelId].getCurrentValue()) + " max " + desToStr(channelSlowBar[graphChannelId].getMaximumValue()));
        double relVal = (channelSlowBar[graphChannelId].getCurrentValue()/channelSlowBar[graphChannelId].getMaximumValue());
        if (relVal<0.0f)
            relVal=0.0f;
        if (relVal>1.0f)
            relVal=1.0f;
        return relVal;
    } else
        return 0.0f; // Prevent division by zero..
}

/* Returns true, if asked channel just found new maximum last frame. */
bool BWMeter::thisChannelSlowBarFoundPrize(int graphChannelId) {
    return channelSlowBar[graphChannelId].getFoundPrize();
}

/* Returns the maximum of this slowbar. */
double BWMeter::thisChannelSlowBarMaximum(int graphChannelId) {
    return channelSlowBar[graphChannelId].getMaximumValue()/100.0f;
}

/* Returns the maximum of this slowbar. */
double BWMeter::thisChannelSlowBarMinimum(int graphChannelId) {
    return channelSlowBar[graphChannelId].getMinimumValue()/100.0f;
}

double BWMeter::thisChannelSlowBarAvg(int graphChannelId) {
    return channelSlowBar[graphChannelId].getAvg()/100.0f;
}

/* Update channel data of given id and value */
void BWMeter::updateChannelData(int graphId, double value)
{
    // Do we have new information or not?
    channelCurrentValue[graphId] = value; // Updated!
}


void BWMeter::startCalibration(bool recordEEGForCalib) {
    recordEEGForCalibration = recordEEGForCalib;
    startVolumeCalibration(); // VOlume calibration and it will automatically start EEG after volume is set.
}


void BWMeter::startVolumeCalibration()
{
    calibrationStartTimeForVolume = al_get_time();
    playerLog("Calibration for volume..");
    audioEnvironment.playCalibrationSound();
}

void BWMeter::startEEGCalibration()
{
    // Start recording EEG for baseline.
    if (recordEEGForCalibration)
        bwManager.startRecording();
    session.automaticCalibrationCount = 0;
    // ..
    calibrationStartTimeForEEG = al_get_time();
    for (int i=0; i<AMOUNT_OF_CHANNELS; i++)
        channelSlowBar[i].startCalibration();
    playerLog("Calibration for EEG started..");
    audioEnvironment.playSystemNotification();
}


bool BWMeter::EEGDeviceIsWorking()  {
    if (mindWaveDevice.isWorkingProperly() || openEEGDevice.isWorkingProperly())
        return true;
    else
        return false;
}

/*
* refresh()
*:::::::::::::
*
* Refresh the Brainwave Meter and it's all channels with information offered by bci devices.
*
*/
void BWMeter::refresh()
{
    // Check first If we'r calibrating the volume
    if ( (al_get_time()-calibrationStartTimeForVolume>60.0d || !audioEnvironment.isPlayingCalibrationSound()) && calibrationStartTimeForVolume>0) {
        calibrationStartTimeForVolume=-1; // Turn off calibration.
        playerLog("Calibration end for volume.");
        startEEGCalibration();
    }
    // Check first If we'r calibrating the slowbars
    if (al_get_time()-calibrationStartTimeForEEG>calibrationTime.getConvertedValue() && calibrationStartTimeForEEG>0) {
        calibrationStartTimeForEEG=-1; // Turn off calibration.
            for (int i=0; i<AMOUNT_OF_CHANNELS; i++)
                channelSlowBar[i].stopCalibration();
        playerLog("Calibration end for EEG.");
        audioEnvironment.playSystemNotification();
    }

    //timeFromLastUpdate = double((std::clock()-lastBWUpdate)/((double)CLOCKS_PER_SEC));
    timeFromLastUpdate = al_get_time()-lastBWUpdate;
    if (timeFromLastUpdate > 0.004)   // Update every 5ms max? 0.005 would.. now it's almost limitless..
    {

        // Reading MindWave If chosen..
        if (usingMindWave()) {


            // Update the bwmeter raw signal.. the classical EEG-view.
            meterValues.insert ( meterValues.begin() , mindWaveDevice.getRawValue() );
            //meterValues.insert ( meterValues.begin() , cos(al_get_time()*10.0d)*1000.0d );
            if (meterValues.size()>al_get_bitmap_width(bwMeterBG)*rawMeterMultipliesBWMeter)
                meterValues.pop_back(); // Keep the sample size tidy.

            // Read packets etc.
            mindWaveDevice.readPackets();
            for (int i=0;i<AMOUNT_OF_CHANNELS;i++) {
                updateChannelData(i, mindWaveDevice.getValueOf(i));
            }


        }

        // OpenEEGDevice spectrums from device handler If chosen..
        if (usingOpenEEG()) {
            // Loop through all channels and make calculate avarage of two channels IF it's the option to do so.
            for (int i=0;i<AMOUNT_OF_CHANNELS;i++)
            {
                // How many channels is selected to be used= 1 or 2?
                if (howManyChannelsInOpenEEG()==1)
                    updateChannelData(i, openEEGDevice.getSpectrumAmplitudeOf(0,i) );
                else
                    updateChannelData(i, (openEEGDevice.getSpectrumAmplitudeOf(0,i) + openEEGDevice.getSpectrumAmplitudeOf(1,i)) / 2.0d );
            }
        }

        lastBWUpdate = al_get_time();

    }

    // Then calculate avarage of power amplitudes and avarage or just the dominating frequency. This should be changed to use weighted avarage brobably.
    float realAvarageSum=0;
    float realAvarageCount=0;
    float avgV = 0.1f;
    float currentBiggest;
    for (int i=0; i<AMOUNT_OF_CHANNELS; i++)
    {
        if (useThisTypeForAvarageCalculation(i)) {
            realAvarageCount++;
            realAvarageSum+=getCurrentEEGChannelValue(i);
            if (getCurrentEEGChannelValue(i)>currentBiggest)   // compare 0-100 to 0-100
            {
                currentBiggest    = getCurrentEEGChannelValue(i); // 0-100
                avgV              = getThisTypeCenterFreq(i); // collect the frequency center.. ..
            }
        }
    }
    if (realAvarageCount!=0) {
        bwMeter.updateChannelData(CHANNEL_AVG, avgV); // Update also bwmeter value.
        bwMeter.updateChannelData(CHANNEL_POWER_AVG, realAvarageSum/realAvarageCount); // Update also bwmeter value.
    }

    // Refresh all smooth bars
    for (int i=0; i<AMOUNT_OF_CHANNELS; i++)
    {
        channelSlowBar[i].refresh(getCurrentEEGChannelValue(i));
    }

}

/* Draw the BrainWave Meter. Background, emotionicons etc.. */
void BWMeter::draw()
{
    float bAlpha = 0.6f;
    drawTintedPicture(bwMeterBG, al_map_rgba_f(1.0f*bAlpha, 1.0f*bAlpha, 1.0f*bAlpha, bAlpha), x, y, 0);

    float rawAlpha = 0.35f;
    float boarderSize = 5;
    if (EEGDeviceModeRadioButton.getSelectionTxt()==EEGDevicegOp1.getText()) {
     for (int ix = boarderSize*rawMeterMultipliesBWMeter ; ix < al_get_bitmap_width(bwMeterBG)*rawMeterMultipliesBWMeter - boarderSize*rawMeterMultipliesBWMeter; ix++ )
     {
        // Meter values.. Meter means the raw value
        // Draw MindWave.
            if (ix+1<meterValues.size())
            {
                al_draw_line(x+al_get_bitmap_width(bwMeterBG)-ix/rawMeterMultipliesBWMeter, y+al_get_bitmap_height(bwMeterBG)/2+((meterValues.at(ix) / 1000.0f) * (al_get_bitmap_height(bwMeterBG)/2.0f) ),
                             x+al_get_bitmap_width(bwMeterBG)-(ix/rawMeterMultipliesBWMeter+1), y+al_get_bitmap_height(bwMeterBG)/2+((meterValues.at(ix+1) / 1000.0f) * (al_get_bitmap_height(bwMeterBG)/2.0f) ),
                             al_map_rgba_f(1.0*bAlpha*rawAlpha, 1.0*bAlpha*rawAlpha, 1.0*bAlpha*rawAlpha, bAlpha*rawAlpha), 2.0f);
            }
     }
    }

    // Draw open eeg meter.
    if (EEGDeviceModeRadioButton.getSelectionTxt()==EEGDevicegOp2.getText() || EEGDeviceModeRadioButton.getSelectionTxt()==EEGDevicegOp3.getText()) {
        double chan2YAdd = al_get_bitmap_height(bwMeterBG)/2.0d;
        for (int ix = 0 ; ix < al_get_bitmap_width(bwMeterBG); ix++ )
        {
            // First channel of Open EEG:
            double valA = openEEGDevice.getRawChannelValueOfTimePoint(OPEN_EEG_RAW_CHAN1, double(ix)   / al_get_bitmap_width(bwMeterBG));
            double valB = openEEGDevice.getRawChannelValueOfTimePoint(OPEN_EEG_RAW_CHAN1, double(ix+1) / al_get_bitmap_width(bwMeterBG));
            al_draw_line(x+ix,   y+al_get_bitmap_height(bwMeterBG)/2+((valA) * ((double)al_get_bitmap_height(bwMeterBG)/2.0f) ),
                         x+ix+1, y+al_get_bitmap_height(bwMeterBG)/2+((valB) * ((double)al_get_bitmap_height(bwMeterBG)/2.0f) ),
                         al_map_rgba_f(1.0*bAlpha*rawAlpha, 1.0*bAlpha*rawAlpha, 1.0*bAlpha*rawAlpha, bAlpha*rawAlpha), 2.0f);

            // Second channel of Open EEG If chosen:
            if (EEGDeviceModeRadioButton.getSelectionTxt()==EEGDevicegOp3.getText()) {
                double valA = openEEGDevice.getRawChannelValueOfTimePoint(OPEN_EEG_RAW_CHAN2, double(ix)   / al_get_bitmap_width(bwMeterBG));
                double valB = openEEGDevice.getRawChannelValueOfTimePoint(OPEN_EEG_RAW_CHAN2, double(ix+1) / al_get_bitmap_width(bwMeterBG));
                al_draw_line(x+ix,   y+al_get_bitmap_height(bwMeterBG)/2+((valA) * ((double)al_get_bitmap_height(bwMeterBG)/2.0f) ) + chan2YAdd,
                             x+ix+1, y+al_get_bitmap_height(bwMeterBG)/2+((valB) * ((double)al_get_bitmap_height(bwMeterBG)/2.0f) ) + chan2YAdd,
                             al_map_rgba_f(1.0*bAlpha*rawAlpha, 1.0*bAlpha*rawAlpha, 1.0*bAlpha*rawAlpha, bAlpha*rawAlpha), 2.0f);
            }
        }
    }


    // Draw the mindwave on top also
    drawTintedPicture(mindWaveImg, al_map_rgba_f(1.0f*bAlpha, 1.0f*bAlpha, 1.0f*bAlpha, bAlpha),  x + al_get_bitmap_width(bwMeterBG) - al_get_bitmap_width(mindWaveImg)*0.75f , y-al_get_bitmap_height(mindWaveImg)*0.15f, 0);

    // Draw the power meter to show signal strength or condition..
    float powerMeterX = x + al_get_bitmap_width(bwMeterBG) + 20;
    float powerMeterY = y + al_get_bitmap_height(bwMeterBG)*0.95f;
    drawPowerMeter(powerMeterX, powerMeterY , al_get_bitmap_width(bwMeterBG)*0.3f, al_get_bitmap_height(bwMeterBG)/3,mindWaveDevice.getSignalStrength());

    float alpha = 0.7f;
    ALLEGRO_COLOR bFontColor  = al_map_rgba_f(0.9*alpha, 0.9*alpha, 1.0*alpha, alpha); // <-- correct
    al_draw_text(buttonFont, bFontColor, powerMeterX-70, powerMeterY + 5, NULL, "Signal Strength");

    GUIElement::draw();
}



