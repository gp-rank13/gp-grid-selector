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
  void mouseDown(const MouseEvent &e) override;
  void paint(Graphics& g) override;
  String name = "";
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
  GridSelectorMain () {};
 
  void paint (Graphics& g) override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GridSelectorMain)
};

class GridMenu : public Component
{
public:
  GridMenu () {};
  void paint (Graphics& g) override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GridMenu)
};

class GridTimer : public Timer
{
public:
  virtual void timerCallback() override;
};

class GridPreferenceUpButton : public ShapeButton
{
public:
    GridPreferenceUpButton(const juce::String& name, juce::Colour normalColour, juce::Colour overColour, juce::Colour downColour)
        : juce::ShapeButton(name, normalColour, overColour, downColour)
    {
      Path p;
      p.loadPathFromData (rightArrowPath, sizeof (rightArrowPath));
      setShape (p, true, true, false);
      setClickingTogglesState(false);
    }
};

class GridPreferenceDownButton : public ShapeButton
{
public:
    GridPreferenceDownButton(const juce::String& name, juce::Colour normalColour, juce::Colour overColour, juce::Colour downColour)
        : juce::ShapeButton(name, normalColour, overColour, downColour)
    {
      Path p;
      p.loadPathFromData (leftArrowPath, sizeof (leftArrowPath));
      setShape (p, true, true, false);
      setClickingTogglesState(false);
    }
};



class GridWindow  : public Component, 
                    public juce::Button::Listener
{
public:
  GridWindow ();
  ~GridWindow() override;
  void resized() override;
  void buttonClicked (Button* buttonThatWasClicked) override;

  void static initialize();
  void static finalize();
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
  void static gridBank(bool down);
  void static songRackspaceModeChanged();

  void updateGridItems(bool presetMode);
  void setGridDisplayMode (bool presetMode);
  void static toggleGridDisplayMode();

  std::unique_ptr<GridSelectorMain> grid;
  std::unique_ptr<GridSelectorTitle> gridTitle;
  std::unique_ptr<GridSelectorBankUp> gridBankUp;
  std::unique_ptr<GridSelectorBankDown> gridBankDown;
  std::unique_ptr<GridMenu> gridMenu;
  std::unique_ptr<ShapeButton> preferencesButton;
  std::unique_ptr<ShapeButton> preferencesCloseButton;
  std::unique_ptr<ShapeButton> closeButton;
  std::unique_ptr<ShapeButton> backButton;

  static GridWindow* gridWindow;
  int gridItemWidthCount = GRID_COLUMNS_DEFAULT;
  int gridItemHeightCount = GRID_ROWS_DEFAULT;
  int directSelectCount = GRID_DIRECT_SELECT_DEFAULT;
  int gridBankRowCount = 1;
  bool gridCloseOnItemSelect = false;
  bool gridDisplaySceneNameInTitle = false;
  bool gridDisplayZeroBasedNumbers = false;
  bool gridPresetMode = true;
  int gridStartIndex = 0;
  int presetIndex = 0;
  int presetGridStartIndex = 0;
  int sceneIndex = 0;
  int sceneGridStartIndex = 0;

private:
  void updateGrid();

  int gridDirectSelect(int index);
  std::unique_ptr<GridTimer> gridTimer;
  //int gridStartIndex = 0; moving to public for testing
  OwnedArray<GridSelectorItem> gridItems;
  StringArray presetNames;
  StringArray sceneNames;

  std::unique_ptr<GridPreferenceUpButton> gridColumnUpButton;
  std::unique_ptr<GridPreferenceDownButton> gridColumnDownButton;
  std::unique_ptr<GridPreferenceUpButton> gridRowUpButton;
  std::unique_ptr<GridPreferenceDownButton> gridRowDownButton;
  std::unique_ptr<GridPreferenceUpButton> gridBankRowUpButton;
  std::unique_ptr<GridPreferenceDownButton> gridBankRowDownButton;
  std::unique_ptr<GridPreferenceUpButton> gridDirectSelectUpButton;
  std::unique_ptr<GridPreferenceDownButton> gridDirectSelectDownButton;
  //std::unique_ptr<DrawableButton> prefToggleLatchingSwitches;
  std::unique_ptr<DrawableButton> prefToggleCloseOnSelect;
  std::unique_ptr<DrawableButton> prefToggleDisplaySceneNameInTitle;
  std::unique_ptr<DrawableButton> prefToggleDisplayZeroBasedNumbers;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GridWindow)
};


