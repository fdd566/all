#include <string.h>
#include <curl/curl.h>
#include <stdlib.h>
#include "mycurl.h"
#include <unistd.h>
#include <stdio.h>


size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)  
{
	char *buffer = (char *)stream;
	strcpy(buffer,(char *)ptr);
    return size*nmemb;
}

int ToServerGetAuth(char *buffer, const char *FinId, int Type)
{
	CURLcode ret;
	CURL *hnd;
	struct curl_slist *slist1;
	char url_final[4096*2] = {0};
	char *url_0 = "http://101.201.146.53:8888/fv/fvr?";
	char *url_1 = "&222222";
	strcat(url_final,url_0);
	strcat(url_final,FinId);
	strcat(url_final,url_1);
		printf("%s\n",url_final);

	hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_URL, url_final);
	curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION,write_data);
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)buffer);
	ret = curl_easy_perform(hnd);

	curl_easy_cleanup(hnd);
	hnd = NULL;
	slist1 = NULL;	
	return ret;
}

int GetLed(char *buffer, const char *FinId, int Type)
{
	CURLcode ret;
	CURL *hnd;
	struct curl_slist *slist1;
	char url_final[4096*2] = "http://101.201.146.53:8888/fv/led?222222";


	printf("%s\n",url_final);

	hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_URL, url_final);
	curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION,write_data);
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)buffer);
	ret = curl_easy_perform(hnd);

	curl_easy_cleanup(hnd);
	hnd = NULL;
	slist1 = NULL;	
	return ret;
}
