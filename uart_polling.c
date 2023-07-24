

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_uart.h"
#include "fsl_gpio.h"
#include "fsl_common.h"
#include "fsl_debug_console.h"
#include "fsl_port.h"

#define DEMO_UART          UART2
#define DEMO_UART_CLKSRC   BUS_CLK
#define DEMO_UART_CLK_FREQ CLOCK_GetFreq(BUS_CLK)

#define BOARD_LED_GPIO     BOARD_LED_RED_GPIO
#define BOARD_LED_GPIO_PIN BOARD_LED_RED_GPIO_PIN

#define BOARD_SW_GPIO        BOARD_SW1_GPIO
#define BOARD_SW_PORT        BOARD_SW1_PORT
#define BOARD_SW_GPIO_PIN    BOARD_SW1_GPIO_PIN
#define BOARD_SW_IRQ         BOARD_SW1_IRQ
#define BOARD_SW_IRQ_HANDLER BOARD_SW1_IRQ_HANDLER
#define BOARD_SW_NAME        BOARD_SW1_NAME

volatile bool g_ButtonPress = false;


uint8_t txbuff[]   = "Uart polling example\r\nBoard will send back received characters\r\n";
uint8_t rxbuff[20] = {0};

void BOARD_SW_IRQ_HANDLER(void)
{
#if (defined(FSL_FEATURE_PORT_HAS_NO_INTERRUPT) && FSL_FEATURE_PORT_HAS_NO_INTERRUPT)
    /* Clear external interrupt flag. */
    GPIO_GpioClearInterruptFlags(BOARD_SW_GPIO, 1U << BOARD_SW_GPIO_PIN);
#else
    /* Clear external interrupt flag. */
    GPIO_PortClearInterruptFlags(BOARD_SW_GPIO, 1U << BOARD_SW_GPIO_PIN);
#endif
    /* Change state of button. */
    g_ButtonPress = true;
    SDK_ISR_EXIT_BARRIER;
}


int main(void)
{
    uint8_t ch[20];
    uart_config_t config;

    /* Define the init structure for the input switch pin */
       gpio_pin_config_t sw_config = {
           kGPIO_DigitalInput,
           0,
       };

       /* Define the init structure for the output LED pin */
       gpio_pin_config_t led_config = {
           kGPIO_DigitalOutput,
           0,
       };

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    /*
     * config.baudRate_Bps = 115200U;
     * config.parityMode = kUART_ParityDisabled;
     * config.stopBitCount = kUART_OneStopBit;
     * config.txFifoWatermark = 0;
     * config.rxFifoWatermark = 1;
     * config.enableTx = false;
     * config.enableRx = false;
     */
    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
    config.enableTx     = true;
    config.enableRx     = true;

    UART_Init(DEMO_UART, &config, DEMO_UART_CLK_FREQ);

    UART_WriteBlocking(DEMO_UART, txbuff, sizeof(txbuff) - 1);

    /* Init input switch GPIO. */
    #if (defined(FSL_FEATURE_PORT_HAS_NO_INTERRUPT) && FSL_FEATURE_PORT_HAS_NO_INTERRUPT)
        GPIO_SetPinInterruptConfig(BOARD_SW_GPIO, BOARD_SW_GPIO_PIN, kGPIO_InterruptFallingEdge);
    #else
        PORT_SetPinInterruptConfig(BOARD_SW_PORT, BOARD_SW_GPIO_PIN,  kPORT_InterruptFallingEdge);
    #endif
        EnableIRQ(BOARD_SW_IRQ);
        GPIO_PinInit(BOARD_SW_GPIO, BOARD_SW_GPIO_PIN, &sw_config);

        /* Init output LED GPIO. */
        GPIO_PinInit(BOARD_LED_GPIO, BOARD_LED_GPIO_PIN, &led_config);

//    while (1)
//    {
//      //  UART_ReadBlocking(DEMO_UART, "sapna", 5);
//        UART_WriteBlocking(DEMO_UART, "led on", 6);
//    }
    while (1)
       {
           if (g_ButtonPress)
           {
             
               /* Toggle LED. */
               GPIO_PortToggle(BOARD_LED_GPIO, 1U << BOARD_LED_GPIO_PIN);
               UART_WriteBlocking(DEMO_UART, "led on ", 7);
               /* Reset state of button. */
               g_ButtonPress = false;
           }
       }
}
