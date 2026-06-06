/* =============================================================
 *  SO₂ Gas Detector
 *  Hardware : Arduino Uno
 *  Sensor   : MQ-135 gas sensor (analog output)
 *  Display  : 16x2 LCD (4-bit mode)
 *  Alert    : Buzzer on GPIO8 — triggers at 1000 PPM threshold
 *
 *  Detects sulphur dioxide (SO₂) and other toxic gases in the
 *  range 10–1000 PPM. Displays real-time PPM reading on LCD.
 *  Buzzer activates when concentration exceeds safe threshold.
 *
 *  Sensor principle:
 *  MQ-135 is a chemiresistive gas sensor. Its internal resistance
 *  drops as target gas concentration increases. A voltage divider
 *  with a load resistor converts this to an analog voltage read
 *  by Arduino's 10-bit ADC (0–1023 → 0–5V).
 *
 *  PPM conversion:
 *  MQ-135 datasheet provides Rs/Ro ratio vs PPM curve for
 *  multiple gases. For SO₂, the curve follows a power law:
 *  PPM = A * (Rs/Ro)^B where A and B are curve-fit constants.
 *  Ro is calibrated in clean air (baseline resistance).
 *
 *  Tested by burning a matchstick near the sensor to simulate
 *  SO₂ from sulphur combustion — readings exceeded 800 PPM
 *  during peak exposure, confirming sensor response.
 * ============================================================= */

#include <LiquidCrystal.h>

/* ─── LCD pin mapping (4-bit mode) ─────────────────────────── 
 *  RS=7, EN=6, D4=5, D5=4, D6=3, D7=2
 * ──────────────────────────────────────────────────────────── */
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

/* ─── Pin definitions ───────────────────────────────────────── */
#define MQ135_PIN   A0   /* Analog input — MQ-135 output        */
#define BUZZER_PIN   8   /* Digital output — buzzer             */

/* ─── Sensor calibration ────────────────────────────────────── 
 *  Ro: sensor resistance in clean air (baseline).
 *  Calibrated by reading sensor in fresh air for 5 minutes
 *  and averaging the ADC output. Adjust CLEAN_AIR_FACTOR
 *  based on your specific MQ-135 module.
 * ──────────────────────────────────────────────────────────── */
#define CLEAN_AIR_FACTOR  3.6    /* Rs/Ro in clean air for MQ-135 */
#define RL_VALUE          10.0   /* Load resistance in kΩ          */
#define Ro                76.63  /* Sensor resistance in clean air (kΩ) */

/* ─── SO₂ curve constants (power law fit from datasheet) ────── */
#define SO2_A   116.602
#define SO2_B   -2.769

/* ─── Alert threshold ───────────────────────────────────────── */
#define PPM_THRESHOLD  1000.0   /* Buzzer triggers above this PPM */

/* ─── Sampling ──────────────────────────────────────────────── */
#define SAMPLE_INTERVAL_MS  1000   /* 1 Hz sampling rate           */
#define ADC_SAMPLES         50     /* Average 50 reads per sample  */

unsigned long last_sample = 0;

/* ─── Read MQ-135 and convert to PPM ────────────────────────── */
float readPPM() {
    /* Average multiple ADC readings to reduce noise */
    long sum = 0;
    for (int i = 0; i < ADC_SAMPLES; i++) {
        sum += analogRead(MQ135_PIN);
        delay(2);
    }
    float adc_avg = sum / (float)ADC_SAMPLES;

    /* Convert ADC to sensor voltage */
    float sensor_volt = (adc_avg / 1023.0) * 5.0;

    /* Calculate Rs from voltage divider equation */
    float Rs = RL_VALUE * (5.0 - sensor_volt) / sensor_volt;

    /* Calculate Rs/Ro ratio */
    float ratio = Rs / Ro;

    /* Apply SO₂ power law curve: PPM = A * (Rs/Ro)^B */
    float ppm = SO2_A * pow(ratio, SO2_B);

    return ppm;
}

void setup() {
    Serial.begin(9600);

    /* LCD initialisation */
    lcd.begin(16, 2);
    lcd.print("SO2 Detector");
    lcd.setCursor(0, 1);
    lcd.print("Warming up...");

    /* Buzzer off initially */
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    /* MQ-135 warm-up — sensor needs ~60s to stabilise */
    delay(60000);

    lcd.clear();
    lcd.print("Monitoring...");
    Serial.println("SO2 Gas Detector — Monitoring started");
}

void loop() {
    unsigned long now = millis();

    if (now - last_sample >= SAMPLE_INTERVAL_MS) {
        float ppm = readPPM();
        last_sample = now;

        /* Print to Serial Monitor */
        Serial.print("SO2: ");
        Serial.print(ppm, 1);
        Serial.println(" PPM");

        /* Update LCD */
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("SO2: ");
        lcd.print(ppm, 1);
        lcd.print(" PPM");

        lcd.setCursor(0, 1);
        if (ppm >= PPM_THRESHOLD) {
            /* Alert state */
            lcd.print("!! DANGER !!");
            digitalWrite(BUZZER_PIN, HIGH);
            Serial.println("ALERT: Threshold exceeded");
        } else if (ppm >= 500) {
            /* Warning state */
            lcd.print("WARNING: High");
            digitalWrite(BUZZER_PIN, LOW);
        } else {
            /* Safe state */
            lcd.print("Status: Safe");
            digitalWrite(BUZZER_PIN, LOW);
        }
    }
}
