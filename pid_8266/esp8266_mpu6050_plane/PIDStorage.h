#pragma once
#include <EEPROM.h>
#include "PIDManager.h"
#include <Arduino.h>

class PIDStorage {
public:
    static void begin() {
        EEPROM.begin(512); // allocate 512 bytes
    }

    static void load(PIDManager &pid) {
        // Check if EEPROM has been initialized (magic number)
        uint8_t flag = EEPROM.read(0);
        if (flag != 0x42) { // 0x42 = arbitrary "initialized" marker
            Serial.println("EEPROM empty, using default PID values");
            return; // leave defaults in PIDManager
        }

        // read each PID: 3 floats per PID, 4 PIDs
        float val;
        int addr = 1;

        // rollAnglePID
        EEPROM.get(addr, val); pid.rollAnglePID.setTunings(val, pid.rollAnglePID.getKi(), pid.rollAnglePID.getKd()); addr += sizeof(float);
        EEPROM.get(addr, val); pid.rollAnglePID.setTunings(pid.rollAnglePID.getKp(), val, pid.rollAnglePID.getKd()); addr += sizeof(float);
        EEPROM.get(addr, val); pid.rollAnglePID.setTunings(pid.rollAnglePID.getKp(), pid.rollAnglePID.getKi(), val); addr += sizeof(float);

        // pitchAnglePID
        EEPROM.get(addr, val); pid.pitchAnglePID.setTunings(val, pid.pitchAnglePID.getKi(), pid.pitchAnglePID.getKd()); addr += sizeof(float);
        EEPROM.get(addr, val); pid.pitchAnglePID.setTunings(pid.pitchAnglePID.getKp(), val, pid.pitchAnglePID.getKd()); addr += sizeof(float);
        EEPROM.get(addr, val); pid.pitchAnglePID.setTunings(pid.pitchAnglePID.getKp(), pid.pitchAnglePID.getKi(), val); addr += sizeof(float);

        // rollRatePID
        EEPROM.get(addr, val); pid.rollRatePID.setTunings(val, pid.rollRatePID.getKi(), pid.rollRatePID.getKd()); addr += sizeof(float);
        EEPROM.get(addr, val); pid.rollRatePID.setTunings(pid.rollRatePID.getKp(), val, pid.rollRatePID.getKd()); addr += sizeof(float);
        EEPROM.get(addr, val); pid.rollRatePID.setTunings(pid.rollRatePID.getKp(), pid.rollRatePID.getKi(), val); addr += sizeof(float);

        // pitchRatePID
        EEPROM.get(addr, val); pid.pitchRatePID.setTunings(val, pid.pitchRatePID.getKi(), pid.pitchRatePID.getKd()); addr += sizeof(float);
        EEPROM.get(addr, val); pid.pitchRatePID.setTunings(pid.pitchRatePID.getKp(), val, pid.pitchRatePID.getKd()); addr += sizeof(float);
        EEPROM.get(addr, val); pid.pitchRatePID.setTunings(pid.pitchRatePID.getKp(), pid.pitchRatePID.getKi(), val); addr += sizeof(float);

        Serial.println("PID values loaded from EEPROM");
    }

    static void save(PIDManager &pid) {
        int addr = 0;
        EEPROM.write(addr++, 0x42); // magic byte to mark as initialized

        // rollAnglePID
        EEPROM.put(addr, pid.rollAnglePID.getKp()); addr += sizeof(float);
        EEPROM.put(addr, pid.rollAnglePID.getKi()); addr += sizeof(float);
        EEPROM.put(addr, pid.rollAnglePID.getKd()); addr += sizeof(float);

        // pitchAnglePID
        EEPROM.put(addr, pid.pitchAnglePID.getKp()); addr += sizeof(float);
        EEPROM.put(addr, pid.pitchAnglePID.getKi()); addr += sizeof(float);
        EEPROM.put(addr, pid.pitchAnglePID.getKd()); addr += sizeof(float);

        // rollRatePID
        EEPROM.put(addr, pid.rollRatePID.getKp()); addr += sizeof(float);
        EEPROM.put(addr, pid.rollRatePID.getKi()); addr += sizeof(float);
        EEPROM.put(addr, pid.rollRatePID.getKd()); addr += sizeof(float);

        // pitchRatePID
        EEPROM.put(addr, pid.pitchRatePID.getKp()); addr += sizeof(float);
        EEPROM.put(addr, pid.pitchRatePID.getKi()); addr += sizeof(float);
        EEPROM.put(addr, pid.pitchRatePID.getKd()); addr += sizeof(float);

        EEPROM.commit();
        Serial.println("PID values saved to EEPROM");
    }
};
