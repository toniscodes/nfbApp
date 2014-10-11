#include <math.h>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/array.hpp>
#include <audiere.h>
#include "GUI/GUIElement.h"
#include "playerCommon.h"



#ifndef CONFIG_H
#define CONFIG_H

// These are now here because some things need these and they are in difficult order so.. Just here for now.. glue

enum CPANEL_MODE {  STATISTIC_PANEL,
                    RECORD_PLAY_PANEL,
                    NFB_PANEL,
                    SETUP_PANEL,
                    AMOUNT_OF_PANEL_MODES
                 }; //! Notice that the last one tells the amount of these things..

enum NFB_SESSION_POINT {    NFB_BEGIN_POINT,
                            NFB_CALIBRATION_POINT,
                            NFB_SESSION_POINT,
                            NFB_PAUSED_POINT,
                            NFB_END_POINT,
                            NFB_END_POINT_IDLE_AFTER
                        };

// These tells the level indexes.
enum NFB_LEVELS {  LEVEL_1,
                   LEVEL_2,
                   LEVEL_3,
                   LEVEL_4,
                   LEVEL_5,
                   LEVEL_6,
                   AMOUNT_OF_LEVELS
                 }; // Last one is for telling the amount of levels.

// NFB Training modes.
enum NFB_MODES {   GAMMA_MEDITATION_TRAINING,
                   THETA_ALPHA_MEDITATION_TRAINING,
                   AMOUNT_OF_TRAINING_MODES
                 };


using namespace std;

class ConfigVar {
    public:
        ConfigVar() {
            varName = "-1";
            varVal = "0";
        }
        ConfigVar(string varNameP, string varValP) {
            varName=varNameP;
            varVal=varValP;
        }
        string varName;
        string varVal;
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & varName;
        ar & varVal;
    }
};

class Config
{
    public:
        Config();
        virtual ~Config();
        void save();
        void load();
        void save(string confFile);
        void load(string confFile);
        vector<ConfigVar> configVars;
        void loadVariablesAfterLoad();
        void bringVariablesForSaving();
    protected:
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & configVars;
        }
};

#endif // CONFIG_H
