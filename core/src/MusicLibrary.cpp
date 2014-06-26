#include <sstream>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include <allegro5/allegro_native_dialog.h>
#include "../include/MusicLibrary.h"
#include "../include/PlayerCommon.h"

using namespace std;
using namespace boost::filesystem;
using namespace plrCommon;

MusicLibrary::MusicLibrary()
{
    //constructor
}

MusicLibrary::MusicLibrary(string initialFilePath)
{
    //constructor
    initialPath = path(initialFilePath);
}
MusicLibrary::~MusicLibrary()
{
    //dtor
}

void MusicLibrary::reset()
{
    // Initialize everyhthing again. This is for clearing the list again totally..
    song.clear();
}

void MusicLibrary::removeSong(string id)
{
    for(size_t i = 0; i < song.size(); ++i)
    {
        if (song[i].getId() == id) // Remove songs with corresponding id's
            // erase the 6th element
            song.erase (song.begin()+i);
        //// erase the first 3 elements: to erase linear area
        //myvector.erase (myvector.begin(),myvector.begin()+3);
    }
}

Song MusicLibrary::getSong(string id)
{
    for(size_t i = 0; i < song.size(); ++i)
    {
        if (song[i].getId() == id) // Return song if id matched!
            return song[i];
    }
    // Otherwise null
    playerLog("\n ERROR: Didn't find song in MusicLibrary(getSong)! \n");
    return Song(NULL);
}

Song *MusicLibrary::getSongAddr(string id)
{
    for(size_t i = 0; i < song.size(); ++i)
    {
        if (song[i].getId() == id) // Return song if id matched!
            return &song[i];
    }
    // Otherwise null.. This should actually crash or something..
    playerLog("Couldn't find song!!!..AaaaRGH!");
    //return &Song(NULL); Propably this should be made a static there somewhere and used from there..
}

// This is propably used for Creating the CES-information of song :) And done when saving the graphs :)
// This could be called more often..
void MusicLibrary::createSongBWInfoToLibrary(string sId)
{
    // ..
    Song *updateSong = getSongAddr(sId);
    updateSong->updateSongBrainWaveInformation();
}

// This acts similarly as query.. in first and push out from back
void MusicLibrary::addSongToDJModeLastPlayedList(string addSongId)
{
    // Get the good size of last played list .. usually percentage ie 25% of the whole list size. This helps not to play the same songs over again.
    int sizeOfLastPlayedList = lastPlayedDJModeItemsListSize*song.size();
    if (sizeOfLastPlayedList==0)
        sizeOfLastPlayedList=1;

    lastDJPlayedItems.insert ( lastDJPlayedItems.begin() , addSongId );
    if (lastDJPlayedItems.size()>sizeOfLastPlayedList)
        lastDJPlayedItems.pop_back(); // Keep the size in limits..
}

// Carefull.. Check the dj list.. not anything else
bool MusicLibrary::isInDJModeLastPlayedList(string searchSongId)
{
    for(size_t i = 0; i < lastDJPlayedItems.size(); i++)
    {
        if (lastDJPlayedItems.at(i) == searchSongId) // Return song if id matched!
            return true;
    }
    return false;
}


float MusicLibrary::getRelativeValueInAllSongsOfChannel(int chanIndx, float value)
{
    float minV = getMinimumValueInAllSongsOfChannel(chanIndx);
    float maxV = getMaximumValueInAllSongsOfChannel(chanIndx);
    float divider = (maxV - minV);
    if (divider == 0) divider = 1.0f; // Prevent division by zero.
    float relativeV = (value - minV)/divider;
    if (relativeV<0.0f) // It sometimes goes over board If it's taken from EEG, because it can have different values.
        relativeV=0.0f;
    if (relativeV>1.0f)
        relativeV=1.0f;
    return relativeV;
}

float MusicLibrary::getMinimumValueInAllSongsOfChannel(int chanIndx)
{

    float minV = 9999999999;
    for(size_t i = 0; i < song.size(); ++i)
    {
        // Count only those which have bw info..!
        if (song[i].isBwInfoUsable())
        {
            if (song[i].getChannelAvg(chanIndx) < minV)
            {
                minV = song[i].getChannelAvg(chanIndx);
            }
        }
    }

    return minV;
}

float MusicLibrary::getMaximumValueInAllSongsOfChannel(int chanIndx)
{

    float maxV = 0;
    for(size_t i = 0; i < song.size(); ++i)
    {
        // Count only those which have bw info..!
        if (song[i].isBwInfoUsable())
        {
            if (song[i].getChannelAvg(chanIndx) > maxV)
            {
                maxV = song[i].getChannelAvg(chanIndx);
            }
        }
    }

    return maxV;
}


int MusicLibrary::amountOfTrialsInDJMode()
{
    // This is totally trial thing :)..
    return int(song.size()*2.3f); //0.75
}

// Search the 2dimensional closest point or some of the closest point and return it.
int MusicLibrary::getRndPeakDistanceMapPos(string songIdp, int mode)
{
    // Loop through simplified information (smoothened) to find the closest position first and return it.
    float lastFoundClosestDist = 999999999999.9f;
    float thisDistance         = 999999999999.9f;
    int randomPeaksListSize    = 3;
    vector<int> foundPosListOfCandidates;
    foundPosListOfCandidates.clear();
    int   bestFoundIndex       = 0;
    Song thisSong = getSong(songIdp);
    for (int i=0; i<thisSong.getLengthInSecs(); i++)
    {

        if (mode == USER_CHOSEN_STATE_LIST)
            thisDistance = thisSong.getMapDistanceFromTimeLinePointAutoAxis(XYDJPicker.getValX(),XYDJPicker.getValY(), i);
        if (mode == BCI_STATE_LIST)
            thisDistance = thisSong.getMapDistanceFromTimeLinePointAutoAxis(bwMeter.getCurrentProfileMindState(bwMeter.PROFILE_CHANNEL_1),bwMeter.getCurrentProfileMindState(bwMeter.PROFILE_CHANNEL_2), i);

        if (thisDistance < lastFoundClosestDist)
        {
            bestFoundIndex = i;
            lastFoundClosestDist = thisDistance;

            // Gather the three(or x) best options.
            foundPosListOfCandidates.insert ( foundPosListOfCandidates.begin() , i );
            if (foundPosListOfCandidates.size()>randomPeaksListSize) // Use it like a queue. The first is the last one that came into the queue.
                foundPosListOfCandidates.pop_back(); // Keep the size in limits..

        }

    }

    // Pick random peak.
    if (foundPosListOfCandidates.size()>0)
    {
        srand (time(NULL));
        return foundPosListOfCandidates[rand() % foundPosListOfCandidates.size()];
    }
    // Or the standard.
    return bestFoundIndex; // Return time in seconds.
}

string MusicLibrary::getRndSongAccordingBrainWaveInfo(int mode)   // Perhaps another parameter for accurate flow or it would be multiplied x 100 to make it more accurate and then divided? :)
{

    srand (time(NULL));
    int   rndSongIndex=0;
    float lastFoundClosestDist = 999999999999.9f;
    int   bestFoundIndex = 0;
    float thisDistance = 999999999999999;
    if (amountOfTrialsInDJMode() == 0)
        return "false";
    if (amountOfTrialsInDJMode() == 1)
        return song[0].getId();

    // Try to find the song in 75% of playlist size trials..:)
    for (int i=0; i<amountOfTrialsInDJMode(); i++)
    {

        rndSongIndex = rand() % song.size(); // WHuuu raandoom!

        //if (song[rndSongIndex].isBwInfoCreated())
        if (song[rndSongIndex].isBwInfoUsable())
        {

            if (mode == USER_CHOSEN_STATE_LIST)
                thisDistance = song[rndSongIndex].getProfileMapDistanceToAutoAxis(XYDJPicker.getValX(),XYDJPicker.getValY());
            if (mode == BCI_STATE_LIST)
                thisDistance = song[rndSongIndex].getProfileMapDistanceToAutoAxis(bwMeter.getCurrentProfileMindState(bwMeter.PROFILE_CHANNEL_1),bwMeter.getCurrentProfileMindState(bwMeter.PROFILE_CHANNEL_2));

            if ( thisDistance < lastFoundClosestDist )
            {

                // Check also that this song is not already played ie. just last round or few rounds ago it would be strange to play it over and over again.
                if (!isInDJModeLastPlayedList(song[rndSongIndex].getId()))
                {

                    // Merge candidate for later usage.. Update the closest distance also.
                    if (mode == USER_CHOSEN_STATE_LIST || mode == BCI_STATE_LIST)
                    {

                        playerLog("Found candidate song: " + song[rndSongIndex].getTitle() + " with distance " + floatToStr(thisDistance));
                        lastFoundClosestDist = thisDistance;
                        bestFoundIndex = rndSongIndex;

                    }

                }
                else
                    playerLog("The song " + song[rndSongIndex].getTitle() + " was in last " + intToStr(lastPlayedDJModeItemsListSize) + " played songs already..");

            }
            else
                playerLog("This wasn't good, not good distance. " + song[rndSongIndex].getTitle());

        }
        else
            playerLog("No good.. bw wasn't created for this guy " + song[rndSongIndex].getTitle());
    }

    //if (mode == CHOSEN_STATE_LIST || mode == BCI_STATE_LIST) {
    addSongToDJModeLastPlayedList(song[bestFoundIndex].getId());
    playerLog("The best song found was: " + song[bestFoundIndex].getTitle() + " with distance " + floatToStr(lastFoundClosestDist));
    return song[bestFoundIndex].getId();
    //}
}


// Parametrina TG_GRAPH_ATT tai med tai flow tai luku?
// Returns the real index str of the song which is accurate.
/*string MusicLibrary::getRndSongAccordingBrainWaveInfo(int mode) { // Perhaps another parameter for accurate flow or it would be multiplied x 100 to make it more accurate and then divided? :)
    srand (time(NULL));
    int rndSongIndex=0;
    // What kind of song having what kind of mental state are we looking for? If flow mode we should look for it..
    float lookForValue = 0.0f;
    float lastFoundClosestDist = 999999999999.9f;
    int bestFoundIndex = 0;
    if ( (mode == MOST_FLOW_LIST || mode == MOST_USER_STATE_LIST) && flowModeRadioButton.getSelectionTxt() == "User's State") {
        // The opposing state should try to balance things up which is what we'r looking for in here flow-mode.. Also some scaling is perhaps good idea

        if (mode == MOST_FLOW_LIST)
            lookForValue = -bwMeter.getCurrentUserFlowValue(); // This is opposite..
        if (mode == MOST_USER_STATE_LIST)
            lookForValue = bwMeter.getCurrentUserFlowValue(); // This is following person's state:)   factor x 0.5?=

        // Let's add some debug comment which tells the situation
        if (bwMeter.getCurrentUserFlowValue()>bwMeter.getFlowBoarder()) playerLog("In attentive state.. ");
        else if (bwMeter.getCurrentUserFlowValue()<-bwMeter.getFlowBoarder()) playerLog("In meditative state.. ");
        else playerLog("Currently in balanced state..:)");

        // Let's add some debug comment which tells the situation
        if (lookForValue>bwMeter.getFlowBoarder()) playerLog("Looking for attentive song.. desired flowvalue " +  floatToStr(lookForValue));
        else if (lookForValue<-bwMeter.getFlowBoarder()) playerLog("Looking for meditative song.. desired flowvalue " +  floatToStr(lookForValue));
        else playerLog("Looking for balanced song, desired flowvalue " + floatToStr(lookForValue));
    }

    for (int i=0;i<15;i++) {
        rndSongIndex = rand() % song.size();
        if (song[rndSongIndex].isBwInfoCreated()) {
            //if (song[rndSongIndex].getSongBwInfo().channelInfo[TG_GRAPH_ATT].getAvarageValue() > song[rndSongIndex].getSongBwInfo().channelInfo[TG_GRAPH_MED].getAvarageValue()*0.75f) {
            if ( (song[rndSongIndex].getAvgFlowAxisValue() > 0.0 && mode == MOST_ATTENTIVE_LIST) ||
                  (song[rndSongIndex].getAvgFlowAxisValue() < -0.0 && mode == MOST_MEDITATIVE_LIST) ||
                  (getDistanceBetweenValues(song[rndSongIndex].getAvgFlowAxisValue(),lookForValue) < lastFoundClosestDist && (mode == MOST_FLOW_LIST || mode == MOST_USER_STATE_LIST) && flowModeRadioButton.getSelectionTxt() == "User's State") ||
                    (flowModeRadioButton.getSelectionTxt() != "User's State" && mode == MOST_FLOW_LIST && song[rndSongIndex].getAvgFlowAxisValue() > -bwMeter.getFlowBoarder() && song[rndSongIndex].getAvgFlowAxisValue() < bwMeter.getFlowBoarder()) ) {
                      // Check also that this song is not already played ie. just last round or few rounds ago it would be strange
                      if (!isInDJModeLastPlayedList(song[rndSongIndex].getId())) {
                            // Handle normal case
                            if (mode == MOST_ATTENTIVE_LIST || mode == MOST_MEDITATIVE_LIST) {
                                playerLog("Found corresponding song " + song[rndSongIndex].getTitle());
                                i = 10+1; // Out of da looooop man!
                                // Add it to the chosen song lists
                                addSongToDJModeLastPlayedLis(song[rndSongIndex].getId());
                                return song[rndSongIndex].getId();
                            }
                            // Handle Flow case.. In here it's important that we don't go out of the loop as we'r looking for better options inside this loop
                            if ((mode == MOST_FLOW_LIST || mode == MOST_USER_STATE_LIST) && flowModeRadioButton.getSelectionTxt() == "User's State") {
                                lastFoundClosestDist = getDistanceBetweenValues(song[rndSongIndex].getAvgFlowAxisValue(),lookForValue);
                                bestFoundIndex       = rndSongIndex;
                                playerLog("Found better song ! Looking for " + floatToStr(lookForValue) + " and found value " + floatToStr(song[rndSongIndex].getAvgFlowAxisValue()) );
                                playerLog("And btw the song title was " + song[rndSongIndex].getTitle());
                            } else if (mode == MOST_FLOW_LIST && flowModeRadioButton.getSelectionTxt() != "User's State") {
                                playerLog("Found Flow Song " + song[rndSongIndex].getTitle());
                                i = 10+1; // Out of da looooop man!
                                // Add it to the chosen song lists
                                addSongToDJModeLastPlayedLis(song[rndSongIndex].getId());
                                return song[rndSongIndex].getId();
                            }

                      }else
                        playerLog("The song " + song[rndSongIndex].getTitle() + " was in last " + intToStr(lastPlayedDJModeItemsListSize) + " played songs already..");
            } else
                playerLog("This wasn't good " + song[rndSongIndex].getTitle() + " and song flow was " + floatToStr(song[rndSongIndex].getAvgFlowAxisValue()));
        } else
            playerLog("No good.. bw wasn't created for this guy " + song[rndSongIndex].getTitle());
    }
    if ((MOST_ATTENTIVE_LIST || mode == MOST_MEDITATIVE_LIST) || (mode == MOST_FLOW_LIST && flowModeRadioButton.getSelectionTxt() != "User's State")) {
        addSongToDJModeLastPlayedLis(song[rndSongIndex].getId());
        playerLog("Oh now.. Couldn't find nice song.. So we will randomise and use the last found song: " + song[rndSongIndex].getTitle());
        return song[rndSongIndex].getId();
    }
   if (mode == MOST_FLOW_LIST || mode == MOST_USER_STATE_LIST) {
       addSongToDJModeLastPlayedLis(song[bestFoundIndex].getId());
       playerLog("The best song found was: " + song[bestFoundIndex].getTitle() + " with flow value " + floatToStr(lastFoundClosestDist) + " and we were looking for " + floatToStr(lookForValue));
       return song[bestFoundIndex].getId();
   }
}*/

bool MusicLibrary::isEmpty()
{
    if (song.size()>0)
        return false;
    else
        return true;
}

// Refreshh!!
void MusicLibrary::refresh()
{

    // Update in 1 second interval.. I gues it's enough for now..
    if (al_get_time()-lastUpdatedGraphCalculatedInformationTime>1.0d)
    {
        lastUpdatedGraphCalculatedInformationTime = al_get_time();
        createSongBWInfoToLibrary(playThis.getId());
    }

}

// flag ALLEGRO_FILECHOOSER_FOLDER or ALLEGRO_FILECHOOSER_FOLDER for example.. To choose folder or just file choosing dialog.
void MusicLibrary::openSongFilesImportDialog(int flag)
{
    ALLEGRO_FILECHOOSER *fchooser;
    if (flag == BROWSE_MULTIPLE_FILES)
        fchooser = al_create_native_file_dialog(initialPath.string().c_str(),"Choose song files.","*.mp3;*.ogg;*.wav;*.flac;*.aiff;*.mod;*.s3m;*.xm;*.it;*.*",ALLEGRO_FILECHOOSER_MULTIPLE);
    else if (flag == BROWSE_FOLDER)
        fchooser = al_create_native_file_dialog(initialPath.string().c_str(),"Choose song folder.","*.mp3;*.ogg;*.wav;*.flac;*.aiff;*.mod;*.s3m;*.xm;*.it;*.*",ALLEGRO_FILECHOOSER_FOLDER);
    else
        fchooser = al_create_native_file_dialog(initialPath.string().c_str(),"Choose song files.","*.mp3;*.ogg;*.wav;*.flac;*.aiff;*.mod;*.s3m;*.xm;*.it;*.*",ALLEGRO_FILECHOOSER_MULTIPLE);

    al_show_native_file_dialog(display,fchooser);

    int amountOfChosenPaths = al_get_native_file_dialog_count(fchooser);

    //const ALLEGRO_PATH = al_get_native_file_dialog_path(fchooser, amountOfChosenPaths);
    playerLog("Amount of chosen files: " + intToStr(amountOfChosenPaths) + "\n");

    if (amountOfChosenPaths>0)
    {
        for (int i=0; i<amountOfChosenPaths; i++)
        {
            const char *chosenPath = al_get_native_file_dialog_path(fchooser, i);
            if (chosenPath != NULL)
            {
                //fprintf(stderr, ("Chosen paths: " + string(chosenPaths) + "\n").c_str());
                playerLog(("Looking for chosen path " + intToStr(i) + ": " + string(chosenPath) + "\n").c_str());
                importFiles(string(chosenPath).c_str());
            }
            else
                playerLog("Null was given from the filechoser..");
        }
    }
    else
    {
        playerLog("No files chosen..");
    }
    al_destroy_native_file_dialog(fchooser);
}

void MusicLibrary::importFiles(const char *importPath)
{

    path p (importPath);

    /*playerLog("#FILE");
    playerLog("Path: " + p.string() );
    path getcwd =  boost::filesystem::current_path();
    playerLog("Relative path: " + (naive_uncomplete(p, getcwd)).string() );*/
    try
    {
        if (exists(p))    // does p actually exist?
        {
            if (is_regular_file(p))         // is p a regular file?
            {
                //playerLog(p.string() + " size is " + intToStr(file_size(p)) + '\n');
                //playerLog("Regular file and extension is: " +extension(p));
                if (isValidMusicPlayerFile(p))
                {
                    //playerLog("Is valid music player file.");
                    Song newSong(p);
                    if (newSong.isSeekable() && !newSong.hasFailedToLoad())  // Only seekables seems to work in our program nowadays..
                        song.push_back(newSong);
                    //playerLog("Added Musical file to Library...\n");
                }
                else
                {
                    //playerLog("Not correct musical file!\n");
                }
            }

            else if (is_directory(p))      // is p a directory?
            {
                //playerLog(p.string() + " is a directory containing:\n");

                typedef vector<path> vec;             // store paths,
                vec v;                                // so we can sort them later

                copy(directory_iterator(p), directory_iterator(), back_inserter(v));

                sort(v.begin(), v.end());             // sort, since directory iteration
                // is not ordered on some file systems

                for (vec::const_iterator it (v.begin()); it != v.end(); ++it)
                {
                    importFiles((*it).string().c_str()); // Recursion
                    //playerLog("   " + (*it).string() + '\n');
                }
            }

            else
            {
                //playerLog(p.string() + " exists, but is neither a regular file nor a directory\n");
            }
        }
        else
        {
            //playerLog(p.string() + " does not exist\n");
        }
    }

    catch (const filesystem_error& ex)
    {
        //playerLog(ex.what() + '\n');
    }
}
