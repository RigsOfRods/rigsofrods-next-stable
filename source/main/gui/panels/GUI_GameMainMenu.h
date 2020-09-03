/*
    This source file is part of Rigs of Rods
    Copyright 2005-2012 Pierre-Michel Ricordel
    Copyright 2007-2012 Thomas Fischer
    Copyright 2014-2017 Petr Ohlidal & contributors

    For more information, see http://www.rigsofrods.org/

    Rigs of Rods is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3, as
    published by the Free Software Foundation.

    Rigs of Rods is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Rigs of Rods. If not, see <http://www.gnu.org/licenses/>.
*/

/// @file
/// @author Petr Ohlidal
/// @date   06/2017

#pragma once

#include <imgui.h>

namespace RoR {
namespace GUI {

class GameMainMenu
{
public:
    // This class implements hand-made keyboard focus - button count must be known for wrapping
    const float   WINDOW_WIDTH          = 110.f;
    const float   WINDOW_HEIGHT          = 785.f;
    const ImVec4  WINDOW_BG_COLOR       = ImVec4(0.1f, 0.1f, 0.1f, 0.8f);
    const ImVec4  BUTTON_BG_COLOR       = ImVec4(0.25f, 0.25f, 0.24f, 0.6f); // Drawn on top of a transparent panel; make it just a shade
    const ImVec2  BUTTON_PADDING        = ImVec2(4.f, 4.f);

    GameMainMenu();

    inline bool   IsVisible() const                { return m_is_visible; }
    inline void   SetVisible(bool v)               { m_is_visible = v; m_kb_focus_index = -1; }
    void          Draw();

private:

    void          DrawMenuPanel();
    void          DrawVersionBox();

    bool   m_is_visible;
    int    m_num_buttons;
    int    m_kb_focus_index; // -1 = no focus; 0+ = button index
    int    m_kb_enter_index; // Focus positon when enter key was pressed.
    bool DrawIcon(Ogre::TexturePtr tex);
};

} // namespace GUI
} // namespace RoR
