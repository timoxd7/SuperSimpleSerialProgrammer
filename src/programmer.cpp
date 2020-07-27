#include "Arduino.h"

#define BUFF_SIZE 1024
#define STD_BT_SERIAL_BAUD 38400



void setup() {
    Serial.begin(9600);
    Serial1.begin(STD_BT_SERIAL_BAUD);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    Serial.println("Bluetooth Programmer Online!");
}

char serialBuffer[BUFF_SIZE];
uint16_t currentSize = 0;
uint16_t currentBTBaud = STD_BT_SERIAL_BAUD;

void parse(char*, uint16_t);
bool parseUInt(char* buffer, uint16_t size, uint16_t& returnVal);
bool getNumerical(char maybeNum, uint16_t& returnVal);

void loop() {
    if (Serial1.available()) {
        Serial.write(Serial1.read());
    }


    if (Serial.available()) {
        digitalWrite(LED_BUILTIN, HIGH);
        char currentChar = Serial.read();
        Serial.write(currentChar);

        if (currentSize == BUFF_SIZE) {
            Serial.println("Error: serialBuffer full! Clearing now.");
            currentSize = 0;
        }

        serialBuffer[currentSize++] = currentChar;

        bool parsed = false;

        if (currentSize >= 2) {
            if (serialBuffer[currentSize-1] == '\n' &&
                serialBuffer[currentSize-2] == '\r') {
                parse(serialBuffer, currentSize - 2);
                parsed = true;
            }
        }

        if (!parsed && (currentChar == '\n')) {
            parse(serialBuffer, currentSize - 1);
            parsed = true;
        }

        if (parsed) {
            currentSize = 0;
        }
    }
}

void parse(char* buffer, uint16_t size) {
    if (size >= 4) {
        if (buffer[0] == 'S' &&
            buffer[1] == 'E' &&
            buffer[2] == 'T') {

            uint16_t newBaud;
            if (parseUInt(&buffer[3], size - 3, newBaud)) {
                Serial1.flush();
                Serial1.begin(newBaud);
                currentBTBaud = newBaud;
            }

            digitalWrite(LED_BUILTIN, LOW);
            return;
        }
    }

    if (size >= 1) {
        if (buffer[0] == 'G') {
            Serial.print("Current set BT Baud: ");
            Serial.println(currentBTBaud);
            digitalWrite(LED_BUILTIN, LOW);
            return;
        }
    }

    // If no statement => Send to Bluetooth Module
    for (uint16_t i = 0; i < size; ++i)
        Serial1.write(buffer[i]);
    Serial1.print("\r\n");
    Serial1.flush();
    digitalWrite(LED_BUILTIN, LOW);
}

bool parseUInt(char* buffer, uint16_t size, uint16_t& returnVal) {
    uint16_t currentMulti = 1;
    returnVal = 0;

    for (uint16_t i = 0; i < size; ++i) {
        uint16_t currentNum;

        if (getNumerical(buffer[size - 1 - i], currentNum)) {
            returnVal += currentNum * currentMulti;
            currentMulti *= 10;
        } else return false;
    }

    return true;
}

bool getNumerical(char maybeNum, uint16_t& returnVal) {
    switch (maybeNum) {
        case '0':
            returnVal = 0;
            return true;
        case '1':
            returnVal = 1;
            return true;
        case '2':
            returnVal = 2;
            return true;
        case '3':
            returnVal = 3;
            return true;
        case '4':
            returnVal = 4;
            return true;
        case '5':
            returnVal = 5;
            return true;
        case '6':
            returnVal = 6;
            return true;
        case '7':
            returnVal = 7;
            return true;
        case '8':
            returnVal = 8;
            return true;
        case '9':
            returnVal = 9;
            return true;
    }

    return false;
}