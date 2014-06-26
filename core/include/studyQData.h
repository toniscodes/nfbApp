#include "playerCommon.h"
#include "ResultPair.h"

#ifndef STUDYQDATA_H
#define STUDYQDATA_H

/**
 * \brief StudyQData.h ...::: StudyQdData :::...
*
* Handling of experiment results. This is where the plot data is outputted. This is where
* the all experiment data of all participants answers to the papers are.
*
* IMPORTANT CLASS FOR ANALYZING RESULTS! VERY!
*
* author: tonis @ 2013
*
*/
class studyQData
{
public:
    studyQData(MusicLibraryExpCollection mLib);
    double* makeRelativeArray(double arr[], int sizet);
    double getMinimum(double arr[], int sizet);
    double getMaximum(double arr[], int sizet);
    int findNextRealIndex(ResultPair pairs[], int sizet, int indexToCheck, bool debugN);
    bool doesThisIndexInPairArrayContainSmallerNumber(ResultPair pairs[], int sizet, int indexToCheck, double valueToCheck, bool debugN);

    virtual ~studyQData();
protected:
private:
};

#endif // STUDYQDATA_H
