#include "stdio.h"
#include "ezdsp5535.h"

extern Int16 led_test( );

int  TestFail    = (int)-1;

void StopTest()
{
    //SW_BREAKPOINT;
    return;
}

/*
 *
 *  main( )
 *
 */
void main( void )
{
    /* Initialize BSL */
    EZDSP5535_init( );

    /* Display test ID */
    printf( "\nTesting LEDs...\n");
    StopTest();
}
