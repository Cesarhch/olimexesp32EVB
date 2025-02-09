#include <Arduino.h>
//necesario importar las librerias IRrecv.h, IRutils. Para esto incluir en VSCode Platformio :lib_deps = IRremoteESP8266
#include <IRrecv.h>
#include <IRutils.h>

#define IR_RECEIVE_PIN 39  // Pin del receptor IR en la ESP32-EVB

IRrecv irrecv(IR_RECEIVE_PIN);
decode_results results;

void setup() {
    Serial.begin(9600);
    irrecv.enableIRIn();  // Inicia el receptor IR
    Serial.println("Receptor IR listo. Apunta el mando y presiona un botón...");
}

void loop() {
    if (irrecv.decode(&results)) {
        Serial.print("Código recibido: ");
        Serial.println(results.value, HEX);
        irrecv.resume();  // Recibir la siguiente señal
    }
}
