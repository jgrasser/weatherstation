#include "Adafruit_DHT/Adafruit_DHT.h"

#define DHTPIN D6
#define DHTTYPE DHT22
#define GRAPHITE_HOSTNAME "myserver"
#define GRAPHITE_PORT 2003
#define TEMT6000PIN A2
#define TMP36PIN A1

DHT dht(DHTPIN, DHTTYPE);
TCPClient client;

void turn_off_photon_leds(){
        RGB.control(true);
        RGB.color(0, 0, 0);
}

void setup() {
	turn_off_photon_leds();
	dht.begin();
}

void loop() {
	delay(2000);
	String oid = "";
	int tstamp = Time.now();

	float h = dht.getHumidity();
	float t = dht.getTempCelcius();
	float f = dht.getTempFarenheit();
	float hi = dht.getHeatIndex();
	float dp = dht.getDewPoint();
	float k = dht.getTempKelvin();

	// Check if any reads failed and exit early (to try again).
	if (isnan(h) || isnan(t) || isnan(f)) {
		Serial.println("Failed to read from DHT sensor!");
		return;
	}

	String oid_humidity = oid.format("sensor.%s.%s.humidity %f %d", "temp", "dht22", h, tstamp);
	String oid_tempc = oid.format("sensor.%s.%s.tempc %f %d", "temp", "dht22", t, tstamp);
	String oid_tempf = oid.format("sensor.%s.%s.tempf %f %d", "temp", "dht22", f, tstamp);
	String oid_dewp = oid.format("sensor.%s.%s.dewp %f %d", "temp", "dht22", dp, tstamp);
	String oid_heati = oid.format("sensor.%s.%s.heati %f %d", "temp", "dht22", hi, tstamp);

	Serial.println( oid_humidity );
	Serial.println( oid_tempc );
	Serial.println( oid_tempf );
	Serial.println( oid_dewp );
	Serial.println( oid_heati );
	
	tstamp = Time.now();

	int tmp36read = analogRead(TMP36PIN);
	float voltage = tmp36read;
	float temperature_in_celcius = (voltage - 500) / 20;
	float temperature_in_fahrenheit = temperature_in_celcius * 1.8 + 32;
   
	String oid_tmp36_tempc = oid.format("sensor.%s.%s.tempc %f %d", "temp", "tmp36", temperature_in_celcius, tstamp);
	String oid_tmp36_tempf = oid.format("sensor.%s.%s.tempf %f %d", "temp", "tmp36", temperature_in_fahrenheit, tstamp);
   
	Serial.println( oid_tmp36_tempc );
	Serial.println( oid_tmp36_tempf );

	tstamp = Time.now();
	int temt6000read = analogRead(TEMT6000PIN);

	String oid_temt6000_read = oid.format("sensor.%s.%s.lightlevel %d %d", "lightlevel", "tmt6000", temt6000read, tstamp);

	Serial.println( oid_temt6000_read );

	client.connect( GRAPHITE_HOSTNAME, GRAPHITE_PORT );
	if( client.connected() ){
		Serial.println( "Writing over network" );
		// Only send data if within acceptable range.
		if (t > 0 && t < 100){
			client.println(oid_humidity);
			client.println(oid_tempc);
			client.println(oid_tempf);
			client.println(oid_dewp);
			client.println(oid_heati);
		}
		client.println(oid_tmp36_tempc);
		client.println(oid_tmp36_tempf);
		client.println(oid_temt6000_read);
	}
	client.flush();
	client.stop();
}
