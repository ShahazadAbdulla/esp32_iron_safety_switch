//iron sender
#include <Wire.h>
#include <ESP32Servo.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <esp_now.h>

Adafruit_MPU6050 mpu;

Servo servo1, servo2, servo3, servo4;
const int servoPins[4] = {18, 19, 32, 33}; // Different pins for servos

// ESP-NOW related
uint8_t receiverMAC[] = {0xF4, 0x65, 0x0B, 0x5A, 0x1F, 0x14}; // MAC address of switch

bool now = false;
unsigned long stableStartTime = 0;
const unsigned long stableThreshold1 = 3000; // 3 seconds
const unsigned long stableThreshold2 = 30000; // 30 seconds
bool wasStable = false;

void onSent(const uint8_t *macAddr, esp_now_send_status_t status) {
    Serial.print("Last Packet Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
    Serial.begin(115200);

    // Setup MPU6050
    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
        while (1) delay(10);
    }
    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

    // Attach servos to pins
    servo1.attach(servoPins[0]);
    servo2.attach(servoPins[1]);
    servo3.attach(servoPins[2]);
    servo4.attach(servoPins[3]);

    // Initial position
    moveServos(0);

    // Initialize ESP-NOW
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW initialization failed!");
        return;
    }
    esp_now_register_send_cb(onSent);

    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, receiverMAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }

    Serial.println("ESP-NOW Sender (with Iron) Ready");
}

void loop() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    float acc = abs(a.acceleration.x) + abs(a.acceleration.y) + abs(a.acceleration.z - 9.81);
    float gyro = abs(g.gyro.x) + abs(g.gyro.y) + abs(g.gyro.z);

    Serial.print("acc: ");
    Serial.println(acc);
    Serial.print("gyro: ");
    Serial.println(gyro);

    bool isStable = (acc < 1 && gyro < 0.4); // Adjust sensitivity as needed

    if (isStable) {
        if (!wasStable) {
            stableStartTime = millis();
            wasStable = true;
        } else {
            unsigned long stableDuration = millis() - stableStartTime;

            if (stableDuration > stableThreshold1) {
                moveServos(90);
            }

            if (stableDuration > stableThreshold2) {
                now = true;
                Serial.println("Box is now fully stable (now = true)");
            }
        }
    } else {
        if (wasStable || now) {
            moveServos(0);
            now = false;
            wasStable = false;
            Serial.println("Movement detected, resetting");
        }
    }

    // Send data via ESP-NOW
    uint8_t dataToSend = now ? 1 : 0;
    esp_err_t result = esp_now_send(receiverMAC, &dataToSend, sizeof(dataToSend));
    if (result == ESP_OK) {
        Serial.println("Sent via ESP-NOW: " + String(dataToSend));
    } else {
        Serial.println("Error sending via ESP-NOW");
    }

    delay(100);
}

void moveServos(int angle) {
    servo1.write(angle);
    servo2.write(angle);
    servo3.write(angle);
    servo4.write(angle);
}