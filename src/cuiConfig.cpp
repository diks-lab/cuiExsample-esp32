/*
 * 設定処理(CUI)
 */
#include <Streaming.h>
#include "cuiConfig.h"

WiFiClient wifi_client;

CConfig &cfg = cfg.getInstance();

// インスタンス生成
CConfig &CConfig::getInstance()
{
  static CConfig instance;
  return instance;
}

// 設定処理(初期化)
void CConfig::init()
{
  cls();
  EEPROM.begin(sizeof(config_t));
  readMemory();

  // SSIDとWiFiKEYがセットされている状態
  if (_ssid_flag)
    wifiConnect();
}

// 設定処理(ループ)
void CConfig::run()
{
  // パスコードチェック
  if (CHECK_OK != cfg.passCheck())
    return;

  printle(1, 1, "Config mode.");

  _menu_num = 0;
  while (_menu_num >= 0)
  {
    // Serial << "\033[4;1H\033[1J\033[1;1Hmenu_num:" << _menu_num << endl;
    if (nullptr != m_pMenu[_menu_num])
    {
      // メニュー実行処理
      (this->*m_pMenu[_menu_num])();
    }
    else
    {
      printle(1, 1, "menu_num:" + String(_menu_num));
      printle(2, 1, "Error! >> Please to Reboot.\033[0K", ERROR);
      panic();
    }
    yield();
  }
}

// *****************************************
// 0.T O P
// *****************************************
// Top Menu
void CConfig::menu00(void)
{
  _menu_num = 0;

  cls();
  uint8_t s_row = 6;
  printle(s_row + 0, 8, "  *****  C O N F I G  ****  ", INVERT);
  printle(s_row + 1, 8, "> 1.WiFi Setup");
  printle(s_row + 2, 8, "  2.Server Setup");
  printle(s_row + 3, 8, "  3.Config Info");
  printle(s_row + 4, 8, "  4.Change Pass Code");
  printle(s_row + 5, 8, "  5.Clear Config");
  printle(s_row + 6, 8, "  6.Exit");
  locate(s_row + 1, 9);

  uint8_t num = 1;
  uint8_t bnum = 1;
  do
  {
    int16_t keyCode = keyPress();
    // Serial.printf("\033[2;1H%04X\r\n", keyCode);
    switch (keyCode)
    {
    case UP_KEY: // [↑]
      bnum = num;
      if (num > 1)
        num--;
      break;
    case DOWN_KEY: // [↓]
      bnum = num;
      if (num < 6)
        num++;
      break;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
      bnum = num;
      num = keyCode - 0x030;
      _menu_num = num;
      break;
    case '6':
    case ESC_KEY: // [Esc]
      bnum = num;
      num = 6;
      _menu_num = -1;
      break;
    case ENTER_KEY: // [Enter]
      bnum = num;
      _menu_num = num;
      switch (num)
      {
      case 1: // 1.WiFi Setup
      case 2: // 2.Server Setup
      case 3: // 3.Config Info
      case 4: // 4.Change Pass Code
      case 5: // 5.Clear Config
        _menu_num = num;
        break;
      case 6: // Exit
        _menu_num = -1;
        break;
      }
    default:
      break;
    }
    if (num != bnum)
    {
      Serial << "\033[" << s_row + bnum << ";" << 8 << "H ";
      Serial << "\033[" << s_row + num << ";" << 8 << "H>";
    }
  } while (_menu_num == 0);
}

// 1.WiFi Setup
void CConfig::menu01()
{
  _menu_num = 10;

  cls();
  uint8_t s_row = 6;
  printle(s_row + 0, 8, "  ****  01.W I F I    ****  ", INVERT);
  printle(s_row + 1, 8, "> 1.Wifi ssid Setup");
  printle(s_row + 2, 8, "  2.Wifi key Setup");
  printle(s_row + 3, 8, "  3.Wifi Connect");
  printle(s_row + 4, 8, "  4.Return");

  if (isExist(_emm.wifi.ssid))
    printle(s_row + 1, 30, ">  " + String(_emm.wifi.ssid));
  if (isExist(_emm.wifi.key))
    printle(s_row + 2, 30, ">  **************");
  if (_wifi_status == CONNECTED)
    printle(s_row + 3, 30, ">  " + String(_ip_address) + "  \033[32mok\033[0m");
  if (_err.row != 0)
  {
    printle(_err.row, _err.col, _err.text, _err.attr);
    memset(&_err, 0, sizeof(message_t));
  }
  locate(s_row + 1, 9);

  uint8_t num = 1;
  uint8_t bnum = 1;
  do
  {
    int16_t keyCode = keyPress();
    switch (keyCode)
    {
    case UP_KEY: // [↑]
      bnum = num;
      if (num > 1)
        num--;
      break;
    case DOWN_KEY: // [↓]
      bnum = num;
      if (num < 4)
        num++;
      break;
    case '1': // 1.wifi ssid Setup
    case '2': // 2.wifi key Setup
    case '3': // 3.wifi Connect
      bnum = num;
      num = keyCode - 0x30;
      _menu_num += num;
      break;
    case '4':
    case ESC_KEY: // [Esc]
      bnum = num;
      num = 4;
      _menu_num = 0;
      break;
    case ENTER_KEY: // [Enter]
      bnum = num;
      if (num < 4)
      {
        _menu_num += num;
      }
      else
      {
        _menu_num = 0;
      }
      break;
    default:
      break;
    }
    if (num != bnum)
    {
      Serial << "\033[" << s_row + bnum << ";" << 8 << "H ";
      Serial << "\033[" << s_row + num << ";" << 8 << "H>";
    }
  } while (_menu_num == 10);
}

// 2.Server Setup
void CConfig::menu02()
{
  _menu_num = 20;

  cls();
  uint8_t s_row = 6;
  printle(s_row + 0, 8, "  ****  02.SERVER     ****  ", INVERT);
  printle(s_row + 1, 8, "> 1.Host Name Setup");
  printle(s_row + 2, 8, "  2.Upload API Setup");
  printle(s_row + 3, 8, "  3.Upload KEY Setup");
  printle(s_row + 4, 8, "  4.User Name Setup");
  printle(s_row + 5, 8, "  5.Target Name Setup");
  printle(s_row + 6, 8, "  6.Return");

  if (isExist(_emm.server.host_name))
    printle(s_row + 1, 30, ">  " + String(_emm.server.host_name));
  if (isExist(_emm.server.upload_api))
    printle(s_row + 2, 30, ">  " + String(_emm.server.upload_api));
  if (isExist(_emm.server.upload_key))
    printle(s_row + 3, 30, ">  " + String(_emm.server.upload_key));
  if (isExist(_emm.server.user_name))
    printle(s_row + 4, 30, ">  " + String(_emm.server.user_name));
  if (isExist(_emm.server.target_name))
    printle(s_row + 5, 30, ">  " + String(_emm.server.target_name));
  locate(s_row + 1, 9);

  uint8_t num = 1;
  uint8_t bnum = 1;
  do
  {
    int16_t keyCode = keyPress();
    switch (keyCode)
    {
    case UP_KEY: // [↑]
      if (1 < num)
      {
        bnum = num--;
      }
      break;
    case DOWN_KEY: // [↓]
      if (6 > num)
      {
        bnum = num++;
      }
      break;
    case '1': // 1.Host Name Setup
    case '2': // 2.Upload API Setup
    case '3': // 3.Upload KEY Setup
    case '4': // 4.User Name Setup
    case '5': // 5.Target Name Setup
      bnum = num;
      num = keyCode - '0';
      _menu_num += num;
      break;
    case '6':
    case ESC_KEY: // [Esc]
      bnum = num;
      num = 6;
      _menu_num = 0;
      break;
    case ENTER_KEY: // [Enter]
      bnum = num;
      if (6 > num)
      {
        _menu_num += num;
      }
      else
      {
        _menu_num = 0;
      }
      break;
    }
    if (num != bnum)
    {
      Serial << "\033[" << s_row + bnum << ";" << 8 << "H ";
      Serial << "\033[" << s_row + num << ";" << 8 << "H>";
    }
  } while (_menu_num == 20);
}

// 3.Config Info
void CConfig::menu03()
{
  ConfigInfo();

  printle(26, 8, "Prease any key.");
  keyPress();
  printle(26, 8, "");
  _menu_num = 0;
}

// 4.Change Pass Code
void CConfig::menu04()
{
  char pass_code[5];

  printle(15, 9, "Change Pass Code.");
  memset(pass_code, 0, sizeof(pass_code));

  if (RET_EXEC == textInput(17, 10, pass_code, sz_pass_code, true))
  {
    // printle(2, 1, pass_code);

    if (4 == strlen(pass_code))
    {
      strcpy(_emm.pass, pass_code);
      _write_flag = true;
      writeMemory();
      _pass_flag = false;
    }
  }
  else
  {
    printle(2, 1, "Please Enter 4 Characters", ERROR);
    delay(2000);
    Serial << "\033[2;1H\033[0K";
  }
  _menu_num = 0;
}

// 5.Clear Config
void CConfig::menu05()
{
  printle(15, 9, "Initialize the Config information.");

  if (RET_EXEC == confirm(17, 9))
    clearConfig();

  Serial << "\033[15;8H";
  _menu_num = 0;
}

// *****************************************
// 1.W I F I
// *****************************************
// 1-1.wifi ssid Setup
void CConfig::menu11()
{
  int8_t s_row = 6;

  printle(12, 1, "Please input the WiFi SSID.  [Tab] key press to view scanned information");
  textInput(13, 1, _emm.wifi.ssid, sz_wifi_ssid);
  if (_write_flag)
  {
    printle(s_row + 1, 30, ">  " + String(_emm.wifi.ssid));
    memset(_ip_address, 0, sizeof(_ip_address));
    printle(s_row + 3, 30, String(_ip_address));
    writeMemory();
    _ssid_flag = isExist(_emm.wifi.ssid) && isExist(_emm.wifi.key);
    _wifi_status = DISCONNECTED;
  }
  _menu_num = 1;
}

// 1-2.wifi key Setup
void CConfig::menu12()
{
  int8_t s_row = 6;

  printle(13, 1, "Please input the WiFi KEY.");
  textInput(14, 3, _emm.wifi.key, sz_wifi_key, true);
  if (_write_flag)
  {
    if (isExist(_emm.wifi.key))
      printle(s_row + 2, 30, ">  **************");
    memset(_ip_address, 0, sizeof(_ip_address));
    printle(s_row + 3, 30, String(_ip_address));
    writeMemory();
    _ssid_flag = isExist(_emm.wifi.ssid) && isExist(_emm.wifi.key);
    _wifi_status = DISCONNECTED;
  }
  _menu_num = 1;
}

// 1-3.wifi Connect
void CConfig::menu13()
{
  int8_t s_row = 6;

  if (_ssid_flag)
  {
    wifiConnect();
    if (_wifi_status != CONNECTED)
    {
      _err.row = s_row + 3;
      _err.col = 32;
      _err.text = "Connect Error!";
    }
  }
  else
  {
    _err.row = 15;
    _err.col = 7;
    _err.text = "** No Wifi SSID and KEY are set.";
  }
  _menu_num = 1;
}

// *****************************************
// 2.S E R V E R
// *****************************************
// 2-1.Host Name Setup
void CConfig::menu21()
{
  printle(14, 1, "Please input the Host Name. ");
  textInput(15, 3, _emm.server.host_name, sz_host_name);
  writeMemory();
  printle(7, 28, ">  " + String(_emm.server.host_name));
  _menu_num = 2;
}

// 2-2.Upload API Setup
void CConfig::menu22()
{
  printle(14, 1, "Please input the Upload API. ");
  textInput(15, 3, _emm.server.upload_api, sz_upload_api);
  writeMemory();
  printle(8, 28, ">  " + String(_emm.server.upload_api));
  _menu_num = 2;
}

// 2-3.Upload KEY Setup
void CConfig::menu23()
{
  printle(14, 1, "Please input the Upload KEY. ");
  textInput(15, 3, _emm.server.upload_key, sz_upload_key);
  writeMemory();
  printle(9, 28, ">  " + String(_emm.server.upload_key));
  _menu_num = 2;
}

// 2-4.User Name Setup
void CConfig::menu24()
{
  printle(14, 1, "Please input the User Name. ");
  textInput(15, 3, _emm.server.user_name, sz_user_name);
  writeMemory();
  printle(10, 28, ">  " + String(_emm.server.user_name));
  _menu_num = 2;
}

// 2-5.Target Name Setup
void CConfig::menu25()
{
  printle(14, 1, "Please input the Target Name. ");
  textInput(15, 3, _emm.server.target_name, sz_target_name);
  writeMemory();
  printle(11, 28, ">  " + String(_emm.server.target_name));
  _menu_num = 2;
}
// **********************************************************
// textInput
return_key_t CConfig::textInput(uint8_t row, uint8_t col, char *buf, int8_t size, bool pass)
{
  return_key_t ret;
  int16_t keyCode;
  char text_buf[50];
  char passText[21];
  int len = 0; // 入力したテキストの長さ
  int pos = 0; // カーソル位置

  memset(text_buf, 0, sizeof(text_buf));
  memcpy(text_buf, buf, size);
  _write_flag = false;
  len = strlen(text_buf);

  printle(row, col, "> ");
  col += 2;

  if (false == pass)
    Serial << text_buf;
  else
  {
    memset(passText, '*', len);
    passText[len] = '\0';
    Serial << passText;
  }
  locate(row, col);

  do
  {
    keyCode = keyPress();
    // Serial.printf("\033[1;1H%04X\r\n", keyCode);
    switch (keyCode)
    {
    case ENTER_KEY:
      if (memcmp(buf, text_buf, size))
        _write_flag = true;
      memcpy(buf, text_buf, size);
      ret = RET_EXEC;
      break;
    case ESC_KEY:
      ret = RET_ESC;
      len = 0;
      break;
    case LEFT_KEY:
      if (pos == 0)
        break;
      pos--;
      break;
    case RIGHT_KEY:
      if (pos == len)
        break;
      pos++;
      break;
    case DEL_KEY:
      len = 0;
      pos = 0;
      memset(text_buf, 0, sizeof(text_buf));
      _write_flag = true;
      break;
    case INS_KEY:
      break;
    case TAB_KEY:
      if (11 == _menu_num) // 1-1.wifi ssid Setup
      {
        wifiScan(text_buf);
        len = strlen(text_buf);
        pos = len;
      }
      break;
    case BS_KEY:
      if (0 == pos)
        break;
      pos--;
      for (int i = pos; i < len; i++)
      {
        text_buf[i] = text_buf[i + 1];
      }
      // Serial.printf("\033[4;1Hpos:%d len:%d ", pos, len);
      len--;
      break;
    default:
      if (0x7E < keyCode)
        break;
      if (0x21 > keyCode)
        break;
      if (pos >= size)
        break;
      if (pos >= size || len >= size)
        break;
      if (pos < len)
      {
        for (int i = len; i > pos; i--)
        {
          text_buf[i] = text_buf[i - 1];
        }
      }
      len++;
      // Serial.printf("\033[4;1Hpos:%d len:%d ", pos, len);
      text_buf[pos] = (char)keyCode;
      pos++;
      break;
    }
    text_buf[len] = '\0';

    if (false == pass)
      printle(row, col, String(text_buf));
    else
    {
      memset(passText, '*', len);
      passText[len] = '\0';
      printle(row, col, String(passText));
    }
    // printle(row + 1, col - 1, "[" + String(text_buf) + "]"); // Test
    locate(row, col + pos);
  } while (keyCode != ENTER_KEY && keyCode != ESC_KEY);
  return ret;
}

// Wifi Scan
void CConfig::wifiScan(char *ssid)
{
  printle(16, 8, "Scan for WiFi...          Max.5");
  printle(17, 8, "--  ---------------------------");
  locate(18, 8);

  int16_t scan_res = WiFi.scanNetworks();

  if (0 < scan_res)
  {
    if (5 < scan_res)
      scan_res = 5;
    int i;
    for (i = 0; i < scan_res; i++)
    {
      printle(18 + i, 9, String(1 + i) + "：" + WiFi.SSID(i) + " (" + WiFi.RSSI(i) + "dBm)");
    }
    // 発見したwifiAPを表示
    printle(18 + i, 8, "--  ---------------------------");
    printle(19 + i, 9, String(scan_res) + " Wifi SSIDs found. [   ]");
  }
  else
  {
    printle(18, 8, "**  No Wifi found.", ERROR);
  }

  do
  {
    locate(19 + scan_res, 31);
    int16_t keyCode = keyPress();
    if (keyCode == ESC_KEY)
    {
      *ssid = '\0';
      break;
    }
    else if (scan_res > 0 && keyCode >= '1' && keyCode <= '1' + scan_res)
    {
      uint8_t num = keyCode - '1';
      strcpy(ssid, WiFi.SSID(num).c_str());
      break;
    }
  } while (1);

  locate(16, 1);
  Serial << "\033[0J";
}

// キー入力
int16_t CConfig::keyPress()
{
  int16_t keyCode = 0;
  char buf[4] = {0}, *esc;
  do
  {
    if (Serial.available())
    {
      byte inKey = (byte)Serial.read();
      if (inKey == ESC_KEY)
      {
        esc = buf;
        keyCode = inKey;
        int8_t n = Serial.available();
        if (1 < n)
        {
          while (n--)
          {
            *esc++ = Serial.read();
          }
          keyCode = (keyCode << 8) | buf[1];
        }
        // Serial.printf("[ESC] %02X %02X\r\n", buf[0], buf[1]);
      }
      else
      {
        keyCode = (int16_t)inKey;
      }
    }
    yield();
  } while (0 == keyCode);
  return keyCode;
}

// パスコードチェック
pass_check_t CConfig::passCheck(void)
{
  int8_t retry = 5; // リトライ回数
  return_key_t ret;
  char pass_code[5];

  Serial << "\033[2J";
  while (0 < retry--)
  {
    printle(13, 9, "Prease Input Pass Code. If you fail 5 times, the Config to clear.");

    memset(pass_code, 0, sizeof(pass_code));
    switch (textInput(15, 9, pass_code, sz_pass_code, true))
    {
    case RET_ESC:
      return CHECK_ESC;
    case RET_EXEC:
      if (4 == strlen(pass_code))
        if (passCompare(pass_code))
          return CHECK_OK;
    }
  }

  // リトライ回数を超えたらConfig情報を初期化
  memset(pass_code, 0, sizeof(pass_code));
  locate(13, 1);
  Serial << "\033[2J";
  printle(2, 1, "Retry limit exceeded.", ERROR);
  printle(3, 1, "Initialize the EEPROM Config.", ERROR);
  delay(2000);
  clearConfig();
  printle(4, 1, "Config has been initialized.", ERROR);

  return CHECK_NG;
}

// パスコード比較
bool CConfig::passCompare(char *pass_code)
{
  _pass_flag = true;
  char *p1 = pass_code;
  char *p2 = _emm.pass;
  Serial.printf("p1:%s p2:%s\r\n", p1, p2);

  for (int i = 0; i < 4; i++)
  {
    if (*p1++ != *p2++)
      _pass_flag = false;
  }
  return _pass_flag;
}

// Wifi接続
void CConfig::wifiConnect(void)
{
  _wifi_status = DISCONNECTED;
  uint8_t row;
  uint8_t col;
  uint8_t bmenu;
  if (0 > _menu_num) // Run Mode
  {
    row = 4;
    col = 1;
    bmenu = _menu_num;
  }
  else // Config Mode
  {
    row = 12;
    col = 8;
    bmenu = 1;
  }

  printle(row, col, "WiFi Connecting..");

  if (!_ssid_flag)
  {
    _err.row = row + 1;
    _err.col = col;
    _err.text = "** Wifi ssid and key are not set.";
    _menu_num = bmenu;
    return;
  }
  WiFi.begin(_emm.wifi.ssid, _emm.wifi.key);
  int erc = 0;
  locate(row + 1, col);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if (erc++ > 10)
    {
      WiFi.disconnect();
      break;
    }
    _menu_num = bmenu;
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED)
  {
    _wifi_status = CONNECTED;
    printle(row + 2, col, "WiFi Connected.");

    strcpy(_ip_address, WiFi.localIP().toString().c_str());
  }
}

// 設定情報表示
void CConfig::ConfigInfo()
{
  uint8_t s_row = 14;
  printle(s_row + 0, 8, "  ****  Information  ****  ", UNDERLINE);
  printle(s_row + 1, 8, "Wifi ssid : ");
  if (isExist(_emm.wifi.ssid))
    printle(s_row + 1, 20, _emm.wifi.ssid);
  else
    printle(s_row + 1, 20, "(NoN)", ERROR);

  printle(s_row + 2, 8, "Wifi key  : ");
  if (isExist(_emm.wifi.key))
    printle(s_row + 2, 20, "***************");
  else
    printle(s_row + 2, 20, "(NoN)", ERROR);

  printle(s_row + 3, 8, "IP Address: ");
  if (isExist(_ip_address))
    printle(s_row + 3, 20, _ip_address);
  else
    printle(s_row + 3, 20, "Un Connected", ERROR);

  printle(s_row + 5, 8, "Host Name : ");
  if (isExist(_emm.server.host_name))
    printle(s_row + 5, 20, _emm.server.host_name);
  else
    printle(s_row + 5, 20, "(NoN)", ERROR);

  printle(s_row + 6, 8, "Upload API: ");
  if (isExist(_emm.server.upload_api))
    printle(s_row + 6, 20, _emm.server.upload_api);
  else
    printle(s_row + 6, 20, "(NoN)", ERROR);

  printle(s_row + 7, 8, "Upload KEY: ");
  if (isExist(_emm.server.upload_key))
    printle(s_row + 7, 20, _emm.server.upload_key);
  else
    printle(s_row + 7, 20, "(NoN)", ERROR);

  printle(s_row + 8, 8, "User Name : ");
  if (isExist(_emm.server.user_name))
    printle(s_row + 8, 20, _emm.server.user_name);
  else
    printle(s_row + 8, 20, "(NoN)", ERROR);

  printle(s_row + 9, 8, "Target ID : ");
  if (isExist(_emm.server.target_name))
    printle(s_row + 9, 20, _emm.server.target_name);
  else
    printle(s_row + 9, 20, "(NoN)", ERROR);

  //  printle(s_row + 10, 8, "Pass Code : ");
  //  printle(s_row + 10, 20, _emm.pass);
}

// 確認処理
return_key_t CConfig::confirm(uint8_t row, uint8_t col)
{
  return_key_t ret = RET_ESC;
  uint8_t keyCode = 0;

  printle(row, col, "[Yes] Enter     [No] Esc");
  locate(row, col);
  do
  {
    keyCode = keyPress();
    if (ENTER_KEY == keyCode)
      ret = RET_EXEC;
  } while (ENTER_KEY != keyCode && ESC_KEY != keyCode);

  printle(row, col, "");
  _menu_num = 0;

  return ret;
}

// EEPROM 設定情報読み込み
void CConfig::readMemory(void)
{
  EEPROM.get(0, _emm);
  if (_emm.version == VERSION && _emm.end == '$')
  {
    _read_flag = true;
  }
  else
  {
    printle(2, 1, "config read error!", ERROR);
    clearConfig();
  }

  _ssid_flag = (isExist(_emm.wifi.ssid) && isExist(_emm.wifi.key));
}

// EEPROM 設定情報書き込み
void CConfig::writeMemory(void)
{
  printle(2, 1, "");
  if (!_write_flag)
    return;

  printle(2, 1, "writeMemory");
  EEPROM.put(0, _emm);
  EEPROM.commit();
  _write_flag = false;
  delay(1000);
  printle(2, 1, "");
}

// 設定情報初期化
void CConfig::clearConfig(void)
{
  memset(&_emm, 0, sizeof(_emm));
  memset(_ip_address, 0, sizeof(_ip_address));
  memset(_emm.pass, '0', sz_pass_code);
  _emm.version = VERSION;
  _emm.end = '$';
  _ssid_flag = false;
  _wifi_status = DISCONNECTED;

  writeMemory();
}

// 文字列表示
void CConfig::printle(uint8_t row, uint8_t col, String str, text_attribute_t attr)
{
  locate(row, col);
  switch (attr)
  {
  case NORMAL:
    Serial.print("\033[39m");
    break;
  case INVERT:
    Serial.print("\033[39m\033[7m");
    break;
  case ERROR:
    Serial.print("\033[31m");
    break;
  case UNDERLINE:
    Serial.print("\033[4m\033[37m");
    break;
  }
  Serial << str << "\033[0m\033[0K";
}
