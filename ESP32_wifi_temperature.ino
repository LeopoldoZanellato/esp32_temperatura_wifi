#include <WiFi.h> // Inclui a biblioteca WiFi para habilitar funções de Wi-Fi.
#include <DHT.h> // Inclui a biblioteca DHT para a leitura de sensores de temperatura e umidade.

// Variáveis para configuração de IP estático
IPAddress ip(192, 168, 15, 99);   // Endereço IP estático para o ESP32.
IPAddress gateway(192, 168, 15, 1);   // Gateway padrão (geralmente o roteador).
IPAddress subnet(255, 255, 255, 0);  // Máscara de sub-rede.

const char* ssid = "xxx"; // Nome da rede Wi-Fi.
const char* password = "xxx"; // Senha da rede Wi-Fi.

// Configurações do sensor DHT
#define DHTPIN 25 // Pino ao qual o sensor DHT está conectado.
#define DHTTYPE DHT22 // Tipo de sensor DHT (DHT22 neste caso).
DHT dht(DHTPIN, DHTTYPE); // Cria uma instância do sensor DHT.

// Cria uma instância do servidor na porta 80.
WiFiServer server(80);

void setup() {
  Serial.begin(115200); // Inicia a comunicação serial com uma taxa de 115200 bps.
  WiFi.mode(WIFI_STA); // Configura o módulo como cliente Wi-Fi (Station).
  WiFi.begin(ssid, password); // Inicia a conexão com a rede Wi-Fi.
  // Configura o endereço IP estático antes de iniciar a conexão Wi-Fi
  WiFi.config(ip, gateway, subnet);
  delay(1000);

  // Inicia a conexão Wi-Fi
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);



  // Aguarda até que a conexão seja estabelecida
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }

  Serial.println(F("WiFi connected"));
  server.begin(); // Inicia o servidor.
  Serial.println(F("Server started"));
  Serial.println(WiFi.localIP()); // Exibe o endereço IP do ESP32.

  // Inicializa o sensor DHT
  dht.begin();
}

void loop() {
  // Leitura dos dados do sensor DHT
  float temperature = dht.readTemperature(); // Lê a temperatura.
  float humidity = dht.readHumidity(); // Lê a umidade.

  // Verifica se a leitura foi bem-sucedida
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.print("    Humidity: ");
  Serial.println(humidity);

  // Código do servidor Wi-Fi
  WiFiClient client = server.available(); // Aguarda por clientes.
  if (client) {
    Serial.println("New client");
    client.setTimeout(5000); // Define um timeout para a conexão.
    String req = client.readStringUntil('\r'); // Lê a requisição do cliente até o caractere '\r'.
    Serial.println("Request: ");
    Serial.println(req);

    // Envia a temperatura e umidade como resposta
    client.print("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
    client.print(temperature);
    client.print(",");
    client.print(humidity);
    client.stop(); // Encerra a conexão com o cliente.
    Serial.println("Disconnecting from client");
  }

  delay(2000); // Espera por 2 segundos antes de repetir o loop.
}