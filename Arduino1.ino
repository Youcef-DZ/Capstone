/*
Name:		Arduino1.ino
Created:	2/7/2017 3:55:31 PM
Author:		Youcef Laidi
*/

#include <Servo.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

const unsigned long FREQUENCY = 60;
const unsigned long SERVOMIN = 650;
const unsigned long SERVOMAX = 2350;

const int numberOfButtons = 7;

const int resetBtnPin = 2;
const int actionBtnPin = 3;

const int pushBtnPin = 4;
const int pourBtnPin = 6;
const int pullDownBtnPin = 8;
const int okBtnPin = 10;
const int triggerBtnPin = 12;

const int ledPins[numberOfButtons] = { 5, 7, 9, 11, 13 };

const int inPins[numberOfButtons] = {
	pushBtnPin,
	pourBtnPin,
	pullDownBtnPin,
	okBtnPin,
	triggerBtnPin,
	resetBtnPin,
	actionBtnPin
};

Servo servo1;

enum OPTION {
	HIGH_5,
	TRIGGER,
	PULL_DOWN,
	POUR,
	PUSH,
	RESET,
	OK,
	COUNTING
};

OPTION SelectedOption = RESET;

void setup() {

	Serial.begin(9600);
	for (int i = 0; i < numberOfButtons; i++) {
		pinMode(ledPins[i], OUTPUT);
		pinMode(inPins[i], INPUT_PULLUP);
	}

	servo1.attach(53);

	pwm.begin();
	pwm.setPWMFreq(FREQUENCY);

	resetAll();
}

void loop() {

	for (int i = 0; i < numberOfButtons; i++) {
		int val = digitalRead(inPins[i]);

		if (val == LOW) {
			switch (inPins[i]) {
			case actionBtnPin:
				Serial.print("ACTION button pressed\n");
				doAction();
				break;
			case resetBtnPin:
				turnOtherLedsOff(resetBtnPin); // any number other than the ouput pins
				Serial.print("RESET button pressed\n");
				SelectedOption = RESET;
				resetAll();
				break;
			case pushBtnPin:
				digitalWrite(ledPins[i], HIGH);
				turnOtherLedsOff(i);
				Serial.print("PUSH button pressed\n");
				SelectedOption = PUSH;
				servo1.write(5);
				delay(500);
				break;
			case pullDownBtnPin:
				digitalWrite(ledPins[i], HIGH);
				turnOtherLedsOff(i);
				Serial.print("PULL DOWN button pressed\n");
				SelectedOption = PULL_DOWN;
				servo1.write(90);
				delay(500);
				break;
			case pourBtnPin:
				digitalWrite(ledPins[i], HIGH);
				turnOtherLedsOff(i);
				Serial.print("POUR button pressed\n");
				SelectedOption = COUNTING;
				servo1.write(90);
				delay(500);
				for (int i = 2; i < 6; i++) {
					turnTo(i, 95);
					delay(100);
				}
				turnTo(1, 130);
				delay(100);
				break;
			case okBtnPin:
				digitalWrite(ledPins[i], HIGH);
				turnOtherLedsOff(i);
				Serial.print("ok tton pressed\n");
				SelectedOption = OK;
				servo1.write(90);
				for (int i = 1; i < 6; i++) {
					turnTo(i, 5);
					delay(100);
				}
				break;
			case triggerBtnPin:
				digitalWrite(ledPins[i], HIGH);
				turnOtherLedsOff(i);
				Serial.print("TRIGGER button pressed\n");
				SelectedOption = TRIGGER;
				servo1.write(90);
				delay(500);
				for (int i = 2; i < 6; i++) {
					turnTo(i, 90);
					delay(100);
				}
				break;
			}
		}
	}
}

void turnOtherLedsOff(int selected) {
	for (int i = 0; i < numberOfButtons; i++) {
		if (i != selected) {
			digitalWrite(ledPins[i], LOW);
		}
	}
}

void doAction() {
	Serial.print("doing action\n");

	if (SelectedOption == PUSH) {
		servo1.write(90);
		delay(5000);
		servo1.write(5);
	}
	else if (SelectedOption == PULL_DOWN) {
		servo1.write(5);
		delay(5000);
		servo1.write(90);
	}
	else if (SelectedOption == TRIGGER) {
		turnTo(2, 110);
		delay(500);
		turnTo(2, 20);
	}
	else if (SelectedOption == OK) {
		turnTo(1, 5);
		for (int i = 2; i < 6; i++) {
			turnTo(i, 90);
		}
		delay(5000);
		for (int i = 2; i < 6; i++) {
			turnTo(i, 5);
		}
	}
	else if (SelectedOption == COUNTING) {
		for (int i = 1; i < 6; i++) {
			turnTo(i, 0);
			delay(1500);
		}
		delay(5000);
		for (int i = 2; i < 6; i++) {
			turnTo(i, 95);
		}
		turnTo(1, 130); // thumb
	}
}

void resetAll() {
	servo1.write(5);
	delay(500);
	for (uint8_t servoPin = 0; servoPin <= 6; servoPin++)
	{
		turnTo(servoPin, 5);
	}
}

void turnTo(int digPin, int deg) {
	pwm.setPWM(digPin, 0, pulseWidth(deg));
	delay(200);
	pwm.setPWM(digPin, 0, 0);
}

int pulseWidth(int angle) // angle in degrees
{
	int pulse_wide, analog_value;
	pulse_wide = map(angle, 0, 180, SERVOMIN, SERVOMAX);
	analog_value = int(float(pulse_wide) / 1000000 * FREQUENCY * 4096);
	return analog_value;
}