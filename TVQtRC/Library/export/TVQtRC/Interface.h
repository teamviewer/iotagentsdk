//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2021 TeamViewer Germany GmbH                                     //
//                                                                                //
// Permission is hereby granted, free of charge, to any person obtaining a copy   //
// of this software and associated documentation files (the "Software"), to deal  //
// in the Software without restriction, including without limitation the rights   //
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      //
// copies of the Software, and to permit persons to whom the Software is          //
// furnished to do so, subject to the following conditions:                       //
//                                                                                //
// The above copyright notice and this permission notice shall be included in all //
// copies or substantial portions of the Software.                                //
//                                                                                //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    //
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  //
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  //
// SOFTWARE.                                                                      //
//********************************************************************************//
#pragma once

#include "AbstractChat.h"
#include "AccessControl.h"
#include "BaseUrlParseResultCode.h"
#include "ChatInfo.h"
#include "ConnectionData.h"
#include "ControlMode.h"
#include "InstantSupportData.h"
#include "InstantSupportError.h"

#include <QtCore/QObject>
#include <QtCore/QUrl>

#include <functional>

QT_FORWARD_DECLARE_CLASS(QWindow)

namespace tvqtsdk
{

class TVQtRCPluginInterface
{
public:
	virtual ~TVQtRCPluginInterface() {}

	/**
	 * @brief setRemoteScreenSdkBaseUrl sets a custom base URL where the plugin will locate the socket used to communicate
	 * with the Agent, as well as create its own (plugin) sockets. If never called, defaults to "unix:///tmp". If you call
	 * this method, you should do so once early on after the plugin has been loaded, before any [register*] methods.
	 * Affects all subsequently initiated communication sessions.
	 * @param remoteScreenSdkBaseUrl the new base URL, e.g., "unix:///root/tvrun"
	 */
	virtual void setRemoteScreenSdkBaseUrl(QUrl remoteScreenSdkBaseUrl) = 0;

	/**
	 * @brief setRemoteScreenSdkBaseUrlChecked sets a custom base URL where the plugin will locate the socket used to communicate
	 * with the Agent, as well as create its own (plugin) sockets. If never called, defaults to "unix:///tmp". If you call
	 * this method, you should do so once early on after the plugin has been loaded, before any [register*] methods.
	 * Affects all subsequently initiated communication sessions.
	 * This functions checks if the provided path in combination with the created sockets names, will exceeed the limit for socket names.
	 * If the limit is reached, the function will return a corresponding error code and the SDK will not use the provided path.
	 * In that case, please provide a shorter path to this function.
	 * @param remoteScreenSdkBaseUrl the new base URL, e.g., "unix:///root/tvrun"
	 */
	virtual BaseUrlParseResultCode setRemoteScreenSdkBaseUrlChecked(QUrl remoteScreenSdkBaseUrl) = 0;

	/**
	 * @brief registerApplication registers the current application on the TeamViewer agent and initiates communication.
	 * For remote control:
	 * - This stops a previously running TeamViewer session and enables view and control of all  application windows
	 *   (depending on the set control mode see setControlMode).
	 * - IMPORTANT: It depends on the current platform and setup if remote control of the whole application is available
	 *   or not!
	 */
	virtual void registerApplication() = 0;

	/**
	 * @brief registerApplication shuts down all communication with the TeamViewer agent.
	 * If there is a remote control session running, it will be stopped and any registered window will be
	 * deregistered.
	 * NOTE: All registered external slots stay registerd (e.g. using registerAgentCommunicationStatusChanged()).
	 */
	virtual void deregisterApplication() = 0;

	/**
	 * @brief registerApplicationWindow sets the given Qt window to be remote controlled by the TeamViewer agent.
	 * There can only be one window registered, any previously registered window will be unregistered.
	 * After registering a window a connection to the TeamViewer agent will be established if needed.
	 * @param window Qt window to be remote controlled
	 */
	virtual void registerApplicationWindow(QWindow* const window) = 0;

	/**
	 * @brief deregisterApplicationWindow unsets a previously registered Qt window. If there is a remote control session
	 * running, it will NOT be stopped to be able to register another Qt Window.
	 * For stopping the remote control session or shutting down the whole communication, see deregisterApplication().
	 */
	virtual void deregisterApplicationWindow() = 0;

	/**
	 * @brief getControlMode indicates the current control mode
	 * @return current control mode
	 */
	virtual ControlMode getControlMode() const = 0;

	/**
	 * @brief setControlMode requests the control mode value to be set to the given value:
	 *     ControlMode::Disabled:    no image and no input
	 *     ControlMode::ViewOnly:    image and no input
	 *     ControlMode::FullControl: image and input
	 * @param value control mode value to set
	 */
	virtual void setControlMode(ControlMode value) = 0;

	/**
	 * @brief registerControlModeChanged registers the given slot to handle a state change for the control mode
	 * @param slot function to execute for this event
	 * @param context trackable QObject that indicates if the slot can be called safely
	 * @return connection object for tracking and managing the resulting signal-slot relationship
	 */
	virtual QMetaObject::Connection registerControlModeChanged(const std::function<void(ControlMode controlModeValue)>& slot, const QObject* context = nullptr) = 0;

	/**
	 * @brief getAccessMode requests the current access mode for the given feature
	 * @param feature is the AccessControl value for which access mode is requested
	 * @param access contains the requested access mode, if the return value of this function is false then requestedAccess will stay unchanged
	 * @return true if this function call is successfull, false otherwise
	 */
	virtual bool getAccessMode(AccessControl feature, Access& access) = 0;

	/**
	 * @brief setAccessMode requests to set the given access value for the given feature
	 * @param feature is the AccessControl value to set the given access mode to this feature
	 * @param access is the access mode value to set
	 * @return true if this function call is successfull, false otherwise
	 */
	virtual bool setAccessMode(AccessControl feature, Access access) = 0;

	/**
	 * @brief registerAccessModeChanged registers the given slot to handle an access change for the given access control
	 * @param slot function to execute for this event
	 * @param context trackable QObject that indicates if the slot can be called safely
	 * @return connection object for tracking and managing the resulting signal-slot relationship
	 */
	virtual QMetaObject::Connection registerAccessModeChanged(const std::function<void(AccessControl feature, Access access)>& slot, const QObject* context = nullptr) = 0;

	/**
	 * @brief registerAcessConfirmationRequest registers the given slot to handle an access confirmation request for the given feature. The reply should be sent by accessConfirmationReply().
	 * @param slot function to execute for this event
	 * @param context trackable QObject that indicates if the slot can be called safely
	 * @return connection object for tracking and managing the resulting signal-slot relationship
	 */
	virtual QMetaObject::Connection registerAccessConfirmationRequest(const std::function<void(AccessControl feature, uint32_t timeout)>& slot, const QObject* context = nullptr) = 0;

	/**
	 * @brief registerForInstantSupportErrorNotification registers the given slot to handle the instant support error notification.
	 * @param slot function to execute for this event
	 * @param context trackable QObject that indicates if the slot can be called safely
	 * @return connection object for tracking and managing the resulting signal-slot relationship
	 */
	virtual QMetaObject::Connection registerForInstantSupportErrorNotification(const std::function<void(InstantSupportError errorCode)>& slot, const QObject* context = nullptr) = 0;

	/**
	 * @brief registerForInstantSupportModifiedNotification registers the given slot to handle the instant support modified notification.
	 * @param slot function to execute for this event
	 * @param context trackable QObject that indicates if the slot can be called safely
	 * @return connection object for tracking and managing the resulting signal-slot relationship
	 */
	virtual QMetaObject::Connection registerForInstantSupportModifiedNotification(const std::function<void(InstantSupportData data)>& slot, const QObject* context = nullptr) = 0;

	using ConfirmationResponseFunction = std::function<void(ConnectionUserConfirmation confirmation)>;
	/**
	 * @brief registerForInstantSupportConfirmationRequest registers the given slot to handle the instant support confirmation request.
	 * @param slot function to execute for this event
	 * @param context trackable QObject that indicates if the slot can be called safely
	 * @return connection object for tracking and managing the resulting signal-slot relationship
	 */
	virtual QMetaObject::Connection registerForInstantSupportConfirmationRequest(const std::function<void(ConfirmationResponseFunction response)>& slot, const QObject* context = nullptr) = 0;

	/**
	 * @brief accessConfirmationReply sends the confirmation reply for the given feature on the access confirmation request received by registerAcessConfirmationRequest()
	 * @param feature is the name of the feature for which confirmation reply is sent
	 * @param confirmed true if the access for the given feature is confirmed, false if not confirmed
	 * @return true if this function call is successfull, false otherwise
	 */
	virtual bool accessConfirmationReply(AccessControl feature, bool confirmed) = 0;

	/**
	 * @brief isTeamViewerSessionRunning indicates if an incoming TeamViewer remote screen session is currently running.
	 * @return true is remote control is enabled, false otherwise
	 */
	virtual bool isTeamViewerSessionRunning() const = 0;

	/**
	 * @brief terminateTeamViewerSessions requests to terminate all incoming TeamViewer remote screen sessions
	 */
	virtual void terminateTeamViewerSessions() = 0;

	/**
	 * @brief registerTeamViewerSessionStarted registers the given slot to handle a started TeamViewer remote screen session
	 * @param slot function to execute for this event
	 * @param context trackable QObject that indicates if the slot can be called safely
	 * @return connection object for tracking and managing the resulting signal-slot relationship
	 */
	virtual QMetaObject::Connection registerTeamViewerSessionStarted(const std::function<void()>& slot, const QObject* context = nullptr) = 0;

	/**
	 * @brief registerTeamViewerSessionStopped registers the given slot to handle a stopped TeamViewer remote screen session
	 * @param slot function to execute for this event
	 * @param context trackable QObject that indicates if the slot can be called safely
	 * @return connection object for tracking and managing the resulting signal-slot relationship
	 */
	virtual QMetaObject::Connection registerTeamViewerSessionStopped(const std::function<void()>& slot, const QObject* context = nullptr) = 0;

	/**
	 * @brief isAgentCommunicationEstablished indicates if a connection to TeamViewerAgent has been established successfully
	 * @return true if a connection to TeamViewerAgent has been established
	 */
	virtual bool isAgentCommunicationEstablished() const = 0;

	/**
	 * @brief registerAgentCommunicationStatusChanged registers the given slot to handle agent communication status changes
	 * @param slot function to execute for this event
	 * @param context trackable QObject that indicates if the slot can be called safely
	 * @return connection object for tracking and managing the resulting signal-slot relationship
	 */
	virtual QMetaObject::Connection registerAgentCommunicationStatusChanged(const std::function<void(bool isConnected)>& slot, const QObject* context = nullptr) = 0;

	/**
	 * @brief startLogging starts file logging for the plugin in the given directory.
	 * @param logFolderPath path to a directory to write log file into
	 */
	virtual void startLogging(const QString& logFolderPath) = 0;

	/**
	 * @brief stopLogging stops file logging
	 */
	virtual void stopLogging() = 0;

	/**
	 * @brief requestInstantSupport requests a service case
	 * @param accessToken User or company access token for authentication
	 * @param group Name of the group the session code will be inserted into.
	 * @param name Name of the end customer. Maximum length is 100 characters.
	 * @param description Description for the service case
	 * @param sessionCode Session code is an optional parameter.
	 * If it is not empty then it will be checked to be valid, otherwise new session code will be created.
	 * @return true if this function call is successfull, false otherwise
	 */
	virtual bool requestInstantSupport(
		const QString& accessToken,
		const QString& name,
		const QString& group,
		const QString& description,
		const QString& sessionCode) = 0;

	/**
	 * @brief requestInstantSupportV2 requests a service case
	 * @param accessToken User or company access token for authentication
	 * @param group Name of the group the session code will be inserted into.
	 * @param name Name of the end customer. Maximum length is 100 characters.
	 * @param description Description for the service case
	 * @param sessionCode Session code is an optional parameter.
	 * If it is not empty then it will be checked to be valid, otherwise new session code will be created.
	 * @param email Email address of the operator, is an optional parameter
	 * @return true if this function call is successfull, false otherwise
	 */
	virtual bool requestInstantSupportV2(
		const QString& accessToken,
		const QString& name,
		const QString& group,
		const QString& description,
		const QString& sessionCode,
		const QString& email) = 0;

	/**
	 * @brief getChat requests pointer to access to the chat module
	 * @return pointer of the chat module
	 */
	virtual AbstractChat* getChat() = 0;
};

} // namespace tvqtsdk

#define TVQtRCPluginInterface_iid "com.teamviewer.TVQtRCInterface"
Q_DECLARE_INTERFACE(tvqtsdk::TVQtRCPluginInterface, TVQtRCPluginInterface_iid)
