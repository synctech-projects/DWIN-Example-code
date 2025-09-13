طراحی پنل ساده خانه هوشمند با نمایشگر های dwin و کامپایلر اردوینو
===
وسایل استفاده شده:

- Wemos D1 mini
- DHT11 Module
- Lcd Dwin Dmg80480C050-03wtc | 800*480 Pxl | Capacitive Touch
- LED
----
![photo20776901191](https://github.com/user-attachments/assets/3d30a13b-d787-4573-9f4e-e3fd39d2056d)
----
نحوه سیم بندی :
===
![schematic](https://github.com/user-attachments/assets/871b911a-6006-4cf0-8985-794bd96e529e)

<table>
<tr>
<td>
  
| **wemos d1 mini** | **DWIN LCD** |
|:------------------|:-------------|
| `(D3) RX pin`     | TX2 pin      |
| `(D4) TX pin`     | RX2 pin      |
| `VCC (+)`         | +5V          |
| `GND (-)`         | GND          |
  
</td> 
<td>

| **wemos d1 mini** | **DHT11 Module** |
|:----------------|:-------------|
| `VCC (+)`       | +5V          |
| `GND (-)`       | GND          |
| `D2`            | Data         |

</td>
<td>
  
| **wemos d1 mini** | **LEDs** |
|:----------------|:-------------|
| `D8`       | + LED1          |
| `D7`       | + LED2          |
| `D6`       | + LED3          |
| `D5`       | + LED4          |

</td>
</tr>
</table>
