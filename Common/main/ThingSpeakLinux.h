#define THINGSPEAK_OFFLINE_ERROR     0
#define THINGSPEAK_CONNECTION_ERROR  1
#define OPEN_SOCKET_ERROR            2
#define PARAMS_ERROR                 3
#define SEND_OK                      4

char SendDataToThingSpeak(int FieldNo, float * FieldArray, char * Key, int SizeOfKey);
