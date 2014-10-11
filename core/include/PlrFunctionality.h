#ifndef PLRFUNCTIONALITY_H
#define PLRFUNCTIONALITY_H

#include "PlayerCommon.h"

/**
 * \brief PlrFunctionality.h ...::: Player Functionality - The player's functionality :::...
*
* Most of the BCI MUSIC PLAYER functionality is here. Like what happens after clicking something etc.
*
* Here is also the experiment data handling. A lots of it.
*
* author: tonis @ 2013
*
*/
class PlrFunctionality
{
public:
    PlrFunctionality();
    virtual ~PlrFunctionality();
protected:
private:
};

namespace plrCommon
{

    void loadBWTable(string bwFileFullPath);
    BWManager getBWManagerOfFile(string bwFileFullPath);
    void saveBWTable(string bwFileFullPath);
    void initializeBrainWaveTable();
    string openFileImportDialogAndReturnPath(string textInBrowser, int flag, string types);

    void autoLoadSession();
    void autoSaveSession();
    void updateAutoSave();

    void printAvarageData();
    void saveExperiment();
    void renderResultsToFile(string path);



};

#endif // PLRFUNCTIONALITY_H
