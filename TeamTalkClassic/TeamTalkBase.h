/*
 * Copyright (c) 2005-2017, BearWare.dk
 * 
 * Contact Information:
 *
 * Bjoern D. Rasmussen
 * Kirketoften 5
 * DK-8260 Viby J
 * Denmark
 * Email: contact@bearware.dk
 * Phone: +45 20 20 54 59
 * Web: http://www.bearware.dk
 *
 * This source code is part of the TeamTalk 5 SDK owned by
 * BearWare.dk. All copyright statements may not be removed 
 * or altered from any source distribution. If you use this
 * software in a product, an acknowledgment in the product 
 * documentation is required.
 *
 */

#if !defined(TEAMTALKBASE_H)
#define TEAMTALKBASE_H

#include <map>
#include <vector>
#include <string>

typedef std::map< int, User > users_t;
typedef std::map< int, Channel > channels_t;
typedef std::map<int, StreamTypes> transmitusers_t;

channels_t GetSubChannels(int nChannelID, const channels_t& channels, BOOL bRecursive = FALSE);
channels_t GetParentChannels(int nChannelID, const channels_t& channels);
int GetRootChannelID(const channels_t& channels);
int GetMaxChannelID(const channels_t& channels);
users_t GetChannelUsers(const users_t& users, int nChannelID = -1);
transmitusers_t& GetTransmitUsers(const Channel& chan, transmitusers_t& transmitUsers);
BOOL ToggleTransmitUser(Channel& chan, int nUserID, StreamTypes streams);
BOOL CanToggleTransmitUsers(int nChannelID);

typedef std::vector< TextMessage > messages_t;
typedef std::map< int, messages_t > msgmap_t;
messages_t GetMessages(int nFromUserID, const messages_t& messages);

BOOL GetSoundDevice(int nSoundDeviceID, const CString& szDeviceID, SoundDevice& dev);
int RefVolume(double percent);
int RefVolumeToPercent(int nVolume);
int RefGain(double percent);

CString GetVersion(const User& user);

BOOL IsMyselfTalking();

#define SPEEX_MODEID_NB 0
#define SPEEX_MODEID_WB 1
#define SPEEX_MODEID_UWB 2

void InitDefaultAudioCodec(AudioCodec& audiocodec);

#ifdef ENABLE_ENCRYPTION
#define DEFAULT_TEAMTALK_TCPPORT 10443
#define DEFAULT_TEAMTALK_UDPPORT 10443
#else
#define DEFAULT_TEAMTALK_TCPPORT 10333
#define DEFAULT_TEAMTALK_UDPPORT 10333
#endif

//Client spefic VU max SOUND_VU_MAX (voice act slider)
#define DEFAULT_SOUND_VU_MAX            20

//Automatic gain control settings
#define DEFAULT_AGC_ENABLE              TRUE
#define DEFAULT_AGC_GAINLEVEL           8000
#define DEFAULT_AGC_INC_MAXDB           12
#define DEFAULT_AGC_DEC_MAXDB           -40
#define DEFAULT_AGC_GAINMAXDB           30
#define DEFAULT_DENOISE_ENABLE          TRUE
#define DEFAULT_DENOISE_SUPPRESS        -30
#define DEFAULT_ECHO_ENABLE             FALSE
#define DEFAULT_ECHO_SUPPRESS           -40
#define DEFAULT_ECHO_SUPPRESSACTIVE     -15

#define DEFAULT_SOUND_DUPLEXMODE        FALSE
#define DEFAULT_AUDIOCODEC              OPUS_CODEC
#define DEFAULT_MSEC_PER_PACKET         40

#define SPEEX_MIN_MSEC_PER_PACKET       20
#define SPEEX_MAX_MSEC_PER_PACKET       100

#define OPUS_MIN_MSEC_PER_PACKET        20
#define OPUS_MAX_MSEC_PER_PACKET        60

//Default Speex codec settings
#define DEFAULT_SPEEX_BANDMODE          SPEEX_MODEID_WB
#define DEFAULT_SPEEX_QUALITY           4
#define DEFAULT_SPEEX_DELAY             DEFAULT_MSEC_PER_PACKET
#define DEFAULT_SPEEX_JITTERBUF         FALSE
#define DEFAULT_SPEEX_SIMSTEREO         FALSE

//Default Speex VBR codec settings
#define DEFAULT_SPEEX_VBR_BANDMODE      SPEEX_MODEID_WB
#define DEFAULT_SPEEX_VBR_QUALITY       4
#define DEFAULT_SPEEX_VBR_BITRATE       0
#define DEFAULT_SPEEX_VBR_MAXBITRATE    0
#define DEFAULT_SPEEX_VBR_DTX           TRUE
#define DEFAULT_SPEEX_VBR_DELAY         DEFAULT_MSEC_PER_PACKET
#define DEFAULT_SPEEX_VBR_JITTERBUF     FALSE
#define DEFAULT_SPEEX_VBR_SIMSTEREO     FALSE

//Default OPUS codec settings
#define DEFAULT_OPUS_SAMPLERATE         48000
#define DEFAULT_OPUS_CHANNELS           1
#define DEFAULT_OPUS_APPLICATION        OPUS_APPLICATION_VOIP
#define DEFAULT_OPUS_COMPLEXITY         10
#define DEFAULT_OPUS_FEC                TRUE
#define DEFAULT_OPUS_DTX                FALSE
#define DEFAULT_OPUS_VBR                TRUE
#define DEFAULT_OPUS_VBRCONSTRAINT      FALSE
#define DEFAULT_OPUS_BITRATE            32000
#define DEFAULT_OPUS_DELAY              DEFAULT_MSEC_PER_PACKET

//Video settings
#define DEFAULT_VIDEO_WIDTH             320
#define DEFAULT_VIDEO_HEIGHT            240
#define DEFAULT_VIDEO_FPS               10
#define DEFAULT_VIDEO_FOURCC            FOURCC_RGB32

//WebM settings
#define DEFAULT_VIDEOCODEC              WEBM_VP8_CODEC
#define DEFAULT_WEBMVP8_DEADLINE        WEBM_VPX_DL_REALTIME
#define DEFAULT_WEBM_VP8_BITRATE        256

// Channel dialog
#define DEFAULT_CHANNEL_AUDIOCONFIG     FALSE

//Default user right for default user-type
#define USERRIGHT_DEFAULT   (USERRIGHT_MULTI_LOGIN |                \
                             USERRIGHT_VIEW_ALL_USERS |             \
                             USERRIGHT_CREATE_TEMPORARY_CHANNEL |   \
                             USERRIGHT_UPLOAD_FILES |               \
                             USERRIGHT_DOWNLOAD_FILES |             \
                             USERRIGHT_TRANSMIT_VOICE |             \
                             USERRIGHT_TRANSMIT_VIDEOCAPTURE |      \
                             USERRIGHT_TRANSMIT_DESKTOP |           \
                             USERRIGHT_TRANSMIT_DESKTOPINPUT |      \
                             USERRIGHT_TRANSMIT_MEDIAFILE_AUDIO |   \
                             USERRIGHT_TRANSMIT_MEDIAFILE_VIDEO)

#define DEFAULT_AUDIOCODEC_BPS_LIMIT 0

#define DEFAULT_SENDDESKTOPWINDOW_TIMEOUT 1000

#define DEFAULT_MAX_STRING_LENGTH   50 //real is TT_STRLEN

//user ID to pass to store local user's voice stream
#define AUDIOSTORAGE_LOCAL_USERID 0

enum StatusMode
{
    STATUSMODE_AVAILABLE        = 0x00000000,
    STATUSMODE_AWAY             = 0x00000001, 
    STATUSMODE_QUESTION         = 0x00000002, 
    STATUSMODE_MASK             = 0x000000FF,
    STATUSMODE_FEMALE           = 0x00000100,
    STATUSMODE_VIDEOTX          = 0x00000200,
    STATUSMODE_DESKTOP          = 0x00000400,
    STATUSMODE_STREAM_MEDIAFILE = 0x00000800
};

enum Gender
{
    GENDER_NONE             = 0,
    GENDER_MALE             = 1,
    GENDER_FEMALE           = 2
};

enum AudioStorageMode
{
    AUDIOSTORAGE_NONE               = 0x0,
    AUDIOSTORAGE_SINGLEFILE         = 0x1,
    AUDIOSTORAGE_SEPARATEFILES      = 0x2
};

enum
{
    TTS_USER_LOGGEDIN                               = 0x00000001,
    TTS_USER_LOGGEDOUT                              = 0x00000002,
    TTS_USER_JOINED                                 = 0x00000004,
    TTS_USER_LEFT                                   = 0x00000008,
    TTS_USER_JOINED_SAME                            = 0x10000000,
    TTS_USER_LEFT_SAME                              = 0x20000000,
    TTS_USER_TEXTMSG_PRIVATE                        = 0x00000010,
    TTS_USER_TEXTMSG_CHANNEL                        = 0x00000020,
    TTS_USER_TEXTMSG_BROADCAST                      = 0x00000040,

    TTS_SUBSCRIPTIONS_TEXTMSG_PRIVATE               = 0x00000100,
    TTS_SUBSCRIPTIONS_TEXTMSG_CHANNEL               = 0x00000200,
    TTS_SUBSCRIPTIONS_TEXTMSG_BROADCAST             = 0x00000400,
    TTS_SUBSCRIPTIONS_VOICE                         = 0x00000800,
    TTS_SUBSCRIPTIONS_VIDEO                         = 0x00001000,
    TTS_SUBSCRIPTIONS_DESKTOP                       = 0x00002000,
    TTS_SUBSCRIPTIONS_DESKTOPINPUT                  = 0x00004000,
    TTS_SUBSCRIPTIONS_MEDIAFILE                     = 0x00008000,

    TTS_SUBSCRIPTIONS_INTERCEPT_TEXTMSG_PRIVATE     = 0x00010000,
    TTS_SUBSCRIPTIONS_INTERCEPT_TEXTMSG_CHANNEL     = 0x00020000,
    TTS_SUBSCRIPTIONS_INTERCEPT_VOICE               = 0x00040000,
    TTS_SUBSCRIPTIONS_INTERCEPT_VIDEO               = 0x00080000,
    TTS_SUBSCRIPTIONS_INTERCEPT_DESKTOP             = 0x00100000,
    TTS_SUBSCRIPTIONS_INTERCEPT_DESKTOPINPUT        = 0x00200000,
    TTS_SUBSCRIPTIONS_INTERCEPT_MEDIAFILE           = 0x00400000,

    TTS_CLASSROOM_VOICE_TX                          = 0x01000000,
    TTS_CLASSROOM_VIDEO_TX                          = 0x02000000,
    TTS_CLASSROOM_DESKTOP_TX                        = 0x04000000,
    TTS_CLASSROOM_MEDIAFILE_TX                      = 0x08000000,

    TTS_USER_ALL              = TTS_USER_LOGGEDIN                               |
                                TTS_USER_LOGGEDOUT                              |
                                TTS_USER_JOINED                                 |
                                TTS_USER_LEFT                                   |
                                TTS_USER_JOINED_SAME                            |
                                TTS_USER_LEFT_SAME                              |
                                TTS_USER_TEXTMSG_PRIVATE                        |
                                TTS_USER_TEXTMSG_CHANNEL                        |
                                TTS_USER_TEXTMSG_BROADCAST,


    TTS_SUBSCRIPTIONS_ALL     = TTS_SUBSCRIPTIONS_TEXTMSG_PRIVATE               |
                                TTS_SUBSCRIPTIONS_TEXTMSG_CHANNEL               |
                                TTS_SUBSCRIPTIONS_TEXTMSG_BROADCAST             |
                                TTS_SUBSCRIPTIONS_VOICE                         |
                                TTS_SUBSCRIPTIONS_VIDEO                         |
                                TTS_SUBSCRIPTIONS_DESKTOP                       |
                                TTS_SUBSCRIPTIONS_DESKTOPINPUT                  |
                                TTS_SUBSCRIPTIONS_MEDIAFILE                     |

                                TTS_SUBSCRIPTIONS_INTERCEPT_TEXTMSG_PRIVATE     | 
                                TTS_SUBSCRIPTIONS_INTERCEPT_TEXTMSG_CHANNEL     |
                                TTS_SUBSCRIPTIONS_INTERCEPT_VOICE               |
                                TTS_SUBSCRIPTIONS_INTERCEPT_VIDEO               |
                                TTS_SUBSCRIPTIONS_INTERCEPT_DESKTOP             |
                                TTS_SUBSCRIPTIONS_INTERCEPT_DESKTOPINPUT        |
                                TTS_SUBSCRIPTIONS_INTERCEPT_MEDIAFILE,

    TTS_CLASSROOM_ALL         = TTS_CLASSROOM_VOICE_TX                          |
                                TTS_CLASSROOM_VIDEO_TX                          |
                                TTS_CLASSROOM_DESKTOP_TX                        |
                                TTS_CLASSROOM_MEDIAFILE_TX,

    TTS_ALL                   = TTS_USER_ALL | TTS_SUBSCRIPTIONS_ALL | TTS_CLASSROOM_ALL
};

typedef __int64 TTSEvents;

//internal TeamTalk 5 commands. Use with TT_DoTextMessage() and 
//message type MSGTYPE_CUSTOM
#define TT_INTCMD_TYPING_TEXT         _T("typing")
#define TT_INTCMD_DESKTOP_ACCESS      _T("desktopaccess")

CString MakeCustomCommand(LPCTSTR szCmd, LPCTSTR szValue);
void GetCustomCommand(const CString& szMessage, CStringList& result);

BOOL IsValid(const VideoFormat& capfmt);

#endif
