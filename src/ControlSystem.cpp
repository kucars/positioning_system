#include "ControlSystem.hpp"
#include <fstream>

ControlSystem::ControlSystem(control_system t_control_system, block_frequency t_bf) {
    _control_system = t_control_system;
    
    controllerSwitcher = new Switcher(switcher_type::controller);
    referenceSwitcher = new Switcher(switcher_type::reference);
    _switchers = {controllerSwitcher, referenceSwitcher};
    _frequency = t_bf;
    _dt = 1.0f / (int)_frequency;

    this->addCallbackMsgReceiver((MsgReceiver*)controllerSwitcher, ControlSystem::unicast_addresses::unicast_controller_switcher);
    this->addCallbackMsgReceiver((MsgReceiver*)referenceSwitcher, ControlSystem::unicast_addresses::unicast_reference_switcher);
    referenceSwitcher->addCallbackMsgReceiver((MsgReceiver*)controllerSwitcher, Switcher::unicast_addresses::unicast_controller_switcher);
    controllerSwitcher->addCallbackMsgReceiver((MsgReceiver*)this, Switcher::unicast_addresses::unicast_control_system);
}

ControlSystem::~ControlSystem() {

}

void ControlSystem::receiveMsgData(DataMessage* t_msg){
    if(t_msg->getType() == msg_type::SWITCHBLOCK){
        this->emitMsgUnicast((DataMessage*) t_msg, ControlSystem::unicast_addresses::unicast_controller_switcher);
        this->emitMsgUnicast((DataMessage*) t_msg, ControlSystem::unicast_addresses::unicast_reference_switcher);
    }
}

void ControlSystem::receiveMsgData(DataMessage* t_msg, int t_channel){
   
    if(t_msg->getType() == msg_type::VECTOR3D){
        Vector3DMessage* provider = (Vector3DMessage*)t_msg;
        this->emitMsgUnicast((DataMessage*) provider, 
                            ControlSystem::unicast_addresses::unicast_reference_switcher,
                            Switcher::receiving_channels::ch_provider);
    }else if(t_msg->getType() == msg_type::FLOAT){
        FloatMsg* float_msg = (FloatMsg*)t_msg;

        if(t_channel == (int)ControlSystem::receiving_channels::ch_reference){
            this->emitMsgUnicast((DataMessage*) float_msg,
                                ControlSystem::unicast_addresses::unicast_reference_switcher,
                                Switcher::receiving_channels::ch_reference);

            reference_ros_msg.data = {0, 0};
            reference_ros_msg.data[0] = (double)(int)(this->getControlSystemType());
            reference_ros_msg.data[1] = (double)(float_msg->data);
            this->emitMsgUnicastDefault((DataMessage*) &reference_ros_msg,
                                        ROSUnit_BroadcastData::ros_broadcast_channels::references);

        }else if(t_channel == (int)ControlSystem::receiving_channels::ch_controller){
            this->emitMsgUnicast((DataMessage*) float_msg,
                                ControlSystem::unicast_addresses::unicast_control_system,
                                ControlSystem::receiving_channels::ch_reference);
            this->emitMsgUnicast((DataMessage*) float_msg,
                                ControlSystem::unicast_addresses::unicast_actuation_system);

            controller_ros_msg.data = {0, 0};
            controller_ros_msg.data[0] = (double)(int)(this->getControlSystemType());
            controller_ros_msg.data[1] = (double)(float_msg->data);
            this->emitMsgUnicastDefault((DataMessage*) &controller_ros_msg,
                                        ROSUnit_BroadcastData::ros_broadcast_channels::control_outputs);
        }

    }
}

control_system ControlSystem::getControlSystemType(){
    return _control_system;
}
//TODO remove
void ControlSystem::getStatus(){
    
    for(Switcher* s : _switchers){
        if(s->getActiveBlock() != nullptr){
            std::cout << "For Control System " << static_cast<int>(_control_system) << std::endl;
            std::cout << "For switcher " << (int)(s->getType()) << " the active block is " << (int)(s->getActiveBlock()->getID()) << std::endl;
        }     
    }
}

void ControlSystem::addBlock(Block* t_block){
    ControlSystemMessage m_add_block_msg;
    m_add_block_msg.setControlSystemMessage(control_system_msg_type::add_block, t_block);
    
    if(t_block->getType() == block_type::controller){
        this->emitMsgUnicast((DataMessage*) &m_add_block_msg, ControlSystem::unicast_addresses::unicast_controller_switcher);
    }else if(t_block->getType() == block_type::reference){
        this->emitMsgUnicast((DataMessage*) &m_add_block_msg, ControlSystem::unicast_addresses::unicast_reference_switcher);
    }
}
