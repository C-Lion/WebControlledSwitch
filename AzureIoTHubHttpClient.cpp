#include <stdlib.h>
#include <stdint.h>
#include <Arduino.h>
#include "AzureIoTHub.h"
#include "AzureIoTHubManager.h"

// Define the Model
//BEGIN_NAMESPACE(IoT);

//DECLARE_MODEL(SwitchController,
//WITH_DATA(ascii_char_ptr, DeviceId),
//WITH_DATA(int, Status),
//WITH_ACTION(Activate, ascii_char_ptr, logInfo),
//WITH_ACTION(TurnOn, ascii_char_ptr, logInfo),
//WITH_ACTION(TurnOff, ascii_char_ptr, logInfo)
//);

//END_NAMESPACE(IoT);

//EXECUTE_COMMAND_RESULT Activate(SwitchController* device, char *logInfo)
//{
//	(void)device;
//	LogInfo("Activate.\r\n");
//	if (logInfo != nullptr)
//		LogInfo("%s", logInfo);
//
//	AzureIoTHubManager::Instance()->OnActivate(logInfo);
//
//	return EXECUTE_COMMAND_SUCCESS;
//}


//EXECUTE_COMMAND_RESULT TurnOn(SwitchController* device, char *logInfo)
//{
//	(void)device;
//	LogInfo("TurnOn.\r\n");
//	if (logInfo != nullptr)
//		LogInfo("%s", logInfo);
//	AzureIoTHubManager::Instance()->OnTurnOn(logInfo);
//	return EXECUTE_COMMAND_SUCCESS;
//}
//
//EXECUTE_COMMAND_RESULT TurnOff(SwitchController* device, char *logInfo)
//{
//	(void)device;
//	LogInfo("TurnOff.\r\n");
//	if (logInfo != nullptr)
//		LogInfo("%s", logInfo);
//	AzureIoTHubManager::Instance()->OnTurnOff(logInfo);
//	return EXECUTE_COMMAND_SUCCESS;
//}

void sendCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
	int messageTrackingId = reinterpret_cast<intptr_t>(userContextCallback);

	LogInfo("Message Id: %d Received.\r\n", messageTrackingId);

	LogInfo("Result Call Back Called! Result is: %s \r\n", ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
}

//static void sendMessage(IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle, const unsigned char* buffer, size_t size)
//{
//	static unsigned int messageTrackingId;
//	IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(buffer, size);
//	if (messageHandle == nullptr)
//	{
//		LogInfo("unable to create a new IoTHubMessage\r\n");
//	}
//	else
//	{
//		if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, sendCallback,
//			reinterpret_cast<void*>(static_cast<uintptr_t>(messageTrackingId))) != IOTHUB_CLIENT_OK)
//		{
//			LogInfo("failed to hand over the message to IoTHubClient");
//		}
//		else
//		{
//			LogInfo("IoTHubClient accepted the message for delivery\r\n");
//		}
//		IoTHubMessage_Destroy(messageHandle);
//	}
//	free(const_cast<unsigned char*>(buffer));
//	messageTrackingId++;
//}

/*this function "links" IoTHub to the serialization library*/
static IOTHUBMESSAGE_DISPOSITION_RESULT IoTHubMessage(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback)
{
	LogInfo("IoTHubMessage\n");
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
		Serial.printf("Message arrived:\n");
		for(int i = 0; i < size; ++i)
		{
			Serial.printf("%c", buffer[i]);
		}

		///*buffer is not zero terminated*/
		//char* temp = static_cast<char *>(malloc(size + 1));
		//if (temp == nullptr)
		//{
		//	LogInfo("failed to malloc\r\n");
		//	result = IOTHUBMESSAGE_REJECTED;
		//}
		//else
		//{
		//	memcpy(temp, buffer, size);
		//	temp[size] = '\0';
		//	EXECUTE_COMMAND_RESULT executeCommandResult = EXECUTE_COMMAND(userContextCallback, temp);
		//	result =
		//		(executeCommandResult == EXECUTE_COMMAND_ERROR) ? IOTHUBMESSAGE_ABANDONED :
		//		(executeCommandResult == EXECUTE_COMMAND_SUCCESS) ? IOTHUBMESSAGE_ACCEPTED :
		//		IOTHUBMESSAGE_REJECTED;
		//	free(temp);
		//}
	}
return IOTHUBMESSAGE_ACCEPTED;
	//return result;
}

IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;
SwitchController* switchController;

extern "C" bool AzureIoTHubInit(const char *connectionString)
{
	Serial.printf("Connection string: %s\n", connectionString);
	//int error = serializer_init(nullptr);
	//if (error != SERIALIZER_OK && error!= SERIALIZER_ALREADY_INIT && error != SERIALIZER_ALREADY_STARTED)
	//{
	//	LogInfo("Failed on serializer_init %d\n", error);
	//	return false;
	//}
	
	iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, HTTP_Protocol);
	srand(static_cast<unsigned int>(time(nullptr)));

	if (iotHubClientHandle == nullptr)
	{
		LogInfo("Failed on IoTHubClient_LL_Create\r\n");
		return false;
	}

	

	unsigned int minimumPollingTime = 3; /*because it can poll "after 9 seconds" polls will happen effectively at 4 seconds*/
	if (IoTHubClient_LL_SetOption(iotHubClientHandle, "MinimumPollingTime", &minimumPollingTime) != IOTHUB_CLIENT_OK)
	{
		LogInfo("failure to set option \"MinimumPollingTime\"\r\n");
		return false;
	}

//	switchController = CREATE_MODEL_INSTANCE(IoT, SwitchController);
	/*if (switchController == nullptr)
	{
		LogInfo("Failed on CREATE_MODEL_INSTANCE\r\n");
		return false;
	}
*/
	if (IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, IoTHubMessage, switchController) != IOTHUB_CLIENT_OK)
	{
		LogInfo("unable to IoTHubClient_SetMessageCallback\r\n");
		return false;
	}
	LogInfo("IoTHubClient initiated\r\n");
	return true;
}

extern "C" bool AzureIoTHubSendMessage(char *deviceId, int status, int messageId)
{
	//switchController->DeviceId = deviceId;
	//switchController->Status = status;
	bool result = false;

	unsigned char* destination;
	size_t destinationSize;
	//if (SERIALIZE(&destination, &destinationSize, switchController->DeviceId/*, switchController->Status*/) != CODEFIRST_OK)
	//{
	//	LogInfo("Failed to serialize\r\n");
	//}

	IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromByteArray(destination, destinationSize);
	if (messageHandle == nullptr)
	{
		LogInfo("unable to create a new IoTHubMessage\r\n");
	}
	else
	{
		if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, sendCallback, reinterpret_cast<void*>(messageId)) != IOTHUB_CLIENT_OK)
		{
			LogInfo("failed to hand over the message to IoTHubClient");
		}
		else
		{
			LogInfo("IoTHubClient accepted the message for delivery\r\n");
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
