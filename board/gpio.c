#include "gpio.h"

#include "GD32E503.h"
#include "BitCodec.h"


int initGpio(void)
{
	// GD32E503_RCU_APB2RST
	// PGRST [8]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2RST), 8, 0b1);
	// PFRST [7]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2RST), 7, 0b1);
	// PERST [6]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2RST), 6, 0b1);
	// PDRST [5]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2RST), 5, 0b1);
	// PCRST [4]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2RST), 4, 0b1);
	// PBRST [3]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2RST), 3, 0b1);
	// PARST [2]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2RST), 2, 0b1);
	// AFRST [1]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2RST), 0, 0b1);
	
	// GD32E503_RCU_APB2EN
	// PGEN [8]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2EN), 8, 0b1);
	// PFEN [7]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2EN), 7, 0b1);
	// PEEN [6]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2EN), 6, 0b1);
	// PDEN [5]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2EN), 5, 0b1);
	// PCEN [4]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2EN), 4, 0b1);
	// PBEN [3]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2EN), 3, 0b1);
	// PAEN [2]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2EN), 2, 0b1);
	// AFEN [0]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2EN), 0, 0b1);
	
	// GD32E503_RCU_APB2RST
	// PGRST [8]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2RST), 8, 0b0);
	// PFRST [7]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2RST), 7, 0b0);
	// PERST [6]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2RST), 6, 0b0);
	// PDRST [5]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2RST), 5, 0b0);
	// PCRST [4]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2RST), 4, 0b0);
	// PBRST [3]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2RST), 3, 0b0);
	// PARST [2]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2RST), 2, 0b0);
	// AFRST [1]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB2RST), 0, 0b0);
	
	// GD32E503_GPIO_CPSCTL
	// CPS_EN [0] 0b0 Disable
	REG32_SET_BIT(GD32E503_AFIO(GD32E503_AFIO_CPSCTL), 0, 0b0);
	
	return 0;
}

int getGpioInput(Gpio gpio, uint16_t* status)
{
	switch(gpio)
	{
	case GpioA: *status = REG32_GET(GD32E503_GPIOA(GD32E503_GPIO_ISTAT)); break;
	case GpioB: *status = REG32_GET(GD32E503_GPIOB(GD32E503_GPIO_ISTAT)); break;
	case GpioC: *status = REG32_GET(GD32E503_GPIOC(GD32E503_GPIO_ISTAT)); break;
	case GpioD: *status = REG32_GET(GD32E503_GPIOD(GD32E503_GPIO_ISTAT)); break;
	case GpioE: *status = REG32_GET(GD32E503_GPIOE(GD32E503_GPIO_ISTAT)); break;
	case GpioF: *status = REG32_GET(GD32E503_GPIOF(GD32E503_GPIO_ISTAT)); break;
	case GpioG: *status = REG32_GET(GD32E503_GPIOG(GD32E503_GPIO_ISTAT)); break;
	
	default:
		return -1;
	}
	
	return 0;
}

int setGpioOutput(Gpio gpio, unsigned channel, GpioState state)
{
	if(channel >= GPIO_CHANNEL_COUNT)
		return -1;
	
	if(state == GpioStateHiZ) switch(gpio)
	{
	// GD32E503_GPIO_CTL
	// CTL 0b01 Float
	// MD 0b00 Input
	case GpioA: REG32_SET_RANGE(GD32E503_GPIOA(GD32E503_GPIO_CTL0) + channel / 8 * 4, channel % 8 * 4 + 3, channel % 8 * 4, 0b0100); break;
	case GpioB: REG32_SET_RANGE(GD32E503_GPIOB(GD32E503_GPIO_CTL0) + channel / 8 * 4, channel % 8 * 4 + 3, channel % 8 * 4, 0b0100); break;
	case GpioC: REG32_SET_RANGE(GD32E503_GPIOC(GD32E503_GPIO_CTL0) + channel / 8 * 4, channel % 8 * 4 + 3, channel % 8 * 4, 0b0100); break;
	case GpioD: REG32_SET_RANGE(GD32E503_GPIOD(GD32E503_GPIO_CTL0) + channel / 8 * 4, channel % 8 * 4 + 3, channel % 8 * 4, 0b0100); break;
	case GpioE: REG32_SET_RANGE(GD32E503_GPIOE(GD32E503_GPIO_CTL0) + channel / 8 * 4, channel % 8 * 4 + 3, channel % 8 * 4, 0b0100); break;
	case GpioF: REG32_SET_RANGE(GD32E503_GPIOF(GD32E503_GPIO_CTL0) + channel / 8 * 4, channel % 8 * 4 + 3, channel % 8 * 4, 0b0100); break;
	case GpioG: REG32_SET_RANGE(GD32E503_GPIOG(GD32E503_GPIO_CTL0) + channel / 8 * 4, channel % 8 * 4 + 3, channel % 8 * 4, 0b0100); break;
	
	default:
		return -2;
	}
	else if(state == GpioStateLow) switch(gpio)
	{
	// GD32E503_GPIO_CTL
	// CTL 0b00 Push-Pull
	// MD 0b11 Output 50MHz
	case GpioA: REG32_SET_RANGE(GD32E503_GPIOA(GD32E503_GPIO_CTL0) + channel / 8 * 4, channel % 8 * 4 + 3, channel % 8 * 4, 0b0011); REG32_SET_BIT(GD32E503_GPIOA(GD32E503_GPIO_BC), channel, 0b1); break;
	case GpioB: REG32_SET_RANGE(GD32E503_GPIOB(GD32E503_GPIO_CTL0) + channel / 8 * 4, channel % 8 * 4 + 3, channel % 8 * 4, 0b0011); REG32_SET_BIT(GD32E503_GPIOB(GD32E503_GPIO_BC), channel, 0b1); break;
	case GpioC: REG32_SET_RANGE(GD32E503_GPIOC(GD32E503_GPIO_CTL0) + channel / 8 * 4, channel % 8 * 4 + 3, channel % 8 * 4, 0b0011); REG32_SET_BIT(GD32E503_GPIOC(GD32E503_GPIO_BC), channel, 0b1); break;
	case GpioD: REG32_SET_RANGE(GD32E503_GPIOD(GD32E503_GPIO_CTL0) + channel / 8 * 4, channel % 8 * 4 + 3, channel % 8 * 4, 0b0011); REG32_SET_BIT(GD32E503_GPIOD(GD32E503_GPIO_BC), channel, 0b1); break;
	case GpioE: REG32_SET_RANGE(GD32E503_GPIOE(GD32E503_GPIO_CTL0) + channel / 8 * 4, channel % 8 * 4 + 3, channel % 8 * 4, 0b0011); REG32_SET_BIT(GD32E503_GPIOE(GD32E503_GPIO_BC), channel, 0b1); break;
	case GpioF: REG32_SET_RANGE(GD32E503_GPIOF(GD32E503_GPIO_CTL0) + channel / 8 * 4, channel % 8 * 4 + 3, channel % 8 * 4, 0b0011); REG32_SET_BIT(GD32E503_GPIOF(GD32E503_GPIO_BC), channel, 0b1); break;
	case GpioG: REG32_SET_RANGE(GD32E503_GPIOG(GD32E503_GPIO_CTL0) + channel / 8 * 4, channel % 8 * 4 + 3, channel % 8 * 4, 0b0011); REG32_SET_BIT(GD32E503_GPIOG(GD32E503_GPIO_BC), channel, 0b1); break;
	
	default:
		return -3;
	}
	else if(state == GpioStateHigh) switch(gpio)
	{
	// GD32E503_GPIO_CTL
	// CTL 0b00 Push-Pull
	// MD 0b11 Output 50MHz
	case GpioA: REG32_SET_RANGE(GD32E503_GPIOA(GD32E503_GPIO_CTL0) + channel / 8 * 4, channel % 8 * 4 + 3, channel % 8 * 4, 0b0011); REG32_SET_BIT(GD32E503_GPIOA(GD32E503_GPIO_BOP), channel, 0b1); break;
	case GpioB: REG32_SET_RANGE(GD32E503_GPIOB(GD32E503_GPIO_CTL0) + channel / 8 * 4, channel % 8 * 4 + 3, channel % 8 * 4, 0b0011); REG32_SET_BIT(GD32E503_GPIOB(GD32E503_GPIO_BOP), channel, 0b1); break;
	case GpioC: REG32_SET_RANGE(GD32E503_GPIOC(GD32E503_GPIO_CTL0) + channel / 8 * 4, channel % 8 * 4 + 3, channel % 8 * 4, 0b0011); REG32_SET_BIT(GD32E503_GPIOC(GD32E503_GPIO_BOP), channel, 0b1); break;
	case GpioD: REG32_SET_RANGE(GD32E503_GPIOD(GD32E503_GPIO_CTL0) + channel / 8 * 4, channel % 8 * 4 + 3, channel % 8 * 4, 0b0011); REG32_SET_BIT(GD32E503_GPIOD(GD32E503_GPIO_BOP), channel, 0b1); break;
	case GpioE: REG32_SET_RANGE(GD32E503_GPIOE(GD32E503_GPIO_CTL0) + channel / 8 * 4, channel % 8 * 4 + 3, channel % 8 * 4, 0b0011); REG32_SET_BIT(GD32E503_GPIOE(GD32E503_GPIO_BOP), channel, 0b1); break;
	case GpioF: REG32_SET_RANGE(GD32E503_GPIOF(GD32E503_GPIO_CTL0) + channel / 8 * 4, channel % 8 * 4 + 3, channel % 8 * 4, 0b0011); REG32_SET_BIT(GD32E503_GPIOF(GD32E503_GPIO_BOP), channel, 0b1); break;
	case GpioG: REG32_SET_RANGE(GD32E503_GPIOG(GD32E503_GPIO_CTL0) + channel / 8 * 4, channel % 8 * 4 + 3, channel % 8 * 4, 0b0011); REG32_SET_BIT(GD32E503_GPIOG(GD32E503_GPIO_BOP), channel, 0b1); break;
	
	default:
		return -4;
	}
	else
		return -5;
	
	return 0;
}
