// #include <Keyboard.h>

#ifndef SERIAL
#define SERIAL TRUE
#endif

#define SWITCH_PIN 4
#define BUTTON_PIN 2

//probably better to just replace these with the input values from digitalRead.
bool prev_deviceSwitch = false;
bool deviceSwitch = false;

//in order to prevent the event from queueing and firing when the event is detached, adding a switch to control it.
volatile bool buttonReady = false;

unsigned long switch_lastMillis = 0;
const unsigned long switch_interval = 3000; //check switch every 3 seconds.
const unsigned long debounce_interval = 300;

const unsigned long buttonActivationDelay = 100; //delay to allow a call to ISR to clear out queued behavior.

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
                //Keyboard.end();
                buttonReady = false; //disable everything in the interupt.
                detachInterrupt(digitalPinToInterrupt(BUTTON_PIN));
                #if SERIAL == TRUE
                Serial.println("switch deactivated");
                #endif

            }
        } else { //switchval = LOW.
            #if SERIAL == TRUE
            Serial.println("switchval = LOW");
            #endif
            deviceSwitch = true;
            static bool buttonActivating = false;
            static unsigned long buttonActivationStartTime = 0;
            if (deviceSwitch != prev_deviceSwitch) {
                // Keyboard.begin();
                // have to wait before flipping button ready.
                buttonActivating = true;
                buttonActivationStartTime = currentMillis;
                // Should be triggered on falling edge.
                attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), button_interrupt, FALLING);
                #if SERIAL == TRUE
                Serial.println("switch activated");
                #endif
            } else if (buttonActivating) {
                if (currentMillis - buttonActivationStartTime > buttonActivationDelay && !buttonReady) {
                    buttonReady = true;
                    buttonActivating = false;   
                }
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


volatile unsigned long prevInterruptTime = 0;
/*
Interupt handler.
I think what I will need to do is immediately deactivate the ISR, and reactivate it after a hundred millis or so
in the main loop.

scratch that, I think the problem might be that I forgot to things as volatile.

The interrupt on that pin may still be queueing though. Yep, interrupt is still queueing.
 */
void button_interrupt() {
    //millis does not advance in the interupt handler, but should still be able to be called.
    //have to debounce the input.
    unsigned long interruptTime = millis();
    if (interruptTime - prevInterruptTime > debounce_interval) {
        #if SERIAL == TRUE
        Serial.println("Interupt handler called");
        Serial.println(email);
        #endif
        prevInterruptTime = interruptTime;
    }
}
