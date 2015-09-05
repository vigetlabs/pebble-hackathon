#include <ArduinoPebbleSerial.h>
#include <Servo.h>

static const uint16_t SUPPORTED_SERVICES[] = {0x1001};
static const uint8_t NUM_SERVICES = 2;
// static uint8_t pebble_buffer[GET_PAYLOAD_BUFFER_SIZE(200)];
static uint8_t pebble_buffer[256];
static bool is_connected = false;

Servo xAxisServo;
Servo yAxisServo;
Servo triggerServo;

bool xRead = false;
bool yRead = false;

int16_t xAxis;
int16_t yAxis;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  for (int i = 0; i < 3; i++) {
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
  }

  // Teensy 3.0/3.1 uses hardware serial mode (pins 0/1) with RX/TX shorted together
  ArduinoPebbleSerial::begin_hardware(pebble_buffer, sizeof(pebble_buffer), Baud57600,
                                      SUPPORTED_SERVICES, NUM_SERVICES);

  xAxisServo.attach(3);
  yAxisServo.attach(4);
  triggerServo.attach(5);
}

void loop() {
  determineState();
  display();
  // servoMessingAround();
}

void determineState() {
  checkForConnection();
  readFromPebble();
}

void checkForConnection() {
  if (ArduinoPebbleSerial::is_connected()) {
    if (!is_connected) {
      Serial.println("Connected to the smartstrap!");
      is_connected = true;
    }
  } else {
    if (is_connected) {
      Serial.println("Disconnected from the smartstrap!");
      is_connected = false;
    }
  }
}

void readFromPebble() {
  // Let the ArduinoPebbleSerial code do its processing
  size_t length;
  uint16_t service_id;
  uint16_t attribute_id;
  RequestType type;

  if (ArduinoPebbleSerial::feed(&service_id, &attribute_id, &length, &type)) {
    if (service_id == 0x1001) {
      if (attribute_id == 0x1001) {
        xAxis = *(int16_t*)pebble_buffer;
        xRead = true;
      }
      else if (attribute_id == 0x1002) {
        yAxis = *(int16_t*)pebble_buffer;
        yRead = true;
      }
    }
  }
}

void display() {
  if (xRead && yRead) {
    xRead = false;
    yRead = false;
    Serial.printf("%d\t%d\n", xAxis, yAxis);
  }
}

void servoMessingAround() {
  int pos;
  for(pos = 10; pos < 170; pos += 1) {
    xAxisServo.write(pos);
    yAxisServo.write(pos);
    triggerServo.write(pos);
    delay(15);
  }
  for(pos = 180; pos>=1; pos-=1) {
    xAxisServo.write(pos);
    yAxisServo.write(pos);
    triggerServo.write(pos);
    delay(15);
  }
}
