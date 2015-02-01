#include "../include/PlrFunctionality.h"
#include "../include/PlrGUI.h"
#include "../include/PlayerCommon.h"
#include "../include/BoostCopyDir.h"

#define UN_ACTIVE -1

PlrFunctionality::PlrFunctionality()
{
    //ctor
}

PlrFunctionality::~PlrFunctionality()
{
    //dtor
}


namespace plrCommon
{

/* Flag can ba anything from
 ALLEGRO_FILECHOOSER_SAVE
 ALLEGRO_FILECHOOSER_FOLDER
 ALLEGRO_FILECHOOSER_FILE_MUST_EXIST
 ALLEGRO_FILECHOOSER_Fplapla
*/
string openFileImportDialogAndReturnPath(string textInBrowser, int flag, string types)
{
    ALLEGRO_FILECHOOSER *fchooser;
    path initialPath = boost::filesystem::current_path();
    fchooser = al_create_native_file_dialog(initialPath.string().c_str(),textInBrowser.c_str(),types.c_str(),flag);


    al_show_native_file_dialog(display,fchooser);
    int amountOfChosenPaths = al_get_native_file_dialog_count(fchooser);
    //playerLog("Amount of chosen files: " + intToStr(amountOfChosenPaths) + "\n");
    if (amountOfChosenPaths>0)
    {
        const char *chosenPath = al_get_native_file_dialog_path(fchooser, 0); // Pick first..
        if (chosenPath != NULL)
        {
            return chosenPath;
        }
        else
            playerLog("Null was given from the filechoser..");
    }
    else
    {
        playerLog("No files chosen..");
    }
    al_destroy_native_file_dialog(fchooser);

    return "";
}


void loadBWTable(string bwFileFullPath)
{
    if (bwFileFullPath!="") {
        // Load bwgraphs if file exists already..
        playerLog( ("Loading BWData from: " + (bwFileFullPath) ).c_str() );
        if ( boost::filesystem::exists( (bwFileFullPath).c_str() ))
        {
            std::ifstream ifs( (bwFileFullPath).c_str()  );
            boost::archive::text_iarchive ia(ifs);
            // read class state from archive
            ia >> bwManager;
            // archive and stream closed when destructors are called
            playerLog("Loaded fine!");
        }
    }
}

// This is a crash function. Don't use this.
BWManager getBWManagerOfFile(string bwFileFullPath) {
    BWManager tmpBWMan;
    // Remember to unlink immediately not to confuse it into GUIsystem!
    elementList.unLinkElementById(tmpBWMan.getId());
    if (bwFileFullPath!="") {
        if ( boost::filesystem::exists( (bwFileFullPath).c_str() ))
        {
            std::ifstream ifs( (bwFileFullPath).c_str()  );
            boost::archive::text_iarchive ia(ifs);
            ia >> tmpBWMan;
        }
    }
    return tmpBWMan;
}

void saveBWTable(string bwFileFullPath)
{
    if (bwFileFullPath!="") {
        // Save the graph table first..
        size_t found = bwFileFullPath.find(".");
        if (found==string::npos) // Add extension if not added.
            bwFileFullPath += ".bwr";
        playerLog(("Saving BWData into: " + (bwFileFullPath)).c_str());
        // STOPPAA RECORDAUS ja aloita savetuksen jälkeen..
        // Save also the brainwave graphs
        std::ofstream ofs( (bwFileFullPath).c_str() );
        // save data to archive
        {
            boost::archive::text_oarchive oa(ofs);
            // write class instance to archive
            oa << bwManager;
            // archive and stream closed when destructors are called
        }
        playerLog("Passed!");
    }
}

void initializeBrainWaveTable()
{
    bwManager.newBWTable();
}

// Tänne load session, save session sekä näppäimet jne.
//! !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



void autoSaveSession() {
    if (isSessionOn()) {
        // Save session.
        // Do things ie save calibration, sessiom time in seconds to continue rec etc.
        session.doThingsBeforeAutoSave();
        std::ofstream ofs( "session.dat" );
        {
            boost::archive::text_oarchive oa(ofs);
            oa << session;
        }
        // Save configurations.
        config.save();
        // Save bwdata.
        saveBWTable(BWFolder+"session.bwr");
        playerLog("#Autosaved.");
    }
}

void autoLoadSession() {
    // Check If the last session exists.
    if ( boost::filesystem::exists( "session.dat" ))
    {
        // Config is loaded automatically already..
        // Bw.. load this first..
        loadBWTable(BWFolder+"session.bwr");
        // Then session data..
        std::ifstream ifs( "session.dat"  );
        boost::archive::text_iarchive ia(ifs);
        ia >> session;
        // Do things ie load calibration and set time point to continue recording et session.
        session.doThingsAfterAutoLoad();
        playerLog("#Autoloaded.");
    }
}

void updateAutoSave() {
    if (al_get_time()-lastAutoSaveTime>30.0d) {
        lastAutoSaveTime = al_get_time();
        autoSaveSession();
    }
}

//! Experiment saving etc related.
void printAvarageData() {
    string resultStr="";
    for (int i=0;i<AMOUNT_OF_CHANNELS;i++) {
        playerLog( getGraphDescriptionText(i) + " avg: " + desToStr(bwManager.channel[i].getAvarage()) );
    }
}

void moveFile(path p1, path p2) {
    if (exists(p1)) {
        copy_file (p1, p2);
        remove (p1);
    }
}
void copyFile(path p1, path p2) {
    if (exists(p1)) {
        copy_file (p1, p2);
    }
}

void saveExperiment()
{
    // Then let's move all the stuff from this experiment for this subject to his own folder for later usage :)


    string resultsFolder = BWFolder+"results/";
    playerLog("Moving experiment data..");
    // Find first empty folder 0-1000 from the results-folder to add all stuff there..
    path pResultFolderForThisExp;
    for (int i=1; i<1001; i++)
    {
        pResultFolderForThisExp = path(resultsFolder+intToStr(i)+"/");
        if (!exists (pResultFolderForThisExp)) // This indexed folder doesn't exists. So we'll have this one!
            break;
    }
    // Create new directory for this guy and copy information..
    create_directory(pResultFolderForThisExp);

    moveFile (path(BWFolder+"session.bwr"), path(pResultFolderForThisExp.string()+"session.bwr"));

    moveFile (path(BWFolder+"baseline.bwr"), path(pResultFolderForThisExp.string()+"baseline.bwr"));

    moveFile (path(BWFolder+"baselineRender.txt"), path(pResultFolderForThisExp.string()+"baselineRender.txt"));

    moveFile (path(BWFolder+"sessionRender.txt"), path(pResultFolderForThisExp.string()+"sessionRender.txt"));

    moveFile (path("session.dat"), path(pResultFolderForThisExp.string()+"session.dat"));

    copyFile (path(configurationFileName.c_str()), path(pResultFolderForThisExp.string()+configurationFileName.c_str()));

    // Okay make also safecopy from the whole results-folder!!
    if (exists(path(resultsFolder)))
    {
        copyDir (path(resultsFolder), path(BWFolder+"resultSafeCopies/"+getTimeStampNow()));
    }
}


void renderResultsToFile(string path)
{
    playerLog("Printing results to file " + path);
    string resultStr = "\n\nData From Current Experiment ( dateTimeStamp:" + session.startTimeStamp + ") :\n";
    resultStr += "Experiment mode("+nfbTraineeModeRadioButton.getSelectionTxt()+") Session length("+desToStr(sessionLength.getConvertedValue())+"min) \n\n";

    // Analyze and bring the data into printing.
    resultStr += bwManager.getStatisticsStr();

    resultStr += "\n\n Automatic calibrations occured: " + intToStr(session.automaticCalibrationCount);

    resultStr += "\n\n Last level reached: " + intToStr(getLastReachedLevel()) + " / " + intToStr(AMOUNT_OF_LEVELS);

    std::ofstream ofs( path.c_str() );
    {
        boost::archive::text_oarchive oa(ofs);
        oa << resultStr;
    }

    playerLog("Done..");
}

}; // End of namespace
