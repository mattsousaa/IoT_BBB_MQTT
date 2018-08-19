/*******************************************************************************
 * Copyright (c) 2012, 2017 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution. 
 *
 * The Eclipse Public License is available at 
 *  0 http://www.eclipse.org/legal/epl-v10.html
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
 
#define ADDRESS         "tcp://192.168.31.44:1883"
#define CLIENTID        "CENTRAL_DE_CONTROLE"
#define CONTROLADOR     "CONTROLADOR/CONTROLADOR_01"
#define SENSORES1       "bloco01/sala01/sensor/ALL"
#define LIGHT_SENSOR    "bloco01/sala01/sensor/01/LDR"
#define THERMAL_SENSOR  "bloco01/sala01/sensor/02/2N3409"
#define LIGHT           "bloco01/sala01/light/01"
#define AC              "bloco01/sala01/AC"
#define QOS             1
#define TIMEOUT         10000L
 
volatile MQTTClient_deliveryToken deliveredtoken;

bool state_light01 = false; // Digamos que esteja apagada
bool state_light02 = true; // Digamos que esteja ligada
bool state_light03 = true; // Digamos que esteja ligada
int sensor_ldr = 23;
int sensor2n = 53;

char *serialized_string = NULL;

void inicialization_system(){

JSON_Value *root_value = json_value_init_object();
JSON_Object *root_object = json_value_get_object(root_value);

char *serialized_string = NULL;

json_object_dotset_value(root_object, "Trabalho Final de Sistemas Distribuídos.Professor: Márcio Espíndola Freire Maia [1].Dupla: Emanuel Angelim Pereira [2] e Mateus Sousa Araújo [3].emails", 
                        json_parse_string("[\"[1] marcioefmaia@gmail.com\",\"[2] emanuel_ec@alu.ufc.br\",\"[3] mateuseng_ec@alu.ufc.br\"]"));

serialized_string = json_serialize_to_string_pretty(root_value);
puts(serialized_string);
json_free_serialized_string(serialized_string);
json_value_free(root_value);

printf("\n");

}
 
void delivered(void *context, MQTTClient_deliveryToken dt){
    //printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}
 
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message){
    int i;
    char* msg = message->payload;
 
    printf("Sala: %s\n", msg);
 
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}
 
void connlost(void *context, char *cause){
    printf("\nConnection lost\n");
    printf("cause: %s\n", cause);
}
 
int main(int argc, char* argv[]){

    inicialization_system();

    
    FILE *fptr1;
    FILE *fptr2;
    FILE *fptr3;
    FILE *fptr4;
    FILE *fptr5;
    char c1;
    char c2;
    char c3;
    char c4;
    char c5;

    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;
    int ch;
 
    MQTTClient_create(&client, ADDRESS, CLIENTID,
    MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
 
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
 
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS){
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
 
    MQTTClient_subscribe(client, CONTROLADOR, QOS);
 
    char msg[10];
    int op;
    int op2;
    int op3;

    // Criação dos objetos JSON para serialização de dados

    JSON_Value *root_value1 = json_value_init_object();
    JSON_Object *root_object1 = json_value_get_object(root_value1);

    JSON_Value *root_value2 = json_value_init_object();
    JSON_Object *root_object2 = json_value_get_object(root_value2);

    JSON_Value *root_value3 = json_value_init_object();
    JSON_Object *root_object3 = json_value_get_object(root_value3);

    char *serialized_string = NULL;

    while(1){

        printf("Tópicos disponíveis:\n\n[1] %s\n[2] %s\n[3] %s\n[4] %s\n[5] %s\n", SENSORES1, LIGHT_SENSOR, THERMAL_SENSOR, LIGHT, AC);

        printf("Selecione o tópico desejado: \n");
        scanf("%d", &op2);

        switch(op2){

            case 1:

                system("/usr/bin/clear");

                strcpy(msg, "all");
                pubmsg.payload = msg;
                pubmsg.payloadlen = strlen(msg);
                pubmsg.qos = QOS;
                pubmsg.retained = 0;
                MQTTClient_publishMessage(client, SENSORES1, &pubmsg, &token);
                rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);

                /*json_object_set_string(root_object1, "ID_01", "LDR");
                json_object_set_string(root_object1, "Tipo_01", "Sensor de Luminosidade");
                json_object_set_number(root_object1, "Valor em Lm", sensor_ldr);
                //json_object_set_number(root_object, "Tipo", 25);

                json_object_set_string(root_object1, "ID_02", "2N3409");
                json_object_set_string(root_object1, "Tipo_02", "Sensor de Temperatura");
                json_object_set_number(root_object1, "Valor em ºC", sensor2n);

                json_serialize_to_file_pretty(root_value1, "SENSORES1.json");

                serialized_string = json_serialize_to_string_pretty(root_value1);
                puts(serialized_string);
                json_free_serialized_string(serialized_string);
                //json_value_free(root_value5);*/


                // printf("Retorno do JSON para o tópico: %s\n", SENSORES1);

                // fptr4 = fopen("SENSORES1.json", "r");

                // if (fptr4 == NULL){
                //     printf("Cannot open file \n");
                //     exit(0);
                // }
 
                // // Read contents from file
                // c4 = fgetc(fptr4);

                // while (c4 != EOF){
                //     printf ("%c", c4);
                //     c4 = fgetc(fptr4);
                // }
 
                // fclose(fptr4);

                // printf("\n");

            break;

            case 2:

                system("/usr/bin/clear");

                strcpy(msg, "ldr");
                pubmsg.payload = msg;
                pubmsg.payloadlen = strlen(msg);
                pubmsg.qos = QOS;
                pubmsg.retained = 0;
                MQTTClient_publishMessage(client, LIGHT_SENSOR, &pubmsg, &token);
                rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);

                //printf("Retorno do JSON para o tópico: %s\n", LIGHT_SENSOR);

                //fptr3 = fopen("LIGHT_SENSOR.json", "r");

                // if (fptr3 == NULL){
                //     printf("Cannot open file \n");
                //     exit(0);
                // }
 
                //     // Read contents from file
                // c3 = fgetc(fptr3);

                // while (c3 != EOF){
                //     printf ("%c", c3);
                //     c3 = fgetc(fptr3);
                // }
 
                // fclose(fptr3);

                // printf("\n");

            break;

            case 3:

                system("/usr/bin/clear");

                strcpy(msg, "2N3409");
                pubmsg.payload = msg;
                pubmsg.payloadlen = strlen(msg);
                pubmsg.qos = QOS;
                pubmsg.retained = 0;
                MQTTClient_publishMessage(client, THERMAL_SENSOR, &pubmsg, &token);
                rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);

                //printf("Retorno do JSON para o tópico: %s\n", THERMAL_SENSOR);

                /*
                json_object_set_string(root_object3, "ID_02", "2N3409");
                json_object_set_string(root_object3, "Tipo", "Sensor de Temperatura");
                json_object_set_number(root_object3, "Valor em ºC", sensor2n);
                //json_object_set_number(root_object, "Tipo", 25);

                json_serialize_to_file_pretty(root_value3, "THERMAL_SENSOR.json");

                serialized_string = json_serialize_to_string_pretty(root_value3);
                puts(serialized_string);
                json_free_serialized_string(serialized_string);
                //json_value_free(root_value6);*/

                // fptr2 = fopen("THERMAL_SENSOR.json", "r");

                // if (fptr2 == NULL){
                //     printf("Cannot open file \n");
                //     exit(0);
                // }
 
                //     // Read contents from file
                // c2 = fgetc(fptr2);

                // while(c2 != EOF){
                //     printf ("%c", c2);
                //     c2 = fgetc(fptr2);
                // }
 
                // fclose(fptr2);

                // printf("\n");

            break;

            case 4:

                op = 0;
                printf("\n1- Ligar Luz\n2- Desligar Luz\n3- Mostrar Estado da luz\n");
                scanf("%d", &op);

                system("/usr/bin/clear");
 
                switch(op){

                    case 1:

                    strcpy(msg, "on");
                    pubmsg.payload = msg;
                    pubmsg.payloadlen = strlen(msg);
                    pubmsg.qos = QOS;
                    pubmsg.retained = 0;
                    MQTTClient_publishMessage(client, LIGHT, &pubmsg, &token);
                    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
                    break;
         
                    case 2:

                    strcpy(msg, "off");
                    pubmsg.payload = msg;
                    pubmsg.payloadlen = strlen(msg);
                    pubmsg.qos = QOS;
                    pubmsg.retained = 0;
                    MQTTClient_publishMessage(client, LIGHT, &pubmsg, &token);
                    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
                    break;
                    
                    case 3:
                    strcpy(msg, "state");
                    pubmsg.payload = msg;
                    pubmsg.payloadlen = strlen(msg);
                    pubmsg.qos = QOS;
                    pubmsg.retained = 0;
                    MQTTClient_publishMessage(client, LIGHT, &pubmsg, &token);
                    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);

                    // fptr1 = fopen("STATE_LIGHT.json", "r");

                    // if (fptr1 == NULL){
                    //     printf("Cannot open file \n");
                    //     exit(0);
                    // }
 
                    // // Read contents from file
                    // c1 = fgetc(fptr1);

                    // while (c1 != EOF){
                    //     printf ("%c", c1);
                    //     c1 = fgetc(fptr1);
                    // }
 
                    // fclose(fptr1);

                    // printf("\n");

                    break;

                    default:
                    printf("Comando Inválido!\n");
                    break;
                }

            break;

            case 5:

                op3 = 0;
                printf("\n1- Ligar AC\n2- Desligar AC\n3- Mostrar Estado do Ar condicionado");
                scanf("%d", &op3);

                system("/usr/bin/clear");
 
                switch(op3){
         
                    case 1:

                    strcpy(msg, "on");
                    pubmsg.payload = msg;
                    pubmsg.payloadlen = strlen(msg);
                    pubmsg.qos = QOS;
                    pubmsg.retained = 0;
                    MQTTClient_publishMessage(client, AC, &pubmsg, &token);
                    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
                    break;
         
                    case 2:

                    strcpy(msg, "off");
                    pubmsg.payload = msg;
                    pubmsg.payloadlen = strlen(msg);
                    pubmsg.qos = QOS;
                    pubmsg.retained = 0;
                    MQTTClient_publishMessage(client, AC, &pubmsg, &token);
                    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
                    break;
                    
                    case 3:
                    strcpy(msg, "state");
                    pubmsg.payload = msg;
                    pubmsg.payloadlen = strlen(msg);
                    pubmsg.qos = QOS;
                    pubmsg.retained = 0;
                    MQTTClient_publishMessage(client, AC, &pubmsg, &token);
                    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);

                    // printf("Retorno do JSON para o tópico: %s\n", AC);

                    // fptr5 = fopen("STATE_AC.json", "r");

                    // if (fptr5 == NULL){
                    //     printf("Cannot open file \n");
                    //     exit(0);
                    // }
 
                    // // Read contents from file
                    // c5 = fgetc(fptr5);

                    // while (c5 != EOF){
                    //     printf ("%c", c5);
                    //     c5 = fgetc(fptr5);
                    // }
 
                    // fclose(fptr5);

                    // printf("\n");

                    break;

                    default:
                    printf("Comando Inválido!\n");
                    break;
                }

            break;

            default:
            printf("Comando Inválido!\n");
            break;

        }
         
    }
     
    MQTTClient_unsubscribe(client, CONTROLADOR);
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}