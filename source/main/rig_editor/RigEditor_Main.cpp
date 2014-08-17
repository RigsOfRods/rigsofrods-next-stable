/*
	This source file is part of Rigs of Rods
	Copyright 2005-2012 Pierre-Michel Ricordel
	Copyright 2007-2012 Thomas Fischer
	Copyright 2013-2014 Petr Ohlidal

	For more information, see http://www.rigsofrods.com/

	Rigs of Rods is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License version 3, as
	published by the Free Software Foundation.

	Rigs of Rods is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Rigs of Rods. If not, see <http://www.gnu.org/licenses/>.
*/

/** 
	@file   RigEditor_Main.cpp
	@date   06/2014
	@author Petr Ohlidal
*/

#include "RigEditor_Main.h"

#include "Application.h"
#include "GlobalEnvironment.h"
#include "GUI_RigEditorMenubar.h"
#include "GUIManager.h"
#include "InputEngine.h"
#include "MainThread.h"
#include "OgreSubsystem.h"
#include "RigEditor_CameraHandler.h"
#include "RigEditor_InputHandler.h"
#include "Settings.h"

#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreMaterialManager.h>
#include <OgreMaterial.h>
#include <OgreMovableObject.h>
#include <OgreEntity.h>

using namespace RoR;
using namespace RoR::RigEditor;

Main::Main():
	m_scene_manager(nullptr),
	m_camera(nullptr),
	m_viewport(nullptr),
	m_rig_entity(nullptr),
	m_exit_loop_requested(false),
	m_input_handler(nullptr),
	m_debug_box(nullptr),
	m_gui_menubar(nullptr),
	m_gui_open_save_file_dialog(nullptr)
{
	/* Load config */
	m_config_file.load(SSETTING("Config Root", "") + "rig_editor.cfg");

	/* Parse config */
	m_config.viewport_background_color = m_config_file.GetColourValue("viewport_background_color_rgb");
	m_config.beam_generic_color        = m_config_file.GetColourValue("beam_generic_color_rgb");
	m_config.scene_ambient_light_color = m_config_file.GetColourValue("scene_ambient_light_color_rgb");

	/* Setup 3D engine */
	OgreSubsystem* ror_ogre_subsystem = RoR::Application::GetOgreSubsystem();
	assert(ror_ogre_subsystem != nullptr);
	m_scene_manager = ror_ogre_subsystem->GetOgreRoot()->createSceneManager(Ogre::ST_GENERIC, "rig_editor_scene_manager");
	m_scene_manager->setAmbientLight(m_config.scene_ambient_light_color);
	m_camera = m_scene_manager->createCamera("rig_editor_camera");

	/* Setup input */
	m_input_handler = new InputHandler();

	/* Camera handling */
	m_camera_handler = new CameraHandler(m_camera);



	/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	TEST
	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/

	m_camera->setNearClipDistance( 0.5 );
	m_camera->setFarClipDistance( 1000.0*1.733 );
	m_camera->setFOVy(Ogre::Degree(60));
	m_camera->setAutoAspectRatio(true);
	//LAST RoR::Application::GetOgreSubsystem()->GetViewport()->setCamera(m_camera);
	//gEnv->mainCamera = m_camera;
	{
		using namespace Ogre;
		m_scene_manager->setAmbientLight(ColourValue(0,1,0.2));
		SceneNode* node = m_scene_manager->getRootSceneNode()->createChildSceneNode();
		Entity* e = m_scene_manager->createEntity("StartupEditorDebugObject", "axes.mesh");
		node->attachObject(e);
		

		m_camera_handler->SetOrbitTarget(node);
		m_camera->setPosition(Ogre::Vector3(300,200,300));
		//m_camera->lookAt(0,100,0);
	}

	/*END TEST
	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
	

	

	/* Debug output box */
	m_debug_box = MyGUI::Gui::getInstance().createWidget<MyGUI::TextBox>
		("TextBox", 50, 50, 300, 300, MyGUI::Align::Top, "Main", "rig_editor_quick_debug_text_box");
	m_debug_box->setCaption("Hello\nRigEditor!");
	m_debug_box->setTextColour(MyGUI::Colour(0.8, 0.8, 0.8));
	m_debug_box->setFontName("DefaultBig");
}

Main::~Main()
{
	if (m_gui_menubar != nullptr)
	{
		delete m_gui_menubar;
		m_gui_menubar = nullptr;
	}
	if (m_gui_open_save_file_dialog != nullptr)
	{
		delete m_gui_open_save_file_dialog;
		m_gui_open_save_file_dialog = nullptr;
	}
}

void Main::EnterMainLoop()
{
	/* Setup 3D engine */
	OgreSubsystem* ror_ogre_subsystem = RoR::Application::GetOgreSubsystem();
	assert(ror_ogre_subsystem != nullptr);
	m_viewport = ror_ogre_subsystem->GetRenderWindow()->addViewport(nullptr);
	int viewport_width = m_viewport->getActualWidth();
	m_viewport->setBackgroundColour(m_config.viewport_background_color);
	m_camera->setAspectRatio(m_viewport->getActualHeight() / viewport_width);
	m_viewport->setCamera(m_camera);
	//m_camera->setPosition(10,10,10);
	//m_camera->lookAt(0,0,0);

	/* Setup GUI */
	RoR::Application::GetGuiManager()->SetSceneManager(m_scene_manager);
	if (m_gui_menubar == nullptr)
	{
		m_gui_menubar = new GUI::RigEditorMenubar(this);
	}
	else
	{
		m_gui_menubar->Show();
	}
	m_gui_menubar->SetWidth(viewport_width);
	if (m_gui_open_save_file_dialog == nullptr)
	{
		m_gui_open_save_file_dialog = new GUI::OpenSaveFileDialog();
	}

	/* Setup input */
	RoR::Application::GetInputEngine()->SetKeyboardListener(m_input_handler);
	RoR::Application::GetInputEngine()->SetMouseListener(m_input_handler);

	/* Show debug box */
	m_debug_box->setVisible(true);

	

	while (! m_exit_loop_requested)
	{
		UpdateMainLoop();

		Ogre::RenderWindow* rw = RoR::Application::GetOgreSubsystem()->GetRenderWindow();
		if (rw->isClosed())
		{
			RoR::Application::GetMainThreadLogic()->RequestShutdown();
			break;
		}

		/* Render */
		RoR::Application::GetOgreSubsystem()->GetOgreRoot()->renderOneFrame();

		if (!rw->isActive() && rw->isVisible())
		{
			rw->update(); // update even when in background !
		}
	}

	/* Hide GUI */
	m_gui_menubar->Hide();
	if (m_gui_open_save_file_dialog->isModal())
	{
		m_gui_open_save_file_dialog->endModal(); // Hides the dialog
	}

	/* Hide debug box */
	m_debug_box->setVisible(false);

	m_exit_loop_requested = false;
}

void Main::UpdateMainLoop()
{
	/* Process window events */
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	RoRWindowEventUtilities::messagePump();
#endif

	/* Update input events */
	m_input_handler->ResetEvents();
	RoR::Application::GetInputEngine()->Capture(); // Also injects input to GUI

	/* Handle key presses */
	if (m_input_handler->WasEventFired(InputHandler::Event::QUIT_RIG_EDITOR))
	{
		m_exit_loop_requested = true;
		return;
	}

	/* Handle mouse move */
	if (m_input_handler->GetMouseMotionEvent().HasMoved() || m_input_handler->GetMouseMotionEvent().HasScrolled())
	{
		m_camera_handler->InjectMouseMove(
			m_input_handler->GetMouseButtonEvent().IsRightButtonDown(), /* (bool do_orbit) */
			m_input_handler->GetMouseMotionEvent().rel_x,
			m_input_handler->GetMouseMotionEvent().rel_y,
			m_input_handler->GetMouseMotionEvent().rel_wheel
		);
	}

	/* Update devel console */
	std::stringstream msg;
	msg << "Camera pos: [X "<<m_camera->getPosition().x <<", Y "<<m_camera->getPosition().y << ", Z "<<m_camera->getPosition().z <<"] "<<std::endl;
	m_debug_box->setCaption(msg.str());
}

void Main::CommandOpenRigFile()
{
	m_gui_open_save_file_dialog->setDialogInfo(MyGUI::UString("Open rig file"), MyGUI::UString("Open"), false);
	m_gui_open_save_file_dialog->eventEndDialog = MyGUI::newDelegate(this, &Main::NotifyFileSelectorEnded);
	m_gui_open_save_file_dialog->doModal(); // Shows the dialog
}

void Main::CommandSaveRigFileAs()
{
	// TODO
}

void Main::CommandSaveRigFile()
{
	// TODO
}

void Main::NotifyFileSelectorEnded(GUI::Dialog* dialog, bool result)
{
	if (result)
	{
		// TODO
	}
	dialog->endModal(); // Hides the dialog
}
