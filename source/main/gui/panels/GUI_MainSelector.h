/*
    This source file is part of Rigs of Rods
    Copyright 2005-2012 Pierre-Michel Ricordel
    Copyright 2007-2012 Thomas Fischer
    Copyright 2013-2017 Petr Ohlidal & contributors

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
/// @author Moncef Ben Slimane
/// @date   11/2014

#pragma once

#include "BeamData.h" // ActorSpawnRequest
#include "CacheSystem.h" // CacheSearchMethod
#include "ForwardDeclarations.h"
#include "GUI_MainSelectorLayout.h"


namespace RoR {
namespace GUI {

class MainSelector : public MainSelectorLayout
{
public:
    MainSelector();
    ~MainSelector();

    bool IsFinishedSelecting();
    void Show(LoaderType type, ActorSpawnRequest req);
    void Show(LoaderType type);
    void Hide(bool smooth = true);
    bool IsVisible();
    void Reset();
    void Cancel();

    CacheEntry* GetSelectedEntry() { return m_selected_entry; }

private:

    void NotifyWindowButtonPressed(MyGUI::WidgetPtr _sender, const std::string& _name);

    // gui events
    void EventComboAcceptConfigComboBox(MyGUI::ComboBoxPtr _sender, size_t _index);
    void EventComboChangePositionTypeComboBox(MyGUI::ComboBoxPtr _sender, size_t _index);
    void EventKeyButtonPressed_Main(MyGUI::WidgetPtr _sender, MyGUI::KeyCode _key, MyGUI::Char _char);
    void EventListChangePositionModelList(MyGUI::ListPtr _sender, size_t _index);
    void EventListChangePositionModelListAccept(MyGUI::ListPtr _sender, size_t _index);
    void EventMouseButtonClickCancelButton(MyGUI::WidgetPtr _sender);
    void EventMouseButtonClickOkButton(MyGUI::WidgetPtr _sender);
    void EventSearchTextChange(MyGUI::EditBox* _sender);
    void EventSearchTextGotFocus(MyGUI::WidgetPtr _sender, MyGUI::WidgetPtr oldWidget);
    void NotifyWindowChangeCoord(MyGUI::Window* _sender);
    void ResizePreviewImage();
    void BindKeys(bool bind = true);

    // other functions
    void UpdateGuiData();
    void OnCategorySelected(int categoryID);
    void OnEntrySelected(int entryID);
    void OnSelectionDone();
    void UpdateSearchParams();
    void UpdateControls(CacheEntry* entry);
    void SetPreviewImage(Ogre::String texture);
    void FrameEntered(float dt);
    bool IsFresh(CacheEntry* entry);

    CacheEntry* m_selected_entry;
    LoaderType m_loader_type;
    CacheSearchMethod m_search_method;
    std::string m_search_query;
    Ogre::String m_preview_image_texture;
    bool m_selection_done;
    std::vector<CacheEntry> m_entries;
    bool m_keys_bound;
    ActorSpawnRequest m_actor_spawn_rq; //!< Serves as context when spawning an actor
    std::time_t m_cache_file_freshness;
    std::map<LoaderType, int> m_category_last_index; //!< Stores the last manually selected category index for each loader type
    std::map<LoaderType, int> m_entry_index;    //!< Stores the last manually selected entry index for each loader type
    bool m_searching;
};

} // namespace GUI
} // namespace RoR
