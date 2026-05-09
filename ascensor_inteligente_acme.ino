#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <IRremote.h>
#include <Adafruit_NeoPixel.h>

#define DHTPIN 2
#define IRPIN 3
#define LEDPIN 5
#define SERVOPIN 6
#define LDRPIN A0

#define DHTTYPE DHT22
#define NUM_LEDS 8

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servoAscensor;
Adafruit_NeoPixel leds(NUM_LEDS, LEDPIN, NEO_GRB + NEO_KHZ800);

int plantaActual = 0;

float tempObjetivo = 25.0;
float margen = 3.0;

void setup() {
  Serial.begin(9600);

  dht.begin();
  lcd.init();
  lcd.backlight();

  servoAscensor.attach(SERVOPIN);
  servoAscensor.write(0);

  IrReceiver.begin(IRPIN);

  leds.begin();
  leds.clear();
  leds.show();

  lcd.setCursor(0, 0);
  lcd.print("Ascensor ACME");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(2000);
  lcd.clear();
}

void loop() {
  float temperatura = dht.readTemperature();
  float humedad = dht.readHumidity();

  int lecturaLuz = analogRead(LDRPIN);
  int luz = map(lecturaLuz, 1023, 0, 0, 100);

  controlarLuz(luz);

  String estadoClima = controlarTemperatura(temperatura);

  if (IrReceiver.decode()) {
    int codigo = IrReceiver.decodedIRData.command;

    if (codigo == 104) moverAscensor(0);
    if (codigo == 48)  moverAscensor(1);
    if (codigo == 24)  moverAscensor(2);
    if (codigo == 122) moverAscensor(3);
    if (codigo == 16)  moverAscensor(4);
    if (codigo == 56)  moverAscensor(5);

    IrReceiver.resume();
  }

  mostrarLCD(temperatura, humedad, estadoClima);

  delay(300);
}

void moverAscensor(int plantaDestino) {
  int angulo = map(plantaDestino, 0, 5, 0, 180);

  lcd.clear();
  lcd.setCursor(0, 0);

  if (plantaDestino > plantaActual) {
    lcd.print("Subiendo a P:");
  } else if (plantaDestino < plantaActual) {
    lcd.print("Bajando a P:");
  } else {
    lcd.print("Ya en planta:");
  }

  lcd.print(plantaDestino);

  servoAscensor.write(angulo);
  delay(1800);

  plantaActual = plantaDestino;
  lcd.clear();
}

String controlarTemperatura(float temperatura) {
  if (temperatura > tempObjetivo + margen) {
    return "FRIO ON";
  }

  if (temperatura < tempObjetivo - margen) {
    return "CALOR ON";
  }

  return "NORMAL";
}

void controlarLuz(int luz) {
  int ledsEncendidos = 0;

  if (luz < 80) {
    ledsEncendidos = map(luz, 80, 0, 0, NUM_LEDS);
  }

  leds.clear();

  for (int i = 0; i < ledsEncendidos; i++) {
    leds.setPixelColor(i, leds.Color(200, 200, 200));
  }

  leds.show();
}

void mostrarLCD(float temperatura, float humedad, String clima) {
  lcd.setCursor(0, 0);
  lcd.print("P:");
  lcd.print(plantaActual);
  lcd.print(" T:");
  lcd.print(temperatura, 1);
  lcd.print("C ");

  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.print(humedad, 0);
  lcd.print("% ");
  lcd.print(clima);
  lcd.print("   ");
}