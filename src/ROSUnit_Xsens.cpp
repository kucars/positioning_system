#include "ROSUnit_Xsens.hpp"
#include <iostream>
#include <fstream>
ROSUnit_Xsens* ROSUnit_Xsens::_instance_ptr = NULL;
Timer ROSUnit_Xsens::t_pedro;
ButterFilter_Xsens ROSUnit_Xsens::filter_gyro_x;
ButterFilter_Xsens ROSUnit_Xsens::filter_gyro_y;
ButterFilter_Xsens ROSUnit_Xsens::filter_gyro_z;
Port* ROSUnit_Xsens::_output_port_0;
Port* ROSUnit_Xsens::_output_port_1;
Port* ROSUnit_Xsens::_output_port_2;
Port* ROSUnit_Xsens::_output_port_3;
Port* ROSUnit_Xsens::_output_port_4;

ROSUnit_Xsens::ROSUnit_Xsens(ros::NodeHandle& t_main_handler) : ROSUnit(t_main_handler){
    _sub_attitude = t_main_handler.subscribe("filter/quaternion", 2, callbackXsensAttitude, ros::TransportHints().tcpNoDelay());
    _sub_body_rate = t_main_handler.subscribe("imu/angular_velocity", 2, callbackXsensBodyRate, ros::TransportHints().tcpNoDelay());
    _sub_acceleration = t_main_handler.subscribe("filter/free_acceleration", 2, callbackXsensFreeAcceleration, ros::TransportHints().tcpNoDelay());
    _instance_ptr = this;

    _output_port_0 = new OutputPort(ports_id::OP_0_ROLL, this);
    _output_port_1 = new OutputPort(ports_id::OP_1_PITCH, this);
    _output_port_2 = new OutputPort(ports_id::OP_2_ROLL_RATE, this);
    _output_port_3 = new OutputPort(ports_id::OP_3_PITCH_RATE, this);
    _output_port_4 = new OutputPort(ports_id::OP_4_YAW_RATE, this);
    _ports = {_output_port_0, _output_port_1, _output_port_2, _output_port_3, _output_port_4};
}

ROSUnit_Xsens::~ROSUnit_Xsens() {

}

void ROSUnit_Xsens::process(DataMessage* t_msg, Port* t_port){

}

std::vector<Port*> ROSUnit_Xsens::getPorts(){
    return this->_ports;
}

DataMessage* ROSUnit_Xsens::runTask(DataMessage*){
    
}

void ROSUnit_Xsens::callbackXsensBodyRate(const geometry_msgs::Vector3Stamped& msg_bodyrate){
    
    Vector3DMessage pv_dot_msg;
    Vector3D<double> angular_vel;
    angular_vel.x = -1 * msg_bodyrate.vector.y;
    angular_vel.y = msg_bodyrate.vector.x;
    angular_vel.z = msg_bodyrate.vector.z;
    
    //FILTERING
    angular_vel.x = filter_gyro_x.perform(angular_vel.x);
    angular_vel.y = filter_gyro_y.perform(angular_vel.y);
    angular_vel.z = filter_gyro_z.perform(angular_vel.z);

    pv_dot_msg.setVector3DMessage(angular_vel);

    FloatMsg roll_rate, pitch_rate, yaw_rate;
    roll_rate.data = angular_vel.x;
    pitch_rate.data = angular_vel.y;
    yaw_rate.data = angular_vel.z;

    _instance_ptr->_output_port_4->receiveMsgData(&yaw_rate);
    _instance_ptr->_output_port_3->receiveMsgData(&pitch_rate);
    _instance_ptr->_output_port_2->receiveMsgData(&roll_rate);
}


void ROSUnit_Xsens::callbackXsensFreeAcceleration(const geometry_msgs::Vector3Stamped& msg_free_acceleration){

    Vector3DMessage pv_dot_dot_msg;
    Vector3D<double> free_acceleration;
    free_acceleration.x = -1 * msg_free_acceleration.vector.y;
    free_acceleration.y = msg_free_acceleration.vector.x;
    free_acceleration.z = msg_free_acceleration.vector.z;

    //FILTERING
    // Vector3D<double> filter_vel;
    // free_acceleration.x = filter_gyro_x.perform(free_acceleration.x);
    // free_acceleration.y = filter_gyro_y.perform(free_acceleration.y);
    // free_acceleration.z = filter_gyro_z.perform(free_acceleration.z);

    pv_dot_dot_msg.setVector3DMessage(free_acceleration);

	//_instance_ptr->emitMsgUnicast((DataMessage*) &pv_dot_dot_msg,(int)ROSUnit_Xsens::unicast_addresses::unicast_XSens_acceleration, (int)PVConcatenator::receiving_channels::ch_pv_dot_dot);
   
}

void ROSUnit_Xsens::callbackXsensAttitude( const geometry_msgs::QuaternionStamped& msg_attitude){

    Vector3DMessage pv_msg;
    Quaternion att_data;
    att_data.x = msg_attitude.quaternion.x;
    att_data.y = msg_attitude.quaternion.y;
    att_data.z = msg_attitude.quaternion.z;
    att_data.w = msg_attitude.quaternion.w;

//Convert Quaternion to euler
//TODO move to outside ROSUnit
    Vector3D<float> _euler;

    double sinr_cosp = +2.0 * ( att_data.w * att_data.x + att_data.y * att_data.z);
    double cosr_cosp = +1.0 - 2.0 * (att_data.x * att_data.x + att_data.y * att_data.y);
    _euler.x = atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    double sinp = +2.0 * ( att_data.w * att_data.y - att_data.z * att_data.x);
    if (fabs(sinp) >= 1)
        _euler.y = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
    else
        _euler.y = asin(sinp);

    // yaw (z-axis rotation)
    double siny_cosp = +2.0 * ( att_data.w * att_data.z + att_data.x * att_data.y);
    double cosy_cosp = +1.0 - 2.0 * (att_data.y * att_data.y + att_data.z * att_data.z );  
    _euler.z= atan2(siny_cosp, cosy_cosp);


    Vector3D<double> orientation_euler;
    orientation_euler.x = -1 * _euler.y;
    orientation_euler.y = _euler.x; //Arranging the frames to match with the drone's
    orientation_euler.z = _euler.z;


    pv_msg.setVector3DMessage(orientation_euler);
	_instance_ptr->emitMsgUnicast((DataMessage*) &pv_msg,(int)ROSUnit_Xsens::unicast_addresses::unicast_XSens_orientation, (int)PVConcatenator::receiving_channels::ch_pv);
	
    FloatMsg roll, pitch;
    roll.data = orientation_euler.x;
    pitch.data = orientation_euler.y;

    _instance_ptr->_output_port_1->receiveMsgData(&roll);
    _instance_ptr->_output_port_0->receiveMsgData(&pitch);
}

void ROSUnit_Xsens::callbackXsensVelocity(const geometry_msgs::TwistStamped& msg_velocity){
 
    Vector3D<double> velocity;
	Vector3DMessage velocity_msg;
    velocity.x = msg_velocity.twist.linear.x;
    velocity.y = msg_velocity.twist.linear.y;
    velocity.z = msg_velocity.twist.linear.z;
	velocity_msg.setVector3DMessage(velocity);


	_instance_ptr->emitMsgUnicast(&velocity_msg,(int)ROSUnit_Xsens::unicast_addresses::unicast_XSens_translation_rate,(int)Global2Inertial::receiving_channels::ch_XSens_vel);
		
}

void ROSUnit_Xsens::receiveMsgData(DataMessage*){

}