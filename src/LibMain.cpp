// Grid extension for Gig Performer by @rank13
#include "LibMain.h"
#include "Panel.h"

namespace gigperformer {
   namespace sdk {
      GigPerformerAPI *CreateGPExtension(LibraryHandle handle) {
         return new LibMain(handle);
      }
   } // namespace sdk
} // namespace gigperformer

// *** Global Variables *** 
bool isGigFileLoading = false;
bool isSetlistMode = false;
bool selectorDisplayedWhenTunerActivated = false;
//int directSelectCount = 4;
std::string Grid_Text = "GP Grid";
int Grid_Duration = -1;
int Grid_Font_Size = 80;

int LibMain::GetPanelCount() {
   return panelNames.size();
}

std::string LibMain::GetPanelName(int index) {
   std::string text; 
   size_t panelIndex = (size_t)index;
   if (panelIndex >= 0 && panelIndex < panelNames.size())
      text = panelNames[index];
         
   return text;      
}

std::string LibMain::GetPanelXML(int index) {
   std::string text;
   size_t panelIndex = (size_t)index;
   if (panelIndex >= 0 && panelIndex < panelNames.size()) {
        text = panelXML[index];
   }
   return text;
}

void LibMain::OnStatusChanged(GPStatusType status) {
    switch (status) {
        case GPStatus_GigStartedLoading:
            isGigFileLoading = true;
            break;
        case GPStatus_GigFinishedLoading:
            isGigFileLoading = false;
            setWidgetValue("GPGS_DISPLAY", 0.0);
            if (inSetlistMode()) {
               GridWindow::presetChanged(getCurrentSongIndex(), getSongNames());
            } else {
               GridWindow::presetChanged(getCurrentRackspaceIndex(), getRackspaceNames()); 
            }
            break;
        default:
            break;
    }
}

void LibMain::OnSongChanged(int, int newIndex) {
    if (isGigFileLoading) return;
    if (newIndex >= 0 && inSetlistMode()) {
        GridWindow::presetChanged(newIndex, getSongNames());
    }
}

void LibMain::OnSongPartChanged(int oldIndex, int newIndex) {
   if (isGigFileLoading) return;
   if (newIndex >= 0 && oldIndex != newIndex && inSetlistMode()) {
      GridWindow::sceneChanged(newIndex, getSongPartNames(getCurrentSongIndex()));
   }
}

void LibMain::OnSetlistChanged(const std::string&) {
   if (isGigFileLoading) return;
   if (inSetlistMode()) {
      GridWindow::presetChanged(getCurrentSongIndex(), getSongNames());    
   }
}

void LibMain::OnRackspaceActivated() {
    if (isGigFileLoading) return;
    if (!inSetlistMode()) {
        GridWindow::presetChanged(getCurrentRackspaceIndex(), getRackspaceNames()); 
    }
}

void LibMain::OnVariationChanged(int oldIndex, int newIndex) {
    if (isGigFileLoading) return;
    if (newIndex >= 0 && oldIndex != newIndex && !inSetlistMode()) {
        GridWindow::sceneChanged(newIndex,getVariationNames(getCurrentRackspaceIndex()));
    }
}

void LibMain::OnModeChanged(int mode) {
   //isSetlistMode = (mode == GP_SetlistMode) ? true : false;
   /*
    if (isGigFileLoading || !isFirstGigFileOpened) 
        return;
    if (mode == GP_SetlistMode) 
        ExtensionWindow::selectSongForCurrentButton();
   */
   if (isGigFileLoading) return;
   GridWindow::songRackspaceModeChanged();
   if (mode == GP_SetlistMode) {
      GridWindow::presetChanged(getCurrentSongIndex(), getSongNames());    
   } else {
      GridWindow::presetChanged(getCurrentRackspaceIndex(), getRackspaceNames()); 
   }
}

void LibMain::OnWidgetValueChanged(const std::string& widgetName, double newValue) {
    if (isGigFileLoading) return;
    if (widgetName == "GPGS_DISPLAY") {
         if (newValue == 1.0) {
            GridWindow::showGrid();
         } else {
            GridWindow::hideGrid();
         }
    } else if (widgetName == "GPGS_MODE") {
         if (newValue == 1.0) {
            GridWindow::toggleGridDisplayMode();
         }
    } else if (widgetName == "GPGS_BANKUP") {
      if (newValue == 1.0) {
         GridWindow::gridBank(false);
      }
    } else if (widgetName == "GPGS_BANKDOWN") {
      if (newValue == 1.0) {
         GridWindow::gridBank(true);
      }
    } else if (widgetName.starts_with("GPGS_DS")) {
      if (newValue == 1.0) {
         GridWindow::directSelect((String)widgetName);
      }
    }
}

void LibMain::OnTunerModeChanged(bool visible) {
   //GridWindow::tunerDisplayChanged(visible);
   if (visible) {
      selectorDisplayedWhenTunerActivated = getWidgetValue("GPGS_DISPLAY") == 1.0;
      setWidgetValue("GPGS_DISPLAY", 0.0);
   } else {
      setWidgetValue("GPGS_DISPLAY", selectorDisplayedWhenTunerActivated ? 1.0 : 0.0);
      selectorDisplayedWhenTunerActivated = false;
   }
}

StringArray LibMain::getSongNames() {
    StringArray songNames;
    String songName;
    int songCount = getSongCount();
    for (int i = 0; i < songCount; ++i) { 
        songName = getSongName(i);
        songNames.add(songName);
    }
    return songNames;
}

StringArray LibMain::getSongPartNames(int songIndex) {
    StringArray songPartNames;
    String songPartName;
    int songPartCount = getSongpartCount(songIndex);
    for (int i = 0; i < songPartCount; ++i) { 
        songPartName = getSongpartName(songIndex, i);
        songPartNames.add(songPartName);
    }
    return songPartNames;
}

StringArray LibMain::getRackspaceNames() {
    StringArray rackspaceNames;
    String rackspaceName;
    for (int i = 0; i < getRackspaceCount(); ++i) { 
        rackspaceName = getRackspaceName(i);
        rackspaceNames.add(rackspaceName);
    }
    return rackspaceNames;
}

StringArray LibMain::getVariationNames(int rackspaceIndex) {
    StringArray variationNames;
    String variationName;
    int variationCount = getVariationCount(rackspaceIndex);
    for (int i = 0; i < variationCount; ++i) { 
        variationName = getVariationName(rackspaceIndex, i);
        variationNames.add(variationName);
    }
    return variationNames;
}