#include "tplayer_dec.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <semaphore.h>
 
#include <tplayer.h>

#define TOTAL_VIDEO_AUDIO_NUM 		100
#define MAX_FILE_NAME_LEN 			256
#define URL_MAX						1024

static int semTimedWait(sem_t* sem, int64_t time_ms);
static int CallbackForTPlayer(void* pUserData, int msg, int param0, void* param1);

typedef struct DemoPlayerContext
{
    TPlayer*        mTPlayer;
    int                 mSeekable;
    int                 mError;
    int                 mVideoFrameNum;
    bool              mPreparedFlag;
    bool              mLoopFlag;
    bool              mSetLoop;
    bool              mComplete;
    char              mUrl[URL_MAX];
    MediaInfo*    mMediaInfo;
    char              mVideoAudioList[TOTAL_VIDEO_AUDIO_NUM][MAX_FILE_NAME_LEN];
    int                 mCurPlayIndex;
    int                 mRealFileNum;
    sem_t             mPreparedSem;
}DemoPlayerContext;

DemoPlayerContext demoPlayer;
DemoPlayerContext gTwoDemoPlayer[2];

int tplayer_dec_init(void)
{
	int ret;
	
	memset(&demoPlayer, 0, sizeof(DemoPlayerContext));
    demoPlayer.mError = 0;
    demoPlayer.mSeekable = 1;
    demoPlayer.mPreparedFlag = 0;
    demoPlayer.mLoopFlag = 0;
    demoPlayer.mSetLoop = 0;
    demoPlayer.mMediaInfo = NULL;
	
	demoPlayer.mTPlayer= TPlayerCreate(CEDARX_PLAYER);
    if(demoPlayer.mTPlayer == NULL)
    {
        printf("can not create tplayer, quit.\n");
        return -1;
    }
	
	TPlayerSetNotifyCallback(demoPlayer.mTPlayer,CallbackForTPlayer, (void*)&demoPlayer);
	
    if(((access("/dev/zero",F_OK)) < 0)||((access("/dev/fb0",F_OK)) < 0)){
        printf("/dev/zero OR /dev/fb0 is not exit\n");
    }else{
        system("dd if=/dev/zero of=/dev/fb0");//clean the framebuffer
    }
	
	sem_init(&demoPlayer.mPreparedSem, 0, 0);
	
	return 0;
}

void tplayer_dec_uninit(void)
{
	if(demoPlayer.mTPlayer != NULL)
    {
        TPlayerDestroy(demoPlayer.mTPlayer);
        demoPlayer.mTPlayer = NULL;
        printf("TPlayerDestroy() successfully\n");
    }

    printf("destroy tplayer \n");

    sem_destroy(&demoPlayer.mPreparedSem);
	
	printf("tplaydemo exit\n");
}

int tplay_dec_url(char *url)
{
	int ret;
	
	memset(demoPlayer.mUrl, 0, URL_MAX);
    strcpy(demoPlayer.mUrl, url);
	ret = TPlayerSetDataSource(demoPlayer.mTPlayer,(const char*)demoPlayer.mUrl, NULL);
	if(ret != 0)
    {
        printf("TPlayerSetDataSource() return fail.\n");
		return -1;
    }else{
        printf("setDataSource end\n");
    }
	
	return 0;
}

int tplay_dec_prepare_async(void)
{
	int ret;
	int waitErr;
	
	ret = TPlayerPrepareAsync(demoPlayer.mTPlayer);
	if(ret != 0)
    {
		printf("TPlayerPrepareAsync() return fail.\n");
		return -1;
    }else{
		printf("preparing...\n");
    }
    waitErr = semTimedWait(&demoPlayer.mPreparedSem,300*1000);
	if(waitErr == -1)
	{
        printf("prepare fail,has wait 300s\n");
    }else if(demoPlayer.mError == 1)
	{
        printf("prepare fail\n");
		return -1;
    }
	return 0;
}

int tplay_dec_start(void)
{
	int ret;
	
	ret = TPlayerStart(demoPlayer.mTPlayer);
	if(ret != 0)
     {
         printf("TPlayerStart() return fail.\n");
		 return -1;
     }
	 else{
         printf("started.\n");
     }
	
	return 0;
}

void tplay_dec_stop(void)
{
	TPlayerStop(demoPlayer.mTPlayer);
}

int tplay_dec_reset(void)
{
	int ret;
	ret = TPlayerReset(demoPlayer.mTPlayer);
	if(ret != 0) return -1;
	return 0;
}

//* a callback for tplayer.
static int CallbackForTPlayer(void* pUserData, int msg, int param0, void* param1)
{
    DemoPlayerContext* pDemoPlayer = (DemoPlayerContext*)pUserData;

    CEDARX_UNUSE(param1);
    switch(msg)
    {
        case TPLAYER_NOTIFY_PREPARED:
        {
            printf("TPLAYER_NOTIFY_PREPARED,has prepared.\n");
            sem_post(&pDemoPlayer->mPreparedSem);
            pDemoPlayer->mPreparedFlag = 1;
            break;
        }

        case TPLAYER_NOTIFY_PLAYBACK_COMPLETE:
        {
            printf("TPLAYER_NOTIFY_PLAYBACK_COMPLETE\n");
            pDemoPlayer->mComplete = 1;
            if(pDemoPlayer->mSetLoop == 1){
                pDemoPlayer->mLoopFlag = 1;
            }else{
                pDemoPlayer->mLoopFlag = 0;
            }
            //PowerManagerReleaseWakeLock("tplayerdemo");
            break;
        }

        case TPLAYER_NOTIFY_SEEK_COMPLETE:
        {
            printf("TPLAYER_NOTIFY_SEEK_COMPLETE>>>>info: seek ok.\n");
            break;
        }

        case TPLAYER_NOTIFY_MEDIA_ERROR:
        {
            switch (param0)
            {
                case TPLAYER_MEDIA_ERROR_UNKNOWN:
                {
                    printf("erro type:TPLAYER_MEDIA_ERROR_UNKNOWN\n");
                    break;
                }
                case TPLAYER_MEDIA_ERROR_UNSUPPORTED:
                {
                    printf("erro type:TPLAYER_MEDIA_ERROR_UNSUPPORTED\n");
                    break;
                }
                case TPLAYER_MEDIA_ERROR_IO:
                {
                    printf("erro type:TPLAYER_MEDIA_ERROR_IO\n");
                    break;
                }
            }
            printf("TPLAYER_NOTIFY_MEDIA_ERROR\n");
            pDemoPlayer->mError = 1;
            if(pDemoPlayer->mPreparedFlag == 0){
                printf("recive err when preparing\n");
                sem_post(&pDemoPlayer->mPreparedSem);
            }
            if(pDemoPlayer->mSetLoop == 1){
                pDemoPlayer->mLoopFlag = 1;
            }else{
                pDemoPlayer->mLoopFlag = 0;
            }
            printf("error: open media source fail.\n");
            break;
        }

        case TPLAYER_NOTIFY_NOT_SEEKABLE:
        {
            pDemoPlayer->mSeekable = 0;
            printf("info: media source is unseekable.\n");
            break;
        }

	case TPLAYER_NOTIFY_BUFFER_START:
        {
            printf("have no enough data to play\n");
            break;
        }

	case TPLAYER_NOTIFY_BUFFER_END:
        {
            printf("have enough data to play again\n");
            break;
        }

        case TPLAYER_NOTIFY_VIDEO_FRAME:
        {
            //printf("get the decoded video frame\n");
            break;
        }

        case TPLAYER_NOTIFY_AUDIO_FRAME:
        {
            //printf("get the decoded audio frame\n");
            break;
        }

        case TPLAYER_NOTIFY_SUBTITLE_FRAME:
        {
            //printf("get the decoded subtitle frame\n");
            break;
        }
        case TPLAYER_NOTYFY_DECODED_VIDEO_SIZE:
        {
            int w, h;
            w   = ((int*)param1)[0];   //real decoded video width
            h  = ((int*)param1)[1];   //real decoded video height
            printf("*****tplayerdemo:video decoded width = %d,height = %d",w,h);
            //int divider = 1;
            //if(w>400){
            //    divider = w/400+1;
            //}
            //w = w/divider;
            //h = h/divider;
            printf("real set to display rect:w = %d,h = %d\n",w,h);
            //TPlayerSetSrcRect(pDemoPlayer->mTPlayer, 0, 0, w, h);
        }

        default:
        {
            printf("warning: unknown callback from Tinaplayer.\n");
            break;
        }
    }
    return 0;
}

static int semTimedWait(sem_t* sem, int64_t time_ms)
{
    int err;

    if(time_ms == -1)
    {
        err = sem_wait(sem);
    }
    else
    {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_nsec += time_ms % 1000 * 1000 * 1000;
        ts.tv_sec += time_ms / 1000 + ts.tv_nsec / (1000 * 1000 * 1000);
        ts.tv_nsec = ts.tv_nsec % (1000*1000*1000);

        err = sem_timedwait(sem, &ts);
    }

    return err;
}
