// gplv2
// jmoore@zedstar.org

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <libsoup/soup.h>

#include "tweet.h"
#include "config.h"

#define log_output(fmtstr, args...) \
  (g_print(PROGNAME ":%s: " fmtstr "\n", __func__, ##args))

static void dump_xml(SoupMessage *msg)
{
  gchar *file_path = NULL;
 
  file_path = g_build_filename ("/tmp/", "twitterPostResponse.xml", NULL);
    
  if (!g_file_set_contents (file_path,
            msg->response_body->data,
            msg->response_body->length,
            NULL)) {
    g_print("Failed to write to filesystem\n");
  }  
  
}

static void auth_callback ( SoupSession *session, 
                            SoupMessage *msg, 
                            SoupAuth *auth, 
                            gboolean retrying, 
                            gpointer data )
{
  tweetObject *tobj;
  
  if (retrying) {
    g_printerr("Check your Twitter username and password as I am having problems authenticating!\n");
  }

  tobj = (tweetObject *)data;
  soup_auth_authenticate (auth, tobj->user, tobj->pass);
  
}

static void post_callback ( SoupSession *session,
                            SoupMessage *msg,
                            gpointer data )
{
  tweetObject *tobj = (tweetObject *)data;
  dump_xml(msg); 
  if (SOUP_STATUS_IS_SUCCESSFUL (msg->status_code)) {
    log_output("Posted update to Twitter");
  }
  else {    
    g_printerr("HTTP failed with response code: %d\n", (msg->status_code));      
  }
  g_free(tobj);
}


tweetObject *make_tweet(gchar *user, gchar *pass, gchar *mac, gchar *name)
{
  tweetObject *tobj;
  
  if ((tobj = (tweetObject *)g_malloc(sizeof(tweetObject))) == NULL) {
    g_printerr("failed to malloc tweetObject!\n");
    exit(EXIT_FAILURE);
  }  
  
  tobj->user = user;
  tobj->pass = pass;
  tobj->formdata = g_strdup_printf ("source=%s&status=%s saw %s with id %s", 
                                    PROGNAME, PROGNAME, name, mac);

  return tobj;  
}

void start_tweet(tweetObject *tobj)
{ 
  SoupSession *session;
  SoupMessage *msg;
  gchar *url = NULL;

  session = soup_session_async_new();

  g_signal_connect (session, "authenticate", G_CALLBACK (auth_callback), tobj); 
  
  url = g_strdup_printf("%s", API_URL);
  
  msg = soup_message_new ("POST", url);
  soup_message_headers_append (msg->request_headers, "X-Twitter-Client", PROGNAME); 
  soup_message_set_request (msg, 
          "application/x-www-form-urlencoded",
          SOUP_MEMORY_TAKE,
          tobj->formdata,
          strlen (tobj->formdata));
  
  g_free(url);
  
  soup_session_queue_message (session, msg, post_callback, tobj);  
  

}