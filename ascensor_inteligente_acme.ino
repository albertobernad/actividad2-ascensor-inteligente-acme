#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <IRremote.h>
#include <Adafruit_NeoPixel.h>

// Definición de pines utilizados en el montaje
#define DHTPIN 2
#define IRPIN 3
#define LEDPIN 5
#define SERVOPIN 6
#define LDRPIN A0

// Tipo de sensor DHT y número de LEDs de la tira NeoPixel
#define DHTTYPE DHT22
#define NUM_LEDS 8

// Creación de los objetos principales del sistema
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servoAscensor;
Adafruit_NeoPixel leds(NUM_LEDS, LEDPIN, NEO_GRB + NEO_KHZ800);

// Variable que guarda la planta actual del ascensor
int plantaActual = 0;

// Parámetros del control de temperatura
// Se toma 25 ºC como temperatura deseada y un margen de +/- 3 ºC
float tempObjetivo = 25.0;
float margen = 3.0;

void setup() {
  // Inicialización de la comunicación serie
  Serial.begin(9600);

  // Inicialización del sensor de temperatura/humedad y del LCD
  dht.begin();
  lcd.init();
  lcd.backlight();

  // Inicialización del servomotor en la planta 0
  servoAscensor.attach(SERVOPIN);
  servoAscensor.write(0);

  // Inicialización del receptor infrarrojo
  IrReceiver.begin(IRPIN);

  // Inicialización de la tira de LEDs apagada
  leds.begin();
  leds.clear();
  leds.show();

  // Mensaje inicial en pantalla
  lcd.setCursor(0, 0);
  lcd.print("Ascensor ACME");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Lectura de temperatura y humedad con el DHT22
  float temperatura = dht.readTemperature();
  float humedad = dht.readHumidity();

  // Lectura del LDR y conversión aproximada a porcentaje de luz
  int lecturaLuz = analogRead(LDRPIN);
  int luz = map(lecturaLuz, 1023, 0, 0, 100);

  // Control de la iluminación artificial según el nivel de luz
  controlarLuz(luz);

  // Control térmico mediante algoritmo ON-OFF con zona muerta
  String estadoClima = controlarTemperatura(temperatura);

  // Si el receptor IR detecta una señal del mando, se interpreta el botón pulsado
  if (IrReceiver.decode()) {
    int codigo = IrReceiver.decodedIRData.command;

    // Asociación de los botones numéricos del mando con las plantas del ascensor
    if (codigo == 104) moverAscensor(0);
    if (codigo == 48)  moverAscensor(1);
    if (codigo == 24)  moverAscensor(2);
    if (codigo == 122) moverAscensor(3);
    if (codigo == 16)  moverAscensor(4);
    if (codigo == 56)  moverAscensor(5);

    // Se deja preparado el receptor para recibir la siguiente orden
    IrReceiver.resume();
  }

  // Actualización de la información mostrada en la pantalla LCD
  mostrarLCD(temperatura, humedad, estadoClima);

  delay(300);
}

void moverAscensor(int plantaDestino) {
  // Conversión de la planta seleccionada a un ángulo del servomotor
  // Planta 0 = 0 grados y planta 5 = 180 grados
  int angulo = map(plantaDestino, 0, 5, 0, 180);

  lcd.clear();
  lcd.setCursor(0, 0);

  // Mensaje mostrado según el sentido del desplazamiento
  if (plantaDestino > plantaActual) {
    lcd.print("Subiendo a P:");
  } else if (plantaDestino < plantaActual) {
    lcd.print("Bajando a P:");
  } else {
    lcd.print("Ya en planta:");
  }

  lcd.print(plantaDestino);

  // Movimiento del servo hasta la posición asociada a la planta
  servoAscensor.write(angulo);

  // Retardo para simular el tiempo de desplazamiento de la cabina
  delay(1800);

  // Actualización de la planta actual
  plantaActual = plantaDestino;
  lcd.clear();
}

String controlarTemperatura(float temperatura) {
  // Control ON-OFF con zona muerta:
  // por encima de 28 ºC se activa refrigeración,
  // por debajo de 22 ºC se activa calefacción,
  // y entre ambos límites el sistema permanece en estado normal.

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

  // Si la iluminación baja del 80 %, se empiezan a encender LEDs.
  // Cuanto menor es la luz detectada, mayor es el número de LEDs encendidos.
  if (luz < 80) {
    ledsEncendidos = map(luz, 80, 0, 0, NUM_LEDS);
  }

  leds.clear();

  // Encendido progresivo de la tira NeoPixel
  for (int i = 0; i < ledsEncendidos; i++) {
    leds.setPixelColor(i, leds.Color(200, 200, 200));
  }

  leds.show();
}

void mostrarLCD(float temperatura, float humedad, String clima) {
  // Primera línea: planta actual y temperatura
  lcd.setCursor(0, 0);
  lcd.print("P:");
  lcd.print(plantaActual);
  lcd.print(" T:");
  lcd.print(temperatura, 1);
  lcd.print("C ");

  // Segunda línea: humedad y estado del control térmico
  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.print(humedad, 0);
  lcd.print("% ");
  lcd.print(clima);
  lcd.print("   ");
}
