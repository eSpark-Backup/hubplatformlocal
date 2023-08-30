// This callback file is created for your convenience. You may add application
// code to this file. If you regenerate this file over a previous version, the
// previous version will be overwritten and any code you have added will be
// lost.

#include "app/framework/include/af.h"
#include EMBER_AF_API_NETWORK_STEERING




/** @brief Message Sent
 *
 * This function is called by the application framework from the message sent
 * handler, when it is informed by the stack regarding the message sent status.
 * All of the values passed to the emberMessageSentHandler are passed on to this
 * callback. This provides an opportunity for the application to verify that its
 * message has been sent successfully and take the appropriate action. This
 * callback should return a bool value of true or false. A value of true
 * indicates that the message sent notification has been handled and should not
 * be handled by the application framework.
 *
 * @param type   Ver.: always
 * @param indexOrDestination   Ver.: always
 * @param apsFrame   Ver.: always
 * @param msgLen   Ver.: always
 * @param message   Ver.: always
 * @param status   Ver.: always
 */
boolean emberAfMessageSentCallback(EmberOutgoingMessageType type,
                                   int16u indexOrDestination,
                                   EmberApsFrame* apsFrame,
                                   int16u msgLen,
                                   int8u* message,
                                   EmberStatus status)
{
  return false;
}

/** @brief Stack Status
 *
 * This function is called by the application framework from the stack status
 * handler.  This callbacks provides applications an opportunity to be notified
 * of changes to the stack status and take appropriate action.  The return code
 * from this callback is ignored by the framework.  The framework will always
 * process the stack status after the callback returns.
 *
 * @param status   Ver.: always
 */
boolean emberAfStackStatusCallback(EmberStatus status)
{
  return false;
}

#ifndef EZSP_HOST

/** @brief emberChildJoinHandler
 *
 * This callback is called by the stack when a child joins or leaves. 'Joining'
 * is true if the child is joining and false if leaving. The index is the same
 * as the value that should be passed to emberGetChildData() to get this child's
 * data. Note that if the child is leaving, emberGetChildData() will return
 * EMBER_NOT_JOINED if called with this index.
 *
 * @param index The index of the child of interest.  Ver.: always
 * @param joining True if the child is joining, false if the child is leaving. 
 * Ver.: always
 */
void emberChildJoinHandler(int8u index,
                           boolean joining)
{
}
#endif // EZSP_HOST

#ifndef EZSP_HOST

/** @brief ezspChildJoinHandler
 *
 * This callback is called by the stack when a child joins or leaves. 'Joining'
 * is true if the child is joining and false if leaving. The index is the same
 * as the value that should be passed to emberGetChildData() to get this child's
 * data. Note that if the child is leaving, emberGetChildData() will return
 * EMBER_NOT_JOINED if called with this index.
 *
 * @param index The index of the child of interest.  Ver.: always
 * @param joining True if the child is joining, false if the child is leaving. 
 * Ver.: always
 * @param childId The node ID of the child of interest.  Ver.: always
 * @param childEui64 The EUI of the child of interest.  Ver.: always
 * @param childType The node type of the child of interest.  Ver.: always
 */
void ezspChildJoinHandler(int8u index,
                          boolean joining,
                          EmberNodeId childId,
                          EmberEUI64 childEui64,
                          EmberNodeType childType)
{
}
#endif // EZSP_HOST

/** @brief Pre ZDO Message Received
 *
 * This function passes the application an incoming ZDO message and gives the
 * appictation the opportunity to handle it. By default, this callback returns
 * false indicating that the incoming ZDO message has not been handled and
 * should be handled by the Application Framework.
 *
 * @param emberNodeId   Ver.: always
 * @param apsFrame   Ver.: always
 * @param message   Ver.: always
 * @param length   Ver.: always
 */
boolean emberAfPreZDOMessageReceivedCallback(EmberNodeId emberNodeId,
                                             EmberApsFrame* apsFrame,
                                             int8u* message,
                                             int16u length)
{
  return false;
}

/** @brief Pre Message Send
 *
 * This function is called by the framework when it is about to pass a message
 * to the stack primitives for sending.   This message may or may not be ZCL,
 * ZDO, or some other protocol.  This is called prior to
        any ZigBee
 * fragmentation that may be done.  If the function returns true it is assumed
 * the callback has consumed and processed the message.  The callback must also
 * set the EmberStatus status code to be passed back to the caller.  The
 * framework will do no further processing on the message.
        If the
 * function returns false then it is assumed that the callback has not processed
 * the mesasge and the framework will continue to process accordingly.
 *
 * @param messageStruct The structure containing the parameters of the APS
 * message to be sent.  Ver.: always
 * @param status A pointer to the status code value that will be returned to the
 * caller.  Ver.: always
 */
boolean emberAfPreMessageSendCallback(EmberAfMessageStruct* messageStruct,
                                      EmberStatus* status)
{
  return false;
}

/** @brief Pre Message Received
 *
 * This callback is the first in the Application Framework's message processing
 * chain. The Application Framework calls it when a message has been received
 * over the air but has not yet been parsed by the ZCL command-handling code. If
 * you wish to parse some messages that are completely outside the ZCL
 * specification or are not handled by the Application Framework's command
 * handling code, you should intercept them for parsing in this callback. 
     
 *   This callback returns a Boolean value indicating whether or not the message
 * has been handled. If the callback returns a value of true, then the
 * Application Framework assumes that the message has been handled and it does
 * nothing else with it. If the callback returns a value of false, then the
 * application framework continues to process the message as it would with any
 * incoming message.
        Note: 	This callback receives a pointer to an
 * incoming message struct. This struct allows the application framework to
 * provide a unified interface between both Host devices, which receive their
 * message through the ezspIncomingMessageHandler, and SoC devices, which
 * receive their message through emberIncomingMessageHandler.
 *
 * @param incomingMessage   Ver.: always
 */
boolean emberAfPreMessageReceivedCallback(EmberAfIncomingMessage* incomingMessage)
{
  return false;
}

/** @brief Pre Command Received
 *
 * This callback is the second in the Application Framework's message processing
 * chain. At this point in the processing of incoming over-the-air messages, the
 * application has determined that the incoming message is a ZCL command. It
 * parses enough of the message to populate an EmberAfClusterCommand struct. The
 * Application Framework defines this struct value in a local scope to the
 * command processing but also makes it available through a global pointer
 * called emberAfCurrentCommand, in app/framework/util/util.c. When command
 * processing is complete, this pointer is cleared.
 *
 * @param cmd   Ver.: always
 */
boolean emberAfPreCommandReceivedCallback(EmberAfClusterCommand* cmd)
{
  return false;
}

/** @brief Main Tick
 *
 * Whenever main application tick is called, this callback will be called at the
 * end of the main tick execution.
 *
 */
void emberAfMainTickCallback(void)
{
}

/** @brief On/off Cluster Client Attribute Changed
 *
 * Client Attribute Changed
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 * @param attributeId Attribute that changed  Ver.: always
 */
void emberAfOnOffClusterClientAttributeChangedCallback(int8u endpoint,
                                                       EmberAfAttributeId attributeId)
{
}

/** @brief Reset To Factory Defaults
 *
 * This function is called by the Basic server plugin when a request to reset
 * to factory defaults is received. The plugin will reset attributes managed by
 * the framework to their default values. The application should perform any
 * other necessary reset-related operations in this callback, including
 * resetting any externally-stored attributes.
 *
 * @param endpoint   Ver.: always
 */
void emberAfPluginBasicResetToFactoryDefaultsCallback(uint8_t endpoint)
{
}

/** @brief Basic Cluster Server Attribute Changed
 *
 * Server Attribute Changed
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 * @param attributeId Attribute that changed  Ver.: always
 */
void emberAfBasicClusterServerAttributeChangedCallback(int8u endpoint,
                                                       EmberAfAttributeId attributeId)
{
}

/** @brief Basic Cluster Client Attribute Changed
 *
 * Client Attribute Changed
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 * @param attributeId Attribute that changed  Ver.: always
 */
void emberAfBasicClusterClientAttributeChangedCallback(int8u endpoint,
                                                       EmberAfAttributeId attributeId)
{
}

/** @brief Basic Cluster Server Manufacturer Specific Attribute Changed
 *
 * Server Manufacturer Specific Attribute Changed
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 * @param attributeId Attribute that changed  Ver.: always
 * @param manufacturerCode Manufacturer Code of the attribute that changed 
 * Ver.: always
 */
void emberAfBasicClusterServerManufacturerSpecificAttributeChangedCallback(int8u endpoint,
                                                                           EmberAfAttributeId attributeId,
                                                                           int16u manufacturerCode)
{
}

/** @brief Basic Cluster Client Manufacturer Specific Attribute Changed
 *
 * Client Manufacturer Specific Attribute Changed
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 * @param attributeId Attribute that changed  Ver.: always
 * @param manufacturerCode Manufacturer Code of the attribute that changed 
 * Ver.: always
 */
void emberAfBasicClusterClientManufacturerSpecificAttributeChangedCallback(int8u endpoint,
                                                                           EmberAfAttributeId attributeId,
                                                                           int16u manufacturerCode)
{
}

/** @brief Registration
 *
 * This callback is called when the device joins a network and the process of
 * registration is complete. This callback provides a success value of true if
 * the registration process was successful and a value of false if registration
 * failed.
 *
 * @param success true if registration succeeded, false otherwise.  Ver.: always
 */
void emberAfRegistrationCallback(boolean success)
{
}

/** @brief IAS ACE Cluster Get Panel Status
 *
 * 
 *
 */
boolean emberAfIasAceClusterGetPanelStatusCallback(void)
{
  return FALSE;
}

/** @brief IAS ACE Cluster Arm
 *
 * 
 *
 * @param armMode   Ver.: always
 * @param armDisarmCode   Ver.: since ha-1.2-05-3520-29
 * @param zoneId   Ver.: since ha-1.2-05-3520-29
 */
boolean emberAfIasAceClusterArmCallback(int8u armMode,
                                        int8u* armDisarmCode,
                                        int8u zoneId)
{
  return FALSE;
}

/** @brief IAS ACE Cluster Panic
 *
 * 
 *
 */
boolean emberAfIasAceClusterPanicCallback(void)
{
  return FALSE;
}

/** @brief IAS ACE Cluster Emergency
 *
 * 
 *
 */
boolean emberAfIasAceClusterEmergencyCallback(void)
{
  return FALSE;
}

boolean emberAfApplianceEventsAndAlertClusterAlertsNotificationCallback(uint8_t alertsCount,
                                                                        uint8_t* alertStructures)
{
    return FALSE;
}

boolean emberAfApplianceEventsAndAlertClusterEventsNotificationCallback(uint8_t eventHeader,
                                                                        uint8_t eventId)
{
    return FALSE;
}

boolean emberAfApplianceEventsAndAlertClusterGetAlertsResponseCallback(uint8_t alertsCount,
                                                                       uint8_t* alertStructures)
{
    return FALSE;
}

