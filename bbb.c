/*******************************************************************************
 * Copyright (c) 2012, 2017 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution. 
 *
 * The Eclipse Public License is available at 
 *   http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at 
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial contribution
 *******************************************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parson.h"
#include "MQTTClient.h"
#include "embedded_linux.h"
 
#define ADDRESS         "tcp://localhost:1883"
#define CLIENTID        "SALA"
#define LIGHT_SENSOR    "bloco01/sala01/sensor/01/LDR"
#define THERMAL_SENSOR  "bloco01/sala01/sensor/02/2N3409"
#define LIGHT           "bloco01/sala01/light/01"
#define SENSORES1       "bloco01/sala01/sensor/ALL"
#define AC              "bloco01/sala01/AC"
#define CONTROLADOR     "CONTROLADOR/CONTROLADOR_01"
#define QOS             1
#define TIMEOUT         10000L
#define ON              1
#define OFF             0
 
volatile MQTTClient_deliveryToken deliveredtoken;
MQTTClient_message pubmsg = MQTTClient_message_initializer;
MQTTClient_deliveryToken token;
MQTTClient client;
int rc;

void delivered(void *context, MQTTClient_deliveryToken dt){
    //printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}
 
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message){
    int i, value, temp, j;
    bool ex = true;
    char* msg = message->payload;
    char msg_back[100] = "\0";
    FILE *fptr;
    char c;

    //printf("topic: %s\n", topicName);
    //printf("message: %s\n", msg);

    bool state_light01;
    bool state_ac;

    printf("%s\n", msg);
    
    if(get_value(GPIO_LED01)){
        state_light01 = true;
    }

    if(!get_value(GPIO_LED01)){
        state_light01 = false;
    }

    if(get_value(GPIO_LEDAC)){
        state_ac = true;
    }

    if(!get_value(GPIO_LEDAC)){
        state_ac = false;
    }


    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    char *serialized_string = NULL;

    printf("Retorno do JSON para o tópico: %s\n", topicName);
    
    if ((strcmp (topicName, "bloco01/sala01/light/01") == 0) && (strcmp(msg, "state") == 0)){
        json_object_dotset_boolean(root_object, "Luz.Estado", state_light01);
        json_serialize_to_file_pretty(root_value, "STATE_LIGHT.json");
        serialized_string = json_serialize_to_string_pretty(root_value);

        pubmsg.payload = serialized_string;
        pubmsg.payloadlen = strlen(serialized_string);
        pubmsg.qos = QOS;
        pubmsg.retained = 0;
        MQTTClient_publishMessage(client, CONTROLADOR, &pubmsg, &token);

    } else if((strcmp (topicName, "bloco01/sala01/AC") == 0) && (strcmp(msg, "state") == 0)){
        json_object_dotset_boolean(root_object, "Ar condicionado.Estado", state_ac);
        json_serialize_to_file_pretty(root_value, "STATE_AC.json");
        serialized_string = json_serialize_to_string_pretty(root_value);

        pubmsg.payload = serialized_string;
        pubmsg.payloadlen = strlen(serialized_string);
        pubmsg.qos = QOS;
        pubmsg.retained = 0;
        MQTTClient_publishMessage(client, CONTROLADOR, &pubmsg, &token);

    } else if(strcmp (topicName, "bloco01/sala01/sensor/01/LDR") == 0){
        value = get_value_ain(0);
        //int value = 120; //comenta e substitui pelo get_value
        json_object_dotset_number(root_object, "Sensor de Luminosidade-LDR.Valor", value);
        json_serialize_to_file_pretty(root_value, "LIGHT_SENSOR.json");
        serialized_string = json_serialize_to_string_pretty(root_value);

        pubmsg.payload = serialized_string;
        pubmsg.payloadlen = strlen(serialized_string);
        pubmsg.qos = QOS;
        pubmsg.retained = 0;
        MQTTClient_publishMessage(client, CONTROLADOR, &pubmsg, &token);

    } else if(strcmp (topicName, "bloco01/sala01/sensor/ALL") == 0){
        value = get_value_ain(0); //Coloca temperatura aqui
        temp = get_value_ain(2); //Coloca temperatura aqui
        temp = (temp*300) >> 12;
        //int value = 1;   //comenta e substitui pelo get_value
        //int temp = 2;      //comenta e substitui pela função de captura de temperatura
        json_object_dotset_number(root_object, "Sensor de Luminosidade-LDR.Valor", value);
        json_object_dotset_number(root_object, "Sensor de Temperatura-2N3409.Valor em ºC", temp);
        json_serialize_to_file_pretty(root_value, "SENSORES1.json");
        serialized_string = json_serialize_to_string_pretty(root_value);

        pubmsg.payload = serialized_string;
        pubmsg.payloadlen = strlen(serialized_string);
        pubmsg.qos = QOS;
        pubmsg.retained = 0;
        MQTTClient_publishMessage(client, CONTROLADOR, &pubmsg, &token);
    

    } else if(strcmp (topicName, "bloco01/sala01/sensor/02/2N3409") == 0){
        temp = get_value_ain(2); //Coloca temperatura aqui
        temp = (temp*300) >> 12;
        //int temp2 = 37; //comenta e substitui pela função de captura de temperatura
        json_object_dotset_number(root_object, "Sensor de Temperatura-2N3409.Valor em ºC", temp);
        json_serialize_to_file_pretty(root_value, "THERMAL_SENSOR.json");
        serialized_string = json_serialize_to_string_pretty(root_value);

        pubmsg.payload = serialized_string;
        pubmsg.payloadlen = strlen(serialized_string);
        pubmsg.qos = QOS;
        pubmsg.retained = 0;
        MQTTClient_publishMessage(client, CONTROLADOR, &pubmsg, &token);
    }

    serialized_string = json_serialize_to_string_pretty(root_value);
    puts(serialized_string);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);

    if(!strcmp(topicName, LIGHT)){
        if(!strcmp(msg, "on")){
            //código de ligar luz
            set_gpio_high(GPIO_LED01);

            strcpy(msg_back, "Luz Ligada!");
            pubmsg.payload = msg_back;
            pubmsg.payloadlen = strlen(msg_back);
            pubmsg.qos = QOS;
            pubmsg.retained = 0;
            MQTTClient_publishMessage(client, CONTROLADOR, &pubmsg, &token);
            //rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
        }
        else if(!strcmp(msg, "off")){
            //código de desligar luz
            set_gpio_low(GPIO_LED01);

            strcpy(msg_back, "Luz Desligada!");
            pubmsg.payload = msg_back;
            pubmsg.payloadlen = strlen(msg_back);
            pubmsg.qos = QOS;
            pubmsg.retained = 0;
            MQTTClient_publishMessage(client, CONTROLADOR, &pubmsg, &token);
            //rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
        }
    }
    if(!strcmp(topicName, AC)){
        if(!strcmp(msg, "on")){
            //código de ligar AC
            set_gpio_high(GPIO_LEDAC);

            strcpy(msg_back, "AC Ligado!");
            pubmsg.payload = msg_back;
            pubmsg.payloadlen = strlen(msg_back);
            pubmsg.qos = QOS;
            pubmsg.retained = 0;
            MQTTClient_publishMessage(client, CONTROLADOR, &pubmsg, &token);
            //rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
        }
        else if(!strcmp(msg, "off")){
            //código de desligar AC
            set_gpio_low(GPIO_LEDAC);

            strcpy(msg_back, "AC Desligado!");
            pubmsg.payload = msg_back;
            pubmsg.payloadlen = strlen(msg_back);
            pubmsg.qos = QOS;
            pubmsg.retained = 0;
            MQTTClient_publishMessage(client, CONTROLADOR, &pubmsg, &token);
            //rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
        }
    }   

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}
 
void connlost(void *context, char *cause){
    printf("\nConnection lost\n");
    printf("cause: %s\n", cause);
}
 
int main(int argc, char* argv[]){

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int ch;
 
    MQTTClient_create(&client, ADDRESS, CLIENTID,
    MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    init_pins();
 
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
 
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS){
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    
    MQTTClient_subscribe(client, THERMAL_SENSOR, QOS);
    MQTTClient_subscribe(client, SENSORES1, QOS);
    MQTTClient_subscribe(client, LIGHT_SENSOR, QOS);
    MQTTClient_subscribe(client, LIGHT, QOS);
    MQTTClient_subscribe(client, AC, QOS);
 
    while(1);
    
    MQTTClient_unsubscribe(client, THERMAL_SENSOR);
    MQTTClient_unsubscribe(client, LIGHT_SENSOR);
    MQTTClient_unsubscribe(client, LIGHT);
    MQTTClient_unsubscribe(client, SENSORES1);
    MQTTClient_unsubscribe(client, AC);
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}