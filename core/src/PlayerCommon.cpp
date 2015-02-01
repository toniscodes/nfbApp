#include <sstream>
#include <stdio.h>
#include <math.h>
#include <limits>
#include <string>
#include <boost/filesystem.hpp>
#include "../include/PlayerCommon.h"
#include "../include/GUI/GUIMouse.h"
#include "../include/GUI/GUIElementList.h"
#include "../include/GUI/GUIScrollPanel.h"
#include "../include/GUI/GUICheckBox.h"
#include "../include/BWMeter.h"
#include "../include/thinkgear.h"
#include <audiere.h>

using namespace boost::filesystem;
using namespace audiere;
using namespace std;


PlayerCommon::PlayerCommon()
{
    //ctor
}

PlayerCommon::~PlayerCommon()
{
    //dtor
}

namespace plrCommon
{

    // Configuration for program.
    Config config;

    // Our nfb session!
    Session session;

    // Variable to tell which control panel is visible.
    int controlPanelMode;

    // Our list of elements.. To help them drawing and refreshing more handy.
    GUIElementList elementList;

    // Our common display
    ALLEGRO_DISPLAY *display = NULL;

    // Triple Buffer
    ALLEGRO_BITMAP *tripleBuffer = NULL;

    // Our image loader..
    ImageLibrary imageLibrary;

    // The delta time to tell difference between frames and sync everything nicely..
    double deltaTime=0;

    // Song position that can be readed from any class..
    int bwPositionInSec=0;

    // Last autosave check.
    double lastAutoSaveTime = 0;

    // Our global mouse!
    GUIMouse guiMouse;


    // Global variables(notice that we have to give still the type again):
    string playerLogTxt = "";

    // Brainwaves folder, for bw data..
    string BWFolder = "brainwaves/";

    // The results txt file
    string resultsFileName = "results.txt";

    // The results txt file 2
    string resultsFileName2 = "results2.txt";

    // The Configuration file name.
    string configurationFileName = "config.txt";

    // To use debug input for testing.
    bool useDebugInput = false;

    // Player's audio device
    AudioDevicePtr device(OpenDevice());

    // The font.. As it must be said that it's here defined.. njah.. I
    ALLEGRO_FONT *buttonFont;
    ALLEGRO_FONT *libraryFont;

    int buttonFontSize;

    bool fullScreen=false;

    float bwManagerFade = 0.0f;

    // Audio environment initial
    AudioEnvironment audioEnvironment;

    /* * * * * * * * * * * * * * * * * * * *
    * S e t u p  P a n e l and GUI ELEMENTS *
    ****************************************/

    GUICheckBox recordRAWData(487+250, 122+15*20, "Record RAW EEG Data", false, "Enable RAW-eeg reading record. This takes more memory.", STORABLE);

    GUIRollButton mindWavePortRollButton(487+300, 170+15*6, 20,"MindWave Search Start Port.", 0, 30, " COM", 3, "Use this to choose right port for your MindWave.", STORABLE);

    GUIRollButton mindWaveScaleFactor(400+180, 170+15*6, 20,"MindWave Amplitude Scaling.", 0.0f, 10.0f, " x", 3.0f, "Use this to scale amplitude of graphs", STORABLE);

    GUIButton reBootMindWave(487+320, 170+13*9.5, 28, 28, 15, "media/gfx/mindWaveBoot.png", "Reboot MindWave!", "Reboot mindwave(rebooting this software may also help).");

    GUIRollButton OpenEEGPortRollButton(487+300, 255+15*6, 20,"OpenEEG Port.", 0, 30, " COM", 2, "Use this to choose right port for your OpenEEG.", STORABLE);

    GUIButton reBootOpenEEG(487+320, 255+13*9.5, 28, 28, 15, "media/gfx/mindWaveBoot.png", "Reboot Open EEG!", "Reboot Open EEG.");

    GUIRollButton minimumPlace(460+70, 180+15*2, 25,"Minimums' place before avg.", 0.0f, 30.0f, " 1%", 10.0f, "Minimum is placed x% before avarage value.", STORABLE);

    GUIRollButton maximumPlace(460+70, 180+15*6, 25,"Maximums' place after avg.", 0.0f, 30.0f, " 1%", 10.0f, "Maximum is placed x% after avarage value.", STORABLE);

    // Remember to make check that each id is existing in configure some day or just always remember to remove the old configuration If doing updates to storable guis!!!!!

    GUIRollButton calibrationTime(487+300, 180+15*2, 25,"Calibration time.", 0.0f, 300.0f, " s", 120.0f, "Calibration time in seconds.", STORABLE);

    GUIRollButton prizeThreshold(487+300,  180+15*6, 25,"Prize threshold.", 0.0f, 100.0f, " 1%", 25.0f, "Threshold to give prize(between min-max)%.", STORABLE);

    GUIRollButton sessionLength(487+300, 180+15*10, 25,"Session length.", 0.0f, 60.0f, " min", 28.0f, "Session length in minutes.", STORABLE);

    GUIRollButton timeOfNextPrizeMin(460+70, 180+15*10, 25,"Minimum next prize time.", 0, 120, " sec", 25, "Time before next prize is allowed to be given.", STORABLE);

    GUIRollButton amountOfPrizesPerLevel(460+70, 180+15*14, 25,"Amount of prices per level.", 0, 25, " prizes", 10, "How many prizes must be reach before next level.", STORABLE);

    GUICheckBox showVisualization(500, 122+15*20, "Show Visualization", false, "Show visualization.");
    GUICheckBox showMirror(500, 122+15*21, "Show Mirror", true, "The mirror effect below. Off to increase performance.");

    // Playlist to show songs for choosing song and editing the list..
    GUINFBPanel NFBPanel(62,125,20,352,20,12,"Neurofeedback panel showing bw values.");

    // Scroller for song position.. length was 440 originally
    GUIScroller bwScrollBar(414+65,467+10,HORISONTAL_AXIS,497,15,500/10, "Timeline of the graph.");

    // Volume rollbutton after song scrollbar in the right corner
    GUIRollButton guiVolumeRollButton(414+94+37+340+30,520+10,20,"Volume", "Adjust volume.", STORABLE); // Rouull

    // Exit, resize, minimize
    GUIButton exitButton(1000-2-25, 6, 25, 25, 15,"media/gfx/quitIcon.png", "Exit.");
    GUIButton resizeButton(1000-4-25*2, 6, 25, 25, 15,"media/gfx/resizeScreenIcon.png", "Resize.");
    GUIButton minimizeButton(1000-6-25*3, 6, 25, 25, 15,"media/gfx/minimizeIcon.png", "Minimize.");

    // Playbutton :)
    GUIButton playButton(414+94+37+320,520+20,25, "media/gfx/speaker_muted_icon.png", "Turn audio on.");
    GUIButton pauseButton(414+94+37+320,520+20,25, "media/gfx/speaker_icon.png", "Turn audio off.");

    GUIButton recordButton(414+94+20,520+10,35, "media/gfx/record_play_icon.png", "Record Brainwaves.");
    GUIButton recordPauseButton(414+94+20,520+10,35, "media/gfx/pause_icon.png", "Pause Recording.");

    GUIButton restartButton(414+94+100,520+18,25, "media/gfx/restart.png", "Restart neurofeedback session.");

    GUIButton StatisticModeButton(725-68-4, 54, 68, 28, 15, "media/gfx/StatisticMode.png", "Statistics of sessions.");
    GUIButton RecordModeButton(725, 54, 68, 28, 15, "media/gfx/RecordMode.png", "Record or play brainwaves.");
    GUIButton NFBConfigureButton(725+68*1+4*2, 54, 68, 28, 15, "media/gfx/NFBConf.png", "Neurofeedback configuration.");
    GUIButton ConfigureButton(725+68*2+4*2, 54, 68, 28, 15, "media/gfx/Configure.png", "Configuration panel.");

    GUIButton bwFileNewButton(56, 55, 28, 28, 15,"media/gfx/file_new_icon.png", "New brainwave file.");
    GUIButton bwFileOpenButton(56+35,55, 28, 28, 15,"media/gfx/file_load_icon.png", "Open brainwave record.");
    GUIButton bwFileSaveButton(56+35*2,55, 28, 28, 15,"media/gfx/file_save_icon.png", "Save brainwave record.");

    // Options for nfb
    GUIRadioButton nfbTraineeModeRadioButton(487+10, 112+12*1, "Neurofeedback mode", "Different training options for neurofeedback.", STORABLE);
    GUICheckBox    traineeOp1( "Gamma Meditation", "Meditation trains gamma.");
    GUICheckBox    traineeOp2( "Theta/Alpha Meditation", "This mode trains theta and alpha meditation." );
    //GUICheckBox    traineeOp2( "Avarage Meditation", "This mode trains to turn of all brainwave channels." );

    // Options for EEG -device.
    GUIRadioButton EEGDeviceModeRadioButton(780, 180, "EEG Input Device", "Choose device to use for EEG input.", STORABLE);
    GUICheckBox    EEGDevicegOp1( "MindWave", "NeuroSky MindWave.");
    GUICheckBox    EEGDevicegOp2( "Olimex EEG-SMT 1 Chan", "Olimex EEG-SMT 1 Channel(Open EEG)." );
    GUICheckBox    EEGDevicegOp3( "Olimex EEG-SMT 2 Chan", "Olimex EEG-SMT 2 Channels(Open EEG)." );

    // Background visualization..
    BgEffect backgroundVisualization(1000,600);

    bool onTopVisualization = false;

    // Set up Brainwave Manager and it's GUI (to the previously declared elementlist!)
    BWManager bwManager("Standard", 478, 220, 500, 260, "Different brainwave readings.");

    // Set up Brainwave statistical view and related buttons.

    BWStatsView bwStatsView("Standard", 478, 200, 500, 260, "Different progress values.");
    GUIRadioButton resultModeRadioButton(487+10, 112+12*1, "Result mode", "Choose which parameters to show.", STORABLE);
    GUICheckBox    resultModeRadioButtonOp1( "Avarage avg phase", "Show relative value of channel(s) related to other channels.");
    GUICheckBox    resultModeRadioButtonOp2( "Amplitude avg phase", "Show amplitude in graph." );
    GUIButton renderStatisticButton(725-34, 112+18, 68, 28, 15, "media/gfx/RenderStatistic.png", "Render statistic.");

    /* * * * *
    * E n d  *
    * * * * */

    // Create MindWave Device..
    MindWaveDevice mindWaveDevice;

    // Create Open EEG Device..
    OpenEEGDevice openEEGDevice;


    // Attributes used in main..
    double dmin = std::numeric_limits<double>::min(); // minimum value
    double dmax = std::numeric_limits<double>::max();

    /* Keep track of pressed mouse button. */
    int mouse;

    /* Simple FPS counter. */
    int fps, fps_accum;
    double fps_time;
    double totalFrameTime;
    // Important global functions to cause smooth movement etc.
    double lastUpdate=0;
    double lastGfxUpdate=0;
    double pauseStartTime;

    double gfxDesiredFps = 1.0f/160.0f; // Ie 80 fps. Put something like double the desired.. Because it's not tuned to work properly.

    bool environmentPlaying=true;

    int down;
    int down_x;
    int down_y;

    string panelText = "Chosen Panel Text";

    ALLEGRO_BITMAP *graphBaseImg;
    ALLEGRO_BITMAP *iconImg;

    // Player samplesource and stream for streaming audio
    SampleSourcePtr samplesource;
    OutputStreamPtr stream;

    float mirrorBottom;

    float songInfoTextX = 0;

    // Allegro bitmaps. For background
    ALLEGRO_BITMAP *bgImg   = NULL;
    ALLEGRO_BITMAP *copyImg = NULL;


    // Our common BWMeter to get some nice current Brainwave-data and show it on the gui :) Start it up candyy!
    BWMeter bwMeter(820,93, "Brainwave meter."); //795

    // Converts int to string. Very nice.
    string intToStr(int number)
    {
       stringstream ss;//create a stringstream
       ss << number;//add number to the stream
       return ss.str();//return a string with the contents of the stream
    }

    // Converts float to string. Very nice.
    string desToStr(double number)
    {
       stringstream ss;//create a stringstream
       ss << number;//add number to the stream
       return ss.str();//return a string with the contents of the stream
    }

    int strToInt(string strToConvert) {
        return atoi(strToConvert.c_str());
    }

    // Tells wether a given file in the path is a valid file for music or not..
    bool isValidAudioFile(path p) {
        bool answer  = false;
        if ( extension(p) == ".mp3" || extension(p) == ".ogg" || extension(p) == ".wav" ||
             extension(p) == ".flac" || extension(p) == ".aiff" || extension(p) == ".mod" ||
             extension(p) == ".s3m" || extension(p) == ".xm" || extension(p) == ".it" )
            answer  = true;
        else
            playerLog("Wasn't valid music file! isValidMusicPlayerFile returned false. ");
        return answer;
    }

    // Logs stuff.. Change this to allegro or windows console depending desire:)
    void playerLog(string s) {
        //playerLogTxt += s; // Add to log
        fprintf(stderr,(s+"\n").c_str());
    }

    bool unLoadFonts() {
        al_destroy_font(buttonFont);
        if (buttonFont){
            playerLog("Button font still alive. It shouldnt be...\n");
            return false;
        }

        al_destroy_font(libraryFont );
        if (libraryFont){
            playerLog("libraryfont font still alive. It shouldnt be...\n");
            return false;
        }

        return true;

    }

    bool loadFonts() {
        buttonFontSize = 8;
        buttonFont = al_load_ttf_font("pirulen.ttf",buttonFontSize,0 );
        if (!buttonFont){
            playerLog("Could not load 'pirulen.ttf'.\n");
            return false;
        }

        // Load font also at the same time..
        libraryFont = al_load_ttf_font("pirulen.ttf",12,0 ); //UnionBold
        if (!libraryFont){
            playerLog("Could not load 'pirulen.ttf '.\n");
            return false;
        }

        return true;
    }

    float getDistance(float x,float y,float x2,float y2) {
        float dx = x-x2;
        float dy = y-y2;
        // And using the Pythagoras we can achieve the distance between two points.. Clever :)
        return sqrt(dx*dx+dy*dy);
    }

    ALLEGRO_BITMAP* getScaledImage(ALLEGRO_BITMAP* src, float scaleFactorX, float scaleFactorY) {
        ALLEGRO_BITMAP* dest = NULL;
        float originalWidth=al_get_bitmap_width(src);
        float originalHeight=al_get_bitmap_height(src);
        //dest = imageLibrary.createImage(originalWidth*scaleFactorX, originalHeight*scaleFactorY);
        dest = al_create_bitmap(originalWidth*scaleFactorX, originalHeight*scaleFactorY);
        if(!dest) {
            playerLog("Failed to create destination image in copyScaledImage()-function..'.\n");
        }
        al_set_target_bitmap(dest);
        al_draw_scaled_bitmap(src, 0, 0, originalWidth, originalHeight, 0, 0, originalWidth*scaleFactorX, originalHeight*scaleFactorY, 0);
        al_set_target_bitmap(al_get_backbuffer(display));
        return dest;
    }


    // split: receives a char delimiter; returns a vector of strings
    // By default ignores repeated delimiters, unless argument rep == 1.
    vector<string>& splitstring::split(char delim, int rep) {
        if (!flds.empty()) flds.clear();  // empty vector if necessary
        string work = data();
        string buf = "";
        int i = 0;
        while (i < work.length()) {
            if (work[i] != delim)
                buf += work[i];
            else if (rep == 1) {
                flds.push_back(buf);
                buf = "";
            } else if (buf.length() > 0) {
                flds.push_back(buf);
                buf = "";
            }
            i++;
        }
        if (!buf.empty())
            flds.push_back(buf);
        return flds;
    }

    string forceRemoveAllDoubleEmptySpaces(string txt) {
        // I think this 10 trial will trim or remove all empty spaces :D
        for (int i=0;i<10;i++) {
            txt = string(removeDoubleEmptySpaces(txt));
        }
        return txt;
    }

    string removeExcessFromMiddle(string sourceStr, int limitStrSize)
    {
        if(sourceStr.length() < limitStrSize)
            return sourceStr;
        // Else.. Remove from middle.. Count how many excess and remove..
        int excess = sourceStr.length()-limitStrSize;
        int middlePoint = int(sourceStr.length()/2);
        int startPoint = middlePoint-int(excess/2);
        int endPoint = middlePoint+int(excess/2);
        if (startPoint<0 || endPoint>sourceStr.length())
            return "trimAndRemoveExcess..Too small string to handle.";
        return (sourceStr.substr(0,startPoint)+sourceStr.substr(endPoint,sourceStr.length()-endPoint));
    }

    // Hey does that hmm.. Is that destroyed.. damnd.. when it goes out of this function
    // No it is not As far as I tested it..
    char* removeDoubleEmptySpaces(string txt) {
        char *cstr = new char[txt.length() + 1];
        strcpy(cstr, txt.c_str());
        int len = (int)strlen(cstr);

        int i = 0;
        int j = 0;

        while (i != len-1) {
          if ( !(cstr[i] == ' ' && cstr[i+1] == ' ') )
            cstr[j++] = cstr[i];
          i++;
        }
        return cstr;
    }


    void drawGlowDot(float px, float py, float glowValue) {
        float alpha = 0.9f;
        al_draw_pixel(px, py,   al_map_rgba_f(0.0*alpha*glowValue, 0.0*alpha*glowValue, 1.0*glowValue*alpha, alpha));
        alpha = 0.2f;
        al_draw_pixel(px, py,   al_map_rgba_f(0.3*alpha*glowValue, 0.3*alpha*glowValue, 1.0*alpha*glowValue, alpha));
        alpha = 0.9f;
        al_draw_pixel(px-1, py, al_map_rgba_f(.0*alpha*glowValue, .0*alpha*glowValue, 1.0*glowValue*alpha, alpha));
        al_draw_pixel(px+1, py, al_map_rgba_f(.0*alpha*glowValue, .0*alpha*glowValue, 1.0*glowValue*alpha, alpha));
        al_draw_pixel(px, py-1, al_map_rgba_f(.0*alpha*glowValue, .0*alpha*glowValue, 1.0*glowValue*alpha, alpha));
        al_draw_pixel(px, py+1, al_map_rgba_f(.0*alpha*glowValue, .0*alpha*glowValue, 1.0*glowValue*alpha, alpha));

        alpha = 0.8f;
        al_draw_pixel(px-1, py-1, al_map_rgba_f(0.0*alpha, 0.0*alpha, 0.8*glowValue*alpha, alpha));
        al_draw_pixel(px+1, py-1, al_map_rgba_f(0.0*alpha, 0.0*alpha, 0.8*glowValue*alpha, alpha));
        al_draw_pixel(px-1, py+1, al_map_rgba_f(0.0*alpha, 0.0*alpha, 0.8*glowValue*alpha, alpha));
        al_draw_pixel(px+1, py+1, al_map_rgba_f(0.0*alpha, 0.0*alpha, 0.8*glowValue*alpha, alpha));

        alpha = 0.3f;
        al_draw_pixel(px-2, py, al_map_rgba_f(0.0*alpha, 0.0*alpha, 0.3*glowValue*alpha, alpha));
        al_draw_pixel(px+2, py, al_map_rgba_f(0.0*alpha, 0.0*alpha, 0.3*glowValue*alpha, alpha));
        al_draw_pixel(px, py-2, al_map_rgba_f(0.0*alpha, 0.0*alpha, 0.3*glowValue*alpha, alpha));
        al_draw_pixel(px, py+2, al_map_rgba_f(0.0*alpha, 0.0*alpha, 0.3*glowValue*alpha, alpha));
    }

    void drawPowerMeter(float px, float py, float width, float height, float value) {
        // Triangle
        float alpha = 0.7f;
        float heightNow = 1;
        float raiseHeightStep = height/width;
        int lightTheseBars = (width * value);
        int skipper=0;
        for (int ix = 0; ix < width; ix++) {
            if (skipper==4) {
                for (int iy = 0; iy < heightNow; iy++) {
                    if (ix<lightTheseBars)
                        al_draw_pixel(px+ix, py-iy,al_map_rgba_f(0.3*alpha, 0.3*alpha, 1.0*alpha, alpha));
                        //drawGlowDot(px+ix, py-iy,1.0f);
                    else //darker
                        //drawGlowDot(px+ix, py-iy,0.3f);
                        al_draw_pixel(px+ix, py-iy,al_map_rgba_f(0.0*alpha, 0.0*alpha, 0.7*alpha, alpha));
                }
            skipper=0;
            }
            skipper++;
            heightNow += raiseHeightStep;
        }
    }



    float getNegationOfThisPrefix(float num) {
        if (num>0.0f)
            return -1.0f;
        else
            return 1.0f;
    }

    float getPositiveOfThis(float num) {
        if (num<0.0f)
            return -num;
        else
            return num;
    }

    float getPrefixOf(float num) {
        if (num>0.0f)
            return 1.0f;
        else
            return -1.0f;
    }

    // Näis on jotain pimeää.. Ei voi tehdä näin muuttuja = limit (muuttuja, raja) .. outoa..
    float limitValueUp(float tval, float limitV) {
        if (tval>limitV)
            tval=limitV;
        return tval;
    }

    float limitValueDown(float tval, float limitV) {
        if (tval<limitV)
            tval=limitV;
        return tval;
    }

    // Koklataas tätä.. Aa ongelma oliki muualla.
    void getValueInsideBoundaries(float &tval, float limitVDown, float limitVUp) {
        float resultV = tval;
        if (resultV>limitVUp)
           resultV = limitVUp;
        if (resultV<limitVDown)
            resultV = limitVDown;
       //return resultV;
       // playerLog("value is " + desToStr(resultV) + " and down limit is " + desToStr(limitVDown2));
        tval = resultV;
    }

    float cumulateValue(float val) {
        val=val*100.0f;
        val*=val;
        val*=0.001f;
        if (val>1.0f)
            val = 1.0f;
        return val;
    }


    // Get color of the text color according this song's brainwave information. Ie calm song is different in color than energetic song.
    ALLEGRO_COLOR getMapColor(float valX, float valY) {

        ALLEGRO_COLOR theColor = al_map_rgb_f(1.0, 0.0, 0.0); // This is init color If no bw data is available.

        // These colors are cumulated a bit to make it look something.
        float amountOfActivity = valX;
        amountOfActivity = cumulateValue(amountOfActivity);

        float amountOfLiking = valY;
        amountOfLiking = cumulateValue(amountOfLiking);

        theColor = al_map_rgb_f(amountOfActivity, amountOfLiking, 0.25f);

        return theColor;
    }

    void drawRotatedTintedPicture(ALLEGRO_BITMAP *img, ALLEGRO_COLOR tint, float dx, float dy, float angle) {
        al_draw_tinted_rotated_bitmap(img, tint, al_get_bitmap_width(img)/2, al_get_bitmap_height(img)/2 , dx, dy, angle, 0);
    }


    void drawRotatedPicture(ALLEGRO_BITMAP *img, float dx, float dy, float angle) {
        al_draw_rotated_bitmap(img,  al_get_bitmap_width(img)/2, al_get_bitmap_height(img)/2 , dx, dy, angle, 0);
    }

    void drawScaledPicture(ALLEGRO_BITMAP *img, float dx, float dy, float width, float height) {
        int dflags = 0;
        al_draw_scaled_bitmap(img,0,0, al_get_bitmap_width(img), al_get_bitmap_height(img), dx, dy, width, height, dflags);
    }

    void drawTintedPicture(ALLEGRO_BITMAP *img, ALLEGRO_COLOR color, float x, float y, int flags) {
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ONE); // Nice tint
            al_draw_tinted_bitmap(img, color, x, y, flags);
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); // No tint for other drawing..
    }

    void drawTintedPictureRegion(ALLEGRO_BITMAP *img, ALLEGRO_COLOR color, float sX, float sY, float sW, float sH , float xDestPos, float yDestPos, int flags) {
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ONE); // Nice tint.. with region
            al_draw_tinted_bitmap_region(img, color, sX, sY, sW, sH, xDestPos, yDestPos, flags);
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); // No tint for other drawing..
    }

    float getDistanceBetweenValues(float val1, float val2) {
        float dist = val1 - val2;
        if (dist<0.0f)
            dist = -dist;
        return dist;
    }

    /* Function to control # of decimal places to be output for x */
    double showDecimals(const double& x, const int& numDecimals) {
        int y=x;
        double z=x-y;
        double m=pow(10,numDecimals);
        double q=z*m;
        double r=round(q);

        return static_cast<double>(y)+(1.0/m)*r;
    }


    string getTimeStamp() {
        // Get current date/time, format is YYYY-MM-DD-HH-mm-ss
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d-%H-%M-%S", &tstruct);
        // Damn boost!! Why you had to be so complicated. See! This is what you got! Now I'm using c++ basic stuff! This should also work on multiple platform.
        return string(buf);
    }

    // Modifie from stackoverflow example. Takes of the characters that are requested.
    string removeChars(string str, char *chars) {
       string strOrig = str;
       for (unsigned int i = 0; i < strlen(chars); ++i)
       {
          // you need include <algorithm> to use general algorithms like std::remove()
          str.erase (std::remove(str.begin(), str.end(), chars[i]), str.end());
       }
       playerLog("# Removing chars from string " + strOrig + " to result " + str);
       return str;
    }

    double Index2Freq(int i, double sampleRate, int n) {
      return (double) i * (sampleRate / n);
    }

    int Freq2Index(double freq, double sampleRate, int n) {
      return (int) (freq / (sampleRate / n));
    }


    // Session related:
    int getTrainingType() {
        if (nfbTraineeModeRadioButton.getSelectionTxt()==traineeOp1.getText()) { // Meditation first option
            return GAMMA_MEDITATION_TRAINING;
        } else {
            return THETA_ALPHA_MEDITATION_TRAINING;
        }
    }

    // Get last reached level in this session. From 1 - X
    int getLastReachedLevel() {
        return session.currentLevel+1;
    }

    // Get the current level in format 0-X.. always starting from the 0 If last level was reached. This is a hack during this experiment.
    int getCurrentLevel() {
        return session.currentLevel%(AMOUNT_OF_LEVELS);
    }

    bool isSessionOn() {
        return session.sessionOn;
    }

    string getTimeStampNow() {
        time_t rawtime;
        struct tm * timeinfo;
        char buffer[80];
        time (&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(buffer,80,"%d_%m_%Y-t-%I-%M-%S",timeinfo);
        std::string timeStampZ(buffer);
        return timeStampZ;
    }

    // This is to give some value to chew on for background effects ..
    double getEffectiveValueOfEEG() {
        return bwMeter.getCurrentEEGChannelSlowBarValue(CHANNEL_BETA)*2.0d-1.0d;
    }

    bool usingMindWave() {
        if (!usingOpenEEG()) // or EEGDeviceModeRadioButton.getSelectionTxt()==EEGDevicegOp1.getText()
            return true;
        else
            return false;
    }

    bool usingOpenEEG() {
        if (EEGDeviceModeRadioButton.getSelectionTxt()==EEGDevicegOp2.getText() || EEGDeviceModeRadioButton.getSelectionTxt()==EEGDevicegOp3.getText())
            return true;
        else
            return false;
    }

    int howManyChannelsInOpenEEG() {
        if (EEGDeviceModeRadioButton.getSelectionTxt()==EEGDevicegOp3.getText())
            return 2;
        else
            return 1;
    }

    bool strToBool(string val) {
        if (val=="1")
            return true;
        else
            return false;
    }

    string boolToStr(bool val) {
        if (val)
            return "1";
        else
            return "0";
    }

};




