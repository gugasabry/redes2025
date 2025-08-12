#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include "SSD1306Wire.h"
#include "pins_arduino.h"

#define SCK     5
#define MISO    19
#define MOSI    27
#define SS      18
#define RST     14
#define DI0     26
#define BANDA   915E6

SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED);

String host = "nome";
String mensagem_recebida = "";
String mensagem_enviada = "";
bool sender;

void VextON(void) {

  pinMode(Vext, OUTPUT);

  digitalWrite(Vext, LOW);

}

void VextOFF(void) {

  pinMode(Vext, OUTPUT);

  digitalWrite(Vext, HIGH);

}

void displayReset(void) {

  pinMode(RST_OLED, OUTPUT);

  digitalWrite(RST_OLED, HIGH);

  delay(1);

  digitalWrite(RST_OLED, LOW);

  delay(1);

  digitalWrite(RST_OLED, HIGH);

  delay(1);

}

String formatarTexto(String texto, int larguraMaxima = 26) {

  int i;
  
  String resultado = "";

  for (i = 0; texto[i] != '\n'; i++) {

    resultado += texto[i];

  }

  resultado += texto[i];

  i = i + 1;

  texto = texto.substring(i);

  for (i = 0; i < texto.length(); i++) {

    resultado += texto[i];

    if ((i + 1) % larguraMaxima == 0 && i != 0) {

      resultado += '\n';

    }

  }

  return resultado;

}


void imprimeMensagem(String texto) {
  
  display.clear();

  display.setTextAlignment(TEXT_ALIGN_LEFT);

  display.drawString(0, 0, formatarTexto(texto));

  display.display();

}

void barraDeProgresso() {

  for (int progress = 1; progress <= 100; progress++) {

    display.clear();

    display.drawProgressBar(0, 32, 120, 10, progress);

    display.setTextAlignment(TEXT_ALIGN_CENTER);

    display.drawString(64, 15, String(progress) + "%");

    display.display();

    delay (10);

  }

}

void boasVindas () {

  display.clear();

  display.setTextAlignment(TEXT_ALIGN_CENTER);

  display.drawString(60, 15, "Aula de Redes\n");

  display.drawString(60, 30, "Prof.: Gustavo Sabry");

  display.drawString(60, 45, "IFPB - Sousa");

  display.display();

}

void setup() {

  Serial.begin(9600);

  while (!Serial) {
    delay(100);
  }

  Serial.println();

  Serial.println();

  VextON();

  displayReset();

  display.init();

  Serial.println ("Display inicializado com sucesso!");

  display.flipScreenVertically();

  display.setFont(ArialMT_Plain_10);

  Serial.println("LoRa Sender");

  LoRa.setPins(SS, RST, DI0);
  
  if (!LoRa.begin(BANDA)) {

    Serial.println("Falha na inicializacao da LoRa!");

    while (1);

  }

  Serial.println("LoRa inicializada com sucesso!");

  barraDeProgresso();

  boasVindas();

}

void loop() {

  sender = false;
  
  if (Serial.available()) {

    mensagem_enviada = Serial.readStringUntil('\n');

    mensagem_enviada.trim();
    
    if (mensagem_enviada.length() > 0) {
      
      sender = true;

      mensagem_enviada = host + " enviou:\n" + mensagem_enviada;

      LoRa.beginPacket();
      LoRa.print(mensagem_enviada);
      LoRa.endPacket();

      Serial.print("Enviado: ");
      Serial.println(mensagem_enviada);

      display.clear();
      imprimeMensagem (mensagem_enviada);
      display.display();

    }
    
  }

  int pacoteRecebido = LoRa.parsePacket();

  if (pacoteRecebido) {

    mensagem_recebida = "";

    while (LoRa.available()) {

      mensagem_recebida += (char)LoRa.read();

    }
    
    Serial.print("Recebido: ");
    Serial.println(mensagem_recebida);

    display.clear();
    imprimeMensagem (mensagem_recebida);
    display.display();

  }
  
}
