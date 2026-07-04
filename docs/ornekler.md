# Kullanım Örnekleri (Potansiyometre & LDR)

Bu iki örnek de aynı pine (GPIO34) bağlanır. GPIO34, giriş sinyalinin/kumandanın
okundugu tek analog pindir. Ikisini ayni anda takip, sadece **hangisinin GND'sini
baglarsan o aktif olur** seklinde secici olarak da kullanabilirsin.

---

## 1) Potansiyometre

Uc bacakli. Ortadaki bacak "yalpalayan" (wiper) uctur.

| Pot bacagi | Nereye |
|---|---|
| Kenar bacak 1 | 3V3 |
| Orta bacak (wiper) | GPIO34 |
| Kenar bacak 2 | GND |

Deger: **10k ideal** (1k-100k arasi olur).

### Nasil kullanilir
- **AMPLITUDE / FREQUENCY / SHAPE** modlarinda pot bir **kumanda dugmesi**dir:
  - AMPLITUDE: pot cevir -> dalga buyur/kucul
  - FREQUENCY: pot cevir -> ekrana daha cok/az dalga sigar
  - SHAPE: pot cevir -> sinus / kare / ucgen / testere arasi gecis
- **LIVE** modunda pot bir **seviye** kaynagidir: cevirdikce VDC 0V<->3.3V degisir
  (cizgi ortada durur, VDC sayisi degisir).

Mod degistirmek icin kart uzerindeki **BOOT** dugmesine bas.

---

## 2) LDR (isik sensoru)

Iki bacakli, yonu farketmez. Yanina bir direnc gerekir (gerilim bolucu icin).

Zincir: **3V3 -> LDR -> orta nokta -> direnc -> GND**, orta noktadan GPIO34'e.

| Baglanti | Bir ucu | Diger ucu |
|---|---|---|
| LDR | 3V3 | Orta nokta |
| Direnc | Orta nokta | GND |
| Jumper | Orta nokta | GPIO34 |

Direnc degeri: **10k onerilir.** Cok kucuk direnc (orn. 220 ohm) ile cikis hep
3.3V'a yapisir ve isik degisse bile kipirdamaz; 10k ile guzel salinir.

### Nasil kullanilir
- **AMPLITUDE** modu en etkileyicisi: LDR'ye **isik tut -> dalga buyur**,
  **golge yap -> dalga kucul.** Yani isikla dalgayi oynatirsin.
- **LIVE** modunda: isik degistikce VDC sayisi degisir (isik sensoru gibi).

---

## Notlar
- Her iki devre de cikisi 0-3.3V arasinda kaldigi icin **guvenlidir**, koruma
  diyoduna gerek yoktur.
- Ayni anda ikisini de bagli tutup GPIO34'u paylastirirsan devreler birbirini
  etkiler; en temizi tek seferde birini kullanmak (ya da GND ile secmek).
