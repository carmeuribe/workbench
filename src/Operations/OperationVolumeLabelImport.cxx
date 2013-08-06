/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "OperationVolumeLabelImport.h"
#include "OperationException.h"

#include "CaretLogger.h"
#include "FileInformation.h"
#include "GiftiLabel.h"
#include "VolumeFile.h"

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <fstream>
#include <map>
#include <set>
#include <string>

using namespace caret;
using namespace std;

AString OperationVolumeLabelImport::getCommandSwitch()
{
    return "-volume-label-import";
}

AString OperationVolumeLabelImport::getShortDescription()
{
    return "IMPORT A LABEL VOLUME TO CARET FORMAT";
}

OperationParameters* OperationVolumeLabelImport::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addVolumeParameter(1, "input", "the label volume to import");
    
    ret->addStringParameter(2, "label-list-file", "text file containing the values and names for labels");
    
    ret->addVolumeOutputParameter(3, "output", "the output workbench label volume");
    
    ret->createOptionalParameter(4, "-discard-others", "set any voxels with values not mentioned in the label list to the ??? label");
    
    OptionalParameter* unlabeledOption = ret->createOptionalParameter(5, "-unlabeled-value", "set the value that will be interpreted as unlabeled");
    unlabeledOption->addIntegerParameter(1, "value", "the numeric value for unlabeled (default 0)");
    
    OptionalParameter* subvolumeSelect = ret->createOptionalParameter(6, "-subvolume", "select a single subvolume to import");
    subvolumeSelect->addStringParameter(1, "subvol", "the subvolume number or name");
    
    ret->createOptionalParameter(7, "-drop-unused-labels", "remove any unused label values from the label table");
    
    ret->setHelpText(
        AString("Creates a new volume with label information in the header in the caret nifti extension format.  ") +
        "You may specify the empty string ('' will work on linux/mac) for <label-list-file>, which will be treated as if it is an empty file.  " +
        "The label list file must have lines of the following format:\n\n" + 
        "<labelname>\n<value> <red> <green> <blue> <alpha>\n\n" + 
        "Do not specify the \"unlabeled\" key in the file, it is assumed that 0 means not labeled unless -unlabeled-value is specified.  " +
        "Label names must be on a separate line, but may contain spaces or other unusual characters (but not newline).  " +
        "Whitespace is trimmed from both ends of the label name, but is kept if it is in the middle of a label.  " +
        "The values of red, green, blue and alpha must be integers from 0 to 255, and will specify the color the label is drawn as " +
        "(alpha of 255 means opaque, which is probably what you want).  " +
        "By default, it will set new label names with names of LABEL_# for any values encountered that are not mentioned in the " +
        "list file, specify -discard-others to instead set these voxels to the \"unlabeled\" key.  " +
        "Negative keys will get remapped to unused positive keys (workbench doesn't allow negative label keys)."
    );
    return ret;
}

void OperationVolumeLabelImport::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    AString temp;
    srand(time(NULL));
    LevelProgress myProgress(myProgObj);
    VolumeFile* myVol = myParams->getVolume(1);
    AString listfileName = myParams->getString(2);
    VolumeFile* outVol = myParams->getOutputVolume(3);
    bool discardOthers = false;
    OptionalParameter* discardOption = myParams->getOptionalParameter(4);
    if (discardOption->m_present)
    {
        discardOthers = true;
    }
    int32_t unlabeledValue = 0;
    OptionalParameter* unlabeledOption = myParams->getOptionalParameter(5);
    if (unlabeledOption->m_present)
    {
        unlabeledValue = (int32_t)unlabeledOption->getInteger(1);
    }
    OptionalParameter* subvolumeSelect = myParams->getOptionalParameter(6);
    int subvol = -1;
    if (subvolumeSelect->m_present)
    {//set up to use the single column
        subvol = (int)myVol->getMapIndexFromNameOrNumber(subvolumeSelect->getString(1));
        if (subvol < 0 || subvol >= myVol->getNumberOfMaps())
        {
            throw OperationException("invalid column specified");
        }
    }
    bool dropUnused = myParams->getOptionalParameter(7)->m_present;
    GiftiLabelTable myTable;
    map<int32_t, int32_t> translate;
    if (listfileName != "")
    {
        FileInformation textFileInfo(listfileName);
        if (!textFileInfo.exists())
        {
            throw OperationException("label list file doesn't exist");
        }
        fstream labelListFile(listfileName.toLocal8Bit().constData(), fstream::in);
        if (!labelListFile.good())
        {
            throw OperationException("error reading label list file");
        }
        string labelName;
        int32_t value, red, green, blue, alpha;
        translate[unlabeledValue] = 0;//placeholder, we don't know the correct translated value yet
        while (labelListFile.good())
        {
            getline(labelListFile, labelName);
            labelListFile >> value;
            labelListFile >> red;
            labelListFile >> green;
            labelListFile >> blue;
            if (!(labelListFile >> alpha))//yes, that is seriously the correct way to check if input was successfully extracted...so much fail
            {
                break;//stop at malformed lines
            }
            while (isspace(labelListFile.peek()))
            {
                labelListFile.ignore();//drop the newline, possible carriage return or other whitespace so that getline doesn't get nothing, and cause int extraction to fail
            }
            temp = AString(labelName.c_str()).trimmed();//drop errant CR or other whitespace from beginning and end of lines
            if (translate.find(value) != translate.end())
            {
                if (value == unlabeledValue)
                {
                    throw OperationException("the unlabeled value must not be specified in label list file");
                } else {
                    throw OperationException(AString("label key ") + AString::number(value) + " specified more than once");
                }
            }
            GiftiLabel myLabel(value, temp, red, green, blue, alpha);
            if (myTable.getLabelKeyFromName(temp) != GiftiLabel::getInvalidLabelKey())
            {
                AString nameBase = temp, newName;//resolve collision by generating a name with an additional number on it
                bool success = false;
                for (int extra = 1; extra < 100; ++extra)//but stop at 100, because really...
                {
                    newName = nameBase + "_" + AString::number(extra);
                    if (myTable.getLabelKeyFromName(newName) == GiftiLabel::getInvalidLabelKey())
                    {
                        success = true;
                        break;
                    }
                }
                if (success)
                {
                    CaretLogWarning("name collision in input name '" + nameBase + "', changing one to '" + newName + "'");
                } else {
                    throw OperationException("giving up on resolving name collision for input name '" + nameBase + "'");
                }
                myLabel.setName(newName);
            }
            int32_t newValue;
            if (value == 0)//because label 0 exists in the default constructed table
            {
                myTable.insertLabel(&myLabel);//but we do want to be able to overwrite the default 0 label
                newValue = 0;//if value 0 is specified twice, or once without specifying a different unlabeled value, the check versus the translate map will catch it
            } else {
                newValue = myTable.addLabel(&myLabel);//we don't want to overwrite relocated labels
            }
            translate[value] = newValue;
        }
    }
    vector<int64_t> myDims;
    myVol->getDimensions(myDims);
    const int64_t FRAMESIZE = myDims[0] * myDims[1] * myDims[2];
    CaretArray<float> frameOut(FRAMESIZE);
    int32_t unusedLabel = myTable.getUnassignedLabelKey();
    translate[unlabeledValue] = unusedLabel;
    if (subvol == -1)
    {
        outVol->reinitialize(myVol->getOriginalDimensions(), myVol->getSform(), myDims[4], SubvolumeAttributes::LABEL);
        for (int s = 0; s < myDims[3]; ++s)
        {
            set<int32_t> usedValues;//track used values if we have dropUnused
            for (int c = 0; c < myDims[4]; ++c)//hopefully noone wants a multi-component label volume, that would be silly, but do it anyway
            {
                const float* frameIn = myVol->getFrame(s, c);//TODO: rework this when support is added for VolumeFile to handle non-float data
                for (int i = 0; i < FRAMESIZE; ++i)
                {
                    int32_t labelval = (int32_t)floor(frameIn[i] + 0.5f);//just in case it somehow got poorly encoded, round to nearest
                    if (dropUnused)
                    {
                        usedValues.insert(labelval);
                    }
                    map<int32_t, int32_t>::iterator myiter = translate.find(labelval);
                    if (myiter == translate.end())
                    {
                        if (discardOthers)
                        {
                            frameOut[i] = unusedLabel;
                        } else {//use a random color, but fully opaque for the label
                            GiftiLabel myLabel(labelval, AString("LABEL_") + AString::number(labelval), rand() & 255, rand() & 255, rand() & 255, 255);
                            if (myTable.getLabelKeyFromName(myLabel.getName()) != GiftiLabel::getInvalidLabelKey())
                            {
                                AString nameBase = myLabel.getName(), newName;//resolve collision by generating a name with an additional number on it
                                bool success = false;
                                for (int extra = 1; extra < 100; ++extra)//but stop at 100, because really...
                                {
                                    newName = nameBase + "_" + AString::number(extra);
                                    if (myTable.getLabelKeyFromName(newName) == GiftiLabel::getInvalidLabelKey())
                                    {
                                        success = true;
                                        break;
                                    }
                                }
                                if (success)
                                {
                                    CaretLogWarning("name collision in auto-generated name '" + nameBase + "', changed to '" + newName + "'");
                                } else {
                                    throw OperationException("giving up on resolving name collision for auto-generated name '" + nameBase + "'");
                                }
                                myLabel.setName(newName);
                            }
                            int32_t newValue = myTable.addLabel(&myLabel);//don't overwrite any values in the table
                            translate[labelval] = newValue;
                            frameOut[i] = newValue;
                        }
                    } else {
                        frameOut[i] = myiter->second;
                    }
                }
                outVol->setFrame(frameOut, s, c);
            }
            if (dropUnused)
            {
                GiftiLabelTable frameTable = myTable;
                frameTable.deleteUnusedLabels(usedValues);
                *(outVol->getMapLabelTable(s)) = frameTable;
            } else {
                *(outVol->getMapLabelTable(s)) = myTable;//set the label table AFTER doing the frame, because we may make new labels while scanning
            }
        }
    } else {
        vector<int64_t> newDims = myDims;
        newDims.resize(3);//spatial only
        outVol->reinitialize(newDims, myVol->getSform(), myDims[4], SubvolumeAttributes::LABEL);
        set<int32_t> usedValues;//track used values if we have dropUnused
        for (int c = 0; c < myDims[4]; ++c)//hopefully noone wants a multi-component label volume, that would be silly, but do it anyway
        {
            const float* frameIn = myVol->getFrame(subvol, c);//TODO: rework this when support is added for VolumeFile to handle non-float data
            for (int i = 0; i < FRAMESIZE; ++i)
            {
                int32_t labelval = (int32_t)floor(frameIn[i] + 0.5f);//just in case it somehow got poorly encoded, round to nearest
                if (dropUnused)
                {
                    usedValues.insert(labelval);
                }
                map<int32_t, int32_t>::iterator myiter = translate.find(labelval);
                if (myiter == translate.end())
                {
                    if (discardOthers)
                    {
                        frameOut[i] = unusedLabel;
                    } else {//use a random color, but fully opaque for the label
                        GiftiLabel myLabel(labelval, AString("LABEL_") + AString::number(labelval), rand() & 255, rand() & 255, rand() & 255, 255);
                        if (myTable.getLabelKeyFromName(myLabel.getName()) != GiftiLabel::getInvalidLabelKey())
                        {
                            AString nameBase = myLabel.getName(), newName;//resolve collision by generating a name with an additional number on it
                            bool success = false;
                            for (int extra = 1; extra < 100; ++extra)//but stop at 100, because really...
                            {
                                newName = nameBase + "_" + AString::number(extra);
                                if (myTable.getLabelKeyFromName(newName) == GiftiLabel::getInvalidLabelKey())
                                {
                                    success = true;
                                    break;
                                }
                            }
                            if (success)
                            {
                                CaretLogWarning("name collision in auto-generated name '" + nameBase + "', changed to '" + newName + "'");
                            } else {
                                throw OperationException("giving up on resolving name collision for auto-generated name '" + nameBase + "'");
                            }
                            myLabel.setName(newName);
                        }
                        int32_t newValue = myTable.addLabel(&myLabel);//don't overwrite any values in the table
                        translate[labelval] = newValue;
                        frameOut[i] = newValue;
                    }
                } else {
                    frameOut[i] = myiter->second;
                }
            }
            outVol->setFrame(frameOut, 0, c);
        }
        if (dropUnused)
        {
            GiftiLabelTable frameTable = myTable;
            frameTable.deleteUnusedLabels(usedValues);
            *(outVol->getMapLabelTable(0)) = frameTable;
        } else {
            *(outVol->getMapLabelTable(0)) = myTable;//set the label table AFTER doing the frame, because we may make new labels while scanning
        }
    }
}
