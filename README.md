# cuiExsample-esp32
# CUI メニュー

## EEPROM 保存フォーマット
'''
typedef struct
{
  int version;
  struct
  {
    char ssid[16 + 1];
    char key[16 + 1];
  } wifi;
  struct
  {
    char host_name[20 + 1];
    char upload_api[40 + 1];
    char upload_key[40 + 1];
    char user_name[20 + 1];
    char target_name[20 + 1];
  } server;
  char pass[4 + 1];
  char end;
} config_t;
'''
### Tera Term環境
Version : 5.2で確認
[シリアルポート(E)]
*  SPEED: 115200bps
*  Data: 8bit
*  Parity: None
*  StopBit: 1bit
*  Flow : None
[端末(T)]
* 改行コード
   受信 : CR
   送信 : CR
* Local Echo: None

### パスコード
 初期値 : [0000]
  5回失敗で初期化

### メニュー階層
'''
TOP Menu
 1.WiFi Setup
 2.Server Setup
 3.Config View
 4.Change Pass Code
 5.Clear Config

WIFI Menu (Top -> 1.WiFi Setup)
 1.wifi ssid Setup
 2.wifi key Setup
 3.wifi Connect

SERVER Menu (Top -> 2.Server Setup)
 1.Host Name Setup
 2.Upload API Setup
 3.Upload KEY Setup
 4.User Name Setup
 Target Name Setup
'''
### Wifi スキャン
表示最大 5局
