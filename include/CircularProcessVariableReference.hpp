#pragma once
#include "Reference.hpp"
#include "FloatMsg.hpp"
#include "Vector3D.hpp"
#include "Vector3DMessage.hpp"
#include <atomic>

class CircularProcessVariableReference : public Reference{

private:
    reference_type _reference_type;
    std::atomic<float> _reference_value; // TODO-Chehadeh: make thread-safe
    Vector3DMessage m_error_msg;
    block_id _id;

public:
    reference_type getReferenceType();
      
    void setReferenceValue(float);
    DataMessage* runTask(DataMessage*);
    block_id getID(){ return _id; }
    CircularProcessVariableReference(block_id t_id);
    ~CircularProcessVariableReference();
};