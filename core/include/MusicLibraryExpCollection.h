#ifndef MUSICLIBRARYEXPCOLLECTION_H
#define MUSICLIBRARYEXPCOLLECTION_H

#include <vector>
#include <string>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include "MusicLibrary.h"

using namespace std;

/**
 * \brief MusicLibraryExpCollection.h ...::: Experimental purposes when analysing results :::...
*
* This gathers a collection of all user's result files for one compact to analyze collection.
*
* author: tonis @ 2013
*
*/
class MusicLibraryExpCollection
{
public:
    MusicLibraryExpCollection();
    virtual ~MusicLibraryExpCollection();
    vector<MusicLibrary>mLibrariesExp;
    void addMusicLibraryToCollection(MusicLibrary libToAdd);
protected:
private:

    // Boost serialization. This way it's easier to save and load everything without parsing problems.
    // I doubt this won't work as the song stuff is in vector.. hmm..
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & mLibrariesExp;
    }

};

#endif // MUSICLIBRARYEXPCOLLECTION_H

