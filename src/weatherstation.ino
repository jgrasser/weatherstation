#include "Adafruit_DHT/Adafruit_DHT.h"

#define LABEL_HUMIDITY       "HUMIDITY"
#define LABEL_LIGHTLEVEL     "LIGHTLEVEL"
#define LABEL_TEMPERATURE_C  "TEMPERATURE_C"
#define LABEL_TEMPERATURE_F  "TEMPERATURE_F"

#define PIN_DHT       D6
#define PIN_TEMT6000  A2
#define PIN_TMP36     A1

#define SENSOR_DHT       "DHT22"
#define SENSOR_TEMT6000  "TEMT6000"
#define SENSOR_TMP36     "TMP36"

#define DHTTYPE DHT22

DHT dht(PIN_DHT, DHTTYPE);

#define GRAPHITE_HOSTNAME "myserver"
#define GRAPHITE_PORT 2003


TCPClient client;

float voltage_to_celcius(float voltage){
        // voltage should be in mV units.
        // Centigrade temperature = [(analog voltage in mV) - 500] / 10
        float temperature_in_celcius = (voltage - 500) / 20;
        return temperature_in_celcius;
}

float celcius_to_farenheit(float celcius){
        //T(°F) = T(°C) ▒~W 1.8 + 32
        float temperature_in_fahrenheit = celcius * 1.8 + 32;
        return temperature_in_fahrenheit;
}

void log_it(char *sensor, char *label, float measurement){
	int timestamp = Time.now();
        String empty = "";
        String metric = empty.format("sensor.%s.%s %f %d", label, sensor, measurement, timestamp);

        client.connect( GRAPHITE_HOSTNAME, GRAPHITE_PORT );
        if( client.connected() ){
                Serial.println("Writing over network: " + metric);
                client.println(metric);
        }
        client.flush();
        client.stop();
}

void turn_off_indicator_light(){
        RGB.control(true);
        RGB.color(0, 0, 0);	
}
 
void setup() {
        turn_off_indicator_light();
	dht.begin();
}

void loop() {
	delay(2000);

	float dht_humidity = dht.getHumidity();
	float dht_temperature_c = dht.getTempCelcius();
	float dht_temperature_f = dht.getTempFarenheit();

	// Check if any reads failed and exit early (to try again).
        // Read is considered to fail if values are nan or not within expected range.
	if (isnan(dht_humidity) || isnan(dht_temperature_c) || isnan(dht_temperature_f) || (dht_temperature_c > 0 && dht_temperature_c < 100)) {
		Serial.println("Failed to read from DHT sensor!");
		return;
	}

        log_it(SENSOR_DHT, LABEL_HUMIDITY, dht_humidity);
        log_it(SENSOR_DHT, LABEL_TEMPERATURE_F, dht_temperature_f);
        log_it(SENSOR_DHT, LABEL_TEMPERATURE_C, dht_temperature_c);

	
	int tmp36read = analogRead(PIN_TMP36); 
	float tmp36_temperature_c = voltage_to_celcius(tmp36read);
	float tmp36_temperature_f = celcius_to_farenheit(tmp36_temperature_c);
        log_it(SENSOR_TMP36, LABEL_TEMPERATURE_F, tmp36_temperature_c);   
        log_it(SENSOR_TMP36, LABEL_TEMPERATURE_C, tmp36_temperature_f);   

	int temt6000read = analogRead(PIN_TEMT6000); 
        log_it(SENSOR_TEMT6000, LABEL_LIGHTLEVEL, temt6000read);
}
