// Dogan Gundogdu - ESP32 OLED Robot Yuzu v3
// Kedi koptu uzgun sahne eklendi. Buralari cok kurcalama.

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Dogan SPI Pinleri
#define DOGAN_RST    22
#define DOGAN_DC      2
#define DOGAN_CS      5
#define MEHMET_BUZZER 4
#define EKRAN_GEN   128
#define EKRAN_YUK    64

#define HIZ_CARPANI  0.8f     

#ifndef PI
#define PI 3.14159265f
#endif

Adafruit_SSD1306 gundogdu_ekran(EKRAN_GEN, EKRAN_YUK, &SPI, DOGAN_DC, DOGAN_RST, DOGAN_CS, 8000000UL);

// --- Ses Ayarlari ---
#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
  void buzzer_kur_dogan()        { ledcAttach(MEHMET_BUZZER, 2000, 10); }
  void ses_ver_mehmet(uint16_t f)  { ledcWriteTone(MEHMET_BUZZER, f); }
#else
  #define BUZZ_CH 0
  void buzzer_kur_dogan()        { ledcSetup(BUZZ_CH, 2000, 10); ledcAttachPin(MEHMET_BUZZER, BUZZ_CH); }
  void ses_ver_mehmet(uint16_t f)  { ledcWriteTone(BUZZ_CH, f); }
#endif

// --- Notalar ---
enum { N_C5 = 523, N_E5 = 659, N_G5 = 784, N_C6 = 1047, N_E6 = 1319, N_G6 = 1568, N_C7 = 2093 };
struct Nota_Dogan { uint16_t frekans; uint16_t sure; };

const Nota_Dogan SES_ACILIS[]     = {{N_C6,70},{0,20},{N_E6,70},{0,20},{N_G6,70},{0,20},{N_C7,180}};
const Nota_Dogan SES_MUTLU[]    = {{N_G6,60},{0,20},{N_C7,60},{0,20},{N_G6,60},{0,20},{N_C7,140}};
const Nota_Dogan SES_SASKIN[] = {{1200,40},{1500,40},{1800,40},{2300,120}};
const Nota_Dogan SES_UYKU[]   = {{N_G5,140},{0,30},{N_E5,140},{0,30},{N_C5,300}};
const Nota_Dogan SES_ASK[]     = {{N_C6,80},{N_E6,80},{N_G6,80},{N_C7,160},{0,40},{N_G6,70},{N_C7,200}};
const Nota_Dogan SES_KIRP[]     = {{N_G6,50},{0,30},{N_C7,110}};
const Nota_Dogan SES_UZGUN[]      = {{N_G5,300},{0,30},{N_E5,300},{0,30},{N_C5,500}};

#define CAL_MEHMET(a) melodi_baslat_gundogdu(a, sizeof(a) / sizeof(a[0]))

const Nota_Dogan* aktif_melodi = nullptr;
uint8_t  melodi_uzunluk = 0, melodi_sira = 0;
bool     melodi_caliyor = false;
uint32_t nota_bitis = 0;

void melodi_baslat_gundogdu(const Nota_Dogan* s, uint8_t n) { aktif_melodi = s; melodi_uzunluk = n; melodi_sira = 0; melodi_caliyor = true; nota_bitis = 0; }

void ses_motoru_dogan(uint32_t suan) {
  if (!melodi_caliyor || suan < nota_bitis) return;
  if (melodi_sira >= melodi_uzunluk) { ses_ver_mehmet(0); melodi_caliyor = false; return; }
  Nota_Dogan n = aktif_melodi[melodi_sira++];
  ses_ver_mehmet(n.frekans);
  nota_bitis = suan + n.sure;
}

// --- Biplemeler ---
bool     konusuyor_dogan = false;
uint32_t konusma_bitis = 0, siradaki_bip = 0, bip_kapanis = 0;

void konusma_baslat_mehmet(const char* yazi, uint32_t suan) {
  konusuyor_dogan = true;
  konusma_bitis = suan + strlen(yazi) * 70 + 500;
  siradaki_bip = suan; bip_kapanis = 0;
}

void konusma_motoru(uint32_t suan) {
  if (!konusuyor_dogan) return;
  if (suan >= konusma_bitis) { konusuyor_dogan = false; ses_ver_mehmet(0); return; }
  if (bip_kapanis && suan >= bip_kapanis) { ses_ver_mehmet(0); bip_kapanis = 0; }
  if (suan >= siradaki_bip) {
    ses_ver_mehmet((uint16_t)random(700, 1700));
    bip_kapanis  = suan + random(30, 55);
    siradaki_bip = suan + random(70, 110);
  }
}

// --- Bagirma Motoru ---
bool     bagiriyor_mu = false;
uint32_t bagirma_bitis = 0, siradaki_bagiris = 0;

void bagirma_motoru(uint32_t suan) {
  if (!bagiriyor_mu) return;
  if (suan >= bagirma_bitis) { bagiriyor_mu = false; ses_ver_mehmet(0); return; }
  if (suan >= siradaki_bagiris) {
    ses_ver_mehmet((uint16_t)random(250, 1100));       
    siradaki_bagiris = suan + random(20, 45);
  }
}

// --- Ruh Halleri Senaryosu ---
enum Dogan_Mod { MOD_NORMAL, MOD_MUTLU, MOD_GOZKIRP, MOD_SASKIN, MOD_ASIK, MOD_SINIRLI, MOD_UZGUN, MOD_UYKULU };
struct Adim_Gundogdu { Dogan_Mod mod; uint16_t sure; const char* yazi; };

const Adim_Gundogdu SENARYO_DOGAN[] = {
  {MOD_NORMAL,    2000, nullptr},
  {MOD_MUTLU,     2000, "Merhaba!"},
  {MOD_GOZKIRP,      1500, nullptr},
  {MOD_SASKIN, 1400, nullptr},
  {MOD_ASIK,      2200, "Seni seviyorum"},
  {MOD_NORMAL,    1500, nullptr},
  {MOD_SINIRLI,     3200, nullptr},
  {MOD_UZGUN,       3500, "Uzgundum..."}, 
  {MOD_UYKULU,    2000, nullptr},
};
const uint8_t SENARYO_UZUNLUK = sizeof(SENARYO_DOGAN) / sizeof(SENARYO_DOGAN[0]);

Dogan_Mod    aktif_mod = MOD_NORMAL;
const char* ekrandaki_yazi = nullptr;
uint8_t  senaryo_sirasi = 0;
uint32_t adim_baslangic = 0, adim_bitis = 0;
bool     ters_renk_gundogdu = false;
uint32_t son_flash_zaman = 0;

float dogan_x = 0, dogan_y = 0;
int   hedef_x = 0, hedef_y = 0;
uint32_t sonraki_bakis = 0, sonraki_kirpma = 0, kirpma_basla = 0;
const uint16_t KIRPMA_SURE = 140;
int titreme_x = 0, titreme_y = 0;                                

static inline int kucuk_olan(int a, int b) { return a < b ? a : b; }
static inline int buyuk_olan(int a, int b) { return a > b ? a : b; }

void renkleri_ters_cevir_mehmet(bool ac) {
  if (ac == ters_renk_gundogdu) return;
  ters_renk_gundogdu = ac;
  gundogdu_ekran.invertDisplay(ac);
}

void senaryo_adimi_baslat(uint8_t i, uint32_t suan) {
  const Adim_Gundogdu& adim = SENARYO_DOGAN[i];
  aktif_mod = adim.mod; ekrandaki_yazi = adim.yazi;
  adim_baslangic = suan; adim_bitis = suan + (uint32_t)(adim.sure * HIZ_CARPANI);

  bagiriyor_mu = false; konusuyor_dogan = false; melodi_caliyor = false;   
  ses_ver_mehmet(0);
  renkleri_ters_cevir_mehmet(false);
  titreme_x = 0; titreme_y = 0;

  switch (aktif_mod) {
    case MOD_MUTLU:     CAL_MEHMET(SES_MUTLU); break;
    case MOD_GOZKIRP:      CAL_MEHMET(SES_KIRP); break;
    case MOD_SASKIN: CAL_MEHMET(SES_SASKIN); break;
    case MOD_ASIK:      CAL_MEHMET(SES_ASK); break;
    case MOD_UYKULU:    CAL_MEHMET(SES_UYKU); break;
    case MOD_UZGUN:       CAL_MEHMET(SES_UZGUN); break; 
    case MOD_SINIRLI:     bagiriyor_mu = true; bagirma_bitis = adim_bitis - 250; siradaki_bagiris = suan; break;
    default: break;
  }
  if (ekrandaki_yazi) konusma_baslat_mehmet(ekrandaki_yazi, suan + 400);
}

// --- Cizim Geometrisi ---
void goz_acik_ciz_dogan(int cx, int cy, int w, int h, int px, int py, int pr) {
  gundogdu_ekran.fillRoundRect(cx - w / 2, cy - h / 2, w, h, kucuk_olan(w, h) / 2, SSD1306_WHITE);
  if (h > 14) {
    int r = kucuk_olan(pr, h / 2 - 2);
    int lim = w / 2 - r - 2;
    px = constrain(px, -lim, lim);
    py = constrain(py, -(h / 2 - r - 2), (h / 2 - r - 2));
    gundogdu_ekran.fillCircle(cx + px, cy + py, r, SSD1306_BLACK);
    int hl = buyuk_olan(2, r / 3);
    gundogdu_ekran.fillCircle(cx + px - r / 3 - 1, cy + py - r / 3 - 1, hl, SSD1306_WHITE);
    gundogdu_ekran.fillCircle(cx + px + r / 3 + 1, cy + py + r / 3 + 1, 1, SSD1306_WHITE);
  }
}

void goz_mutlu_mehmet(int cx, int cy) {
  gundogdu_ekran.fillCircle(cx, cy + 6, 16, SSD1306_WHITE);
  gundogdu_ekran.fillCircle(cx, cy + 6, 12, SSD1306_BLACK);
  gundogdu_ekran.fillRect(cx - 17, cy + 6, 35, 18, SSD1306_BLACK);
}

void goz_kapali_gundogdu(int cx, int cy) { gundogdu_ekran.fillRoundRect(cx - 15, cy + 2, 30, 4, 2, SSD1306_WHITE); }

void kalp_ciz(int cx, int cy, int s) {
  gundogdu_ekran.fillCircle(cx - s, cy - s / 2, s, SSD1306_WHITE);
  gundogdu_ekran.fillCircle(cx + s, cy - s / 2, s, SSD1306_WHITE);
  gundogdu_ekran.fillTriangle(cx - 2 * s + 1, cy - s / 2 + 2, cx + 2 * s - 1, cy - s / 2 + 2, cx, cy + 2 * s - 1, SSD1306_WHITE);
}

void yanak_kizart_dogan(int x, int y) {
  for (int i = 0; i < 3; i++) gundogdu_ekran.drawLine(x + i * 6, y + 4, x + i * 6 + 3, y, SSD1306_WHITE);
}

void gulumseme_ciz(int cx, int cy) {
  gundogdu_ekran.drawCircleHelper(cx - 3, cy, 3, 0x0C, SSD1306_WHITE);
  gundogdu_ekran.drawCircleHelper(cx + 3, cy, 3, 0x0C, SSD1306_WHITE);
}

void goz_sinirli_mehmet(int cx, int cy, bool solMu, int pupX) {
  gundogdu_ekran.fillRoundRect(cx - 17, cy - 20, 34, 40, 17, SSD1306_WHITE);
  if (solMu) gundogdu_ekran.fillTriangle(cx - 19, cy - 22, cx + 19, cy - 22, cx + 19, cy + 2, SSD1306_BLACK);
  else         gundogdu_ekran.fillTriangle(cx + 19, cy - 22, cx - 19, cy - 22, cx - 19, cy + 2, SSD1306_BLACK);
  gundogdu_ekran.fillCircle(cx + pupX, cy + 5, 7, SSD1306_BLACK);
  gundogdu_ekran.fillCircle(cx + pupX - 2, cy + 3, 2, SSD1306_WHITE);
}

void agiz_bagiran_gundogdu() {                            
  for (int i = 0; i < 6; i++) {
    int y1 = (i % 2) ? 58 : 52, y2 = ((i + 1) % 2) ? 58 : 52;
    gundogdu_ekran.drawLine(46 + i * 6 + titreme_x, y1 + titreme_y, 52 + i * 6 + titreme_x, y2 + titreme_y, SSD1306_WHITE);
    gundogdu_ekran.drawLine(46 + i * 6 + titreme_x, y1 + titreme_y + 1, 52 + i * 6 + titreme_x, y2 + titreme_y + 1, SSD1306_WHITE);
  }
}

void goz_uzgun_dogan(int cx, int cy, bool solMu) {
  gundogdu_ekran.fillRoundRect(cx - 15, cy - 10, 30, 24, 12, SSD1306_WHITE);
  if (solMu) gundogdu_ekran.fillTriangle(cx - 17, cy - 12, cx + 5, cy - 12, cx - 17, cy, SSD1306_BLACK);
  else         gundogdu_ekran.fillTriangle(cx + 17, cy - 12, cx - 5, cy - 12, cx + 17, cy, SSD1306_BLACK);
  
  gundogdu_ekran.fillCircle(cx, cy + 4, 5, SSD1306_BLACK);
  gundogdu_ekran.fillCircle(cx - 1, cy + 3, 1, SSD1306_WHITE);
}

// --- Davranis Motoru ---
void davranis_guncelle_mehmet(uint32_t suan) {
  if (suan >= sonraki_bakis) {
    hedef_x = (random(0, 4) == 0) ? 0 : random(-5, 6);
    hedef_y = (random(0, 4) == 0) ? 0 : random(-3, 4);
    sonraki_bakis = suan + random(500, 1400);
  }
  dogan_x += (hedef_x - dogan_x) * 0.35f;
  dogan_y += (hedef_y - dogan_y) * 0.35f;

  if (suan >= sonraki_kirpma && kirpma_basla == 0) {
    kirpma_basla = suan;
    sonraki_kirpma = suan + random(900, 2200);
  }

  if (aktif_mod == MOD_SINIRLI) {                          
    if (suan - son_flash_zaman > 110) { son_flash_zaman = suan; renkleri_ters_cevir_mehmet(!ters_renk_gundogdu); }
    titreme_x = random(-2, 3); titreme_y = random(-2, 3);
  }
  
  if (suan >= adim_bitis) {                            
    senaryo_sirasi = (senaryo_sirasi + 1) % SENARYO_UZUNLUK;
    senaryo_adimi_baslat(senaryo_sirasi, suan);
  }
}

void ekrani_cizdir_dogan(uint32_t suan) {
  gundogdu_ekran.clearDisplay();

  float aciklik = 1.0f;
  if (kirpma_basla) {
    float t = (float)(suan - kirpma_basla) / KIRPMA_SURE;
    if (t >= 1.0f) kirpma_basla = 0;
    else aciklik = fabsf(1.0f - 2.0f * t);
  }

  int cy = 26;
  int lx = 36 + (int)(dogan_x / 2), rx = 92 + (int)(dogan_x / 2);
  int px = (int)dogan_x, py = (int)dogan_y;
  int eh = buyuk_olan(4, (int)(40 * aciklik));
  bool yazi_var_mi = (ekrandaki_yazi != nullptr);

  switch (aktif_mod) {
    case MOD_NORMAL:
      goz_acik_ciz_dogan(lx, cy + (int)(dogan_y / 2), 34, eh, px, py, 11);
      goz_acik_ciz_dogan(rx, cy + (int)(dogan_y / 2), 34, eh, px, py, 11);
      if (!yazi_var_mi) gulumseme_ciz(64, 53);
      break;
    case MOD_MUTLU:
      goz_mutlu_mehmet(lx, cy); goz_mutlu_mehmet(rx, cy);
      yanak_kizart_dogan(16, 42); yanak_kizart_dogan(90, 42);
      if (!yazi_var_mi) gulumseme_ciz(64, 53);
      break;
    case MOD_GOZKIRP:
      goz_acik_ciz_dogan(lx, cy, 34, 40, px, py, 11);
      goz_mutlu_mehmet(rx, cy);
      yanak_kizart_dogan(90, 42);
      gulumseme_ciz(64, 53);
      break;
    case MOD_SASKIN:
      goz_acik_ciz_dogan(lx, cy, 36, 46, 0, 0, 7);
      goz_acik_ciz_dogan(rx, cy, 36, 46, 0, 0, 7);
      gundogdu_ekran.fillCircle(64, 55, 2 + random(0, 3), SSD1306_WHITE);
      break;
    case MOD_ASIK: {
      int s = 6 + (int)(1.5f * (1.0f + sinf(suan / 120.0f)));
      kalp_ciz(lx, cy, s); kalp_ciz(rx, cy, s);
      yanak_kizart_dogan(16, 44); yanak_kizart_dogan(90, 44);
      break; }
    case MOD_UYKULU:
      goz_kapali_gundogdu(lx, cy); goz_kapali_gundogdu(rx, cy);
      gundogdu_ekran.setTextSize(1); gundogdu_ekran.setTextColor(SSD1306_WHITE);
      gundogdu_ekran.setCursor(104, 4 + (int)((suan / 400) % 3) * 3); gundogdu_ekran.print("z");
      gundogdu_ekran.setCursor(112, 12 - (int)((suan / 400) % 3) * 2); gundogdu_ekran.print("Z");
      break;
    case MOD_SINIRLI: {
      int deli_goz = random(-2, 3);
      goz_sinirli_mehmet(lx + titreme_x, cy - 2 + titreme_y, true, deli_goz);
      goz_sinirli_mehmet(rx + titreme_x, cy - 2 + titreme_y, false, deli_goz);
      agiz_bagiran_gundogdu();
      if ((suan / 120) % 2) {                       
        gundogdu_ekran.setTextSize(1); gundogdu_ekran.setTextColor(SSD1306_WHITE);
        gundogdu_ekran.setCursor(2, 2); gundogdu_ekran.print("#!");
        gundogdu_ekran.setCursor(110, 2); gundogdu_ekran.print("!#");
      }
      break; }
    case MOD_UZGUN:
      goz_uzgun_dogan(lx, cy, true);
      goz_uzgun_dogan(rx, cy, false);
      if (!yazi_var_mi) {
        gundogdu_ekran.drawCircleHelper(64, 54, 4, 0x03, SSD1306_WHITE);
      }
      break;
  }

  if (yazi_var_mi) {                                   
    int toplam = strlen(ekrandaki_yazi);
    int gosterilen = kucuk_olan(toplam, (int)((suan - adim_baslangic) / 70));
    gundogdu_ekran.setTextSize(1); gundogdu_ekran.setTextColor(SSD1306_WHITE);
    gundogdu_ekran.setCursor((EKRAN_GEN - 6 * toplam) / 2, 55);
    for (int i = 0; i < gosterilen; i++) gundogdu_ekran.write(ekrandaki_yazi[i]);
  }

  gundogdu_ekran.display();
}

void setup() {
  Serial.begin(115200);
  buzzer_kur_dogan();
  randomSeed(esp_random());

  if (!gundogdu_ekran.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("Ekran yok baglantilari kontrol et"));
    for (;;) delay(1000);
  }
  gundogdu_ekran.clearDisplay();
  gundogdu_ekran.display();

  uint32_t suan = millis();
  sonraki_bakis = suan + 500;
  sonraki_kirpma = suan + 1200;
  senaryo_sirasi = 0;
  senaryo_adimi_baslat(senaryo_sirasi, suan);
  CAL_MEHMET(SES_ACILIS);                                  
}

void loop() {
  static uint32_t son_kare_dogan = 0;
  uint32_t suan = millis();

  davranis_guncelle_mehmet(suan);
  ses_motoru_dogan(suan);
  if (!melodi_caliyor) { konusma_motoru(suan); bagirma_motoru(suan); } 

  if (suan - son_kare_dogan >= 33) {
    son_kare_dogan = suan;
    ekrani_cizdir_dogan(suan);
  }
}
