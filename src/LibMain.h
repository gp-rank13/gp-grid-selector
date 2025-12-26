// Grid extension for Gig Performer by @rank13

#pragma once

#include <gigperformer/sdk/GPUtils.h>
#include <gigperformer/sdk/GigPerformerAPI.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "GridWindow.h"
#include "Version.h"

const std::string XMLProductDescription =
    "<Library>"
    "<Product Name=\"" + PROJECT_TITLE + "\" Version=\"" + PROJECT_VERSION + "\" BuildDate=\"" + PROJECT_BUILD_DATE + "\"></Product>"
    "<Description>" + PROJECT_DESCRIPTION + "</Description>"
    "<ImagePath></ImagePath>"
    "</Library>";

// Globals
extern bool isSetlistMode;
extern int directSelectCount;
extern std::string Grid_Text;
extern int Grid_Font_Size;
extern int Grid_Duration;
extern int Grid_Width;
extern int Grid_Height;
extern int Grid_Position_X;
extern int Grid_Position_Y;

// Define your class here - it MUST be called LibMain and it must inherit from GigPerformerAPI

class LibMain : public gigperformer::sdk::GigPerformerAPI
{
public:
    // These must be here but no need to do anything unless you want extra behavior
     LibMain(LibraryHandle handle) : GigPerformerAPI(handle) {}
    virtual ~LibMain() {}
        
    void OnOpen() override
    {
        GridWindow::initialize();
    }
    
    void OnClose() override
    {
        GridWindow::finalize();
    }

    void OnStatusChanged(GPStatusType status) override; 
    void OnModeChanged(int mode) override;
    void OnSongChanged(int oldIndex, int newIndex) override;
    void OnSongPartChanged(int oldIndex, int newIndex) override;
    void OnSetlistChanged(const std::string &newSetlistName) override;
    void OnRackspaceActivated() override;
    void OnWidgetValueChanged(const std::string &widgetName, double newValue) override;  

    void Initialization() override
    {
        // Register all the methods that you are going to actually use, i.e, the ones you declared above as override
        registerCallback("OnOpen");
        registerCallback("OnClose");
        registerCallback("OnStatusChanged");
        registerCallback("OnSongChanged");
        registerCallback("OnSongPartChanged");
        registerCallback("OnSetlistChanged");
        registerCallback("OnRackspaceActivated");
        registerCallback("OnModeChanged");
        registerCallback("OnWidgetValueChanged");

        listenForWidget("GPGS_DISPLAY", true);

        for (int i = 1; i <= 20; ++i) {  
            listenForWidget("GPGS_DS" + std::to_string(i), true);
        }
    }

private:
    StringArray getSongNames();
    StringArray getSongPartNames(int songIndex);
    StringArray getRackspaceNames();
   
    // An XML description of your product
    std::string GetProductDescription() override // This MUST be defined in your class
    {
        return XMLProductDescription;
    }
};