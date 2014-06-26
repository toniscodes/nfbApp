#include "../include/MusicLibraryExpCollection.h"

MusicLibraryExpCollection::MusicLibraryExpCollection()
{
    //ctor
    mLibrariesExp.clear();
}

MusicLibraryExpCollection::~MusicLibraryExpCollection()
{
    //dtor
}

void MusicLibraryExpCollection::addMusicLibraryToCollection(MusicLibrary libToAdd)
{
    mLibrariesExp.push_back(libToAdd);
}
