#include <ctime>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
/*#include <boost/serialization/vector.hpp>*/
#include <boost/array.hpp>
#include "GUI/GUIElement.h"
#include <vector>
//#include "PlayerCommon.h"
#include "BWGraph.h"
#include "BWMeter.h"

#ifndef BWMANAGER_H
#define BWMANAGER_H


using namespace std;
//using namespace plrCommon;

/**
 * \brief BWManager.h ...:::Brainwave Manager Takes Care Of The Graphs:::...
*
* This class holds all the main graphs, updates(Data from BWMeter), draws them and calculates avarageGraph as well.
*
* author: tonis @ 2013
*
*/
class BWManager : public GUIElement
{
public:
    BWManager();
    BWManager(string s, int posX, int posY, int width, int height, string toolTipText);
    virtual ~BWManager();
    BWGraph channel[AMOUNT_OF_CHANNELS];
    vector< vector<int> > rawValues;
    void draw();
    void refresh();
    void init();
    void fly();
    void reset();
    void unLoad();
    void newBWTable();
    void positionChanged();
    void stopRecording();
    void startRecording();
    void drawAllAgain();
    bool mouseInsideManager();
    bool mouseClickedInsideManager();
    void updateRawValues(int chanIndx, int cval);
    float visibilityShade;
    float graphScaleX;
    void updateAvarageGraph();
    void adjustVisibility(int direction);
    int graphUIX;
    int graphUIY;
    int graphWidth;
    int graphHeight;
    int bwRecordLengthInSecs;
protected:
private:
    bool recording;
    double lastGraphUpdate;
    string bwRecordId;
    float graphScaleSpeed;
    // Boost serialization. This way it's easier to save and load everything without parsing problems.
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & graphUIX;
        ar & graphUIY;
        ar & graphWidth; // These parameters .. should be overwritten by app rather than loaded after all.. so it can be used in any version :)
        ar & graphHeight;
        ar & graphScaleX;
        ar & channel;
        ar & rawValues;
        ar & bwRecordId;
        ar & bwRecordLengthInSecs;
        ar & recording;
        ar & lastGraphUpdate;
    }
};

#endif // BWMANAGER_H
