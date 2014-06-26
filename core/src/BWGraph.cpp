#include "../include/BWGraph.h"
#include "../include/BWMeter.h"
#include "../include/PlayerCommon.h"

using namespace plrCommon;

CHANNEL_THEME getGraphTheme(int type)
{
    CHANNEL_THEME graphTheme;
    graphTheme.r = 0;
    graphTheme.b = 0;
    graphTheme.b = 0;
    graphTheme.text = "Template Graph";
    graphTheme.useForAvarageCalculation = false;
    graphTheme.showInBarPanel = false;
    graphTheme.useInRelativeCalculation = false;

    // Then override with real values..
    // The ranges has different sources and meanings. In this case they were assumed on previous experiment with neurofeedback.
    if (type==CHANNEL_DELTA)
    {
        graphTheme.r = 0;
        graphTheme.g = 30;
        graphTheme.b = 110;
        graphTheme.text = "Delta";
        graphTheme.bottomFreq = 0.0f;
        graphTheme.topFreq    = 4.5f;
        graphTheme.centerFreq = (graphTheme.topFreq + graphTheme.bottomFreq) / 2.0f;
        graphTheme.useForAvarageCalculation = true;
        graphTheme.showInBarPanel = true;
        graphTheme.useInRelativeCalculation = true;
    }
    if (type==CHANNEL_THETA)
    {
        graphTheme.r = 255;
        graphTheme.g = 0;
        graphTheme.b = 130;
        graphTheme.text = "Theta";
        graphTheme.bottomFreq = 4.5f;
        graphTheme.topFreq    = 8.0f;
        graphTheme.centerFreq = (graphTheme.topFreq + graphTheme.bottomFreq) / 2.0f;
        graphTheme.useForAvarageCalculation = true;
        graphTheme.showInBarPanel = true;
        graphTheme.useInRelativeCalculation = true;
    }
    if (type==CHANNEL_ALPHA)
    {
        graphTheme.r = 255;
        graphTheme.g = 255;
        graphTheme.b = 0;
        graphTheme.text = "Alpha";
        graphTheme.bottomFreq = 8.0f;
        graphTheme.topFreq    = 15.0f;
        graphTheme.centerFreq = (graphTheme.topFreq + graphTheme.bottomFreq) / 2.0f;
        graphTheme.useForAvarageCalculation = true;
        graphTheme.showInBarPanel = true;
        graphTheme.useInRelativeCalculation = true;
    }
    if (type==CHANNEL_BETA)
    {
        graphTheme.r = 0;
        graphTheme.g = 255;
        graphTheme.b = 0;
        graphTheme.text = "Beta";
        graphTheme.bottomFreq = 16.0f;
        graphTheme.topFreq    = 31.0f;
        graphTheme.centerFreq = (graphTheme.topFreq + graphTheme.bottomFreq) / 2.0f;
        graphTheme.useForAvarageCalculation = true;
        graphTheme.showInBarPanel = true;
        graphTheme.useInRelativeCalculation = true;
    }
    if (type==CHANNEL_GAMMA)
    {
        graphTheme.r = 255;
        graphTheme.g = 0;
        graphTheme.b = 255;
        graphTheme.text = "Gamma";
        graphTheme.bottomFreq = 32.0f;
        graphTheme.topFreq    = 44.0f;
        graphTheme.centerFreq = (graphTheme.topFreq + graphTheme.bottomFreq) / 2.0f;
        graphTheme.useForAvarageCalculation = true;
        graphTheme.showInBarPanel = true;
        graphTheme.useInRelativeCalculation = true;
    }
    if (type==CHANNEL_AVG)
    {
        // Varies.. Color varies according the dominating state which takes color from here up or down....
        graphTheme.r = 255;
        graphTheme.g = 255;
        graphTheme.b = 255;
        graphTheme.text = "Dominating";
        graphTheme.showInBarPanel = true;
    }
    if (type==CHANNEL_ATT)
    {
        graphTheme.r = 255;
        graphTheme.g = 70;
        graphTheme.b = 0;
        graphTheme.text = "Attention";
        graphTheme.showInBarPanel = true;
    }
    if (type==CHANNEL_MED)
    {
        graphTheme.r = 0;
        graphTheme.g = 60;
        graphTheme.b = 255;
        graphTheme.text = "Meditation";
        graphTheme.showInBarPanel = true;
    }
    if (type == CHANNEL_ALPHA1)
    {
        graphTheme.r = 120;
        graphTheme.g = 45;
        graphTheme.b = 16;
        graphTheme.bottomFreq = 8.0f;
        graphTheme.topFreq    = 11.0f;
        graphTheme.centerFreq = (graphTheme.topFreq + graphTheme.bottomFreq) / 2.0f;
        graphTheme.text = "Alpha 1";
        graphTheme.showInBarPanel = true;
        graphTheme.useInRelativeCalculation = true;
    }
    if (type == CHANNEL_ALPHA2)
    {
        graphTheme.r = 122;
        graphTheme.g = 90;
        graphTheme.b = 51;
        graphTheme.bottomFreq = 11.0f;
        graphTheme.topFreq    = 14.0f;
        graphTheme.centerFreq = (graphTheme.topFreq + graphTheme.bottomFreq) / 2.0f;
        graphTheme.text = "Alpha 2";
        graphTheme.showInBarPanel = true;
        graphTheme.useInRelativeCalculation = true;
    }
    if (type == CHANNEL_BETA1)
    {
        graphTheme.g = 42;
        graphTheme.bottomFreq = 14.0f;
        graphTheme.topFreq    = 25.0f;
        graphTheme.centerFreq = (graphTheme.topFreq + graphTheme.bottomFreq) / 2.0f;
        graphTheme.text = "Beta 1";
        graphTheme.showInBarPanel = true;
        graphTheme.useInRelativeCalculation = true;
    }
    if (type == CHANNEL_BETA2)
    {
        graphTheme.g = 77;
        graphTheme.bottomFreq = 25.0f;
        graphTheme.topFreq    = 35.0f;
        graphTheme.centerFreq = (graphTheme.topFreq + graphTheme.bottomFreq) / 2.0f;
        graphTheme.text = "Beta 2";
        graphTheme.showInBarPanel = true;
        graphTheme.useInRelativeCalculation = true;
    }
    if (type == CHANNEL_GAMMA1)
    {
        graphTheme.r = 42;
        graphTheme.g = 0;
        graphTheme.b = 42;
        graphTheme.bottomFreq = 32.0f;
        graphTheme.topFreq    = 38.0f;
        graphTheme.centerFreq = (graphTheme.topFreq + graphTheme.bottomFreq) / 2.0f;
        graphTheme.text = "Gamma 1";
        graphTheme.showInBarPanel = true;
        graphTheme.useInRelativeCalculation = true;
    }
    if (type == CHANNEL_GAMMA2)
    {
        graphTheme.r = 67;
        graphTheme.g = 0;
        graphTheme.b = 67;
        graphTheme.bottomFreq = 39.0f;
        graphTheme.topFreq    = 44.0f;
        graphTheme.centerFreq = (graphTheme.topFreq + graphTheme.bottomFreq) / 2.0f;
        graphTheme.text = "Gamma 2";
        graphTheme.showInBarPanel = true;
        graphTheme.useInRelativeCalculation = true;
    }
    if (type == CHANNEL_POWER_AVG)
    {
        graphTheme.r = 255;
        graphTheme.g = 255;
        graphTheme.b = 255;
        graphTheme.text = "Power Avarage";
        graphTheme.showInBarPanel = true;
    }

    // Adjust the graph theme color indexes from 0-255 to 0-1.0f->
    graphTheme.r = graphTheme.r/255.0f;
    graphTheme.g = graphTheme.g/255.0f;
    graphTheme.b = graphTheme.b/255.0f;

    return graphTheme;
}

ALLEGRO_COLOR getGraphThemeColorOf(int typeP) {
    float alphaq = 1.0f;
    return al_map_rgba_f (getGraphTheme(typeP).r*alphaq,getGraphTheme(typeP).g*alphaq,getGraphTheme(typeP).b*alphaq, alphaq);
}

bool useThisTypeInRelativeCalculations(int type) {
    return getGraphTheme(type).useInRelativeCalculation;
}

string getGraphDescriptionText(int typeP) {
    return getGraphTheme(typeP).text;
}
int getThisTypeCenterFreq(int type)
{
    return getGraphTheme(type).centerFreq;
}
bool BWGraph::useThisForAvarageCalculation()
{
    return getGraphTheme(type).useForAvarageCalculation;
}
bool useThisTypeForAvarageCalculation(int type)
{
    return getGraphTheme(type).useForAvarageCalculation;
}

// GLobal version of this to use in channelInfo
ALLEGRO_COLOR getGlobalColorOfFrequency(double value)
{
    for (int i=0; i<AMOUNT_OF_CHANNELS; i++)
    {
        if (getGraphTheme(i).useForAvarageCalculation)
            if (value>getGraphTheme(i).bottomFreq && value<=getGraphTheme(i).topFreq)
            {
                return al_map_rgba_f (getGraphTheme(i).r, getGraphTheme(i).g, getGraphTheme(i).b, 1.0f);
            }
    }
}


BWGraph::BWGraph()
{
    //ctor
    x = 0;
    y = 0;
}

BWGraph::~BWGraph()
{
    //dtor
}

void BWGraph::create(int x2, int y2, int type2, int secAmount)
{
    x    = x2;
    y    = y2;
    type = type2;

    theLatestValue = 0;

    // Length in seconds..
    lengthInSecs = secAmount;

    // Initialize the data table size according seconds amount..
    clearGraph();

    // Let's calculate other neccessary values..
    w = (int)CHANNEL_SIZE_X; // or bwManager.graphWidth
    h = (int)CHANNEL_SIZE_Y;

    alpha = 1.0f;

    // Copy the information from graph struct according the chosen channel..
    CHANNEL_THEME graphTheme = getGraphTheme(type);
    text = graphTheme.text;

    // Make zero line and clear it etc..
    clearGraph();

}


void BWGraph::clearGraph()
{
    graphValues.clear();
    for (int i=0; i<lengthInSecs+1; i++)
        graphValues.push_back(CHANNEL_EMPTY_INDEX);
}


// This will take out the value from given index
double BWGraph::valueOf(int gIndex)
{
    if (gIndex<graphValues.size()&&gIndex>=0)
        return graphValues[gIndex];
    else
    {
        return CHANNEL_EMPTY_INDEX;
    }
}




ALLEGRO_COLOR BWGraph::getCurrentDrawColor(double value)
{
    if (type==CHANNEL_AVG)
    {
        // For avarage we check in which range this is and return the corresponding color..
        return getGlobalColorOfFrequency(value);
    }
    else
    {
        // Normal coloring
        float cAlpha = value/100.0f;
        float fableScaleLightSuperb=1.0f;
        float cR = getGraphTheme(type).r*(cAlpha*cAlpha*fableScaleLightSuperb);
        float cG = getGraphTheme(type).g*(cAlpha*cAlpha*fableScaleLightSuperb);
        float cB = getGraphTheme(type).b*(cAlpha*cAlpha*fableScaleLightSuperb);
        cR += cAlpha*cAlpha*0.25f;
        cG += cAlpha*cAlpha*0.25f;
        cB += cAlpha*cAlpha*0.25f;
        if (cR>1.0f) cR = 1.0f;
        if (cG>1.0f) cG = 1.0f;
        if (cB>1.0f) cB = 1.0f;

        float superAlpha = cAlpha*cAlpha*fableScaleLightSuperb;
        if (superAlpha>1.0f) superAlpha = 1.0f;

        ALLEGRO_COLOR color = al_map_rgba_f (cR,cG,cB, superAlpha);
        return color;
    }
}

void BWGraph::refresh()
{
    refresh(bwMeter.getCurrentEEGChannelSlowBarValue(type)*100.0d); // Get the data from meter according this graph's id
}

void BWGraph::refresh(double newValue)
{

    theLatestValue=newValue;

    // Now we just write the data to vector If it's inside current size..
    if (bwPositionInSec<=lengthInSecs) {
        int indPos = bwPositionInSec-1; //.. This is weird. But it works now in comfortable way so I leave it.
        if (indPos<0)
            indPos = 0;
        graphValues[indPos] = newValue;
    } else { // Otherwise we just expand the vector and write new value. and expand the length.
        // Let's fill the gap If we wanted to record long way ahead of real recording status..
        if (bwPositionInSec-graphValues.size()>1) {
            int addAmount = bwPositionInSec-graphValues.size();
            if (addAmount<0) addAmount = 0;
            for (int i=0;i<addAmount;i++) // Fill it .. But push the last value which is new real value.
                graphValues.push_back(CHANNEL_EMPTY_INDEX);
            lengthInSecs = bwPositionInSec-1;
            //playerLog("new length of graph " + intToStr(lengthInSecs));
        }
        graphValues.push_back(newValue);
        lengthInSecs++;
    }

}

double BWGraph::getHzCenter()
{
    return getGraphTheme(type).centerFreq;
}


// I just got pissed off with this shitbag and I try to make this output the latest value this way IF vector doesn't work out..
double BWGraph::getLatestUpdatedValue()
{
    return theLatestValue;
}


double BWGraph::getCurrentValue()
{
    return valueOf(bwPositionInSec);
}

// Get the time point where is the latest recorded data available.
int BWGraph::getRecordedLength() {
    for (int i=lengthInSecs-1;i>-1;i--) {
        if (valueOf(i)!=CHANNEL_EMPTY_INDEX) {
            return i;
        }
    }
}

double BWGraph::getAvarage() {
    int indexAmount=0;
    double sum=0;
    for (int i=0;i<lengthInSecs;i++) {
        if (valueOf(i)!=CHANNEL_EMPTY_INDEX) {
            indexAmount++;
            sum+=valueOf(i);
        }
    }
    if (indexAmount==0||sum==0)
        return 0;
    else
        return double(sum/double(indexAmount));
}

void BWGraph::draw(float vShade)
{
    // Draw the basic graph..
    if (vShade>0.0f)
    {
        // Select the color also for main drawing here..
        ALLEGRO_COLOR chosenColor = al_map_rgba_f (getGraphTheme(type).r*alpha,getGraphTheme(type).g*alpha,getGraphTheme(type).b*alpha, alpha);

        int secondsInOneScreenX = bwManager.graphWidth/bwManager.graphScaleX;
        int firstSecToDraw = bwScrollBar.getBarPosition()*(lengthInSecs-secondsInOneScreenX);
        if (firstSecToDraw<0.0f)
            firstSecToDraw=0.0f;
        int lastSecToDraw  = firstSecToDraw+secondsInOneScreenX;
        float drawToXPos  = 0;
        float drawToXPosLast = bwManager.graphUIX;
        double drawVal=0;
        al_draw_line( bwManager.graphUIX, bwManager.graphUIY+y, bwManager.graphUIX+bwManager.graphWidth, bwManager.graphUIY+y+1, al_map_rgba_f (getGraphTheme(type).r*vShade,getGraphTheme(type).g*vShade,getGraphTheme(type).b*vShade, vShade), 1 );
        for (int i=firstSecToDraw; i<lastSecToDraw; i++)
        {
            drawVal = (CHANNEL_SIZE_Y/100.0f)*valueOf(i)/2.0f; // Asking this time point value of 0-100 scaled.. This should then fit it into the graph size doesn't it? ^__- Divided by two to match drawing both sides
            if (drawVal>CHANNEL_SIZE_Y/2.0f)
                drawVal = CHANNEL_SIZE_Y/2.0f;
            if (drawVal<0.0f)
                drawVal=0.0f;

            // Draw the actual area.
            drawToXPos = float(float((i+1)-firstSecToDraw)*bwManager.graphScaleX) + bwManager.graphUIX;

            if (valueOf(i)!=CHANNEL_EMPTY_INDEX)
            {
                if (drawToXPos-drawToXPosLast>=1) { // Draw only 1 pixel .. not many pixels on each other so we spend resources and program won't lag:)?
                    al_draw_filled_rectangle( drawToXPosLast, bwManager.graphUIY+y-drawVal, drawToXPos , bwManager.graphUIY+y+drawVal, getCurrentDrawColor(valueOf(i)));
                    drawToXPosLast = drawToXPos;
                }

            }
            else
            {
                // Mark this as not recorded area. Draw there dotted line to show that next to graph line. .....
                if (i%2)
                    al_draw_filled_rectangle( drawToXPos, bwManager.graphUIY+y-1, drawToXPos+1, bwManager.graphUIY+y, chosenColor);
                drawToXPosLast = drawToXPos; // So new position also start after this clean part is drawn..
            }
        }

        al_draw_text(buttonFont,al_map_rgba_f(chosenColor.r,chosenColor.g,chosenColor.b,vShade), x+bwManager.graphUIX+w-70, y+bwManager.graphUIY, 0, text.c_str());

        // Draw anyway always the cursor / song time position.
        float timeLineCursorX = float(float(bwPositionInSec+1-firstSecToDraw)*bwManager.graphScaleX)+bwManager.graphUIX;
        if (timeLineCursorX>=bwManager.graphUIX&&timeLineCursorX<=bwManager.graphUIX+bwManager.graphWidth)
            al_draw_filled_rectangle(timeLineCursorX,  bwManager.graphUIY+y-h/2, timeLineCursorX+1,  bwManager.graphUIY+y+h/2, al_map_rgba_f (vShade, vShade, vShade, vShade) );

    }

}
