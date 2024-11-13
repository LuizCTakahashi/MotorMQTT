#include <WiFi.h>              // Biblioteca para conectar o Wi-Fi no ESP32
#include <PubSubClient.h>       // Biblioteca para MQTT
#include <ESP32Servo.h>         // Biblioteca para controlar os servos

// Definindo as credenciais do Wi-Fi
const char* ssid = "Luiz";          // Substitua com seu SSID
const char* password = "luiz1234";     // Substitua com sua senha Wi-Fi

// Definindo o broker MQTT e os 6 tópicos
const char* mqtt_broker = "test.mosquitto.org";  // Broker MQTT público
const char* mqtt_topic1 = "Motor1";       // Tópico 1
const char* mqtt_topic2 = "Motor2";       // Tópico 2
const char* mqtt_topic3 = "Motor3";       // Tópico 3
const char* mqtt_topic4 = "Motor4";       // Tópico 4
const char* mqtt_topic5 = "Motor5";       // Tópico 5
const char* mqtt_topic6 = "Motor6";       // Tópico 6

WiFiClient espClient;            // Cliente Wi-Fi
PubSubClient client(espClient);  // Cliente MQTT

// Criação de objetos Servo para cada motor
Servo servo1, servo2, servo3, servo4, servo5, servo6;

// Defina os pinos de conexão dos servos
const int servoPins[] = {4, 5, 6, 7, 15, 16}; // Pinos de exemplo, altere conforme sua conexão

// Função para conectar ao Wi-Fi
void connect_wifi() {
  Serial.print("Conectando ao Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Conectado ao Wi-Fi!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

// Função para conectar ao broker MQTT
void connect_mqtt() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    
    // Tenta se conectar usando o nome do cliente (pode ser alterado)
    if (client.connect("esp32s3_client")) {
      Serial.println("Conectado ao broker MQTT!");
      
      // Inscreve-se nos 6 tópicos
      client.subscribe(mqtt_topic1);
      client.subscribe(mqtt_topic2);
      client.subscribe(mqtt_topic3);
      client.subscribe(mqtt_topic4);
      client.subscribe(mqtt_topic5);
      client.subscribe(mqtt_topic6);
    } else {
      Serial.print("Falha na conexão. Estado MQTT: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

// Função de callback para processar as mensagens recebidas
void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';  // Adiciona o terminador de string
  String message = String((char*)payload);
  
  // Exibe o tópico e a mensagem recebida no monitor serial
  Serial.print("Mensagem recebida no tópico: ");
  Serial.print(topic);
  Serial.print(" -> ");
  Serial.println(message);

  // Converte a mensagem para um valor de posição do servo (0-180)
  int pos = message.toInt();
  if (pos >= 0 && pos <= 180) {  // Verifica se o valor da mensagem está dentro do intervalo válido
    if (strcmp(topic, mqtt_topic1) == 0) {
      servo1.write(pos);  // Mover servo 1
    } else if (strcmp(topic, mqtt_topic2) == 0) {
      servo2.write(pos);  // Mover servo 2
    } else if (strcmp(topic, mqtt_topic3) == 0) {
      servo3.write(pos);  // Mover servo 3
    } else if (strcmp(topic, mqtt_topic4) == 0) {
      servo4.write(pos);  // Mover servo 4
    } else if (strcmp(topic, mqtt_topic5) == 0) {
      servo5.write(pos);  // Mover servo 5
    } else if (strcmp(topic, mqtt_topic6) == 0) {
      servo6.write(pos);  // Mover servo 6
    }
  } else {
    Serial.println("Mensagem inválida para controle de servo.");
  }
}

void setup() {
  // Inicia o monitor serial
  Serial.begin(115200);

  // Conecta ao Wi-Fi
  connect_wifi();
  
  // Define o servidor MQTT e o callback
  client.setServer(mqtt_broker, 1883);
  client.setCallback(callback);

  // Conecta ao broker MQTT
  connect_mqtt();

  // Anexa os servos aos pinos definidos
  for (int i = 0; i < 6; i++) {
    Servo* servo = nullptr;
    switch (i) {
      case 0: servo = &servo1; break;
      case 1: servo = &servo2; break;
      case 2: servo = &servo3; break;
      case 3: servo = &servo4; break;
      case 4: servo = &servo5; break;
      case 5: servo = &servo6; break;
    }
    if (servo != nullptr) {
      servo->attach(servoPins[i], 500, 2400);  // Anexa o servo ao pino
    }
  }
}

void loop() {
  // Mantém a conexão MQTT ativa e processa mensagens
  if (!client.connected()) {
    connect_mqtt();
  }

  client.loop();  // Necessário para o cliente MQTT processar mensagens
}
