// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include <Arduino.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
	
#include "AzureIoTHub.h"
#include "iot_logging.h"
#include "Configuration.h"
#include "AzureIoTHubHttpClient.h"
#include "AzureIoTHubManager.h"

//TODO: Send notification only when the relay changes its state and not with interval

BEGIN_NAMESPACE(GateDevice);
DECLARE_MODEL(GateController,
WITH_DATA(ascii_char_ptr, DeviceId),
	WITH_DATA(int, Status),
	WITH_ACTION(Activate, ascii_char_ptr, logInfo),
	WITH_ACTION(TurnOn, ascii_char_ptr, logInfo),
	WITH_ACTION(TurnOff, ascii_char_ptr, logInfo));
END_NAMESPACE(GateDevice);

DEFINE_ENUM_STRINGS(IOTHUB_CLIENT_CONFIRMATION_RESULT, IOTHUB_CLIENT_CONFIRMATION_RESULT_VALUES)


EXECUTE_COMMAND_RESULT Activate(GateController* device, char *logInfo)
{
	AzureIoTHubManager::Instance()->OnActivate(logInfo);

	return EXECUTE_COMMAND_SUCCESS;
}

EXECUTE_COMMAND_RESULT TurnOn(GateController* device, char *logInfo)
{
	AzureIoTHubManager::Instance()->OnTurnOn(logInfo);

	return EXECUTE_COMMAND_SUCCESS;
}

EXECUTE_COMMAND_RESULT TurnOff(GateController* device, char *logInfo)
{
	AzureIoTHubManager::Instance()->OnTurnOff(logInfo);

	return EXECUTE_COMMAND_SUCCESS;
}

void sendCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
	int messageTrackingId = (intptr_t)userContextCallback;

	LogInfo("Message Id: %d Received.\r\n", messageTrackingId);

	LogInfo("Result Call Back Called! Result is: %s \r\n", ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
}

static void sendMessage(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const unsigned char* buffer, size_t size)
{
	static unsigned int messageTrackingId;
	IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(buffer, size);
	if (messageHandle == NULL)
	{
		LogInfo("unable to create a new IoTHubMessage\r\n");
	}
	else
	{
		if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, sendCallback, (void*)(uintptr_t)messageTrackingId) != IOTHUB_CLIENT_OK)
		{
			LogInfo("failed to hand over the message to IoTHubClient");
		}
		else
		{
			LogInfo("IoTHubClient accepted the message for delivery\r\n");
		}
		IoTHubMessage_Destroy(messageHandle);
	}
	free((void*)buffer);
	messageTrackingId++;
}

/*this function "links" IoTHub to the serialization library*/
static IOTHUBMESSAGE_DISPOSITION_RESULT IoTHubMessage(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
	IOTHUBMESSAGE_DISPOSITION_RESULT result;
	const unsigned char* buffer;
	size_t size;
	if (IoTHubMessage_GetByteArray(message, &buffer, &size) != IOTHUB_MESSAGE_OK)
	{
		LogInfo("unable to IoTHubMessage_GetByteArray\r\n");
		result = IOTHUBMESSAGE_REJECTED;
	}
	else
	{
		/*buffer is not zero terminated*/
		char* temp = (char *)malloc(size + 1);
		if (temp == NULL)
		{
			LogInfo("failed to malloc\r\n");
			result = IOTHUBMESSAGE_REJECTED;
		}
		else
		{
			memcpy(temp, buffer, size);
			temp[size] = '\0';
			EXECUTE_COMMAND_RESULT executeCommandResult = EXECUTE_COMMAND(userContextCallback, temp);
			result =
				(executeCommandResult == EXECUTE_COMMAND_ERROR) ? IOTHUBMESSAGE_ABANDONED :
				(executeCommandResult == EXECUTE_COMMAND_SUCCESS) ? IOTHUBMESSAGE_ACCEPTED :
				IOTHUBMESSAGE_REJECTED;
			free(temp);
		}
	}
	return result;
}

IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;
GateController* gateController;
int startInterval;

bool InitAzureIoTHubClient(void)
{
	startInterval = millis();

	if (serializer_init(nullptr) != SERIALIZER_OK)
	{
		LogInfo("Failed on serializer_init\r\n");
		return false;
	}
	//else
	iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(azureIoTHubDeviceConnectionString, HTTP_Protocol);
	srand(static_cast<unsigned int>(time(nullptr)));

	if (iotHubClientHandle == nullptr)
	{
		LogInfo("Failed on IoTHubClient_LL_Create\r\n");
		return false;
	}
	//else

	unsigned int minimumPollingTime = 1;
	if (IoTHubClient_LL_SetOption(iotHubClientHandle, "MinimumPollingTime", &minimumPollingTime) != IOTHUB_CLIENT_OK)
	{
		LogInfo("failure to set option \"MinimumPollingTime\"\r\n");
		return false;
	}

	GateController* gateController = CREATE_MODEL_INSTANCE(GateDevice, GateController);
	if (gateController == nullptr)
	{
		LogInfo("Failed on CREATE_MODEL_INSTANCE\r\n");
		return false;
	}
	//else
	return true;
}



bool AzureIoTHubClientLoop(int deviceStatus)
{
	if ((millis() - startInterval) > 10000) //every 10 seconds
	{
		startInterval = millis();

		if (IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, IoTHubMessage, gateController) != IOTHUB_CLIENT_OK)
		{
			LogInfo("unable to IoTHubClient_SetMessageCallback\r\n");
			return false;
		}
		//else
		gateController->DeviceId = "GateDevice";
		gateController->Status = deviceStatus;
		unsigned char* destination;
		size_t destinationSize;

		if (SERIALIZE(&destination, &destinationSize, gateController->DeviceId, gateController->Status) != IOT_AGENT_OK)
		{
			LogInfo("Failed to serialize\r\n");
			return false;
		}
		//else
		IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(destination, destinationSize);
		if (messageHandle == nullptr)
		{
			LogInfo("unable to create a new IoTHubMessage\r\n");
			return false;
		}
		//else
		if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, sendCallback, (void*)1) != IOTHUB_CLIENT_OK)
		{
			LogInfo("failed to hand over the message to IoTHubClient");
			return false;
		}
		//else
		LogInfo("IoTHubClient accepted the message for delivery\r\n");
		IoTHubMessage_Destroy(messageHandle);
		free(destination);
	}
		
	IoTHubClient_LL_DoWork(iotHubClientHandle);
}

void Destroy()
{
	DESTROY_MODEL_INSTANCE(gateController);
	IoTHubClient_LL_Destroy(iotHubClientHandle);
	serializer_deinit();
}
