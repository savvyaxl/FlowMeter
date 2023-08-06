/*
YF‐ S201 Water Flow Sensor
Water Flow Sensor output processed to read in litres/hour
Adaptation Courtesy: www.hobbytronics.co.uk
*/

volatile int interruptCount; // Measures flow sensor pulsesunsigned 

unsigned char flowsensor = 2; // Sensor Input GPIO
unsigned long lastMeasureTime = 0;
unsigned long lastReadTime = 0;
const float FLOW_FACTOR = 6.6; // Example: 6.6 L/min per pulse
float totalFlow = 0.0;
int advertiseCount = 0;
int advertiseCountMAX = 3600; //every hour


void flow () // Interrupt function
{
   interruptCount++;
}

float readFlowData() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - lastReadTime;
//  //Q=L/MIN
  float Q = (1000 * (interruptCount / (float)elapsedTime)) / FLOW_FACTOR;
  float flowLitesPerSecond = 0;
  if (Q == Q ){
    flowLitesPerSecond = Q / 60 ;
  }
  totalFlow += flowLitesPerSecond;
  interruptCount = 0;
  lastReadTime = millis();
  return flowLitesPerSecond;
}

void reportTotalFlow() {
  String str = String("{ ");
  str += String(  "\"WaterFlowMain\" : ") + String(totalFlow);
  str += String(" }");
  Serial.println(str);
  // Reset the total flow
  totalFlow = 0.0;
}

void printConfig (){
  Serial.println("CONFIGdevice_class:water,name:Water_Flow_Main,unit_of_measurement:L,value_template:{{value_json.WaterFlowMain}}");
}

   void setup()
 {
   pinMode(flowsensor, INPUT_PULLUP);
   Serial.begin(115200);
   Serial.println("Starting... yf-b5 Flow Meter");
   delay(20000);
   printConfig ();

  // homeassistant/sensor/84f3eb23ea09/Water_Flow_Main/config
  //  {
  //  "device_class": "water",
  //  "name": "Water_Flow_Main",
  //  "unit_of_measurement": "L",
  //  "value_template": "{{value_json.WaterFlowMain}}",
  //  "state_topic": "homeassistant/sensor/84f3eb23ea09/state"
  //  }

   attachInterrupt(digitalPinToInterrupt(flowsensor), flow, RISING); // Setup Interrupt
   sei(); // Enable interrupts
   lastMeasureTime = millis();
   lastReadTime = millis();
}

int sec_counter = 0;
void loop() {
  if ((millis() - lastMeasureTime) >= 1000) {
    float flow = readFlowData();
    sec_counter++;
    lastMeasureTime = millis(); // Update lastMeasureTime for every second
    // Check if one minute has passed and report the total flow
    if (sec_counter >= 60 ) {
      reportTotalFlow();
      sec_counter = 0;
    }
    advertiseCount++;
    if ( advertiseCount > advertiseCountMAX ){
      // send the config every advertiseCountMAX
      printConfig ();
      advertiseCount = 0;
    }
  }
}
