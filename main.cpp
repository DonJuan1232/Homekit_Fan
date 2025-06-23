#include <Arduino.h>
#include <HomeSpan.h>

#define SERVO_PIN 18
#define SERVO_CHANNEL 0
#define PWM_FREQ 50
#define PWM_RESOLUTION 16

#define FAN_SPEED_MAX 2500
#define FAN_SPEED_OFF 4500
#define FAN_SPEED_ONE_ROTATE 6000
#define FAN_SPEED_TWO_ROTATE 7700

struct FanAccessory : Service::Fan {
  SpanCharacteristic *power;
  SpanCharacteristic *speed;

  FanAccessory() : Service::Fan() {
    power = new Characteristic::On();
    speed = new Characteristic::RotationSpeed(0);

    speed->setRange(0, 100, 33);
  }

  boolean update() {
    if (!power->getNewVal()) {
      ledcWrite(SERVO_CHANNEL, FAN_SPEED_OFF);
    } else {
      int spd = speed->getNewVal();

      if (spd < 33)
        ledcWrite(SERVO_CHANNEL, FAN_SPEED_OFF);
      else if (spd < 66)
        ledcWrite(SERVO_CHANNEL, FAN_SPEED_ONE_ROTATE);
      else if (spd < 88)
        ledcWrite(SERVO_CHANNEL, FAN_SPEED_TWO_ROTATE);
      else
        ledcWrite(SERVO_CHANNEL, FAN_SPEED_MAX);
    }

    return true;
  }
};

void setup() {
  ledcSetup(SERVO_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(SERVO_PIN, SERVO_CHANNEL);
  ledcWrite(SERVO_CHANNEL, FAN_SPEED_OFF);

  Serial.begin(115200);

  homeSpan.setPairingCode("11122333");
  homeSpan.setQRID("111-22-333");
  homeSpan.begin(Category::Bridges, "HomeSpan Fan Bridge");

  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();
    new FanAccessory();
}

void loop() {
  homeSpan.poll();
}
