#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ArduinoJson.h>

// Ganti dengan kredensial Wi-Fi Anda
const char* ssid = "Uhuy";
const char* password = "87654321";

// Ganti dengan URL Realtime Database Firebase Anda
const char* firebaseHost = "https://coba-53d06-default-rtdb.asia-southeast1.firebasedatabase.app/";

// Path data dalam database
const char* dataPath = "/kipas/"; // Menggunakan .json di akhir path

#define DHTPIN 18      // Pin yang terhubung ke sensor DHT11
#define DHTTYPE DHT11  // Menentukan tipe sensor (DHT11)
DHT dht(DHTPIN, DHTTYPE);

#define relay 5
bool relayState = false;
int templimit = 29;

// Fungsi untuk menghubungkan ke NTP dan mendapatkan waktu
void setupTime() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov"); // Menggunakan NTP server
  Serial.print("Waiting for NTP time sync: ");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" Time synced");
}

bool fetchDataFromFirebase() {
  StaticJsonDocument<200> doc;
  
  // Mengambil data dari Firebase menggunakan GET request
  String url = String(firebaseHost) + dataPath + ".json"; // URL untuk mendapatkan data
  HTTPClient http;
  http.begin(url);

  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    String payload = http.getString();
    Serial.println("Response Code: " + String(httpResponseCode));
    Serial.println("Data: " + payload);  // Menampilkan data yang diterima
    DeserializationError error = deserializeJson(doc, payload);

//  Periksa jika terjadi kesalahan saat parsing JSON
    if (error) {
      Serial.print("JSON Parsing Error: ");
      Serial.println(error.c_str());
      return false;
    }

    // Membaca nilai dari JSON
    bool switchState = doc["switch"];
    return switchState;
  } else {
    Serial.println("Error on HTTP request");
    return false; // Nilai default jika gagal mendapatkan data
  }

  http.end();
}

void setup() {
  // Mulai serial monitor
  Serial.begin(115200);

  // Koneksi ke Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Setup NTP untuk sinkronisasi waktu
  setupTime();
  
  dht.begin();
  
  pinMode(relay, OUTPUT);
  digitalWrite(relay, relayState);
}

void loop() {
  float suhu = dht.readTemperature();   // Suhu dalam Celsius
  float kelembapan = dht.readHumidity();
  bool switchState = fetchDataFromFirebase(); // Menggunakan nama variabel yang diperbaiki

  if (suhu >= templimit || switchState == true) {
    relayState = true;
  } else if (suhu < templimit || switchState == false) {
    relayState = false;
  }

  digitalWrite(relay, relayState);
  
  // Membuat JSON data yang akan dikirim
  String dataJson = "{\"temp\":" + String(suhu) + ", \"humid\":" + String(kelembapan) + ", \"status\":" + (relayState ? "true" : "false") + ", \"switch\":" + (switchState ? "true" : "false") + "}";

  // Konfigurasi URL dan header untuk PUT request
  String url = String(firebaseHost) + dataPath + ".json";
  Serial.println("Mengirim data ke Firebase...");
  
  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  // Kirim PUT request dengan data JSON
  int httpResponseCode = http.PUT(dataJson); 

  // Periksa respons dari server
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Response Code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
  } else {
    Serial.println("Error on HTTP request");
  }

  http.end();
  
  delay(1000); 
}
