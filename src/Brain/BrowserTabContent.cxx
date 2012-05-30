
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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

#define __BROWSER_TAB_CONTENT_DECLARE__
#include "BrowserTabContent.h"
#undef __BROWSER_TAB_CONTENT_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventModelGetAll.h"
#include "EventManager.h"
#include "ModelSurface.h"
#include "ModelSurfaceMontage.h"
#include "ModelSurfaceSelector.h"
#include "ModelVolume.h"
#include "ModelWholeBrain.h"
#include "ModelYokingGroup.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "Surface.h"
#include "StructureEnum.h"
#include "VolumeSurfaceOutlineSetModel.h"

using namespace caret;


/**
 * Constructor.
 * @param tabNumber
 *    Number for this tab.
 */
BrowserTabContent::BrowserTabContent(const int32_t tabNumber,
                                     ModelYokingGroup* defaultYokingGroup)
: CaretObject()
{
    this->tabNumber = tabNumber;
    this->surfaceModelSelector = new ModelSurfaceSelector();
    this->selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
    this->volumeModel = NULL;
    this->wholeBrainModel = NULL;
    this->surfaceMontageModel = NULL;
    this->guiName = "";
    this->userName = "";
    this->volumeSurfaceOutlineSetModel = new VolumeSurfaceOutlineSetModel();
    this->selectedYokingGroup = defaultYokingGroup;
}

/**
 * Destructor.
 */
BrowserTabContent::~BrowserTabContent()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    delete this->surfaceModelSelector;
    this->surfaceModelSelector = NULL;
    
    delete this->volumeSurfaceOutlineSetModel;
    this->volumeSurfaceOutlineSetModel = NULL;
}

/**
 * Get the name of this browser tab.
 * Name priority is (1) name set by user, (2) name set by
 * user-interface, and (3) the default name.
 *
 * @return  Name of this tab.
 */
AString 
BrowserTabContent::getName() const
{
    AString s = "(" + AString::number(this->tabNumber + 1) + ") ";
    
    if (this->userName.isEmpty() == false) {
        s += userName;
    }
    else {
        const Model* displayedController =
            this->getModelControllerForDisplay();
        if (displayedController != NULL) {
            const AString name = displayedController->getNameForBrowserTab();
            s += name;
        }
    }
    
    return s;
}

/*
 * Set the name of this tab.  This is typically
 * called by the user-interface with a name that
 * indicates the content of the tab.
 *
 * @param guiName
 *    New name for tab.
 *
void 
BrowserTabContent::setGuiName(const AString& guiName)
{
    this->guiName = guiName;
}
*/

/**
 * Set the user name of this tab.  The user name
 * overrides the default naming.
 *
 * @param userName
 *    User name for tab.
 */
void 
BrowserTabContent::setUserName(const AString& userName)
{
    this->userName = userName;
}

/**
 * @return The user name.
 */
AString 
BrowserTabContent::getUserName() const
{
    return this->userName;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
BrowserTabContent::toString() const
{
    return "WindowTabContent";
}

/**
 * Get the selected model type.
 * 
 * @return The selected model type.
 */   
ModelTypeEnum::Enum 
BrowserTabContent::getSelectedModelType() const
{
    return this->selectedModelType;
}

/**
 * Set the selected model type.
 *
 * @param selectedModelType
 *    New selected model type. 
 */   
void 
BrowserTabContent::setSelectedModelType(ModelTypeEnum::Enum selectedModelType)
{
    this->selectedModelType = selectedModelType;
    this->updateTransformationsForYoking();
}

/**
 * Get the model controller for DISPLAY purposes.  Note: When applying
 * transformations, use getModelControllerForTransformation() so that
 * any yoking is properly carried out.
 * 
 * @return  Pointer to displayed controller or NULL
 *          if none are available.
 */   
Model* 
BrowserTabContent::getModelControllerForDisplay()
{
    Model* mdc = NULL;
    
    switch (this->selectedModelType) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            mdc = this->surfaceModelSelector->getSelectedSurfaceController();
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            mdc = this->surfaceMontageModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            mdc = this->volumeModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            mdc = this->wholeBrainModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_YOKING:
            CaretAssertMessage(0, "Request model display yoking controller for display! Should never happend.");
            break;
    }
    
    return mdc;
}

/**
 * Get the model controller for DISPLAY purposes.  Note: When applying
 * transformations, use getModelControllerForTransformation() so that
 * any yoking is properly carried out.
 * 
 * @return  Pointer to displayed controller or NULL
 *          if none are available.
 */   
const Model* 
BrowserTabContent::getModelControllerForDisplay() const
{
    Model* mdc = NULL;
    
    switch (this->selectedModelType) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            mdc = this->surfaceModelSelector->getSelectedSurfaceController();
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            mdc = this->surfaceMontageModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            mdc = this->volumeModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            mdc = this->wholeBrainModel;
            break;
        case ModelTypeEnum::MODEL_TYPE_YOKING:
            CaretAssertMessage(0, "Request model display yoking controller for display! Should never happend.");
            break;
    }
    
    return mdc;
}

/**
 * Get the model controller for TRANSFORMATION purposes.  If yoked
 * to a different tab, this will return the model controller for
 * the tab to which this is yoked.
 * 
 * @return  Pointer to transformation controller or NULL
 *          if none are available.
 */   
Model* 
BrowserTabContent::getModelControllerForTransformation()
{
    Model* mdc = this->getModelControllerForDisplay();
    if (mdc == NULL) {
        return NULL;
    }
    
    if (mdc->isYokeable()) {
        if (this->selectedYokingGroup != NULL) {
            mdc = this->selectedYokingGroup;
        }
    }
    
    return mdc;
}

/**
 * @return Is the displayed model a right surface that is lateral/medial yoked?
 */
bool 
BrowserTabContent::isDisplayedModelSurfaceRightLateralMedialYoked() const
{
    bool itIs = false;
    
    const ModelSurface* surfaceController = this->getDisplayedSurfaceModel();
    if (surfaceController != NULL) {
        const Surface* surface = surfaceController->getSurface();
        if (surface->getStructure() == StructureEnum::CORTEX_RIGHT) {
            if (this->selectedYokingGroup != NULL) {
                itIs = true;
            }
        }
    }
    
    return itIs;
}


/**
 * Get the displayed surface model.
 * 
 * @return  Pointer to displayed surface model or 
 *          NULL if the displayed model is NOT a 
 *          surface.
 */   
ModelSurface* 
BrowserTabContent::getDisplayedSurfaceModel()
{
    ModelSurface* mdcs =
        dynamic_cast<ModelSurface*>(this->getModelControllerForDisplay());
    return mdcs;
}

/**
 * Get the displayed surface model.
 * 
 * @return  Pointer to displayed surface model or 
 *          NULL if the displayed model is NOT a 
 *          surface.
 */   
const ModelSurface* 
BrowserTabContent::getDisplayedSurfaceModel() const
{
    const ModelSurface* mdcs =
    dynamic_cast<const ModelSurface*>(this->getModelControllerForDisplay());
    return mdcs;
}

/**
 * Get the displayed volume model.
 * 
 * @return  Pointer to displayed volume model or 
 *          NULL if the displayed model is NOT a 
 *          volume.
 */   
ModelVolume* 
BrowserTabContent::getDisplayedVolumeModel()
{
    ModelVolume* mdcv =
        dynamic_cast<ModelVolume*>(this->getModelControllerForDisplay());
    return mdcv;
}

/**
 * Get the displayed whole brain model.
 * 
 * @return  Pointer to displayed whole brain model or 
 *          NULL if the displayed model is NOT a 
 *          whole brain.
 */   
ModelWholeBrain* 
BrowserTabContent::getDisplayedWholeBrainModel()
{
    ModelWholeBrain* mdcwb =
        dynamic_cast<ModelWholeBrain*>(this->getModelControllerForDisplay());
    return mdcwb;

}

/**
 * Get the displayed volume model.
 * 
 * @return  Pointer to displayed volume model or 
 *          NULL if the displayed model is not a 
 *          volume.
 */   
ModelVolume* 
BrowserTabContent::getSelectedVolumeModel()
{
    ModelVolume* mdcv =
        dynamic_cast<ModelVolume*>(this->getModelControllerForDisplay());
    return mdcv;
}

/**
 * Get the selected whole brain model.
 * 
 * @return  Pointer to displayed whole brain model or 
 *          NULL if the displayed model is not a 
 *          whole brain.
 */   
ModelWholeBrain* 
BrowserTabContent::getSelectedWholeBrainModel()
{
    ModelWholeBrain* mdcwb =
        dynamic_cast<ModelWholeBrain*>(this->getModelControllerForDisplay());
    return mdcwb;
}

/**
 * @return Pointer to displayed surface montage model
 * or NULL if the displayed model is not a surface
 * montage model.
 */
ModelSurfaceMontage* 
BrowserTabContent::getDisplayedSurfaceMontageModel()
{
    ModelSurfaceMontage* mdcsm =
    dynamic_cast<ModelSurfaceMontage*>(this->getModelControllerForDisplay());
    return mdcsm;
}

/**
 * @return Pointer to selected surface montage model
 * or NULL if the displayed model is not a surface
 * montage model.
 */
ModelSurfaceMontage* 
BrowserTabContent::getSelectedSurfaceMontageModel()
{
    ModelSurfaceMontage* mdcsm =
    dynamic_cast<ModelSurfaceMontage*>(this->getModelControllerForDisplay());
    return mdcsm;
}

/**
 * Get all of the available surface models.
 * 
 * @return Vector containing all surface models.
 */   
const std::vector<ModelSurface*> 
BrowserTabContent::getAllSurfaceModels() const
{
    return this->allSurfaceModels;
}

/**
 * @return The surface model selector used to 
 * select a surface and structure.
 */
ModelSurfaceSelector* 
BrowserTabContent::getSurfaceModelSelector()
{
    return this->surfaceModelSelector;
}

/**
 * Get the overlay assignments for this tab.
 * 
 * @return  Overlay assignments for this tab or NULL if no valid model.
 */
OverlaySet* 
BrowserTabContent::getOverlaySet()
{
    Model* modelDisplayController = this->getModelControllerForDisplay();
    if (modelDisplayController != NULL) {
       return modelDisplayController->getOverlaySet(this->tabNumber);
    }
    return NULL;
}

/**
 * Get the tab number for this content.
 * 
 * @return  Tab number.
 */
int32_t 
BrowserTabContent::getTabNumber() const
{
    return this->tabNumber;
}

/**
 * Update the selected models.
 */
void 
BrowserTabContent::update(const std::vector<Model*> modelDisplayControllers)
{
    this->surfaceModelSelector->updateSelector(modelDisplayControllers);
    
    const int32_t numModels = static_cast<int32_t>(modelDisplayControllers.size());
    
    this->allSurfaceModels.clear();
    this->surfaceModelSelector->getSelectableSurfaceControllers(allSurfaceModels);
    this->volumeModel = NULL;
    this->wholeBrainModel = NULL;
    this->surfaceMontageModel = NULL;
    
    for (int i = 0; i < numModels; i++) {
        Model* mdc = modelDisplayControllers[i];
        
        ModelSurface* mdcs = dynamic_cast<ModelSurface*>(mdc);
        ModelVolume* mdcv = dynamic_cast<ModelVolume*>(mdc);
        ModelWholeBrain* mdcwb = dynamic_cast<ModelWholeBrain*>(mdc);
        ModelSurfaceMontage* mdcsm = dynamic_cast<ModelSurfaceMontage*>(mdc);
        
        if (mdcs != NULL) {
            /* nothing to do since the surface model selector handles surfaces */
        }
        else if (mdcv != NULL) {
            CaretAssertMessage((this->volumeModel == NULL), "There is more than one volume model.");
            this->volumeModel = mdcv;
        }
        else if (mdcwb != NULL) {
            CaretAssertMessage((this->wholeBrainModel == NULL), "There is more than one whole brain model.");
            this->wholeBrainModel = mdcwb;
        }
        else if (mdcsm != NULL) {
            CaretAssertMessage((this->surfaceMontageModel == NULL), "There is more than one surface montage model.");
            this->surfaceMontageModel = mdcsm;
        }
        else {
            CaretAssertMessage(0, (AString("Unknown type of brain model.") + mdc->getNameForGUI(true)));
        }
    }
    
    switch (this->selectedModelType) {
        case ModelTypeEnum::MODEL_TYPE_INVALID:
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE:
            if (this->surfaceModelSelector->getSelectedSurfaceController() == NULL) {
                this->selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE:
            if (this->surfaceMontageModel == NULL) {
                this->selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES:
            if (this->volumeModel == NULL) {
                this->selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN:
            if (this->wholeBrainModel == NULL) {
                this->selectedModelType = ModelTypeEnum::MODEL_TYPE_INVALID;
            }
            break;
        case ModelTypeEnum::MODEL_TYPE_YOKING:
            CaretAssertMessage(0, "Request model display yoking controller for display! Should never happend.");
            break;
    }
    
    if (this->selectedModelType == ModelTypeEnum::MODEL_TYPE_INVALID) {
        if (this->surfaceModelSelector->getSelectedSurfaceController() != NULL) {
            this->selectedModelType = ModelTypeEnum::MODEL_TYPE_SURFACE;
        }
        else if (this->volumeModel != NULL) {
            this->selectedModelType = ModelTypeEnum::MODEL_TYPE_VOLUME_SLICES;
        }
        else if (this->wholeBrainModel != NULL) {
            this->selectedModelType = ModelTypeEnum::MODEL_TYPE_WHOLE_BRAIN;
        }
        else if (this->surfaceMontageModel != NULL) {
            this->selectedModelType = ModelTypeEnum::MODEL_TYPE_SURFACE_MONTAGE;
        }
    }

    this->updateTransformationsForYoking();
}

/**
 * Is the surface model selection valid?
 *
 * @return bool indicating validity.
 */
bool 
BrowserTabContent::isSurfaceModelValid() const
{
    bool valid = (this->allSurfaceModels.empty() == false);
    return valid;
}

/**
 * Is the volume model selection valid?
 *
 * @return bool indicating validity.
 */
bool 
BrowserTabContent::isVolumeSliceModelValid() const
{
    bool valid = (this->volumeModel != NULL);
    return valid;
}

/**
 * Is the whole brain model selection valid?
 *
 * @return bool indicating validity.
 */
bool 
BrowserTabContent::isWholeBrainModelValid() const
{
    bool valid = (this->wholeBrainModel != NULL);
    return valid;
}

/**
 * Is the surface montage model selection valid?
 *
 * @return bool indicating validity.
 */
bool 
BrowserTabContent::isSurfaceMontageModelValid() const
{
    bool valid = (this->surfaceMontageModel != NULL);
    return valid;
}

/**
 * Receive an event.
 * 
 * @param event
 *     The event that the receive can respond to.
 */
void 
BrowserTabContent::receiveEvent(Event* /*event*/)
{
}

/**
 * Update the transformations for yoking prior to drawing.
 * Does nothing if not yoked.
 */
void 
BrowserTabContent::updateTransformationsForYoking()
{
    Model* transformController = this->getModelControllerForTransformation();
    ModelYokingGroup* yokingController = 
    dynamic_cast<ModelYokingGroup*>(transformController);
    if (yokingController != NULL) {
        Model* mdc = this->getModelControllerForDisplay();
        if (mdc != NULL) {
            mdc->copyTransformationsAndViews(*yokingController, 
                                     0, // always used window 0  
                                     this->tabNumber);
        }
    }
}

/**
 * Get the map files for which a palette should be displayed in the 
 * graphcis window.  Note that the order of map files and indices starts
 * with the bottom most layer and ends with the top most overlay.
 *
 * @param mapFiles
 *    Outut Map files that should have a palette displayed in the graphics window.
 * @param mapIndices
 *    Output Indices of the maps in the mapFiles.
 */
void 
BrowserTabContent::getDisplayedPaletteMapFiles(std::vector<CaretMappableDataFile*>& mapFiles,
                                               std::vector<int32_t>& mapIndices)
{
    mapFiles.clear();
    mapIndices.clear();
    
    OverlaySet* overlaySet = this->getOverlaySet();
    const int32_t numOverlays = overlaySet->getNumberOfDisplayedOverlays();
    for (int32_t i = (numOverlays - 1); i >= 0; i--) {
        Overlay* overlay = overlaySet->getOverlay(i);
        if (overlay->isEnabled()) {
            if (overlay->isPaletteDisplayEnabled()) {
                CaretMappableDataFile* mapFile;
                int32_t mapFileIndex;
                overlay->getSelectionData(mapFile, mapFileIndex);
                if (mapFile != NULL) {
                    if (mapFile->isMappedWithPalette()) {
                        if ((mapFileIndex >= 0) 
                            && (mapFileIndex < mapFile->getNumberOfMaps())) {
                            mapFiles.push_back(mapFile);
                            mapIndices.push_back(mapFileIndex);
                        }
                    }
                }
            }
        }
    }
}

/**
 * @return The volume surface outline model for this tab.
 */
VolumeSurfaceOutlineSetModel* 
BrowserTabContent::getVolumeSurfaceOutlineSet()
{
    return this->volumeSurfaceOutlineSetModel;
}

/**
 * @return The volume surface outline model for this tab.
 */
const VolumeSurfaceOutlineSetModel* 
BrowserTabContent::getVolumeSurfaceOutlineSet() const
{
    return this->volumeSurfaceOutlineSetModel;
}

/**
 * @return The model yoking group (NULL if NOT yoked).
 */
ModelYokingGroup* 
BrowserTabContent::getSelectedYokingGroup()
{
    return this->selectedYokingGroup;
}

/**
 * Set the model yoking group to the given value.
 * @param selectedYokingGroup
 *     New value for yoking group.
 */
void 
BrowserTabContent::setSelectedYokingGroup(ModelYokingGroup* selectedYokingGroup)
{
    this->selectedYokingGroup = selectedYokingGroup;
}


