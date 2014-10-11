#include "GUI/GUIElement.h"
#include <vector>
#include "BWManager.h"
#include "Session.h"
#include "Vec2.h"

#ifndef BWSTATSVIEW_H
#define BWSTATSVIEW_H

using namespace std;

class BWStatsView : public GUIElement
{
    public:
        BWStatsView();
        virtual ~BWStatsView();

        BWStatsView(string s, double posX, double posY, double width, double height, string toolTipText);
        void reset();
        bool mouseInside();
        bool mouseClickedInside();
        void refresh();
        void draw();
        void drawStatsGraph(int statsListIndex, int yGraphIndex, int amountOfGraphs, ALLEGRO_COLOR useColor, string descTxt);

        // --> bwmanagerista-> channelStats[AMOUNT_OF_STAT_MODES][AMOUNT_OF_CHANNELS][AMOUNT_OF_PHASES];

        vector< vector<double> > statisticPoints;

        vector< BWManager > resultSessionBWs;
        vector< BWManager > resultBaselineBWs;
        vector< Session > resultSessions;

        void refreshStatistic();

        Vec2d position;
        Vec2d dimension;

    protected:
    private:
};

#endif // BWSTATSVIEW_H
