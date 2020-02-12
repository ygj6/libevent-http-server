#include <iostream>
#include <string>
#include <event.h>
#include <event2/http.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>
#include <future>
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#pragma comment (lib, "ws2_32.lib")

void http_req_gencb(struct evhttp_request* req, void* arg) {
	std::thread([req]() {
		auto uri = evhttp_request_get_evhttp_uri(req);
		const char *path = evhttp_uri_get_path(uri);
		auto inbuff = evhttp_request_get_input_buffer(req);
		auto sz = evbuffer_get_length(inbuff);
		auto data = evbuffer_pullup(inbuff, sz);
		LOG(INFO) << "rvc req:" << path << " content:" << std::string((char*)data, sz);

		SYSTEMTIME st;
		GetLocalTime(&st);
		struct evbuffer* buf = evbuffer_new();

		evbuffer_add_printf(buf, "request path: %s, time: %d-%d-%d-%d\n", path, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		evhttp_send_reply(req, HTTP_OK, "OK", buf);
		evbuffer_free(buf);
	}).detach();

}

struct bufferevent* bev_cb(struct event_base *base, void *)
{
	return bufferevent_socket_new(base, -1, BEV_OPT_THREADSAFE);
}


int main()
{
	WSADATA wsdata = {};
	WSAStartup(0x0202, &wsdata);
	google::InitGoogleLogging("tt");
	FLAGS_alsologtostderr = true;
	evthread_use_windows_threads();
	struct event_config* cfg = event_config_new();
	event_config_set_flag(cfg, EVENT_BASE_FLAG_STARTUP_IOCP);
	event_config_set_num_cpus_hint(cfg, 8);
	struct event_base* base = event_base_new_with_config(cfg);
	event_config_free(cfg);

	struct evhttp* http = evhttp_new(base);
	evhttp_bind_socket(http, "0.0.0.0", 6790);
	evhttp_set_bevcb(http, bev_cb, http);
	evhttp_set_gencb(http, http_req_gencb, http);

	LOG(INFO) << "start http server";
	event_base_dispatch(base);

	event_base_free(base);
	LOG(INFO) << "close http server";
	WSACleanup();
	return 0;
}
