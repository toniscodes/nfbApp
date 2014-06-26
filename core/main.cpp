//#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <math.h>
#include <limits>
#include <string>

#include <audiere.h>

#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_primitives.h"

#include "include/PlayerCommon.h"

#include "include/GUI/GUIMouse.h"
#include "include/GUI/GUIScrollPanel.h"
#include "include/GUI/GUIButton.h"
#include "include/GUI/GUICheckBox.h"
#include "include/GUI/GUIScroller.h"
#include "include/GUI/GUIRadioButton.h"
#include "include/GUI/GUIRollButton.h"

#include "include/BWManager.h"
#include "include/BWMeter.h"
#include "include/BgEffect.h"

#include "include/PlrFunctionality.h"
#include "include/PlrGUI.h"

using namespace audiere;
using namespace boost::filesystem;
using namespace std;
using namespace plrCommon;


int main(int argc, char **argv)
{

    // Initializing different things for start.
    playButton.hideAndUnactivate();
    recordPauseButton.hideAndUnactivate();

    // Do we have audiodevice ready also?
    if (!device)
    {
        playerLog("Failed to load audiosystem!");// failure
    }

    ALLEGRO_TIMER *timer;
    ALLEGRO_EVENT_QUEUE *queue;
    bool redraw = true;

    srand(time(NULL));

    /* Init Allegro addons. */
    al_init();
    al_init_image_addon();
    al_init_primitives_addon();
    al_install_mouse();
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();


    if(!al_init())
    {
        playerLog("failed to initialize allegro!\n");
        return -1;
    }

    if(!al_init_image_addon())
    {
        playerLog("Failed to initialize al_init_image_addon!\n");
        return -1;
    }

    // Frameless but it's not working in this version!.. I should try later in new allegro?
    al_set_new_display_flags(ALLEGRO_NOFRAME);

    if (fullScreen)
    {
        al_set_new_display_flags(ALLEGRO_FULLSCREEN);
        display = al_create_display(1024, 768);
        mirrorBottom = 570;
    }
    else
    {
        al_set_new_display_flags(ALLEGRO_WINDOWED);
        display = al_create_display(1000, 600);
        mirrorBottom = 565;
    }

    if(!display)
    {
        playerLog("failed to create display!\n");
        return -1;
    }

    // Make the program to run in the background also..
    //set_display_switch_mode(SWITCH_BACKGROUND);

    // Asettaa ikonin, titlen jne ikkunaan
    iconImg = imageLibrary.loadImage("media/gfx/programWindowIcon.png");
    al_set_display_icon(display,iconImg);

    al_set_window_title(display, "Neurofeedback Trainer");
    //al_set_window_position(display,0, 0);


    // Here we load all fonts for player like button fonts, normal fonts, tooltip whatever fonts to avoid instantiating them multiple times..
    if (!loadFonts())
        return -1;

    timer = al_create_timer(1.0f / 150.0f); // This is the FPS .. 1 / FPS -> ie 120 means 1 / 120 so it's 120 frames per second.
    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_start_timer(timer);

    // *** Adding all checkboxes into radiobuttons ***


    // Add buttons for neurofeedback mode:
    nfbTraineeModeRadioButton.addOption(&traineeOp1);
    nfbTraineeModeRadioButton.addOption(&traineeOp2);
    nfbTraineeModeRadioButton.setCheckedIndex(1);

    // Add buttons for EEG Device choise:
    EEGDeviceModeRadioButton.addOption(&EEGDevicegOp1);
    EEGDeviceModeRadioButton.addOption(&EEGDevicegOp2);
    EEGDeviceModeRadioButton.addOption(&EEGDevicegOp3);
    EEGDeviceModeRadioButton.setCheckedIndex(2);

    // Initialize components. Fonts, images etc that needs allegro..
    elementList.initElements();

    // Load old configurations for elements.
    config.load();

    playerLog("Initializing complete..\n");

    // Show elements info just for fun .. before it was to see z-ordering. :)
    elementList.showElementsInfo();

    // Choose the initial panel to show..
    showPanel(RECORD_PLAY_PANEL);

    // Triplebuffering. This should be changed later If optimization is desired!
   // tripleBuffer = imageLibrary.createImage(al_get_display_width(display), al_get_display_height(display));
    tripleBuffer = al_create_bitmap(al_get_display_width(display), al_get_display_height(display));

    // Background image.
    bgImg = imageLibrary.loadImage("media/gfx/playerBg1.png");

    // Load graphic base image.
    graphBaseImg = imageLibrary.loadImage("media/gfx/graphBase.png");

    // Triple buffer as target.
    al_set_target_bitmap(tripleBuffer);

    // Initialize the audio environment. This is loaded once in elementlist call.
    //audioEnvironment.init();

    // Thses should be moved to player functionality?
    initializeBrainWaveTable();

    // Do autoloading If session was interrupted.
    autoLoadSession();

    /* * * * * * * * * *
    * Program Main Loop *
    * * * * * * * * * */
    while (1)
    {

        al_set_target_bitmap(tripleBuffer);

        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);
        //al_get_next_event(queue, &event);

        guiMouse.mouseUpdate(&event);

        // These keys should be moved to plrGUI? They are a kind of gui actions..

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            break;
        if (event.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                break;

            // ### DEMO AND EXPERIMENT KEYS ->

            if (event.keyboard.keycode == ALLEGRO_KEY_F1)   // Remove old stuff and replace with the initialized folder for brainwaves, mlibrary file and remove result.txt
            {
                //initializeExperiment();
                // Load different configs.
            }

            if (event.keyboard.keycode == ALLEGRO_KEY_F2)   // Print results to file and copy everything.. Doesn't remove though :)
            {
                config.load("config_gamma_med.txt");
            }

            if (event.keyboard.keycode == ALLEGRO_KEY_F3)   // Print results to file and copy everything.. Doesn't remove though :)
            {
                config.load("config_theta_alfa_med.txt");
            }


            if (event.keyboard.keycode == ALLEGRO_KEY_PAD_PLUS)   // NFB Session load.
            {
                if (getCurrentLevel()<AMOUNT_OF_LEVELS-1)
                {
                    session.nextLevel();
                }
            }

            if (event.keyboard.keycode == ALLEGRO_KEY_MINUS)   // NFB Session save.
            {
                if (getCurrentLevel()>0)
                {
                    session.previousLevel();
                }
            }


            if (event.keyboard.keycode == ALLEGRO_KEY_F7)   // Print results to file and copy everything.. Doesn't remove though :)
            {
                session.endNeurofeedBackSessionInExperiment();
            }

            if (event.keyboard.keycode == ALLEGRO_KEY_G)
            {

                //gatherExperimentData();

            }

            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
            {

                //analyzeExperimentData();

            }

            // EXPERIMENT KEYS ### ^

        }

        // Allegro functions.

        // Draw again If desired fps time reached.
        if (event.type == ALLEGRO_EVENT_TIMER && al_get_time()-lastGfxUpdate>gfxDesiredFps) {
            redraw = true;
        }

        if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE)
        {
            al_acknowledge_resize(display);
            redraw = true;
        }

        // Special thing for this dragging
        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            if (event.mouse.button == 1 && event.mouse.x)
            {
                down = true;
                down_x = event.mouse.x;
                down_y = event.mouse.y;
            }
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
        {
            if (event.mouse.button == 1)
                down = false;
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_AXES)
        {
            if (down && down_y < 30)
            {
                int cx, cy;
                if (al_get_mouse_cursor_position(&cx, &cy))
                {
                    al_set_window_position(display, cx - down_x, cy - down_y);
                }
            }
        }
        // <- Dragging of window..

        // Update All Components And Element States
        elementList.refresh();

        // NORMAL KEYS -->
        // Handle most of the events and actions user may do with the editor..

        if (guiVolumeRollButton.valueWasChanged()) {
            if (guiVolumeRollButton.getValue()>0.0f) {
                //audioEnvironment.play();
                playButton.hideAndUnactivate();
                pauseButton.showAndActivate();
            }
        }

        if (recordButton.wasClicked())
        {
            // ACtion to record..
            recordButton.hideAndUnactivate();
            recordPauseButton.showAndActivate();
            //bwManager.startRecording();
            //audioEnvironment.play();
            session.start();
        }
        else if (recordPauseButton.wasClicked())
        {
            // Action to stop recording..
            recordButton.showAndActivate();
            recordPauseButton.hideAndUnactivate();
            //bwManager.stopRecording();
            //audioEnvironment.stop();
            session.pause();
        }


        if (bwFileNewButton.wasClicked())
        {
            initializeBrainWaveTable();
        }
        if (bwFileOpenButton.wasClicked())
        {
            loadBWTable(openFileImportDialogAndReturnPath("Choose file to load brainwaves fron.",ALLEGRO_FILECHOOSER_FILE_MUST_EXIST,"*.bwr2;*.bwr;*.*"));// Load bw data?// Load bw data?
        }
        if (bwFileSaveButton.wasClicked())
        {
            saveBWTable(openFileImportDialogAndReturnPath("Choose file to save brainwaves into.",ALLEGRO_FILECHOOSER_SAVE,"*.bwr2;*.bwr;*.*"));// Load bw data?
        }

        //if (traineeOp1.wasClicked())
        if (nfbTraineeModeRadioButton.getRadioOptionChanged())
            audioEnvironment.modeChanged();

        if (restartButton.wasClicked()) {
            session.restart();
        }

        // Refresh NFB-Session.
        session.refresh();

        updateAutoSave();

        // Mode-Buttons for changing controlPanelMode
        if (RecordModeButton.wasClicked())
        {
            showPanel(RECORD_PLAY_PANEL);
        }
        if (NFBConfigureButton.wasClicked())
        {
            showPanel(NFB_PANEL);
        }

        if (ConfigureButton.wasClicked())
        {
            showPanel(SETUP_PANEL);
        }

        // Reboot mindwave button
        if (reBootMindWave.wasClicked())
            mindWaveDevice.forceLoadThinkGearConnection();

        if (reBootOpenEEG.wasClicked())
            openEEGDevice.init();


        if (exitButton.wasClicked())
            break;

        // Resizing from the resize button or from F5
        //if (resizeButton.wasClicked() || event.keyboard.keycode == ALLEGRO_KEY_F5)
        //if (event.keyboard.keycode == ALLEGRO_KEY_F5)
        if (resizeButton.wasClicked())
        {
            //if (resizeButton.wasClicked())
              //  playerLog("Did you press really resize button? :D");

            if (fullScreen)
            {
                al_set_new_display_flags(ALLEGRO_WINDOWED);
                fullScreen = false;
            }
            else
            {
                al_set_new_display_flags(ALLEGRO_FULLSCREEN);
                fullScreen = true;
            }

            unLoadFonts();
            imageLibrary.destroyImages();
            elementList.unLoadElements();
            al_set_target_bitmap(NULL);
            al_destroy_display(display);

            if (fullScreen)
                display = al_create_display(1024, 768);
            else
                display = al_create_display(1000, 600);

            if(!display)
            {
                playerLog("failed to create display!\n");
                return -1;
            }
            loadFonts();
            tripleBuffer = al_create_bitmap(al_get_display_width(display), al_get_display_height(display));
            bgImg = imageLibrary.loadImage("media/gfx/playerBg1.png");
            graphBaseImg = imageLibrary.loadImage("media/gfx/graphBase.png");
            elementList.initElements();
        }


        // Changed volume? Update :)!
        if (guiVolumeRollButton.valueWasChanged() && stream)
        {
            // Volume..
            //stream->setVolume(guiVolumeRollButton.getValue()); // Set volume according rolled value
        }

        // Update delta time.. According the main update time, not the graphical update time.
        if (lastUpdate>0) {
            // deltaTime is time in seconds. s  usually in accuracy of milliseconds (double)
            deltaTime = al_get_time()-lastUpdate;
            //playerLog("deltaTime " + desToStr(deltaTime));
        }
        else
            deltaTime = 0;

        // The main update time
        lastUpdate = al_get_time();

        /************************
        * UPDATE GRAPHICS / GUI *
        *************************/
        if (redraw && al_is_event_queue_empty(queue))   // Normal update
        {
            updateMainGUI();
            redraw = false; // Wait for next graphic update..
        }

    }

    // Quit->

    // Save configurations.
    config.save();

    // Destroy and unload all gui elements etc.
    elementList.unLoadElements();
    mindWaveDevice.closeDevice();
    openEEGDevice.closeDevice();
    al_destroy_display(display);
    imageLibrary.destroyImages();
    al_destroy_bitmap(copyImg);

    // I'm done! ..
    return 0;
}
