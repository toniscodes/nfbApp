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

// These are used for getting values to basic graph
enum GRAPH_PHASES {  START_PHASE,
                       MIDDLE_PHASE,
                       END_PHASE,
                       AVG_PHASE,
                    AMOUNT_OF_PHASES
                  }; // Notice that the last one tells the amount of these things..

enum STAT_MODES { STAT_POWER,
                    STAT_RELATIVE,
                   AMOUNT_OF_STAT_MODES};

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
    double channelStats[AMOUNT_OF_STAT_MODES][AMOUNT_OF_CHANNELS][AMOUNT_OF_PHASES];
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

    // Analyzing statistics.
    void getChannelAvgAmplResultTable(double *table, int chanIndx, int parts);
    void analyzeStatistics();
    void clearStatistics();
    string getStatisticsStr();
    double getSpecificOfResult(string inspected, int phase, int mode);

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
        ar & channelStats;
    }
};

#endif // BWMANAGER_H
