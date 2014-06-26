#include "../include/Config.h"
#include "../include/PlayerCommon.h"

using namespace plrCommon;

Config::Config()
{
    //ctor
}

Config::~Config()
{
    //dtor
}

void Config::load() {
    load(configurationFileName);
}

void Config::load(string confFile) {
    playerLog("Loading configurations..");
    try {
        if ( boost::filesystem::exists( confFile.c_str() ) )
        {
            std::ifstream ifs(confFile.c_str());
            boost::archive::text_iarchive ia(ifs);
            ia >> config;
            loadVariablesAfterLoad();
        }
        else
        {
            playerLog("Configuration file not created yet. It's normal for first time.");
        }
    } catch (int e) {
        playerLog("Old version of config? No problem. Just save.");
    }
    loadVariablesAfterLoad();
}

void Config::loadVariablesAfterLoad() {

    /*for(size_t i = 0; i < configVars.size(); ++i)
    {
        playerLog("Log " + configVars[i].varVal);
    }*/
    vector<GUIElement*> elements = elementList.getElements();
    for(size_t i = 0; i < configVars.size(); ++i)
    {
        for(size_t e = 0; e < elements.size(); ++e)
        {
            if (elements[e]->isStorable() && configVars[i].varVal!="-1")
            {
                if (dynamic_cast<GUIRollButton*>(elements[e]) != NULL)
                {
                    GUIRollButton* b = dynamic_cast<GUIRollButton*>(elements[e]);
                    if (intToStr(b->getId()) == configVars[i].varName)
                    {
                        double temp = ::atof(configVars[i].varVal.c_str());
                        b->setValue(temp);
                    }
                }
                if (dynamic_cast<GUIRadioButton*>(elements[e]) != NULL)
                {
                    GUIRadioButton* b = dynamic_cast<GUIRadioButton*>(elements[e]);
                    if (intToStr(b->getId()) == configVars[i].varName)
                    {
                        int temp = ::atoi(configVars[i].varVal.c_str());
                        b->setCheckedIndex(temp);
                    }
                }
                if (dynamic_cast<GUICheckBox*>(elements[e]) != NULL)
                {
                    GUICheckBox* b = dynamic_cast<GUICheckBox*>(elements[e]);
                    if (intToStr(b->getId()) == configVars[i].varName)
                    {
                        bool temp = strToBool(configVars[i].varVal.c_str());
                        b->setChecked(temp);
                    }
                }
            }
        }
    }
}

void Config::bringVariablesForSaving() {
    // Initialize all elements from element list that can be stored.
    vector<GUIElement*> elements = elementList.getElements();
    configVars.clear();
    for(size_t i = 0; i < elements.size(); ++i)
    {
         if (elements[i]->isStorable()) {
            if (dynamic_cast<GUIRollButton*>(elements[i]) != NULL)
            {
                GUIRollButton* b = dynamic_cast<GUIRollButton*>(elements[i]);
                //if (b->getUnitType()==FLOAT_NUM_TYPE)
                configVars.push_back(ConfigVar(intToStr(b->getId()), (desToStr(b->getConvertedValue())+"").c_str() ));
            }
            if (dynamic_cast<GUIRadioButton*>(elements[i]) != NULL)
            {
                GUIRadioButton* b = dynamic_cast<GUIRadioButton*>(elements[i]);
                configVars.push_back(ConfigVar(intToStr(b->getId()), intToStr(b->getCheckedIndex()).c_str() ));
            }
            if (dynamic_cast<GUICheckBox*>(elements[i]) != NULL)
            {
                GUICheckBox* b = dynamic_cast<GUICheckBox*>(elements[i]);
                configVars.push_back(ConfigVar(intToStr(b->getId()), boolToStr(b->isChecked()).c_str() ));
            }
         }
    }
}

void Config::save() {
    save(configurationFileName);
}

void Config::save(string confFile) {
    bringVariablesForSaving();
    std::ofstream ofs(confFile.c_str());
    {
        boost::archive::text_oarchive oa(ofs);
        oa << config;
    }
}
