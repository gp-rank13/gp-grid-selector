// Grid selector extension for Gig Performer by @rank13

#pragma once
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "Constants.h"

using namespace juce;

class GridSelectorItem : public juce::Component
{
public:
    GridSelectorItem() {};
    void mouseDown(const MouseEvent &e) override;
    void mouseEnter (const MouseEvent& e) override;
    void mouseExit (const MouseEvent& e) override;
    
    void paint(Graphics& g) override;
    String name = "Song";
    int number = 0;
    int directSelectNumber = 0;
    bool selected = false;
    bool hover = false;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GridSelectorItem)
};

class GridSelectorTitle : public juce::Component
{
public:
  GridSelectorTitle() {};
  void mouseUp(const MouseEvent &e) override
  {
    const int nbOfClicks = e.getNumberOfClicks();
    if (nbOfClicks == 2)
      {
        this->getParentComponent()->setVisible(false);
      }
  }
  void paint(Graphics& g) override;
  String name = "Song";
  int number = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GridSelectorTitle)
};

class GridSelectorBankUp : public juce::Component
{
public:
  GridSelectorBankUp() {};
  void mouseDown(const MouseEvent &e) override;
  void mouseEnter (const MouseEvent& e) override;
  void mouseExit (const MouseEvent& e) override;
  void paint(Graphics& g) override;
private:
  bool hover = false;
    
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GridSelectorBankUp)
};

class GridSelectorBankDown : public juce::Component
{
public:
  GridSelectorBankDown() {};
  void mouseDown(const MouseEvent &e) override;
  void mouseEnter (const MouseEvent& e) override;
  void mouseExit (const MouseEvent& e) override;
  void paint(Graphics& g) override;
private:
    bool hover = false;
    
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GridSelectorBankDown)
};

class GridSelectorMain : public Component
{
public:
  GridSelectorMain () {
    /*
    GridSelectorItem* gsi = new GridSelectorItem();
    gsi->setBounds(50,50,400,200);
    addAndMakeVisible(gsi);
    */

  }
  //void resized() override;
  /*
  void mouseUp(const MouseEvent &e) override
  {
    const int nbOfClicks = e.getNumberOfClicks();
    if (nbOfClicks == 2)
      {
        //this->setVisible(false);
        this->getParentComponent()->setVisible(false);
      }
  }
  void mouseDown (const MouseEvent& e) override
  {
      dragger.startDraggingComponent (this, e);
  }

  void mouseDrag (const MouseEvent& e) override
  {
      // As there's no titlebar we have to manage the dragging ourselves
      dragger.dragComponent (this, e, nullptr);
  }
*/
  void paint (Graphics& g) override;
  //OwnedArray<GridSelectorItem> gridItems;
  //int gridItemCountW = 4;
  //int gridItemCountH = 2;

//private:
  //ComponentDragger dragger;
  //int gridItemWidthCount = 4;
  //int gridItemHeightCount = 3;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GridSelectorMain)
};

class GridTimer : public Timer
{
public:
  virtual void timerCallback() override;
};

class GridWindow  : public Component, 
                    public juce::Button::Listener
{
public:
  GridWindow ();
  ~GridWindow() override;
  void resized() override;
  void buttonClicked (Button* buttonThatWasClicked) override;

  void static initialize(); // Call this to show the component
  void static finalize(); // Call this when library is about to be unloaded to free resources
  void static showGrid();
  void static hideGrid();
  void static setGridSize(int width, int height);
  void static setGridPosition(int x, int y);
  void static setGridPositionByName(std::string positionName);
  void static setGridDuration(int ms);
  void static setGridPositionSize(int x, int y, int width, int height);
  void static setGridNamedPositionSize(std::string positionName, int width, int height);
  void static presetChanged(int index, StringArray names);
  void static sceneChanged(int index, StringArray names);
  void static titleChanged(int index, String name);
  void static directSelect(String name);
  void gridBank(bool down);

  std::unique_ptr<GridSelectorMain> grid;
  std::unique_ptr<GridSelectorTitle> gridTitle;
  std::unique_ptr<GridSelectorBankUp> gridBankUp;
  std::unique_ptr<GridSelectorBankDown> gridBankDown;
  static GridWindow* gridWindow;

private:
  
  void updateGrid();
  int gridDirectSelect(int index);
  std::unique_ptr<GridTimer> gridTimer;
  int gridItemWidthCount = 4;
  int gridItemHeightCount = 3;
  int gridBankRowCount = 1;
  int gridStartIndex = 0;
  OwnedArray<GridSelectorItem> gridItems;
  std::unique_ptr<ShapeButton> menuButton;
  std::unique_ptr<ShapeButton> closeButton;
  //std::unique_ptr<ShapeButton> upButton;
  //std::unique_ptr<ShapeButton> downButton;

  void showTransparentWindow()
  {
      grid.reset (new GridSelectorMain ());
      grid->addToDesktop (ComponentPeer::windowIsTemporary);
      //Rectangle<int> area (0, 0, 500, 150); // Default position, width and height
      //Rectangle<int> area = Desktop::getInstance().getDisplays().getMainDisplay().userArea; //Deprecated
      //const juce::Displays::Display* screen = Desktop::getInstance().getDisplays().getPrimaryDisplay();
      //Rectangle<int> area = Desktop::getInstance().getDisplays().getPrimaryDisplay().userArea();
       auto bounds = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()->totalArea;
      //Rectangle<int> area = screen.userArea();
      grid->setBounds (bounds);
      grid->setAlwaysOnTop (true);
      grid->setVisible (false);
  }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GridWindow)
};


