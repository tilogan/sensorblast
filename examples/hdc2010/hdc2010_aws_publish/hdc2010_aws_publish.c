/*
 * Copyright (C) 2019 Shintako LLC
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

/* Standard Includes */
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <syslog.h>
#include <fcntl.h>
#include <time.h>

/* Sensor Blast Includes*/
#include "sb_codes.h"
#include "hdc2010.h"

/* AWS Includes */
#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"

/* Parameters for code example */
#define I2C_PATH "/dev/i2c-1"
#define CERTS_DIR "../../../../aws-iot-device-sdk-embedded-C/certs/"
#define PUBLISH_TOPC "hdc2010/data"
#define JSON_TEMPLATE "{\"temperature\":%f, \"humidity\":%f }"
#define PAYLOAD_MAX 100

/* Configuration Structures */
extern HDC2010_Config hdcConfig;
extern HDC2010_MeasurementConfig hdcMeasureConfig;
extern HDC2010_InterruptConfig hdcInterruptConfig;

void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data)
{
    IOT_WARN("MQTT Disconnect");
    IoT_Error_t rc = FAILURE;

    if(pClient == NULL)
    {
        return;
    }

    if(aws_iot_is_autoreconnect_enabled(pClient))
    {
        IOT_INFO("Auto Reconnect is enabled, Reconnecting attempt will start now");
    }
    else
    {
        IOT_WARN("Auto Reconnect not enabled. Starting manual reconnect...");
        rc = aws_iot_mqtt_attempt_reconnect(pClient);
        if(rc == NETWORK_RECONNECTED)
        {
            IOT_WARN("Manual Reconnect Successful");
        } else {
            IOT_WARN("Manual Reconnect Failed - %d", rc);
        }
    }
}

int main(void)
{
    HDC2010_Measurement curMeasure;
    HDC2010_Interface hdcInf;
    int32_t status;
    time_t logTime;
    char payload[PAYLOAD_MAX];
    char* curTime;
    char rootCA[PATH_MAX + 1];
    char clientCRT[PATH_MAX + 1];
    char clientKey[PATH_MAX + 1];
    char CurrentWD[PATH_MAX + 1];


    /* AWS Parameters */
    AWS_IoT_Client client;
    IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;
    IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;
    IoT_Publish_Message_Params paramsQOS0;
    IoT_Error_t rc = FAILURE;

    /* Parsing all of the certificate files */
    getcwd(CurrentWD, sizeof(CurrentWD));
    snprintf(rootCA, PATH_MAX + 1, "%s/%s/%s", CurrentWD, CERTS_DIR,
             AWS_IOT_ROOT_CA_FILENAME);
    snprintf(clientCRT, PATH_MAX + 1, "%s/%s/%s", CurrentWD, CERTS_DIR,
            AWS_IOT_CERTIFICATE_FILENAME);
    snprintf(clientKey, PATH_MAX + 1, "%s/%s/%s", CurrentWD, CERTS_DIR,
            AWS_IOT_PRIVATE_KEY_FILENAME);

    printf("\n---Sensor Blast - AWS MQTT Publish---\n");

    /* Initializing the MQTT parameters */
    mqttInitParams.enableAutoReconnect = true;
    mqttInitParams.pHostURL = AWS_IOT_MQTT_HOST;
    mqttInitParams.port = AWS_IOT_MQTT_PORT;
    mqttInitParams.pRootCALocation = rootCA;
    mqttInitParams.pDeviceCertLocation = clientCRT;
    mqttInitParams.pDevicePrivateKeyLocation = clientKey;
    mqttInitParams.mqttCommandTimeout_ms = 20000;
    mqttInitParams.tlsHandshakeTimeout_ms = 5000;
    mqttInitParams.isSSLHostnameVerify = true;
    mqttInitParams.disconnectHandler = disconnectCallbackHandler;
    mqttInitParams.disconnectHandlerData = NULL;

    rc = aws_iot_mqtt_init(&client, &mqttInitParams);
    if(rc != SUCCESS)
    {
        IOT_ERROR("aws_iot_mqtt_init returned error : %d ", rc);
        return (rc);
    }

    /* Connecting to AWS */
    connectParams.keepAliveIntervalInSec = 600;
    connectParams.isCleanSession = true;
    connectParams.MQTTVersion = MQTT_3_1_1;
    connectParams.pClientID = AWS_IOT_MQTT_CLIENT_ID;
    connectParams.clientIDLen = (uint16_t) strlen(AWS_IOT_MQTT_CLIENT_ID);
    connectParams.isWillMsgPresent = false;

    rc = aws_iot_mqtt_connect(&client, &connectParams);
    if(SUCCESS != rc)
    {
        IOT_ERROR("Error(%d) connecting to %s:%d", rc, mqttInitParams.pHostURL,
                    mqttInitParams.port);
        return (rc);
    }

    /* Initializing the payload objects */
    paramsQOS0.qos = QOS0;
    paramsQOS0.payload = (void *) payload;
    paramsQOS0.isRetained = 0;

    /* Open the I2C Bus */
    hdcInf.i2cHandle = open(I2C_PATH, O_RDWR);
    if (hdcInf.i2cHandle  < 0)
    {
        printf("\nERROR: Could not open I2C driver. Try running as root?\n");
        return (-1);
    }

    /* Opening the driver */
    HDC2010_openDriver(&hdcInf, &hdcConfig);

    /* Setting the interrupt config (no interrupts, only polling  */
    HDC2010_setInterruptConfig(&hdcInf, &hdcInterruptConfig);

    while(1)
    {

        /* Getting the measurement */
        status = HDC2010_getManualMeasurement(&hdcInf, &hdcMeasureConfig,
                                               &curMeasure);

        if(status == SENSOR_BLAST_OK)
        {
            printf("--- Sensor Reading ---\n");
            printf("Temperature Reading %f\n", curMeasure.temperature);
            printf("Humidity Reading %f\n", curMeasure.humidity);

            /* Throwing the results into a formatting JSON string */
            snprintf(payload, PAYLOAD_MAX, JSON_TEMPLATE,
                     curMeasure.temperature, curMeasure.humidity);

            /* Calculating the payload length */
            paramsQOS0.payloadLen = strlen(payload);


            rc = aws_iot_mqtt_publish(&client, PUBLISH_TOPC, 12, &paramsQOS0);

            if(rc == SUCCESS)
            {
                printf("Reading published to AWS IOT Cloud!\n");
                printf("Raw JSON: %s\n", payload);
            }
            else
            {
                printf("ERROR: Could not send data reading to cloud!");
                break;
            }
        }
        else
        {
            printf("ERROR: Could not read HDC2010! (err 0x%x)\n", status);
            return status;
        }

        /* Sleeping for a second */
        sleep(1);
    }

    /* Flush out the buffer */
    aws_iot_mqtt_yield(&client, 100);
}
