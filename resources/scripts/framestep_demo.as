// L_* literals are generated by RoR

CVar@ G_sim_gearbox_mode = null;
CVar@ G_sim_terrain_name = null;
CVar@ G_gfx_enable_videocams = null;

bool  G_show_simbuffer_window = false;

Ogre::TexturePtr G_icon_info;
Ogre::TexturePtr G_icon_warn;
Ogre::TexturePtr G_icon_err;

Ogre::TexturePtr G_icon_arrow;
float            G_icon_arrow_rot = 0; // Radian

int setup(string arg)
{
    @G_sim_gearbox_mode     = RoR::GetConsole().CVarFind("sim_gearbox_mode");
    @G_sim_terrain_name     = RoR::GetConsole().CVarFind("sim_terrain_name");
    @G_gfx_enable_videocams = RoR::GetConsole().CVarFind("gfx_enable_videocams");
    
    G_icon_info  = Ogre::TextureManager::getSingleton().load("information.png", "IconsRG");
    G_icon_warn  = Ogre::TextureManager::getSingleton().load("error.png",       "IconsRG");
    G_icon_err   = Ogre::TextureManager::getSingleton().load("cancel.png",      "IconsRG");
    G_icon_arrow = Ogre::TextureManager::getSingleton().load("arrow_up.png",    "IconsRG");
    return 0;
}

string Vector3Str(vector3 v)
{
    return "{X: " + v.x + ", Y: " + v.y + ", Z: " + v.z + "}";
}

void DrawSimBufferWindow(ActorSimBuffer@ data)
{
    ImGui::SetNextWindowSize(vector2(550, 550));
    ImGui::Begin("ActorSimBuffer", true);
    ImGui::Columns(3);
    ImGui::SetColumnOffset(1, 70);
    ImGui::SetColumnOffset(2, 250);
    
    ImGui::Text("vector3"); ImGui::NextColumn(); ImGui::Text("pos                    "); ImGui::NextColumn(); ImGui::Text("" + Vector3Str(data.pos)        ); ImGui::NextColumn();
    ImGui::Text("vector3"); ImGui::NextColumn(); ImGui::Text("node0_velo             "); ImGui::NextColumn(); ImGui::Text("" + Vector3Str(data.node0_velo) ); ImGui::NextColumn();
    ImGui::Text("bool   "); ImGui::NextColumn(); ImGui::Text("live_local             "); ImGui::NextColumn(); ImGui::Text("" + data.live_local             ); ImGui::NextColumn();
    ImGui::Text("bool   "); ImGui::NextColumn(); ImGui::Text("physics_paused         "); ImGui::NextColumn(); ImGui::Text("" + data.physics_paused         ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("rotation               "); ImGui::NextColumn(); ImGui::Text("" + data.rotation               ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("tyre_pressure          "); ImGui::NextColumn(); ImGui::Text("" + data.tyre_pressure          ); ImGui::NextColumn();
    ImGui::Text("string "); ImGui::NextColumn(); ImGui::Text("net_username           "); ImGui::NextColumn(); ImGui::Text("" + data.net_username           ); ImGui::NextColumn();
    ImGui::Text("bool   "); ImGui::NextColumn(); ImGui::Text("is_remote              "); ImGui::NextColumn(); ImGui::Text("" + data.is_remote              ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("wheel_speed            "); ImGui::NextColumn(); ImGui::Text("" + data.wheel_speed            ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("hydro_dir_state        "); ImGui::NextColumn(); ImGui::Text("" + data.hydro_dir_state        ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("hydro_aileron_state    "); ImGui::NextColumn(); ImGui::Text("" + data.hydro_aileron_state    ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("hydro_elevator_state   "); ImGui::NextColumn(); ImGui::Text("" + data.hydro_elevator_state   ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("hydro_aero_rudder_state"); ImGui::NextColumn(); ImGui::Text("" + data.hydro_aero_rudder_state); ImGui::NextColumn();
    ImGui::Text("int    "); ImGui::NextColumn(); ImGui::Text("cur_cinecam            "); ImGui::NextColumn(); ImGui::Text("" + data.cur_cinecam            ); ImGui::NextColumn();
    ImGui::Text("bool   "); ImGui::NextColumn(); ImGui::Text("parking_brake          "); ImGui::NextColumn(); ImGui::Text("" + data.parking_brake          ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("brake                  "); ImGui::NextColumn(); ImGui::Text("" + data.brake                  ); ImGui::NextColumn();
    ImGui::Text("int    "); ImGui::NextColumn(); ImGui::Text("aero_flap_state        "); ImGui::NextColumn(); ImGui::Text("" + data.aero_flap_state        ); ImGui::NextColumn();
    ImGui::Text("int    "); ImGui::NextColumn(); ImGui::Text("airbrake_state         "); ImGui::NextColumn(); ImGui::Text("" + data.airbrake_state         ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("wing4_aoa              "); ImGui::NextColumn(); ImGui::Text("" + data.wing4_aoa              ); ImGui::NextColumn();
    ImGui::Text("vector3"); ImGui::NextColumn(); ImGui::Text("direction              "); ImGui::NextColumn(); ImGui::Text("" + Vector3Str(data.direction)  ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("top_speed              "); ImGui::NextColumn(); ImGui::Text("" + data.top_speed              ); ImGui::NextColumn();
    ImGui::Text("bool   "); ImGui::NextColumn(); ImGui::Text("hook_locked            "); ImGui::NextColumn(); ImGui::Text("" + data.hook_locked            ); ImGui::NextColumn();
    ImGui::Text("int    "); ImGui::NextColumn(); ImGui::Text("ties_secured_state     "); ImGui::NextColumn(); ImGui::Text("" + data.ties_secured_state     ); ImGui::NextColumn();
    ImGui::Text("bool   "); ImGui::NextColumn(); ImGui::Text("hydropump_ready        "); ImGui::NextColumn(); ImGui::Text("" + data.hydropump_ready        ); ImGui::NextColumn();
    ImGui::Text("int    "); ImGui::NextColumn(); ImGui::Text("tc_dashboard_mode      "); ImGui::NextColumn(); ImGui::Text("" + data.tc_dashboard_mode      ); ImGui::NextColumn();
    ImGui::Text("int    "); ImGui::NextColumn(); ImGui::Text("alb_dashboard_mode     "); ImGui::NextColumn(); ImGui::Text("" + data.alb_dashboard_mode     ); ImGui::NextColumn();
     // Lights
    ImGui::Text("bool   "); ImGui::NextColumn(); ImGui::Text("headlight_on           "); ImGui::NextColumn(); ImGui::Text("" + data.headlight_on           ); ImGui::NextColumn();
    ImGui::Text("bool   "); ImGui::NextColumn(); ImGui::Text("beaconlight_active     "); ImGui::NextColumn(); ImGui::Text("" + data.beaconlight_active     ); ImGui::NextColumn();
    ImGui::Text("bool   "); ImGui::NextColumn(); ImGui::Text("turn_signal_left       "); ImGui::NextColumn(); ImGui::Text("" + data.turn_signal_left       ); ImGui::NextColumn();
    ImGui::Text("bool   "); ImGui::NextColumn(); ImGui::Text("turn_signal_right      "); ImGui::NextColumn(); ImGui::Text("" + data.turn_signal_right      ); ImGui::NextColumn();
     // Autopilot
    ImGui::Text("int    "); ImGui::NextColumn(); ImGui::Text("ap_heading_mode        "); ImGui::NextColumn(); ImGui::Text("" + data.ap_heading_mode        ); ImGui::NextColumn();
    ImGui::Text("int    "); ImGui::NextColumn(); ImGui::Text("ap_heading_value       "); ImGui::NextColumn(); ImGui::Text("" + data.ap_heading_value       ); ImGui::NextColumn();
    ImGui::Text("int    "); ImGui::NextColumn(); ImGui::Text("ap_alt_mode            "); ImGui::NextColumn(); ImGui::Text("" + data.ap_alt_mode            ); ImGui::NextColumn();
    ImGui::Text("int    "); ImGui::NextColumn(); ImGui::Text("ap_alt_value           "); ImGui::NextColumn(); ImGui::Text("" + data.ap_alt_value           ); ImGui::NextColumn();
    ImGui::Text("bool   "); ImGui::NextColumn(); ImGui::Text("ap_ias_mode            "); ImGui::NextColumn(); ImGui::Text("" + data.ap_ias_mode            ); ImGui::NextColumn();
    ImGui::Text("int    "); ImGui::NextColumn(); ImGui::Text("ap_ias_value           "); ImGui::NextColumn(); ImGui::Text("" + data.ap_ias_value           ); ImGui::NextColumn();
    ImGui::Text("bool   "); ImGui::NextColumn(); ImGui::Text("ap_gpws_mode           "); ImGui::NextColumn(); ImGui::Text("" + data.ap_gpws_mode           ); ImGui::NextColumn();
    ImGui::Text("bool   "); ImGui::NextColumn(); ImGui::Text("ap_ils_available       "); ImGui::NextColumn(); ImGui::Text("" + data.ap_ils_available       ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("ap_ils_vdev            "); ImGui::NextColumn(); ImGui::Text("" + data.ap_ils_vdev            ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("ap_ils_hdev            "); ImGui::NextColumn(); ImGui::Text("" + data.ap_ils_hdev            ); ImGui::NextColumn();
    ImGui::Text("int    "); ImGui::NextColumn(); ImGui::Text("ap_vs_value            "); ImGui::NextColumn(); ImGui::Text("" + data.ap_vs_value            ); ImGui::NextColumn();
     // Engine & powertrain
    ImGui::Text("int    "); ImGui::NextColumn(); ImGui::Text("gear                   "); ImGui::NextColumn(); ImGui::Text("" + data.gear                   ); ImGui::NextColumn();
    ImGui::Text("int    "); ImGui::NextColumn(); ImGui::Text("autoshift              "); ImGui::NextColumn(); ImGui::Text("" + data.autoshift              ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("engine_rpm             "); ImGui::NextColumn(); ImGui::Text("" + data.engine_rpm             ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("engine_crankfactor     "); ImGui::NextColumn(); ImGui::Text("" + data.engine_crankfactor     ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("engine_turbo_psi       "); ImGui::NextColumn(); ImGui::Text("" + data.engine_turbo_psi       ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("engine_accel           "); ImGui::NextColumn(); ImGui::Text("" + data.engine_accel           ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("engine_torque          "); ImGui::NextColumn(); ImGui::Text("" + data.engine_torque          ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("inputshaft_rpm         "); ImGui::NextColumn(); ImGui::Text("" + data.inputshaft_rpm         ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("drive_ratio            "); ImGui::NextColumn(); ImGui::Text("" + data.drive_ratio            ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("clutch                 "); ImGui::NextColumn(); ImGui::Text("" + data.clutch                 ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("clutch_force           "); ImGui::NextColumn(); ImGui::Text("" + data.clutch_force           ); ImGui::NextColumn();
    ImGui::Text("float  "); ImGui::NextColumn(); ImGui::Text("clutch_torque          "); ImGui::NextColumn(); ImGui::Text("" + data.clutch_torque          ); ImGui::NextColumn();
    ImGui::Text("bool   "); ImGui::NextColumn(); ImGui::Text("engine_ignition        "); ImGui::NextColumn(); ImGui::Text("" + data.engine_ignition        ); ImGui::NextColumn();
    ImGui::Text("bool   "); ImGui::NextColumn(); ImGui::Text("engine_running         "); ImGui::NextColumn(); ImGui::Text("" + data.engine_running         ); ImGui::NextColumn();
    
    ImGui::Columns(1);
    ImGui::End();
}

int loop(GfxActor@ actor)
{
    string str;

    ImGui::SetNextWindowSize(vector2(440, 345));
    ImGui::Begin("Scripting test", true);
    ImGui::TextWrapped(L_main_welcome);
    
    ImGui::Separator(); ImGui::Text("SimBuffer:");
    
        if (ImGui::Button("open/close window"))
        {
            G_show_simbuffer_window = !G_show_simbuffer_window;
        }
    
    ImGui::Separator(); ImGui::Text("CVars:");
    
        str = "> gearbox mode:" + G_sim_gearbox_mode.GetActiveInt();
        ImGui::Text(str);
        
        str = "> terrain name:" + G_sim_terrain_name.GetActiveStr();
        ImGui::Text(str);
        
        str = "> videocams on:" + G_gfx_enable_videocams.GetActiveBool();
        ImGui::Text(str);
        
    ImGui::Separator(); ImGui::Text("Images:");

        ImGui::Image(G_icon_info, vector2(16, 16));
        ImGui::SameLine();
        ImGui::Image(G_icon_warn, vector2(16, 16));
        ImGui::SameLine();
        ImGui::Image(G_icon_err, vector2(16, 16));
        
    ImGui::Separator(); ImGui::Text("Rotated image:");
        
        // *DrawList* functions use screen coordinates and don't update window cursor
        ImGuiEx::DrawListAddImageRotated(G_icon_arrow, ImGui::GetCursorScreenPos() + vector2(16, 16), vector2(32, 32), G_icon_arrow_rot);
        ImGui::SetCursorPosX(50); // Make space for the icon
        ImGui::PushItemWidth(100);
        ImGui::SliderFloat("Rotation", G_icon_arrow_rot, 0, 3.14 * 2);
        ImGui::PopItemWidth();

    ImGui::End();
    
    if (G_show_simbuffer_window)
    {
        ActorSimBuffer@ data = actor.GetSimDataBuffer();
        DrawSimBufferWindow(data);
    }
    return 0;
}
