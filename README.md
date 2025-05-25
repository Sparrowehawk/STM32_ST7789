# STM32_ST7789
An SPI driver for ST7789 displays (e.g., GMT020-02) on a STM32F103C6T6; works with any STM32F1xx chip

## Parts Used
- STM32F103C6T6
- GMT020-02
- Logic Analyzer (used to see the communications)

## Setup

- Use the STM32CubeIde (.ioc file) to select and enable the SPI pins.
  - Go into the .ioc file and select connectivity -> SPI
  - Enable SPI
  - See the two pins that are green on the chip, they are the SPI pins
- Select an addiotional 3 GPIO pins to be GPIO output
  - One each for RST, CS and DC    

   ![image](https://github.com/user-attachments/assets/7abb081f-0e13-4d5f-9790-d82484ffb8c3)


- Add this line of code in `main.h`
   ```
   /* USER CODE BEGIN Includes */
  #include "gmt020-02.h"
  /* USER CODE END Includes */
  ```

- Add `gmt020-02.c` and `fonts.c` into  `/Src`
- Add `gmt020-02.h` and `fonts.h` to `/Inc`
- In `main.c`
  - Create a private variable called hi2c1 (depending on what the SPI creates in MX_SPIx_Init())
```
/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
/* USER CODE BEGIN PV */
```
  - After GPIO and I2C initialises, initialise the MPU6050
    - Note: `ST7789_init()` can be changed to be a void if you don't care about error handling
  - In the `while(1)` loop, use the display driver commands
    
---

## License
MIT License

---

## Author
[Sparrowehawk](https://github.com/Sparrowehawk)
