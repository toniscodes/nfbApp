#ifndef PLAYERCOMMON_H
#define PLAYERCOMMON_H

#include <string>
#include <boost/filesystem.hpp>
#include <audiere.h>
#include "Config.h"
#include "BWManager.h"
#include "BWStatsView.h"
#include "BWMeter.h"
#include "MindWaveDevice.h"
#include "OpenEEGDevice.h"
#include "GUI/GUIMouse.h"
#include "GUI/GUIElementList.h"
#include "GUI/GUIScrollPanel.h"
#include "GUI/GUINFBPanel.h"
#include "GUI/GUICheckBox.h"
#include "GUI/GUIRollButton.h"
#include "GUI/GUIRadioButton.h"
#include "GUI/GUIXYPicker.h"
#include "GUI/GUIScroller.h"
#include "ImageLibrary.h"
#include "BoostRelativePath.h"
#include "AudioEnvironment.h"
#include "Session.h"
#include "BgEffect.h"

using namespace boost::filesystem;
using namespace audiere;
using namespace std;



/**
 * \brief PlayerCommon.h ...::: Player Commons - All is here :::...
*
* This class holds the variable definitions of globally used classes such as BWManager, BWMeter, musicLibrary and GUIElements.
*
* author: tonis @ 2013
*
*/


class PlayerCommon
{
public:
    PlayerCommon();

    virtual ~PlayerCommon();

protected:
private:
};

//! NOTICE For future: Represent the value here and also in .cpp file. These are for global usage.
namespace plrCommon
{

//! Our nfb session!
extern Session session;

//! Configuration for program.
extern Config config;

//! MindWaveDevice which is used in the program to get data from mindwave.
extern MindWaveDevice mindWaveDevice;

//! Open EEG Device .. This is Olimex specifically.
extern OpenEEGDevice openEEGDevice;

//! Audio environment which playes all the audio for Neurofeedback etc.
extern AudioEnvironment audioEnvironment;

//! The session for neurofeedback.
extern Session session;

//! Set up Brainwave Manager
extern BWManager bwManager;

//! Set up statistical view.
extern BWStatsView bwStatsView;

//! Variable to tell which control panel is visible.(standard playmode-panel, djmode panel, settings panel).
extern int controlPanelMode;

//! Our player's list of all components.
extern GUIElementList elementList;

//! Our common display
extern ALLEGRO_DISPLAY *display;

//! Our triple buffer..
extern ALLEGRO_BITMAP *tripleBuffer;

//! Image library also as extern to help loading all images and destroying them!
extern ImageLibrary imageLibrary;

//! Some fonts that are used commonly.
extern ALLEGRO_FONT *buttonFont;

//! The size of the button font.
extern int buttonFontSize;

//! Font again used in playlist?.
extern ALLEGRO_FONT *libraryFont;

//! Our common mouse for gui to help this world a bit again..
extern GUIMouse guiMouse;

//! Global variables..
extern AudioDevicePtr device;

//! Player log text collected in this variable..
extern string playerLogTxt;

//! Music library file. All music and some raw information abour recorded brainwaves.
extern string musicLibraryFileName;

//! Name of the result file..
extern string resultsFileName;

//! Name of the result file..
extern string resultsFileName2;

//! Name of the configuration file.
extern string configurationFileName;

//! Folder for saved brainwaves.
extern string BWFolder;

//! Time between each frame..
extern double deltaTime;

//! Last autosave check.
extern double lastAutoSaveTime;

//! Globally told song position
extern int bwPositionInSec;

//! Takes away excess decimals from giben number with giben amount of decimals.
extern double showDecimals(const double& x, const int& numDecimals);

//! Global function that changes integer to string.
string intToStr(int number);

//! Changes float to String repsresentation.
string desToStr(double number);

//! Changes str to int.
int strToInt(string strToConvert);

//! Checks wether giben filepath is proper music file.
bool isValidAudioFile(path p);

//! Font loading function.
bool loadFonts();

//! Font unloading functin.
bool unLoadFonts();

//! Player log. This is used everywhere to print debug information.
/*!
      Details: Give string to log something.
*/
void playerLog(string s);

//! Calculates distance between points 1 and 2.
/*!
      Details: Give point 1 x and y and point 2 x and y and function returns the distance.
*/
float getDistance(float x,float y,float x2,float y2);

//! Remove double empty spaces like these "  ".
char* removeDoubleEmptySpaces(string txt);

//! This tells whether we'r in fullscreen or in windowed mode.
extern bool fullScreen;

//! RAW Data EEG record switch. RAW Data is a bit memory consuming to save. If not chosen only spectrum data is saved.
extern GUICheckBox recordRAWData;

//! Minimum place ie 25% before avarage.
extern GUIRollButton minimumPlace;

//! Maximum place ie. 25% after avarage.
extern GUIRollButton maximumPlace;

//! Calibration time ie 30seconds to calibrate Neurofeedback for openeeg or mindwave.(baseline)
extern GUIRollButton calibrationTime;

//! Prize threshold ie. 75% of minimum to maximum distance to reach prize. Training to get more powerful alfa values or similar.
extern GUIRollButton prizeThreshold;

//! Time of next prize minimum.
extern GUIRollButton timeOfNextPrizeMin;

//! The actual session length.
extern GUIRollButton sessionLength;

//! Amount of prizes per level. Next level will be changed after enough of prizes is reached.
extern GUIRollButton amountOfPrizesPerLevel;

//! Time of next prize minimum. Ie prize cannot be given before next 20 seconds.
extern GUIRollButton timeOfNextPrizeMin;

//! Some checkboxes for common usage in setup panel:)
extern GUICheckBox showDividedBox;

//! MindWave Port options.
extern GUIRollButton mindWavePortRollButton;

//! Scaling for mindwave values.
extern GUIRollButton mindWaveScaleFactor;

//! Reboot mindwave.
extern GUIButton reBootMindWave;

//! OpenEEG Port and reboot.
extern GUIRollButton OpenEEGPortRollButton;

//! Reboot Open EEG device.
extern GUIButton reBootOpenEEG;

//! Show visualization checkbox.
extern GUICheckBox showVisualization;

//! Show mirror down there?
extern GUICheckBox showMirror;

//! Playlist to show songs for choosing song and editing the list..
extern GUINFBPanel NFBPanel;

//! Scroller for song position.. length was 440 originally
extern GUIScroller bwScrollBar;

//! Volume rollbutton after song scrollbar in the right corner
extern GUIRollButton guiVolumeRollButton;

//! Exit, resize, minimize
extern GUIButton exitButton;
extern GUIButton resizeButton;
extern GUIButton minimizeButton;

//! Playbutton and pause button :)
extern GUIButton playButton;
extern GUIButton pauseButton;

//! Record buttons
extern GUIButton recordButton;
extern GUIButton recordPauseButton;

//! Restart button
extern GUIButton restartButton;

//! These are the three big page-options to choose [Record(normal play)][NFBPanel][Configurations]
extern GUIButton StatisticModeButton;
extern GUIButton RecordModeButton;
extern GUIButton NFBConfigureButton;
extern GUIButton ConfigureButton;

//! File handling.
extern GUIButton bwFileOpenButton;
extern GUIButton bwFileSaveButton;
extern GUIButton bwFileNewButton;


//! Different mode choosing radio-choose menu. Modes for BCI or user chosen options.
extern GUIRadioButton djModeRadioButton;

//! Options to choose.. Added later in the main-void to the radiobutton. See main.cpp.
extern GUICheckBox    djModeOp1;
extern GUICheckBox    djModeOp2;

/*
extern GUIRadioButton activityMeterRadioButton;
extern GUICheckBox    actOp1;
extern GUICheckBox    actOp2;
*/
//! Neurofeedback mode options.
extern GUIRadioButton nfbTraineeModeRadioButton;
extern GUICheckBox    traineeOp1;
extern GUICheckBox    traineeOp2;


//! EEGDevice options.
extern GUIRadioButton EEGDeviceModeRadioButton;
extern GUICheckBox    EEGDevicegOp1;
extern GUICheckBox    EEGDevicegOp2;
extern GUICheckBox    EEGDevicegOp3;

//! Resultmode options.
extern GUIButton renderStatisticButton;
extern GUIRadioButton resultModeRadioButton;
extern GUICheckBox    resultModeRadioButtonOp1;
extern GUICheckBox    resultModeRadioButtonOp2;


//! Visualization mode. There are some fun things in these. Whether o show it with the GUI usual elements or not. I don't remember anymore how did this work.
extern bool onTopVisualization;

//! Wtf is this..
extern float bwManagerFade;

//! This is the Brainwave Meter which is interface for data to program from devices.
extern BWMeter bwMeter;

//! Parameter to tell error on com port opening.
extern bool SerialOpenErr;

//! Attributes used in main mostly..

/* Keep track of pressed mouse button. */
extern int mouse;

//! Get the minimum values of integer of whatever we need here
extern double dmin; //! minimum value
extern double dmax;

/* Simple FPS counter. */
extern int fps, fps_accum;
extern double fps_time;
extern double totalFrameTime;
//! Important global functions to cause smooth movement etc.
extern double lastUpdate;
extern double lastGfxUpdate;
extern double gfxDesiredFps; // This is the main gfx desired fps. Ie 80 frames per second for drawing again and again.
extern double pauseStartTime;

extern bool environmentPlaying;
extern int pausedAtPoint;

extern int down;
extern int down_x;
extern int down_y;

//! This is the panel text "Normal record mode-panel" "Dj Mode-panel" "..-panel"
extern string panelText;

extern ALLEGRO_BITMAP *graphBaseImg;
extern ALLEGRO_BITMAP *iconImg;

//! Player samplesource and stream for streaming audio
extern SampleSourcePtr samplesource;
extern OutputStreamPtr stream;

extern double songNumberScreenFadeAmount;

extern float mirrorBottom;

extern float songInfoTextX;

//! Allegro bitmaps. For background
extern ALLEGRO_BITMAP *bgImg;
extern ALLEGRO_BITMAP *copyImg;

extern ALLEGRO_TIMER *timerMs;

//! Debug input which is random just for testing..
extern bool useDebugInput;

//! Background visualization..
extern BgEffect backgroundVisualization;

//! Function to get scaled image of giben image.
ALLEGRO_BITMAP* getScaledImage(ALLEGRO_BITMAP* src, float scaleFactorX, float scaleFactorY);

//! Nice function to split string or something. Where did I get this? Anyway thank you!
class splitstring : public string
{
    vector<string> flds;
public:
    splitstring(const char *s) : string(s) { };
    vector<string>& split(char delim, int rep=0);
};

//! Draw glowing dots.. Ahh Staars.. I can see them! @_@ *_* This is used to draw the glowing bars in the meters next to rollbuttons.
void drawGlowDot(float px, float py, float glowValue);

//! ^  Mostly same. This is used to draw the glowing bars in the meters next to rollbuttons.
void drawPowerMeter(float px, float py, float width, float height, float value);


//! Get positive value.
float getPositiveOfThis(float num);

//! Get negation of this.
float getNegationOfThisPrefix(float num);

//! Get's the prefix +- from value.
float getPrefixOf(float num);

//! Draws scaled picture of given image.
void drawScaledPicture(ALLEGRO_BITMAP *img, float dx, float dy, float width, float height);

//! Draws rotated tinted picture.
void drawRotatedTintedPicture(ALLEGRO_BITMAP *img, ALLEGRO_COLOR tint, float dx, float dy, float angle);

//! Draw rotated picture.
void drawRotatedPicture(ALLEGRO_BITMAP *img, float dx, float dy, float angle);

//!  Draw tinted picture with given color.
void drawTintedPicture(ALLEGRO_BITMAP *img, ALLEGRO_COLOR color, float x, float y, int flags);

//!  Draw tinted picture with given color and region.
void drawTintedPictureRegion(ALLEGRO_BITMAP *img, ALLEGRO_COLOR color, float sX, float sY, float sW, float sH , float xDestPos, float yDestPos, int flags);

//! Delta-value of values.
float getDistanceBetweenValues(float val1, float val2);

//! Limits upper part of value.
float limitValueUp(float tval, float limitV);

//! Limits value down.
float limitValueDown(float tval, float limitV);

//! Limits value inside both upper and down part.
void getValueInsideBoundaries(float& tval, float limitVDown, float limitVUp);

//! Remove excess characters. Used to cut middle character from too large string. Useful when we can assume that in the middle there is not so importatn info. Artist - album - songtitle <- album is not so important.
string removeExcessFromMiddle(string sourceStr, int limitStrSize);

//! Get's the 2D-map color according desired visualization of given values 0-1f , 0-1f
ALLEGRO_COLOR getMapColor(float valX, float valY);

//! Cumulates value n^2 or something a bit..
float cumulateValue(float valR);

//! Get's timestamp.
string getTimeStamp();

//! Remove asked characters. Thank you Stack overflow people!
string removeChars(string str, char *chars);

//! Same but uses more force :D.
string forceRemoveAllDoubleEmptySpaces(string txt);

//! Get frequency of asked index in spectrum.
double Index2Freq(int i, double sampleRate, int n);

//! Get index of asked frequency in spectrum.
int Freq2Index(double freq, double sampleRate, int n);

//! Get things done.
int getTrainingType();

//! ..
int getCurrentLevel();

//! Last reached level
int getLastReachedLevel();

//! Ssession running?
bool isSessionOn();

//! Jeah! Get time stamp now.
string getTimeStampNow();

//! Easy way to ask which device is used.
bool usingOpenEEG();

//! How many channels in openEEG to use now?
int howManyChannelsInOpenEEG();

//! Easy way to ask which device is used.
bool usingMindWave();

extern double getEffectiveValueOfEEG();

bool strToBool(string str);
string boolToStr(bool val);

//! These are player functionality stuff, but declared here so they can be later used anywhere in the project.
/*void saveBWTable(string bwFileFullPath);
void initializeBrainWaveTable();
void loadBWTable(string bwFileFullPath);
void renderResultsToFile(string path);
void saveExperiment();
void autoSaveSession();*/

};

#endif //! PLAYERCOMMON_H

