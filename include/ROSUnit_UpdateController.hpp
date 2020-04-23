#pragma once
#include "ROSUnit.hpp"
#include <geometry_msgs/PoseStamped.h>
#include "Vector3D.hpp"
#include "PID_values.hpp"
#include <flight_controller/PID_param.h>
#include <flight_controller/MRFT_param.h>
#include <flight_controller/SM_param.h>
#include <flight_controller/Update_Controller_PID.h>
#include <flight_controller/Update_Controller_MRFT.h>
#include <flight_controller/Update_Controller_SM.h>
#include "common_types.hpp"
#include "ControllerMessage.hpp"

class ROSUnit_UpdateController :  public ROSUnit{

    private:
        static control_system _id;
        static ROSUnit_UpdateController* _instance_ptr;
        static ControllerMessage _update_controller_msg; 
        ros::ServiceServer _srv_update_controller_pid;
        ros::ServiceServer _srv_update_controller_mrft;
        ros::ServiceServer _srv_update_controller_sm;

        static bool callbackUpdateControllerPID(flight_controller::Update_Controller_PID::Request  &req, 
                                             flight_controller::Update_Controller_PID::Response &res);
        static bool callbackUpdateControllerMRFT(flight_controller::Update_Controller_MRFT::Request  &req, 
                                             flight_controller::Update_Controller_MRFT::Response &res);
        static bool callbackUpdateControllerSM(flight_controller::Update_Controller_SM::Request  &req, 
                                             flight_controller::Update_Controller_SM::Response &res);

                                             
        void receiveMsgData(DataMessage* t_msg);  

    public:
        enum unicast_addresses {broadcast, pid, mrft, sm};
        ROSUnit_UpdateController(ros::NodeHandle&);
        ~ROSUnit_UpdateController();
};