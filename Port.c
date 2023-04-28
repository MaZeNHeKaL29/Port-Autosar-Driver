/******************************************************************************
 *
 * Module: Port
 *
 * File Name: Port.c
 *
 * Description: Source file for TM4C123GH6PM Microcontroller - Port Driver.
 *
 * Author: Mazen Hekal
 ******************************************************************************/

#include "Port.h"
#include "Port_Regs.h"

#if (PORT_DEV_ERROR_DETECT == STD_ON)

#include "Det.h"
/* AUTOSAR Version checking between Det and Port Modules */
#if ((DET_AR_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
		|| (DET_AR_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
		|| (DET_AR_PATCH_VERSION != PORT_AR_RELEASE_PATCH_VERSION))
#error "The AR version of Det.h does not match the expected version"
#endif

#endif

STATIC const Port_ConfigPin * Port_Pins = NULL_PTR;
STATIC uint8 Port_Status = PORT_NOT_INITIALIZED;

/************************************************************************************
 * Service Name: Port_Init
 * Service ID[hex]: 0x00
 * Sync/Async: Synchronous
 * Reentrancy: Non Reentrant
 * Parameters (in): ConfigPtr - Pointer to configuration set
 * Parameters (inout): None
 * Parameters (out): None
 * Return value: None
 * Description: Initializes the Port Driver module
 ************************************************************************************/
void Port_Init(const Port_ConfigType* ConfigPtr)
{
#if (PORT_DEV_ERROR_DETECT == STD_ON)
	/* check if the input configuration pointer is not a NULL_PTR */
	if (NULL_PTR == ConfigPtr)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_INIT_SID,
				PORT_E_PARAM_CONFIG);
	}
	else
#endif
	{
		Port_Pins = ConfigPtr->Pins;
		Port_Status = PORT_INITIALIZED;
		volatile uint32 * PortGpio_Ptr = NULL_PTR; /* point to the required Port Registers base address */
		volatile uint32 delay = 0;
		for(uint8 i = 0; i < PORT_CONFIGURED_PINS; i++)
		{
			uint8 port_Num = ConfigPtr->Pins[i].port_Num;
			uint8 pin_Num = ConfigPtr->Pins[i].pin_Num;
			uint8 pin_Direction = ConfigPtr->Pins[i].pin_Direction;
			uint8 internal_Resistor = ConfigPtr->Pins[i].internal_Resistor;
			uint8 pin_Init_Value = ConfigPtr->Pins[i].pin_Init_Value;
			uint8 pin_Mode = ConfigPtr->Pins[i].pin_Mode;
			switch(port_Num)
			{
			case  PORTA: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
			break;
			case  PORTB: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
			break;
			case  PORTC: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
			break;
			case  PORTD: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
			break;
			case  PORTE: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
			break;
			case  PORTF: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
			break;
			default: break;
			}
			if(PortGpio_Ptr == NULL_PTR)
			{
				continue;
			}
			/* Enable clock for PORT and allow time for clock to start*/
			SYSCTL_REGCGC2_REG |= (1<<port_Num);
			delay = SYSCTL_REGCGC2_REG;

			if( ((port_Num == PORTD) && (pin_Num == 7)) || ((port_Num == PORTF) && (pin_Num == 0)) ) /* PD7 or PF0 */
			{
				*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_LOCK_REG_OFFSET) = 0x4C4F434B;                     /* Unlock the GPIOCR register */
				SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_COMMIT_REG_OFFSET) , pin_Num);  /* Set the corresponding bit in GPIOCR register to allow changes on this pin */
			}
			else if( (port_Num == PORTC) && (pin_Num <= 3) ) /* PC0 to PC3 */
			{
				/* Do Nothing ...  this is the JTAG pins */
			}
			else
			{
				/* Do Nothing ... No need to unlock the commit register for this pin */
			}

			/* Setup Pin Mode */
			if(pin_Mode == PORT_MODE_DIO)
			{
				CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , pin_Num);      /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
				CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , pin_Num);             /* Disable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
				*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (pin_Num * 4));     /* Clear the PMCx bits for this pin */
				SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , pin_Num);        /* Set the corresponding bit in the GPIODEN register to enable digital functionality on this pin */
			}
			else if(pin_Mode == PORT_MODE_ANALOG)
			{
				SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , pin_Num);      /* Set the corresponding bit in the GPIOAMSEL register to enable analog functionality on this pin */
				CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , pin_Num);        /* Clear the corresponding bit in the GPIODEN register to disable digital functionality on this pin */
			}
			else
			{
				CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , pin_Num);          /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
				SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , pin_Num);                   /* enable Alternative function for this pin */
				*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (pin_Num * 4));         /* Clear the PMCx bits for this pin */
				*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) |= (pin_Mode << (pin_Num * 4));      	  /* Set the PMCx bits for this pin to the selected Alternate function in the configurations */
			}

			/* Setup Pin Direction */
			if(pin_Direction == PORT_PIN_OUT)
			{
				SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , pin_Num);                /* Set the corresponding bit in the GPIODIR register to configure it as output pin */

				if(pin_Init_Value == STD_HIGH)
				{
					SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DATA_REG_OFFSET) , pin_Num);          /* Set the corresponding bit in the GPIODATA register to provide initial value 1 */
				}
				else
				{
					CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DATA_REG_OFFSET) , pin_Num);        /* Clear the corresponding bit in the GPIODATA register to provide initial value 0 */
				}
			}
			else if(pin_Direction == PORT_PIN_IN)
			{
				CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , pin_Num);             /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */

				if(internal_Resistor == PORT_PIN_PULL_UP)
				{
					SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_UP_REG_OFFSET) , pin_Num);       /* Set the corresponding bit in the GPIOPUR register to enable the internal pull up pin */
				}
				else if(internal_Resistor == PORT_PIN_PULL_DOWN)
				{
					SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_DOWN_REG_OFFSET) , pin_Num);     /* Set the corresponding bit in the GPIOPDR register to enable the internal pull down pin */
				}
				else
				{
					CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_UP_REG_OFFSET) , pin_Num);     /* Clear the corresponding bit in the GPIOPUR register to disable the internal pull up pin */
					CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_DOWN_REG_OFFSET) , pin_Num);   /* Clear the corresponding bit in the GPIOPDR register to disable the internal pull down pin */
				}
			}
			else
			{
				/* Do Nothing */
			}
		}
	}
}


/************************************************************************************
 * Service Name: Port_SetPinDirection
 * Service ID[hex]: 0x01
 * Sync/Async: Synchronous
 * Reentrancy: Reentrant
 * Parameters (in): - Pin - Port Pin ID number
 	 	 	 	 	- Direction - Port Pin Direction
 * Parameters (inout): None
 * Parameters (out): None
 * Return value: None
 * Description: Sets the port pin direction
 ************************************************************************************/
void Port_SetPinDirection(Port_PinType Pin, Port_PinDirection Direction)
{
	boolean error = FALSE;
#if (PORT_DEV_ERROR_DETECT == STD_ON)
	/* check if API service called prior to module initialization */
	if(Port_Status == PORT_NOT_INITIALIZED)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SETPINDIRECTION_SID,
				PORT_E_UNINIT);
		error = TRUE;
	}
	else
	{
		/* No Action Required */
	}
	/* check if Incorrect Port Pin ID passed */
	if (Pin >= PORT_CONFIGURED_PINS)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SETPINDIRECTION_SID,
				PORT_E_PARAM_PIN);
		error = TRUE;
	}
	else
	{
		/* No Action Required */
	}
	/* check if Port Pin not configured as changeable */
	if(Port_Pins[Pin].pin_Direction_Changeable == FALSE)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SETPINDIRECTION_SID,
				PORT_E_DIRECTION_UNCHANGEABLE);
		error = TRUE;
	}
	else
	{
		/* No Action Required */
	}
#endif

	if(error == FALSE)
	{
		volatile uint32 * PortGpio_Ptr = NULL_PTR; /* point to the required Port Registers base address */
		uint8 port_Num = Port_Pins[Pin].port_Num;
		uint8 pin_Num = Port_Pins[Pin].pin_Num;
		uint8 pin_Direction = Port_Pins[Pin].pin_Direction;
		uint8 internal_Resistor = Port_Pins[Pin].internal_Resistor;
		uint8 pin_Init_Value = Port_Pins[Pin].pin_Init_Value;
		switch(port_Num)
		{
		case  PORTA: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
		break;
		case  PORTB: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
		break;
		case  PORTC: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
		break;
		case  PORTD: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
		break;
		case  PORTE: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
		break;
		case  PORTF: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
		break;
		default: break;
		}
		if(PortGpio_Ptr == NULL_PTR)
		{
			return;
		}
		/* Setup Pin Direction */
		if(pin_Direction == PORT_PIN_OUT)
		{
			SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , pin_Num);                /* Set the corresponding bit in the GPIODIR register to configure it as output pin */

			if(pin_Init_Value == STD_HIGH)
			{
				SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DATA_REG_OFFSET) , pin_Num);          /* Set the corresponding bit in the GPIODATA register to provide initial value 1 */
			}
			else
			{
				CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DATA_REG_OFFSET) , pin_Num);        /* Clear the corresponding bit in the GPIODATA register to provide initial value 0 */
			}
		}
		else if(pin_Direction == PORT_PIN_IN)
		{
			CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , pin_Num);             /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */

			if(internal_Resistor == PORT_PIN_PULL_UP)
			{
				SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_UP_REG_OFFSET) , pin_Num);       /* Set the corresponding bit in the GPIOPUR register to enable the internal pull up pin */
			}
			else if(internal_Resistor == PORT_PIN_PULL_DOWN)
			{
				SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_DOWN_REG_OFFSET) , pin_Num);     /* Set the corresponding bit in the GPIOPDR register to enable the internal pull down pin */
			}
			else
			{
				CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_UP_REG_OFFSET) , pin_Num);     /* Clear the corresponding bit in the GPIOPUR register to disable the internal pull up pin */
				CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_DOWN_REG_OFFSET) , pin_Num);   /* Clear the corresponding bit in the GPIOPDR register to disable the internal pull down pin */
			}
		}
		else
		{
			/* Do Nothing */
		}
	}
}

/************************************************************************************
 * Service Name: Port_RefreshPortDirection
 * Service ID[hex]: 0x02
 * Sync/Async: Synchronous
 * Reentrancy: Non Reentrant
 * Parameters (in): None
 * Parameters (inout): None
 * Parameters (out): None
 * Return value: None
 * Description: Refreshes port direction
 ************************************************************************************/
void Port_RefreshPortDirection(void)
{
#if (PORT_DEV_ERROR_DETECT == STD_ON)
	/* check if API service called prior to module initialization */
	if(Port_Status == PORT_NOT_INITIALIZED)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_REFRESHPORTDIRECTION_SID,
				PORT_E_UNINIT);
	}
	else
#endif
	{
		for(uint8 i = 0; i < PORT_CONFIGURED_PINS; i++)
		{
			volatile uint32 * PortGpio_Ptr = NULL_PTR; /* point to the required Port Registers base address */
			uint8 port_Num = Port_Configuration.Pins[i].port_Num;
			uint8 pin_Num = Port_Configuration.Pins[i].pin_Num;
			uint8 pin_Direction = Port_Configuration.Pins[i].pin_Direction;
			switch(port_Num)
			{
			case  PORTA: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
			break;
			case  PORTB: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
			break;
			case  PORTC: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
			break;
			case  PORTD: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
			break;
			case  PORTE: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
			break;
			case  PORTF: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
			break;
			default: break;
			}
			if(PortGpio_Ptr == NULL_PTR)
			{
				continue;
			}
			/* Refresh PIN Direction */
			if(pin_Direction == PORT_PIN_OUT)
			{
				SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , pin_Num);                /* Set the corresponding bit in the GPIODIR register to configure it as output pin */
			}

			if(pin_Direction == PORT_PIN_IN)
			{
				CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , pin_Num);                /* Set the corresponding bit in the GPIODIR register to configure it as output pin */
			}
			else
			{
				/* No Action Required */
			}
		}
	}
}

/************************************************************************************
 * Service Name: Port_GetVersionInfo
 * Service ID[hex]: 0x03
 * Sync/Async: Synchronous
 * Reentrancy: Non Reentrant
 * Parameters (in): None
 * Parameters (inout): None
 * Parameters (out): versioninfo - Pointer to where to store the version information of this module
 * Return value: None
 * Description: Returns the version information of this module
 ************************************************************************************/
#if (PORT_VERSION_INFO_API == STD_ON)
void Port_GetVersionInfo(Std_VersionInfoType *versioninfo)
{
	boolean error = FALSE;
#if (PORT_DEV_ERROR_DETECT == STD_ON)
	/* check if API service called prior to module initialization */
	if(Port_Status == PORT_NOT_INITIALIZED)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_GETVERSIONINFO_SID,
				PORT_E_UNINIT);
		error = TRUE;
	}
	else
	{
		/* No Action Required */
	}
	if(NULL_PTR == versioninfo)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_GETVERSIONINFO_SID,
				PORT_E_UNINIT);
		error = TRUE;
	}
	else
	{
		/* No Action Required */
	}
#endif
	if(error == FALSE)
	{
		/* Copy the vendor Id */
		versioninfo->vendorID = (uint16)PORT_VENDOR_ID;
		/* Copy the module Id */
		versioninfo->moduleID = (uint16)PORT_MODULE_ID;
		/* Copy Software Major Version */
		versioninfo->sw_major_version = (uint8)PORT_SW_MAJOR_VERSION;
		/* Copy Software Minor Version */
		versioninfo->sw_minor_version = (uint8)PORT_SW_MINOR_VERSION;
		/* Copy Software Patch Version */
		versioninfo->sw_patch_version = (uint8)PORT_SW_PATCH_VERSION;
	}
}
#endif

/************************************************************************************
 * Service Name: Port_SetPinMode
 * Service ID[hex]: 0x04
 * Sync/Async: Synchronous
 * Reentrancy: Reentrant
 * Parameters (in): - Pin - Port Pin ID number
 * 					- Mode - New Port Pin mode to be set on port pin
 * Parameters (inout): None
 * Parameters (out): None
 * Return value: None
 * Description: Sets the port pin mode
 ************************************************************************************/
void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode)
{
	boolean error = FALSE;
#if (PORT_DEV_ERROR_DETECT == STD_ON)
	/* check if API service called prior to module initialization */
	if(Port_Status == PORT_NOT_INITIALIZED)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SETPINMODE_SID,
				PORT_E_UNINIT);
		error = TRUE;
	}
	else
	{
		/* No Action Required */
	}
	/* check if Incorrect Port Pin ID passed */
	if (Pin >= PORT_CONFIGURED_PINS)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SETPINMODE_SID,
				PORT_E_PARAM_PIN);
		error = TRUE;
	}
	else
	{
		/* No Action Required */
	}
	/* check if Port Pin not configured as changeable */
	if(Port_Pins[Pin].pin_Mode_Changeable == FALSE)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SETPINMODE_SID,
				PORT_E_DIRECTION_UNCHANGEABLE);
		error = TRUE;
	}
	else
	{
		/* No Action Required */
	}
	/* check if Port Pin Mode passed not valid */
	if(Mode > NO_MODES)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SETPINMODE_SID,
				PORT_E_PARAM_INVALID_MODE);
		error = TRUE;
	}
	else
	{
		/* No Action Required */
	}
#endif
	if(error == FALSE)
	{
		volatile uint32 * PortGpio_Ptr = NULL_PTR; /* point to the required Port Registers base address */
		uint8 port_Num = Port_Pins[Pin].port_Num;
		uint8 pin_Num = Port_Pins[Pin].pin_Num;
		uint8 pin_Mode = Port_Pins[Pin].pin_Mode;
		switch(port_Num)
		{
		case  PORTA: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
		break;
		case  PORTB: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
		break;
		case  PORTC: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
		break;
		case  PORTD: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
		break;
		case  PORTE: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
		break;
		case  PORTF: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
		break;
		default: break;
		}
		if(PortGpio_Ptr == NULL_PTR)
		{
			return;
		}
		/* Setup Pin Mode */
		if(pin_Mode == PORT_MODE_DIO)
		{
			CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , pin_Num);      /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
			CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , pin_Num);             /* Disable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
			*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (pin_Num * 4));     /* Clear the PMCx bits for this pin */
			SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , pin_Num);         /* Set the corresponding bit in the GPIODEN register to enable digital functionality on this pin */
		}
		else if(pin_Mode == PORT_MODE_ANALOG)
		{
			SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , pin_Num);         /* Set the corresponding bit in the GPIOAMSEL register to enable analog functionality on this pin */
			CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , pin_Num);        /* Clear the corresponding bit in the GPIODEN register to disable digital functionality on this pin */
		}
		else
		{
			CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , pin_Num);          /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
			SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , pin_Num);                   /* enable Alternative function for this pin */
			*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (pin_Num * 4));         /* Clear the PMCx bits for this pin */
			*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) |= (pin_Mode << (pin_Num * 4));      	  /* Set the PMCx bits for this pin to the selected Alternate function in the configurations */
		}
	}
}

