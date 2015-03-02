#include "../include/BWManager.h"
#include "../include/thinkgear.h"
#include "../include/PlayerCommon.h"


using namespace plrCommon;

BWManager::BWManager(string s, int posX, int posY, int width, int height, string toolTipText)
{
    reset();
    graphUIX = posX;
    graphUIY = posY;
    graphWidth = width;
    graphHeight = height;
    GUIElement::setToolTipArea(posX,posY,CHANNEL_SIZE_X,CHANNEL_SIZE_Y*8,toolTipText);
}

BWManager::BWManager()
{
    //ctor
    reset();
}

/*
* Reset()
*::::::::
*
* This method just resets some basic stuff and load MindWave(it shoold be actually loaded already some-elsewhere.)
*
*/
void BWManager::reset()
{
    visibilityShade=0.0f;
    graphUIX = 0;
    graphUIY = 0;
}

// Stops recording of brainwaves
void BWManager::stopRecording()
{
    recording = false;
}

// Starts recording of brainwaves
void BWManager::startRecording()
{
    recording = true;
    lastGraphUpdate = int( std::clock() );
}

/*
* newBWTable(string songId,int songLengthInSecs)
*::::::::
*
* This method just creates all the graphs of this song. This seems to be used only when the process starts for new song. Otherwise the graphs are loaded from serialization system.
*
*
*/
void BWManager::newBWTable()
{
        // Expand the raw channel sizes first.
        rawValues.clear();
        rawValues.resize(OPEN_EEG_RAW_AMOUNT_OF_CHANNELS);

        graphScaleX = 1.0f;
        graphScaleSpeed = 0.0f;
        bwPositionInSec = 0; // Start recordign from beginning.

        string bwId = "plapla";
        int bwDefaultLength = 60 * 15; // 5 minutes default length,..

        bwRecordId = bwId;

        bwRecordLengthInSecs = bwDefaultLength;

        playerLog("Creating new Brainwave Table Information..");

        lastGraphUpdate = int( clock() );

        /* initialize random seed: */
        srand (time(NULL));

        // Create the channels and their drawing information
        int i=1;

        // GRaphs are placed relative to bw manager's position :)
        channel[CHANNEL_DELTA].create(0,0+(CHANNEL_SIZE_Y*0+CHANNEL_SPACING*i), CHANNEL_DELTA,bwRecordLengthInSecs);
        channel[CHANNEL_THETA].create(0,0+(CHANNEL_SIZE_Y*1+CHANNEL_SPACING*i), CHANNEL_THETA,bwRecordLengthInSecs);

        channel[CHANNEL_ALPHA].create(0,0+(CHANNEL_SIZE_Y*2+CHANNEL_SPACING*i),CHANNEL_ALPHA,bwRecordLengthInSecs);

        channel[CHANNEL_ALPHA1].create(0,0-50,CHANNEL_ALPHA1,bwRecordLengthInSecs); // 1. and 2. graphs are not displayed. We use just their avarage and show that.
        channel[CHANNEL_ALPHA2].create(0,0-100,CHANNEL_ALPHA2,bwRecordLengthInSecs);

        channel[CHANNEL_BETA].create(0,0+(CHANNEL_SIZE_Y*3+CHANNEL_SPACING*i), CHANNEL_BETA,bwRecordLengthInSecs);

        channel[CHANNEL_BETA1].create(0,0-50, CHANNEL_BETA1,bwRecordLengthInSecs);  // 1. and 2. graphs are not displayed. We use just their avarage and show that.
        channel[CHANNEL_BETA2].create(0,0-100, CHANNEL_BETA2,bwRecordLengthInSecs);

        channel[CHANNEL_GAMMA].create(0,0+(CHANNEL_SIZE_Y*4+CHANNEL_SPACING*i),CHANNEL_GAMMA,bwRecordLengthInSecs);

        channel[CHANNEL_GAMMA1].create(0,0-50,CHANNEL_GAMMA1,bwRecordLengthInSecs);  // 1. and 2. graphs are not displayed. We use just their avarage and show that.
        channel[CHANNEL_GAMMA2].create(0,0-100,CHANNEL_GAMMA2,bwRecordLengthInSecs);

        // Some of these maybe placed on top of each other because lack of space.. So it's hard to show em at the same time..->
        channel[CHANNEL_ATT].create(0,0+(CHANNEL_SIZE_Y*5+CHANNEL_SPACING*i),CHANNEL_ATT,bwRecordLengthInSecs);
        channel[CHANNEL_MED].create(0,0+(CHANNEL_SIZE_Y*6+CHANNEL_SPACING*i),CHANNEL_MED,bwRecordLengthInSecs);
        channel[CHANNEL_AVG].create(0,0+(CHANNEL_SIZE_Y*7+CHANNEL_SPACING*i),CHANNEL_AVG,bwRecordLengthInSecs);

        // Power avarage.. I think this doesn't fit?
        channel[CHANNEL_POWER_AVG].create(0,0+(CHANNEL_SIZE_Y*7+CHANNEL_SPACING*i),CHANNEL_POWER_AVG,bwRecordLengthInSecs);

        // Clear the statistic.
        clearStatistics();

}


BWManager::~BWManager()
{
    //dtor
}

// Called when initializing gui elements..
void BWManager::init()
{
    playerLog("This was BWManager.. Do you copy?");
}

// Adjusts visibility
void BWManager::adjustVisibility(int direction)
{
    visibilityShade += direction*deltaTime*0.3f;
    if (visibilityShade>1.0f) visibilityShade=1.0f;
    if (visibilityShade<0.0f) visibilityShade=0.0f;
    bwManagerFade = visibilityShade; // Update this way-round-around solution value for peak position drawing also..
}

bool BWManager::mouseInsideManager() {
    if (guiMouse.getMouseClickX()>graphUIX && guiMouse.getMouseClickY()>graphUIY-CHANNEL_SIZE_Y/2 && guiMouse.getMouseClickX()<graphUIX+CHANNEL_SIZE_X && guiMouse.getMouseClickY()<graphUIY+CHANNEL_SIZE_Y*9-CHANNEL_SIZE_Y/2)
        return true;
    else
        return false;
}

bool BWManager::mouseClickedInsideManager() {
    if (mouseInsideManager()) {
        if (guiMouse.getLeftButtonState()) {
            return true;
        }
    }
}


// NOTE THAT This is very dummy recording for raw values! They may be slightly different in length as some bits are lost sometimes and
// the recording for raw doesn't support jumping! It's just straight forward :D
void BWManager::updateRawValues(int chanIndx, int cval) {
    if (recording && recordRAWData.isChecked()) {
        rawValues[chanIndx].insert ( rawValues[chanIndx].begin() , cval );
    }
}

/*
* refresh()
*::::::::
*
* To refresh all the channels data. Usually this is done in one second interval. Here also is calculated the avarage channel's value of all graphs(dominating freq).
*
*/
void BWManager::refresh()
{

    // Check for interaction If it's visible.
    if (isVisible())
    {
        // Check for mousehover
        if (!showVisualization.isChecked())
        {
            adjustVisibility(1);
        }
        else
        {
            if (mouseInsideManager())
            {
                adjustVisibility(1);
            }
            else
            {
                adjustVisibility(-1);
            }
        }
        // The manager can be zoomed out and in..
        if (mouseInsideManager()) {
            if (guiMouse.mouseWheelRolledUp())
                graphScaleSpeed += 0.5f;
            if (guiMouse.mouseWheelRolledDown())
                graphScaleSpeed -= 0.5f;
        }
        graphScaleX += graphScaleSpeed*deltaTime;
        graphScaleSpeed *= 0.98f;
        if (graphScaleX<0.01f)
            graphScaleX = 0.01f;
        if (graphScaleX>150.0f)
            graphScaleX = 150.0f;

        // It's possible to choose timeline position and view graph information.
        if (mouseClickedInsideManager()) {
            int secondsInOneScreenX = graphWidth/graphScaleX;
            int firstSecToDraw = bwScrollBar.getBarPosition()*(bwRecordLengthInSecs-secondsInOneScreenX);
            if (firstSecToDraw<0.0f)
                firstSecToDraw=0.0f;
            float selectedAtTimeLine = int((float(guiMouse.getMouseX()-graphUIX)/float(graphWidth)*float(secondsInOneScreenX)) + firstSecToDraw); // -1 because we will move it soon again to +1 when second is reached..
            if (selectedAtTimeLine!=bwPositionInSec) {
                // Choose only If it's different position. Not to flood console..
                bwPositionInSec = selectedAtTimeLine;
                if (bwPositionInSec<0) bwPositionInSec=0;
                playerLog("Position selected " + intToStr(bwPositionInSec));
            }
        }
    }

    if (recording && (useDebugInput || bwMeter.EEGDeviceIsWorking()))
    {

        double timeFromLastGraphUpdate = double((std::clock()-lastGraphUpdate)/((double)CLOCKS_PER_SEC));

        if (timeFromLastGraphUpdate > 1.0 && (recording || useDebugInput))
        {

            lastGraphUpdate = int( std::clock() );

            bwPositionInSec++; // Update time after refreshging channels so that 0 gets recorded as first time point index.
            if (bwPositionInSec>bwRecordLengthInSecs) // Enwarp the timeline If neccessary..
                bwRecordLengthInSecs = bwPositionInSec;

            // Fille the graphs with new data values.. Or old If nothing is updated..
            // Thhis is now done in this way. All information should be transfgered still properly from bwmeter.. :)
            for (int i=0; i<AMOUNT_OF_CHANNELS; i++)
                channel[i].refresh();

        } // If time from last update is one second.. Go ahdea and try record!

    } // Recording?<-

}


// Unload all
void BWManager::unLoad()
{
    /* Clean up */
    //..
}

/*
* draw()
*::::::::
*
* Simply draws all the channels that this manager holds for the song. This is the screen of the graphs seen in the program.
*
*/
void BWManager::draw()
{
    // Draw all channels to screen that are wanted to be shown.
    if (isVisible())
    {
        if (!showVisualization.isChecked())
            drawScaledPicture(graphBaseImg,graphUIX-2,graphUIY-CHANNEL_SIZE_Y/2,graphWidth+4,graphHeight);

        float graphLookAtX = 0;

        channel[CHANNEL_DELTA].draw(visibilityShade);

        channel[CHANNEL_THETA].draw(visibilityShade);

        channel[CHANNEL_ALPHA].draw(visibilityShade);

        //   channel[CHANNEL_ALPHA1].draw(surface);
        //   channel[CHANNEL_ALPHA2].draw(surface);

        channel[CHANNEL_BETA].draw(visibilityShade);

        //   channel[CHANNEL_BETA1].draw(surface);
        //   channel[CHANNEL_BETA2].draw(surface);

        channel[CHANNEL_GAMMA].draw(visibilityShade);

        //   channel[CHANNEL_GAMMA1].draw(surface);
        //   channel[CHANNEL_GAMMA2].draw(surface);

        // Choose which of these three! --> No let's juust draw all!s

        channel[CHANNEL_ATT].draw(visibilityShade);
        channel[CHANNEL_AVG].draw(visibilityShade);
        channel[CHANNEL_MED].draw(visibilityShade);

        // Draw timeline. n time function time demanding.
        int gridSpacing = 70;
        int secondsInOneScreenX = graphWidth/graphScaleX;
        int firstSecToDraw = bwScrollBar.getBarPosition()*(bwRecordLengthInSecs-secondsInOneScreenX);
        if (firstSecToDraw<0.0f)
            firstSecToDraw=0.0f;
        int lastSecToDraw  = firstSecToDraw+secondsInOneScreenX;
        float drawToXPos  = 0;
        float drawToXPosLast = -INT_MIN;
        float drawToYPos   = graphUIY+graphHeight-20;
        for (int i=firstSecToDraw; i<lastSecToDraw; i++)
        {
            drawToXPos = float(float((i+1)-firstSecToDraw)*graphScaleX)+graphUIX;
            if (i<=bwRecordLengthInSecs)
            {
                // Draw the actual area.
                if (drawToXPos-drawToXPosLast>=gridSpacing) { // Draw only 25 pixels apart grid.
                    al_draw_filled_rectangle(drawToXPos, drawToYPos-5, drawToXPos+1, drawToYPos+5, al_map_rgba_f (visibilityShade, visibilityShade, visibilityShade, visibilityShade) );
                    if (i<60) {
                        al_draw_text(buttonFont,al_map_rgba_f(0,1,0,visibilityShade), drawToXPos, drawToYPos, 0, (intToStr(i)+" sec").c_str());
                    } else {
                        int minutes = int(i/60);
                        int seconds = i%60;
                        al_draw_text(buttonFont,al_map_rgba_f(0,1,0,visibilityShade), drawToXPos, drawToYPos-15, 0, (intToStr(minutes)+" min ").c_str());
                        al_draw_text(buttonFont,al_map_rgba_f(0,1,0,visibilityShade), drawToXPos, drawToYPos, 0, (intToStr(seconds)+" sec").c_str());
                    }
                    drawToXPosLast = drawToXPos;
                }

            }
            if (i==int(sessionLength.getConvertedValue()*60.0d)) {
                // Let's draw marking of the session ending point
                al_draw_filled_rectangle(drawToXPos, drawToYPos-45, drawToXPos+1, drawToYPos+5, al_map_rgba_f (visibilityShade, visibilityShade, visibilityShade, visibilityShade) );
            }
        }

        // It's possible to choose timeline position and view graph information. This is to show the position line.
        if (mouseInsideManager()) {
            al_draw_filled_rectangle(guiMouse.getMouseX(), graphUIY-CHANNEL_SIZE_Y/2, guiMouse.getMouseX()+1, graphUIY+graphHeight-CHANNEL_SIZE_Y/2, al_map_rgba_f (visibilityShade, visibilityShade, visibilityShade, visibilityShade) );
        }


    }
    GUIElement::draw();
}


// TO analyze avarage power spectrums of phases and relative values compared to other values during different phases.
void BWManager::analyzeStatistics() {

    clearStatistics(); // Just to make sure nothing won't mess up.

    // This calculates first the avarage amplitudes for channels.. then the relative volumes (compared to avarage of all other channels) of current phase of asked channel.

    // Copy the avarage amplitudes of different phases to table first.
    for (int i=0;i<AMOUNT_OF_CHANNELS;i++)
        if (useThisTypeInRelativeCalculations(i))
            getChannelAvgAmplResultTable(channelStats[STAT_POWER][i], i, AMOUNT_OF_PHASES-1);

    // Then make relative map. This is the channel's volume relative to other channels ie 1.5, 0.4, 1.3 etc on asked time..
    for (int z=0;z<AMOUNT_OF_PHASES-1;z++)
        for (int i=0;i<AMOUNT_OF_CHANNELS;i++) {
            // Avarage of all em except this one
            double avgSum = 0;
            int avgCount  = 0;
            for (int q=0;q<AMOUNT_OF_CHANNELS;q++) {
                if (useThisTypeInRelativeCalculations(q) && i!=q) {
                    avgSum+=channelStats[STAT_POWER][q][z];
                    avgCount++;
                }
            }
            channelStats[STAT_RELATIVE][i][z] = channelStats[STAT_POWER][i][z] / (avgSum/avgCount);
        }

    // Then calculate finally the avarage of the channels own relative values.
    for (int i=0;i<AMOUNT_OF_CHANNELS;i++) {
        double avgSum = 0;
        int avgCount  = 0;
        for (int z=0;z<AMOUNT_OF_PHASES-1;z++) {
            avgSum+=channelStats[STAT_RELATIVE][i][z];
            avgCount++;
        }
        channelStats[STAT_RELATIVE][i][AMOUNT_OF_PHASES-1] = (avgSum/avgCount);
    }

}

string BWManager::getStatisticsStr() {
    analyzeStatistics();
    // Printing for testing.
    string str = "* <- means relative-value of the channel.\n\n";
    for (int y=0;y<AMOUNT_OF_CHANNELS;y++) {
        str += getGraphDescriptionText(y);
        for (int z=0;z<AMOUNT_OF_PHASES;z++)
            str += "|" + desToStr(showDecimals(channelStats[STAT_POWER][y][z],3));
        str += "\n";
        str += getGraphDescriptionText(y) + "_*";
        for (int z=0;z<AMOUNT_OF_PHASES;z++)
            str += "|" + desToStr(showDecimals(channelStats[STAT_RELATIVE][y][z],3));
        str += "\n";
    }

    // Add relative specifics for session types.
    str += "\nThese next values are not including delta-channel for calculations. just theta,alpha,beta,gamma for now: \n";
    str += "\nTheta/alfa-ratio(*) ";
    for (int z=0;z<AMOUNT_OF_PHASES;z++)
        str += "|" + desToStr(showDecimals(getSpecificOfResult("theta/alfa",z,STAT_RELATIVE),3));

    str += "\nTheta+Alfa-ratio(*) ";
    for (int z=0;z<AMOUNT_OF_PHASES;z++)
        str += "|" + desToStr(showDecimals(getSpecificOfResult("theta+alfa",z,STAT_RELATIVE),3));

    str += "\nGamma-ratio(*) ";
    for (int z=0;z<AMOUNT_OF_PHASES;z++)
        str += "|" + desToStr(showDecimals(getSpecificOfResult("gamma",z,STAT_RELATIVE),3));

    // Change all dots to , for helping later math processing in external programs.
    replace( str.begin(), str.end(), '.', ',' );

    return str;
    playerLog(str);

}

// Gets either amplitude or relative value of asked parameter ie. "gamma", "t+a" or "t/a" from asked phase and mode(power or relative).
double BWManager::getSpecificOfResult(string inspected, int phase, int mode) {
    analyzeStatistics(); // This can be taken away later for optimization. Now to make sure it's always calculated.
    if (mode == STAT_RELATIVE) {
        // The relative of these specifics are currently done between gamma, alpha, beta and theta. delta was left off for some reason. could be because of big values.. could be later tried to include as well.
        double *thetax = channelStats[STAT_RELATIVE][CHANNEL_THETA];
        double *alphax = channelStats[STAT_RELATIVE][CHANNEL_ALPHA];
        double *betax  = channelStats[STAT_RELATIVE][CHANNEL_BETA];
        double *gammax = channelStats[STAT_RELATIVE][CHANNEL_GAMMA];
        double result = 0;
        if (inspected == "theta+alfa" || inspected == "t+a") {
            //result = ((thetax[phase]+alphax[phase])/2.0d)/((gammax[phase]+betax[phase])/2.0d);
            result = (thetax[phase]+alphax[phase])/2.0d;
        }
        if (inspected == "theta/alfa" || inspected == "t/a") {
            result = thetax[phase]/alphax[phase];
        }
        if (inspected == "gamma") {
            //result = gammax[phase]/((thetax[phase]+alphax[phase]+betax[phase])/3.0d); olikohan nää jonkun aivopierun ansiosta? :D
            result = gammax[phase];
        }
        return result;
    }
    else {
        // POWER is just simply the amplitude (avarage inside the asked area ie middle :) but not between channels as it's in relative as far as I remember how this works)
        double *thetax = channelStats[STAT_POWER][CHANNEL_THETA];
        double *alphax = channelStats[STAT_POWER][CHANNEL_ALPHA];
        double *gammax = channelStats[STAT_POWER][CHANNEL_GAMMA];
        double result = 0;
        if (inspected == "theta+alfa" || inspected == "t+a") {
            result = ((thetax[phase]+alphax[phase])/2.0d);
        }
        if (inspected == "theta/alfa" || inspected == "t/a") {
            result = thetax[phase]/alphax[phase];
        }
        if (inspected == "gamma") {
            result = gammax[phase];
        }
        return result;
    }
}

void BWManager::clearStatistics() {
    for (int x=0;x<AMOUNT_OF_STAT_MODES;x++)
        for (int y=0;y<AMOUNT_OF_CHANNELS;y++)
            for (int z=0;z<AMOUNT_OF_PHASES;z++)
                    channelStats[x][y][z] = 0.0d;
}

// Returns table of the avarage power calculation results. Give always one index bigger table as input than parts is. Because last one is total avarage.
// Avarage here means the avarage of the amplitude in the requested phases. not between total channels.
void BWManager::getChannelAvgAmplResultTable(double *table, int chanIndx, int parts)
{
    double avgSum=0;
    double totalAvgSum=0;
    double value=0;
    double avg=0;
    double totalRecordLength = channel[chanIndx].getRecordedLength(); // The last recorded point
    for (int q=0;q<parts;q++) {
        int from = (int)((totalRecordLength/(double)parts)*(double)q);
        int to   = (int)((totalRecordLength/(double)parts)*(double)(q+1));
        avgSum   = 0;
        for (int i=from; i<to; i++)
        {
            int value = channel[chanIndx].valueOf(i);
            if (value==CHANNEL_EMPTY_INDEX) // Empty recording area is rare? Why someone record in parts? But If that occurs it will be counted as zero now.
                value = 0;
            avgSum += value;
        }
        double partAvg = (avgSum/((double)(to-from)));
        totalAvgSum += partAvg;
        table[q] = partAvg;
    }
    table[parts] = totalAvgSum/parts; // The last one is reserved for the total avarage.
}


