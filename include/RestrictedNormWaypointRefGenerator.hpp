#pragma once
#include "MsgEmitter.hpp"
#include "MsgReceiver.hpp"
#include "Vector3D.hpp"
#include "RestrictedNormRefSettingsMsg.hpp"
#include "PosesMsg.hpp"
#include <vector>
#include "ControlSystem.hpp"
#include "Waypoint.hpp"
#include "DoubleMsg.hpp"

class RestrictedNormWaypointRefGenerator : public MsgEmitter, public MsgReceiver{

    private:
    static pthread_mutex_t lock;
    std::vector<Waypoint> Waypoints;
    double max_norm = 0.2;
    bool enabled=false;
    ControlSystem* x_control_system;
    ControlSystem* y_control_system;
    ControlSystem* z_control_system;
    ControlSystem* yaw_control_system;
    int old_size = 0;
    void updateControlSystemsReferences(Vector3D<double> position,double yaw);
public:
    
    void add_x_control_system(ControlSystem*);
    void add_y_control_system(ControlSystem*);
    void add_z_control_system(ControlSystem*);
    void add_yaw_control_system(ControlSystem*);
    void receiveMsgData(DataMessage* t_msg);
    void receiveMsgData(DataMessage* t_msg, int t_channel);
};