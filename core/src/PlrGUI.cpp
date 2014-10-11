#include "../include/PlrFunctionality.h"
#include "../include/PlrGUI.h"
#include "../include/PlayerCommon.h"

PlrGUI::PlrGUI()
{
    //ctor
}

PlrGUI::~PlrGUI()
{
    //dtor
}

using namespace boost::filesystem;
using namespace audiere;
using namespace std;

namespace plrCommon
{


void hideAllPanels()
{
    bwManager.hide();
    bwStatsView.hide();
    recordRAWData.hide();
    //activityMeterRadioButton.hide();
    mindWavePortRollButton.hide();
    reBootMindWave.hide();
    showVisualization.hide();
    showMirror.hide();
    nfbTraineeModeRadioButton.hide();
    EEGDeviceModeRadioButton.hide();
    OpenEEGPortRollButton.hide();
    reBootOpenEEG.hide();
    mindWaveScaleFactor.hide();

    resultModeRadioButton.hide();
    renderStatisticButton.hide();

    sessionLength.hide();
    minimumPlace.hide();
    maximumPlace.hide();
    calibrationTime.hide();
    prizeThreshold.hide();
    timeOfNextPrizeMin.hide();
    amountOfPrizesPerLevel.hide();
    timeOfNextPrizeMin.hide();

    panelText="";
}

//! Shows different GUI Elements according chosen panel. See this should be delted and changed later.
void showPanel(int panelIndex)
{
    hideAllPanels();
    controlPanelMode = panelIndex;
    // This could be changed later automatically be chosen with button's panel variable or similar that determines in which panel button belongs to.
    // BUt for now.. Still doing this ancient way :)
    if (panelIndex == STATISTIC_PANEL) {
        bwStatsView.show();
        resultModeRadioButton.show();
        renderStatisticButton.show();
    }
    if (panelIndex == RECORD_PLAY_PANEL)
    {
        if (!showVisualization.isChecked())   // Note that we want to hide everything in this basic panel.. To have more view to the actual visualization
        {
            panelText = "Record Play Panel";
            bwManager.show();
        }
    }
    if (panelIndex == NFB_PANEL)
    {
        nfbTraineeModeRadioButton.show();
        minimumPlace.show();
        maximumPlace.show();
        calibrationTime.show();
        prizeThreshold.show();
        timeOfNextPrizeMin.show();
        amountOfPrizesPerLevel.show();
        timeOfNextPrizeMin.show();
        sessionLength.show();

    }
    if (panelIndex == SETUP_PANEL)
    {
        panelText = "Setup Panel";
        recordRAWData.show();
        mindWavePortRollButton.show();
        reBootMindWave.show();
        showVisualization.show();
        showMirror.show();
       // activityMeterRadioButton.show();
        EEGDeviceModeRadioButton.show();
        OpenEEGPortRollButton.show();
        reBootOpenEEG.show();
        mindWaveScaleFactor.show();
    }

}


/************************
* UPDATE GRAPHICS / GUI *
*************************/
void updateMainGUI()
{
    /*
    * DRAWING
    */
    // Let's start drawing to the old flipped as our new double buffer :)
    al_clear_to_color(al_map_rgb(0,0,0));

    // Draw the background visualization if it's in background mode..
    backgroundVisualization.drawVisualization();

    // Draw background..
    //onTopVisualization = true;
    if (!showVisualization.isChecked())
        al_draw_bitmap(bgImg,0,0,0);


    if (libraryFont)
    {
        float alpha           = 0.5f+cos(totalFrameTime)*0.5f;
        ALLEGRO_COLOR color   = al_map_rgba_f(1.0*alpha, 0.4*alpha, 0.6*alpha, alpha); // <-- correct
        al_draw_textf(libraryFont, al_map_rgb(255, 255, 255),54, 8, ALLEGRO_ALIGN_CENTRE, "FPS: %d", fps);

        // Draw panel text
        al_draw_textf(libraryFont, al_map_rgb(255, 255, 255),474, 90, ALLEGRO_ALIGN_LEFT, panelText.c_str());

        // If record play panel. Show the chosen mode! Chewing gum!
        if (controlPanelMode == RECORD_PLAY_PANEL)
        {
            al_draw_textf(libraryFont, al_map_rgb(59, 255, 79),480, 140+cos(al_get_time())*15.0f, ALLEGRO_ALIGN_LEFT, (nfbTraineeModeRadioButton.getSelectionTxt()+" F2/F3").c_str());
        }

        // Draw all basic GUI Elements
        elementList.draw();

    }



    // Draw mirror If desired so..:P
    if (showMirror.isChecked())
    {
        copyImg = al_create_sub_bitmap(tripleBuffer, 0, mirrorBottom-(al_get_display_height(display)-mirrorBottom), al_get_display_width(display), (al_get_display_height(display)-mirrorBottom));
        //al_draw_bitmap(copyImg,74,554,ALLEGRO_FLIP_VERTICAL);
        float mirrorAlpha = 0.4f;
        // Draw black before mirror..
        al_draw_filled_rectangle(0, mirrorBottom, al_get_display_width(display), al_get_display_height(display), al_map_rgb(0,0,0));
        //al_draw_tinted_bitmap(copyImg, al_map_rgba_f(0.0f*mirrorAlpha, 0.5f*mirrorAlpha, 1.0f*mirrorAlpha, mirrorAlpha), 74,554, ALLEGRO_FLIP_VERTICAL);
        al_draw_tinted_bitmap(copyImg, al_map_rgba_f(0.0f*mirrorAlpha, 0.5f*mirrorAlpha, 1.0f*mirrorAlpha, mirrorAlpha), 0, mirrorBottom, ALLEGRO_FLIP_VERTICAL);
    }

    al_set_target_bitmap(al_get_backbuffer(display));
    al_draw_bitmap(tripleBuffer,0, 0, 0);

    // Flip the backbuffer to display! :P
    al_flip_display();

    // Now how much  time did we spent drawing stuff?
    double t = al_get_time();

    //deltaTime       = t-lastUpdate;      UPDATE THIS COULD BE REPLACED WITH AL_GET_TIME() :)
    totalFrameTime += deltaTime;
    if (totalFrameTime>dmax) totalFrameTime = 0; // When maximum.. go to zero. This maybe problem in some special cases in long run but.. not important now

    fps_accum++;
    if (t - fps_time >= 1)
    {
        fps       = fps_accum;
        fps_accum = 0;
        fps_time  = t;
        // Calculate also functional fps just for curiousity.
        //if (deltaTime==0) deltaTime=1.0f; // It's never 0 but anyway..
        //playerLog("Functional framerate: " + intToStr(int(1.0d/deltaTime)));
    }

    // And last merge the last gfx update
    lastGfxUpdate = t;

}


};
