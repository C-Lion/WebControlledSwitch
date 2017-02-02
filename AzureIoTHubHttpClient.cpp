#include <stdlib.h>
#include <stdint.h>
#include <Arduino.h>
#include "AzureIoTHub.h"
#include "AzureIoTHubManager.h"

// Define the Model
BEGIN_NAMESPACE(IoT);

DECLARE_MODEL(SwitchController,
WITH_DATA(ascii_char_ptr, DeviceId),
WITH_DATA(int, Status),
WITH_ACTION(Command, int, commandid, ascii_char_ptr, commandName, ascii_char_ptr, logInfo)
);

END_NAMESPACE(IoT);

DEFINE_ENUM_STRINGS(IOTHUB_CLIENT_CONFIRMATION_RESULT, IOTHUB_CLIENT_CONFIRMATION_RESULT_VALUES)

EXECUTE_COMMAND_RESULT Command(SwitchController* device, int commandId, char *commandName, char *logInfo)
{
	(void)device;
	Serial.write("Activate.\n");
	if (logInfo != nullptr)
		Serial.write(logInfo);

	AzureIoTHubManager::Instance()->HandleCommand(commandName, commandId, logInfo);

	return EXECUTE_COMMAND_SUCCESS;
}

void sendCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
	int messageTrackingId = reinterpret_cast<intptr_t>(userContextCallback);
	Serial.printf("Message Id: %d Received.\n", messageTrackingId);
}

static void sendMessage(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const unsigned char* buffer, size_t size)
{
	static unsigned int messageTrackingId;
	IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(buffer, size);
	if (messageHandle == nullptr)
	{
		Serial.write("unable to create a new IoTHubMessage\n");
	}
	else
	{
		if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, sendCallback,
			reinterpret_cast<void*>(static_cast<uintptr_t>(messageTrackingId))) != IOTHUB_CLIENT_OK)
		{
			Serial.write("failed to hand over the message to IoTHubClient");
		}
		else
		{
			Serial.write("IoTHubClient accepted the message for delivery\n");
		}
		IoTHubMessage_Destroy(messageHandle);
	}
	free(const_cast<unsigned char*>(buffer));
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
		Serial.write("unable to IoTHubMessage_GetByteArray");
		result = IOTHUBMESSAGE_REJECTED;
	}
	else
	{
		/*buffer is not zero terminated*/
		char* temp = static_cast<char *>(malloc(size + 1));
		if (temp == nullptr)
		{
			Serial.write("failed to malloc\r\n");
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
SwitchController* switchController;

extern "C" bool AzureIoTHubInit(const char *connectionString)
{
	if (serializer_init(nullptr) != SERIALIZER_OK)
	{
		Serial.write("Failed on serializer_init\n");
		return false;
	}

	iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, HTTP_Protocol);
	srand(static_cast<unsigned int>(time(nullptr)));

	if (iotHubClientHandle == nullptr)
	{
		Serial.write("Failed on IoTHubClient_LL_Create\n");
		return false;
	}
	unsigned int minimumPollingTime = 3; /*because it can poll "after 9 seconds" polls will happen effectively at 4 seconds*/
	if (IoTHubClient_LL_SetOption(iotHubClientHandle, "MinimumPollingTime", &minimumPollingTime) != IOTHUB_CLIENT_OK)
	{
		Serial.write("failure to set option \"MinimumPollingTime\"\n");
		return false;
	}

	switchController = CREATE_MODEL_INSTANCE(IoT, SwitchController);
	if (switchController == nullptr)
	{
		Serial.write("Failed on CREATE_MODEL_INSTANCE\n");
		return false;
	}

	if (IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, IoTHubMessage, switchController) != IOTHUB_CLIENT_OK)
	{
		Serial.write("unable to IoTHubClient_SetMessageCallback\n");
		return false;
	}
	return true;
}

extern "C" bool AzureIoTHubSendMessage(char *deviceId, int status, int messageId)
{
	switchController->DeviceId = deviceId;
	switchController->Status = status;
	bool result = false;

	unsigned char* destination;
	size_t destinationSize;
	if (SERIALIZE(&destination, &destinationSize, switchController->DeviceId, switchController->Status) != CODEFIRST_OK)
	{
		Serial.write("Failed to serialize\n");
	}

	IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(destination, destinationSize);
	if (messageHandle == nullptr)
	{
		Serial.write("unable to create a new IoTHubMessage\n");
	}
	else
	{
		if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, sendCallback, reinterpret_cast<void*>(messageId)) != IOTHUB_CLIENT_OK)
		{
			Serial.write("failed to hand over the message to IoTHubClient");
		}
		else
		{
			Serial.write("IoTHubClient accepted the message for delivery\n");
			result = true;
		}

		IoTHubMessage_Destroy(messageHandle);
	}
	free(destination);
	return result;
}


extern "C" void AzureIoTHubLoop(void)
{
	Serial.write(".");
	IoTHubClient_LL_DoWork(iotHubClientHandle);
}

