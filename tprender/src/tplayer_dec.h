#ifndef _TPLAYER_DEC_H_
#define _TPLAYER_DEC_H_

int tplayer_dec_init(void);
void tplayer_dec_uninit(void);
int tplay_dec_url(char *url);
int tplay_dec_prepare_async(void);
int tplay_dec_start(void);
void tplay_dec_stop(void);
int tplay_dec_reset(void);


#endif
