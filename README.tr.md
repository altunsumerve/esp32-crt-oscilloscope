# ESP32 CRT Osiloskop

ESP32 ile sifirdan yapilmis, goruntusu eski bir siyah-beyaz CRT televizyona
kompozit video (lale/RCA) uzerinden **dogrudan** cizilen minik bir osiloskop.
Ekran modulu yok - **TV'nin kendisi ekran.** Analog girisi orneklemliyor, dalgayi
canli ciziyor ve bir potansiyometre/LDR'yi, uretilen bir dalgayi sekillendiren
bir kumanda dugmesine ceviriyor.

> Adim adim yapildi; bu arada bir DAC pini yandi. Bu yuzden video cikisi
> **GPIO26 (DAC2)** uzerinden - DAC1/GPIO25 yapim sirasinda hasar gordu.

<p align="center">
  <img src="docs/images/4.png" width="30%" alt="Amplitude modu – sinüs dalgası">
  <img src="docs/images/1.png" width="30%" alt="Shape modu – kare dalga">
  <img src="docs/images/5.png" width="30%" alt="Shape modu – testere dalgası">
</p>
<p align="center">
  <img src="docs/images/3.png" width="30%" alt="Frequency modu – 1 cycle">
  <img src="docs/images/2.png" width="30%" alt="Live modu – VDC ve VPP ölçümleri">
  <img src="docs/images/6.png" width="30%" alt="ESP32 PAL test ekranı">
</p>

## Ozellikler
- ESP32'den dogrudan CRT TV'ye kompozit (PAL) video
- Siyah zemin uzerinde kalin, okunakli canli trace
- Cift cekirdek: ADC ornekleme core 1'de, cizim core 0'da video karesine kilitli
- Ekran olcumleri: **VDC**, **Vpp**, **frekans**
- Kart uzerindeki **BOOT** dugmesiyle gecilen dort mod:
  - **AMPLITUDE** - pot sinus genligini kontrol eder
  - **FREQUENCY** - pot ekrana kac dalga sigacagini kontrol eder
  - **SHAPE** - pot dalga turunu degistirir: sinus / kare / ucgen / testere
  - **LIVE** - ADC'deki gercek sinyali cizer (mic modul, RC egrisi, pil, vb.)

## Donanim
- ESP32-WROOM gelistirme karti (klasik ESP32, DAC gerekli)
- Kompozit (RCA) girisli, PAL, siyah-beyaz CRT TV
- 1x 100 ohm direnc (video terminasyonu)
- Potansiyometre (~10k) ve/veya LDR + ~10k direnc
- (Opsiyonel) 220 ohm + 2x 1N4148 diyot (giris korumasi)

### Baglanti (ozet)
| Nereden | Nereye |
|---|---|
| GPIO26 | Sari RCA ic pin (video) |
| GND | Sari RCA dis metal |
| 100 ohm | GPIO26 hatti ile GND arasi (terminasyon) |
| Sinyal / pot orta bacak / sensor | GPIO34 (ADC1_CH6) |
| BOOT dugmesi (kart uzerinde) | mod degistirme |

Tam baglanti ve korumali giris kati: [docs/baglanti.md](docs/baglanti.md).

## Derleme (PlatformIO + ESP-IDF 4.4)
```bash
# 1) ucuncu parti bilesenleri indir ve tek satirlik DAC yamasini uygula
./scripts/setup.sh        # Windows: powershell -File scripts/setup.ps1

# 2) derle ve yukle (PlatformIO)
pio run -t upload -t monitor
```
`platformio.ini`, `espressif32@5.4.0`'i sabitler; bu da **ESP-IDF 4.4.x** getirir -
video kutuphanesinin test edildigi surum. Yeni ESP-IDF (5.x) bu kutuphanenin
dayandigi I2S DAC yolunu kaldirdigi icin surumu yukseltme.

## Sinirlar (kendine karsi durust ol)
- **Sadece 0-3.3 V giris.** Bolme/zayiflatma yok. Sebekeyi ya da 3.3 V ustunu asla verme.
- **Dusuk frekans.** Tek tek (one-shot) ADC (hizli I2S yolunu video kullaniyor),
  bu yuzden birkac kHz'e kadar anlamli - DC, pil, ses-benzeri sinyaller, RC
  egrileri icin harika; MHz sinyaller icin degil.
- Daha yuksek gerilim icin yuksek degerli bir bolucu; AC icin bias + coupling
  kondansatoru ekle.

## Krediler & Lisans
Kompozit video sinyali uretimi **bize ait degil** -
[aquaticus/esp32_composite_video_lib](https://github.com/aquaticus/esp32_composite_video_lib)
(GPL-3.0) ve derleme bagimliligi olarak [LVGL](https://github.com/lvgl/lvgl) (MIT)
kullaniliyor. Geri kalan her sey (ornekleme, cizim, font, modlar, arayuz, olcumler)
bu proje icin yazildi. GPL-3.0 koduna dayandigi icin bu proje de
**GPL-3.0-or-later**. Bkz. [CREDITS.md](CREDITS.md) ve [LICENSE](LICENSE).
