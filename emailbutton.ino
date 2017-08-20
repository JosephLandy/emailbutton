//#include <Keyboard.h>

#ifndef SERIAL
#define SERIAL TRUE
#endif

#define SWITCH_PIN 4
#define BUTTON_PIN 2

//probably better to just replace these with the input values from digitalRead.
bool prev_deviceSwitch = false;
bool deviceSwitch = false;

unsigned long switch_lastMillis = 0;
const long switch_interval = 10000; //check switch every 5 seconds.

int btn_prevState = HIGH;

const char email[] = "joseph.landy.email@gmail.com";

void setup() {
    #if SERIAL == TRUE
    Serial.begin(9600);
    Serial.println("serial activated");
    #endif

    pinMode(SWITCH_PIN, INPUT_PULLUP);
    pinMode(BUTTON_PIN,INPUT_PULLUP);
}

void loop() {
    //since the switch activates the keyboard detection functionality and will be set for long periods of time, I
    //won't have to read it for long periods of time. Even once every 5 seconds would be enough.
    unsigned long currentMillis = millis();
    //check if it's time to read the switch.
    if (currentMillis - switch_lastMillis > switch_interval) {
        //if input is high, switch is false since it's connected with a pullup resistor.
        int switchval = digitalRead(SWITCH_PIN);
        if (switchval == HIGH) {
            #if SERIAL == TRUE
            Serial.println("switchval = HIGH");
            #endif
            deviceSwitch = false;
            if (deviceSwitch != prev_deviceSwitch) {
                //device switch is false, so deactivate everything.
                Keyboard.end();
                #if SERIAL == TRUE
                Serial.println("switch deactivated");
                #endif
            }
        } else { //switchval = LOW.
            #if SERIAL == TRUE
            Serial.println("switchval = LOW");
            #endif
            deviceSwitch = true;
            if (deviceSwitch != prev_deviceSwitch) {
                Keyboard.begin();
                #if SERIAL == TRUE
                Serial.println("switch activated");
                #endif
            }
        }
        prev_deviceSwitch = deviceSwitch;
        switch_lastMillis = currentMillis;
    }
}

#if SERIAL == TRUE
void send_deviceSwitch() {
    if (deviceSwitch) {
        Serial.println("device switch is true");
    } else {
        Serial.println("device switch is false");
    }
}
#endif


unsigned long prevInterruptTime = 0;
/*
Interupt handler.
 */
void button_interrupt() {
    //millis does not advance in the interupt handler, but should still be able to be called.

}
