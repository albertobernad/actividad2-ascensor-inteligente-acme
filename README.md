# Actividad 2 - Ascensor inteligente ACME S.A.

Este repositorio contiene el desarrollo de la Actividad 2 de la asignatura Equipos e Instrumentación Electrónica.

El proyecto consiste en una simulación en Wokwi de un ascensor inteligente de cinco plantas, desarrollado con Arduino UNO. El sistema permite seleccionar la planta mediante un mando infrarrojo, simular el desplazamiento de la cabina con un servomotor y supervisar las condiciones ambientales mediante sensores de temperatura, humedad e iluminación.

## Enlace a la simulación en Wokwi

(https://wokwi.com/projects/461754320294918145)

## Componentes utilizados

- Arduino UNO
- Receptor infrarrojo y mando a distancia
- Servomotor SG90
- Sensor DHT22
- Sensor LDR
- Display LCD 16x2 I2C
- Tira LED NeoPixel

## Funcionamiento del sistema

El mando infrarrojo permite seleccionar la planta de destino del ascensor. Cada planta se asocia a una posición del servomotor, simulando el desplazamiento de la cabina.

El sistema también mide temperatura, humedad e iluminación. Estos valores se muestran en el display LCD junto con la planta actual y el estado del control térmico.

## Control de temperatura

Se ha implementado un algoritmo ON-OFF con zona muerta.

Parámetros utilizados:

- Temperatura de referencia: 25 ºC
- Margen de control: ±3 ºC

Funcionamiento:

- Si la temperatura es inferior a 22 ºC, se activa el estado CALOR ON.
- Si la temperatura está entre 22 ºC y 28 ºC, el sistema permanece en estado NORMAL.
- Si la temperatura supera los 28 ºC, se activa el estado FRIO ON.

## Control de iluminación

El sistema mide la iluminación mediante un LDR. Si el nivel de luz ambiental es inferior al 80 %, se encienden progresivamente los LEDs de la tira NeoPixel. Cuanta menos luz detecta el sensor, más LEDs se activan.

## Evidencias de funcionamiento

Las capturas de funcionamiento del sistema se incluyen en la memoria de la actividad. En ellas se muestra el circuito completo, el estado normal del sistema, el movimiento del ascensor, la activación de los modos CALOR ON y FRIO ON, y el control de iluminación mediante el LDR y la tira NeoPixel.

## Código fuente

El código fuente se encuentra en el archivo:

`ascensor_inteligente_acme.ino`

## Conclusión

El sistema desarrollado permite simular el funcionamiento de un ascensor inteligente con control ambiental básico. La solución integra sensores, actuadores y una interfaz LCD, cumpliendo los requisitos planteados en la actividad.
