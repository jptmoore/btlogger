#ifndef TWEET_H_
#define TWEET_H_

#endif /*TWEET_H_*/

typedef struct {
  gchar *user;
  gchar *pass;
  gchar *formdata;
} tweetObject;

tweetObject *make_tweet(gchar *user, gchar *pass, gchar *mac, gchar *name);
void start_tweet(tweetObject *tobj);