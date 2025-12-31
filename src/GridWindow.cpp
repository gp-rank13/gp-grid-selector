// Grid selector extension for Gig Performer by @rank13

#include <juce_events/juce_events.h>
#include "GridWindow.h"
#include "LibMain.h"

GridWindow* GridWindow::gridWindow = nullptr;
LibMain* lib = new LibMain(nullptr);   

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

    prefToggleCloseOnSelect.reset (new DrawableButton("closeOnSelect", DrawableButton::ImageFitted));
    prefToggleCloseOnSelect->setImages(&off, 0, 0, 0, &on);
    prefToggleCloseOnSelect->setClickingTogglesState(true);
    prefToggleCloseOnSelect->setRepaintsOnMouseActivity(true); 
    prefToggleCloseOnSelect->addListener (this);
    prefToggleCloseOnSelect->setColour (DrawableButton::backgroundOnColourId, Colour(0x00000000));
    //gridMenu->addAndMakeVisible(prefToggleCloseOnSelect.get());

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
                                                        //lib->setWidgetValue("GPGS_DISPLAY", 0.0);
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
        //setVisible(false);
        gridMenu->setVisible(false);
        preferencesButton->setToggleState(false, juce::NotificationType::dontSendNotification);
        lib->setWidgetValue("GPGS_DISPLAY", 0.0);
        hideGrid();
    } else if (buttonThatWasClicked == backButton.get()) {
        setGridDisplayMode(true);
    } else if (buttonThatWasClicked == preferencesButton.get()) {
        gridMenu->setVisible(buttonThatWasClicked->getToggleState());
    } else if (buttonThatWasClicked == preferencesCloseButton.get()) {
        gridMenu->setVisible(false);
        preferencesButton->setToggleState(false, juce::NotificationType::dontSendNotification);
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
    } else if (buttonThatWasClicked == prefToggleCloseOnSelect.get()) {
        gridCloseOnItemSelect = buttonThatWasClicked->getToggleState();
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
        //gridWindow->gridPresetMode = true;
        gridWindow->presetNames.clear();
        gridWindow->presetNames.addArray(names);
        gridWindow->presetIndex = index;
        gridWindow->sceneGridStartIndex = 0;
        gridWindow->setGridDisplayMode(gridWindow->gridPresetMode);
        /*
        if (gridWindow->gridPresetMode) {
            //gridWindow->updateGridItems(true);
            gridWindow->setGridDisplayMode(true);
        } else {
            //gridWindow->sceneChanged(lib->getCurrentSongpartIndex(),lib->getSongPartNames(index));
        }
        */
        /*
        gridWindow->gridItems.clear();
        gridWindow->grid->removeAllChildren();
        
        for (int i = 0; i < names.size(); ++i) { 
            GridSelectorItem* gsi = new GridSelectorItem();
            gsi->number = i;
            gsi->name = names[i];
            gsi->selected = i == index;

            gridWindow->gridItems.add(gsi);
            gridWindow->grid->addAndMakeVisible(gsi);
        }
        titleChanged(index, names[index]);
        gridWindow->updateGrid();
        gridWindow->resized();
        */
    });
}

void GridWindow::sceneChanged(int index, StringArray names) {
    if (gridWindow == nullptr) return;
    if (gridWindow->grid == nullptr) return;
    //if (gridWindow->gridPresetMode) return;
    MessageManager::getInstance()->callAsync([index, names]() {
        //gridWindow->gridPresetMode = true;
        gridWindow->sceneNames.clear();
        gridWindow->sceneNames.addArray(names);
        gridWindow->sceneIndex = index;
        if (!gridWindow->gridPresetMode)
            //gridWindow->updateGridItems(false);
            gridWindow->setGridDisplayMode(false);
        gridWindow->gridTitle->repaint();
        /*
        gridWindow->gridItems.clear();
        gridWindow->grid->removeAllChildren();
        
        for (int i = 0; i < names.size(); ++i) { 
            GridSelectorItem* gsi = new GridSelectorItem();
            gsi->number = i;
            gsi->name = names[i];
            gsi->selected = i == index;

            gridWindow->gridItems.add(gsi);
            gridWindow->grid->addAndMakeVisible(gsi);
        }
        titleChanged(presetIndex, presetName);
        gridWindow->updateGrid();
        gridWindow->resized();
        */
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

void GridWindow::updateGridItems(bool presetMode) {

    gridWindow->gridItems.clear();
    gridWindow->grid->removeAllChildren();
    StringArray names = presetMode ? gridWindow->presetNames : gridWindow->sceneNames;
    int index = presetMode ? gridWindow->presetIndex : gridWindow->sceneIndex;

    if (!presetMode) {
        //gridWindow->sceneChanged(lib->getCurrentSongpartIndex(),lib->getSongPartNames(gridWindow->presetIndex));
        gridWindow->sceneIndex = lib->inSetlistMode() ? lib->getCurrentSongpartIndex() : lib->getCurrentVariationIndex();
        gridWindow->sceneNames = lib->inSetlistMode() ? lib->getSongPartNames(gridWindow->presetIndex) : lib->getVariationNames(gridWindow->presetIndex);
        names = gridWindow->sceneNames;
        index = gridWindow->sceneIndex;
    }
    
    for (int i = 0; i < names.size(); ++i) { 
        GridSelectorItem* gsi = new GridSelectorItem();
        gsi->number = i;
        gsi->name = names[i];
        gsi->selected = i == index;

        gridWindow->gridItems.add(gsi);
        gridWindow->grid->addAndMakeVisible(gsi);
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
    String label = lib->inSetlistMode() ? "Songs" : "Rackspaces";
    if (!gridWindow->gridPresetMode) {
        label = lib->inSetlistMode() ? "Song Parts" : "Variations";
    }
    lib->setWidgetCaption("GPGS_DIRECT_SELECT_LABEL","Direct Select - " + label.toStdString());
}

void GridWindow::setGridDisplayMode (bool presetMode) {
    // Store current bank/grid item starting inde
    if (gridPresetMode) {
        presetGridStartIndex = gridStartIndex;
    } else {
        sceneGridStartIndex = gridStartIndex;
    }
    backButton->setVisible(!presetMode);
    gridPresetMode = presetMode;
    gridStartIndex = presetMode ? presetGridStartIndex : sceneGridStartIndex;
    updateGridItems(presetMode);
    updateDirectSelectLabel();
}

void GridWindow::toggleGridDisplayMode() {
    if (!gridWindow->isVisible()) {
        lib->setWidgetValue("GPGS_DISPLAY", 1.0);
        //gridWindow->setVisible(true);
        
    } else if (gridWindow->gridPresetMode) {
        gridWindow->setGridDisplayMode(false);
    } else {
        gridWindow->setGridDisplayMode(true);
    }
}

void GridWindow::triggerGridItem (int number) {
    if (gridWindow->gridPresetMode) {
        int onSelectionAction = gridWindow->prefOnSelectionPresetMenu->selectedItem;
        if (onSelectionAction == 1) {
            if (gridWindow->presetIndex == number) {
                gridWindow->setGridDisplayMode(false);
            }  else {
                if (lib->inSetlistMode()) {
                    (void)lib->switchToSong(number, 0);
                } else {
                    (void)lib->switchToRackspace(number, 0);
                }
                juce::Timer::callAfterDelay (500,[]() {
                    gridWindow->setGridDisplayMode(false);
                });
            }
        } else if (onSelectionAction >= 2) {
            if (lib->inSetlistMode()) {
                (void)lib->switchToSong(number, 0);
            } else {
                (void)lib->switchToRackspace(number, 0);
            }
            if (onSelectionAction == 2) {
                juce::Timer::callAfterDelay (500,[]() {
                        hideGrid();
                        lib->setWidgetValue("GPGS_DISPLAY", 0.0);
                });
            }
        }
    } else {
        int onSelectionAction = gridWindow->prefOnSelectionSceneMenu->selectedItem;
        if (lib->inSetlistMode()) {
            (void)lib->switchToSongPart(number);
        } else {
            (void)lib->switchToVariation(number);
        }
        if (onSelectionAction == 1) {
            juce::Timer::callAfterDelay (500,[]() {
                    hideGrid();
                    lib->setWidgetValue("GPGS_DISPLAY", 0.0);
            });
        }
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
    if (selected) {
        g.setColour (Colour(0xff101010));
    } else if (hover) {
        g.setColour (Colour(0xff303030));
    } else {
        g.setColour (Colour(0xff252525));
    }
    g.fillRoundedRectangle (area, cornerSize);
    
    // Border
    if (selected) {
        g.setColour (Colours::white);
        g.drawRoundedRectangle(area, cornerSize, 5.f);
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
    g.setColour (Colours::white);
    g.drawFittedText (name,
                        getLocalBounds().reduced (20, 0),
                        Justification::centred, displayLines, 1.f);
    // Number
    g.setColour (Colour(0xff606060));
    int displayNumber = GridWindow::gridWindow->gridDisplayZeroBasedNumbers ? number : number + 1;
    g.drawFittedText (String(displayNumber),
                        getLocalBounds().reduced (25, 15),
                        Justification::topLeft, 1, 1.f);
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
        if (lib->inSetlistMode()) {
            g.setColour (Colour(0xfff2924f));
        } else {
            g.setColour (Colour(0xff3d6e31));
        }
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
                if (lib->inSetlistMode()) {
                    (void)lib->switchToSong(number, 0);
                } else {
                    (void)lib->switchToRackspace(number, 0);
                }
                juce::Timer::callAfterDelay (500,[]() {
                    GridWindow::gridWindow->setGridDisplayMode(false);
                });
            }
        } else if (onSelectionAction >= 2) {
            if (lib->inSetlistMode()) {
                (void)lib->switchToSong(number, 0);
            } else {
                (void)lib->switchToRackspace(number, 0);
            }
            if (onSelectionAction == 2) {
                juce::Timer::callAfterDelay (500,[]() {
                        GridWindow::hideGrid();
                        lib->setWidgetValue("GPGS_DISPLAY", 0.0);
                });
            }
        }
    } else {
        int onSelectionAction = GridWindow::gridWindow->prefOnSelectionSceneMenu->selectedItem;
        if (lib->inSetlistMode()) {
            (void)lib->switchToSongPart(number);
        } else {
            (void)lib->switchToVariation(number);
        }
        if (onSelectionAction == 1) {
            juce::Timer::callAfterDelay (500,[]() {
                    GridWindow::hideGrid();
                    lib->setWidgetValue("GPGS_DISPLAY", 0.0);
            });
        }
    }
    */
   /*
    if (lib->inSetlistMode()) {
        if (GridWindow::gridWindow->gridPresetMode) {

            if (GridWindow::gridWindow->presetIndex == number) {
                GridWindow::gridWindow->setGridDisplayMode(false);
            } else {
                (void)lib->switchToSong(number, 0);
                juce::Timer::callAfterDelay (500,[]() {
                    GridWindow::gridWindow->setGridDisplayMode(false);
                });
            }
        } else {
            (void)lib->switchToSongPart(number);
        }
    } else {
        if (GridWindow::gridWindow->gridPresetMode) {
            if (GridWindow::gridWindow->presetIndex == number) {
                GridWindow::gridWindow->setGridDisplayMode(false);
            } else {
                (void)lib->switchToRackspace(number, 0);
                juce::Timer::callAfterDelay (500,[]() {
                    GridWindow::gridWindow->setGridDisplayMode(false);
                });
            }
        } else {
            (void)lib->switchToVariation(number);
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
    //int newIndex = juce::jmax(lib->getCurrentSongIndex() - 1, 0);
    //lib->switchToSong(newIndex, 0);
    GridWindow::gridWindow->gridBank(false);
}

void GridSelectorBankDown::mouseDown(const MouseEvent&) {
    //int newIndex = juce::jmin(lib->getCurrentSongIndex() + 1, lib->getSongCount() - 1);
    //lib->switchToSong(newIndex, 0);
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
    menu.addItem (2, ON_SELECTION_MENU_PRESETS[1]);
    menu.addItem (3, ON_SELECTION_MENU_PRESETS[2]);

    juce::PopupMenu::Options opts = juce::PopupMenu::Options()
        .withTargetComponent(this)
        .withPreferredPopupDirection(juce::PopupMenu::Options::PopupDirection::downwards)
        .withStandardItemHeight(40);

    menu.showMenuAsync(opts,[=] (int result)
    {
        // Menu selection
        if (result >= 1 && result <= ON_SELECTION_MENU_PRESETS.size()) {
            selectedItemText = ON_SELECTION_MENU_PRESETS[result - 1];
            selectedItem = result;
        }
        repaint();
    });
    
}

void GridPopupMenuScene::mouseDown(const MouseEvent&) {
    juce::PopupMenu menu;
    menu.setLookAndFeel(popupLNF);
    menu.addItem (1, ON_SELECTION_MENU_SCENES[0]);
    menu.addItem (2, ON_SELECTION_MENU_SCENES[1]);

    juce::PopupMenu::Options opts = juce::PopupMenu::Options()
        .withTargetComponent(this)
        .withPreferredPopupDirection(juce::PopupMenu::Options::PopupDirection::downwards)
        .withStandardItemHeight(40);

    menu.showMenuAsync(opts,[=] (int result)
    {
        // Menu selection
        if (result >= 1 && result <= ON_SELECTION_MENU_SCENES.size()) {
            selectedItemText = ON_SELECTION_MENU_PRESETS[result - 1];
            selectedItem = result;
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
        String sceneName = lib->inSetlistMode() ? lib->getSongpartName(lib->getCurrentSongIndex(), lib->getCurrentSongpartIndex())
                                                : lib->getVariationName(lib->getCurrentRackspaceIndex(), lib->getCurrentVariationIndex());
        title = title + " — " + sceneName;
    }
    g.drawFittedText (title,
                        area.reduced (50 + g.getCurrentFont().getStringWidth(String(displayNumber)), 0),
                        Justification::left, 1, 1.f);
    
    // Line highlight
    if (lib->inSetlistMode()) {
        g.setColour (Colour(0xfff2924f));
    } else {
        g.setColour (Colour(0xff3d6e31));
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
    g.setColour (Colour(0xff404040));
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
                    getLocalBounds().withLeft(500).withTop(500),
                    Justification::topLeft, 1, 1.f);
    g.drawFittedText ("Display Part/Variation Name in Header",
                    getLocalBounds().withLeft(500).withTop(555),
                    Justification::topLeft, 2, 1.f);
}

void GridPopupMenu::paint (Graphics& g)
{
    auto area = getLocalBounds().toFloat();

    // Background
    float cornerSize = 4.f;
    if (hover) {
        g.setColour (Colour(0xff252525));
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
}

void GridTimer::timerCallback()
{
    GridWindow::hideGrid();
    this->stopTimer();
    lib->setWidgetValue("GPGS_DISPLAY", 0.0);
}

