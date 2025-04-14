# ESP32 Iron Safety Switch
![WhatsApp Image 2025-04-14 at 22 48 34 (1)](https://github.com/user-attachments/assets/2dcec8e9-bda4-4b79-8568-7c7fec563df3)
![WhatsApp Image 2025-04-14 at 22 48 35 (1)](https://github.com/user-attachments/assets/d11ca4df-f949-40bc-8c57-dbb92d8f40ee)
![WhatsApp Image 2025-04-14 at 22 48 35 (2)](https://github.com/user-attachments/assets/8034f688-883c-4e4b-9e80-34bb83d69744)



## Overview

This project implements a safety device for a standard clothes iron using two ESP32 microcontrollers communicating via ESP-NOW. The primary goal is to automatically cut power to the iron if it is left stationary (e.g., face down or unattended) for a predetermined period, helping to prevent burns and potential fires.

The system consists of two main parts:
1.  **Sender Unit:** Attached to the iron, uses an MPU6050 sensor to detect motion.
2.  **Receiver Unit:** Controls the power supply to the iron via a relay module.

## Features

*   **Motion Detection:** Uses an MPU6050 accelerometer and gyroscope to sense if the iron is stable or moving.
*   **Wireless Communication:** Utilizes ESP-NOW for direct peer-to-peer communication between the ESP32s without needing a WiFi router.
*   **Automatic Power Cutoff:** If the iron remains stationary for 30 seconds, the Sender signals the Receiver to deactivate a relay, cutting power to the iron.
*   **Intermediate Warning/Action:** After 3 seconds of stability, four servos attached to the Sender unit move to a 90-degree position (the physical purpose of this action depends on the mechanical setup).
*   **Automatic Power Restore:** As soon as movement is detected again, the Sender signals the Receiver to reactivate the relay, restoring power to the iron, and the servos return to their initial position.
*   **Status Feedback:** Serial monitor output on both units for debugging and status monitoring.

## Hardware Requirements

*   **Sender Unit:**
    *   1 x ESP32 Development Board
    *   1 x MPU6050 Accelerometer/Gyroscope Module
    *   4 x Servo Motors (e.g., SG90)
    *   Jumper Wires
    *   Appropriate power supply for the ESP32 and servos.
    *   Enclosure/Mounting mechanism to safely attach to the iron.
*   **Receiver Unit:**
    *   1 x ESP32 Development Board
    *   1 x Relay Module (Coil voltage compatible with ESP32 GPIO, contact rating sufficient for your iron's power consumption - e.g., 10A/250VAC)
    *   Jumper Wires
    *   Appropriate power supply for the ESP32.
    *   Safe enclosure for mains voltage wiring.
*   **Other:**
    *   Clothes Iron
    *   USB cables for programming

## Software Requirements & Libraries

*   **Development Environment:**
    *   Arduino IDE or PlatformIO
*   **ESP32 Board Support:**
    *   ESP32 Board definitions installed in your IDE.
*   **Libraries (Install via Arduino Library Manager or PlatformIO):**
    *   `Wire` (usually built-in)
    *   `ESP32Servo` by Kevin Harrington
    *   `Adafruit MPU6050` by Adafruit
    *   `Adafruit Unified Sensor` by Adafruit (Dependency for MPU6050 library)
    *   `WiFi` (built-in for ESP32)
    *   `esp_now` (part of ESP32 core)

## Wiring

**Note:** Wiring diagrams are highly recommended for clarity. This is a textual description.

**Sender Unit:**
*   **MPU6050:**
    *   `VCC` -> ESP32 `3.3V`
    *   `GND` -> ESP32 `GND`
    *   `SCL` -> ESP32 `GPIO 22` (Default I2C Clock)
    *   `SDA` -> ESP32 `GPIO 21` (Default I2C Data)
*   **Servos:**
    *   `Servo 1 Signal` -> ESP32 `GPIO 18` (as defined in `servoPins[0]`)
    *   `Servo 2 Signal` -> ESP32 `GPIO 19` (as defined in `servoPins[1]`)
    *   `Servo 3 Signal` -> ESP32 `GPIO 32` (as defined in `servoPins[2]`)
    *   `Servo 4 Signal` -> ESP32 `GPIO 33` (as defined in `servoPins[3]`)
    *   `Servos VCC` -> External 5V Power Supply (Recommended, ESP32 5V pin might not be sufficient)
    *   `Servos GND` -> External Power Supply `GND` **and** ESP32 `GND` (Common Ground is essential!)

**Receiver Unit:**
*   **Relay Module:**
    *   `VCC` -> ESP32 `5V` or `3.3V` (Check relay module requirements)
    *   `GND` -> ESP32 `GND`
    *   `IN` (Control Pin) -> ESP32 `GPIO 2` (as defined in `relayPin`)
*   **Mains Wiring (USE EXTREME CAUTION):**
    *   The **Live** wire from your mains power source should be interrupted by the relay.
    *   Connect the mains **Live** wire to the relay's `COM` (Common) terminal.
    *   Connect the wire going **to the iron's Live** input to the relay's `NO` (Normally Open) or `NC` (Normally Closed) terminal, depending on the desired default state and relay behavior (The code assumes LOW=ON, HIGH=OFF, often meaning the `NO` contact should be used if LOW activates the relay coil).
    *   The **Neutral** and **Ground** wires usually bypass the relay directly (consult local electrical codes and safety practices).
    *   **!!! WARNING: Working with mains voltage is dangerous. If you are unsure, consult a qualified electrician. Ensure all mains connections are properly insulated and housed in a safe enclosure. !!!**

## Setup & Configuration

1.  **Install IDE and Libraries:** Set up your Arduino IDE or PlatformIO environment and install all the required libraries listed above.
2.  **Wire Components:** Connect the hardware for both the Sender and Receiver units as described in the Wiring section.
3.  **Get Receiver MAC Address:**
    *   Upload the `iron_receiver.ino` code to the **Receiver** ESP32.
    *   Open the Serial Monitor (Baud Rate: 115200).
    *   The Receiver ESP32 will print its unique MAC address. Copy this address (e.g., `F4:65:0B:5A:1F:14`).
4.  **Configure Sender Code:**
    *   Open the `iron_sender.ino` code.
    *   Find the line: `uint8_t receiverMAC[] = {0xF4, 0x65, 0x0B, 0x5A, 0x1F, 0x14};`
    *   Replace the example MAC address with the actual MAC address you copied from the Receiver unit. Make sure to format it as hexadecimal bytes separated by commas (e.g., `0xF4, 0x65, 0x0B, 0x5A, 0x1F, 0x14`).
    *   Verify the `servoPins` array matches the GPIO pins you connected the servos to.
5.  **Configure Receiver Code:**
    *   Open the `iron_receiver.ino` code.
    *   Verify the `relayPin` constant matches the GPIO pin you connected the relay control pin to.
6.  **Upload Code:**
    *   Upload the modified `iron_sender.ino` to the **Sender** ESP32.
    *   Upload the `iron_receiver.ino` (if you made changes) to the **Receiver** ESP32.
7.  **Power Up & Test:**
    *   Power both ESP32 units.
    *   Power the iron *through the relay circuit*.
    *   Observe the Serial Monitor output from both units for status messages ("Sent via ESP-NOW", "Message received", "Turning ON/OFF the relay").
    *   Test the functionality:
        *   Keep the iron still. After 3 seconds, servos should move. After 30 seconds, the relay should click OFF (iron loses power).
        *   Move the iron. The relay should click ON (iron gets power), and servos should return to 0 degrees.

## How It Works

1.  **Sender:** Continuously reads acceleration and gyroscope data from the MPU6050. It calculates a combined magnitude of change to determine if the iron is `isStable`.
2.  **Stability Timers:** If `isStable` is true, it starts a timer.
    *   If stable for > 3 seconds (`stableThreshold1`), it triggers `moveServos(90)`.
    *   If stable for > 30 seconds (`stableThreshold2`), it sets a flag `now = true`.
3.  **Movement Reset:** If movement is detected after being stable, it resets timers, sets `now = false`, and calls `moveServos(0)`.
4.  **ESP-NOW Transmission:** The Sender periodically sends the state of the `now` flag (1 for stable > 30s, 0 otherwise) to the Receiver's MAC address using ESP-NOW.
5.  **Receiver:** Listens for incoming ESP-NOW messages.
6.  **Relay Control:** When a message is received:
    *   If the data is `1`, it sets the `relayPin` `HIGH` (configured in the code to turn the relay OFF, cutting power).
    *   If the data is `0`, it sets the `relayPin` `LOW` (configured in the code to turn the relay ON, restoring power).

## Potential Improvements

*   **MPU6050 Calibration:** Implement a calibration routine for the MPU6050 at startup for more accurate readings.
*   **Sensitivity Adjustment:** Make the stability thresholds (`acc < 1`, `gyro < 0.4`) configurable, potentially without re-uploading code (e.g., via buttons or a simple web interface).
*   **Timer Configuration:** Allow adjustment of `stableThreshold1` and `stableThreshold2`.
*   **Visual Indicators:** Add LEDs to both units for status (Power ON, Connected, Stable, Power Cutoff).
*   **Robustness:** Improve error handling and connection monitoring for ESP-NOW.
*   **Physical Design:** Develop refined and safe enclosures for both units, especially considering heat and mains voltage.
*   **Low Power Modes:** Explore ESP32 sleep modes for the Sender if battery power is considered.

## **Disclaimer**

**Working with mains voltage (110V/230V AC) is extremely dangerous and can lead to severe injury or death.** This project involves interfacing with mains electricity via a relay.

*   You undertake this project **entirely at your own risk**.
*   Ensure you understand the principles of mains wiring and safety precautions.
*   **If you are not experienced or comfortable working with mains voltage, seek assistance from a qualified electrician.**
*   Always disconnect the power supply before working on the mains wiring section.
*   Use appropriate enclosures, insulation, and strain relief for all connections.
*   The creators of this code and documentation are not liable for any damage, injury, or loss resulting from the use or misuse of this project. This is an experimental setup. Test thoroughly in a safe environment.
