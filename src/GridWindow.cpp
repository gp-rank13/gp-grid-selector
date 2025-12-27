// Grid extension for Gig Performer by @rank13

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

    prefToggleCloseOnSelect.reset (new DrawableButton("closeOnSelect", DrawableButton::ImageFitted));
    prefToggleCloseOnSelect->setImages(&off, 0, 0, 0, &on);
    prefToggleCloseOnSelect->setClickingTogglesState(true);
    prefToggleCloseOnSelect->setRepaintsOnMouseActivity(true); 
    prefToggleCloseOnSelect->addListener (this);
    prefToggleCloseOnSelect->setColour (DrawableButton::backgroundOnColourId, Colour(0x00000000));
    gridMenu->addAndMakeVisible(prefToggleCloseOnSelect.get());
    
    // Component position/size
    auto y = bounds.getHeight() * 0.1f;
    auto rightX = bounds.getWidth() - y;
    gridTitle->setBounds(bounds.withHeight(y));
    grid->setBounds(bounds.withTop(y).withRight(rightX));
    gridBankUp->setBounds(rightX, y, y, grid->getHeight() / 2);
    gridBankDown->setBounds(rightX, y + (grid->getHeight() / 2), y, grid->getHeight() / 2);
    preferencesButton->setBounds(getWidth() - 140, 38, 38, 38);
    closeButton->setBounds(getWidth() - 60, 40, 35, 35);
    gridMenu->setBounds(bounds.getWidth() - 480, y - 10, 400, 900 );
    preferencesCloseButton->setBounds(gridMenu->getWidth() - 60, 40, 35, 35);
    gridColumnDownButton->setBounds(260,200,50,50);
    gridColumnUpButton->setBounds(320,200,50,50);
    gridRowDownButton->setBounds(260,270,50,50);
    gridRowUpButton->setBounds(320,270,50,50);
    gridBankRowDownButton->setBounds(260,440,50,50);
    gridBankRowUpButton->setBounds(320,440,50,50);
    gridDirectSelectDownButton->setBounds(260,610,50,50);
    gridDirectSelectUpButton->setBounds(320,610,50,50);
    prefToggleCloseOnSelect->setBounds(30,780,50,50);
}

GridWindow::~GridWindow()
{
    grid = nullptr;
    gridTimer = nullptr;
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
        setVisible(false);
        gridMenu->setVisible(false);
        preferencesButton->setToggleState(false, juce::NotificationType::dontSendNotification);
        lib->setWidgetValue("GPGS_DISPLAY", 0.0);
    } else if (buttonThatWasClicked == preferencesButton.get()) {
        gridMenu->setVisible(buttonThatWasClicked->getToggleState());
    } if (buttonThatWasClicked == preferencesCloseButton.get()) {
        gridMenu->setVisible(false);
        preferencesButton->setToggleState(false, juce::NotificationType::dontSendNotification);
    } else if (buttonThatWasClicked == gridColumnUpButton.get()) {
        gridItemWidthCount = jmin(gridItemWidthCount + 1, GRID_COLUMNS_MAX);
        updateGrid();
    } else if (buttonThatWasClicked == gridColumnDownButton.get()) {
        gridItemWidthCount = jmax(gridItemWidthCount - 1, GRID_COLUMNS_MIN);
        updateGrid();
    } else if (buttonThatWasClicked == gridRowUpButton.get()) {
        gridItemHeightCount = jmin(gridItemHeightCount + 1, GRID_ROWS_MAX);
        updateGrid();
    } else if (buttonThatWasClicked == gridRowDownButton.get()) {
        gridItemHeightCount = jmax(gridItemHeightCount - 1, GRID_ROWS_MIN);
        updateGrid();
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
    }
}

void GridWindow::presetChanged(int index, StringArray names) {
    if (gridWindow == nullptr) return;
    if (gridWindow->grid == nullptr) return;
    MessageManager::getInstance()->callAsync([index, names]() {
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
    });
}

void GridWindow::sceneChanged(int index, StringArray names) {
    MessageManager::getInstance()->callAsync([index, names]() {
    });
}

void GridWindow::titleChanged(int index, String name) {
    gridWindow->gridTitle->number = index;
    gridWindow->gridTitle->name = name;
    gridWindow->gridTitle->repaint();
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
        gridWindow->gridStartIndex = down ? jmin(gridWindow->gridStartIndex + (gridWindow->gridBankRowCount * gridWindow->gridItemWidthCount), gridWindow->gridItems.size() - (gridWindow->gridBankRowCount * gridWindow->gridItemWidthCount) - 1)
                                        : jmax(gridWindow->gridStartIndex - (gridWindow->gridBankRowCount * gridWindow->gridItemWidthCount), 0);
        gridWindow->updateGrid();
    });
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
}

void GridWindow::directSelect(String name) {
    MessageManager::getInstance()->callAsync([name]() {
        // Extract direct select number from name
        int number = name.getTrailingIntValue();
        for (int i = gridWindow->gridStartIndex; i < gridWindow->gridItems.size(); ++i) { 
            if (gridWindow->gridItems[i]->directSelectNumber == number) {
                if (lib->inSetlistMode()) {
                    (void)lib->switchToSong(i, 0);
                } else {
                    (void)lib->switchToRackspace(i, 0);
                }
                if (GridWindow::gridWindow->gridCloseOnItemSelect) {
                    gridWindow->setGridDuration(GRID_CLOSE_DELAY_MS);
    }
                return;
            }
        }
    });
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
    g.drawFittedText (String(number),
                        getLocalBounds().reduced (25, 15),
                        Justification::topLeft, 1, 1.f);
    // Direct Select Number
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
    if (lib->inSetlistMode()) {
        (void)lib->switchToSong(number, 0);
    } else {
        (void)lib->switchToRackspace(number, 0);
    }
    if (GridWindow::gridWindow->gridCloseOnItemSelect) {
        GridWindow::setGridDuration(GRID_CLOSE_DELAY_MS);
    }
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
    g.drawFittedText (String(number),
                        getLocalBounds().reduced (30, 0),
                        Justification::left, 1, 1.f);
    // Name
    g.setColour (Colours::white);
    g.drawFittedText (name,
                        area.reduced (50 + g.getCurrentFont().getStringWidth(String(number)), 0),
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
                        getLocalBounds().withLeft(30).withTop(30),
                        Justification::topLeft, 1, 1.f);
    
    g.setColour (Colour(0xff404040));                    
    g.fillRect(30, 110, 340, 1);

    g.setFont (40);
    g.setColour (Colour(0xffe0e0e0));
    g.drawFittedText ("Grid Size",
                        getLocalBounds().withLeft(30).withTop(130),
                        Justification::topLeft, 1, 1.f);
    g.setFont (35);
    g.setColour (Colour(0xffd0d0d0));                    
    g.drawFittedText ("Columns: " + String(GridWindow::gridWindow->gridItemWidthCount),
                        getLocalBounds().withLeft(30).withTop(205),
                        Justification::topLeft, 1, 1.f);
   
    g.drawFittedText ("Rows: " + String(GridWindow::gridWindow->gridItemHeightCount),
                        getLocalBounds().withLeft(30).withTop(275),
                        Justification::topLeft, 1, 1.f);
    
    g.setColour (Colour(0xff404040));                    
    g.fillRect(30, 350, 340, 1);

    g.setFont (40);
    g.setColour (Colour(0xffe0e0e0));
    g.drawFittedText ("Bank Size",
                        getLocalBounds().withLeft(30).withTop(370),
                        Justification::topLeft, 1, 1.f);
    g.setFont (35);    
    g.setColour (Colour(0xffd0d0d0));                                
    g.drawFittedText ("Rows: " + String(GridWindow::gridWindow->gridBankRowCount),
                        getLocalBounds().withLeft(30).withTop(445),
                        Justification::topLeft, 1, 1.f);
    
    g.setColour (Colour(0xff404040));                    
    g.fillRect(30, 520, 340, 1);

    g.setFont (40);
    g.setColour (Colour(0xffe0e0e0));
    g.drawFittedText ("Direct Select",
                        getLocalBounds().withLeft(30).withTop(540),
                        Justification::topLeft, 1, 1.f);
    g.setFont (35);    
    g.setColour (Colour(0xffd0d0d0));                                
    g.drawFittedText ("Count: " + String(GridWindow::gridWindow->directSelectCount),
                        getLocalBounds().withLeft(30).withTop(615),
                        Justification::topLeft, 1, 1.f);

    g.setColour (Colour(0xff404040));                    
    g.fillRect(30, 690, 340, 1);

    g.setFont (40);
    g.setColour (Colour(0xffe0e0e0));
    g.drawFittedText ("On Item Selection",
                        getLocalBounds().withLeft(30).withTop(710),
                        Justification::topLeft, 1, 1.f);
    g.setFont (35);    
    g.setColour (Colour(0xffd0d0d0));                     
    g.drawFittedText ("Close Selector",
                        getLocalBounds().withLeft(100).withTop(785),
                        Justification::topLeft, 1, 1.f);
}

void GridTimer::timerCallback()
{
    GridWindow::hideGrid();
    this->stopTimer();
    lib->setWidgetValue("GPGS_DISPLAY", 0.0);
}

