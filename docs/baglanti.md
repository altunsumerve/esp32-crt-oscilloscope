# ESP32 Osiloskop — Bağlantı (Adım Adım)

Şema yok, sadece sırayla "şunu şuraya bağla" listesi. Her adımı yaptıkça işaretle.

**Başlamadan önce: ESP32'nin USB kablosunu ÇIKAR.** Enerjisiz haldeyken bağla, yanlış olursa bir şey yanmaz. Her şey bitince takarsın.

Malzeme: breadboard, ESP32, RCA kablosu/soketi, 1x 100Ω direnç, 1x 220Ω direnç, 2x diyot, jumper kablolar.

---

## BÖLÜM A — Video Bağlantısı (TV Ekranı)

Bu adımlarla ESP32'yi televizyona bağlayarak ekranımızı oluşturuyoruz:

- A1. ESP32 **GPIO26** → sarı RCA fişin **iç pini** (sinyal hattı)
- A2. ESP32 **GND** → sarı RCA fişin **dış metal** kısmı (toprak)
- A3. **100Ω** direnç: bir bacağı GPIO26 hattında, öbür bacağı GND hattında olacak şekilde bağla (sinyal kalitesi/sonlandırma için).

---

## BÖLÜM B — Sinyal girişi + koruma (YENİ, bunu kuracaksın)

Breadboard'da boş bir dikey sıra seç, buna **DÜĞÜM** diyeceğiz. Bütün giriş bağlantıları bu DÜĞÜM etrafında toplanacak.

Sırayla:

- **B1.** 220Ω direncin **bir bacağını** breadboard'da boş bir sıraya tak. Bu sıra **PROB GİRİŞİ** olsun (ölçeceğin sinyalin geleceği yer).

- **B2.** 220Ω direncin **öbür bacağını DÜĞÜM** sırasına tak.
  *(Artık direnç, PROB GİRİŞİ ile DÜĞÜM arasında köprü.)*

- **B3.** Bir jumper: **DÜĞÜM** → ESP32 **GPIO34**.
  *(ADC bu düğümü okuyacak.)*

- **B4. Diyot 1 (D1):**
  - Bantlı ucu (katot) → ESP32 **3V3**
  - Bantsız ucu (anot) → **DÜĞÜM**

- **B5. Diyot 2 (D2):**
  - Bantlı ucu (katot) → **DÜĞÜM**
  - Bantsız ucu (anot) → ESP32 **GND**

- **B6.** Bir jumper: ölçeceğin devrenin **GND'si** → ESP32 **GND** (ortak toprak şart).

> **Diyot bandı = katot.** Üzerindeki gümüş/siyah çizgi hangi uçtaysa o katottur.
> - D1'in çizgili ucu **3V3'e** bakar.
> - D2'nin çizgili ucu **DÜĞÜM'e** bakar.
> Yanlış takarsan koruma çalışmaz. Takmadan önce çizgilere bak.

---

## BÖLÜM C — Dahili test sinyali (opsiyonel, ilk denemede kullan)

Elinde ölçecek sinyal yoksa, kartın kendi ürettiği kare dalgayı görmek için:

- **C1.** Bir jumper: ESP32 **GPIO27** → **PROB GİRİŞİ** sırası (B1'deki yer).

Gerçek bir şey (LDR, potansiyometre, sinyal) ölçerken **bu C1 jumper'ını çıkar.**

---

## BÖLÜM D — Bitirme

- **D1.** Tüm bağlantıları bir kez daha yukarıdan kontrol et (özellikle diyot yönleri).
- **D2.** ESP32'nin **USB'sini tak.**
- **D3.** TV'yi AV moduna al, kontrast/parlaklığı aç.
- **D4.** Ekranda: ızgara + (C1 takılıysa) yaklaşık 5 kare dalga görmelisin.

---

## Bağlantı özeti (tek bakışta)

| Ne | Bir ucu | Diğer ucu |
|---|---|---|
| 220Ω direnç | PROB GİRİŞİ | DÜĞÜM |
| Jumper | DÜĞÜM | GPIO34 |
| Diyot D1 | anot → DÜĞÜM | katot(çizgi) → 3V3 |
| Diyot D2 | anot → GND | katot(çizgi) → DÜĞÜM |
| Jumper (test) | GPIO27 | PROB GİRİŞİ |
| Jumper (toprak) | Ölçülen devre GND | ESP32 GND |

---

## Notlar

- **Sadece 0–3.3V sinyaller.** Bu giriş yükseltmez/bölmez.
- LDR / potansiyometre ölçerken diyotlar (B4, B5) şart değil — onlar zaten 0–3.3V. Diyotlar dışarıdan gelen bilinmeyen sinyaller için.
- **10Ω** ve **"13.7"** direnç bu kurulumda kullanılmıyor.
- Video tarafı (BÖLÜM A) hep aynı kalır, giriş kurarken ona dokunma.
