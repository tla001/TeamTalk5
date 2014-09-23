/*
 * Copyright (c) 2005-2014, BearWare.dk
 * 
 * Contact Information:
 *
 * Bjoern D. Rasmussen
 * Skanderborgvej 40 4-2
 * DK-8000 Aarhus C
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

#include <string.h>
#include <TeamTalk.h>
#include "ttconvert-jni.h"
#include "teamtalk-jni.h"
#include <assert.h>
#include <iostream>
#include <vector>
using namespace std;

#include <jni.h>

extern "C" {

    JNIEXPORT jstring JNICALL Java_dk_bearware_TeamTalkBase_getVersion(JNIEnv* env,
                                                                       jclass)
    {
        const char* ttv = TT_GetVersion();
        return env->NewStringUTF(ttv);
    }

    JNIEXPORT jlong JNICALL Java_dk_bearware_TeamTalkBase_initTeamTalkPoll(JNIEnv* env,
                                                                           jobject thiz)
    {
        return (jlong)TT_InitTeamTalkPoll();
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_closeTeamTalk(JNIEnv* env,
                                                                           jobject thiz,
                                                                           jlong lpTTInstance)
    {
        return TT_CloseTeamTalk(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getMessage(JNIEnv* env,
                                                                        jobject thiz,
                                                                        jlong lpTTInstance,
                                                                        jobject pMsg,
                                                                        jint pnWaitMs)
    {
        THROW_NULLEX(env, pMsg, false);

        TTMessage msg;
        BOOL b = TT_GetMessage(reinterpret_cast<TTInstance*>(lpTTInstance),
                               &msg, &pnWaitMs);
        if(b)
            setTTMessage(env, msg, pMsg);
        return b;
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_getFlags(JNIEnv* env,
                                                                  jobject thiz,
                                                                  jlong lpTTInstance)
    {
        return TT_GetFlags(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_setLicenseInformation(JNIEnv* env,
                                                                                   jclass,
                                                                                   jstring szRegName,
                                                                                   jstring szRegKey)
    {
        THROW_NULLEX(env, szRegName, false);
        THROW_NULLEX(env, szRegKey, false);
        return TT_SetLicenseInformation(ttstr(env, szRegName), ttstr(env, szRegKey));
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getDefaultSoundDevices(JNIEnv* env,
                                                                                    jclass,
                                                                                    jobject lpnInputDeviceID,
                                                                                    jobject lpnOutputDeviceID)
    {
        THROW_NULLEX(env, lpnInputDeviceID, false);
        THROW_NULLEX(env, lpnOutputDeviceID, false);

        int inputid, outputid;
        if(TT_GetDefaultSoundDevices(&inputid, &outputid))
        {
            setIntPtr(env, lpnInputDeviceID, inputid);
            setIntPtr(env, lpnOutputDeviceID, outputid);
            return true;
        }
        return false;
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getDefaultSoundDevicesEx(JNIEnv* env,
                                                                                      jclass,
                                                                                      jint nSndSystem,
                                                                                      jobject lpnInputDeviceID,
                                                                                      jobject lpnOutputDeviceID)
    {
        THROW_NULLEX(env, lpnInputDeviceID, false);
        THROW_NULLEX(env, lpnOutputDeviceID, false);

        int inputid, outputid;
        if(TT_GetDefaultSoundDevicesEx((SoundSystem)nSndSystem, &inputid, &outputid))
        {
            setIntPtr(env, lpnInputDeviceID, inputid);
            setIntPtr(env, lpnOutputDeviceID, outputid);
            return true;
        }
        return false;
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getSoundDevices(JNIEnv* env,
                                                                             jobject thiz,
                                                                             jobjectArray lpSoundDevices,
                                                                             jobject lpnHowMany)
    {
        THROW_NULLEX(env, lpnHowMany, false);

        int howmany = 0;
        if(lpSoundDevices == NULL)
        {
            if(!TT_GetSoundDevices(NULL, &howmany))
                return false;
            setIntPtr(env, lpnHowMany, howmany);
            return true;
        }

        jsize arr_size = env->GetArrayLength(lpSoundDevices);
        std::vector<SoundDevice> devs((size_t)arr_size);
        if(arr_size && !TT_GetSoundDevices(&devs[0], &arr_size))
            return false;

        for(jsize i=0;i<arr_size;i++)
            env->SetObjectArrayElement(lpSoundDevices, i, newSoundDevice(env, devs[i]));

        setIntPtr(env, lpnHowMany, arr_size);

        return true;
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_restartSoundSystem(JNIEnv* env,
                                                                                jclass)
    {
        return TT_RestartSoundSystem();
    }

    JNIEXPORT jlong JNICALL Java_dk_bearware_TeamTalkBase_startSoundLoopbackTest(JNIEnv* env,
                                                                                 jclass,
                                                                                 jint nInputDeviceID, 
                                                                                 jint nOutputDeviceID,
                                                                                 jint nSampleRate,
                                                                                 jint nChannels,
                                                                                 jboolean bDuplexMode,
                                                                                 jobject lpAudioConfig)
    {
    
        TTSoundLoop* inst;
        if(lpAudioConfig)
        {
            AudioConfig audcfg;
            ZERO_STRUCT(audcfg);
            setAudioConfig(env, audcfg, lpAudioConfig, J2N);
        
            inst = TT_StartSoundLoopbackTest(nInputDeviceID, nOutputDeviceID, 
                                             nSampleRate, nChannels, bDuplexMode, &audcfg);
        }
        else
        {
            inst = TT_StartSoundLoopbackTest(nInputDeviceID, nOutputDeviceID, 
                                             nSampleRate, nChannels, bDuplexMode, NULL);
        }
        return reinterpret_cast<jlong>(inst);
    }
    
    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_closeSoundLoopbackTest(JNIEnv* env,
                                                                                    jclass,
                                                                                    jlong lpTTSoundLoop)
    {
        return TT_CloseSoundLoopbackTest(reinterpret_cast<TTSoundLoop*>(lpTTSoundLoop));
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_initSoundInputDevice(JNIEnv* env,
                                                                                  jobject thiz,
                                                                                  jlong lpTTInstance,
                                                                                  jint nInputDeviceID)
    {
        return TT_InitSoundInputDevice(reinterpret_cast<TTInstance*>(lpTTInstance), nInputDeviceID);
                                   
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_initSoundOutputDevice(JNIEnv* env,
                                                                                   jobject thiz,
                                                                                   jlong lpTTInstance,
                                                                                   jint nOutputDeviceID)
    {
        return TT_InitSoundOutputDevice(reinterpret_cast<TTInstance*>(lpTTInstance), nOutputDeviceID);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_closeSoundInputDevice(JNIEnv* env,
                                                                                   jobject thiz,
                                                                                   jlong lpTTInstance)
    {
        return TT_CloseSoundInputDevice(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_closeSoundOutputDevice(JNIEnv* env,
                                                                                    jobject thiz,
                                                                                    jlong lpTTInstance)
    {
        return TT_CloseSoundOutputDevice(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_getSoundInputLevel(JNIEnv* env,
                                                                            jobject thiz,
                                                                            jlong lpTTInstance)
    {
        return TT_GetSoundInputLevel(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_setSoundInputGainLevel(JNIEnv* env,
                                                                                    jobject thiz,
                                                                                    jlong lpTTInstance,
                                                                                    jint nLevel)
    {
        return TT_SetSoundInputGainLevel(reinterpret_cast<TTInstance*>(lpTTInstance),
                                         nLevel);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_getSoundInputGainLevel(JNIEnv* env,
                                                                                jobject thiz,
                                                                                jlong lpTTInstance)
    {
        return TT_GetSoundInputGainLevel(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_setAudioConfig(JNIEnv* env,
                                                                            jobject thiz,
                                                                            jlong lpTTInstance,
                                                                            jobject lpAudioConfig)
    {
        THROW_NULLEX(env, lpAudioConfig, false);

        AudioConfig audcfg;
        ZERO_STRUCT(audcfg);
        setAudioConfig(env, audcfg, lpAudioConfig, J2N);
        return TT_SetAudioConfig(reinterpret_cast<TTInstance*>(lpTTInstance), &audcfg);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getAudioConfig(JNIEnv* env,
                                                                            jobject thiz,
                                                                            jlong lpTTInstance,
                                                                            jobject lpAudioConfig)
    {
        THROW_NULLEX(env, lpAudioConfig, false);

        AudioConfig audcfg;
        ZERO_STRUCT(audcfg);
        if(TT_GetAudioConfig(reinterpret_cast<TTInstance*>(lpTTInstance), &audcfg))
        {
            setAudioConfig(env, audcfg, lpAudioConfig, N2J);
            return true;
        }
        return false;
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_setSoundOutputVolume(JNIEnv* env,
                                                                                  jobject thiz,
                                                                                  jlong lpTTInstance,
                                                                                  jint nVolume)
    {
        return TT_SetSoundOutputVolume(reinterpret_cast<TTInstance*>(lpTTInstance),
                                       nVolume);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_getSoundOutputVolume(JNIEnv* env,
                                                                              jobject thiz,
                                                                              jlong lpTTInstance)
    {
        return TT_GetSoundOutputVolume(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_setSoundOutputMute(JNIEnv* env,
                                                                                jobject thiz,
                                                                                jlong lpTTInstance,
                                                                                jboolean bMuteAll)
    {
        return TT_SetSoundOutputMute(reinterpret_cast<TTInstance*>(lpTTInstance),
                                     bMuteAll);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_enableVoiceTransmission(JNIEnv* env,
                                                                                     jobject thiz,
                                                                                     jlong lpTTInstance,
                                                                                     jboolean bEnable)
    {
        return TT_EnableVoiceTransmission(reinterpret_cast<TTInstance*>(lpTTInstance),
                                          bEnable);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_enable3DSoundPositioning(JNIEnv* env,
                                                                                      jobject thiz,
                                                                                      jlong lpTTInstance,
                                                                                      jboolean bEnable)
    {
        return TT_Enable3DSoundPositioning(reinterpret_cast<TTInstance*>(lpTTInstance),
                                           bEnable);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_autoPositionUsers(JNIEnv* env,
                                                                               jobject thiz,
                                                                               jlong lpTTInstance)
    {
        return TT_AutoPositionUsers(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_enableAudioBlockEvent(JNIEnv* env,
                                                                                   jobject thiz,
                                                                                   jlong lpTTInstance,
                                                                                   jboolean bEnable)
    {
        return TT_EnableAudioBlockEvent(reinterpret_cast<TTInstance*>(lpTTInstance),
                                        bEnable);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_enableVoiceActivation(JNIEnv* env,
                                                                                   jobject thiz,
                                                                                   jlong lpTTInstance,
                                                                                   jboolean bEnable)
    {
        return TT_EnableVoiceActivation(reinterpret_cast<TTInstance*>(lpTTInstance),
                                        bEnable);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_setVoiceActivationLevel(JNIEnv* env,
                                                                                     jobject thiz,
                                                                                     jlong lpTTInstance,
                                                                                     jint nLevel)
    {
        return TT_SetVoiceActivationLevel(reinterpret_cast<TTInstance*>(lpTTInstance),
                                          nLevel);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_getVoiceActivationLevel(JNIEnv* env,
                                                                                 jobject thiz,
                                                                                 jlong lpTTInstance)
    {
        return TT_GetVoiceActivationLevel(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_setVoiceActivationStopDelay(JNIEnv* env,
                                                                                         jobject thiz,
                                                                                         jlong lpTTInstance,
                                                                                         jint nDelayMSec)
    {
        return TT_SetVoiceActivationStopDelay(reinterpret_cast<TTInstance*>(lpTTInstance),
                                              nDelayMSec);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_getVoiceActivationStopDelay(JNIEnv* env,
                                                                                     jobject thiz,
                                                                                     jlong lpTTInstance)
    {
        return TT_GetVoiceActivationStopDelay(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_startRecordingMuxedAudioFile(JNIEnv* env,
                                                                                          jobject thiz,
                                                                                          jlong lpTTInstance,
                                                                                          jobject lpAudioCodec,
                                                                                          jstring szAudioFileName,
                                                                                          jint uAFF)
    {
        THROW_NULLEX(env, lpAudioCodec, false);
        THROW_NULLEX(env, szAudioFileName, false);

        AudioCodec audcodec;
        ZERO_STRUCT(audcodec);
        setAudioCodec(env, audcodec, lpAudioCodec, J2N);
        
        return TT_StartRecordingMuxedAudioFile(reinterpret_cast<TTInstance*>(lpTTInstance),
                                               &audcodec, ttstr(env, szAudioFileName), (AudioFileFormat)uAFF);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_stopRecordingMuxedAudioFile(JNIEnv* env,
                                                                                         jobject thiz,
                                                                                         jlong lpTTInstance)
    {
        return TT_StopRecordingMuxedAudioFile(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_startVideoCaptureTransmission(JNIEnv* env,
                                                                                           jobject thiz,
                                                                                           jlong lpTTInstance,
                                                                                           jobject lpVideoCodec)
    {
        THROW_NULLEX(env, lpVideoCodec, false);

        VideoCodec vidcodec;
        ZERO_STRUCT(vidcodec);
        setVideoCodec(env, vidcodec, lpVideoCodec, J2N);
        return TT_StartVideoCaptureTransmission(reinterpret_cast<TTInstance*>(lpTTInstance),
                                                &vidcodec);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_stopVideoCaptureTransmission(JNIEnv* env,
                                                                                          jobject thiz,
                                                                                          jlong lpTTInstance)
    {
        return TT_StopVideoCaptureTransmission(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getVideoCaptureDevices(JNIEnv* env,
                                                                                    jobject thiz,
                                                                                    jobjectArray lpVideoDevices,
                                                                                    jobject lpnHowMany)
    {
        THROW_NULLEX(env, lpnHowMany, false);

        int howmany = 0;
        if(lpVideoDevices == NULL)
        {
            if(!TT_GetVideoCaptureDevices(NULL, &howmany))
                return false;
            setIntPtr(env, lpnHowMany, howmany);
            return true;
        }

        jsize arr_size = env->GetArrayLength(lpVideoDevices);
        std::vector<VideoCaptureDevice> devs((size_t)arr_size);
        if(arr_size && !TT_GetVideoCaptureDevices(&devs[0], &arr_size))
            return false;

        for(jsize i=0;i<arr_size;i++)
            env->SetObjectArrayElement(lpVideoDevices, i, newVideoDevice(env, devs[i]));

        setIntPtr(env, lpnHowMany, arr_size);

        return true;
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_initVideoCaptureDevice(JNIEnv* env,
                                                                                   jobject thiz,
                                                                                   jlong lpTTInstance,
                                                                                   jstring szDeviceID,
                                                                                   jobject lpVideoFormat)
    {
        VideoFormat fmt;
        ZERO_STRUCT(fmt);
        setVideoFormat(env, fmt, lpVideoFormat, J2N);
        
        return TT_InitVideoCaptureDevice(reinterpret_cast<TTInstance*>(lpTTInstance),
                                         ttstr(env, szDeviceID), &fmt);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_closeVideoCaptureDevice(JNIEnv* env,
                                                                                    jobject thiz,
                                                                                    jlong lpTTInstance)
    {
        return TT_CloseVideoCaptureDevice(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jobject JNICALL Java_dk_bearware_TeamTalkBase_acquireUserVideoCaptureFrame(JNIEnv* env,
                                                                                         jobject thiz,
                                                                                         jlong lpTTInstance,
                                                                                         jint nUserID)
    {
        VideoFrame* vidframe = TT_AcquireUserVideoCaptureFrame(reinterpret_cast<TTInstance*>(lpTTInstance),
                                                               nUserID);
        if(!vidframe)
            return NULL;

        jclass cls = env->FindClass("dk/bearware/VideoFrame");
        jobject vidframe_obj = newObject(env, cls);
        setVideoFrame(env, *vidframe, vidframe_obj);

        TT_ReleaseUserVideoCaptureFrame(reinterpret_cast<TTInstance*>(lpTTInstance), vidframe);
        return vidframe_obj;
    }

// ReleaseUserVideoCaptureFrame()

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_startStreamingMediaFileToChannel(JNIEnv* env,
                                                                                              jobject thiz,
                                                                                              jlong lpTTInstance,
                                                                                              jstring szMediaFilePath,
                                                                                              jobject lpVideoCodec)
    {
        THROW_NULLEX(env, szMediaFilePath, false);
        THROW_NULLEX(env, lpVideoCodec, false);

        VideoCodec vidcodec;
        ZERO_STRUCT(vidcodec);
        vidcodec.nCodec = NO_CODEC;
        if(lpVideoCodec)
            setVideoCodec(env, vidcodec, lpVideoCodec, J2N);
        return TT_StartStreamingMediaFileToChannel(reinterpret_cast<TTInstance*>(lpTTInstance),
                                                   ttstr(env, szMediaFilePath), &vidcodec);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_stopStreamingMediaFileToChannel(JNIEnv* env,
                                                                                             jobject thiz,
                                                                                             jlong lpTTInstance)
    {
        return TT_StopStreamingMediaFileToChannel(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getMediaFileInfo(JNIEnv* env,
                                                                              jobject thiz,
                                                                              jstring szMediaFilePath,
                                                                              jobject lpMediaFileInfo)
    {
        THROW_NULLEX(env, szMediaFilePath, false);
        THROW_NULLEX(env, lpMediaFileInfo, false);

        MediaFileInfo mfi;
        if(TT_GetMediaFileInfo(ttstr(env, szMediaFilePath), &mfi))
        {
            setMediaFileInfo(env, mfi, lpMediaFileInfo);
            return true;
        }
        return false;
    }

    JNIEXPORT jobject JNICALL Java_dk_bearware_TeamTalkBase_acquireUserMediaVideoFrame(JNIEnv* env,
                                                                                       jobject thiz,
                                                                                       jlong lpTTInstance,
                                                                                       jint nUserID)
    {
        VideoFrame* vidframe = TT_AcquireUserMediaVideoFrame(reinterpret_cast<TTInstance*>(lpTTInstance),
                                                             nUserID);

        if(!vidframe)
            return NULL;

        jclass cls = env->FindClass("dk/bearware/VideoFrame");
        jobject vidframe_obj = newObject(env, cls);
        setVideoFrame(env, *vidframe, vidframe_obj);

        TT_ReleaseUserMediaVideoFrame(reinterpret_cast<TTInstance*>(lpTTInstance), vidframe);
        return vidframe_obj;

    }

/*
    JNIEXPORT jobject JNICALL Java_dk_bearware_TeamTalkBase_releaseUserMediaVideoFrame(JNIEnv* env,
                                                                                       jobject thiz,
                                                                                       jlong lpTTInstance,
                                                                                       jobject lpVideoFrame)
    {
        return NULL;
    }
*/
    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_sendDesktopWindow(JNIEnv* env,
                                                                           jobject thiz,
                                                                           jlong lpTTInstance,
                                                                           jobject lpDesktopWindow,
                                                                           int nConvertBmpFormat)
    {
        DesktopWindow wnd;
        setDesktopWindow(env, wnd, lpDesktopWindow, J2N);
        
        jclass cls = env->GetObjectClass(lpDesktopWindow);
        jfieldID fid_frmbuf = env->GetFieldID(cls, "frameBuffer", "[B");
        assert(fid_frmbuf);

        jbyteArray buf = (jbyteArray)env->GetObjectField(lpDesktopWindow, fid_frmbuf);
        jbyte* bufptr = env->GetByteArrayElements(buf, 0);
        if(!bufptr)
            return -1;

        wnd.frameBuffer = bufptr;
        wnd.nFrameBufferSize = env->GetArrayLength(buf);
        
        jint ret = TT_SendDesktopWindow(reinterpret_cast<TTInstance*>(lpTTInstance),
                                        &wnd, (BitmapFormat)nConvertBmpFormat);
        
       env->ReleaseByteArrayElements(buf, bufptr, 0);
       return ret;
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_closeDesktopWindow(JNIEnv* env,
                                                                                jobject thiz,
                                                                                jlong lpTTInstance)
    {
        return TT_CloseDesktopWindow(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

//TODO: Palette_GetColorTable

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_sendDesktopCursorPosition(JNIEnv* env,
                                                                                       jobject thiz,
                                                                                       jlong lpTTInstance,
                                                                                       jint nPosX, 
                                                                                       jint nPosY)
    {
        return TT_SendDesktopCursorPosition(reinterpret_cast<TTInstance*>(lpTTInstance),
                                            nPosX, nPosY);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_sendDesktopInput(JNIEnv* env,
                                                                              jobject thiz,
                                                                              jlong lpTTInstance,
                                                                              jint nUserID,
                                                                              jobjectArray lpDesktopInputs)
    {
        THROW_NULLEX(env, lpDesktopInputs, false);

        size_t len = env->GetArrayLength(lpDesktopInputs);
        std::vector<DesktopInput> inputs(len);
        for(size_t i=0;i<len;i++)
            setDesktopInput(env, inputs[i], env->GetObjectArrayElement(lpDesktopInputs, i), J2N);
        return TT_SendDesktopInput(reinterpret_cast<TTInstance*>(lpTTInstance), nUserID,
                                   &inputs[0], len);
    }

    JNIEXPORT jobject JNICALL Java_dk_bearware_TeamTalkBase_acquireUserDesktopWindow(JNIEnv* env,
                                                                                     jobject thiz,
                                                                                     jlong lpTTInstance,
                                                                                     jint nUserID) {
        
        DesktopWindow* deskwnd = TT_AcquireUserDesktopWindow(reinterpret_cast<TTInstance*>(lpTTInstance), 
                                                             nUserID);
        if(!deskwnd)
            return NULL;

        jclass cls = env->FindClass("dk/bearware/DesktopWindow");
        jobject deskwnd_obj = newObject(env, cls);
        setDesktopWindow(env, *deskwnd, deskwnd_obj, N2J);

        TT_ReleaseUserDesktopWindow(reinterpret_cast<TTInstance*>(lpTTInstance), deskwnd);
        return deskwnd_obj;
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_connect(JNIEnv* env,
                                                                     jobject thiz,
                                                                     jlong lpTTInstance,
                                                                     jstring szHostAddress,
                                                                     jint nTcpPort, 
                                                                     jint nUdpPort, 
                                                                     jint nLocalTcpPort, 
                                                                     jint nLocalUdpPort,
                                                                     jboolean bEncrypted)
    {
        THROW_NULLEX(env, szHostAddress, false);

        return TT_Connect(reinterpret_cast<TTInstance*>(lpTTInstance),
                          ttstr(env,szHostAddress), nTcpPort, nUdpPort, 
                          nLocalTcpPort, nLocalUdpPort, bEncrypted);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_connectEx(JNIEnv* env,
                                                                       jobject thiz,
                                                                       jlong lpTTInstance,
                                                                       jstring szHostAddress,
                                                                       jint nTcpPort,
                                                                       jint nUdpPort,
                                                                       jstring szBindIPAddr,
                                                                       jint nLocalTcpPort,
                                                                       jint nLocalUdpPort,
                                                                       jboolean bEncrypted)
    {
        THROW_NULLEX(env, szHostAddress, false);
        THROW_NULLEX(env, szBindIPAddr, false);

        return TT_ConnectEx(reinterpret_cast<TTInstance*>(lpTTInstance),
                            ttstr(env, szHostAddress), nTcpPort, nUdpPort,
                            ttstr(env, szBindIPAddr), nLocalTcpPort, nLocalUdpPort,
                            bEncrypted);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_disconnect(JNIEnv* env,
                                                                        jobject thiz,
                                                                        jlong lpTTInstance)

    {
        return TT_Disconnect(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_queryMaxPayload(JNIEnv* env,
                                                                             jobject thiz,
                                                                             jlong lpTTInstance,
                                                                             jint nUserID)
    {
        return TT_QueryMaxPayload(reinterpret_cast<TTInstance*>(lpTTInstance), nUserID);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getClientStatistics(JNIEnv* env,
                                                                                 jobject thiz,
                                                                                 jlong lpTTInstance,
                                                                                 jobject lpClientStatistics)
    {
        THROW_NULLEX(env, lpClientStatistics, false);

        ClientStatistics stats;
        if(TT_GetClientStatistics(reinterpret_cast<TTInstance*>(lpTTInstance),
                                  &stats))
        {
            setClientStatistics(env, stats, lpClientStatistics);
            return true;
        }
        return false;
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doPing(JNIEnv* env,
                                                                jobject thiz,
                                                                jlong lpTTInstance)
    {
        return TT_DoPing(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doLogin(JNIEnv* env,
                                                                 jobject thiz,
                                                                 jlong lpTTInstance,
                                                                 jstring szNickname, 
                                                                 jstring szUsername,
                                                                 jstring szPassword)
    {
        THROW_NULLEX(env, szNickname, -1);
        THROW_NULLEX(env, szUsername, -1);
        THROW_NULLEX(env, szPassword, -1);

        return TT_DoLogin(reinterpret_cast<TTInstance*>(lpTTInstance), 
                          ttstr(env, szNickname), ttstr(env, szUsername), 
                          ttstr(env, szPassword));
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doLogout(JNIEnv* env,
                                                                  jobject thiz,
                                                                  jlong lpTTInstance)
    {
        return TT_DoLogout(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doJoinChannel(JNIEnv* env,
                                                                       jobject thiz,
                                                                       jlong lpTTInstance,
                                                                       jobject lpChannel)
    {
        THROW_NULLEX(env, lpChannel, -1);

        Channel chan = {0};
        setChannel(env, chan, lpChannel, J2N);
        return TT_DoJoinChannel(reinterpret_cast<TTInstance*>(lpTTInstance), &chan);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doJoinChannelByID(JNIEnv* env,
                                                                           jobject thiz,
                                                                           jlong lpTTInstance,
                                                                           jint nChannelID, 
                                                                           jstring szPassword)
    {
        THROW_NULLEX(env, szPassword, -1);

        return TT_DoJoinChannelByID(reinterpret_cast<TTInstance*>(lpTTInstance),
                                    nChannelID, ttstr(env, szPassword));
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doLeaveChannel(JNIEnv* env,
                                                                        jobject thiz,
                                                                        jlong lpTTInstance)
    {
        return TT_DoLeaveChannel(reinterpret_cast<TTInstance*>(lpTTInstance));
                             
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doChangeNickname(JNIEnv* env,
                                                                          jobject thiz,
                                                                          jlong lpTTInstance,
                                                                          jstring szNewNick)
    {
        THROW_NULLEX(env, szNewNick, -1);

        return TT_DoChangeNickname(reinterpret_cast<TTInstance*>(lpTTInstance),
                                   ttstr(env, szNewNick));
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doChangeStatus(JNIEnv* env,
                                                                        jobject thiz,
                                                                        jlong lpTTInstance,
                                                                        jint nStatusMode,
                                                                        jstring szStatusMessage)
    {
        THROW_NULLEX(env, szStatusMessage, -1);

        return TT_DoChangeStatus(reinterpret_cast<TTInstance*>(lpTTInstance),
                                 nStatusMode, ttstr(env, szStatusMessage));
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doTextMessage(JNIEnv* env,
                                                                       jobject thiz,
                                                                       jlong lpTTInstance,
                                                                       jobject lpTextMessage)
    {
        THROW_NULLEX(env, lpTextMessage, -1);

        TextMessage msg;
        setTextMessage(env, msg, lpTextMessage, J2N);
        return TT_DoTextMessage(reinterpret_cast<TTInstance*>(lpTTInstance), &msg);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doChannelOp(JNIEnv* env,
                                                                     jobject thiz,
                                                                     jlong lpTTInstance,
                                                                     jint nUserID,
                                                                     jint nChannelID,
                                                                     jboolean bMakeOperator)
    {
        return TT_DoChannelOp(reinterpret_cast<TTInstance*>(lpTTInstance), nUserID, nChannelID, bMakeOperator);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doChannelOpEx(JNIEnv* env,
                                                                       jobject thiz,
                                                                       jlong lpTTInstance,
                                                                       jint nUserID,
                                                                       jint nChannelID,
                                                                       jstring szOpPassword,
                                                                       jboolean bMakeOperator)
    {
        THROW_NULLEX(env, szOpPassword, -1);

        return TT_DoChannelOpEx(reinterpret_cast<TTInstance*>(lpTTInstance), nUserID, 
                                nChannelID, ttstr(env, szOpPassword), bMakeOperator);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doKickUser(JNIEnv* env,
                                                                    jobject thiz,
                                                                    jlong lpTTInstance,
                                                                    jint nUserID,
                                                                    jint nChannelID)
    {
        return TT_DoKickUser(reinterpret_cast<TTInstance*>(lpTTInstance), nUserID, 
                             nChannelID);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doSendFile(JNIEnv* env,
                                                                    jobject thiz,
                                                                    jlong lpTTInstance,
                                                                    jint nChannelID,
                                                                    jstring szLocalFilePath)
    {
        THROW_NULLEX(env, szLocalFilePath, -1);

        return TT_DoSendFile(reinterpret_cast<TTInstance*>(lpTTInstance), nChannelID,
                             ttstr(env, szLocalFilePath));
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doRecvFile(JNIEnv* env,
                                                                    jobject thiz,
                                                                    jlong lpTTInstance,
                                                                    jint nChannelID,
                                                                    jint nFileID,
                                                                    jstring szLocalFilePath)
    {
        THROW_NULLEX(env, szLocalFilePath, -1);

        return TT_DoRecvFile(reinterpret_cast<TTInstance*>(lpTTInstance), nChannelID, nFileID,
                             ttstr(env, szLocalFilePath));
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doDeleteFile(JNIEnv* env,
                                                                      jobject thiz,
                                                                      jlong lpTTInstance,
                                                                      jint nChannelID,
                                                                      jint nFileID)
    {
        return TT_DoDeleteFile(reinterpret_cast<TTInstance*>(lpTTInstance), nChannelID, nFileID);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doSubscribe(JNIEnv* env,
                                                                     jobject thiz,
                                                                     jlong lpTTInstance,
                                                                     jint nUserID,
                                                                     jint uSubscriptions)
    {
        return TT_DoSubscribe(reinterpret_cast<TTInstance*>(lpTTInstance), nUserID, uSubscriptions);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doUnsubscribe(JNIEnv* env,
                                                                       jobject thiz,
                                                                       jlong lpTTInstance,
                                                                       jint nUserID,
                                                                       jint uSubscriptions)
    {
        return TT_DoUnsubscribe(reinterpret_cast<TTInstance*>(lpTTInstance), nUserID, uSubscriptions);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doMakeChannel(JNIEnv* env,
                                                                       jobject thiz,
                                                                       jlong lpTTInstance,
                                                                       jobject lpChannel)
    {
        THROW_NULLEX(env, lpChannel, -1);

        Channel chan;
        setChannel(env, chan, lpChannel, J2N);
        return TT_DoMakeChannel(reinterpret_cast<TTInstance*>(lpTTInstance), &chan);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doUpdateChannel(JNIEnv* env,
                                                                         jobject thiz,
                                                                         jlong lpTTInstance,
                                                                         jobject lpChannel)
    {
        THROW_NULLEX(env, lpChannel, -1);

        Channel chan;
        setChannel(env, chan, lpChannel, J2N);
        return TT_DoUpdateChannel(reinterpret_cast<TTInstance*>(lpTTInstance), &chan);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doRemoveChannel(JNIEnv* env,
                                                                         jobject thiz,
                                                                         jlong lpTTInstance,
                                                                         jint nChannelID)
    {
        return TT_DoRemoveChannel(reinterpret_cast<TTInstance*>(lpTTInstance), nChannelID);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doMoveUser(JNIEnv* env,
                                                                    jobject thiz,
                                                                    jlong lpTTInstance,
                                                                    jint nUserID,
                                                                    jint nChannelID)
    {
        return TT_DoMoveUser(reinterpret_cast<TTInstance*>(lpTTInstance), nUserID, nChannelID);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doUpdateServer(JNIEnv* env,
                                                                        jobject thiz,
                                                                        jlong lpTTInstance,
                                                                        jobject lpServerProperties)
    {
        THROW_NULLEX(env, lpServerProperties, -1);

        ServerProperties srvprop;
        setServerProperties(env, srvprop, lpServerProperties, J2N);
        return TT_DoUpdateServer(reinterpret_cast<TTInstance*>(lpTTInstance), &srvprop);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doListUserAccounts(JNIEnv* env,
                                                                            jobject thiz,
                                                                            jlong lpTTInstance,
                                                                            jint nIndex, 
                                                                            jint nCount)
    {
        return TT_DoListUserAccounts(reinterpret_cast<TTInstance*>(lpTTInstance), nIndex, nCount);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doNewUserAccount(JNIEnv* env,
                                                                          jobject thiz,
                                                                          jlong lpTTInstance,
                                                                          jobject lpUserAccount)
    {
        THROW_NULLEX(env, lpUserAccount, -1);

        UserAccount account;
        setUserAccount(env, account, lpUserAccount, J2N);
        return TT_DoNewUserAccount(reinterpret_cast<TTInstance*>(lpTTInstance), &account);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doDeleteUserAccount(JNIEnv* env,
                                                                             jobject thiz,
                                                                             jlong lpTTInstance,
                                                                             jstring szUsername)
    {
        THROW_NULLEX(env, szUsername, -1);

        return TT_DoDeleteUserAccount(reinterpret_cast<TTInstance*>(lpTTInstance), ttstr(env, szUsername));
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doBanUser(JNIEnv* env,
                                                                   jobject thiz,
                                                                   jlong lpTTInstance,
                                                                   jint nUserID)
    {
        return TT_DoBanUser(reinterpret_cast<TTInstance*>(lpTTInstance), nUserID);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doBanIPAddress(JNIEnv* env,
                                                                        jobject thiz,
                                                                        jlong lpTTInstance,
                                                                        jstring szIPAddress)
    {
        THROW_NULLEX(env, szIPAddress, -1);

        return TT_DoBanIPAddress(reinterpret_cast<TTInstance*>(lpTTInstance), 
                                 ttstr(env, szIPAddress));
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doUnBanUser(JNIEnv* env,
                                                                     jobject thiz,
                                                                     jlong lpTTInstance,
                                                                     jstring szIPAddress)
    {
        THROW_NULLEX(env, szIPAddress, -1);

        return TT_DoUnBanUser(reinterpret_cast<TTInstance*>(lpTTInstance), 
                              ttstr(env, szIPAddress));
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doListBans(JNIEnv* env,
                                                                    jobject thiz,
                                                                    jlong lpTTInstance,
                                                                    jint nIndex,
                                                                    jint nCount)
    {
        return TT_DoListBans(reinterpret_cast<TTInstance*>(lpTTInstance), 
                             nIndex, nCount);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doSaveConfig(JNIEnv* env,
                                                                      jobject thiz,
                                                                      jlong lpTTInstance)
    {
        return TT_DoSaveConfig(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doQueryServerStats(JNIEnv* env,
                                                                            jobject thiz,
                                                                            jlong lpTTInstance)
    {
        return TT_DoQueryServerStats(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_doQuit(JNIEnv* env,
                                                                jobject thiz,
                                                                jlong lpTTInstance)
    {
        return TT_DoQuit(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getServerProperties(JNIEnv* env,
                                                                                 jobject thiz,
                                                                                 jlong lpTTInstance,
                                                                                 jobject lpServerProperties)
    {
        THROW_NULLEX(env, lpServerProperties, false);

        ServerProperties srvprop;
        if(TT_GetServerProperties(reinterpret_cast<TTInstance*>(lpTTInstance),
                                  &srvprop))
        {
            setServerProperties(env, srvprop, lpServerProperties, N2J);
            return true;
        }
        return false;
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getServerUsers(JNIEnv* env,
                                                                            jobject thiz,
                                                                            jlong lpTTInstance,
                                                                            jobjectArray lpUsers,
                                                                            jobject lpnHowMany)
    {
        THROW_NULLEX(env, lpnHowMany, false);

        int n_users = 0;
        if(!lpUsers)
        {
            if(TT_GetServerUsers(reinterpret_cast<TTInstance*>(lpTTInstance),
                                 NULL, &n_users))
            {
                setIntPtr(env, lpnHowMany, n_users);
                return true;
            }
            return false;
        }
    
        n_users = getIntPtr(env, lpnHowMany);
        std::vector<User> users(n_users);

        if(n_users>0 &&
           TT_GetServerUsers(reinterpret_cast<TTInstance*>(lpTTInstance), &users[0],
                             &n_users))
        {
            n_users = std::min(n_users, getIntPtr(env, lpnHowMany));
            setIntPtr(env, lpnHowMany, n_users);

            for(size_t i=0;i<n_users;i++)
            {
                jclass cls = env->FindClass("dk/bearware/User");
                jobject user_obj = newObject(env, cls);
                setUser(env, users[i], user_obj);
                env->SetObjectArrayElement(lpUsers, i, user_obj);
            }
            return true;
        }
        return false;
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_getRootChannelID(JNIEnv* env,
                                                                          jobject thiz,
                                                                          jlong lpTTInstance)
    {
        return TT_GetRootChannelID(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_getMyChannelID(JNIEnv* env,
                                                                        jobject thiz,
                                                                        jlong lpTTInstance)
    {
        return TT_GetMyChannelID(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getChannel(JNIEnv* env,
                                                                        jobject thiz,
                                                                        jlong lpTTInstance,
                                                                        jint nChannelID,
                                                                        jobject lpChannel)
    {
        THROW_NULLEX(env, lpChannel, false);

        Channel chan;
        if(TT_GetChannel(reinterpret_cast<TTInstance*>(lpTTInstance),
                         nChannelID, &chan))
        {
            setChannel(env, chan, lpChannel, N2J);
            return true;
        }
        return false;
    }

    JNIEXPORT jstring JNICALL Java_dk_bearware_TeamTalkBase_getChannelPath(JNIEnv* env,
                                                                           jobject thiz,
                                                                           jlong lpTTInstance,
                                                                           jint nChannelID)
    {
        TTCHAR channel[TT_STRLEN] = "";
        TT_GetChannelPath(reinterpret_cast<TTInstance*>(lpTTInstance),
                          nChannelID, channel);
        return env->NewStringUTF(channel);
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_getChannelIDFromPath(JNIEnv* env,
                                                                              jobject thiz,
                                                                              jlong lpTTInstance,
                                                                              jstring szChannelPath)
    {
        THROW_NULLEX(env, szChannelPath, -1);

        ttstr channel(env, szChannelPath);
        return TT_GetChannelIDFromPath(reinterpret_cast<TTInstance*>(lpTTInstance),
                                       channel);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getChannelUsers(JNIEnv* env,
                                                                             jobject thiz,
                                                                             jlong lpTTInstance,
                                                                             jint nChannelID,
                                                                             jobjectArray lpUsers,
                                                                             jobject lpnHowMany)
    {
        THROW_NULLEX(env, lpnHowMany, false);

        int n_users = 0;
        if(!lpUsers)
        {
            if(TT_GetChannelUsers(reinterpret_cast<TTInstance*>(lpTTInstance), 
                                  nChannelID, NULL, &n_users))
            {
                setIntPtr(env, lpnHowMany, n_users);
                return true;
            }
            return false;
        }
    
        n_users = getIntPtr(env, lpnHowMany);
        std::vector<User> users(n_users);

        if(n_users>0 &&
           TT_GetChannelUsers(reinterpret_cast<TTInstance*>(lpTTInstance), 
                              nChannelID, &users[0], &n_users))
        {
            n_users = std::min(n_users, getIntPtr(env, lpnHowMany));
            setIntPtr(env, lpnHowMany, n_users);
            for(size_t i=0;i<n_users;i++)
            {
                jclass cls = env->FindClass("dk/bearware/User");
                jobject user_obj = newObject(env, cls);
                setUser(env, users[i], user_obj);
                env->SetObjectArrayElement(lpUsers, i, user_obj);
            }

            return true;
        }
        return false;
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getChannelFiles(JNIEnv* env,
                                                                             jobject thiz,
                                                                             jlong lpTTInstance,
                                                                             jint nChannelID,
                                                                             jobjectArray lpRemoteFiles,
                                                                             jobject lpnHowMany)
    {
        THROW_NULLEX(env, lpnHowMany, false);

        int n_files = 0;
        if(!lpRemoteFiles)
        {
            if(TT_GetChannelFiles(reinterpret_cast<TTInstance*>(lpTTInstance), 
                                  nChannelID, NULL, &n_files))
            {
                setIntPtr(env, lpnHowMany, n_files);
                return true;
            }
            return false;
        }
    
        n_files = getIntPtr(env, lpnHowMany);
        std::vector<RemoteFile> files(n_files);

        if(n_files>0 &&
           TT_GetChannelFiles(reinterpret_cast<TTInstance*>(lpTTInstance), 
                              nChannelID, &files[0], &n_files))
        {
            n_files = std::min(n_files, getIntPtr(env, lpnHowMany));
            if(n_files>0)
            {
                std::vector<jobject> jfiles(n_files);
                jclass cls = env->FindClass("dk/bearware/RemoteFile");
                for(size_t i=0;i<n_files;i++)
                {
                    jobject file_obj = newObject(env, cls);
                    setRemoteFile(env, files[i], file_obj);
                    env->SetObjectArrayElement(lpRemoteFiles, i, file_obj);
                }
            }
            setIntPtr(env, lpnHowMany, n_files);
            return true;
        }
        return false;
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getChannelRemoteFile(JNIEnv* env,
                                                                                  jobject thiz,
                                                                                  jlong lpTTInstance,
                                                                                  jint nChannelID,
                                                                                  jint nFileID,
                                                                                  jobject lpRemoteFile)
    {
        THROW_NULLEX(env, lpRemoteFile, false);

        RemoteFile finfo;
        if(TT_GetChannelFile(reinterpret_cast<TTInstance*>(lpTTInstance), 
                             nChannelID, nFileID, &finfo))
        {
            setRemoteFile(env, finfo, lpRemoteFile);
            return true;
        }
        return false;
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_isChannelOperator(JNIEnv* env,
                                                                               jobject thiz,
                                                                               jlong lpTTInstance,
                                                                               jint nUserID,
                                                                               jint nChannelID)
    {
        return TT_IsChannelOperator(reinterpret_cast<TTInstance*>(lpTTInstance), 
                                    nUserID, nChannelID);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getServerChannels(JNIEnv* env,
                                                                               jobject thiz,
                                                                               jlong lpTTInstance,
                                                                               jobjectArray lpChannels,
                                                                               jobject lpnHowMany)
    {
        THROW_NULLEX(env, lpnHowMany, false);

        int n_channels = 0;
        if(!lpChannels)
        {
            if(TT_GetServerChannels(reinterpret_cast<TTInstance*>(lpTTInstance),
                                    NULL, &n_channels))
            {
                setIntPtr(env, lpnHowMany, n_channels);
                return true;
            }
            return false;
        }
    
        n_channels = getIntPtr(env, lpnHowMany);
        std::vector<Channel> channels(n_channels);

        if(n_channels>0 &&
           TT_GetServerChannels(reinterpret_cast<TTInstance*>(lpTTInstance), &channels[0],
                                &n_channels))
        {
            n_channels = std::min(n_channels, getIntPtr(env, lpnHowMany));
            setIntPtr(env, lpnHowMany, n_channels);
            jclass cls = env->FindClass("dk/bearware/Channel");
            for(size_t i=0;i<n_channels;i++)
            {
                jobject chan_obj = newObject(env, cls);
                setChannel(env, channels[i], chan_obj, N2J);
                env->SetObjectArrayElement(lpChannels, i, chan_obj);
            }
            return true;
        }
        return false;
    }

    JNIEXPORT jint JNICALL Java_dk_bearware_TeamTalkBase_getMyUserID(JNIEnv* env,
                                                                     jobject thiz,
                                                                     jlong lpTTInstance)
    {
        return TT_GetMyUserID(reinterpret_cast<TTInstance*>(lpTTInstance));
    }                                                                           

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getMyUserAccount(JNIEnv* env,
                                                                              jobject thiz,
                                                                              jlong lpTTInstance,
                                                                              jobject lpUserAccount)
    {
        THROW_NULLEX(env, lpUserAccount, false);

        UserAccount account;
        if(TT_GetMyUserAccount(reinterpret_cast<TTInstance*>(lpTTInstance), &account))
        {
            setUserAccount(env, account, lpUserAccount, N2J);
            return true;
        }
        return false;
    }                                                                           

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getMyUserType(JNIEnv* env,
                                                                           jobject thiz,
                                                                           jlong lpTTInstance)
    {
        return TT_GetMyUserType(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getMyUserData(JNIEnv* env,
                                                                           jobject thiz,
                                                                           jlong lpTTInstance)
    {
        return TT_GetMyUserData(reinterpret_cast<TTInstance*>(lpTTInstance));
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getUser(JNIEnv* env,
                                                                     jobject thiz,
                                                                     jlong lpTTInstance,
                                                                     jint nUserID,
                                                                     jobject lpUser)
    {
        THROW_NULLEX(env, lpUser, false);

        User user;
        if(TT_GetUser(reinterpret_cast<TTInstance*>(lpTTInstance),
                      nUserID, &user))
        {
            setUser(env, user, lpUser);
            return true;
        }
        return false;
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getUserStatistics(JNIEnv* env,
                                                                               jobject thiz,
                                                                               jlong lpTTInstance,
                                                                               jint nUserID,
                                                                               jobject lpUserStatistics)
    {
        THROW_NULLEX(env, lpUserStatistics, false);

        UserStatistics stats;
        if(TT_GetUserStatistics(reinterpret_cast<TTInstance*>(lpTTInstance),
                                nUserID, &stats))
        {
            setUserStatistics(env, stats, lpUserStatistics);
            return true;
        }
        return false;
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getUserByUsername(JNIEnv* env,
                                                                               jobject thiz,
                                                                               jlong lpTTInstance,
                                                                               jstring szUsername,
                                                                               jobject lpUser)
    {
        THROW_NULLEX(env, lpUser, false);

        User user;
        if(TT_GetUserByUsername(reinterpret_cast<TTInstance*>(lpTTInstance),
                                ttstr(env, szUsername), &user))
        {
            setUser(env, user, lpUser);
            return true;
        }
        return false;
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_setUserVolume(JNIEnv* env,
                                                                           jobject thiz,
                                                                           jlong lpTTInstance,
                                                                           jint nUserID,
                                                                           jint nStreamType,
                                                                           jint nVolume) 
    {
        return TT_SetUserVolume(reinterpret_cast<TTInstance*>(lpTTInstance),
                                nUserID, (StreamType)nStreamType, nVolume);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_setUserGainLevel(JNIEnv* env,
                                                                              jobject thiz,
                                                                              jlong lpTTInstance,
                                                                              jint nUserID,
                                                                              jint nStreamType,
                                                                              jint nGainLevel)
    {
        return TT_SetUserGainLevel(reinterpret_cast<TTInstance*>(lpTTInstance),
                                   nUserID, (StreamType)nStreamType, nGainLevel);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_setUserMute(JNIEnv* env,
                                                                         jobject thiz,
                                                                         jlong lpTTInstance,
                                                                         jint nUserID,
                                                                         jint nStreamType,
                                                                         jboolean bMute)
    {
        return TT_SetUserMute(reinterpret_cast<TTInstance*>(lpTTInstance),
                              nUserID, (StreamType)nStreamType, bMute);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_setUserStoppedPlaybackDelay(JNIEnv* env,
                                                                                         jobject thiz,
                                                                                         jlong lpTTInstance,
                                                                                         jint nUserID,
                                                                                         jint nStreamType,
                                                                                         jint nDelayMSec)
    {
        return TT_SetUserStoppedPlaybackDelay(reinterpret_cast<TTInstance*>(lpTTInstance),
                                              nUserID, (StreamType)nStreamType, nDelayMSec);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_setUserPosition(JNIEnv* env,
                                                                             jobject thiz,
                                                                             jlong lpTTInstance,
                                                                             jint nUserID,
                                                                             jint nStreamType,
                                                                             jfloat x, 
                                                                             jfloat y, 
                                                                             jfloat z)
    {
        return TT_SetUserPosition(reinterpret_cast<TTInstance*>(lpTTInstance),
                                  nUserID, (StreamType)nStreamType, x, y, z);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_setUserStereo(JNIEnv* env,
                                                                           jobject thiz,
                                                                           jlong lpTTInstance,
                                                                           jint nUserID,
                                                                           jint nStreamType,
                                                                           jboolean bLeftSpeaker,
                                                                           jboolean bRightSpeaker)
    {
        return TT_SetUserStereo(reinterpret_cast<TTInstance*>(lpTTInstance),
                                nUserID, (StreamType)nStreamType, bLeftSpeaker, bRightSpeaker);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_setUserAudioFolder(JNIEnv* env,
                                                                                jobject thiz,
                                                                                jlong lpTTInstance,
                                                                                jint nUserID,
                                                                                jstring szFolderPath,
                                                                                jstring szFileNameVars,
                                                                                jint uAFF)
    {
        THROW_NULLEX(env, szFolderPath, false);
        THROW_NULLEX(env, szFileNameVars, false);

        return TT_SetUserAudioFolder(reinterpret_cast<TTInstance*>(lpTTInstance),
                                     nUserID, ttstr(env, szFolderPath), 
                                     ttstr(env, szFileNameVars),
                                     (AudioFileFormat)uAFF);
    }


    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_setUserAudioStreamBufferSize(JNIEnv* env,
                                                                                          jobject thiz,
                                                                                          jlong lpTTInstance,
                                                                                          jint nUserID,
                                                                                          jint uStreamType,
                                                                                          jint nMSec)
    {
        return TT_SetUserAudioStreamBufferSize(reinterpret_cast<TTInstance*>(lpTTInstance),
                                               nUserID, (StreamType)uStreamType, nMSec);
    }

//TODO: TT_AcquireUserAudioBlock
//TODO: TT_ReleaseUserAudioBlock

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getFileTransferInfo(JNIEnv* env,
                                                                                 jobject thiz,
                                                                                 jlong lpTTInstance,
                                                                                 jint nTransferID,
                                                                                 jobject lpFileTransfer)
    {
        THROW_NULLEX(env, lpFileTransfer, false);

        FileTransfer filetx;
        if(TT_GetFileTransferInfo(reinterpret_cast<TTInstance*>(lpTTInstance),
                                  nTransferID, &filetx))
        {
            setFileTransfer(env, filetx, lpFileTransfer);
            return true;
        }
        return false;
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_cancelFileTransfer(JNIEnv* env,
                                                                                jobject thiz,
                                                                                jlong lpTTInstance,
                                                                                jint nTransferID)
    {
        return TT_CancelFileTransfer(reinterpret_cast<TTInstance*>(lpTTInstance),
                                     nTransferID);
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getBannedUsers(JNIEnv* env,
                                                                            jobject thiz,
                                                                            jlong lpTTInstance,
                                                                            jobjectArray lpBannedUsers,
                                                                            jobject lpnHowMany)
    {
        THROW_NULLEX(env, lpnHowMany, false);

        int n_bans = 0;
        if(!lpBannedUsers)
        {
            if(TT_GetBannedUsers(reinterpret_cast<TTInstance*>(lpTTInstance),
                                 NULL, &n_bans))
            {
                setIntPtr(env, lpnHowMany, n_bans);
                return true;
            }
            return false;
        }

        n_bans = getIntPtr(env, lpnHowMany);
        std::vector<BannedUser> bans(n_bans);
        if(n_bans > 0 &&
           TT_GetBannedUsers(reinterpret_cast<TTInstance*>(lpTTInstance),
                             &bans[0], &n_bans))
        {
            n_bans = std::min(n_bans, getIntPtr(env, lpnHowMany));
            if(n_bans>0)
            {
                std::vector<jobject> jbans(n_bans);
                jclass cls_ban = env->FindClass("dk/bearware/BannedUser");
                jmethodID midInit = env->GetMethodID(cls_ban, "<init>", "()V");
                assert(midInit);
                for(size_t i=0;i<n_bans;i++)
                {
                    jbans[i] = env->NewObject(cls_ban, midInit);
                    assert(jbans[i]);
                    setBannedUser(env, bans[i], jbans[i]);
                    env->SetObjectArrayElement(lpBannedUsers, i, jbans[i]);
                }
            }
            setIntPtr(env, lpnHowMany, n_bans);
            return true;
        }
        return false;
    }

    JNIEXPORT jboolean JNICALL Java_dk_bearware_TeamTalkBase_getUserAccounts(JNIEnv* env,
                                                                             jobject thiz,
                                                                             jlong lpTTInstance,
                                                                             jobjectArray lpUserAccounts,
                                                                             jobject lpnHowMany)
    {
        THROW_NULLEX(env, lpnHowMany, false);

        int n_accounts = 0;
        if(!lpUserAccounts)
        {
            if(TT_GetUserAccounts(reinterpret_cast<TTInstance*>(lpTTInstance),
                                  NULL, &n_accounts))
            {
                setIntPtr(env, lpnHowMany, n_accounts);
                return true;
            }
            return false;
        }

        n_accounts = getIntPtr(env, lpnHowMany);
        std::vector<UserAccount> accounts(n_accounts);
        if(n_accounts > 0 &&
           TT_GetUserAccounts(reinterpret_cast<TTInstance*>(lpTTInstance),
                              &accounts[0], &n_accounts))
        {
            n_accounts = std::min(n_accounts, getIntPtr(env, lpnHowMany));
            if(n_accounts>0)
            {
                std::vector<jobject> jaccounts(n_accounts);
                jclass cls_account = env->FindClass("dk/bearware/UserAccount");
                jmethodID midInit = env->GetMethodID(cls_account, "<init>", "()V");
                assert(midInit);
                for(size_t i=0;i<n_accounts;i++)
                {
                    jaccounts[i] = env->NewObject(cls_account, midInit);
                    assert(jaccounts[i]);
                    setUserAccount(env, accounts[i], jaccounts[i], N2J);
                    env->SetObjectArrayElement(lpUserAccounts, i, jaccounts[i]);
                }
            }
            setIntPtr(env, lpnHowMany, n_accounts);
            return true;
        }
        return false;
    }

    JNIEXPORT jstring JNICALL Java_dk_bearware_TeamTalkBase_getErrorMessage(JNIEnv* env,
                                                                            jobject thiz,
                                                                            jint nError)
    {
        TTCHAR szError[TT_STRLEN] = "";
        TT_GetErrorMessage(nError, szError);
        return env->NewStringUTF(szError);
    }

//TODO: TT_DesktopInput_KeyTranslate
//TODO: TT_DesktopInput_Execute
//TODO: TT_HotKey_*
                                                                              
} //extern "C"