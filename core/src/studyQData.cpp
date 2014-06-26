#include "../include/studyQData.h"
#include "../include/playerCommon.h"
#include "../include/ResultPair.h"

using namespace std;
using namespace plrCommon;

#define ACTIVE_ANSWER 0
#define LIKE_ANSWER   1

#define AMOUNT_OF_SONGS_IN_STUDY 10
#define AMOUNT_OF_TEST_SUBJECTS  20

// This takes some of the songs away from results as it was planned..
#define AMOUNT_OF_SONGS_TAKEN_FROM_BEGINNING  1
#define AMOUNT_OF_SONGS_TAKEN_FROM_ENDING     1

double studyQData::getMinimum(double arr[], int sizet)
{
    double theBestMin = std::numeric_limits<double>::infinity();
    for (int i=0; i<sizet; i++)
    {
        if (arr[i]<theBestMin)
            theBestMin = arr[i];
    }
    //playerLog("Found min " + floatToStr(theBestMin));
    return theBestMin;
}

double studyQData::getMaximum(double arr[], int sizet)
{
    double theBestMax = -std::numeric_limits<double>::infinity();
    for (int i=0; i<sizet; i++)
    {
        if (arr[i]>theBestMax)
            theBestMax = arr[i];
    }
    //playerLog("Found max " + floatToStr(theBestMax));
    return theBestMax;
}

double *studyQData::makeRelativeArray(double arr[], int sizet)
{
    double *arrPointer = new double[sizet];
    //arrPointer[a]=a;
    double minValue = getMinimum(arr,sizet);
    double maxValue = getMaximum(arr,sizet);
    double deltaValue = maxValue-minValue;
    if (deltaValue==0) deltaValue=1.0d;
    //string resultStr="Koehenkilön kanavan arvot: ";
    for (int i=0; i<sizet; i++)
    {
        arrPointer[i] = (arr[i]-minValue)/deltaValue;  // Ei saatana.. siinä se nyt vittu on.. Toivotaan että toimaa!
        //resultStr += floatToStr(arrPointer[i]) + ":";
        //resultStr += floatToStr(arr[i]) + ":";
    }
    //playerLog(resultStr);
    //playerLog("Joista maksimi olit " + intToStr(maxValue) + " ja minimi oli " + intToStr(minValue));
    return arrPointer;
}

// Etsii seuraavan oikeasti arvioidussa käytetyn indeksin ja palauttaa sen jos sitä löytyi.
int studyQData::findNextRealIndex(ResultPair pairs[], int sizet, int indexToCheck, bool debugN)
{
    if (indexToCheck>10)
        return 11;
    if (debugN) playerLog("Finding bigger index. Given " + intToStr(indexToCheck));
    for (int i=0; i<sizet; i++)
    {
        if (pairs[i].x > indexToCheck)
        {
            if (debugN) playerLog("Found bigger index.. " + intToStr(pairs[i].x));
            return pairs[i].x;
        }
    }
    return 11;
}

bool studyQData::doesThisIndexInPairArrayContainSmallerNumber(ResultPair pairs[], int sizet, int indexToCheck, double valueToCheck, bool debugN)
{
    indexToCheck = findNextRealIndex(pairs,sizet,indexToCheck, debugN); // etsii seuraavan oikean arvioidun indeksin taulusta. ei hotkaise tyhjää!
    if (indexToCheck>10)
        return false;
    for (int i=0; i<sizet; i++)
    {
        if (pairs[i].x == indexToCheck )
        {
            if (debugN) playerLog("Löysi indeksin " + intToStr(indexToCheck) + " jonka arvo on " + floatToStr(pairs[i].y));
            if (pairs[i].y < valueToCheck)
            {
                if (debugN) playerLog("Kiinni jäi!");
                return true;
            }
        }
    }
    return false;
}


studyQData::studyQData(MusicLibraryExpCollection mlibCollection)
{
    //ctor

    int CHANNEL_1_TO_COMPARE = TG_GRAPH_BETA;
    int CHANNEL_2_TO_COMPARE = TG_GRAPH_THETA;


    int answerArray[AMOUNT_OF_TEST_SUBJECTS][2][AMOUNT_OF_SONGS_IN_STUDY] =
    {
        {
            // #1
            { 1, 7, 1, 1, 3, 8, 6, 8, 9, 8 }, // Active opinion.
            { 8, 8, 5, 7, 6, 8, 3, 5, 5, 6 }, // Liking of the song.
        },

        {
            // #2
            { 0, 7, 2, 4, 0, 9, 10, 10, 9, 9 }, // Active opinion.
            { 9, 7, 8, 7, 9, 6,  6, 10, 3, 5 }, // Liking of the song.
        },

        {
            // #3
            { 2, 4, 0, 1, 4, 8, 8, 8, 9, 6 }, // Active opinion.
            { 9, 6, 8, 6, 9, 5, 6, 7, 8, 3 }, // Liking of the song.
        },

        {
            // #4
            { 2, 5, 0, 3, 1, 9, 6, 7, 8, 7 }, // Active opinion.
            { 8, 5, 8, 8, 6, 9, 5, 4, 7, 8 }, // Liking of the song.
        },

        {
            // #5
            { 8, 6, 7, 6, 8, 8, 9, 8, 4, 10 }, // Active opinion.
            { 7, 6, 5, 4, 6, 8, 7, 9, 2, 9  }, // Liking of the song.
        },

        {
            // #6
            { 0,  0, 0, 0, 0, 8, 10, 5, 10, 9 }, // Active opinion.
            { 8, 10, 5, 5, 6, 8, 10, 5,  7, 8 }, // Liking of the song.
        },

        {
            // #7
            { 2, 6, 0, 3, 2, 9, 4, 8, 9, 8 }, // Active opinion.
            { 7, 8, 9, 4, 6, 9, 8, 8, 9, 7 }, // Liking of the song.
        },

        {
            // #8
            { 0,  5,  0, 2,  5, 10, 10, 0, 10, 5 }, // Active opinion.
            { 10, 5, 10, 10, 6,  8, 10, 9, 10, 0 }, // Liking of the song.
        },

        {
            // #9
            { 3, 4, 5, 5, 5, 6, 8, 7, 5, 6 }, // Active opinion.
            { 5, 3, 4, 4, 3, 5, 6, 4, 4, 4 }, // Liking of the song.
        },

        {
            // #10
            { 2, 6, 0,  2, 4, 9, 7, 8, 8, 8 }, // Active opinion.
            { 7, 5, 9, 10, 6, 4, 7, 6, 4, 5 }, // Liking of the song.
        },

        {
            // #11
            { 2, 6, 2, 3, 2, 8, 6, 7, 9, 7 }, // Active opinion.
            { 8, 7, 7, 8, 7, 8, 6, 5, 8, 8 }, // Liking of the song.
        },

        {
            // #12
            { 2, 10, 3, 3, 2,  8, 9, 8, 10, 9 }, // Active opinion.
            { 9,  1, 8, 8, 9, 10, 5, 7,  6, 6 }, // Liking of the song.
        },

        {
            // #13
            { 0, 3, 0, 0, 2, 7, 9, 9, 10, 8 }, // Active opinion.
            { 5, 0, 4, 8, 2, 8, 2, 3,  8, 7 }, // Liking of the song.
        },

        {
            // #14
            { 1, 5, 1, 5, 2, 9, 9, 5, 9, 8 }, // Active opinion.
            { 7, 3, 9, 5, 2, 9, 5, 5, 6, 7 }, // Liking of the song.
        },

        {
            // #15
            { 0, 2, 0, 3, 1, 8, 8, 6, 8, 5 }, // Active opinion.
            { 3, 2, 7, 7, 1, 9, 6, 8, 9, 7 }, // Liking of the song.
        },

        {
            // #16
            { 0, 4, 1, 5, 4,  6, 9, 7, 9, 8 }, // Active opinion.
            { 7, 4, 5, 5, 5, 10, 8, 7, 6, 7 }, // Liking of the song.
        },

        {
            // #17
            { 1, 5, 2, 3, 1, 7, 5, 7, 6, 7 }, // Active opinion.
            { 7, 8, 6, 8, 5, 8, 2, 5, 2, 4 }, // Liking of the song.
        },

        {
            // #18
            { 2, 5, 2, 1, 4, 10, 10, 7, 8, 8 }, // Active opinion.
            { 8, 4, 7, 9, 5,  5,  6, 4, 2, 6 }, // Liking of the song.
        },

        {
            // #19
            { 0, 1, 0, 0, 1, 8, 5, 4, 7, 7 }, // Active opinion.
            { 7, 2, 7, 8, 2, 9, 2, 6, 6, 6 }, // Liking of the song.
        },

        {
            // #20
            { 0, 4, 0, 3, 4, 10, 10, 10, 10, 10 }, // Active opinion.
            { 8, 8, 3, 7, 8,  8, 10,  2, 10,  6 }, // Liking of the song.
        }

    };


    // NOTICE! There are less indexes in these because taken away result songs!
    double bciResultArray[AMOUNT_OF_TEST_SUBJECTS][2][AMOUNT_OF_SONGS_IN_STUDY]; // float, because these are more accurate values..
    double bciResultArrayTmp[AMOUNT_OF_TEST_SUBJECTS][2][AMOUNT_OF_SONGS_IN_STUDY]; // float, because these are more accurate values..

    for (int i=0; i<mlibCollection.mLibrariesExp.size(); i++)
    {

        MusicLibrary mLib = mlibCollection.mLibrariesExp.at(i);

        // VITTU!! UNOHDIN BREIKKIKAPPALEET VÄLISTÄ :DD ELI NIIDE YLI HYPPIII!

        // Stack all the stuff into arrays.. Yes..!
        string resultStr = "";
        int avoidBreaksIndex=0;
        for (int s=0; s<AMOUNT_OF_SONGS_IN_STUDY; s++)
        {
            bciResultArrayTmp[i][0][avoidBreaksIndex] = mLib.song.at(avoidBreaksIndex).getChannelAvg(CHANNEL_1_TO_COMPARE);
            bciResultArrayTmp[i][1][avoidBreaksIndex] = mLib.song.at(avoidBreaksIndex).getChannelAvg(CHANNEL_2_TO_COMPARE);
            resultStr += floatToStr(mLib.song.at(avoidBreaksIndex).getChannelAvg(CHANNEL_1_TO_COMPARE)) + ":";
            avoidBreaksIndex++;
            if (avoidBreaksIndex==5)
                avoidBreaksIndex++;
            // [subject index][answermode][song index]
        }
        if (i==9)
            playerLog("Kymmenen kappaleen betat henkilöltä: " + resultStr);

        // Now make them relative!
        double *array1 = makeRelativeArray(bciResultArrayTmp[i][0],AMOUNT_OF_SONGS_IN_STUDY);
        double *array2 = makeRelativeArray(bciResultArrayTmp[i][1],AMOUNT_OF_SONGS_IN_STUDY);
        for (int p=0; p<AMOUNT_OF_SONGS_IN_STUDY; p++)
        {
            bciResultArray[i][0][p] = array1[p];
            bciResultArray[i][1][p] = array2[p];
        }

    }

    // Tässä kokeillaan tän soittimen kannalle asioita..
    // Järjestetään tulokset henkilön oman arvion mukaan esim aktiivisuuden mukaan aluksi..
    ResultPair results[AMOUNT_OF_TEST_SUBJECTS][AMOUNT_OF_SONGS_IN_STUDY-4];
    for (int q=0; q<AMOUNT_OF_TEST_SUBJECTS; q++)
    {
        // Täytetään vektoritaulu
        MusicLibrary mLib = mlibCollection.mLibrariesExp.at(q);
        int newPoint= 0;
        for (int i=0; i<AMOUNT_OF_SONGS_IN_STUDY; i++)
        {
            // Jump over first and last songs of the both parts
            if (i==0)
                i=1;
            if (i==9)
            {
                playerLog("The shit out of here.");
                break;
            }
            if (i==4)
                i=6;

            // X kertoo henkilön itsearvion, Y kertokoon beta-arvon
            results[q][newPoint].x = answerArray[q][0][i];
            results[q][newPoint].y = mLib.song.at(i).getChannelAvg(CHANNEL_1_TO_COMPARE); //bciResultArray[q][0][i];

            playerLog("Tallensin henkilölle " + intToStr(q) + " tuloksen X:" + floatToStr(results[q][newPoint].x) + " Y:" + floatToStr(results[q][newPoint].y));
            newPoint++;
        }
    }

    int debugThisGuy = 1;

    // Sitten tulostetaan ja katotaan, että kaikki on okei
    // On ok.. sit järjestellään taulu x indeksin mukaan. helppoa ku tiedetää et kaikki on 0-10
    for (int q=0; q<AMOUNT_OF_TEST_SUBJECTS; q++)
    {

        vector<ResultPair> pairsVectorResult;
        vector<ResultPair> pairsVector;
        // This vector tells which songs are not yet added to anywhere
        for (int i=0; i<AMOUNT_OF_SONGS_IN_STUDY-4; i++)
        {
            pairsVector.push_back(results[q][i]);
            //playerLog("This is what found " + floatToStr(results[q][i].x));
        }

        //for (int z=0;z<pairsVector.size();z++)
        // playerLog("This is what found " + floatToStr(pairsVector.at(z).x));

        for (int i=0; i<11; i++)
        {
            // Loop through all choises and add thjem
            for (int z=0; z<pairsVector.size(); z++)
            {
                //if (pairsVector.size()>z) { // Do this or otherwise it will break when deleting the element.. it is decreaseb vy 1 on
                if (int(pairsVector.at(z).x)==i)
                {
                    // Copy
                    pairsVectorResult.push_back(pairsVector.at(z));

                    // Remove from not added vector
                    //vector<ResultPair>::iterator it = remove(pairsVector.begin(), pairsVector.end(), z);
                    //pairsVector.erase(it, pairsVector.end());
                    pairsVector.erase (pairsVector.begin()+z);
                    z--; // Z one bvack!
                }
                //}
            }
        }

        // Copy here the final thingy
        for (int i=0; i<pairsVectorResult.size(); i++)
        {
            results[q][i] = pairsVectorResult.at(i);
            //if (q==debugThisGuy)
            playerLog("Uusi järjestely henkilö " + intToStr(q) + " tulostaa X:" + floatToStr(results[q][i].x) + " Y:" + floatToStr(results[q][i].y));
        }

    }

    // Then the algorithm to determine how many songs go with the rule: "Does the next index have bigger beta among them"
    double allPropabilityToHaveItRight=0;
    string resultStr = "\n";
    for (int q=0; q<AMOUNT_OF_TEST_SUBJECTS; q++)
    {

        // Start going through indexes..
        int dosentFollowTheory=0;
        for (int i=0; i<AMOUNT_OF_SONGS_IN_STUDY-4; i++)
        {
            int indexToCheck = results[q][i].x; // This is the index.. eli tästä seuraavaksi isoin arvioitu indeksi etsitään tuolla funktiossa itsestään
            bool debugN=false;
            if (q==debugThisGuy)   // katotaan 18 tuloksia
            {
                playerLog("Tsekkaa indeksi " + intToStr(indexToCheck) + " ja arvo " + floatToStr(results[q][i].y));
                debugN = true;
            }
            if (doesThisIndexInPairArrayContainSmallerNumber(results[q], AMOUNT_OF_SONGS_IN_STUDY-4, indexToCheck, results[q][i].y, debugN))
                dosentFollowTheory++;
        }

        //playerLog("Subject " + intToStr(q) + " had " + intToStr(dosentFollowTheory) + " not following theory out of 6");

        // playerLog("Subject " + intToStr(q+1) + " had " + intToStr(6-dosentFollowTheory) + " out of 6 songs right.");
        double propabilityToHaveItRight = ( 6.0d-( (double)dosentFollowTheory ) ) / 6.0d;
        allPropabilityToHaveItRight += propabilityToHaveItRight;
        playerLog("Propab: " + floatToStr(propabilityToHaveItRight));
        resultStr += floatToStr(propabilityToHaveItRight) + "\n";
    }


    allPropabilityToHaveItRight = allPropabilityToHaveItRight/AMOUNT_OF_TEST_SUBJECTS;
    playerLog("\n After all the total probability is " + floatToStr(allPropabilityToHaveItRight));

    playerLog("\n So this means in avarage " + floatToStr((int)(allPropabilityToHaveItRight*100)) + "%% of the songs are in correct position for the user accroding his own appreciation." );

    resultStr += floatToStr(allPropabilityToHaveItRight) + "\n";
    std::ofstream ofs( "resultBetaListCorrectness.txt" );
    //std::ofstream ofs( "resultThetaListCorrectness.txt" );
    {
        boost::archive::text_oarchive oa(ofs);
        //boost::archive::basic_text_oarchive oa(ofs);
        oa << resultStr;
    }

}

studyQData::~studyQData()
{
    //dtor
}
