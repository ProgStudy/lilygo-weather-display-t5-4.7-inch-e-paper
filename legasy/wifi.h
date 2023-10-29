
#define COUNT_MAX_LIST       = 4 //максимальное кол-во подключений
#define FIRST_ADDR_EEPROM    = 3 //начальный адрес eeprom 

// Данные на подключение wifi сети к устройству 
#define SSID    = "LILYGO-T5-4.7"
#define PASSWD  = "Rf3G72"

class ServiceWifi
{
    public:

        int[] addresses;

        struct data {
            char* ssid;
            char* password;
        };

        ServiceWifi() {
            //инициируем сервис
            addresses = new int[COUNT_MAX_LIST - 1];

            for (int i = 0; i < COUNT_MAX_LIST) {
                addresses[i] = FIRST_ADDR_EEPROM + i;
            }
        }

        addPoint(char* ssid, char* password) {

        }

    private:
}