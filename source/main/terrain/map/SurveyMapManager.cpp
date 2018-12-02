/*
    This source file is part of Rigs of Rods
    Copyright 2005-2012 Pierre-Michel Ricordel
    Copyright 2007-2012 Thomas Fischer

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



#include "SurveyMapManager.h"

#include "Application.h"
#include "Beam.h"
#include "GUIManager.h"
#include "GUI_MainSelector.h"
#include "InputEngine.h"
#include "OgreSubsystem.h"
#include "RoRFrameListener.h"
#include "SurveyMapEntity.h"
#include "SurveyMapTextureCreator.h"

using namespace RoR;
using namespace Ogre;

SurveyMapManager::SurveyMapManager(Ogre::Vector2 terrain_size) :
      mMapCenter(terrain_size / 2)
    , mLastMapMode(SurveyMapMode::SMALL)
    , mMapMode(SurveyMapMode::NONE)
    , mMapSize(terrain_size)
    , mMapTextureCreator(new SurveyMapTextureCreator(terrain_size))
    , mMapZoom(0.0f)
    , mPlayerPosition(terrain_size / 2)
    , mTerrainSize(terrain_size)
{
    initialiseByAttributes(this);
    mMainWidget->setVisible(false);

    mMapTextureCreator->init();
    mMapTextureCreator->update(mMapCenter, mMapSize);
    mMapTexture->setImageTexture(mMapTextureCreator->getTextureName());
}

SurveyMapManager::~SurveyMapManager()
{
    for (auto entity : mMapEntities)
    {
        if (entity)
        {
            delete entity;
        }
    }
}

SurveyMapEntity* SurveyMapManager::createMapEntity(String type)
{
    auto entity = new SurveyMapEntity(type, mMapTexture);
    mMapEntities.insert(entity);
    return entity;
}

void SurveyMapManager::deleteMapEntity(SurveyMapEntity* entity)
{
    if (entity)
    {
        mMapEntities.erase(entity);
        delete entity;
    }
}

void SurveyMapManager::updateWindow()
{
    switch (mMapMode)
    {
    case SurveyMapMode::SMALL: setWindowPosition(1, -1, 0.30f); break;
    case SurveyMapMode::BIG:   setWindowPosition(0,  0, 0.98f); break;
    default:;
    }
    mMainWidget->setVisible(mMapMode != SurveyMapMode::NONE);
}

void SurveyMapManager::windowResized()
{
    this->updateWindow();
}

void SurveyMapManager::setMapZoom(Real zoom)
{
    zoom = Math::Clamp(zoom, 0.0f, std::max(0.0f, (mTerrainSize.x - 50.0f) / mTerrainSize.x));

    if (mMapZoom == zoom)
        return;

    mMapZoom = zoom;
    updateMap();
}

void SurveyMapManager::setMapZoomRelative(Real delta)
{
    setMapZoom(mMapZoom + 0.5f * delta * std::max(0.1f, 1.0f - mMapZoom));
}

void SurveyMapManager::updateMap()
{
    mMapSize = mTerrainSize * (1.0f - mMapZoom);

    mMapCenter.x = Math::Clamp(mPlayerPosition.x, mMapSize.x / 2, mTerrainSize.x - mMapSize.x / 2);
    mMapCenter.y = Math::Clamp(mPlayerPosition.y, mMapSize.y / 2, mTerrainSize.y - mMapSize.y / 2);

    mMapTextureCreator->update(mMapCenter, mMapSize);
}

void SurveyMapManager::setPlayerPosition(Ogre::Vector2 position)
{
    if (mPlayerPosition.distance(position) < (1.0f - mMapZoom))
        return;

    mPlayerPosition = position;
    updateMap();
}

void SurveyMapManager::setWindowPosition(int x, int y, float size)
{
    int rWinLeft, rWinTop;
    unsigned int rWinWidth, rWinHeight, rWinDepth;
    App::GetOgreSubsystem()->GetRenderWindow()->getMetrics(rWinWidth, rWinHeight, rWinDepth, rWinLeft, rWinTop);

    int realx = 0;
    int realy = 0;
    int realw = size * std::min(rWinWidth, rWinHeight);
    int realh = size * std::min(rWinWidth, rWinHeight);

    if (x == -1)
    {
        realx = 0;
    }
    else if (x == 0)
    {
        realx = (rWinWidth - realw) / 2;
    }
    else if (x == 1)
    {
        realx = rWinWidth - realw;
    }

    if (y == -1)
    {
        realy = 0;
    }
    else if (y == 0)
    {
        realy = (rWinHeight - realh) / 2;
    }
    else if (y == 1)
    {
        realy = rWinHeight - realh;
    }

    mMainWidget->setCoord(realx, realy, realw, realh);
}

Ogre::String SurveyMapManager::getTypeByDriveable(int driveable)
{
    switch (driveable)
    {
    case NOT_DRIVEABLE:
        return "load";
    case TRUCK:
        return "truck";
    case AIRPLANE:
        return "airplane";
    case BOAT:
        return "boat";
    case MACHINE:
        return "machine";
    default:
        return "unknown";
    }
}

void SurveyMapManager::Update(Ogre::Real dt, Actor* curr_truck)
{
    if (dt == 0)
        return;

    if (App::GetInputEngine()->getEventBoolValueBounce(EV_SURVEY_MAP_CYCLE))
        cycleMode();

    if (App::GetInputEngine()->getEventBoolValueBounce(EV_SURVEY_MAP_TOGGLE))
        toggleMode();

    if (mMapMode == SurveyMapMode::NONE)
        return;

    switch (mMapMode)
    {
    case SurveyMapMode::SMALL:
        if (App::GetInputEngine()->getEventBoolValue(EV_SURVEY_MAP_ZOOM_IN))
        {
            setMapZoomRelative(+dt);
        }
        if (App::GetInputEngine()->getEventBoolValue(EV_SURVEY_MAP_ZOOM_OUT))
        {
            setMapZoomRelative(-dt);
        }
        if (mMapZoom > 0.0f)
        {
            if (curr_truck)
            {
                auto& simbuf = curr_truck->GetGfxActor()->GetSimDataBuffer();
                setPlayerPosition(Vector2(simbuf.simbuf_pos.x, simbuf.simbuf_pos.z));
            }
            else
            {
                auto& simbuf = App::GetSimController()->GetGfxScene().GetSimDataBuffer();
                setPlayerPosition(Vector2(simbuf.simbuf_character_pos.x, simbuf.simbuf_character_pos.z));
            }
        }
        else
        {
            setPlayerPosition(mTerrainSize / 2);
        }
        break;

    case SurveyMapMode::BIG:
        setMapZoom(0.0f);
        setPlayerPosition(mTerrainSize / 2);
        if (App::GetSimController()->AreControlsLocked() || App::GetGuiManager()->GetMainSelector()->IsVisible())
        {
            toggleMode();
        }
        break;

    default:
        break;
    }
}

void SurveyMapManager::cycleMode()
{
    switch (mMapMode)
    {
    case SurveyMapMode::NONE:  mLastMapMode = mMapMode = SurveyMapMode::SMALL; break;
    case SurveyMapMode::SMALL: mLastMapMode = mMapMode = SurveyMapMode::BIG;   break;
    case SurveyMapMode::BIG:                  mMapMode = SurveyMapMode::NONE;  break;
    default:;
    }
    updateWindow();
}

void SurveyMapManager::toggleMode()
{
    mMapMode = (mMapMode == SurveyMapMode::NONE) ? mLastMapMode : SurveyMapMode::NONE;
    updateWindow();
}

void SurveyMapManager::UpdateMapEntity(SurveyMapEntity* e, String caption, Vector3 pos, float rot, int state, bool visible)
{
    if (e)
    {
        Vector2 origin = mMapCenter - mMapSize / 2;
        Vector2 relPos = Vector2(pos.x, pos.z) - origin;
        bool culled = !(Vector2(0) < relPos && relPos < mMapSize);
        e->setState(state);
        e->setRotation(rot);
        e->setCaption(caption);
        e->setPosition(relPos.x / mMapSize.x, relPos.y / mMapSize.y);
        e->setVisibility(visible && !culled);
    }
}
