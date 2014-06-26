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
