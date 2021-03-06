/****************************************************************************
**
** Copyright (C) 2012 Lorem Ipsum Mediengesellschaft m.b.H.
**
** GNU General Public License
** This file may be used under the terms of the GNU General Public License
** version 3 as published by the Free Software Foundation and
** appearing in the file LICENSE.GPL included in the packaging of this file.
**
****************************************************************************/

#ifndef PHONE_INCLUDE_H
#define PHONE_INCLUDE_H

#include "e.h"

#include <QObject>
#include <QVector>
#include <QVariantMap>

class GUI_Handler;
class LogInfo;

namespace phone
{
    class Account;
    class Call;

    namespace api {
        class Interface;
    }

/**
 * Phone settings
 */
struct Settings
{
    enum Srtp {
        SRTP_DISABLED = 0,
        SRTP_OPTIONAL,
        SRTP_MANDATORY
    };
    enum SrtpSignaling {
        SRTP_SIGNALING_NOTREQUIRED = 0,
        SRTP_SIGNALING_TLS,
        SRTP_SIGNALING_ENDTOEND
    };

    unsigned int port_;

    QString stun_server_;

    float sound_level_;
    float micro_level_;

    /**
     * SRTP usage
     * SRTP_DISABLED (0): SRTP is disabled, and incoming call with RTP/SAVP 
     *    transport will be rejected with 488/Not Acceptable Here response.
     * SRTP_OPTIONAL (1): SRTP will be used if  remote supports it, but the
     *    call may fall back to unsecure media. Incoming call with RTP/SAVP
     *    is accepted and responded with RTP/SAVP too.
     * SRTP_MANDATORY (2): Secure media is mandatory, and the call can only
     *    proceed if secure media can be established.
     */
    Srtp srtp;

    /**
     * SRTP secure signaling; valid values:
     * SRTP_SIGNALING_NOTREQUIRED (0): SRTP does not require secure signaling
     * SRTP_SIGNALING_TLS (1): SRTP requires secure transport such as TLS.
     * SRTP_SIGNALING_ENDTOEND (2): SRTP requires secure end-to-end transport
     *    (sips: URI scheme).
     */
    SrtpSignaling srtp_signaling;
};

/**
 * Base phone class
 */
class Phone : public QObject
{
    Q_OBJECT

public:
    static const QString ERROR_FILE;

    /**
     * Constructor
     * @param api Pointer to the phone API
     */
    Phone(api::Interface *api);

    /**
     * Destructor
     * Delete calls and api
     */
    virtual ~Phone();

    /**
     * Initialize the phone
     */
    bool init(const Settings &settings);
    
    /**
     * Set the javascript handler object
     */
    void set_GUI_Handler(GUI_Handler* gui_handler);

    /**
     * @return Phone API implementation
     */
    api::Interface *getApi();

    /**
     * Return last error message
     * @return error message
     */
    const QString &getErrorMessage() const;

    /**
     * Checks if account is valid
     * @return true, if account is valid
     */
    bool checkAccountStatus() const;

    /**
     * Registers the account
     * @param acc Account
     * @return true, if registration was successful
     */
    bool registerUser(const Account &acc);

    /**
     * Get information about the account
     * @return Map with account information
     */
    QVariantMap getAccountInfo() const;

    /**
     * Starting a call to the given address
     * @param url Destination address (e.g. "SIP:user@domain")
     * @return New Call object or NULL if unsuccessful
     */
    Call *makeCall(const QString &url);
    
    /**
     * Starting a call to the given address
     * @param url Destination address (e.g. "SIP:user@domain")
     * @param header_map Map of SIP header names and values.
     * @return New Call object or NULL if unsuccessful
     */
    Call *makeCall(const QString &url, const QVariantMap &header_map);

    /**
     * Hanging up incoming and all active calls
     */
    void hangUpAll();

    /**
     * Return call object via id
     * @param call_id Call ID
     * @return Call object
     */
    Call *getCall(const int call_id);

    /**
     * Get list of active calls
     * @return List of active calls
     */
    QVariantList getActiveCallList() const;

    /**
     * Set sound level
     * @param soundLevel 0.0f (mute) to 1.0f (full)
     */
    void setSoundSignal(const float soundLevel);

    /**
     * Set microphone level
     * @param microLevel 0.0f (mute) to 1.0f (full)
     */
    void setMicroSignal(const float microLevel);

    /**
     * Get information about signal levels of sound and microphone
     * @param Map with 'sound' and 'micro' signal levels (floats)
     */
    QVariantMap getSignalLevels() const;
    
    /**
     * Set priority of codec to new_priority
     * @param codec Name of Codec
     * @param new_priority Range 0...PJMEDIA_CODEC_PRIO_HIGHEST
     */
    void setCodecPriority(const QString &codec, int new_priority);
    
    /**
     * Select or change sound device
     * @param input Device ID of the capture device.
     * @param output Device ID of the playback device.
     */
    void setSoundDevice(const int input, const int output);
    
    /**
     * Return a list of all available sound devices
     * @return list of QVariantMaps with device information
     */
    QVariantList getSoundDevices() const;

    /**
     * Return all codecs and priorities
     * @return Map of codecs with priorities
     */
    QVariantMap getCodecPriorities() const;

    /**
     * Hangup all active calls and unregister account
     */
    void unregister();

public slots:
    /**
     * This slot gets called on incoming calls
     * @param call Call*, the call handler class
     */
    void slotIncomingCall(int call_id, const QString &url, const QString &name, const QVariantMap &header_map);

    /**
     * This slot gets called when the state of a call has changed
     * @param call_id ID of call
     * @param call_state The new state
     * @param last_status The old state
     */
    void slotCallState(int call_id, int call_state, int last_status);

    /**
     * This slot gets called when the sound level has changed
     * @param level The new sound level
     */
    void slotSoundLevel(int level);

    /**
     * This slot gets called when the microphone level has changed
     * @param level The new micro level
     */
    void slotMicroLevel(int level);

    /**
     * This slot gets called when the account registration state has changed
     * @param state The new account state
     */
    void slotAccountState(const int state);

    /**
     * Slot to log messages
     * @param info
     */
    void slotLogData(const LogInfo &info);

    /**
     * Slot to start ring sound
     */
    void slotRingSound();
    
    /**
     * Slot to stop sounds
     */
    void slotStopSound();
    
    /**
     * Send DTMF digits.
     * @param call_id ID of call
     * @param digits The DTMF digits that should be sent.
     */
    void sendDTMFDigits(int call_id, const QString &digits);

    /**
     * Send signal of an received text message
     */
    void slotIncomingTextMessage(int call_id, const QString &from, const QString &to, const QString &contact, const QString &mime_type, const QString &body);

signals:
    void signalIncomingCall(const QString &call, const QVariantMap &header_map);

private:
    api::Interface *api_;
    GUI_Handler* gui_handler_;
    QVector<Call*> calls_;
    QString error_msg_;

    bool addToCallList(Call *call);
};

} // phone::

#endif // PHONE_INCLUDE_H
