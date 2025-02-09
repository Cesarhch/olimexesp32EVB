#include <Arduino.h>
//necesario importar las librerias IRrecv.h, IRutils. Para esto incluir en VSCode Platformio :lib_deps = IRremoteESP8266
#include <IRutils.h>
#include <IRsend.h>

#define IR_SEND_PIN 12  // Pin del emisor IR en la ESP32-EVB
#define TV_POWER_CODE 0x20DF10EF  // Código IR de encendido/apagado de la TV
#define TV_PROTOCOL NEC  // Protocolo del control remoto (generalmente NEC)

IRsend irsend(IR_SEND_PIN);
bool estado = false;  // Variable para alternar el encendido y apagado

void setup() {
    Serial.begin(9600);
    irsend.begin();  // Inicializa el transmisor IR
    Serial.println("Enviando señal IR cada 10 segundos...");
}

void loop() {
    Serial.print("Enviando código IR: ");
    Serial.println(TV_POWER_CODE, HEX);
    
    irsend.sendNEC(TV_POWER_CODE, 32); // Enviar el código IR con 32 bits

    estado = !estado;  // Alternar estado
    Serial.println(estado ? "Encendiendo TV" : "Apagando TV");

    delay(20000); // Esperar 10 segundos antes de enviar de nuevo
}
