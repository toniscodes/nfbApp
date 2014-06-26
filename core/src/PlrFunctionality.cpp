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

// Tänne load session, save sesseion sekä näppäimet jne.
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


// Printing results of the current bw data:
// Channel index is asked and the partition amount. Ie avarage of 3 parts or just 1 part. Total avarage in the end.
string getChannelResultRow(int chanIndx, int parts)
{
    string resultStr="";
    resultStr += getGraphTheme(chanIndx).text;
    string theValueStr;
    double avgSum=0;
    double totalAvgSum=0;
    double value=0;
    double avg=0;
    double totalRecordLength = bwManager.channel[chanIndx].getRecordedLength(); // The last recorded point
    //playerLog("length of recording " + desToStr(totalRecordLength));
    for (int q=0;q<parts;q++) {
        int from = (int)((totalRecordLength/(double)parts)*(double)q);
        int to   = (int)((totalRecordLength/(double)parts)*(double)(q+1));
        avgSum   = 0;
        for (int i=from; i<to; i++)
        {
            int value = bwManager.channel[chanIndx].valueOf(i);
            if (value==CHANNEL_EMPTY_INDEX) // Empty recording area is rare? Why someone record in parts? But If that occurs it will be counted as zero now.
                value = 0;
            avgSum += value;
            //playerLog("val " + intToStr(i) + ":" + desToStr(value));
        }
        double partAvg = (avgSum/((double)(to-from)));
        totalAvgSum += partAvg;
        //playerLog("partAvg " + desToStr(partAvg));
        theValueStr = desToStr(showDecimals(partAvg,3)); // Let's change the . to , for better handling in calculations!
        replace( theValueStr.begin(), theValueStr.end(), '.', ',' ); // This is for calc app to understand it straightly.
        resultStr += "|"+theValueStr;
        //resultStr += "|"+intToStr((int)(avgSum));
    }

    avg = totalAvgSum/parts;
    theValueStr = desToStr(showDecimals(avg,3)); // Let's change the . to , for better handling in calculations!
    replace( theValueStr.begin(), theValueStr.end(), '.', ',' );
    resultStr += "|"+theValueStr;
    //resultStr += "|"+intToStr((int)(avg));
    return resultStr;
}



// Returns table of the avarage calculation results. Give always one index bigger table as input than parts is. Because last one is total avarage.
void getChannelResultTable(double *table, int chanIndx, int parts)
{
    double avgSum=0;
    double totalAvgSum=0;
    double value=0;
    double avg=0;
    double totalRecordLength = bwManager.channel[chanIndx].getRecordedLength(); // The last recorded point
    for (int q=0;q<parts;q++) {
        int from = (int)((totalRecordLength/(double)parts)*(double)q);
        int to   = (int)((totalRecordLength/(double)parts)*(double)(q+1));
        avgSum   = 0;
        for (int i=from; i<to; i++)
        {
            int value = bwManager.channel[chanIndx].valueOf(i);
            if (value==CHANNEL_EMPTY_INDEX) // Empty recording area is rare? Why someone record in parts? But If that occurs it will be counted as zero now.
                value = 0;
            avgSum += value;
        }
        double partAvg = (avgSum/((double)(to-from)));
        totalAvgSum += partAvg;
        table[q] = partAvg;
    }

    table[parts] = totalAvgSum/parts; // The last one is reserved for the total avarage.
}

// This calculates the relative volumes (compared to avarage of all other channels) of current phase of asked channel.
void getChannelResultTableRelative(double *table, int chanIndx, int parts)
{
    double results[AMOUNT_OF_CHANNELS][parts+1]; // The last given is the avarage of all so this is why this is +1
    // Copy the amplitudes of different phases to table.
    for (int i=0;i<AMOUNT_OF_CHANNELS;i++)
        if (useThisTypeInRelativeCalculations(i))
            getChannelResultTable(results[i], i, parts);

    // Then make relative map.
    double resultsRel[AMOUNT_OF_CHANNELS][parts+1]; // The last given is the avarage of all so this is why this is +1
    for (int z=0;z<parts;z++)
        for (int i=0;i<AMOUNT_OF_CHANNELS;i++) {
            // Avarage of all em except this one
            double avgSum = 0;
            int avgCount  = 0;
            for (int q=0;q<AMOUNT_OF_CHANNELS;q++) {
                if (useThisTypeInRelativeCalculations(q) && i!=q) {
                        avgSum+=results[q][z];
                        avgCount++;
                }
            }
            resultsRel[i][z] = results[i][z] / (avgSum/avgCount);
        }

    // Then calclate the last draw.. That avarage of the whole .. The total avarage of all em.. Sweep.. Swipe.. Clean..
    for (int i=0;i<AMOUNT_OF_CHANNELS;i++) {
        double avgSum = 0;
        int avgCount  = 0;
        for (int z=0;z<parts;z++) {
            avgSum+=resultsRel[i][z];
            avgCount++;
        }
        resultsRel[i][parts] = (avgSum/avgCount);
    }

    // Then copy the asked channel to given table.
    for (int z=0;z<parts+1;z++) {
        table[z] = resultsRel[chanIndx][z];
    }

}

string getChannelResultRowRel(int chanIndx, int parts)
{
    string resultStr="";
    resultStr += getGraphTheme(chanIndx).text + "Rel";
    string theValueStr;
    double relTable[parts+1];
    getChannelResultTableRelative(relTable,chanIndx,parts);
    for (int z=0;z<parts+1;z++) {
        theValueStr = desToStr(showDecimals(relTable[z],3)); // Let's change the . to , for better handling in calculations!
        replace( theValueStr.begin(), theValueStr.end(), '.', ',' );
        resultStr += "|"+theValueStr;
    }
    return resultStr;
}

void renderResultsToFile(string path)
{
    playerLog("Printing results to file " + path);
    string resultStr = "\n\nData From Current Experiment ( dateTimeStamp:" + getTimeStampNow() + ") :\n";
    resultStr += "Experiment mode("+nfbTraineeModeRadioButton.getSelectionTxt()+") Session length("+desToStr(sessionLength.getConvertedValue())+"min) \n\n";

    int partitions = 3;

    for (int i=0;i<AMOUNT_OF_CHANNELS;i++) {
        resultStr += getChannelResultRow(i,partitions)+"\n";
        if (useThisTypeInRelativeCalculations(i))
            resultStr += getChannelResultRowRel(i,partitions)+"\n";
    }

    // These next things are a bit old, were used in experiment. Could be changed a bit.

    // Calculate also t/a ratios.
    double thetax[partitions+1];
    double alphax[partitions+1];
    getChannelResultTable(thetax, CHANNEL_THETA, partitions);
    getChannelResultTable(alphax, CHANNEL_ALPHA1, partitions);
    resultStr += "t/a-ratio";
    for (int i=0;i<partitions+1;i++) {
        double tA = thetax[i]/alphax[i];
        string theValueStr = desToStr(showDecimals(tA,3)); // Let's change the . to , for better handling in calculations!
        replace( theValueStr.begin(), theValueStr.end(), '.', ',' ); // This is for calc app to understand it straightly.
        resultStr += "|"+theValueStr;
    }

    // Also could be possible to.. compare alfa channel 2 ..? Delta is not used as it has so much noise.
    // Calculate also relative t+a -ratio to other channels(gamma,beta).
    double gammax[partitions+1];
    double betax[partitions+1];
    getChannelResultTable(gammax, CHANNEL_GAMMA, partitions);
    getChannelResultTable(betax, CHANNEL_BETA, partitions);
    resultStr += "\n*(t+a)-ratio";
    for (int i=0;i<partitions+1;i++) {
        double taRel = ((thetax[i]+alphax[i])/2.0d)/((gammax[i]+betax[i])/2.0d);
        string theValueStr = desToStr(showDecimals(taRel,3)); // Let's change the . to , for better handling in calculations!
        replace( theValueStr.begin(), theValueStr.end(), '.', ',' ); // This is for calc app to understand it straightly.
        resultStr += "|"+theValueStr;
    }

    // Calculate also relative gamma ratios (to alpa+theta+beta).
    getChannelResultTable(gammax, CHANNEL_GAMMA, partitions);
    getChannelResultTable(betax, CHANNEL_BETA, partitions);
    resultStr += "\n*g-ratio";
    for (int i=0;i<partitions+1;i++) {
        double gRel = gammax[i]/((thetax[i]+alphax[i]+betax[i])/3.0d);
        string theValueStr = desToStr(showDecimals(gRel,3)); // Let's change the . to , for better handling in calculations!
        replace( theValueStr.begin(), theValueStr.end(), '.', ',' ); // This is for calc app to understand it straightly.
        resultStr += "|"+theValueStr;
    }

    resultStr += "\n\n Automatic calibrations occured: " + intToStr(session.automaticCalibrationCount);

    resultStr += "\n\n Last level reached: " + intToStr(getCurrentLevel());

    std::ofstream ofs( path.c_str() );
    {
        boost::archive::text_oarchive oa(ofs);
        oa << resultStr;
    }

    playerLog("Done..");
}


}; // End of namespace
