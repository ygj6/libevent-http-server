/*
 * pipeline client va curl
 */

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <assert.h>

int main(int argc, char **argv)
{
    int i;
    int ret = 1;
    CURLM *mcurl;
    CURL *handle;
    CURL **handles;
    int url_len;

    int pipeline_req_cnt;
    char pipeline_req_url[512];

    if (argc <= 2) {
        printf("Please input two arguments: count and url\n");
        return -1;
    }
    pipeline_req_cnt = atol(argv[1]);
    url_len = strlen(argv[2]);
    assert(url_len > 0 && url_len < 400);
    memset(pipeline_req_url, 0, sizeof(pipeline_req_url));
    memcpy_s(pipeline_req_url, url_len, argv[2], url_len);

    i = pipeline_req_cnt;

    mcurl = curl_multi_init();
    curl_multi_setopt(mcurl, CURLMOPT_PIPELINING, 1L);
    handles = malloc(sizeof(CURL *) * pipeline_req_cnt);

    while (i) {
        handle = curl_easy_init();
        if (!handle) {
            printf("Error: curl_easy_init\n");
            break;
        }

        sprintf_s(pipeline_req_url + url_len, sizeof(pipeline_req_url) - url_len - 1, "%d", i);
        if (curl_easy_setopt(handle, CURLOPT_URL, pipeline_req_url) != CURLE_OK) {
            printf("Error: curl_easy_setopt(CURLOPT_URL)\n");
            break;
        }

        if (curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L) != CURLE_OK) {
            printf("Error: curl_easy_setopt(CURLOPT_FOLLOWLOCATION)\n");
            break;
        }

        if (curl_multi_add_handle(mcurl, handle) != CURLE_OK) {
            printf("Error: curl_multi_add_handle\n");
            break;
        }
        --i;
        handles[i] = handle;
    }
    if (i)
        return -1;
    
    do {
        if (curl_multi_perform(mcurl, &ret) != CURLE_OK)
            return -1;
    } while(ret);

    for (i = 0; i < pipeline_req_cnt; ++i) {
        curl_multi_remove_handle(mcurl, handles[i]);
        curl_easy_cleanup(handles[i]);
    }
    curl_multi_cleanup(mcurl);

    return 0;
}
