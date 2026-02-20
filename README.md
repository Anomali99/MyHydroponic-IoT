# MyHydroponic IoT (Firmware)

Repositori ini berisi kode *firmware* (berbasis ESP32) untuk perangkat IoT pada sistem otomasi hidroponik **MyHydroponic**. Perangkat ini bertugas untuk membaca data dari sensor (seperti sensor pH dan PPM), serta mengontrol aktuator. Data tersebut kemudian dikirimkan ke *server* melalui protokol MQTT untuk diproses lebih lanjut oleh *logic engine* (n8n).

---

## 📋 Prasyarat
Sebelum mengunggah (*upload*) kode ke mikrokontroler, pastikan Anda telah menyiapkan:
1. **Perangkat Keras:** ESP32 beserta rangkaian sensor pendukung (pH, TDS/PPM, dll.).
2. **Perangkat Lunak:** Arduino IDE atau VS Code dengan ekstensi PlatformIO.
3. **Pustaka (Libraries):** Pastikan semua *library* yang dibutuhkan untuk sensor dan *client* MQTT (seperti `PubSubClient`) sudah terinstal.

---

## ⚙️ Konfigurasi Jaringan dan MQTT

Sebelum melakukan *flashing* ke ESP32, Anda wajib menyesuaikan kredensial WiFi dan konfigurasi *broker* MQTT pada kode sumber Anda (`config.h`).

Ubah nilai variabel berikut sesuai dengan lingkungan jaringan Anda:

```cpp
// Konfigurasi WiFi
#define WIFI_SSID "Kembar Dua"
#define WIFI_PASS "sasa1234"

// Konfigurasi MQTT Broker
#define MQTT_HOST IPAddress(192, 168, 1, 11)
#define MQTT_USER "esp_user"
#define MQTT_PASS "5127"
#define MQTT_PORT 1883
```
**Catatan**: Pengaturan `MQTT_HOST` di atas menggunakan IP lokal _server_ (`192.168.1.11`). Konfigurasi ini digunakan karena perangkat ESP32 dan _server_ (yang menjalankan kontainer Eclipse Mosquitto) terhubung dalam satu jaringan lokal (LAN/WiFi) yang sama.

## 📡 Topik MQTT (Pub/Sub)

Perangkat ESP32 ini menggunakan beberapa topik MQTT untuk komunikasi dua arah dengan *server* (n8n) dan memantau status perangkat. Berikut adalah rincian topik yang digunakan:

### ⚙️ Status Perangkat (Last Will)
* **`device/status`**
  Digunakan sebagai *Last Will and Testament* (LWT). Jika perangkat tiba-tiba terputus dari jaringan atau kehilangan daya, *broker* MQTT akan otomatis mengirimkan pesan *offline* ke topik ini agar *server* mengetahui status perangkat.

### 📤 Publish (Mengirim Data ke Server)
* **`environment/data`**
  Mengirim data pembacaan sensor lingkungan (seperti pH, PPM, suhu, dll). Topik ini mengirimkan data dengan *flag retained* (`true`), sehingga data terakhir akan selalu tersimpan di *broker* dan langsung didapatkan oleh klien yang baru *subscribe*.
* **`pump/locally`**
  Mengirim pembaruan status ketika pompa dihidupkan secara lokal (langsung dari perangkat keras).
* **`tank/notification`**
  Mengirimkan notifikasi peringatan terkait kondisi tangki air atau koreksi jika batas air terlalu rendah.
* **`history/pending`**
  Mengirimkan data riwayat sensor dan pompa lokal yang sebelumnya gagal dikirimkan melalui topik utama (`environment/data` atau `pump/locally`).

### 📥 Subscribe (Menerima Perintah dari Server/Aplikasi)
* **`environment/refresh`**
  Menerima perintah (biasanya dari *logic engine* atau aplikasi *mobile*) untuk segera membaca ulang sensor dan memublikasikan hasilnya.
* **`pump/automation`**
  Menerima perintah untuk menghidupkan pompa secara otomatis (hasil _rule-based_).
* **`pump/manually`**
  Menerima perintah untuk menghidupkan pompa secara manual dari jarak jauh (via mobile app).

## 🔌 Skema Rangkaian (Wiring Diagram)

1. **Regulator 5V**: Menyuplai daya bagi komponen dengan kebutuhan daya besar atau logika 5V (ADS1115, MCP23017, Layar LCD, Relay, dan *microcontroller* ESP32).
2. **Regulator 3.3V**: Dikhususkan untuk menyuplai sensor-sensor dan tombol interaksi yang terhubung langsung ke pin GPIO ESP32, untuk mencegah pembebanan berlebih pada regulator internal ESP32.

### 1. Wiring Diagram Keseluruhan
Gambar di bawah ini menunjukkan wiring keseluruhan antar komponen.
![Wiring Diagram Utama](docs/Wiring Diagram.svg)

---

### 2. Konfigurasi Pin ESP32 (Microcontroller Utama)
![Wiring Diagram ESP32](docs/Wiring Diagram 1.svg)

**Tabel Konfigurasi Pin ESP32**

| Komponen / Modul | Pin Modul | Koneksi Tujuan (Pin/Daya) | Keterangan Tambahan |
| :--- | :--- | :--- | :--- |
| **Daya ESP32** | VIN | LM2596 OUT+ (5V) | - |
| | GND | LM2596 OUT- (5V) | - |
| **Logic Shifter** | AVCC | ESP32 3V3 | - |
| | ASCL | ESP32 D22 | - |
| | ASDA | ESP32 D21 | - |
| | AGND | ESP32 GND | - |
| **DS18B20 (Suhu)** | VCC | LM2596 OUT+ (3.3V) | - |
| | DATA | ESP32 D4 | Resistor *pull-up* 4.7kΩ ke VCC |
| | GND | LM2596 OUT- (3.3V) | - |
| **JSN-SR04T (Tk. Utama)** | 5V | LM2596 OUT+ (3.3V) | - |
| | TRIG | ESP32 D25 | - |
| | ECHO | ESP32 D26 | - |
| | GND | LM2596 OUT- (3.3V) | - |
| **JSN-SR04T (Tk. Nutrisi A)**| 5V | LM2596 OUT+ (3.3V) | - |
| | TRIG | ESP32 D27 | - |
| | ECHO | ESP32 D23 | - |
| | GND | LM2596 OUT- (3.3V) | - |
| **JSN-SR04T (Tk. Nutrisi B)**| 5V | LM2596 OUT+ (3.3V) | - |
| | TRIG | ESP32 D19 | - |
| | ECHO | ESP32 D13 | - |
| | GND | LM2596 OUT- (3.3V) | - |
| **JSN-SR04T (Tk. pH-Up)** | 5V | LM2596 OUT+ (3.3V) | - |
| | TRIG | ESP32 D15 | - |
| | ECHO | ESP32 D18 | - |
| | GND | LM2596 OUT- (3.3V) | - |
| **JSN-SR04T (Tk. pH-Down)**| 5V | LM2596 OUT+ (3.3V) | - |
| | TRIG | ESP32 D16 | - |
| | ECHO | ESP32 D17 | - |
| | GND | LM2596 OUT- (3.3V) | - |
| **Tombol (Reconnect)** | PIN1 | ESP32 D34 | Resistor *pull-down* 12kΩ ke OUT- (3.3V) |
| | PIN2 | LM2596 OUT+ (3.3V) | - |
| **Tombol (Baca Data)** | PIN1 | ESP32 D35 | Resistor *pull-down* 12kΩ ke OUT- (3.3V) |
| | PIN2 | LM2596 OUT+ (3.3V) | - |
| **Tombol (Tambah pH-Up)** | PIN1 | ESP32 VP | Resistor *pull-down* 12kΩ ke OUT- (3.3V) |
| | PIN2 | LM2596 OUT+ (3.3V) | - |
| **Tombol (Tambah pH-Dwn)**| PIN1 | ESP32 VN | Resistor *pull-down* 12kΩ ke OUT- (3.3V) |
| | PIN2 | LM2596 OUT+ (3.3V) | - |
| **Tombol (Tambah Nutrisi)**| PIN1 | ESP32 D32 | Resistor *pull-down* 12kΩ ke OUT- (3.3V) |
| | PIN2 | LM2596 OUT+ (3.3V) | - |
| **LED Power** | + (Anoda) | LM2596 OUT+ (3.3V) | Resistor seri 680Ω |
| | - (Katoda)| ESP32 GND | - |

---

### 3. Konfigurasi Logic Level Shifter
![Wiring Diagram Logic Shifter](docs/Wiring Diagram 2.svg)

**Tabel Konfigurasi Pin Logic Shifter**

| Komponen / Modul | Pin Modul | Koneksi Tujuan (Shifter / Daya) | Keterangan |
| :--- | :--- | :--- | :--- |
| **Daya (LM2596 5V)** | OUT+ (5V) | Shifter BVCC | Sisi tegangan tinggi (5V) |
| | OUT- (5V) | Shifter BGND & AGND | *Common ground* |
| **ESP32** | 3V3 | Shifter AVCC | Sisi tegangan rendah (3.3V) |
| | D21 | Shifter ASDA | Jalur SDA (3.3V) |
| | D22 | Shifter ASCL | Jalur SCL (3.3V) |
| **MCP23017** | VCC | LM2596 OUT+ (5V) | - |
| | GND | LM2596 OUT- (5V) | - |
| | SDA | Shifter BSDA | Jalur SDA (5V) |
| | SCL | Shifter BSCL | Jalur SCL (5V) |
| | RESET | LM2596 OUT+ (5V) | Resistor *pull-up* 12kΩ |
| **ADS1115** | VCC | LM2596 OUT+ (5V) | - |
| | GND | LM2596 OUT- (5V) | - |
| | SDA | Shifter BSDA | Jalur SDA (5V) |
| | SCL | Shifter BSCL | Jalur SCL (5V) |
| **LCD 1602** | VCC | LM2596 OUT+ (5V) | - |
| | GND | LM2596 OUT- (5V) | - |
| | SDA | Shifter BSDA | Jalur SDA (5V) |
| | SCL | Shifter BSCL | Jalur SCL (5V) |

---

### 4. Konfigurasi MCP23017 (I/O Expander)
![Wiring Diagram MCP23017](docs/Wiring Diagram 3.svg)

**Tabel Konfigurasi Pin MCP23017**

| Komponen / Modul | Pin Modul | Koneksi MCP23017 / Daya | Keterangan Tambahan |
| :--- | :--- | :--- | :--- |
| **Buzzer** | + | MCP23017 A0 | - |
| | - | LM2596 OUT- (5V) | - |
| **Relay 8-Channel** | VCC | LM2596 OUT+ (5V) | Daya koil relay |
| | GND | LM2596 OUT- (5V) | - |
| | IN1 - IN7 | MCP23017 A1 - A7 | Saluran relay 1 hingga 7 |
| **LED Heartbeat** | + | MCP23017 B7 | Resistor seri 680Ω |
| | - | LM2596 OUT- (5V) | - |
| **LED Warning** | + | MCP23017 B1 | Resistor seri 680Ω |
| | - | LM2596 OUT- (5V) | - |
| **LED Status WiFi** | + | MCP23017 B2 | Resistor seri 12kΩ |
| | - | LM2596 OUT- (5V) | - |
| **LED Status MQTT** | + | MCP23017 B3 | Resistor seri 12kΩ |
| | - | LM2596 OUT- (5V) | - |
| **LED Baca Data** | + | MCP23017 B4 | Resistor seri 680Ω |
| | - | LM2596 OUT- (5V) | - |
| **LED Beri Larutan** | + | MCP23017 B5 | Resistor seri 680Ω |
| | - | LM2596 OUT- (5V) | - |

---

### 5. Konfigurasi ADS1115 (ADC) & Isolator Daya
![Wiring Diagram ADS1115](docs/Wiring Diagram 4.svg)


**Tabel Konfigurasi Pin ADS1115 dan B0505S-1W**

| Komponen / Modul | Pin Modul | Koneksi ADS1115 / Isolator | Keterangan |
| :--- | :--- | :--- | :--- |
| **B0505S-1W (1)** | IN+ & IN- | LM2596 OUT+ & OUT- (5V) | Input daya utama ke isolator 1 |
| **PH-4502C** | V+ | B0505S(1) OUT+ | Daya terisolasi (positif) |
| | G | ADS1115 A1 & B0505S(1) OUT- | *Ground* referensi ADC & Isolator |
| | Po | ADS1115 A0 | Sinyal analog pH |
| **B0505S-1W (2)** | IN+ & IN- | LM2596 OUT+ & OUT- (5V) | Input daya utama ke isolator 2 |
| **Sensor TDS** | + | B0505S(2) OUT+ | Daya terisolasi (positif) |
| | - | ADS1115 A3 & B0505S(2) OUT- | *Ground* referensi ADC & Isolator |
| | A | ADS1115 A2 | Sinyal analog TDS |

---