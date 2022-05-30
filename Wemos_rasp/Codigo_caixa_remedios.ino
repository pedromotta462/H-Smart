// Projeto de Python 
// Automação residencial direcionado a idosos
// Autores: Ana Carolinna, Ingrid Caroline e Pedro Motta

#include <PubSubClient.h>

#include <ESP8266WiFi.h>
#include <CertStoreBearSSL.h>
#include <WiFiServer.h>
#include <ESP8266WiFiScan.h>
#include <ArduinoWiFiServer.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiSTA.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266WiFiType.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGratuitous.h>
#include <ESP8266WiFiGeneric.h>
#include <WiFiServerSecureBearSSL.h>
#include <BearSSLHelpers.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServerSecure.h>

 
//defines:
#define TOPICO_LEITURA "MQTTprojetopythonCarolEnviaaaa"     //tópico MQTT de escuta
#define TOPICO_LEITURA2 "MQTTprojetopythonCarolEnviaaaa2"     //tópico MQTT de escuta
#define TOPICO_ENVIO_ESTADO_CAIXA   "MQTTprojetopythonCarolRecebeeee"    //tópico de envio da informação da

#define ID_MQTT  "PythonCarolprojeto"     //id mqtt (para identificação de sessão)                              
 
//Mapeamento de pinos na Weemos
#define D0    16
#define D1    5
#define D2    4
#define D3    0
#define D4    2
#define D5    14
#define D6    12
#define D7    13
#define D8    15
#define D9    3
#define D10   1
 
 
// WIFI
const char* SSID = "Carol"; // SSID / nome da rede WI-FI que deseja se conectar
const char* PASSWORD = "pipocadoce"; // Senha da rede WI-FI que deseja se conectar
  
// MQTT
const char* BROKER_MQTT = "broker.mqtt-dashboard.com"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; // Porta do Broker MQTT
 
 
//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient


bool estado_caixa = 0;  //variável que armazena o estado atual da saída
bool flag_caixa = 0;
String msgm;


void initSerial();
void initWiFi();
void initMQTT();
void reconectWiFi(); 
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void VerificaConexoesWiFIEMQTT(void);
void InitOutput(void);
 
/* 
 *  Implementações das funções
 */
 
void setup() 
{
    //inicializações:
    InitOutput();
    initSerial();
    initWiFi();
    initMQTT();
}
  
//Função: inicializa comunicação serial com baudrate 115200 (para fins de monitorar no terminal serial 
//        o que está acontecendo.
//Parâmetros: nenhum
//Retorno: nenhum
void initSerial() 
{
    Serial.begin(115200);
}
 
//Função: inicializa e conecta-se na rede WI-FI desejada
//Parâmetros: nenhum
//Retorno: nenhum
void initWiFi() 
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
     
    reconectWiFi();
}
  
//Função: inicializa parâmetros de conexão MQTT(endereço do 
//        broker, porta e seta função de callback)
void initMQTT() 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
    MQTT.setCallback(mqtt_callback);            //atribui função de callback 
}
  
//Função: função de callback 
//        esta função é chamada toda vez que uma informação de 
//        um dos tópicos subescritos chega)
void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
    String msg;
 
    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }
      
    Serial.println("Mensagem lida: " + msg); //Teste de chegada da mensagem
    msgm = msg;
}
  
//Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
//        em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.
//Parâmetros: nenhum
//Retorno: nenhum
void reconnectMQTT() 
{
    while (!MQTT.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) 
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_LEITURA); 
            MQTT.subscribe(TOPICO_LEITURA2); 
        } 
        else
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Havera nova tentatica de conexao em 2s");
            delay(2000);
        }
    }
}
  
//Função: reconecta-se ao WiFi
//Parâmetros: nenhum
//Retorno: nenhum
void reconectWiFi() 
{
    //se já está conectado a rede WI-FI, nada é feito. 
    //Caso contrário, são efetuadas tentativas de conexão
    if (WiFi.status() == WL_CONNECTED)
        return;
         
    WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
     
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
   
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
}
 
//Função: verifica o estado das conexões WiFI e ao broker MQTT. 
//        Em caso de desconexão (qualquer uma das duas), a conexão
//        é refeita.
//Parâmetros: nenhum
//Retorno: nenhum
void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected()) 
        reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
     
     reconectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}
 
//Função: envia ao Broker o estado atual do output 
//Parâmetros: nenhum
//Retorno: nenhum
//void EnviaEstadoOutputMQTT(void)
//{
//    if (EstadoSaida == '0'){
//      MQTT.publish(TOPICO_ENVIO_ESTADO_CAIXA, "D");
//      Serial.print("Enviado D");}
// 
//    if (EstadoSaida == '1'){
//      MQTT.publish(TOPICO_ENVIO_ESTADO_CAIXA, "L");
//      Serial.print("Enviado L");}
// 
//    Serial.println("- Estado da saida D0 enviado ao broker! " + String(EstadoSaida));
//    delay(5000);
//    
//}
 
//Função: inicializa o output em nível lógico baixo
//Parâmetros: nenhum
//Retorno: nenhum
void InitOutput(void)
{
    pinMode(D8, OUTPUT); //Pino do aviso visual
    pinMode(D7, OUTPUT); //Pino do aviso visual
    pinMode(D6, OUTPUT); //Pino do aviso visual
    pinMode(D5, OUTPUT); //Pino do aviso visual
    pinMode(D2, INPUT); //O LED associado a ele possui nível lógico invertido
          
}
 
 
//programa principal
void loop() 
{   
    //garante funcionamento das conexões WiFi e MQTT
    VerificaConexoesWiFIEMQTT();

    estado_caixa = digitalRead(D2);
    if(!estado_caixa && !flag_caixa){
      MQTT.publish(TOPICO_ENVIO_ESTADO_CAIXA, "ABRIU");
      flag_caixa = 1;          
    }
    if(msgm == "Conferido"){
    flag_caixa = 0;
    msgm = "OK";
    }
    
    else if(msgm == "Perto"){
      msgm = "OK";
      while(digitalRead(D2)){
    digitalWrite(D8, HIGH);
    digitalWrite(D7, HIGH);
    digitalWrite(D6, HIGH);
    digitalWrite(D5, HIGH);
    delay(1000);
    digitalWrite(D8, LOW);
    digitalWrite(D7, LOW);
    digitalWrite(D6, LOW);
    digitalWrite(D5, LOW);
    delay(1000);
    }
    }
 
 
    //keep-alive da comunicação com broker MQTT
    MQTT.loop();
}
