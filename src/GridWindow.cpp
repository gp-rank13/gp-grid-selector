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
    ///showTransparentWindow();
    
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

    Path p1;
    p1.loadPathFromData (menuPath, sizeof (menuPath));
    menuButton.reset (new ShapeButton ( "menuButton", Colours::lightgrey, Colours::grey, Colours::darkgrey ));
    menuButton->setShape (p1, true, false, false);
    menuButton->setClickingTogglesState(true);
    menuButton->setTooltip("Menu");
    //menuButton->addListener (this);
    addAndMakeVisible (menuButton.get());

    Path p2;
    p2.loadPathFromData (closePath, sizeof (closePath));
    closeButton.reset (new ShapeButton ( "closeButton", Colours::lightgrey, Colours::grey, Colours::darkgrey ));
    closeButton->setShape (p2, true, true, false);
    closeButton->setClickingTogglesState(false);
    closeButton->setTooltip("Close");
    closeButton->addListener (this);
    addAndMakeVisible (closeButton.get());


/*
    Path p2;
    p2.loadPathFromData (playPath, sizeof (playPath));
    p2.applyTransform(juce::AffineTransform::rotation(3.14159 / -2));
    upButton.reset (new ShapeButton ( "upButton", Colours::lightgrey, Colours::grey, Colours::darkgrey ));
    upButton->setShape (p2, true, true, false);
    upButton->setClickingTogglesState(true);
    upButton->setTooltip("Up");
    //menuButton->addListener (this);
    //gridBank->addAndMakeVisible (upButton.get());

    p2.applyTransform(juce::AffineTransform::rotation(3.14159));
    downButton.reset (new ShapeButton ( "downButton", Colours::lightgrey, Colours::grey, Colours::darkgrey ));
    downButton->setShape (p2, true, true, false);
    downButton->setClickingTogglesState(true);
    downButton->setTooltip("Down");
    //menuButton->addListener (this);
    //gridBank->addAndMakeVisible (downButton.get());
*/

    // Component sizing
    auto y = bounds.getHeight() * 0.1f;
    auto rightX = bounds.getWidth() - y;
    gridTitle->setBounds(bounds.withHeight(y));
    grid->setBounds(bounds.withTop(y).withRight(rightX));
    gridBankUp->setBounds(rightX, y, y, grid->getHeight() / 2);
    gridBankDown->setBounds(rightX, y + (grid->getHeight() / 2), y, grid->getHeight() / 2);
    menuButton->setBounds(getWidth() - 140, 40, 40, 35);
    closeButton->setBounds(getWidth() - 60, 40, 35, 35);
    //upButton->setBounds(gridBank->getWidth() / 2 - 20, gridBank->getHeight() * 0.25 - 20, 40, 40);
    //downButton->setBounds(gridBank->getWidth() / 2 - 20, gridBank->getHeight() * 0.75 - 20, 40, 40);
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
                                                        //gridWindow->grid->setVisible(false);
                                                        //gridWindow->grid->setVisible(true);
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
                                                        //gridWindow->grid->setVisible(false);
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
                                                        if (Grid_Duration >= 0)
                                                            gridWindow->gridTimer->startTimer(ms);
                                                        });
    }

}

void GridWindow::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == closeButton.get()) {
        setVisible(false);
        lib->setWidgetValue("GPGS_DISPLAY", 0.0);
    }
}

void GridWindow::presetChanged(int index, StringArray names) {
    if (gridWindow == nullptr) return;
    if (gridWindow->grid == nullptr) return;
    gridWindow->gridItems.clear();
    gridWindow->grid->removeAllChildren();
    
    for (int i = 0; i < names.size(); ++i) { 
        GridSelectorItem* gsi = new GridSelectorItem();
        gsi->number = i;
        gsi->name = names[i];
        gsi->selected = i == index;
        //gsi->directSelectNumber = gridWindow->gridDirectSelect(i);

        gridWindow->gridItems.add(gsi);
        gridWindow->grid->addAndMakeVisible(gsi);
    }
    titleChanged(index, names[index]);
    gridWindow->updateGrid();
    gridWindow->resized();

}

void GridWindow::sceneChanged(int index, StringArray names) {

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
    gridWindow->gridStartIndex = down ? jmin(gridStartIndex + (gridBankRowCount * gridItemWidthCount), gridWindow->gridItems.size() - (gridBankRowCount * gridItemWidthCount) - 1)
                                      : jmax(gridStartIndex - (gridBankRowCount * gridItemWidthCount), 0);
    gridWindow->updateGrid();
}

int GridWindow::gridDirectSelect(int index) {
    int directSelect = 0;
    
    // Direct select number. Determine the starting index, targeting the middle row.
    int directSelectRows = ceil(directSelectCount / gridWindow->gridItemWidthCount);
    int startingRow = floor((gridWindow->gridItemHeightCount - directSelectRows) / 2.f);
    int startingIndex = startingRow * gridWindow->gridItemWidthCount - 1;

    if (index - gridStartIndex >= startingIndex && index - gridStartIndex < startingIndex + directSelectCount + 1) {
        directSelect = index - gridStartIndex - startingIndex;
    } else {
        directSelect = 0;
    }

    return directSelect;
}

void GridWindow::updateGrid() {

    int gridCount = gridItemWidthCount * gridItemHeightCount;

    // Direct select number: Determine the starting index, targeting the middle row.
    int directSelectRows = ceil(directSelectCount / gridWindow->gridItemWidthCount);
    int startingRow = floor((gridWindow->gridItemHeightCount - directSelectRows) / 2.f);
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
        }
    }
    resized();
}

void GridWindow::directSelect(String name) {
    // Extract direct select number from name
    int number = name.getTrailingIntValue();
    for (int i = gridWindow->gridStartIndex; i < gridWindow->gridItems.size(); ++i) { 
        if (gridWindow->gridItems[i]->directSelectNumber == number) {
            if (lib->inSetlistMode()) {
                auto _ = lib->switchToSong(i, 0);
            } else {
                auto _ = lib->switchToRackspace(i, 0);
            }
            return;
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
        bool success = lib->switchToSong(number, 0);
    } else {
        bool success = lib->switchToRackspace(number, 0);
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

void GridTimer::timerCallback()
{
    GridWindow::hideGrid();
    this->stopTimer();
}