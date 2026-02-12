// Mixer.h
#pragma once

struct ElevatorOutput {
    float left;
    float right;
};

class Mixer {
public:
    static ElevatorOutput mix(float pitchOutput, float rollOutput) {
        ElevatorOutput out;
        out.left  = pitchOutput + rollOutput;
        out.right = pitchOutput - rollOutput;
        return out;
    }
};
