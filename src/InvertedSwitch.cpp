#include "InvertedSwitch.hpp"
#include "common_srv/FloatMsg.hpp"

InvertedSwitch::InvertedSwitch(std::function<bool(float,float)> t_operation, float t_trigger_value) {
    this->_operation = t_operation;
    this->_trigger_value = t_trigger_value;

    this->_input_port_0 = new InputPort(ports_id::IP_0_DATA, this);
    this->_trigger_port = new InputPort(ports_id::IP_1_TRIGGER, this);
    this->_input_port_1 = new InputPort(ports_id::IP_2_DATA, this);
    this->_output_port = new OutputPort(ports_id::OP_0_DATA, this);
    this->_active_input_port = _input_port_0;
    _ports = {_input_port_0, _trigger_port, _input_port_1, _output_port};
}

InvertedSwitch::~InvertedSwitch() {

}

void InvertedSwitch::triggerCallback(float t_current_value){

    if (this->_operation(t_current_value, _trigger_value)){
        _active_input_port = _output_port_1;
    }else{
        _active_input_port = _output_port_0;
    }
}

DataMessage* InvertedSwitch::runTask(DataMessage* t_msg){
    _active_input_port->receiveMsgData(t_msg);
    return t_msg;
}

void InvertedSwitch::process(DataMessage* t_msg, Port* t_port) {
    
    if(t_port->getID() == _active_input_port->getID()){
        std::cout << "I'm a InvertedSwitch" << std::endl;
        this->runTask(t_msg);

    }else if(t_port->getID() == ports_id::IP_1_TRIGGER){
        float data = 0.0;

        if(t_msg->getType() == msg_type::VECTOR3D){
            Vector3DMessage* vector_3D_msg = (Vector3DMessage*)t_msg;
            data = vector_3D_msg->getData().x; //TODO check if Z comes from a Vector3D
        }

        this->triggerCallback(data) ;
    }
}

std::vector<Port*> InvertedSwitch::getPorts(){
    return _ports;
}