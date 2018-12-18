/*
    This source file is part of Rigs of Rods
    Copyright 2005-2012 Pierre-Michel Ricordel
    Copyright 2007-2012 Thomas Fischer
    Copyright 2013-2018 Petr Ohlidal

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

#include "ContentManager.h"


#include <Overlay/OgreOverlayManager.h>
#include <Overlay/OgreOverlay.h>
#include <Plugins/ParticleFX/OgreBoxEmitterFactory.h>


#include "Application.h"
#include "Settings.h"
#include "ColoredTextAreaOverlayElementFactory.h"
#include "ErrorUtils.h"
#include "SoundScriptManager.h"
#include "SkinManager.h"
#include "Language.h"
#include "PlatformUtils.h"

#include "CacheSystem.h"

#include "OgreShaderParticleRenderer.h"

// Removed by Skybon as part of OGRE 1.9 port 
// Disabling temporarily for 1.8.1 as well. ~ only_a_ptr, 2015-11
// TODO: Study the system, then re-enable or remove entirely.
//#include "OgreBoxEmitterFactory.h"

#ifdef USE_ANGELSCRIPT
#include "FireExtinguisherAffectorFactory.h"
#include "ExtinguishableFireAffectorFactory.h"
#endif // USE_ANGELSCRIPT

#include "Utils.h"

#include <OgreFileSystem.h>
#include <regex>
#include <sstream>

using namespace Ogre;
using namespace std;
using namespace RoR;

// ================================================================================
// Static variables
// ================================================================================

#define DECLARE_RESOURCE_PACK(_FIELD_, _NAME_, _RESOURCE_GROUP_) \
    const ContentManager::ResourcePack ContentManager::ResourcePack::_FIELD_(_NAME_, _RESOURCE_GROUP_);

DECLARE_RESOURCE_PACK( OGRE_CORE,             "OgreCore",             "OgreCoreRG");
DECLARE_RESOURCE_PACK( WALLPAPERS,            "wallpapers",           "Wallpapers");
DECLARE_RESOURCE_PACK( AIRFOILS,              "airfoils",             "AirfoilsRG");
DECLARE_RESOURCE_PACK( CAELUM,                "caelum",               "CaelumRG");
DECLARE_RESOURCE_PACK( CUBEMAPS,              "cubemaps",             "CubemapsRG");
DECLARE_RESOURCE_PACK( DASHBOARDS,            "dashboards",           "DashboardsRG");
DECLARE_RESOURCE_PACK( FAMICONS,              "famicons",             "FamiconsRG");
DECLARE_RESOURCE_PACK( FLAGS,                 "flags",                "FlagsRG");
DECLARE_RESOURCE_PACK( HYDRAX,                "hydrax",               "HydraxRG");
DECLARE_RESOURCE_PACK( ICONS,                 "icons",                "IconsRG");
DECLARE_RESOURCE_PACK( MATERIALS,             "materials",            "MaterialsRG");
DECLARE_RESOURCE_PACK( MESHES,                "meshes",               "MeshesRG");
DECLARE_RESOURCE_PACK( MYGUI,                 "mygui",                "MyGuiRG");
DECLARE_RESOURCE_PACK( OVERLAYS,              "overlays",             "OverlaysRG");
DECLARE_RESOURCE_PACK( PAGED,                 "paged",                "PagedRG");
DECLARE_RESOURCE_PACK( PARTICLES,             "particles",            "ParticlesRG");
DECLARE_RESOURCE_PACK( PSSM,                  "pssm",                 "PssmRG");
DECLARE_RESOURCE_PACK( RTSHADER,              "rtshader",             "RtShaderRG");
DECLARE_RESOURCE_PACK( SCRIPTS,               "scripts",              "ScriptsRG");
DECLARE_RESOURCE_PACK( SOUNDS,                "sounds",               "SoundsRG");
DECLARE_RESOURCE_PACK( TEXTURES,              "textures",             "TexturesRG");
DECLARE_RESOURCE_PACK( SKYX,                  "SkyX",                 "SkyXRG");

// ================================================================================
// Functions
// ================================================================================

void ContentManager::AddResourcePack(ResourcePack const& resource_pack)
{
    Ogre::ResourceGroupManager& rgm = Ogre::ResourceGroupManager::getSingleton();

    if (rgm.resourceGroupExists(resource_pack.resource_group_name)) // Already loaded?
    {
        return; // Nothing to do, nothing to report
    }

    std::stringstream log_msg;
    log_msg << "[RoR|ContentManager] Loading resource pack \"" << resource_pack.name << "\" from group \"" << resource_pack.resource_group_name << "\"";
    Ogre::String resources_dir = Ogre::String(App::sys_resources_dir.GetActive()) + PATH_SLASH;
    Ogre::String zip_path = resources_dir + resource_pack.name + Ogre::String(".zip");
    if (FileExists(zip_path))
    {
        log_msg << " (ZIP archive)";
        LOG(log_msg.str());
        rgm.addResourceLocation(zip_path, "Zip", resource_pack.resource_group_name);
        rgm.initialiseResourceGroup(resource_pack.resource_group_name);
    }
    else
    {
        Ogre::String dir_path = resources_dir + resource_pack.name;
        if (FolderExists(dir_path))
        {
            log_msg << " (directory)";
            LOG(log_msg.str());
            ResourceGroupManager::getSingleton().addResourceLocation(dir_path, "FileSystem", resource_pack.resource_group_name);
            rgm.initialiseResourceGroup(resource_pack.resource_group_name);
        }
        else
        {
            log_msg << " failed, data not found.";
            throw std::runtime_error(log_msg.str());
        }
    }
}

void ContentManager::InitContentManager()
{
    // Initialize "managed materials" first
    //   These are base materials referenced by user content
    //   They must be initialized before any content is loaded, including mod-cache update.
    //   Otherwise material links are unresolved and loading ends with an exception
    // TODO: Study Ogre::ResourceLoadingListener and implement smarter solution (not parsing materials on cache refresh!)
    this->InitManagedMaterials();

    // set listener if none has already been set
    if (!Ogre::ResourceGroupManager::getSingleton().getLoadingListener())
        Ogre::ResourceGroupManager::getSingleton().setLoadingListener(this);

    // by default, display everything in the depth map
    Ogre::MovableObject::setDefaultVisibilityFlags(DEPTHMAP_ENABLED);


    this->AddResourcePack(ResourcePack::MYGUI);
    this->AddResourcePack(ResourcePack::DASHBOARDS);


#ifdef _WIN32
    // TODO: FIX UNDER LINUX!
    // register particle classes
    LOG("RoR|ContentManager: Registering Particle Box Emitter");
    ParticleSystemRendererFactory* mParticleSystemRendererFact = OGRE_NEW ShaderParticleRendererFactory();
    ParticleSystemManager::getSingleton().addRendererFactory(mParticleSystemRendererFact);

    // Removed by Skybon as part of OGRE 1.9 port 
    // Disabling temporarily for 1.8.1 as well.  ~ only_a_ptr, 2015-11
    //ParticleEmitterFactory *mParticleEmitterFact = OGRE_NEW BoxEmitterFactory();
    //ParticleSystemManager::getSingleton().addEmitterFactory(mParticleEmitterFact);

#endif // _WIN32

#ifdef USE_ANGELSCRIPT
    // FireExtinguisherAffector
    ParticleAffectorFactory* pAffFact = OGRE_NEW FireExtinguisherAffectorFactory();
    ParticleSystemManager::getSingleton().addAffectorFactory(pAffFact);

    // ExtinguishableFireAffector
    pAffFact = OGRE_NEW ExtinguishableFireAffectorFactory();
    ParticleSystemManager::getSingleton().addAffectorFactory(pAffFact);
#endif // USE_ANGELSCRIPT

    // sound is a bit special as we mark the base sounds so we don't clear them accidentally later on
#ifdef USE_OPENAL
    LOG("RoR|ContentManager: Creating Sound Manager");
    SoundScriptManager::getSingleton().setLoadingBaseSounds(true);
#endif // USE_OPENAL

    AddResourcePack(ResourcePack::SOUNDS);

    // streams path, to be processed later by the cache system
    LOG("RoR|ContentManager: Loading filesystems");

    // config, flat
    ResourceGroupManager::getSingleton().addResourceLocation(std::string(RoR::App::sys_config_dir.GetActive()), "FileSystem", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    // packs, to be processed later by the cache system

    // add scripts folder
    ResourceGroupManager::getSingleton().addResourceLocation(std::string(App::sys_user_dir.GetActive()) + PATH_SLASH + "scripts", "FileSystem", "Scripts");

    // init skin manager, important to happen before trucks resource loading!
    LOG("RoR|ContentManager: Registering Skin Manager");
    m_skin_manager = new RoR::SkinManager(); // SkinManager registers itself

    LOG("RoR|ContentManager: Registering colored text overlay factory");
    ColoredTextAreaOverlayElementFactory* pCT = new ColoredTextAreaOverlayElementFactory();
    OverlayManager::getSingleton().addOverlayElementFactory(pCT);

    // set default mipmap level (NB some APIs ignore this)
    if (TextureManager::getSingletonPtr())
        TextureManager::getSingleton().setDefaultNumMipmaps(5);

    TextureFilterOptions tfo = TFO_NONE;
    switch (App::gfx_texture_filter.GetActive())
    {
    case GfxTexFilter::ANISOTROPIC: tfo = TFO_ANISOTROPIC;        break;
    case GfxTexFilter::TRILINEAR:   tfo = TFO_TRILINEAR;          break;
    case GfxTexFilter::BILINEAR:    tfo = TFO_BILINEAR;           break;
    case GfxTexFilter::NONE:        tfo = TFO_NONE;               break;
    }
    MaterialManager::getSingleton().setDefaultAnisotropy(Math::Clamp(App::gfx_anisotropy.GetActive(), 1, 16));
    MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);

    // load all resources now, so the zip files are also initiated
    LOG("RoR|ContentManager: Calling initialiseAllResourceGroups()");
    try
    {
        if (BSETTING("Background Loading", false))
            ResourceBackgroundQueue::getSingleton().initialiseAllResourceGroups();
        else
            ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    }
    catch (Ogre::Exception& e)
    {
        LOG("RoR|ContentManager: catched error while initializing Resource groups: " + e.getFullDescription());
    }
#ifdef USE_OPENAL
    SoundScriptManager::getSingleton().setLoadingBaseSounds(false);
#endif // USE_OPENAL
}

void ContentManager::InitModCache()
{
    if (BSETTING("NOCACHE", false)) // TODO: Can you even use RoR with this? ~ only_a_ptr, 10/2018
    {
        LOG("Cache disabled via command line switch");
        return;
    }

    m_mod_cache.LoadCategoriesConfig();

    ResourceGroupManager::getSingleton().addResourceLocation(App::sys_cache_dir.GetActive(), "FileSystem", "cache", false, false);

    // and the content; TODO: Is this code necessary beyond modcache-generation/update? ~ only_a_ptr, 10/2018
    std::string user_content_base = std::string(App::sys_user_dir.GetActive())    + PATH_SLASH;
    std::string content_base      = std::string(App::sys_process_dir.GetActive()) + PATH_SLASH;

    ResourceGroupManager::getSingleton().addResourceLocation(content_base      + "content" , "FileSystem", "Packs", true);
    ResourceGroupManager::getSingleton().addResourceLocation(user_content_base + "packs"   , "FileSystem", "Packs", true);
    ResourceGroupManager::getSingleton().addResourceLocation(user_content_base + "mods"    , "FileSystem", "Packs", true);

    ResourceGroupManager::getSingleton().addResourceLocation(user_content_base + "vehicles", "FileSystem", "VehicleFolders");
    ResourceGroupManager::getSingleton().addResourceLocation(user_content_base + "terrains", "FileSystem", "TerrainFolders");

    exploreFolders("VehicleFolders"); //TODO: These traversals of 'resource bundles' (see CacheSystem doc) are duplicate with traversals during cache-update. Unify it! ~ only_a_ptr, 10/2018
    exploreFolders("TerrainFolders");
    exploreZipFolders("Packs"); // this is required for skins to work

    ResourceGroupManager::getSingleton().addResourceLocation(
        content_base + "resources" + PATH_SLASH + "beamobjects.zip", "Zip", "RoR_BeamObjects", true);
    exploreZipFolders("RoR_BeamObjects");

    LOG("RoR|ContentManager: Calling initialiseAllResourceGroups() - Content");
    try
    {
        if (BSETTING("Background Loading", false))
            ResourceBackgroundQueue::getSingleton().initialiseResourceGroup("Packs");
        else
            ResourceGroupManager::getSingleton().initialiseResourceGroup("Packs");
    }
    catch (Ogre::Exception& e)
    {
        LOG("RoR|ContentManager: catched error while initializing Content Resource groups: " + e.getFullDescription());
    }

    CacheSystem::CacheValidityState validity = m_mod_cache.EvaluateCacheValidity();
    m_mod_cache.LoadModCache(validity);
    App::SetCacheSystem(&m_mod_cache); // Temporary solution until Modcache+ContentManager are fully merged and `App::GetCacheSystem()` is removed ~ only_a_ptr, 10/2018
}

Ogre::DataStreamPtr ContentManager::resourceLoading(const Ogre::String& name, const Ogre::String& group, Ogre::Resource* resource)
{
    return Ogre::DataStreamPtr();
}

void ContentManager::resourceStreamOpened(const Ogre::String& name, const Ogre::String& group, Ogre::Resource* resource, Ogre::DataStreamPtr& dataStream)
{
}

bool ContentManager::resourceCollision(Ogre::Resource* resource, Ogre::ResourceManager* resourceManager)
{
    /*
    // TODO: do something useful here
    if (resourceManager->getResourceType() == "Material")
    {
        if (instanceCountMap.find(resource->getName()) == instanceCountMap.end())
        {
            instanceCountMap[resource->getName()] = 1;
        }
        int count = instanceCountMap[resource->getName()]++;
        MaterialPtr mat = (MaterialPtr)resourceManager->create(resource->getName() + TOSTRING(count), resource->getGroup());
        resource = (Ogre::Resource *)mat.getPointer();
        return true;
    }
    */
    return false;
}

void ContentManager::exploreZipFolders(Ogre::String rg)
{
    ResourceGroupManager& rgm = ResourceGroupManager::getSingleton();

    FileInfoListPtr files = rgm.findResourceFileInfo(rg, "*.skinzip"); //search for skins
    FileInfoList::iterator iterFiles = files->begin();
    for (; iterFiles != files->end(); ++iterFiles)
    {
        if (!iterFiles->archive)
            continue;
        String fullpath = iterFiles->archive->getName() + PATH_SLASH;
        rgm.addResourceLocation(fullpath + iterFiles->filename, "Zip", rg);
    }
    // DO NOT initialize ...
}

void ContentManager::exploreFolders(Ogre::String rg)
{
    ResourceGroupManager& rgm = ResourceGroupManager::getSingleton();

    FileInfoListPtr files = rgm.findResourceFileInfo(rg, "*", true); // searching for dirs
    FileInfoList::iterator iterFiles = files->begin();
    for (; iterFiles != files->end(); ++iterFiles)
    {
        if (!iterFiles->archive)
            continue;
        if (iterFiles->filename == String(".svn"))
            continue;
        // trying to get the full path
        String fullpath = iterFiles->archive->getName() + PATH_SLASH;
        rgm.addResourceLocation(fullpath + iterFiles->filename, "FileSystem", rg);
    }
    LOG("initialiseResourceGroups: "+rg);
    try
    {
        if (BSETTING("Background Loading", false))
            ResourceBackgroundQueue::getSingleton().initialiseResourceGroup(rg);
        else
            ResourceGroupManager::getSingleton().initialiseResourceGroup(rg);
    }
    catch (Ogre::Exception& e)
    {
        LOG("catched error while initializing Resource group '" + rg + "' : " + e.getFullDescription());
    }
}

void ContentManager::InitManagedMaterials()
{
    Ogre::String managed_materials_dir = Ogre::String(App::sys_resources_dir.GetActive()) + PATH_SLASH + "managed_materials" + PATH_SLASH;

    //Dirty, needs to be improved
    if (App::gfx_shadow_type.GetActive() == GfxShadowType::PSSM)
        ResourceGroupManager::getSingleton().addResourceLocation(managed_materials_dir + "shadows/pssm/on/", "FileSystem", "ShadowsMats");
    else
        ResourceGroupManager::getSingleton().addResourceLocation(managed_materials_dir + "shadows/pssm/off/", "FileSystem", "ShadowsMats");

    ResourceGroupManager::getSingleton().initialiseResourceGroup("ShadowsMats");

    ResourceGroupManager::getSingleton().addResourceLocation(managed_materials_dir + "texture/", "FileSystem", "TextureManager");
    ResourceGroupManager::getSingleton().initialiseResourceGroup("TextureManager");

    //Last
    ResourceGroupManager::getSingleton().addResourceLocation(managed_materials_dir, "FileSystem", "ManagedMats");
    ResourceGroupManager::getSingleton().initialiseResourceGroup("ManagedMats");
}

void ContentManager::LoadGameplayResources()
{
    if (!m_base_resource_loaded)
    {
        this->AddResourcePack(ContentManager::ResourcePack::AIRFOILS);
        this->AddResourcePack(ContentManager::ResourcePack::TEXTURES);
        this->AddResourcePack(ContentManager::ResourcePack::ICONS);
        this->AddResourcePack(ContentManager::ResourcePack::FAMICONS);
        this->AddResourcePack(ContentManager::ResourcePack::FLAGS);
        this->AddResourcePack(ContentManager::ResourcePack::MATERIALS);
        this->AddResourcePack(ContentManager::ResourcePack::MESHES);
        this->AddResourcePack(ContentManager::ResourcePack::OVERLAYS);
        this->AddResourcePack(ContentManager::ResourcePack::PARTICLES);
        this->AddResourcePack(ContentManager::ResourcePack::SCRIPTS);

        m_base_resource_loaded = true;
    }

    if (App::gfx_water_mode.GetActive() == GfxWaterMode::HYDRAX)
        this->AddResourcePack(ContentManager::ResourcePack::HYDRAX);

    if (App::gfx_sky_mode.GetActive() == GfxSkyMode::CAELUM)
        this->AddResourcePack(ContentManager::ResourcePack::CAELUM);

    if (App::gfx_sky_mode.GetActive() == GfxSkyMode::SKYX)
        this->AddResourcePack(ContentManager::ResourcePack::SKYX);

    if (App::gfx_vegetation_mode.GetActive() != RoR::GfxVegetation::NONE)
        this->AddResourcePack(ContentManager::ResourcePack::PAGED);}

std::string ContentManager::ListAllUserContent()
{
    // Define temporary OGRE resource group
    const Ogre::String RG_NAME = "RoR-Temp-ModCacheScan";
    Ogre::ResourceGroupManager& ogre_rgm = Ogre::ResourceGroupManager::getSingleton();

    // Add user content locations
    const Ogre::String homedir     = Ogre::String(App::sys_user_dir.GetActive())    + PATH_SLASH;
    const Ogre::String process_dir = Ogre::String(App::sys_process_dir.GetActive()) + PATH_SLASH;

    bool recursive = true; // For clarity
    ogre_rgm.addResourceLocation(process_dir  + "content" , "FileSystem", RG_NAME, recursive); // TODO: Does anybody use this one? ~ only_a_ptr, 10/2018
    ogre_rgm.addResourceLocation(homedir      + "packs"   , "FileSystem", RG_NAME, recursive);
    ogre_rgm.addResourceLocation(homedir      + "mods"    , "FileSystem", RG_NAME, recursive); // TODO: Does anybody use this one? ~ only_a_ptr, 10/2018

    ogre_rgm.addResourceLocation(homedir      + "vehicles", "FileSystem", RG_NAME, recursive);
    ogre_rgm.addResourceLocation(homedir      + "terrains", "FileSystem", RG_NAME, recursive);

    // Gather filenames
    std::stringstream buf;
    Ogre::FileInfoListPtr dir_list = ogre_rgm.listResourceFileInfo(RG_NAME, true); // true = List only directories
    for (auto dir: *dir_list)
    {
        buf << dir.filename << std::endl;
    }

    std::regex file_whitelist("^.\\.(airplane|boat|car|fixed|load|machine|terrn2|train|truck)$", std::regex::icase); // Any filename + listed extensions, ignore case
    Ogre::FileInfoListPtr file_list = ogre_rgm.listResourceFileInfo(RG_NAME, false); // false = List only files
    for (auto file: *file_list)
    {
        if ((file.archive != nullptr) || std::regex_match(file.filename, file_whitelist))
        {
            buf << file.filename << std::endl;
        }
    }

    // Cleanup
    ogre_rgm.destroyResourceGroup(RG_NAME);

    return buf.str();
}

