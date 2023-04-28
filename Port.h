 /******************************************************************************
 *
 * Module: Port
 *
 * File Name: Port.h
 *
 * Description: Header file for TM4C123GH6PM Microcontroller - Port Driver.
 *
 * Author: Mazen Hekal
 ******************************************************************************/

#ifndef PORT_H
#define PORT_H


/* Id for the company in the AUTOSAR */
#define PORT_VENDOR_ID    (1000U)

/* Port Module Id */
#define PORT_MODULE_ID    (121U)

/* PORT Instance Id */
#define PORT_INSTANCE_ID  (0U)


/*
 * Module Version 1.0.0
 */
#define PORT_SW_MAJOR_VERSION           (1U)
#define PORT_SW_MINOR_VERSION           (0U)
#define PORT_SW_PATCH_VERSION           (0U)

/*
 * AUTOSAR Version 4.0.3
 */
#define PORT_AR_RELEASE_MAJOR_VERSION   (4U)
#define PORT_AR_RELEASE_MINOR_VERSION   (0U)
#define PORT_AR_RELEASE_PATCH_VERSION   (3U)

/*
 * Macros for Port Status
 */
#define PORT_INITIALIZED                (1U)
#define PORT_NOT_INITIALIZED            (0U)

/* PORT Numbers */
#define PORTA		(0U)
#define PORTB		(1U)
#define PORTC		(2U)
#define PORTD		(3U)
#define PORTE		(4U)
#define PORTF		(5U)

/* PIN Numbers */
#define PIN0		(uint8)0x00
#define PIN1		(uint8)0x01
#define PIN2		(uint8)0x02
#define PIN3		(uint8)0x03
#define PIN4		(uint8)0x04
#define PIN5		(uint8)0x05
#define PIN6		(uint8)0x06
#define PIN7		(uint8)0x07
#define PIN8		(uint8)0x01


/* PIN Modes */
#define NO_MODES			(15U)
#define PORT_MODE_DIO		(0U)
#define PORT_MODE_ALT01         (1U)
#define PORT_MODE_ALT02         (2U)
#define PORT_MODE_ALT03         (3U)
#define PORT_MODE_ALT04         (4U)
#define PORT_MODE_ALT05         (5U)
#define PORT_MODE_ALT06         (6U)
#define PORT_MODE_ALT07         (7U)
#define PORT_MODE_ALT08         (8U)
#define PORT_MODE_ALT09         (9U)
#define PORT_MODE_ALT10         (10U)
#define PORT_MODE_ALT11         (11U)
#define PORT_MODE_ALT12         (12U)
#define PORT_MODE_ALT13         (13U)
#define PORT_MODE_ALT14         (14U)
#define PORT_MODE_ANALOG	(15U)

/* Standard AUTOSAR types */
#include "Std_Types.h"

/* AUTOSAR checking between Std Types and Port Modules */
#if ((STD_TYPES_AR_RELEASE_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
 ||  (STD_TYPES_AR_RELEASE_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
 ||  (STD_TYPES_AR_RELEASE_PATCH_VERSION != PORT_AR_RELEASE_PATCH_VERSION))
  #error "The AR version of Std_Types.h does not match the expected version"
#endif

/* Port Pre-Compile Configuration Header file */
#include "Port_Cfg.h"

/* AUTOSAR Version checking between Port_Cfg.h and Port.h files */
#if ((PORT_CFG_AR_RELEASE_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
 ||  (PORT_CFG_AR_RELEASE_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
 ||  (PORT_CFG_AR_RELEASE_PATCH_VERSION != PORT_AR_RELEASE_PATCH_VERSION))
  #error "The AR version of Dio_Cfg.h does not match the expected version"
#endif

/* Software Version checking between Port_Cfg.h and Port.h files */
#if ((PORT_CFG_SW_MAJOR_VERSION != PORT_SW_MAJOR_VERSION)\
 ||  (PORT_CFG_SW_MINOR_VERSION != PORT_SW_MINOR_VERSION)\
 ||  (PORT_CFG_SW_PATCH_VERSION != PORT_SW_PATCH_VERSION))
  #error "The SW version of Port_Cfg.h does not match the expected version"
#endif

/* Non AUTOSAR files */
#include "Common_Macros.h"

/******************************************************************************
 *                      API Service Id Macros                                 *
 ******************************************************************************/

/* Service ID for Port_Init */
#define PORT_INIT_SID					(uint8)0x00

/* Service ID for Port_SetPinDirection */
#define PORT_SETPINDIRECTION_SID		(uint8)0x01

/* Service ID for Port_RefreshPortDirection */
#define PORT_REFRESHPORTDIRECTION_SID	(uint8)0x02

/* Service ID for Port_GetVersionInfo */
#define PORT_GETVERSIONINFO_SID			(uint8)0x03

/* Service ID for Port_SetPinMode */
#define PORT_SETPINMODE_SID				(uint8)0x04


/*******************************************************************************
 *                      DET Error Codes                                        *
 *******************************************************************************/

/* DET code to report Invalid Port Pin ID requested */
#define PORT_E_PARAM_PIN				(uint8)0x0A

/* DET code to report Port Pin not configured as changeable */
#define PORT_E_DIRECTION_UNCHANGEABLE	(uint8)0x0B

/* API Port_Init service called with wrong parameter */
#define PORT_E_PARAM_CONFIG				(uint8)0x0C

/* API Port_SetPinMode service called when mode is unchangeable */
#define PORT_E_PARAM_INVALID_MODE 		(uint8)0x0D
#define PORT_E_MODE_UNCHANGEABLE		(uint8)0x0E


/* API service called without module initialization */
#define PORT_E_UNINIT					(uint8)0x0F

/* APIs called with a Null Pointer */
#define PORT_E_PARAM_POINTER			(uint8)0x10


/*******************************************************************************
 *                              Module Data Types                              *
 *******************************************************************************/

/* Type definition for the symbolic name of a port */
typedef uint8	Port_PortType;

/* Type definition for the symbolic name of a port pin */
typedef uint8	Port_PinType;

/* Type definition for Different port pin modes */
typedef uint8	Port_PinModeType;

/* Enumeration type for possible directions of a port pin */
typedef enum
{
	PORT_PIN_IN, PORT_PIN_OUT
}Port_PinDirection;

/* Enumeration type to hold internal resistor type of a port PIN */
typedef enum
{
    PORT_PIN_OFF,PORT_PIN_PULL_UP,PORT_PIN_PULL_DOWN
}Port_InternalResistor;



/* Structure Type for type of the external data structure containing the initialization data for this module */
typedef struct
{
	Port_PortType port_Num;
	Port_PinType  pin_Num;
	Port_PinDirection pin_Direction;
	Port_InternalResistor internal_Resistor;
	uint8 pin_Init_Value;
	Port_PinModeType pin_Mode;
	boolean pin_Direction_Changeable;
	boolean pin_Mode_Changeable;
}Port_ConfigPin;

/* Data Structure required for initializing the port Driver */
typedef struct
{
	Port_ConfigPin Pins[PORT_CONFIGURED_PINS];
}Port_ConfigType;


/* Description: Enum to hold PIN direction */
typedef enum
{
    INPUT,OUTPUT
}Port_PinDirection2;

/* Description: Enum to hold internal resistor type for PIN */
typedef enum
{
    OFF,PULL_UP,PULL_DOWN
}Port_InternalResistor2;

/* Description: Structure to configure each individual PIN:
 *	1. the PORT Which the pin belongs to. 0, 1, 2, 3, 4 or 5
 *	2. the number of the pin in the PORT.
 *      3. the direction of pin --> INPUT or OUTPUT
 *      4. the internal resistor --> Disable, Pull up or Pull down
 */
typedef struct 
{
    uint8 port_num; 
    uint8 pin_num; 
    Port_PinDirection2 direction;
    Port_InternalResistor2 resistor;
    uint8 initial_value;
}Port_ConfigType2;


/*******************************************************************************
 *                      Function Prototypes                                    *
 *******************************************************************************/

/* Function for PORT Initialization API */
void Port_Init(const Port_ConfigType* ConfigPtr);

/* Function for PORT setup PIN direction */
void Port_SetPinDirection(Port_PinType Pin, Port_PinDirection Direction);

/* Function for PORT refresh port direction */
void Port_RefreshPortDirection(void);

/* Function for PORT Get Version Info API */
#if (PORT_VERSION_INFO_API == STD_ON)
void Port_GetVersionInfo(Std_VersionInfoType *versioninfo);
#endif

/* Function for PORT setup PIN mode */
void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode);


/*******************************************************************************
 *                       External Variables                                    *
 *******************************************************************************/

/* Extern PB structures to be used by Port and other modules */
extern const Port_ConfigType Port_Configuration;


#endif /* PORT_H */
