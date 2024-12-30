#include "Arduino.h"

const int piezoSensorPin = 36;
const int greenLED = 12;
const int redLED = 27;
const int blueLED = 26;
const int yellowLED = 25;
const int reedSwitchPin = 16;
const int programButtonPin = 32;

const int threshold = 1;
const int maxKnocks = 10;
const int timeout = 3500;
int defaultPattern[maxKnocks] = {200, 400, 200}; 
int defaultPatternLength = 3;
int recordedPattern[maxKnocks];
int knockCount = 0;

unsigned long lastKnockTime = 0;
const float tolerance = 10; 
bool program = false;

void resetPattern();
bool detectKnock();
void recordPattern();
bool patternMatch();
void changeDefaultPattern();
void ledBlink(int pin, int times, int duration);

void setup() {
    Serial.begin(115200);
    pinMode(piezoSensorPin, INPUT);
    pinMode(greenLED, OUTPUT);
    pinMode(redLED, OUTPUT);
    pinMode(blueLED, OUTPUT);
    pinMode(yellowLED, OUTPUT);
    pinMode(reedSwitchPin, INPUT_PULLUP);
    pinMode(programButtonPin, INPUT_PULLUP);

    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, LOW);
    digitalWrite(blueLED, LOW);
}

void loop() {
    
    if (!digitalRead(programButtonPin) && !program) {
        Serial.println("Program button pressed");
        program = true;
        ledBlink(yellowLED, 3, 300);
    }

    if (detectKnock()) {
        Serial.println("Knock detected!");
        resetPattern();
        lastKnockTime = millis();
        recordPattern();

        if (program) {
            changeDefaultPattern();
            program = false;
            ledBlink(greenLED, 3, 300);
        } else if (patternMatch()) {
            ledBlink(greenLED, 1, 500);
            Serial.println("Pattern matched!");
        } else {
            ledBlink(redLED, 1, 500);
            Serial.println("Pattern mismatch!");
        }
    }

    if (digitalRead(reedSwitchPin) == LOW) {
        Serial.println("Door opened!");
        delay(500);
    }
}

void ledBlink(int pin, int times, int duration) {
    for (int i = 0; i < times; i++) {
        digitalWrite(pin, HIGH);
        delay(duration);
        digitalWrite(pin, LOW);
        delay(duration);
    }
}

void changeDefaultPattern() {
    for (int i = 0; i < maxKnocks; i++) {
        defaultPattern[i] = 0;
    }

    for (int i = 0; i < knockCount; i++) {
        defaultPattern[i] = recordedPattern[i];
    }

    defaultPatternLength = knockCount;
}

void resetPattern() {
    knockCount = 0;
    for (int i = 0; i < maxKnocks; i++) {
        recordedPattern[i] = 0;
    }
}

bool detectKnock() {
    int sensorValue = analogRead(piezoSensorPin);
    if (sensorValue > threshold) {
        ledBlink(blueLED, 1, 40);
        return true;
    }
    return false;
}

void recordPattern() {
    Serial.println("Recording pattern");
    unsigned long knockStart = millis();
    unsigned long ledBlinkStart = millis();
    bool ledState = false;
    digitalWrite(yellowLED, LOW);

    while (millis() - knockStart < timeout && knockCount < maxKnocks) {
        if (millis() - ledBlinkStart >= 150) {
            ledState = !ledState;
            digitalWrite(yellowLED, ledState ? HIGH : LOW);
            ledBlinkStart = millis();
        }

        if (detectKnock()) {
            unsigned long currentTime = millis();
            recordedPattern[knockCount] = currentTime - lastKnockTime;
            lastKnockTime = currentTime;
            Serial.printf("Knock %d recorded: %d ms\n", knockCount + 1, recordedPattern[knockCount]);
            knockCount++;
        }
    }

    digitalWrite(yellowLED, LOW);
    Serial.println("Recording done");
}


bool patternMatch() {
    Serial.println("\nPattern time comparison:");
    Serial.println("Idx | Default | Recorded");
    Serial.println("-------------------------");

    int maxLength = max(defaultPatternLength, knockCount);

    for (int i = 0; i < maxLength; i++) {
        int defaultVal = (i < defaultPatternLength) ? defaultPattern[i] : 0;
        int recordedVal = (i < knockCount) ? recordedPattern[i] : 0;

        Serial.printf("%3d | %5dms | %6dms\n", i + 1, defaultVal, recordedVal);
    }

    Serial.println("");

    if (knockCount != defaultPatternLength) {
        Serial.println("Pattern lengths do not match.");
        return false;
    }

    Serial.println("\nPattern timing ratio comparison:");

    int defaultTotal = 0, recordedTotal = 0;
    for (int i = 0; i < defaultPatternLength; i++) {
        defaultTotal += defaultPattern[i];
    }
    for (int i = 0; i < knockCount; i++) {
        recordedTotal += recordedPattern[i];
    }

    float normalizedDefault[maxKnocks];
    float normalizedRecorded[maxKnocks];
    for (int i = 0; i < defaultPatternLength; i++) {
        normalizedDefault[i] = (float)defaultPattern[i] / defaultTotal;
    }
    for (int i = 0; i < knockCount; i++) {
        normalizedRecorded[i] = (float)recordedPattern[i] / recordedTotal;
    }

    float matchScore = 0;
    float defaultRatioSum = 0;
    float recordedRatioSum = 0;
    Serial.printf("Idx | Default | Recorded | Diff\n");
    Serial.printf("------+---------+----------+------\n");
    for (int i = 0; i < knockCount; i++) {
        float diff = abs(normalizedDefault[i] - normalizedRecorded[i]);
        matchScore += diff;
        defaultRatioSum += normalizedDefault[i];

        Serial.printf("%3d | %7.3f | %8.3f | %4.3f\n", i + 1, normalizedDefault[i], normalizedRecorded[i], diff);
    }

    Serial.println("");

    float matchPercentage = 100.0 * ( 1.0 - matchScore/defaultRatioSum);
    Serial.printf("Match Percentage: %.2f%%\n", matchPercentage);

    if (matchPercentage >= 100-tolerance) { 
        return true;
    } else {
        return false;
    }
}
