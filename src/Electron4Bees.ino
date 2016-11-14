// This #include statement was automatically added by the Particle IDE.
#include "application.h" //Notwendig damit die externe RGB-LED schon beim Booten leuchtet!
#include "idDHT22/idDHT22.h"
#include "cloud4bees.h"
#include "HX711.h"
　
// Automatically mirror the onboard RGB LED to an external RGB LED
// No additional code needed in setup() or loop()
　
class ExternalRGB {
  public:
    ExternalRGB(pin_t r, pin_t g, pin_t b) : pin_r(r), pin_g(g), pin_b(b) {
      pinMode(pin_r, OUTPUT);
      pinMode(pin_g, OUTPUT);
      pinMode(pin_b, OUTPUT);
      RGB.onChange(&ExternalRGB::handler, this);
    }
　
    void handler(uint8_t r, uint8_t g, uint8_t b) {
     
      analogWrite(pin_r, r);
      analogWrite(pin_g, g);
      analogWrite(pin_b, b);
    }
　
private:
      pin_t pin_r;
      pin_t pin_g;
      pin_t pin_b;
};
　
// Connect an external RGB LED to D0, D1 and D2 (R, G, and B)
ExternalRGB myRGB(D0, D1, D2);
　
　
// declaration for DHT11 handler
int idDHT22pin = D4; //Digital pin for comunications
void dht22_wrapper(); // must be declared before the lib initialization
　
// DHT instantiate
idDHT22 DHT22(idDHT22pin, dht22_wrapper);
　
　
　

Cloud4BeesLibrary::Cloud4Bees cloud4bees ("cloud4Bees - API-Schlüssel");
　
// HX711.DOUT	- pin #D1
// HX711.PD_SCK	- pin #D0
　
// HX711 scale(D1, D0);		// parameter "gain" is ommited; the default value 128 is used by the library
　
　
#define DOUT  A0
#define CLK  A1
　
　
　
HX711 scale(DOUT, CLK);
　
　
//float offset = 0;
//float scalefactor = 1;
　
float offset = 276.9;
float scalefactor = 31679.0;
　
double weight = 0;
　
double voltage = 0; // Variable to keep track of LiPo voltage
double soc = 0; // Variable to keep track of LiPo state-of-charge (SOC)
　
double temperature = 0;
double humidity = 0;
double dewpoint = 0;
　
　
// This wrapper is in charge of calling
// must be defined like this for the lib work
void dht22_wrapper() {
DHT22.isrCallback();
}
  
  
　
　
void setup()
{
    
  
  scale.set_scale(scalefactor);                      //this value is obtained by calibrating the scale with known weights; 
                                                 /*   How to Calibrate Your Scale
                                                      1.Call set_scale() with no parameter.
                                                      2.Call set_tare() with no parameter.
                                                      3.Place a known weight on the scale and call get_units(10).
                                                      4.Divide the result in step 3 to your known weight. You should get about the parameter you need to pass to set_scale.
                                                      5.Adjust the parameter in step 4 until you get an accurate reading. 
                                                */ 
 
}
　
　
　
void loop()
{ 
    Particle.process();
    //scale.get_units(10) returns the medium of 10 measures
    weight = (scale.get_units(10) - offset);
　
    // Fuel Gauge
    FuelGauge fuel;
    voltage = fuel.getVCell();
    soc = fuel.getSoC();
	
	  // Temperature, Humidity and Dewpoint from the DHT22 Sensor
	  DHT22.acquire();
	  while (DHT22.acquiring());
	  temperature = DHT22.getCelsius();
	  humidity = DHT22.getHumidity();
	  dewpoint = DHT22.getDewPoint();
    
    
    //Send Value to cloud4Bees
    cloud4bees.recordValue(1, String(weight));
   	cloud4bees.recordValue(2, String(temperature));
	  cloud4bees.recordValue(3, String(humidity));
	  cloud4bees.recordValue(4, String(dewpoint));
	  cloud4bees.recordValue(5, String(voltage));
	  cloud4bees.recordValue(6, String(soc));
	  cloud4bees.sendValues();
    delay(60000);
    Cellular.off();
    delay(100);
    System.sleep(SLEEP_MODE_DEEP, 3600);
       
}
