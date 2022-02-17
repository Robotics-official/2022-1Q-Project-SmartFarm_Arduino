int Soil = A1;

void setup() {
  Serial.begin(9600);
}
 
void loop()
{
  fun_Soil();
}

void fun_Soil()
{
  int Soil_moisture = analogRead(Soil);
  Serial.print("Soil_moisture: ");  
  Serial.println(Soil_moisture);        
  delay(1000); //check 1 Second
}
