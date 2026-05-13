<p align="center">
  <img src="IMAGENES/logoUNIS.png" width="25%" />
</p>

# Hardware Development Kit - STM32G071RBT6
## Proyecto Final de Electrónica y Diseño Electrónico

**Autor:** Káterin Sagastume  
**Carrera:** Ingeniería en Electrónica y Telecomunicaciones[cite: 1]  
**Institución:** Universidad del Istmo de Guatemala[cite: 1]  
**Fecha:** Mayo de 2026[cite: 1]  

<br>


## **Descripción general**


<br>


<p align="center">
  <img src="IMAGENES/TOPISOMETRICO_3D.png" width="80%" />
</p>

Este repositorio documenta el diseño de una tarjeta de desarrollo personalizada basada en el microcontrolador **STM32G071RBT6**[cite: 1]. El objetivo principal es crear un **Hardware Development Kit (HDK)** que facilite el prototipado rápido, integrando periféricos esenciales y manteniendo un diseño optimizado para fabricación de alta calidad[cite: 1].

La tarjeta se desarrolló como proyecto final, abarcando desde la concepción del diagrama de bloques y la selección de componentes hasta el ruteo de señales de alta densidad y la generación de archivos para manufactura industrial.


<br>


## **Tabla de Contenido**


<br>


* [Objetivos del proyecto](#objetivos-del-proyecto)
* [Arquitectura general](#arquitectura-general)
* [Imágenes del proyecto](#imágenes-del-proyecto)
* [Diseño y restricciones de PCB](#diseño-y-restricciones-de-pcb)
* [Contenido del repositorio](#contenido-del-repositorio)
* [Estado actual](#estado-actual)


<br>


## **Objetivos del proyecto**


<br>


* Diseñar un HDK funcional basado en la arquitectura ARM Cortex-M0+.
* Implementar un diseño de PCB de dos capas utilizando **Allegro PCB Editor**[cite: 1].
* Asegurar la integridad de las señales de clock y líneas de comunicación serial.
* Generar documentación técnica completa incluyendo BOM (Bill of Materials) y archivos de fabricación.


<br>


## **Arquitectura general**


<br>


La tarjeta se centra en el **STM32G071RBT6**, aprovechando su versatilidad en gestión de energía y periféricos digitales[cite: 1]. Los bloques principales incluyen:

1. **Gestión de Energía**: Reguladores de voltaje lineales para los 3.3V requeridos por el núcleo y periféricos.
2. **Interfaz de Programación**: Conectores estandarizados para depuración y carga de firmware vía SWD.
3. **Periféricos de Interfaz**: Acceso total a pines GPIO, canales ADC de precisión y protocolos I2C/SPI/UART.


<br>


## **Imágenes del proyecto**


<br>


### **Layout de la PCB**

<p align="center">
  <img src="IMAGENES/PCB_COMPLETA.png" width="80%" />
</p>
<p align="center"><i>Visualización del ruteo completo y planos de cobre</i></p>

### **Concepto 3D**

<p align="center">
  <img src="IMAGENES/TOP_3D.png" width="45%" />
  <img src="IMAGENES/BOTTOM_3D.png" width="45%" />
</p>
<p align="center"><i>Vista tridimensional: Cara superior y cara inferior</i></p>


<br>


## **Diseño y restricciones de PCB**


<br>


El trabajo de diseño en Cadence Allegro incluyó[cite: 1]:

* **Ruteo de señales**: Optimización de trayectorias para minimizar inductancias parásitas.
* **Control de Impedancia**: Aplicado en líneas de reloj críticas según el stack-up seleccionado.
* **Plano de Masa**: Vertido de cobre integral para mejorar la disipación térmica y el retorno de corrientes.
* **Validación DRC**: Limpieza total de errores de diseño según las reglas de la casa de fabricación[cite: 1].


<br>


## **Contenido del repositorio**


<br>


```text
.
├── Cotización/           # Listado de materiales (BOM) y análisis de costos
├── KIT_STM32.DSN         # Esquema electrónico en OrCAD Capture
├── kit_stm32.brd         # Layout de la PCB en Allegro PCB Editor
├── KIT-STM32_Doc.pdf     # Reporte técnico final y planos
└── IMAGENES/             # Directorio de recursos visuales y logos
