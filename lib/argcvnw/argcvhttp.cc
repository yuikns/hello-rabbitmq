#include "argcvhttp.h"
#include <string>
#include <cstring>
#include <cstdlib>

#include <cstdio>

#ifndef ARGCV_DEFAULT_FILE
#define ARGCV_DEFAULT_FILE "/"
#endif // ARGCV_DEFAULT_FILE

#ifndef ARGCV_DEFAULT_HTTP_PROTOCOL
#define ARGCV_DEFAULT_HTTP_PROTOCOL 0
#endif // ARGCV_DEFAULT_HTTP_PROTOCOL

#ifndef ARGCV_USER_AGENT
#define ARGCV_USER_AGENT "ArgcvSpider/0.5 "
#endif //ARGCV_USER_AGENT

char * argcv_http_request_get_url(const char * url)
{
	if(url == NULL) return NULL;
	//http://localhost/http/http-messages.html
	int url_len = strlen(url);
	bool found = false;
	bool host_stop_found = false;
	size_t offset = 0;
	int start_offset = 0;
	for(int i = 0 ; i < 7 && i < url_len - 1; i ++ )
	{
		if(url[i] == '/' && url[i+1] == '/') start_offset = i + 2;
	}
	
	int host_stop_offset = 0;
	for(int i = start_offset; i < url_len ; i ++ )
	{
		if(url[i] == '/')
		{
			offset = i;
			found = true;
			i ++ ;
			break;
		}else if(!host_stop_found && url[i] == ':')
		{
			host_stop_offset = i;
			host_stop_found = true;
		}
	}
	std::string s;
	s.append("GET ").append(found?(char * )(url+offset):ARGCV_DEFAULT_FILE).append(" HTTP/1.").append(std::to_string(ARGCV_DEFAULT_HTTP_PROTOCOL)).append("\r\n");
	
	if(host_stop_found)
	{
		s.append("HOST: ").append(url+start_offset,host_stop_offset-start_offset).append("\r\n");
	}else if(found)
	{
		s.append("HOST: ").append(url+start_offset,offset-start_offset).append("\r\n");
	}else
	{
		s.append("HOST: ").append(url+start_offset).append("\r\n");
	}
	s.append("User-Agent: ").append(ARGCV_USER_AGENT).append("\r\n"); // User-Agent
	//User-Agent: curl/7.16.3 libcurl/7.16.3 OpenSSL/0.9.7l zlib/1.2.3
	s.append("\r\n");
	size_t str_size = s.size();
	char * rt_str = (char * ) malloc((str_size + 1)* sizeof(char));
	rt_str[str_size] = '\0';
	memcpy(rt_str,s.c_str(),str_size);
	return rt_str;
}

int url_parse(const char * url,char * host , char * port)
{
	if(url == NULL) return 0;
	//http://localhost/http/http-messages.html
	int url_len = strlen(url);
	bool found = false;
	bool host_stop_found = false;
	size_t offset = 0;
	int start_offset = 0;
	for(int i = 0 ; i < 7 && i < url_len - 1; i ++ )
	{
		if(url[i] == '/' && url[i+1] == '/') start_offset = i + 2;
	}
	
	int host_stop_offset = 0;
	for(int i = start_offset; i < url_len ; i ++ )
	{
		if(url[i] == '/')
		{
			offset = i;
			found = true;
			i ++ ;
			break;
		}else if(!host_stop_found && url[i] == ':')
		{
			host_stop_offset = i;
			host_stop_found = true;
		}
	}
	int end_offset = found ? host_stop_found?host_stop_offset:offset : url_len;
	if(host_stop_found)
	{
		memcpy(host,url+start_offset,sizeof(char) * (end_offset - start_offset));
		host[end_offset - start_offset] = '\0';
		memcpy(port,url+host_stop_offset+1,sizeof(char) * (end_offset-host_stop_offset-1));
		port[end_offset-host_stop_offset-1] = '\0';
	}else
	{
		memcpy(host,url+start_offset,sizeof(char) * (end_offset - start_offset));
		host[end_offset - start_offset] = '\0';
		memcpy(port,"80",3);
	}
	printf("ok : %s ~ %s\n",host,port);
	return 1;
}




