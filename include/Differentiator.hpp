#pragma once
#include "MsgEmitter.hpp"
#include "MsgReceiver.hpp"
#include "UpdatePoseMessage.hpp"
#include "ROSMsg.hpp"
#include "cmath"
#include "ControlSystemMessage.hpp"
#include "Vector3DMessage.hpp"
#include "Timer.hpp"
#include "FloatMsg.hpp"

class Differentiator : public msg_emitter, public msg_receiver {

private:
    float _old_float_data;
    Vector3D<float> _old_vector3d_data;
    Timer timer;

public:
    void receive_msg_data(DataMessage*);
    void differentiate(float);
    void differentiate(Vector3D<float>);
    Differentiator();
    ~Differentiator();
};