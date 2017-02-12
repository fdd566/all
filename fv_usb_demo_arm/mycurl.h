#ifndef MY_CURL_H
#define MY_CURL_H

int ToServerGetAuth(char *buffer, const char *FinId, int Type);
int GetLed(char *buffer, const char *FinId, int Type);

#endif