// Grid selector extension for Gig Performer by @rank13

#include <juce_events/juce_events.h>
#include "LibMain.h"
#include "GridWindow.h"

GridWindow* GridWindow::gridWindow = nullptr;
//LibMain* lib = new LibMain(nullptr);   
LibMain* LibMain::lib = nullptr;

int gridFontSizeMin = 40;
int gridFontSizeMax = 50;

GridWindow::GridWindow ()
{   
    addToDesktop (ComponentPeer::windowIsTemporary);
    auto bounds = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()->totalArea;
    setBounds (bounds);
    setAlwaysOnTop (true);
    setVisible (false);

    gridTimer.reset (new GridTimer);

    grid.reset (new GridSelectorMain ());
    addAndMakeVisible(grid.get());

    gridTitle.reset (new GridSelectorTitle ());
    addAndMakeVisible(gridTitle.get());

    gridBankUp.reset (new GridSelectorBankUp ());
    addAndMakeVisible(gridBankUp.get());

    gridBankDown.reset (new GridSelectorBankDown ());
    addAndMakeVisible(gridBankDown.get());

    gridMenu.reset (new GridMenu ());
    addChildComponent(gridMenu.get());

    Path p1;
    p1.loadPathFromData (preferencesPath, sizeof (preferencesPath));
    preferencesButton.reset (new ShapeButton ( "prefsButton", Colours::lightgrey, Colours::grey, Colours::darkgrey ));
    preferencesButton->setShape (p1, true, true, false);
    preferencesButton->setClickingTogglesState(true);
    preferencesButton->setTooltip("Preferences");
    preferencesButton->addListener (this);
    addAndMakeVisible (preferencesButton.get());

    Path p2;
    p2.loadPathFromData (closePath, sizeof (closePath));
    closeButton.reset (new ShapeButton ( "closeButton", Colours::lightgrey, Colours::grey, Colours::darkgrey ));
    closeButton->setShape (p2, true, true, false);
    closeButton->setClickingTogglesState(false);
    closeButton->setTooltip("Close");
    closeButton->addListener (this);
    addAndMakeVisible (closeButton.get());

    Path p3;
    p3.loadPathFromData (backUpArrowPath, sizeof (backUpArrowPath));
    p3.applyTransform(juce::AffineTransform::verticalFlip(0));
    backButton.reset (new ShapeButton ( "backButton", Colours::lightgrey, Colours::grey, Colours::darkgrey ));
    backButton->setShape (p3, true, true, false);
    backButton->setClickingTogglesState(false);
    backButton->setTooltip("Back");
    backButton->addListener (this);
    addChildComponent (backButton.get());

    globeIcon.loadPathFromData (globePath, sizeof (globePath));

    preferencesCloseButton.reset (new ShapeButton ( "prefsCloseButton", Colours::lightgrey, Colours::grey, Colours::darkgrey ));
    preferencesCloseButton->setShape (p2, true, true, false);
    preferencesCloseButton->setClickingTogglesState(false);
    preferencesCloseButton->setTooltip("Close");
    preferencesCloseButton->addListener (this);
    gridMenu->addAndMakeVisible (preferencesCloseButton.get());

    gridColumnUpButton.reset (new GridPreferenceUpButton( "columnUp", Colours::lightgrey, Colours::grey, Colours::darkgrey ));
    gridColumnUpButton->addListener (this);
    gridMenu->addAndMakeVisible(gridColumnUpButton.get());

    gridColumnDownButton.reset (new GridPreferenceDownButton( "columnDown", Colours::lightgrey, Colours::grey, Colours::darkgrey ));
    gridColumnDownButton->addListener (this);
    gridMenu->addAndMakeVisible(gridColumnDownButton.get());
    
    gridRowUpButton.reset (new GridPreferenceUpButton( "rowUp", Colours::lightgrey, Colours::grey, Colours::darkgrey ));
    gridRowUpButton->addListener (this);
    gridMenu->addAndMakeVisible(gridRowUpButton.get());

    gridRowDownButton.reset (new GridPreferenceDownButton( "rowDown", Colours::lightgrey, Colours::grey, Colours::darkgrey ));
    gridRowDownButton->addListener (this);
    gridMenu->addAndMakeVisible(gridRowDownButton.get());

    gridBankRowUpButton.reset (new GridPreferenceUpButton( "bankRowUp", Colours::lightgrey, Colours::grey, Colours::darkgrey ));
    gridBankRowUpButton->addListener (this);
    gridMenu->addAndMakeVisible(gridBankRowUpButton.get());

    gridBankRowDownButton.reset (new GridPreferenceDownButton( "bankRowDown", Colours::lightgrey, Colours::grey, Colours::darkgrey ));
    gridBankRowDownButton->addListener (this);
    gridMenu->addAndMakeVisible(gridBankRowDownButton.get());

    gridDirectSelectUpButton.reset (new GridPreferenceUpButton( "directSelectUp", Colours::lightgrey, Colours::grey, Colours::darkgrey ));
    gridDirectSelectUpButton->addListener (this);
    gridMenu->addAndMakeVisible(gridDirectSelectUpButton.get());

    gridDirectSelectDownButton.reset (new GridPreferenceDownButton( "directSelectDown", Colours::lightgrey, Colours::grey, Colours::darkgrey ));
    gridDirectSelectDownButton->addListener (this);
    gridMenu->addAndMakeVisible(gridDirectSelectDownButton.get());

    // Preferences toggle buttons
    String imageBase64Off = TOGGLE_OFF;
    Image toggleOff;
    MemoryOutputStream mosOff;
    auto resultOff = Base64::convertFromBase64(mosOff, imageBase64Off);
    if (resultOff) {
        toggleOff = ImageFileFormat::loadFrom(mosOff.getData(), mosOff.getDataSize());
    }
    String imageBase64On = TOGGLE_ON;
    Image toggleOn;
    MemoryOutputStream mosOn;
    auto resultOn = Base64::convertFromBase64(mosOn, imageBase64On);
    if (resultOn) {
        toggleOn = ImageFileFormat::loadFrom(mosOn.getData(), mosOn.getDataSize());
    }
    DrawableImage on, off;
    on.setImage(toggleOn);
    off.setImage(toggleOff);
    /*
    prefToggleLatchingSwitches.reset (new DrawableButton("latchingSwitches", DrawableButton::ImageFitted));
    prefToggleLatchingSwitches->setImages(&off, 0, 0, 0, &on);
    prefToggleLatchingSwitches->setClickingTogglesState(true);
    prefToggleLatchingSwitches->setRepaintsOnMouseActivity(true); 
    prefToggleLatchingSwitches->addListener (this);
    prefToggleLatchingSwitches->setColour (DrawableButton::backgroundOnColourId, Colour(0x00000000));
    gridMenu->addAndMakeVisible(prefToggleLatchingSwitches.get());
    */
    /*
    prefToggleCloseOnSelect.reset (new DrawableButton("closeOnSelect", DrawableButton::ImageFitted));
    prefToggleCloseOnSelect->setImages(&off, 0, 0, 0, &on);
    prefToggleCloseOnSelect->setClickingTogglesState(true);
    prefToggleCloseOnSelect->setRepaintsOnMouseActivity(true); 
    prefToggleCloseOnSelect->addListener (this);
    prefToggleCloseOnSelect->setColour (DrawableButton::backgroundOnColourId, Colour(0x00000000));
    gridMenu->addAndMakeVisible(prefToggleCloseOnSelect.get());
    */
    prefToggleDisplaySceneNameInTitle.reset (new DrawableButton("displaySceneName", DrawableButton::ImageFitted));
    prefToggleDisplaySceneNameInTitle->setImages(&off, 0, 0, 0, &on);
    prefToggleDisplaySceneNameInTitle->setClickingTogglesState(true);
    prefToggleDisplaySceneNameInTitle->setRepaintsOnMouseActivity(true); 
    prefToggleDisplaySceneNameInTitle->addListener (this);
    prefToggleDisplaySceneNameInTitle->setColour (DrawableButton::backgroundOnColourId, Colour(0x00000000));
    gridMenu->addAndMakeVisible(prefToggleDisplaySceneNameInTitle.get());

    prefToggleDisplayZeroBasedNumbers.reset (new DrawableButton("displayZeroBasedNumbers", DrawableButton::ImageFitted));
    prefToggleDisplayZeroBasedNumbers->setImages(&off, 0, 0, 0, &on);
    prefToggleDisplayZeroBasedNumbers->setClickingTogglesState(true);
    prefToggleDisplayZeroBasedNumbers->setRepaintsOnMouseActivity(true); 
    prefToggleDisplayZeroBasedNumbers->addListener (this);
    prefToggleDisplayZeroBasedNumbers->setColour (DrawableButton::backgroundOnColourId, Colour(0x00000000));
    gridMenu->addAndMakeVisible(prefToggleDisplayZeroBasedNumbers.get());

    prefOnSelectionPresetMenu.reset (new GridPopupMenuPreset ());
    prefOnSelectionPresetMenu->selectedItem = 1;
    prefOnSelectionPresetMenu->selectedItemText = ON_SELECTION_MENU_PRESETS[0];
    gridMenu->addAndMakeVisible(prefOnSelectionPresetMenu.get());

    prefOnSelectionSceneMenu.reset (new GridPopupMenuScene ());
    prefOnSelectionSceneMenu->selectedItem = 2;
    prefOnSelectionSceneMenu->selectedItemText = ON_SELECTION_MENU_SCENES[1];
    gridMenu->addAndMakeVisible(prefOnSelectionSceneMenu.get());
    
    // Component position/size
    auto y = bounds.getHeight() * 0.1f;
    auto rightX = bounds.getWidth() - y;
    gridTitle->setBounds(bounds.withHeight(y));
    grid->setBounds(bounds.withTop(y).withRight(rightX));
    gridBankUp->setBounds(rightX, y, y, grid->getHeight() / 2);
    gridBankDown->setBounds(rightX, y + (grid->getHeight() / 2), y, grid->getHeight() / 2);
    backButton->setBounds(getWidth() - 220, 40, 35, 35);
    preferencesButton->setBounds(getWidth() - 140, 38, 38, 38);
    closeButton->setBounds(getWidth() - 60, 40, 35, 35);
    gridMenu->setBounds(bounds.getWidth() - 885, y - 15, 810, 650 );
    preferencesCloseButton->setBounds(gridMenu->getWidth() - 75, 30, 35, 35);
    gridColumnDownButton->setBounds(260,170,50,50);
    gridColumnUpButton->setBounds(320,170,50,50);
    gridRowDownButton->setBounds(260,230,50,50);
    gridRowUpButton->setBounds(320,230,50,50);
    gridBankRowDownButton->setBounds(260,380,50,50);
    gridBankRowUpButton->setBounds(320,380,50,50);
    gridDirectSelectDownButton->setBounds(260,530,50,50);
    gridDirectSelectUpButton->setBounds(320,530,50,50);
    //prefToggleLatchingSwitches->setBounds(30,640,50,50);
    //prefToggleCloseOnSelect->setBounds(30,675,50,50);
    prefToggleDisplayZeroBasedNumbers->setBounds(430,495,50,50);
    prefToggleDisplaySceneNameInTitle->setBounds(430,550,50,50);

    prefOnSelectionPresetMenu->setBounds(430,225,300,50);
    prefOnSelectionSceneMenu->setBounds(430,345,300,50);
}

GridWindow::~GridWindow()
{
    grid = nullptr;
    gridWindow = nullptr;
    gridTimer = nullptr;
    gridMenu = nullptr;
    gridTitle = nullptr;
    gridBankUp = nullptr;
    gridBankDown = nullptr;
}

void GridWindow::initialize()
{
    MessageManager::getInstance()->callAsync([]() {
                                                        if (gridWindow == nullptr)
                                                        {
                                                            gridWindow = new GridWindow();
                                                        }
                                                        jassert(gridWindow != nullptr);
                                                        gridWindow->readPreferences();
                                                    });
}

void GridWindow::finalize()
{
    if (gridWindow != nullptr)
    {
        delete gridWindow;
        gridWindow = nullptr;
    }
}

void GridWindow::showGrid()
{
    if (gridWindow != nullptr and gridWindow->grid != nullptr and gridWindow->gridTimer != nullptr)
    {
        MessageManager::getInstance()->callAsync([]() {
                                                        gridWindow->setVisible(false);
                                                        gridWindow->setVisible(true);
                                                        if (Grid_Duration >= 0)
                                                            gridWindow->gridTimer->startTimer(Grid_Duration);
                                                    });
    }
}

void GridWindow::hideGrid()
{
    if (gridWindow != nullptr and gridWindow->grid != nullptr)
    {
        MessageManager::getInstance()->callAsync([]() {
                                                        gridWindow->setVisible(false);
                                                        //gridWindow->gridPresetMode = true;
                                                        //gridWindow->updateGridItems(true);
                                                        //LibMain::lib->setWidgetValue("GPGS_DISPLAY", 0.0);
                                                    });
    }
}

void GridWindow::setGridSize(int width, int height)
{
    if (gridWindow != nullptr and gridWindow->grid != nullptr)
    {
        auto bounds = gridWindow->grid->getBounds();
        int x = bounds.getX();
        int y = bounds.getY();
        MessageManager::getInstance()->callAsync([x,y,width,height]() {
                                                        gridWindow->grid->setBounds(x, y, width, height);
                                                        gridWindow->grid->setVisible(false);
                                                        gridWindow->grid->setVisible(true);
                                                            });
    }
}

void GridWindow::setGridPosition(int x, int y)
{
    if (gridWindow != nullptr and gridWindow->grid != nullptr)
    {
        auto bounds = gridWindow->grid->getBounds();
        int width = bounds.getWidth();
        int height = bounds.getHeight();
        MessageManager::getInstance()->callAsync([x,y,width,height]() {
                                                        gridWindow->grid->setBounds(x, y, width, height);
                                                        gridWindow->grid->setVisible(false);
                                                        gridWindow->grid->setVisible(true);
                                                            });
    }
}

void GridWindow::setGridPositionSize(int x, int y, int width, int height)
{
    if (gridWindow != nullptr and gridWindow->grid != nullptr)
    {
        MessageManager::getInstance()->callAsync([x,y,width,height]() {
                                                        gridWindow->grid->setBounds(x, y, width, height);
                                                            });
    }
}

void GridWindow::setGridPositionByName(std::string positionName)
{
    if (gridWindow != nullptr and gridWindow->grid != nullptr)
    {
        auto bounds = gridWindow->grid->getBounds();
        Rectangle<int> area (0, 0, bounds.getWidth(), bounds.getHeight());
        Rectangle<int> result; 
        if (positionName == "Center")
        {
                RectanglePlacement placement (RectanglePlacement::xMid
                                       | RectanglePlacement::yMid
                                       | RectanglePlacement::doNotResize);

        result = placement.appliedTo (bounds, Desktop::getInstance().getDisplays()
                                                         .getPrimaryDisplay()->userArea.reduced (0));
        } else if (positionName == "TopRight")
        {
            RectanglePlacement placement (RectanglePlacement::xRight
                                       | RectanglePlacement::yTop
                                       | RectanglePlacement::doNotResize);

        result = placement.appliedTo (bounds, Desktop::getInstance().getDisplays()
                                                         .getPrimaryDisplay()->userArea.reduced (0));
        } else if (positionName == "TopRight")
        {
            RectanglePlacement placement (RectanglePlacement::xRight
                                       | RectanglePlacement::yTop
                                       | RectanglePlacement::doNotResize);

        result = placement.appliedTo (bounds, Desktop::getInstance().getDisplays()
                                                         .getPrimaryDisplay()->userArea.reduced (0));
        } else if (positionName == "TopLeft")
        {
            RectanglePlacement placement (RectanglePlacement::xLeft
                                       | RectanglePlacement::yTop
                                       | RectanglePlacement::doNotResize);

        result = placement.appliedTo (bounds, Desktop::getInstance().getDisplays()
                                                         .getPrimaryDisplay()->userArea.reduced (0));
        } else if (positionName == "TopCenter")
        {
            RectanglePlacement placement (RectanglePlacement::xMid
                                       | RectanglePlacement::yTop
                                       | RectanglePlacement::doNotResize);

        result = placement.appliedTo (bounds, Desktop::getInstance().getDisplays()
                                                         .getPrimaryDisplay()->userArea.reduced (0));
        } else
        {
            
            result = bounds;
        }
        MessageManager::getInstance()->callAsync([result]() {
                                                        gridWindow->grid->setTopLeftPosition(result.getX(),result.getY());
                                                        gridWindow->grid->setVisible(false);
                                                        gridWindow->grid->setVisible(true);
                                                            });
    }
}

void GridWindow::setGridNamedPositionSize(std::string positionName, int width, int height)
{
   if (gridWindow != nullptr and gridWindow->grid != nullptr)
    {
        Rectangle<int> area (0, 0, width, height);
        Rectangle<int> result; 
        if (positionName == "Center")
        {
                RectanglePlacement placement (RectanglePlacement::xMid
                                       | RectanglePlacement::yMid
                                       | RectanglePlacement::doNotResize);

        result = placement.appliedTo (area, Desktop::getInstance().getDisplays()
                                                         .getPrimaryDisplay()->userArea.reduced (0));
        } else if (positionName == "TopRight")
        {
            RectanglePlacement placement (RectanglePlacement::xRight
                                       | RectanglePlacement::yTop
                                       | RectanglePlacement::doNotResize);

        result = placement.appliedTo (area, Desktop::getInstance().getDisplays()
                                                         .getPrimaryDisplay()->userArea.reduced (0));
        } else if (positionName == "TopRight")
        {
            RectanglePlacement placement (RectanglePlacement::xRight
                                       | RectanglePlacement::yTop
                                       | RectanglePlacement::doNotResize);

        result = placement.appliedTo (area, Desktop::getInstance().getDisplays()
                                                         .getPrimaryDisplay()->userArea.reduced (0));
        } else if (positionName == "TopLeft")
        {
            RectanglePlacement placement (RectanglePlacement::xLeft
                                       | RectanglePlacement::yTop
                                       | RectanglePlacement::doNotResize);

        result = placement.appliedTo (area, Desktop::getInstance().getDisplays()
                                                         .getPrimaryDisplay()->userArea.reduced (0));
        } else if (positionName == "TopCenter")
        {
            RectanglePlacement placement (RectanglePlacement::xMid
                                       | RectanglePlacement::yTop
                                       | RectanglePlacement::doNotResize);

        result = placement.appliedTo (area, Desktop::getInstance().getDisplays()
                                                         .getPrimaryDisplay()->userArea.reduced (0));
        } else
        {
            auto bounds = gridWindow->grid->getBounds();
            bounds.setWidth(width);
            bounds.setHeight(height);
            result = bounds;
        }
        MessageManager::getInstance()->callAsync([result]() {

                                                        gridWindow->grid->setBounds(result);
                                                        gridWindow->grid->setVisible(false);
                                                            });
    }
}

void GridWindow::setGridDuration(int ms)
{
    if (gridWindow != nullptr and gridWindow->grid != nullptr and gridWindow->gridTimer != nullptr )
    {
        MessageManager::getInstance()->callAsync([ms]() {
                                                        gridWindow->gridTimer->stopTimer();
                                                        //if (Grid_Duration >= 0)
                                                            gridWindow->gridTimer->startTimer(ms);
                                                        });
    }

}

void GridWindow::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == closeButton.get()) {
        gridMenu->setVisible(false);
        preferencesButton->setToggleState(false, juce::NotificationType::dontSendNotification);
        LibMain::lib->setWidgetValue("GPGS_DISPLAY", 0.0);
        hideGrid();
    } else if (buttonThatWasClicked == backButton.get()) {
        setGridDisplayMode(gridPresetMode == Mode_stomps ? Mode_scenes : Mode_presets);
    } else if (buttonThatWasClicked == preferencesButton.get()) {
        gridMenu->setVisible(buttonThatWasClicked->getToggleState());
        if (!buttonThatWasClicked->getToggleState()) {
            savePreferences();
        }
    } else if (buttonThatWasClicked == preferencesCloseButton.get()) {
        gridMenu->setVisible(false);
        preferencesButton->setToggleState(false, juce::NotificationType::dontSendNotification);
        savePreferences();
    } else if (buttonThatWasClicked == gridColumnUpButton.get()) {
        gridItemWidthCount = jmin(gridItemWidthCount + 1, GRID_COLUMNS_MAX);
        updateGrid();
        gridMenu->repaint();
    } else if (buttonThatWasClicked == gridColumnDownButton.get()) {
        gridItemWidthCount = jmax(gridItemWidthCount - 1, GRID_COLUMNS_MIN);
        updateGrid();
        gridMenu->repaint();
    } else if (buttonThatWasClicked == gridRowUpButton.get()) {
        gridItemHeightCount = jmin(gridItemHeightCount + 1, GRID_ROWS_MAX);
        updateGrid();
        gridMenu->repaint();
    } else if (buttonThatWasClicked == gridRowDownButton.get()) {
        gridItemHeightCount = jmax(gridItemHeightCount - 1, GRID_ROWS_MIN);
        gridBankRowCount = jmin(gridItemHeightCount, gridBankRowCount);
        updateGrid();
        gridMenu->repaint();
    } else if (buttonThatWasClicked == gridBankRowUpButton.get()) {
        gridBankRowCount = jmin(gridBankRowCount + 1, gridItemHeightCount);
        updateGrid();
        gridMenu->repaint();
    } else if (buttonThatWasClicked == gridBankRowDownButton.get()) {
        gridBankRowCount = jmax(gridBankRowCount - 1, 1);
        updateGrid();
        gridMenu->repaint();
    } else if (buttonThatWasClicked == gridDirectSelectUpButton.get()) {
        directSelectCount = jmin(directSelectCount + 1, GRID_DIRECT_SELECT_MAX);
        updateGrid();
        gridMenu->repaint();
    } else if (buttonThatWasClicked == gridDirectSelectDownButton.get()) {
        directSelectCount = jmax(directSelectCount - 1, 0);
        updateGrid();
        gridMenu->repaint();
    } else if (buttonThatWasClicked == prefToggleDisplaySceneNameInTitle.get()) {
        gridDisplaySceneNameInTitle = buttonThatWasClicked->getToggleState();
        gridTitle->repaint();
    } else if (buttonThatWasClicked == prefToggleDisplayZeroBasedNumbers.get()) {
        gridDisplayZeroBasedNumbers = buttonThatWasClicked->getToggleState();
        updateGrid();
    }
}

void GridWindow::presetChanged(int index, StringArray names) {
    if (gridWindow == nullptr) return;
    if (gridWindow->grid == nullptr) return;
    MessageManager::getInstance()->callAsync([index, names]() {
        gridWindow->presetNames.clear();
        gridWindow->presetNames.addArray(names);
        gridWindow->presetIndex = index;
        gridWindow->sceneGridStartIndex = 0;
        gridWindow->stompGridStartIndex = 0;
        gridWindow->setGridDisplayMode(Mode_presets);
    });
}

void GridWindow::sceneChanged(int index, StringArray names) {
    if (gridWindow == nullptr) return;
    if (gridWindow->grid == nullptr) return;
    MessageManager::getInstance()->callAsync([index, names]() {
        gridWindow->sceneNames.clear();
        gridWindow->sceneNames.addArray(names);
        gridWindow->sceneIndex = index;
        gridWindow->stompGridStartIndex = 0;
        if (gridWindow->gridPresetMode == Mode_scenes) {
            gridWindow->setGridDisplayMode(Mode_scenes);
        } else if (gridWindow->gridPresetMode == Mode_stomps) {
            gridWindow->setGridDisplayMode(Mode_stomps);
        }
        gridWindow->gridTitle->repaint();
    });
}

void GridWindow::stompChanged() {
    MessageManager::getInstance()->callAsync([]() {
        if (gridWindow->gridPresetMode == Mode_stomps) {
            //gridWindow->setGridDisplayMode(Mode_stomps);
            gridWindow->updateGridItems(Mode_stomps);
        }
    });
}

void GridWindow::titleChanged(int index, String name) {
    gridWindow->gridTitle->number = index;
    gridWindow->gridTitle->name = name;
    gridWindow->gridTitle->repaint();
}

void GridWindow::songRackspaceModeChanged() {
    if (gridWindow == nullptr) return;
    gridWindow->gridStartIndex = 0;
}

void GridWindow::resized() {
    int pad = 8;
    int gridHeight = getHeight();
    int gridWidth = getWidth();
    if (grid != nullptr) {
        gridHeight = grid->getHeight() - (pad * 2);
        gridWidth = grid->getWidth() - (pad * 2);
    }
  
    int gridCount = gridItemWidthCount * gridItemHeightCount;
    int gridItemHeight = gridHeight / gridItemHeightCount;
    int gridItemWidth = gridWidth / gridItemWidthCount;
    if (gridItems.size() == 0) return;

    int minIndex = gridStartIndex;
    int maxIndex = jmin(gridStartIndex + gridCount, gridItems.size());
    for (int i = minIndex; i < maxIndex; ++i) { 
        int displayIndex = i - gridStartIndex;
        gridItems[i]->setBounds((displayIndex % gridItemWidthCount) * gridItemWidth + pad, floor(displayIndex / gridItemWidthCount) * gridItemHeight + pad, gridItemWidth, gridItemHeight);
    }
}

void GridWindow::gridBank(bool down) {
    MessageManager::getInstance()->callAsync([down]() {
        int startIndex = gridWindow->gridStartIndex;
        int itemSize = gridWindow->gridItems.size();
        int gridSize = gridWindow->gridItemWidthCount * gridWindow->gridItemHeightCount;
        int bankSize = gridWindow->gridItemWidthCount * gridWindow->gridBankRowCount;

        if (down && startIndex + bankSize >= itemSize) return;
        
        gridWindow->gridStartIndex = down ? jmin(startIndex + bankSize, itemSize - 1)// - (gridWindow->gridBankRowCount * gridWindow->gridItemWidthCount))
                                        : jmax(startIndex - bankSize, 0);
        
        gridWindow->updateGrid();
    });
}

void GridWindow::updateGridItems(modes presetMode) {

    gridWindow->gridItems.clear();
    gridWindow->grid->removeAllChildren();
    StringArray names = presetMode == Mode_presets ? gridWindow->presetNames : gridWindow->sceneNames;
    int index = presetMode == Mode_presets ? gridWindow->presetIndex : gridWindow->sceneIndex;

    if (presetMode == Mode_scenes) {
        //gridWindow->sceneChanged(LibMain::lib->getCurrentSongpartIndex(),LibMain::lib->getSongPartNames(gridWindow->presetIndex));
        gridWindow->sceneIndex = LibMain::lib->inSetlistMode() ? LibMain::lib->getCurrentSongpartIndex() : LibMain::lib->getCurrentVariationIndex();
        gridWindow->sceneNames = LibMain::lib->inSetlistMode() ? LibMain::lib->getSongPartNames(gridWindow->presetIndex) : LibMain::lib->getVariationNames(gridWindow->presetIndex);
        names = gridWindow->sceneNames;
        index = gridWindow->sceneIndex;
    } 
    
    if (presetMode != Mode_stomps) {
        for (int i = 0; i < names.size(); ++i) { 
            GridSelectorItem* gsi = new GridSelectorItem();
            gsi->number = i;
            gsi->name = names[i];
            gsi->selected = i == index;

            gridWindow->gridItems.add(gsi);
            gridWindow->grid->addAndMakeVisible(gsi);
        }
    } else {
        refreshStompList();
        for (int i = 0; i < gridWindow->stomps.size(); ++i) { 
            GridSelectorItem* gsi = new GridSelectorItem();
            gsi->number = i;
            gsi->name = stomps[i].name;
            gsi->selected = stomps[i].isActive;

            gridWindow->gridItems.add(gsi);
            gridWindow->grid->addAndMakeVisible(gsi);
        }
    }
    titleChanged(presetIndex, presetNames[presetIndex]);
    gridWindow->updateGrid();
    gridWindow->resized();
}

void GridWindow::updateGrid() {

    int gridCount = gridItemWidthCount * gridItemHeightCount;

    // Direct select number: Determine the starting index, targeting the middle row.
    //int directSelectRows = ceil(directSelectCount / gridWindow->gridItemWidthCount);
    int startingRow = 0; //floor((gridWindow->gridItemHeightCount - directSelectRows) / 2.f);
    int startingIndex = startingRow * gridWindow->gridItemWidthCount - 1;

    for (int i = 0; i < gridWindow->gridItems.size(); ++i) { 
        if (i < gridStartIndex || i - gridStartIndex >= gridCount) {
            gridItems[i]->directSelectNumber = 0;
            gridItems[i]->setVisible(false);
        } else {
            if (i - gridStartIndex >= startingIndex && i - gridStartIndex < startingIndex + directSelectCount + 1) {
                gridItems[i]->directSelectNumber = i - gridStartIndex - startingIndex;
            } else {
                gridItems[i]->directSelectNumber = 0;
            }
            gridItems[i]->setVisible(true);
            gridItems[i]->repaint();
        }
    }
    resized();
    gridTitle->repaint();
}

void GridWindow::directSelect(String name) {
    MessageManager::getInstance()->callAsync([name]() {
        // Extract direct select number from name
        int directSelectNumber = name.getTrailingIntValue();
        for (int i = gridWindow->gridStartIndex; i < gridWindow->gridItems.size(); ++i) { 
            if (gridWindow->gridItems[i]->directSelectNumber == directSelectNumber) {
                gridWindow->triggerGridItem(gridWindow->gridItems[i]->number);
            }
        }
    });
}

void GridWindow::updateDirectSelectLabel() {
    String label = LibMain::lib->inSetlistMode() ? "Songs" : "Rackspaces";
    if (gridWindow->gridPresetMode == Mode_scenes) {
        label = LibMain::lib->inSetlistMode() ? "Song Parts" : "Variations";
    } else if (gridWindow->gridPresetMode == Mode_stomps) {
        label = "Widgets";
    }
    LibMain::lib->setWidgetCaption("GPGS_DIRECT_SELECT_LABEL","Direct Select - " + label.toStdString());
}
/*
void GridWindow::directSelectWidget(String name) {
    MessageManager::getInstance()->callAsync([name]() {
        // Extract direct select number from name
        int directSelectNumber = name.getTrailingIntValue();
        for (int i = gridWindow->gridStartIndex; i < gridWindow->gridItems.size(); ++i) { 
            if (gridWindow->gridItems[i]->directSelectNumber == directSelectNumber) {
                gridWindow->triggerGridItem(gridWindow->gridItems[i]->number);
            }
        }
    });
}
*/

StringArray GridWindow::getWidgetList(bool isGlobal) {
    StringArray widgetList;
    std::vector<std::string> list;
    LibMain::lib->getWidgetList(list, isGlobal);
    for (int i = 0; i < static_cast<int>(list.size()); ++i) {
        String widget = list[i];
        String widgetName = widget.fromLastOccurrenceOf(":", false, true);
        if (widgetName.startsWith("GPGS_W") || widgetName.startsWith("GPGS_GW")) {
            widgetList.add(widgetName);
        }
        if (!LibMain::lib->listeningForWidget(widgetName.toStdString())) {
            LibMain::lib->listenForWidget(widgetName.toStdString(),true);
        }
    }
    widgetList.sort(true);
    return widgetList;
}

void GridWindow::refreshStompList() {
    
    gridWindow->stomps.clear();

    // Local widgets
    StringArray localWidgets = getWidgetList(false);
    for (int i = 0; i < localWidgets.size(); ++i) {
        widget localWidget;
        std::string handle = localWidgets[i].toStdString();
        localWidget.handle = localWidgets[i];
        localWidget.name = LibMain::lib->getWidgetCaption(handle);
        localWidget.isActive = LibMain::lib->getWidgetValue(handle) == 1.0;
        localWidget.isGlobal = false;
        gridWindow->stomps.add(localWidget);
    }

    // Global widgets
    StringArray globalWidgets = getWidgetList(true);
    for (int i = 0; i < globalWidgets.size(); ++i) {
        widget globalWidget;
        std::string handle = globalWidgets[i].toStdString();
        globalWidget.handle = globalWidgets[i];
        globalWidget.name = LibMain::lib->getWidgetCaption(handle);
        globalWidget.isActive = LibMain::lib->getWidgetValue(handle) == 1.0;
        globalWidget.isGlobal = true;
        gridWindow->stomps.add(globalWidget);
    }
    gridWindow->stompsExist = stomps.size() > 0;
}

void GridWindow::setGridDisplayMode (modes presetMode) {
    gridWindow->refreshStompList();
    if (presetMode == Mode_stomps && !stompsExist) {
        toggleModeDirectionDown = false;
        // If currently in stomp mode and no stomps exist, then switch to scene mode
        if (gridPresetMode == Mode_stomps) {
            presetMode = Mode_scenes;
        } else {
            return;
        }
    }
    // Store current bank/grid item starting index
    if (gridPresetMode == Mode_presets) {
        presetGridStartIndex = gridStartIndex;
        toggleModeDirectionDown = true;
    } else if (gridPresetMode == Mode_scenes) {
        sceneGridStartIndex = gridStartIndex;
    } else {
        stompGridStartIndex = gridStartIndex;
        toggleModeDirectionDown = false;
    }
    // Update starting index based on new mode
    if (presetMode == Mode_presets) {
        gridStartIndex = presetGridStartIndex;
    } else if (presetMode == Mode_scenes) {
        gridStartIndex = sceneGridStartIndex;
    } else if (presetMode == Mode_stomps) {
        if (!stompsExist) return;
        gridStartIndex = stompGridStartIndex;
    } else {
        gridStartIndex = 0;
    }
    backButton->setVisible(presetMode != Mode_presets);
    gridPresetMode = presetMode;
    updateGridItems(presetMode);
    updateDirectSelectLabel();
}

void GridWindow::toggleGridDisplayMode() {
    MessageManager::getInstance()->callAsync([]() {
        if (!gridWindow->isVisible()) {
            LibMain::lib->setWidgetValue("GPGS_DISPLAY", 1.0);        
        } else if (gridWindow->gridPresetMode == Mode_presets) {
            gridWindow->setGridDisplayMode(Mode_scenes);
        } else if (gridWindow->gridPresetMode == Mode_scenes) {
            if (!gridWindow->stompsExist && gridWindow->toggleModeDirectionDown) {
                gridWindow->toggleModeDirectionDown = false;
            }
            gridWindow->setGridDisplayMode(gridWindow->toggleModeDirectionDown ? Mode_stomps : Mode_presets);
        } else if (gridWindow->gridPresetMode == Mode_stomps){
            gridWindow->setGridDisplayMode(Mode_scenes);
        }
    });
}

void GridWindow::triggerGridItem (int number) {
    // Preset Mode
    if (gridWindow->gridPresetMode == Mode_presets) {
        // If triggering the already active item, drill down to next view
        if (gridWindow->presetIndex == number) {
            gridWindow->setGridDisplayMode(Mode_scenes);
        } else {
            // Select song or rackspacce
            if (LibMain::lib->inSetlistMode()) {
                    (void)LibMain::lib->switchToSong(number, 0);
            } else {
                (void)LibMain::lib->switchToRackspace(number, 0);
            }
            // Perform the 'On Selection' action
            int onSelectionAction = gridWindow->prefOnSelectionPresetMenu->selectedItem;
            if (onSelectionAction == Preset_displayPartsVariations) {
                juce::Timer::callAfterDelay (GRID_CLOSE_DELAY_MS,[]() {
                    gridWindow->setGridDisplayMode(Mode_scenes);
                });
            } else if (onSelectionAction == Preset_displayWidgets) {
                juce::Timer::callAfterDelay (GRID_CLOSE_DELAY_MS,[]() {
                    gridWindow->setGridDisplayMode(Mode_stomps);
                });
            } else if (onSelectionAction == Preset_closeSelector) {
                juce::Timer::callAfterDelay (GRID_CLOSE_DELAY_MS,[]() {
                        hideGrid();
                        LibMain::lib->setWidgetValue("GPGS_DISPLAY", 0.0);
                });
            }
        }
    } 
    // Scene Mode
    else if (gridWindow->gridPresetMode == Mode_scenes) {
        // If triggering the already active item, drill down to next view
        if (gridWindow->sceneIndex == number) {
            gridWindow->setGridDisplayMode(Mode_stomps);
        } else {
            // Select song part or variation
            if (LibMain::lib->inSetlistMode()) {
                (void)LibMain::lib->switchToSongPart(number);
            } else {
                (void)LibMain::lib->switchToVariation(number);
            }
            // Perform the 'On Selection' action
            int onSelectionAction = gridWindow->prefOnSelectionSceneMenu->selectedItem;
            if (onSelectionAction == Scene_displayWidgets) {
                juce::Timer::callAfterDelay (GRID_CLOSE_DELAY_MS,[]() {
                    gridWindow->setGridDisplayMode(Mode_stomps);
                });
            } else if (onSelectionAction == Scene_closeSelector) {
                juce::Timer::callAfterDelay (GRID_CLOSE_DELAY_MS,[]() {
                        hideGrid();
                        LibMain::lib->setWidgetValue("GPGS_DISPLAY", 0.0);
                });
            }
        }
    } 
    // Stomp Mode
    else if (gridWindow->gridPresetMode == Mode_stomps) {
        int newValue = gridWindow->stomps[number].isActive ? 0.0 : 1.0;
        LibMain::lib->setWidgetValue(gridWindow->stomps[number].handle.toStdString(), newValue);
    }
}

void GridSelectorMain::paint (Graphics& g)
{
    //auto area = getLocalBounds().toFloat().reduced (0.8f);
    //float cornerSize = 8.f;
    //g.setColour (Colour(0xdd151515));
    //g.setColour (Colour(0xff000000));
    //g.fillRoundedRectangle (area, cornerSize);
    g.fillAll(Colour(0xff151515));
    //auto fontSize = (float) Grid_Font_Size;
    //g.setFont (fontSize);
    //g.setColour (Colours::white);
    /*
    g.drawFittedText (Grid_Text,
                        getLocalBounds().reduced (8, 0),
                        Justification::centred, 5);
    */
}

void GridSelectorItem::paint (Graphics& g)
{
    auto area = getLocalBounds().toFloat().reduced (8.f);
    float cornerSize = 8.f;
    
    // Background
    Colour backgroundColour;
    if (selected) {
        //g.setColour (Colour(0xff101010));
        if (GridWindow::gridWindow->gridPresetMode == Mode_presets) {
            backgroundColour = LibMain::lib->inSetlistMode() ? 
                    GridWindow::gridWindow->prefSongColour
                    : GridWindow::gridWindow->prefRackspaceColour;
        } else if (GridWindow::gridWindow->gridPresetMode == Mode_scenes) {
            backgroundColour = LibMain::lib->inSetlistMode() ? 
                    GridWindow::gridWindow->prefSongpartColour
                    : GridWindow::gridWindow->prefVariationColour;
        } else if (GridWindow::gridWindow->gridPresetMode == Mode_stomps) {
            String customColour = GridWindow::gridWindow->stomps[number].handle.fromLastOccurrenceOf("_", false, true);
            if (customColour.length() == 8) {
                backgroundColour = Colour::fromString(customColour);
            } else {
                backgroundColour = GridWindow::gridWindow->prefWidgetColour;
            }
        }
    } else if (hover) {
        backgroundColour = Colour(0xff303030);
    } else {
        backgroundColour = Colour(0xff252525);
    }
    g.setColour(backgroundColour);
    g.fillRoundedRectangle (area, cornerSize);
    
    // Border
    if (selected) {
        g.setColour(backgroundColour.withMultipliedLightness(0.6f));
        g.drawRoundedRectangle(area, cornerSize, 2.f);
    } else {
        g.setColour (Colour(0xff404040));
        g.drawRoundedRectangle(area, cornerSize, 1.f);
    }
    
    // Name
    auto fontSize = 60;
    Font font (fontSize);
    auto fontHeight = font.getHeight();
    auto ratio = area.getHeight() / (fontHeight * 4.75f);
    font = font.withHeight(fontHeight * ratio);
    auto stringWidth = (float)font.getStringWidth("XXXXXXXXXX");
    if (stringWidth > area.getWidth()) {
        auto newRatio = (float)getWidth() / stringWidth;
        font = font.withHeight(font.getHeight() * newRatio);
        fontHeight = font.getHeight();
    }
    int displayLines = ratio > 1.5f ? 3 : 2;
    g.setFont (font);
    // Determine font colour
    float luminance = (0.299f * backgroundColour.getFloatRed()) + 
                  (0.587f * backgroundColour.getFloatGreen()) + 
                  (0.114f * backgroundColour.getFloatBlue());

    // If luminance > 0.5, the background is light; use black text. Otherwise, use white.
    auto fontColour = luminance > 0.5f ? juce::Colours::black : juce::Colours::white;

    g.setColour (fontColour);
    g.drawFittedText (name,
                        getLocalBounds().reduced (20, 0),
                        Justification::centred, displayLines, 1.f);
    // Number
    if (selected) {
        g.setColour(backgroundColour.withMultipliedLightness(1.5f));
    } else {
        g.setColour(Colour(0xff606060));
    }
    
    int displayNumber = GridWindow::gridWindow->gridDisplayZeroBasedNumbers ? number : number + 1;
    bool displayGlobal = GridWindow::gridWindow->stomps[number].isGlobal;
    //String displayGlobal = GridWindow::gridWindow->stomps[number].isGlobal ? "G" : "";
    //String label = GridWindow::gridWindow->gridPresetMode == Mode_stomps ? displayGlobal : (String)displayNumber;
    if (GridWindow::gridWindow->gridPresetMode == Mode_stomps && displayGlobal) {
        GridWindow::gridWindow->globeIcon.scaleToFit(25.f, 20.f, font.getHeight() * 0.8f, font.getHeight() * 0.8f, true);
        g.fillPath(GridWindow::gridWindow->globeIcon);
    } else if (GridWindow::gridWindow->gridPresetMode != Mode_stomps) {
        g.drawFittedText ((String(displayNumber)),
                            getLocalBounds().reduced (25, 15),
                            Justification::topLeft, 1, 1.f);
        
    }

    // Direct Select Number
    fontHeight = font.getHeight();
    if (directSelectNumber >= 1 ) {
        String controllerNumberText = String(directSelectNumber);
        auto stringWidth = font.getStringWidth(controllerNumberText);
        
        // Background
        g.setColour (Colour(0xff000000));
        Rectangle<float> newArea {area.getWidth() - 8.f - stringWidth - (fontHeight * 0.4f), 8.f + (fontHeight * 0.25f), stringWidth + (fontHeight * 0.4f) , fontHeight};
        g.fillRoundedRectangle(newArea, cornerSize);
        
        //Border
        g.setColour(backgroundColour.withMultipliedLightness(1.5f));
        g.drawRoundedRectangle(newArea, cornerSize, 1.f);
        
        // Number
        g.setColour (Colours::white);
        g.drawFittedText (controllerNumberText,
                    newArea.toNearestInt(),
                    Justification::centred, 1, 1.f);
    }
}

void GridSelectorItem::mouseDown(const MouseEvent&) {
    GridWindow::gridWindow->triggerGridItem(number);
    /*
    if (GridWindow::gridWindow->gridPresetMode) {
        int onSelectionAction = GridWindow::gridWindow->prefOnSelectionPresetMenu->selectedItem;
        if (onSelectionAction == 1) {
            if (GridWindow::gridWindow->presetIndex == number) {
                GridWindow::gridWindow->setGridDisplayMode(false);
            }  else {
                if (LibMain::lib->inSetlistMode()) {
                    (void)LibMain::lib->switchToSong(number, 0);
                } else {
                    (void)LibMain::lib->switchToRackspace(number, 0);
                }
                juce::Timer::callAfterDelay (500,[]() {
                    GridWindow::gridWindow->setGridDisplayMode(false);
                });
            }
        } else if (onSelectionAction >= 2) {
            if (LibMain::lib->inSetlistMode()) {
                (void)LibMain::lib->switchToSong(number, 0);
            } else {
                (void)LibMain::lib->switchToRackspace(number, 0);
            }
            if (onSelectionAction == 2) {
                juce::Timer::callAfterDelay (500,[]() {
                        GridWindow::hideGrid();
                        LibMain::lib->setWidgetValue("GPGS_DISPLAY", 0.0);
                });
            }
        }
    } else {
        int onSelectionAction = GridWindow::gridWindow->prefOnSelectionSceneMenu->selectedItem;
        if (LibMain::lib->inSetlistMode()) {
            (void)LibMain::lib->switchToSongPart(number);
        } else {
            (void)LibMain::lib->switchToVariation(number);
        }
        if (onSelectionAction == 1) {
            juce::Timer::callAfterDelay (500,[]() {
                    GridWindow::hideGrid();
                    LibMain::lib->setWidgetValue("GPGS_DISPLAY", 0.0);
            });
        }
    }
    */
   /*
    if (LibMain::lib->inSetlistMode()) {
        if (GridWindow::gridWindow->gridPresetMode) {

            if (GridWindow::gridWindow->presetIndex == number) {
                GridWindow::gridWindow->setGridDisplayMode(false);
            } else {
                (void)LibMain::lib->switchToSong(number, 0);
                juce::Timer::callAfterDelay (500,[]() {
                    GridWindow::gridWindow->setGridDisplayMode(false);
                });
            }
        } else {
            (void)LibMain::lib->switchToSongPart(number);
        }
    } else {
        if (GridWindow::gridWindow->gridPresetMode) {
            if (GridWindow::gridWindow->presetIndex == number) {
                GridWindow::gridWindow->setGridDisplayMode(false);
            } else {
                (void)LibMain::lib->switchToRackspace(number, 0);
                juce::Timer::callAfterDelay (500,[]() {
                    GridWindow::gridWindow->setGridDisplayMode(false);
                });
            }
        } else {
            (void)LibMain::lib->switchToVariation(number);
        }
    }
    if (GridWindow::gridWindow->gridCloseOnItemSelect) {
        GridWindow::setGridDuration(GRID_CLOSE_DELAY_MS);
    }
    */
}

void GridSelectorItem::mouseEnter(const MouseEvent&) {
    hover = true;
    repaint();
}

void GridSelectorItem::mouseExit(const MouseEvent&) {
    hover = false;
    repaint();
}

void GridSelectorBankUp::mouseDown(const MouseEvent&) {
    //int newIndex = juce::jmax(LibMain::lib->getCurrentSongIndex() - 1, 0);
    //LibMain::lib->switchToSong(newIndex, 0);
    GridWindow::gridWindow->gridBank(false);
}

void GridSelectorBankDown::mouseDown(const MouseEvent&) {
    //int newIndex = juce::jmin(LibMain::lib->getCurrentSongIndex() + 1, LibMain::lib->getSongCount() - 1);
    //LibMain::lib->switchToSong(newIndex, 0);
    GridWindow::gridWindow->gridBank(true);
}

void GridSelectorTitle::mouseDown(const MouseEvent &) {
    if (GridWindow::gridWindow->gridMenu->isVisible()) {
        GridWindow::gridWindow->gridMenu->setVisible(false);
        GridWindow::gridWindow->preferencesButton->setToggleState(false, juce::NotificationType::dontSendNotification);
    }
}

void GridPopupMenuPreset::mouseDown(const MouseEvent&) {
    juce::PopupMenu menu;
    menu.setLookAndFeel(popupLNF);
    menu.addItem (1, ON_SELECTION_MENU_PRESETS[0]);
    menu.addItem (4, ON_SELECTION_MENU_PRESETS[3]);
    menu.addItem (2, ON_SELECTION_MENU_PRESETS[1]);
    menu.addItem (3, ON_SELECTION_MENU_PRESETS[2]);

    juce::PopupMenu::Options opts = juce::PopupMenu::Options()
        .withTargetComponent(this)
        .withPreferredPopupDirection(juce::PopupMenu::Options::PopupDirection::downwards)
        .withStandardItemHeight(40);

    menu.showMenuAsync(opts,[=] (int result)
    {
        // Menu selection
        switch(result) {
            case 1:
                selectedItemText = ON_SELECTION_MENU_PRESETS[0];
                selectedItem = Preset_displayPartsVariations;
                break;
            case 2:
                selectedItemText = ON_SELECTION_MENU_PRESETS[1];
                selectedItem = Preset_closeSelector;
                break;
            case 3:
                selectedItemText = ON_SELECTION_MENU_PRESETS[2];
                selectedItem = Preset_noAction;
                break;
            case 4:
                selectedItemText = ON_SELECTION_MENU_PRESETS[3];
                selectedItem = Preset_displayWidgets;
                break;
            default:
                break;
        }
        repaint();
    });
}

void GridPopupMenuScene::mouseDown(const MouseEvent&) {
    juce::PopupMenu menu;
    menu.setLookAndFeel(popupLNF);
    menu.addItem (3, ON_SELECTION_MENU_SCENES[2]);
    menu.addItem (1, ON_SELECTION_MENU_SCENES[0]);
    menu.addItem (2, ON_SELECTION_MENU_SCENES[1]);

    juce::PopupMenu::Options opts = juce::PopupMenu::Options()
        .withTargetComponent(this)
        .withPreferredPopupDirection(juce::PopupMenu::Options::PopupDirection::downwards)
        .withStandardItemHeight(40);

    menu.showMenuAsync(opts,[=] (int result)
    {
        // Menu selection
        switch(result) {
            case 1:
                selectedItemText = ON_SELECTION_MENU_SCENES[0];
                selectedItem = Scene_closeSelector;
                break;
            case 2:
                selectedItemText = ON_SELECTION_MENU_SCENES[1];
                selectedItem = Scene_noAction;
                break;
            case 3:
                selectedItemText = ON_SELECTION_MENU_SCENES[2];
                selectedItem = Scene_displayWidgets;
                break;
            default:
                break;
        }
        repaint();
    });
    
}

void GridSelectorBankUp::mouseEnter(const MouseEvent&) {
    hover = true;
    repaint();
}

void GridSelectorBankUp::mouseExit(const MouseEvent&) {
    hover = false;
    repaint();
}

void GridSelectorBankDown::mouseEnter(const MouseEvent&) {
    hover = true;
    repaint();
}

void GridSelectorBankDown::mouseExit(const MouseEvent&) {
    hover = false;
    repaint();
}

void GridPopupMenu::mouseEnter(const MouseEvent&) {
    hover = true;
    repaint();
}

void GridPopupMenu::mouseExit(const MouseEvent&) {
    hover = false;
    repaint();
}

void GridSelectorTitle::paint (Graphics& g)
{
    auto area = getLocalBounds().withRight(getWidth() - 140); // Reduction for icons
    auto fontSize = (float) Grid_Font_Size;

    // Background
    g.setColour (Colour(0xff000000));
    g.fillAll();
    
    // Number
    g.setColour (Colour(0xff808080));
    g.setFont (fontSize);
    int displayNumber = GridWindow::gridWindow->gridDisplayZeroBasedNumbers ? number : number + 1;
    g.drawFittedText (String(displayNumber),
                        getLocalBounds().reduced (30, 0),
                        Justification::left, 1, 1.f);
    // Name
    g.setColour (Colours::white);
    String title = name;
    if (GridWindow::gridWindow->gridDisplaySceneNameInTitle) {
        String sceneName = LibMain::lib->inSetlistMode() ? LibMain::lib->getSongpartName(LibMain::lib->getCurrentSongIndex(), LibMain::lib->getCurrentSongpartIndex())
                                                : LibMain::lib->getVariationName(LibMain::lib->getCurrentRackspaceIndex(), LibMain::lib->getCurrentVariationIndex());
        title = title + " — " + sceneName;
    }
    g.drawFittedText (title,
                        area.reduced (50 + g.getCurrentFont().getStringWidth(String(displayNumber)), 0),
                        Justification::left, 1, 1.f);
    
    // Line highlight
    if (GridWindow::gridWindow->gridPresetMode == Mode_presets) {
        g.setColour (LibMain::lib->inSetlistMode() ? GridWindow::gridWindow->prefSongColour : GridWindow::gridWindow->prefRackspaceColour);
    } else if (GridWindow::gridWindow->gridPresetMode == Mode_scenes) {
        g.setColour (LibMain::lib->inSetlistMode() ? GridWindow::gridWindow->prefSongpartColour : GridWindow::gridWindow->prefVariationColour);
    } else if (GridWindow::gridWindow->gridPresetMode == Mode_stomps) {
         g.setColour (GridWindow::gridWindow->prefWidgetColour);
    }
    g.fillRect(0, getHeight() - 2, getWidth(), 2);
}

void GridSelectorBankUp::paint (Graphics& g)
{
    auto area = getLocalBounds().toFloat().reduced(16.f);
    
    // Background
    g.fillAll(Colour(0xff000000));
    
    // Button background
    float cornerSize = 8.f;
    if (hover) {
        g.setColour (Colour(0xff303030));
    } else {
        g.setColour (Colour(0xff252525));
    }
    g.fillRoundedRectangle (area, cornerSize);

    // Button border
    g.setColour (Colour(0xff505050));
    g.drawRoundedRectangle(area, cornerSize, 1.f);

    // Arrow
    float arrowHeight = 40.0f;
    juce::Path arrow;
    arrow.addTriangle ((float)getWidth() / 2.0f - (arrowHeight / 2.0f), (float)getHeight() / 2.0f + (arrowHeight / 2.0f), // Bottom-left point
                          (float)getWidth() / 2.0f + (arrowHeight / 2.0f), (float)getHeight() / 2.0f + (arrowHeight / 2.0f), // Bottom-right point
                          (float)getWidth() / 2.0f, (float)getHeight() / 2.0f - (arrowHeight / 2.0f)); // Top-center point
    g.setColour (Colour(0xffffffff));
    g.fillPath (arrow);
}

void GridSelectorBankDown::paint (Graphics& g)
{
    auto area = getLocalBounds().toFloat().reduced(16.f);
    
    // Background
    g.fillAll(Colour(0xff000000));

    // Button background
    float cornerSize = 8.f;
    if (hover) {
        g.setColour (Colour(0xff303030));
    } else {
        g.setColour (Colour(0xff252525));
    }
    g.fillRoundedRectangle (area, cornerSize);

    // Button border
    g.setColour (Colour(0xff505050));
    g.drawRoundedRectangle(area, cornerSize, 1.f);

    // Arrow
    float arrowHeight = 40.0f;
    juce::Path arrow;
    arrow.addTriangle ((float)getWidth() / 2.0f - (arrowHeight / 2.0f), (float)getHeight() / 2.0f - (arrowHeight / 2.0f), // Top-left point
                          (float)getWidth() / 2.0f + (arrowHeight / 2.0f), (float)getHeight() / 2.0f - (arrowHeight / 2.0f), // Top-right point
                          (float)getWidth() / 2.0f, (float)getHeight() / 2.0f + (arrowHeight / 2.0f)); // Bottom-center point
    g.setColour (Colour(0xffffffff));
    g.fillPath (arrow); 
}

void GridMenu::paint (Graphics& g)
{
    auto area = getLocalBounds().toFloat();

    // Background
    float cornerSize = 8.f;
    g.setColour (Colour(0xf8000000));
    g.fillRoundedRectangle (area, cornerSize);

    // Border
    g.setColour (Colour(0xff808080));
    g.drawRoundedRectangle(area, cornerSize, 1.f);

    // Text
    g.setFont (50);
    g.setColour (Colours::white);
    g.drawFittedText ("Preferences",
                        getLocalBounds().withLeft(30).withTop(20),
                        Justification::topLeft, 1, 1.f);
    
    g.setColour (Colour(0xff404040));                    
    g.fillRect(30, 90, 340, 1);

    g.setFont (40);
    g.setColour (Colour(0xffe0e0e0));
    g.drawFittedText ("Grid Size",
                        getLocalBounds().withLeft(30).withTop(110),
                        Justification::topLeft, 1, 1.f);
    g.setFont (35);
    g.setColour (Colour(0xffd0d0d0));                    
    g.drawFittedText ("Columns: " + String(GridWindow::gridWindow->gridItemWidthCount),
                        getLocalBounds().withLeft(30).withTop(175),
                        Justification::topLeft, 1, 1.f);
   
    g.drawFittedText ("Rows: " + String(GridWindow::gridWindow->gridItemHeightCount),
                        getLocalBounds().withLeft(30).withTop(235),
                        Justification::topLeft, 1, 1.f);
    
    g.setColour (Colour(0xff404040));                    
    g.fillRect(30, 300, 340, 1);

    g.setFont (40);
    g.setColour (Colour(0xffe0e0e0));
    g.drawFittedText ("Bank Size",
                        getLocalBounds().withLeft(30).withTop(320),
                        Justification::topLeft, 1, 1.f);
    g.setFont (35);    
    g.setColour (Colour(0xffd0d0d0));                                
    g.drawFittedText ("Rows: " + String(GridWindow::gridWindow->gridBankRowCount),
                        getLocalBounds().withLeft(30).withTop(385),
                        Justification::topLeft, 1, 1.f);
    
    g.setColour (Colour(0xff404040));                    
    g.fillRect(30, 450, 340, 1);

    g.setFont (40);
    g.setColour (Colour(0xffe0e0e0));
    g.drawFittedText ("Direct Select",
                        getLocalBounds().withLeft(30).withTop(470),
                        Justification::topLeft, 1, 1.f);
    g.setFont (35);    
    g.setColour (Colour(0xffd0d0d0));                                
    g.drawFittedText ("Count: " + String(GridWindow::gridWindow->directSelectCount),
                        getLocalBounds().withLeft(30).withTop(535),
                        Justification::topLeft, 1, 1.f);
    /*
    g.drawFittedText ("Latching Switches",
                        getLocalBounds().withLeft(100).withTop(645),
                        Justification::topLeft, 1, 1.f);
    */
    //g.setColour (Colour(0xff404040));                    
    //g.fillRect(30, 600, 340, 1);

    // Column two

    g.setColour (Colour(0xff404040));                    
    g.fillRect(430, 90, 340, 1);

    g.setFont (40);
    g.setColour (Colour(0xffe0e0e0));
    g.drawFittedText ("On Selection",
                        getLocalBounds().withLeft(430).withTop(110),
                        Justification::topLeft, 1, 1.f);
    g.setFont (35);
    g.setColour (Colour(0xffd0d0d0));                    
    g.drawFittedText ("Songs/Rackspaces: ",
                        getLocalBounds().withLeft(430).withTop(175),
                        Justification::topLeft, 1, 1.f);
    g.drawFittedText ("Parts/Variations: ",
                        getLocalBounds().withLeft(430).withTop(295),
                        Justification::topLeft, 1, 1.f);
    
    g.setColour (Colour(0xff404040));                    
    g.fillRect(430, 420, 340, 1);

    g.setFont (40);
    g.setColour (Colour(0xffe0e0e0));
    g.drawFittedText ("Other",
                        getLocalBounds().withLeft(430).withTop(440),
                        Justification::topLeft, 1, 1.f);
    g.setFont (30);
    g.setColour (Colour(0xffd0d0d0)); 
    g.drawFittedText ("Zero-Based Numbers",
                    getLocalBounds().withLeft(500).withTop(503),
                    Justification::topLeft, 1, 1.f);
    g.drawFittedText ("Display Part/Variation Name in Header",
                    getLocalBounds().withLeft(500).withTop(558),
                    Justification::topLeft, 2, 1.f);
}

void GridPopupMenu::paint (Graphics& g)
{
    auto area = getLocalBounds().toFloat();

    // Background
    float cornerSize = 4.f;
    if (hover) {
        g.setColour (Colour(0xff303030));
    } else {
        g.setColour (Colour(0xff202020));;
    }
    
    g.fillRoundedRectangle (area, cornerSize);

    // Text
    g.setFont (25);
    g.setColour (Colour(0xffe0e0e0));
    g.drawFittedText (selectedItemText,
                        getLocalBounds().withLeft(10),
                        Justification::centredLeft, 1, 1.f);
    
    // Arrow
    float arrowWidth = 12.0f;
    float arrowHeight = 8.0f;
    float arrowRightPad = 10.0f;
    float arrowInnerPad = 3.0f;
    juce::Path arrowUp;
    arrowUp.addTriangle ((float)getWidth() - arrowWidth - arrowRightPad, (float)getHeight() / 2.0f - arrowInnerPad, // Bottom-left point
                         (float)getWidth() - arrowRightPad, (float)getHeight() / 2.0f - arrowInnerPad, // Bottom-right point
                          (float)getWidth() - (arrowWidth / 2.0f) - arrowRightPad, (float)getHeight() / 2.0f - arrowHeight - arrowInnerPad); // Top-center point
    
    juce::Path arrowDown;
    arrowDown.addTriangle ((float)getWidth() - arrowWidth - arrowRightPad, (float)getHeight() / 2.0f + arrowInnerPad, // Top-left point
                         (float)getWidth() - arrowRightPad, (float)getHeight() / 2.0f + arrowInnerPad, // Top-right point
                          (float)getWidth() - (arrowWidth / 2.0f) - arrowRightPad, (float)getHeight() / 2.0f + arrowHeight + arrowInnerPad); // Bottom-center point
    
    g.setColour (Colour(0xff909090));
    g.fillPath (arrowUp);
    g.fillPath(arrowDown);
}

ValueTree GridWindow::setPreferenceDefaults() {
    ValueTree defaultPreferences = ValueTree{"ExtensionData", {}};
    defaultPreferences.setProperty("name", (String)PROJECT_NAME, nullptr);
    defaultPreferences.setProperty("version", (String)PROJECT_VERSION, nullptr);

    ValueTree gridPreferences = ValueTree{"Grid", {}};
    gridPreferences.setProperty("columns", GRID_COLUMNS_DEFAULT, nullptr);
    gridPreferences.setProperty("rows", GRID_ROWS_DEFAULT, nullptr);
    gridPreferences.setProperty("bankRows", GRID_BANK_ROWS_DEFAULT, nullptr);
    gridPreferences.setProperty("onSelectPreset", Preset_displayPartsVariations, nullptr);
    gridPreferences.setProperty("onSelectScene", Scene_noAction, nullptr);

    ValueTree directSelectPreferences = ValueTree{"DirectSelect", {}};
    directSelectPreferences.setProperty("count", GRID_DIRECT_SELECT_DEFAULT, nullptr);
    directSelectPreferences.setProperty("activeOnClose", GRID_DIRECT_SELECT_ACTIVE_ON_CLOSE, nullptr);

    ValueTree displayPreferences = ValueTree{"Display", {}};
    displayPreferences.setProperty("zeroBasedNumbers", DISPLAY_ZERO_BASED_NUMBERS_DEFAULT, nullptr);
    displayPreferences.setProperty("sceneNameInHeader", DISPLAY_SCENE_NAME_IN_HEADER_DEFAULT, nullptr);

    defaultPreferences.addChild(gridPreferences, -1, nullptr);
    defaultPreferences.addChild(directSelectPreferences, -1, nullptr);
    defaultPreferences.addChild(displayPreferences, -1, nullptr);

    return defaultPreferences;
}

void GridWindow::readPreferences() {
    readDataFile();

    /*
    // Parse preferences file and populate the default values of any new sections or properties
    if (!preferences.isValid()){
        ValueTree extensionData = ValueTree{"ExtensionData", {}};
        extensionData.setProperty("name", (String)PROJECT_NAME, nullptr);
        extensionData.setProperty("version", (String)PROJECT_VERSION, nullptr); 
        preferences = extensionData;
    }
    // Grid section
    ValueTree gridSection = preferences.getChildWithName("Grid");
    if (!gridSection.isValid()) {
        preferences.addChild(ValueTree{"Grid", {}}, -1, nullptr);
    }
    */

    // Read preferences and populate missing properties with default values
    //gridItemWidthCount = preferences.getChildWithName("Grid").getProperty("columns");
    gridItemWidthCount = preferences.getChildWithName("Grid").hasProperty("columns") ? 
                            preferences.getChildWithName("Grid").getProperty("columns").toString().getIntValue()
                            : GRID_COLUMNS_DEFAULT;
    
    //gridItemHeightCount = preferences.getChildWithName("Grid").getProperty("rows");
    gridItemHeightCount = preferences.getChildWithName("Grid").hasProperty("rows") ? 
                            preferences.getChildWithName("Grid").getProperty("rows").toString().getIntValue()
                            : GRID_ROWS_DEFAULT;

    //gridBankRowCount = preferences.getChildWithName("Grid").getProperty("bankRows");
    gridBankRowCount = preferences.getChildWithName("Grid").hasProperty("bankRows") ?
                            preferences.getChildWithName("Grid").getProperty("bankRows").toString().getIntValue()
                            : GRID_BANK_ROWS_DEFAULT;
    
    //prefOnSelectionPresetMenu->selectedItem = preferences.getChildWithName("Grid").getProperty("onSelectPreset")
    prefOnSelectionPresetMenu->selectedItem = preferences.getChildWithName("Grid").hasProperty("onSelectPreset") ?
                            preferences.getChildWithName("Grid").getProperty("onSelectPreset").toString().getIntValue()
                            : Preset_displayPartsVariations;

    //prefOnSelectionSceneMenu->selectedItem = preferences.getChildWithName("Grid").getProperty("onSelectScene");
    prefOnSelectionSceneMenu->selectedItem = preferences.getChildWithName("Grid").hasProperty("onSelectScene") ?
                            preferences.getChildWithName("Grid").getProperty("onSelectScene").toString().getIntValue()
                            : Scene_noAction;

    // Direct Select section
    //directSelectCount = preferences.getChildWithName("DirectSelect").getProperty("count");
    directSelectCount = preferences.getChildWithName("DirectSelect").hasProperty("count") ?
                            preferences.getChildWithName("DirectSelect").getProperty("count").toString().getIntValue()
                            : GRID_DIRECT_SELECT_DEFAULT;
    
    // Display section
    //gridDisplayZeroBasedNumbers = preferences.getChildWithName("Display").getProperty("zeroBasedNumbers");
    gridDisplayZeroBasedNumbers = preferences.getChildWithName("Display").hasProperty("zeroBasedNumbers") ?
                            preferences.getChildWithName("Display").getProperty("zeroBasedNumbers").toString().getIntValue()
                            : DISPLAY_ZERO_BASED_NUMBERS_DEFAULT;

    //gridDisplaySceneNameInTitle = preferences.getChildWithName("Display").getProperty("sceneNameInHeader");
    gridDisplaySceneNameInTitle = preferences.getChildWithName("Display").hasProperty("sceneNameInHeader") ?
                            preferences.getChildWithName("Display").getProperty("sceneNameInHeader").toString().getIntValue()
                            : DISPLAY_SCENE_NAME_IN_HEADER_DEFAULT;

    prefSongColour = preferences.getChildWithName("Display").hasProperty("songColor") ?
                            Colour::fromString(preferences.getChildWithName("Display").getProperty("songColor").toString())
                            : Colour::fromString(SONG_COLOUR);
    
    prefSongpartColour = preferences.getChildWithName("Display").hasProperty("songpartColor") ?
                            Colour::fromString(preferences.getChildWithName("Display").getProperty("songpartColor").toString())
                            : Colour::fromString(SONGPART_COLOUR);
      
    prefRackspaceColour = preferences.getChildWithName("Display").hasProperty("rackspaceColor") ?
                            Colour::fromString(preferences.getChildWithName("Display").getProperty("rackspaceColor").toString())
                            : Colour::fromString(RACKSPACE_COLOUR);
    
    prefVariationColour = preferences.getChildWithName("Display").hasProperty("variationColor") ?
                            Colour::fromString(preferences.getChildWithName("Display").getProperty("variationColor").toString())
                            : Colour::fromString(VARIATION_COLOUR);
    
    prefWidgetColour = preferences.getChildWithName("Display").hasProperty("widgetColor") ?
                            Colour::fromString(preferences.getChildWithName("Display").getProperty("widgetColor").toString())
                            : Colour::fromString(WIDGET_COLOUR);

 
    // Populate preference text labels
    switch(prefOnSelectionPresetMenu->selectedItem) {
        case Preset_displayPartsVariations:
            prefOnSelectionPresetMenu->selectedItemText = ON_SELECTION_MENU_PRESETS[0];
            break;
        case Preset_closeSelector:
            prefOnSelectionPresetMenu->selectedItemText = ON_SELECTION_MENU_PRESETS[1];
            break;
        case Preset_noAction:
            prefOnSelectionPresetMenu->selectedItemText = ON_SELECTION_MENU_PRESETS[2];
            break;
        case Preset_displayWidgets:
            prefOnSelectionPresetMenu->selectedItemText = ON_SELECTION_MENU_PRESETS[3];
            break;
        default:
            break;
    }

    switch(prefOnSelectionSceneMenu->selectedItem) {
        case Scene_closeSelector:
            prefOnSelectionSceneMenu->selectedItemText = ON_SELECTION_MENU_SCENES[0];
            break;
        case Scene_noAction:
            prefOnSelectionSceneMenu->selectedItemText = ON_SELECTION_MENU_SCENES[1];
            break;
        case Scene_displayWidgets:
            prefOnSelectionSceneMenu->selectedItemText = ON_SELECTION_MENU_SCENES[2];
            break;
        default:
            break;
    }   

    // Set preference toggle buttons
    prefToggleDisplayZeroBasedNumbers->setToggleState(gridDisplayZeroBasedNumbers, dontSendNotification);
    prefToggleDisplaySceneNameInTitle->setToggleState(gridDisplaySceneNameInTitle, dontSendNotification);
}

void GridWindow::savePreferences() {
    // Build structure
    ValueTree header = ValueTree{"ExtensionData", {}};
    header.setProperty("name", (String)PROJECT_NAME, nullptr);
    header.setProperty("version", (String)PROJECT_VERSION, nullptr);
    // Grid
    ValueTree gridSection = ValueTree{"Grid", {}};
    gridSection.setProperty("columns", gridItemWidthCount, nullptr);
    gridSection.setProperty("rows", gridItemHeightCount, nullptr);
    gridSection.setProperty("bankRows", gridBankRowCount, nullptr);
    gridSection.setProperty("onSelectPreset", prefOnSelectionPresetMenu->selectedItem, nullptr);
    gridSection.setProperty("onSelectScene", prefOnSelectionSceneMenu->selectedItem, nullptr);
    // Direct Select
    ValueTree directSelectSection = ValueTree{"DirectSelect", {}};
    directSelectSection.setProperty("count", directSelectCount, nullptr);
    // Display
    ValueTree displaySection = ValueTree{"Display", {}};
    displaySection.setProperty("zeroBasedNumbers", gridDisplayZeroBasedNumbers, nullptr);
    displaySection.setProperty("sceneNameInHeader", gridDisplaySceneNameInTitle, nullptr);
    displaySection.setProperty("songColor", prefSongColour.toString(), nullptr);
    displaySection.setProperty("songpartColor", prefSongpartColour.toString(), nullptr);
    displaySection.setProperty("rackspaceColor", prefRackspaceColour.toString(), nullptr);
    displaySection.setProperty("variationColor", prefVariationColour.toString(), nullptr);
    displaySection.setProperty("widgetColor", prefWidgetColour.toString(), nullptr);

    // Merge
    header.addChild(gridSection, -1, nullptr);
    header.addChild(directSelectSection, -1, nullptr);
    header.addChild(displaySection, -1, nullptr);
    gridWindow->preferences = header;

    /*
    preferences.getChildWithName("Grid").setProperty("columns", gridItemWidthCount, nullptr);
    preferences.getChildWithName("Grid").setProperty("rows", gridItemHeightCount, nullptr);
    preferences.getChildWithName("Grid").setProperty("bankRows", gridBankRowCount, nullptr);
    preferences.getChildWithName("DirectSelect").setProperty("count", directSelectCount, nullptr);
    preferences.getChildWithName("Display").setProperty("zeroBasedNumbers", gridDisplayZeroBasedNumbers, nullptr);
    preferences.getChildWithName("Display").setProperty("sceneNameInHeader", gridDisplaySceneNameInTitle, nullptr);
    preferences.getChildWithName("Grid").setProperty("onSelectPreset", prefOnSelectionPresetMenu->selectedItem, nullptr);
    preferences.getChildWithName("Grid").setProperty("onSelectScene", prefOnSelectionSceneMenu->selectedItem, nullptr);
    */
    saveDataFile();
}

void GridWindow::readDataFile() {
    String path = LibMain::lib->getPathToMe() + File::getSeparatorString() + PROJECT_NAME + ".xml";
    File dataFile = File(path);
    /*
    if (!dataFile.existsAsFile()) {
        gridWindow->preferences = gridWindow->setPreferenceDefaults();
        return;
    }
    */
    if (dataFile.existsAsFile()) {
        std::unique_ptr<XmlElement> xml = XmlDocument(dataFile.loadFileAsString()).getDocumentElement();
        gridWindow->preferences = ValueTree::fromXml(*xml);
    }
}

void GridWindow::saveDataFile() {
    // Update version
    //gridWindow->preferences.setProperty("version", (String)PROJECT_VERSION, nullptr);
    
    String output = gridWindow->preferences.toXmlString();
    String path = LibMain::lib->getPathToMe() + File::getSeparatorString() + PROJECT_NAME + ".xml";;
    File dataFile = File(path);
    dataFile.create();
    dataFile.replaceWithText(output);
}

void GridTimer::timerCallback()
{
    GridWindow::hideGrid();
    this->stopTimer();
    LibMain::lib->setWidgetValue("GPGS_DISPLAY", 0.0);
}

