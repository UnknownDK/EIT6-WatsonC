/*  ============================================================================
 *   Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2008
 *
 *   Use of this software is controlled by the terms and conditions found in the
 *   license agreement under which this software has been supplied.
 *  ============================================================================
 */

/** @file csl_lcdc_textDisplayExample.c
 *
 *  @brief LCDC test code to display text on the LCD panel
 *
 *
 * \page    page8  LCD EXAMPLE DOCUMENTATION
 *
 * \section LCD4   LCD EXAMPLE4 - TEXT DISPLAY TEST
 *
 * \subsection LCD4x    TEST DESCRIPTION:
 *      This test code demonstrates displaying a string of characters on the
 * LCD panel. Different text is displayed on the LCD panel in two steps.
 *
 * In the first step two character strings are displayed on the LCD panel
 * using DMA mode with text in White color. LCD panel is divided into two
 * vertical parts to display the strings in separate planes.
 * String "IMAGINATION IS BEGINNING OF CREATION.YOU IMAGINE WHAT YOU DESIRE"
 * will be displayed on the left half of the LCD.
 * String "YOU WILL WHAT YOU IMAGINE AND AT LAST YOU CREATE WHAT YOU WILL"
 * will be displayed on the right half of the LCD.
 * This two strings will be displayed for few seconds and then text will be
 * cleared line by line. During this step text displayed will be smaller
 * in size as the LCD panel is divided into two parts.
 *
 * In the second step one character string will be displayed on the LCD panel
 * using polled mode with text in Red color.
 * String "CSL FOR TEXAS INSTRUMENTS C5505(C5515) LCD MODULE" will be displayed
 * on complete plane of the LCD. During this step text displayed will be bigger
 * in size and more clear.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5505 AND
 * C5515. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection LCD4y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5505/C5515 EVM)
 *  @li Open the project "CSL_LCDC_TextDisplayExample_Out.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at PLL frequencies 40, 60, 75 and 100MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection LCD4z    TEST RESULT:
 *  @li All the CSL APIs should return success.
 *  @li String "IMAGINATION IS BEGINNING OF CREATION.YOU IMAGINE WHAT YOU DESIRE"
 *      should be displayed on the left half of the LCD. String "YOU WILL WHAT
 *      YOU IMAGINE AND AT LAST YOU CREATE WHAT YOU WILL" will be displayed on
 *      the right half of the LCD. The strings should be cleared line by line
 *      after few seconds.
 *  @li String "CSL FOR TEXAS INSTRUMENTS C5505(C5515) LCD MODULE" should be displayed
 *      on the LCD panel with text in Red color.
 *
 * =============================================================================
 */

/* ============================================================================
 * Revision History
 * ================
 * 24-Sep-2008 Created
 * 08-Jul-2009 Modified the test and added Documentation
 * ============================================================================
 */

#include <string.h>
#include <stdio.h>
#include"ezdsp5535.h"
#include"ezdsp5535_lcd.h"


Int16 oled_test();

void main(void)
{
    //EZDSP5535_OSD9616_init( );
    oled_test();


}

Int16 oled_test()
{
    Int16 i;
    Uint16 cmd[10];    // For multibyte commands

    /* Initialize OLED display */
    EZDSP5535_OSD9616_init( );

    EZDSP5535_OSD9616_send(0x00,0x81);//send til kontrast
    EZDSP5535_OSD9616_send(0x00,0x01);//kontrast vaerdi

    EZDSP5535_OSD9616_send(0x00,0x2e);  // Deactivate Scrolling


    /* Fill page 0 */
    EZDSP5535_OSD9616_send(0x00,0x00);   // Set low column address
    EZDSP5535_OSD9616_send(0x00,0x10);   // Set high column address
    EZDSP5535_OSD9616_send(0x00,0xb0+0); // Set page for page 0 to page 5
    for(i=0;i<128;i++)
    {
        EZDSP5535_OSD9616_send(0x40,0xff);
    }
    /* Write to page 0 */
    EZDSP5535_OSD9616_send(0x00,0x00);   // Set low column address
    EZDSP5535_OSD9616_send(0x00,0x10);   // Set high column address
    EZDSP5535_OSD9616_send(0x00,0xb0+0); // Set page for page 0 to page 5
    for(i=0;i<22;i++)
    {
        EZDSP5535_OSD9616_send(0x40,0x00);  // Spaces
    }
    EZDSP5535_OSD9616_printLetter(0x32,0x49,0x49,0x26);  // S
    EZDSP5535_OSD9616_printLetter(0x01,0x7F,0x01,0x01);  // T
    EZDSP5535_OSD9616_printLetter(0x7F,0x30,0x0E,0x7F);  // N
    EZDSP5535_OSD9616_printLetter(0x41,0x49,0x49,0x7F);  // E
    EZDSP5535_OSD9616_printLetter(0x7F,0x06,0x06,0x7F);  // M
    EZDSP5535_OSD9616_printLetter(0x3F,0x40,0x40,0x3F);  // U
    EZDSP5535_OSD9616_printLetter(0x46,0x29,0x19,0x7F);  // R
    EZDSP5535_OSD9616_printLetter(0x01,0x7F,0x01,0x01);  // T
    EZDSP5535_OSD9616_printLetter(0x32,0x49,0x49,0x26);  // S
    EZDSP5535_OSD9616_printLetter(0x7F,0x30,0x0E,0x7F);  // N
    EZDSP5535_OSD9616_printLetter(0x00,0x7F,0x00,0x00);  // I
    for(i=0;i<5;i++)
    {
        EZDSP5535_OSD9616_send(0x40,0x00);  // Spaces
    }
    EZDSP5535_OSD9616_printLetter(0x32,0x49,0x49,0x26);  // S
    EZDSP5535_OSD9616_printLetter(0x7C,0x09,0x0A,0x7C);  // A
    EZDSP5535_OSD9616_printLetter(0x63,0x1C,0x1C,0x63);  // X
    EZDSP5535_OSD9616_printLetter(0x41,0x49,0x49,0x7F);  // E
    EZDSP5535_OSD9616_printLetter(0x01,0x7F,0x01,0x01);  // T

    for(i=0;i<23;i++)
    {
        EZDSP5535_OSD9616_send(0x40,0x00);  // Spaces
    }
    /* Fill page 1*/
    EZDSP5535_OSD9616_send(0x00,0x00);   // Set low column address
    EZDSP5535_OSD9616_send(0x00,0x10);   // Set high column address
    EZDSP5535_OSD9616_send(0x00,0xb0+1); // Set page for page 0 to page 5
    for(i=0;i<128;i++)
    {
        EZDSP5535_OSD9616_send(0x40,0xff);
    }

    /* Write to page 1*/
    EZDSP5535_OSD9616_send(0x00,0x00);   // Set low column address
    EZDSP5535_OSD9616_send(0x00,0x10);   // Set high column address
    EZDSP5535_OSD9616_send(0x00,0xb0+1); // Set page for page 0 to page 5
    for(i=0;i<46;i++)
    {
        EZDSP5535_OSD9616_send(0x40,0x00);
    }

    EZDSP5535_OSD9616_printLetter(0x06,0x09,0x09,0x7F);  // P
    EZDSP5535_OSD9616_printLetter(0x32,0x49,0x49,0x26);  // S
    EZDSP5535_OSD9616_printLetter(0x3E,0x41,0x41,0x7F);  // D
    EZDSP5535_OSD9616_printLetter(0x43,0x4D,0x51,0x61);  // Z
    EZDSP5535_OSD9616_printLetter(0x10,0x58,0x58,0x30);  // e
    for(i=0;i<5;i++)
    {
        EZDSP5535_OSD9616_send(0x40,0x00);  // Spaces
    }
    EZDSP5535_OSD9616_printLetter(0x31,0x49,0x49,0x2F);  // 5
    EZDSP5535_OSD9616_printLetter(0x36,0x49,0x49,0x22);  // 3
    EZDSP5535_OSD9616_printLetter(0x31,0x49,0x49,0x2F);  // 5
    EZDSP5535_OSD9616_printLetter(0x31,0x49,0x49,0x2F);  // 5
    EZDSP5535_OSD9616_printLetter(0x22,0x41,0x41,0x3E);  // C

    for(i=0;i<47;i++)
    {
        EZDSP5535_OSD9616_send(0x40,0x00);       // Spaces
    }

    EZDSP5535_OSD9616_send(0x00,0x2e);  // Deactivate Scrolling

    /* Set vertical and horizontal scrolling */
    cmd[0] = 0x00;
    cmd[1] = 0x29;  // Vertical and Right Horizontal Scroll
    cmd[2] = 0x00;  // Dummy byte
    cmd[3] = 0x00;  // Define start page address
    cmd[4] = 0x03;  // Set time interval between each scroll step
    cmd[5] = 0x01;  // Define end page address
    cmd[6] = 0x01;  // Vertical scrolling offset
    EZDSP5535_OSD9616_multiSend( cmd, 7 );
    EZDSP5535_OSD9616_send(0x00,0x2f);

    /* Keep first 8 rows from vertical scrolling  */
    cmd[0] = 0x00;
    cmd[1] = 0xa3;  // Set Vertical Scroll Area
    cmd[2] = 0x08;  // Set No. of rows in top fixed area
    cmd[3] = 0x08;  // Set No. of rows in scroll area
    EZDSP5535_OSD9616_multiSend( cmd, 4 );

    return 0;
}
