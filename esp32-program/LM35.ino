#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 18      // Pin yang terhubung ke sensor DHT11
#define DHTTYPE DHT11  // Menentukan tipe sensor (DHT11)
DHT dht(DHTPIN, DHTTYPE);

#define relay 5

void setup() {
  Serial.begin(115200);
  pinMode(relay, OUTPUT);

  digitalWrite(relay, false);
  //  dht.begin();
}

void loop() {
  float suhu = dht.readTemperature();   // Suhu dalam Celsius
  float kelembapan = dht.readHumidity();

  Serial.print("suhu: ");
  Serial.println(suhu);

  delay(500);
  //  digitalWrite(relay, false);
  //  Serial.println("relay LOW");
  //  delay(1000);
  //
  //  digitalWrite(relay, true);
  //  Serial.println("relay HIGH");
  //  delay(1000);
}
