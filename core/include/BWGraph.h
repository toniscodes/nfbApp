#ifndef BWCHANNEL_H
#define BWCHANNEL_H

#include <math.h>
#include <string>
#include <limits.h>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/array.hpp>
#include <audiere.h>
#include "GUI/GUIMouse.h"
#include "GUI/GUIElementList.h"
#include "ImageLibrary.h"

using namespace std;

#define CHANNEL_TABLE_LOG_SIZE 3600

#define CHANNEL_SIZE_X 499
#define CHANNEL_SIZE_Y 30
#define CHANNEL_SPACING 2

// The empty index to in graph. This means this is not recorded point. Graph value is anyway from 0-100
#define CHANNEL_EMPTY_INDEX -1010101.0d

#define deltaHzMax   4.0f
#define thetaHzMax   7.0f
#define alphaHzMax   11.0f
#define betaHzMax    20.0f

typedef struct
{
    float r;
    float g;
    float b;
    string text;
    float bottomFreq;
    float topFreq;
    float centerFreq;
    bool useForAvarageCalculation;
    bool showInBarPanel;
    bool useInRelativeCalculation;
} CHANNEL_THEME;

string        getGraphDescriptionText(int type);
CHANNEL_THEME   getGraphTheme(int type);
ALLEGRO_COLOR getGraphThemeColorOf(int type);
ALLEGRO_COLOR getGlobalColorOfFrequency(double value);
bool useThisTypeForAvarageCalculation(int type);
bool useThisTypeForOpenEEG(int type);
bool useThisTypeInRelativeCalculations(int type);
int  getThisTypeCenterFreq(int type);

/**
 * \brief BWGraph.h ...::: Brainwave Graph - Dataholder for brainwave channel logging. :::...
*
* This class holds the information of one single channel of brainwave information. It receives the updates from BWManager(which receives it from BWMeter).
* Here are functions to draw the graphs and put them information. Also here exists the themes of each different graphs as they have different colors and center frequencies etc.
*
* Sidenode(This is captured originally from my old project for recording brainwaves also.):
*  Toni Sanio 2011
*  #18.10.2011 class created..
*  #7.7.2013 Class captured and modified for music player with bci.. One graph represents the channel of information from bci device.
*  #x.x.2014 Class modified many times again. I don't know what's left from the original anymore :D
* author: tonis @ 2013
*
*/
class BWGraph
{
public:
    BWGraph();
    virtual ~BWGraph();
    void create(int x2, int y2, int type2, int secAmount);
    void draw(float vShade);
    void refresh();
    void refresh(double newValue);
    void insertTo(int gIndex,double newValue);
    double getCurrentValue();
    double getHzCenter();
    ALLEGRO_COLOR getCurrentDrawColor(double value);
    void clearGraph();
    double getAvarage();
    bool useThisForAvarageCalculation();
    double valueOf(int gIndex);
    void  dontUseLastPosition();
    double getLatestUpdatedValue();
    int getRecordedLength();

    // Raw values array that holds actually the most precious data :)
    vector<double> graphValues;

    int x,y,w,h;
    int type;
    int lengthInSecs;
    float alpha;
    string text;
    double theLatestValue;


protected:
private:
    // Boost serialization. This way it's easier to save and load everything without parsing problems.
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & graphValues;
        ar & text;
        ar & alpha;
        ar & x;
        ar & y;
        ar & w;
        ar & h;
        ar & type;
        ar & theLatestValue;
        ar & lengthInSecs;
    }

};

#endif // BWCHANNEL_H

