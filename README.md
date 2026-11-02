Kelompok 1 - Node-RED dan ESP32

Tugas ini membuat sistem pemantauan empat ruangan menggunakan ESP32, DHT11, MQTT, dan Node-RED. Data suhu ditampilkan dalam satu chart, kelembapan ditampilkan dengan empat gauge, kemudian setiap ruangan mempunyai dua saklar lampu dan indikator online atau offline.

Instruksi tugas: https://hackmd.io/@OkPpYLjoRcu8iYCOtxZkcA/r1vsD7xaMe

Video presentasi: [Google Drive](https://drive.google.com/file/d/1RXAmBEpFWL_j2R8jiQ2R7CQglyxhyUez/view?usp=sharing)

Video demo alat: [Google Drive](https://drive.google.com/file/d/1WcqoYLmbJet25XF7vnzTUILJZRUzmtnv/view?usp=sharing)

Anggota kelompok

| Nama | NRP | Ruangan |
| --- | --- | --- |
| Dzikrina Azaria Salma | 2124600071 | ruang1 |
| Mochammad Aditya Fariz Ramadhani | 2124600082 | ruang2 |
| Ahmad Kholifiy | 2124600088 | ruang3 |
| Achmad Adam Maulana Bisri | 2124600089 | ruang4 |

Alur sistem

```text
ESP32 ruang1 ─┐
ESP32 ruang2 ─┤
ESP32 ruang3 ─┼── broker MQTT ── Node-RED ── dashboard /ui
ESP32 ruang4 ─┘
```

ESP32 mengirim suhu, kelembapan, dan status ke broker. Perintah lampu dikirim dari dashboard Node-RED ke ESP32 melalui broker yang sama.

Perangkat yang dipakai pada setiap ruangan

| Komponen | Pin ESP32 |
| --- | --- |
| DHT11 DATA | GPIO16 |
| LED lampu 1 | GPIO25 |
| LED lampu 2 | GPIO26 |
| DHT11 VCC | 3,3 V |
| DHT11 GND | GND |

Foto rangkaian saat pengujian:

![Rangkaian ESP32](docs/images/rangkaian.jpg)

Broker dan topic

Broker yang dipakai adalah `broker.mqttdashboard.com` port `1883`. Prefix MQTT kelompok kami adalah `pens/klp1`.

| Topic | Isi data | QoS | Retain |
| --- | --- | --- | --- |
| `pens/klp1/ruangN/temperature` | suhu dalam °C | 0 | tidak |
| `pens/klp1/ruangN/humidity` | kelembapan dalam % | 0 | tidak |
| `pens/klp1/ruangN/lamp1` | `1` atau `0` | 1 | tidak |
| `pens/klp1/ruangN/lamp2` | `1` atau `0` | 1 | tidak |
| `pens/klp1/ruangN/status` | `online` atau `offline` | 0 | ya |

Dashboard mengikuti susunan tugas dosen: satu tab `Gedung`, satu group `Suhu`, dan satu group untuk setiap ruangan. Di dalamnya terdapat satu chart suhu empat garis, empat gauge kelembapan, delapan saklar lampu, dan empat indikator status.

![Flow Node-RED](docs/images/flow.png)

![Dashboard Node-RED](docs/images/dashboard.png)

Bukti akses dashboard dari HP

![Status offline dari HP](docs/images/dashboard-hp-status-offline.jpeg)

![Status online dari HP](docs/images/dashboard-hp-status-online.jpeg)

Laporan: [docs/laporan.pdf](docs/laporan.pdf)

Materi presentasi: [ppt-KEL1.pptx](ppt-KEL1.pptx)

Cara menjalankan

1. Pasang library `EspMQTTClient`, `PubSubClient`, `DHT sensor library`, dan `Adafruit Unified Sensor` di Arduino IDE.
2. Salin `secrets.example.h` menjadi `secrets.h`, lalu isi nama dan password Wi-Fi sendiri. File `secrets.h` tidak ikut diunggah ke GitHub.
3. Buka `esp32/esp32_ruang/esp32_ruang.ino`. Ganti nilai `ROOM` sesuai board yang digunakan, lalu upload ke ESP32.
4. Pasang Node-RED dan paket `node-red-dashboard`.
5. Import `node-red/flows.json`, klik Deploy, lalu buka `http://localhost:1880/ui`.

Hasil pengujian

| Uji | Hasil |
| --- | --- |
| T1 Koneksi ESP32 | berhasil terhubung ke Wi-Fi dan broker MQTT |
| T2 Data empat ruangan | berhasil ditampilkan pada dashboard |
| T3 Chart empat garis | berhasil menampilkan data ruang1 sampai ruang4 |
| T4 Perubahan sensor per ruangan | berhasil; perubahan terlihat pada dashboard dan video |
| T5 Lampu tepat sasaran | berhasil; setiap saklar mengendalikan lampu ruang yang sesuai |
| T6 Mematikan lampu | berhasil; perintah OFF diterima perangkat |
| T7 Payload selain `1` dan `0` diabaikan | berhasil; payload tidak valid tidak mengubah lampu |
| T8 Status semua ruangan online | berhasil ditampilkan pada dashboard |
| T9 Last Will offline | berhasil; status berubah offline saat koneksi diputus |
| T10 Status retained | berhasil; status terakhir tetap tersedia setelah restart |
| T11 Reconnect otomatis | berhasil; perangkat kembali online setelah koneksi pulih |
| T12 Dashboard diakses dari HP | berhasil; dibuktikan dengan dua screenshot HP |

Ruang1 dan ruang2 memakai perangkat fisik, sedangkan ruang3 dan ruang4 menggunakan simulasi. Seluruh skenario T1-T12 telah diuji; bukti tersedia pada dashboard, video, dan screenshot akses dari HP.

Pembagian pekerjaan: Fariz menangani integrasi MQTT, Node-RED, dan ruang2; Adam menangani status LWT serta dokumentasi ruang4.

Flow awal masih menggunakan topic `gedung/...` dan payload lampu `ON/OFF`. Flow tersebut sudah disesuaikan menjadi `pens/klp1/...` dengan payload `1/0` agar sama dengan instruksi dosen.
