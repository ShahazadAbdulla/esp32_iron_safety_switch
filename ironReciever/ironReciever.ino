//iron receiver
#include <WiFi.h>
#include <esp_now.h>

// Define the GPIO pin connected to the relay control signal
const int relayPin = 2; // You can change this to your desired GPIO pin

void onDataReceive(const esp_now_recv_info_t *recvInfo, const uint8_t *data, int dataLen) {
    // Print the sender's MAC address
    Serial.print("Message received from MAC: ");
    for (int i = 0; i < 6; i++) {
        Serial.printf("%02X", recvInfo->src_addr[i]);
        if (i < 5) Serial.print(":");
    }
    Serial.println();

    // Process the received data
    if (dataLen == 1) {
        uint8_t receivedData = data[0];
        Serial.print("Received data: ");
        Serial.println(receivedData);

        if (receivedData == 1) {
            Serial.println("Turning OFF the relay");
            digitalWrite(relayPin, HIGH); // Typically LOW activates the relay
        } else if (receivedData == 0) {
            Serial.println("Turning ON the relay");
            digitalWrite(relayPin, LOW); // Typically HIGH deactivates the relay
        } else {
            Serial.println("Unknown data received.");
        }
    } else {
        Serial.println("Received data length is incorrect.");
    }
}

void setup() {
    Serial.begin(115200);

    // Initialize relay pin
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, HIGH); // Initialize relay OFF

    // Initialize ESP-NOW
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW initialization failed!");
        return;
    }

    esp_now_register_recv_cb(onDataReceive); // Register receive callback

    Serial.println("ESP-NOW Receiver (with Switch) Ready");

    // Get and display the ESP32's MAC address for the sender
    Serial.print("This ESP32's MAC Address (for sender): ");
    Serial.println(WiFi.macAddress());
}

void loop() {
    // Receiver works on interrupt; no need for loop logic
}