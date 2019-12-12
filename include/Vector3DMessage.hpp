#pragma once
#include "DataMessage.hpp"
#include "Vector3D.hpp"

class Vector3DMessage : public DataMessage{

private:
    msg_type _type;
    Vector3D _data;

public:

    msg_type getType();
    const int getSize();
    Vector3D getData();
    
    Vector3DMessage();
    ~Vector3DMessage();

    void setVector3DMessage(Vector3D);
};