#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <IRremote.h>
#include <Adafruit_NeoPixel.h>

// Pines principales del montaje
#define DHTPIN 2
#define IRPIN 3
#define LEDPIN 5
#define SERVOPIN 6
#define TRIGPIN 9
#define ECHOPIN 10
#define LDRPIN A0

// Configuración de sensores y actuadores
#define DHTTYPE DHT22
#define NUM_LEDS 8

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servoAscensor;
Adafruit_NeoPixel leds(NUM_LEDS, LEDPIN, NEO_GRB + NEO_KHZ800);

int plantaActual = 0;

// Parámetros del control de temperatura
float tempObjetivo = 25.0;
float margen = 3.0;

// Variables para no imprimir el registro continuamente
unsigned long ultimoRegistro = 0;
const unsigned long intervaloRegistro = 2000;

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

  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);

  lcd.setCursor(0, 0);
  lcd.print("Ascensor ACME");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Lectura de temperatura y humedad
  float temperatura = dht.readTemperature();
  float humedad = dht.readHumidity();

  // Lectura de iluminación
  int lecturaLuz = analogRead(LDRPIN);
  int luz = map(lecturaLuz, 1023, 0, 0, 100);

  // Detección de presencia en cabina
  bool presencia = detectarPresencia();

  // Control de iluminación y temperatura
  controlarLuz(luz);
  String estadoClima = controlarTemperatura(temperatura);

  // Lectura del mando infrarrojo para seleccionar planta
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

  mostrarLCD(temperatura, humedad, estadoClima, presencia);
  registrarDatos(temperatura, humedad, presencia, estadoClima);

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

bool detectarPresencia() {
  // El HC-SR04 simula la detección de una persona dentro de la cabina.
  // Si la distancia es menor de 30 cm, se considera que hay presencia.

  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);

  long duracion = pulseIn(ECHOPIN, HIGH, 30000);
  float distancia = duracion * 0.034 / 2;

  if (distancia > 2 && distancia < 30) {
    return true;
  } else {
    return false;
  }
}

String controlarTemperatura(float temperatura) {
  // Control ON-OFF con zona muerta:
  // <22 ºC: calefacción; 22-28 ºC: normal; >28 ºC: refrigeración.

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

  // Si la luz baja del 80 %, se encienden progresivamente los LEDs.
  if (luz < 80) {
    ledsEncendidos = map(luz, 80, 0, 0, NUM_LEDS);
  }

  leds.clear();

  for (int i = 0; i < ledsEncendidos; i++) {
    leds.setPixelColor(i, leds.Color(200, 200, 200));
  }

  leds.show();
}

void mostrarLCD(float temperatura, float humedad, String clima, bool presencia) {
  // Primera línea: planta y temperatura
  lcd.setCursor(0, 0);
  lcd.print("P:");
  lcd.print(plantaActual);
  lcd.print(" T:");
  lcd.print(temperatura, 1);
  lcd.print("C ");

  // Segunda línea: presencia y estado térmico
  lcd.setCursor(0, 1);
  lcd.print("U:");
  lcd.print(presencia ? "SI " : "NO ");
  lcd.print(clima);
  lcd.print("   ");
}

void registrarDatos(float temperatura, float humedad, bool presencia, String clima) {
  // Registro periódico por monitor serie.
  // Aquí se deja constancia de planta, presencia, temperatura, humedad y estado.

  if (millis() - ultimoRegistro >= intervaloRegistro) {
    ultimoRegistro = millis();

    Serial.print("Planta: ");
    Serial.print(plantaActual);
    Serial.print(" | Presencia: ");
    Serial.print(presencia ? "SI" : "NO");
    Serial.print(" | Temp motor: ");
    Serial.print(temperatura, 1);
    Serial.print(" C | Humedad: ");
    Serial.print(humedad, 1);
    Serial.print(" % | Estado: ");
    Serial.println(clima);
  }
}
