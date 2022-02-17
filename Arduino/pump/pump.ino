int Watering = 2;

void setup()
{
 Serial.begin(9600); // serial communication reset
 pinMode(Watering, OUTPUT);
 digitalWrite(Watering, LOW);
}

void loop()
{
  fun_Watering();
}

void fun_Watering()
{
  Serial.println(" ** Watering System ReStart ** ");
  digitalWrite(Watering, LOW); // relay off + common situation
  while (1) {
    if (Serial.available()) {
      String inString = Serial.readStringUntil('\n');
      Serial.println(inString);
      if (inString== "WATER_ON") {
        digitalWrite(Watering, HIGH); // relay on + watering start
        delay(5000); //(for 5 Second)
        Serial.println( " ** Watering is Finished ** ");
        break;
      }
    }
  }
}
