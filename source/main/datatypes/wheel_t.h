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

#pragma once

#include "RoRPrerequisites.h"
#include <OgrePrerequisites.h>

/**
* SIM-CORE; Wheel.
*/
struct wheel_t
{
    enum class BrakeCombo /// Wheels are braked by three mechanisms: A footbrake, a handbrake/parkingbrake, and directional brakes used for skidsteer steering.
    {
        NONE,                 /// - 0 = no  footbrake, no  handbrake, no  direction control -- wheel is unbraked
        FOOT_HAND,            /// - 1 = yes footbrake, yes handbrake, no  direction control
        FOOT_HAND_SKID_LEFT,  /// - 2 = yes footbrake, yes handbrake, yes direction control (braked when truck steers to the left)
        FOOT_HAND_SKID_RIGHT, /// - 3 = yes footbrake, yes handbrake, yes direction control (braked when truck steers to the right)
        FOOT_ONLY             /// - 4 = yes footbrake, no  handbrake, no  direction control -- footbrake only, such as with the front wheels of a passenger car
    };

    inline void UpdateGfxSkidmarkSlip(float slip_velo) // TODO: This shouldn't be here; refactor and move to GfxActor ~ only_a_ptr, 10/2017
    {
        const float SMOOTH_FACTOR = 0.7; // See https://en.wikipedia.org/wiki/Exponential_smoothing
        wfx_skidmark_slip = (slip_velo * SMOOTH_FACTOR) + (wfx_skidmark_slip * (1.f - SMOOTH_FACTOR));
    }

    int         wh_num_nodes;
    node_t*     wh_nodes[50];             // TODO: remove limit, make this dyn-allocated ~ only_a_ptr, 08/2017
    BrakeCombo  wh_braking;
    node_t*     wh_arm_node;
    node_t*     wh_near_attach_node;
    node_t*     wh_axis_node_0;
    node_t*     wh_axis_node_1;
    int         wh_propulsed;             // TODO: add enum ~ only_a_ptr, 08/2017
    Ogre::Real  wh_radius;
    Ogre::Real  wh_speed;
    Ogre::Real  wh_last_speed;
    Ogre::Real  wh_avg_speed;
    Ogre::Real  wh_delta_rotation;    ///< Difference in wheel position
    float       wh_net_rp;
    float       wh_net_rp1;           //<! Networking; triple buffer
    float       wh_net_rp2;           //<! Networking; triple buffer
    float       wh_net_rp3;           //<! Networking; triple buffer
    float       wh_width;
    int         wh_detacher_group;
    bool        wh_is_detached;
    bool        wh_alb_first_lock; // anti lock brake

    node_t*         wfx_skidmark_node;    ///< GFX state: current node touching ground
    ground_model_t* wfx_skidmark_gm;      ///< GFX state: current ground surface in touch with wheel
    float           wfx_skidmark_slip;    ///< GFX state: current slipping velocity
    bool            wfx_skidmark_fresh;   ///< Helper flag: was there any contact in last physics tick?
};

/**
* SIM-CORE; Visual wheel.
*/
struct vwheel_t
{
    Flexable *fm;
    Ogre::SceneNode *cnode;
    bool meshwheel;
};
