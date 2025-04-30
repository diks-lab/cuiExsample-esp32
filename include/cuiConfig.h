/*
 * 設定処理(CUI)
 */
#pragma once
#include <Arduino.h>
#include <Streaming.h>
#include <WiFi.h>
#include <EEPROM.h>

#define VERSION 1

const int16_t BS_KEY = 0x0008;
const int16_t TAB_KEY = 0x0009;
const int16_t ENTER_KEY = 0x000D;
const int16_t ESC_KEY = 0x001B;
const int16_t DEL_KEY = 0x007F;
const int16_t HOME_KEY = 0x1B31;
const int16_t INS_KEY = 0x1B32;
const int16_t END_KEY = 0x1B34;
const int16_t PAGEUP_KEY = 0x1B35;
const int16_t PAGEDOWN_KEY = 0x1B36;
const int16_t UP_KEY = 0x1B41;
const int16_t DOWN_KEY = 0x1B42;
const int16_t RIGHT_KEY = 0x1B43;
const int16_t LEFT_KEY = 0x1B44;

#define isExist(s) (String(s).length() > 0)
#define cls() Serial << "\033[3;1H\033[0J"
#define locate(row, col) Serial << "\033[" << row << ";" << col << "H"
#define panic() \
  while (1)     \
    yield();
;

typedef enum
{
  CHECK_NG = 0,
  CHECK_OK,
  CHECK_ESC
} pass_check_t;

typedef enum
{
  RET_ESC = 0,
  RET_EXEC
} return_key_t;

// ---------- クラス
class CConfig
{
private:
  static const int8_t sz_wifi_ssid = 16;
  static const int8_t sz_wifi_key = 16;
  static const int8_t sz_host_name = 20;
  static const int8_t sz_upload_api = 40;
  static const int8_t sz_upload_key = 40;
  static const int8_t sz_user_name = 20;
  static const int8_t sz_target_name = 20;
  static const int8_t sz_pass_code = 4;

  typedef enum
  {
    DISCONNECTED = 0,
    CONNECTED
  } wifi_status_t;

  typedef enum
  {
    NORMAL = 0,
    INVERT,
    UNDERLINE,
    ERROR
  } text_attribute_t;

  typedef struct
  {
    uint8_t num;
    typedef void (*menu)(void);
  } menu_t;

  typedef struct
  {
    int8_t row;
    int8_t col;
    String text;
    text_attribute_t attr;
  } message_t;

  typedef struct
  {
    int version;
    struct
    {
      char ssid[sz_wifi_ssid + 1];
      char key[sz_wifi_key + 1];
    } wifi;
    struct
    {
      char host_name[sz_host_name + 1];
      char upload_api[sz_upload_api + 1];
      char upload_key[sz_upload_key + 1];
      char user_name[sz_user_name + 1];
      char target_name[sz_target_name + 1];
    } server;
    char pass[sz_pass_code + 1];
    char end;
  } config_t;

  // ---------- コンストラクタ
  CConfig()
  {
    m_pMenu[0] = &CConfig::menu00;
    m_pMenu[1] = &CConfig::menu01;
    m_pMenu[2] = &CConfig::menu02;
    m_pMenu[3] = &CConfig::menu03;
    m_pMenu[4] = &CConfig::menu04;
    m_pMenu[5] = &CConfig::menu05;
    m_pMenu[11] = &CConfig::menu11;
    m_pMenu[12] = &CConfig::menu12;
    m_pMenu[13] = &CConfig::menu13;
    m_pMenu[21] = &CConfig::menu21;
    m_pMenu[22] = &CConfig::menu22;
    m_pMenu[23] = &CConfig::menu23;
    m_pMenu[24] = &CConfig::menu24;
    m_pMenu[25] = &CConfig::menu25;
  };

public:
  static CConfig &getInstance(void);

  void init(void);
  void run(void);

  wifi_status_t wifi_status(void) { return _wifi_status; }
  int8_t menu_num() { return _menu_num; }

private:
  void (CConfig::*m_pMenu[30])(); // 関数ポインタの配列

  void menu00(void); // Top Menu
  void menu01(void); // 1.Wifi Menu
  void menu02(void); // 2.Server Menu
  void menu03(void); // 3.Config Info
  void menu04(void); // 4.Change Pass Code
  void menu05(void); // 5.Clear Config
  void menu11(void); // 1-1.wifi ssid Setup
  void menu12(void); // 1-2.wifi key Setup
  void menu13(void); // 1-3.wifi Connect
  void menu21(void); // 2-1.Host Name Setup
  void menu22(void); // 2-2.Upload API Setup
  void menu23(void); // 2-3.Upload KEY Setup
  void menu24(void); // 2-4.User Name Setup
  void menu25(void); // 2-5.Target Name Setup

  return_key_t textInput(uint8_t row, uint8_t col, char *buf, const int8_t size, bool pass = false);

  void wifiScan(char *ssid);
  int16_t keyPress(void);
  pass_check_t passCheck(void);
  bool passCompare(char *pass);

  void wifiConnect(void);
  void ConfigInfo(void);
  return_key_t confirm(const uint8_t row, const uint8_t col);
  void readMemory(void);
  void writeMemory(void);
  void clearConfig(void);
  void printle(uint8_t row, uint8_t col, String str, text_attribute_t attr = NORMAL);

  config_t _emm;
  message_t _err;
  message_t _info;

  wifi_status_t _wifi_status = DISCONNECTED;
  int8_t _menu_num = -1;

  char _ip_address[20];
  bool _read_flag = false;  // EEPROM読み込み完了
  bool _write_flag = false; // EEPROM WRITEフラグ
  bool _ssid_flag = false;  // Wifi ssid/key設定完了
  bool _pass_flag = false;  // Pass Code 認証済
};

extern CConfig &cfg;