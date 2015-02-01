#include "PlayerCommon.h"

#ifndef SESSION_H
#define SESSION_H


using namespace std;

class Session : public GUIElement
{
    public:
        Session();
        virtual ~Session();
        void init();
        void adjustLevelLogic();
        void increasePrizeCount();
        void nextLevel();
        void previousLevel();
        double  calibratedAvarages[AMOUNT_OF_CHANNELS];

        void restart();
        void refresh();

        void doThingsBeforeAutoSave();
        void doThingsAfterAutoLoad();
        void pause();
        void start();
        void endNeurofeedBackSessionInExperiment();

        int   automaticCalibrationCount;
        float lastFoundPrizeTime;
        int   currentLevel;
        //int   lastLevelReached; using currentLevel now until these experiments are done. currentLevel%(amountoflevels) will tell the current level
        int   prizesGot;
        int   trainingType;
        bool  sessionOn;
        int   amountOfPrizePerLevel;

        //! NFB-Session running variables.
        int    nfbSessionPoint;
        int    recTimePoint;

        string startTimeStamp;

    protected:
    private:

        // Boost serialization. This way it's easier to save and load everything without parsing problems.
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & automaticCalibrationCount;
            ar & lastFoundPrizeTime;
            ar & currentLevel;
            ar & prizesGot;
            ar & trainingType;
            ar & sessionOn;
            ar & amountOfPrizePerLevel;
            ar & startTimeStamp;
            ar & calibratedAvarages;

            ar & nfbSessionPoint;
            ar & recTimePoint;
        }

};

#endif // SESSION_H
