// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_DHT/Adafruit_DHT.h"

#define TEMT6000PIN A2
#define TMP36PIN A1
#define DHTPIN D6

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11		// DHT 11 
#define DHTTYPE DHT22		// DHT 22 (AM2302)
//#define DHTTYPE DHT21		// DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

#define GRAPHITE_HOSTNAME "myserver"
#define GRAPHITE_PORT 2003

TCPClient client;

void setup() {
    RGB.control(true); 
    RGB.color(0, 0, 0);
    dht.begin();
}

void loop() {
    delay(2000);
    String oid = "";
    int tstamp = Time.now();

	// Reading temperature or humidity takes about 250 milliseconds!
	// Sensor readings may also be up to 2 seconds 'old' (its a 
	// very slow sensor)
	float h = dht.getHumidity();
	// Read temperature as Celsius
	float t = dht.getTempCelcius();
	// Read temperature as Farenheit
	float f = dht.getTempFarenheit();
  
	// Check if any reads failed and exit early (to try again).
	if (isnan(h) || isnan(t) || isnan(f)) {
		Serial.println("Failed to read from DHT sensor!");
		return;
	}

	// Compute heat index
	// Must send in temp in Fahrenheit!
	float hi = dht.getHeatIndex();
	float dp = dht.getDewPoint();
	float k = dht.getTempKelvin();
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
	
    int tmp36read = analogRead(TMP36PIN); 
    tstamp = Time.now();
    // Read will be a value between 0 and 4096. This
    // represents that fraction of the input voltage. 
    // 0.8 mV per unit to be exact. 
    
    float voltage = tmp36read;
    // voltage should be in mV units. 
    // Centigrade temperature = [(analog voltage in mV) - 500] / 10
    float temperature_in_celcius = (voltage - 500) / 20;
    //T(°F) = T(°C) × 1.8 + 32
    float temperature_in_fahrenheit = temperature_in_celcius * 1.8 + 32;
    
    String oid_tmp36_tempc = oid.format("sensor.%s.%s.tempc %f %d", "temp", "tmp36", temperature_in_celcius, tstamp);
    String oid_tmp36_tempf = oid.format("sensor.%s.%s.tempf %f %d", "temp", "tmp36", temperature_in_fahrenheit, tstamp);
    
    Serial.println( oid_tmp36_tempc );
    Serial.println( oid_tmp36_tempf );
    
    //read data from light sensor
    int temt6000read = analogRead(TEMT6000PIN); 
    tstamp = Time.now();
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