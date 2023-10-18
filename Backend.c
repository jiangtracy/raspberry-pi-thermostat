/*
Final Project - Back End Section, gathering data
Data Range: May 3, 4, 5, 6
Mengxi Jiang(Tracy)
Known Bugs: None
Input Device: keyboard and touch screen
Output Device: raspberry pi touchscrren and i2c lcd panel
*/


#include <mysql/mysql.h>
#include <stdio.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#define MAXTIMINGS 85
#define DHTPIN 7


void insert_query(char[]);
int dht11_dat[5] = { 0, 0, 0, 0, 0 };
void read_dht11_dat()
{

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char datetime[20];
    char query[100];

    uint8_t laststate = HIGH;
    uint8_t counter = 0;
    uint8_t j = 0, i;
    float f;
    dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;
    pinMode( DHTPIN, OUTPUT );
    digitalWrite( DHTPIN, LOW );
    delay( 18 );
    digitalWrite( DHTPIN, HIGH );
    delayMicroseconds( 40 );
    pinMode( DHTPIN, INPUT );
    for ( i = 0; i < MAXTIMINGS; i++ )
    {
        counter = 0;
        while ( digitalRead( DHTPIN ) == laststate )
        {
            counter++;
            delayMicroseconds( 1 );
            if ( counter == 255 )
            {
                break;
            }
        }
        laststate = digitalRead( DHTPIN );
        if ( counter == 255 )
            break;
        if ( (i >= 4) && (i % 2 == 0) )
        {
            dht11_dat[j / 8] <<= 1;
            if ( counter > 16 )
                dht11_dat[j / 8] |= 1;
            j++;
        }
    }
    if ( (j >= 40) &&
            (dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) )
    {

        strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", tm);

        f = dht11_dat[2] * 9. / 5. + 32;
        printf( "Humidity = %d.%d %% Temperature = %d.%d C (%.1f F)\n",
                dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3], f );


        sprintf(query, "insert into thdata (hum, temp, time) values (%d.%d, %d.%d, '%s')", dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3], datetime);
        insert_query(query);

    }
    else
    {
        printf( "Data not good, skip\n" );
    }



}

void insert_query(char string[])
{

    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    /* Database info */
    char *server = "localhost";
    char *user = "tracy";
    char *password = "password";
    char *database = "temphumdb";
    conn = mysql_init(NULL);
    /* Connect to database */
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    if (mysql_query(conn, string))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    /* close connection */
    mysql_free_result(res);
    mysql_close(conn);

}

int main( void )
{

    printf( "Raspberry Pi wiringPi DHT11 Temperature test program\n" );
    if ( wiringPiSetup() == -1 )
        exit( 1 );
    while ( 1 )
    {
        read_dht11_dat();
        delay(2000);
    }


    return(0);
}

