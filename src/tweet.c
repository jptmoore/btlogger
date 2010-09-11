// License: GPLv3
// Copyright 2010 John P. T. Moore
// jmoore@zedstar.org

#include <rest/oauth-proxy.h>
#include <stdio.h>

#include "tweet.h"
#include "config.h"

#define log_output(fmtstr, args...) \
  (g_print(PROGNAME ":%s: " fmtstr "\n", __func__, ##args))


RestProxy *authenticate(gchar *key, gchar *secret)
{
  RestProxy *proxy;
  GError *error = NULL;  
  char pin[256];

  /* Create the proxy */
  proxy = oauth_proxy_new (key, secret, "https://twitter.com/", FALSE);

  /* First stage authentication, this gets a request token */
  if (!oauth_proxy_request_token (OAUTH_PROXY (proxy), "oauth/request_token", "oob", &error))
    g_error ("Cannot get request token: %s", error->message);

  /* From the token construct a URL for the user to visit */
  g_print ("Go to http://twitter.com/oauth/authorize?oauth_token=%s then enter the PIN\n",
	   oauth_proxy_get_token (OAUTH_PROXY (proxy)));

  fgets (pin, sizeof (pin), stdin);
  g_strchomp (pin);

  /* Second stage authentication, this gets an access token */
  if (!oauth_proxy_access_token (OAUTH_PROXY (proxy), "oauth/access_token", pin, &error))
    g_error ("Cannot get access token: %s", error->message);

  return proxy;
}

int tweet(RestProxy *proxy, gchar *message)
{
  RestProxyCall *call = NULL;
  GError *error = NULL;

  call = rest_proxy_new_call (proxy);
  rest_proxy_call_set_function (call, "statuses/update.xml");
  rest_proxy_call_set_method (call, "POST");
  rest_proxy_call_add_param (call, "status", message);

  if (!rest_proxy_call_sync (call, &error))
    g_error ("Cannot make call: %s", error->message);

  //g_print ("%s\n", rest_proxy_call_get_payload (call));

  g_object_unref (call);

  log_output("Posted update to Twitter");

  return 0;
}

