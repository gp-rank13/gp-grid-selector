using namespace juce;

class PopupMenuLookAndFeel : public LookAndFeel_V4 {
public:

    int getPopupMenuBorderSize() override
    {
        return 2;
    }

    void drawPopupMenuBackgroundWithOptions (Graphics& g, int width, int height, const PopupMenu::Options& ) override
    {    
        g.setColour(Colours::yellow);
        g.fillAll(Colour(0xff242424));
        //g.setColour(Colour(0x979797));
        g.setColour(Colour(0xffffffff));
        g.drawRect(0, 0, width, height);
    }

    void drawPopupMenuSectionHeader (Graphics& g,
                                                 const Rectangle<int>& area,
                                                 const String& sectionName) override
    {
        auto font = getPopupMenuFont().boldened();
        font.setHeight(15.0f);
        g.setFont(font);
        //g.setFont (getPopupMenuFont().boldened());
        g.setColour (findColour (PopupMenu::headerTextColourId));

        g.drawFittedText (sectionName,
                        area.getX() + 12, area.getY(), area.getWidth() - 16, (int) ((float) area.getHeight() * 0.8f),
                        Justification::bottomLeft, 1);
    }
    void drawPopupMenuItem (Graphics &g, const Rectangle< int > &area, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, 
                              const String &text, const String &shortcutKeyText, const Drawable *icon, const Colour *textColourToUse) override
    {
        if (isSeparator)
        {
            auto r  = area.reduced (5, 0);
            r.removeFromTop (roundToInt (((float) r.getHeight() * 0.5f) - 0.5f));
            g.setColour (findColour (PopupMenu::textColourId).withAlpha (0.3f));
            g.fillRect (r.removeFromTop (1));
        }
        else
        {
            auto textColour = (textColourToUse == nullptr ? findColour (PopupMenu::textColourId)
                                                        : *textColourToUse);

            auto r  = area.reduced (1);

            if (isHighlighted && isActive)
            {
                g.setColour(Colour(0xff386da5));
                g.fillRect (r);
                g.setColour (findColour (PopupMenu::highlightedTextColourId));
            }
            else
            {
                g.setColour (textColour.withMultipliedAlpha (isActive ? 1.0f : 0.5f));
            }

            r.reduce (jmin (5, area.getWidth() / 20), 0);

            auto font = getPopupMenuFont();

            auto maxFontHeight = (float) r.getHeight() / 1.3f;

            if (font.getHeight() > maxFontHeight)
                font.setHeight (maxFontHeight);

            font.setHeight(20.0f);
            g.setFont(font);

            auto iconArea = r.removeFromLeft (roundToInt (maxFontHeight)).toFloat();

            if (icon != nullptr)
            {
                icon->drawWithin (g, iconArea, RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize, 1.0f);
                r.removeFromLeft (roundToInt (maxFontHeight * 0.5f));
            }
            else if (isTicked)
            {
                auto tick = getTickShape (1.0f);
                g.fillPath (tick, tick.getTransformToScaleToFit (iconArea.reduced (iconArea.getWidth() / 5, 0).toFloat(), true));
            }

            if (hasSubMenu)
            {
                auto arrowH = 0.6f * getPopupMenuFont().getAscent();

                auto x = static_cast<float> (r.removeFromRight ((int) arrowH).getX());
                auto halfH = static_cast<float> (r.getCentreY());

                Path path;
                path.startNewSubPath (x, halfH - arrowH * 0.5f);
                path.lineTo (x + arrowH * 0.6f, halfH);
                path.lineTo (x, halfH + arrowH * 0.5f);

                g.strokePath (path, PathStrokeType (2.0f));
            }

            r.removeFromRight (3);
            g.drawFittedText (text, r, Justification::centredLeft, 1);

            if (shortcutKeyText.isNotEmpty())
            {
                auto f2 = font;
                f2.setHeight (f2.getHeight() * 0.75f);
                f2.setHorizontalScale (0.95f);
                g.setFont (f2);

                g.drawText (shortcutKeyText, r, Justification::centredRight, true);
            }
        }
    }
};