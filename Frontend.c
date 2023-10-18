/*
Final Project - Front End Section
Mengxi Jiang(Tracy)
Known Bugs: None
Input Device: keyboard and touch screen
Output Device: raspberry pi touchscreen and i2c lcd panel
*/

#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
//for i2c lcd
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <string.h>


//Setting up lcd
int LCDAddr = 0x27;
int BLEN = 1;
int fd;

void write_word(int data);
void send_command(int comm);
void send_data(int data);
void init();
void clear();
void write(int x, int y, char data[]);
unsigned int enterChoice(void);
void display_daily_min_temp(unsigned int day);
void display_daily_max_temp(unsigned int day);
void display_daily_min_humidity(unsigned int day);
void display_daily_max_humidity(unsigned int day);
void display_avg_temp(unsigned int day, unsigned int from_hour, unsigned int to_hour);
void display_avg_humidity(unsigned int day, unsigned int from_hour, unsigned int to_hour);

int main()
{
    unsigned int choice;
    unsigned int date;
    unsigned int from_hour;
    unsigned int to_hour;

    //Initialize the lcd panel
    //Display a welcome message
    init();
    fd = wiringPiI2CSetup(LCDAddr);
    clear();
    write(0, 0, "Welcome");
    write(0, 1, "Ready for query");

    /*menu driven user query
    user can query the following information
    Data (May 3, 4, 5 and 6)
    1. Daily min temp (user chosen date)
    2. Daily max temp (user chosen date)
    3. Daily min humidity (user chosen date)
    4. Daily max humidity ( user chosen date)
    5. Average temp (user chosen day and time)
    6. Average humidity (user chosen day and time)
    7. End the program
    */
    while((choice = enterChoice()) != 7)
    {
        switch (choice)
        {
        // Output daily minimum temperature
        case 1:
            clear();
            puts("Which date's minimum temperature would you like to view: ");
            puts("Please select from 3, 4, 5, or 6");
            scanf("%d", &date);
            display_daily_min_temp(date);
            break;
        // Output daily maximum temperature
        case 2:
            clear();
            puts("Which date's maximum temperature would you like to view: ");
            puts("Please select from 3, 4, 5, or 6");
            scanf("%d", &date);
            display_daily_max_temp(date);
            break;
        // Output daily minimum humidity
        case 3:
            clear();
            puts("Which date's minimum humidity would you like to view: ");
            puts("Please select from 3, 4, 5, or 6");
            scanf("%d", &date);
            display_daily_min_humidity(date);
            break;
        // Output daily maximum humidity
        case 4:
            clear();
            puts("Which date's maximum humidity would you like to view: ");
            puts("Please select from 3, 4, 5, or 6");
            scanf("%d", &date);
            display_daily_max_humidity(date);
            break;
        //Display the average temperature of the user input time frame
        case 5:
            clear();
            puts("Which date's average temperature would you like to view: ");
            puts("Please select from 3, 4, 5, or 6");
            scanf("%d", &date);
            puts("From what hour: " );
            scanf("%d", &from_hour);
            puts("To what hour: " );
            scanf("%d", &to_hour);
            display_avg_temp(date, from_hour, to_hour);
            break;
        case 6:
        //Display the average humidity of the user input time frame
            clear();
            puts("Which date's average humidity would you like to view: ");
            puts("Please select from 3, 4, 5, or 6");
            scanf("%d", &date);
            puts("From what hour: " );
            scanf("%d", &from_hour);
            puts("To what hour: " );
            scanf("%d", &to_hour);
            display_avg_humidity(date, from_hour, to_hour);
            break;
        // display message if user does not select valid choice
        default:
            clear();
            puts("Incorrect choice");
            break;
        }
    }

    //Display an end of the program message
    clear();
    write(0, 0, "Have a");
    write(0, 1, "nice day!");

    return 0;
}

unsigned int enterChoice(void)
{
    // display available options
    printf("%s", "\nEnter your choice\n"
           "1 - Daily minimum temperature\n"
           "2 - Daily maximum temperature\n"
           "3 - Daily minimum humidity\n"
           "4 - Daily maximum humidity\n"
           "5 - Average temperature of user input time frame\n"
           "6 - Average humidity of user input time frame\n"
           "7 - end program\n? ");
    unsigned int menuChoice;  // variable to store user's choice
    scanf("%u", &menuChoice); // receive choice from user
    return menuChoice;
}

void display_daily_min_temp(unsigned int date)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    /* database credential */
    char *server = "localhost";
    char *user = "tracy";
    char *password = "password";
    char *database = "temphumdb";
    conn = mysql_init(NULL);

    char query[100];
    char user_input_date[20];
    sprintf(user_input_date, "May %d", date);
    sprintf(query,"select MIN(temp) from thdata where time >= '2023-05-0%d 00:00:00' AND time <= '2023-05-0%d 23:59:59'", date, date);

    /* Connect to database */
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    /* send SQL query */
    if (mysql_query(conn, query))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    res = mysql_use_result(conn);
    write(0, 0, "Min temp on");
    write(0, 1, user_input_date);
    delay(3000);
    clear();
    while ((row = mysql_fetch_row(res)) != NULL)
    {
        write(0, 0, "in Celsius is");
        write(0, 1, row[0]);
        printf("%s \n", row[0]);
    }
    delay(4000);

    /* close connection */
    mysql_free_result(res);
    mysql_close(conn);

}

void display_daily_max_temp(unsigned int date)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    /* database credential */
    char *server = "localhost";
    char *user = "tracy";
    char *password = "password";
    char *database = "temphumdb";
    conn = mysql_init(NULL);

    char query[100];
    char user_input_date[20];
    sprintf(user_input_date, "May %d", date);
    sprintf(query,"select MAX(temp) from thdata where time >= '2023-05-0%d 00:00:00' AND time <= '2023-05-0%d 23:59:59'", date, date);

    /* Connect to database */
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    /* send SQL query */
    if (mysql_query(conn, query))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    res = mysql_use_result(conn);
    fd = wiringPiI2CSetup(LCDAddr);
    write(0, 0, "Max temp on");
    write(0, 1, user_input_date);
    delay(3000);
    clear();
    while ((row = mysql_fetch_row(res)) != NULL)
    {
        write(0, 0, "in Celsius is");
        write(0, 1, row[0]);
        printf("%s \n", row[0]);
    }
    delay(4000);
    clear();

    /* close connection */
    mysql_free_result(res);
    mysql_close(conn);
}

void display_daily_min_humidity(unsigned int date)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    /* database credential */
    char *server = "localhost";
    char *user = "tracy";
    char *password = "password";
    char *database = "temphumdb";
    conn = mysql_init(NULL);

    char query[100];
    char user_input_date[20];
    sprintf(user_input_date, "May %d", date);
    sprintf(query,"select MIN(hum) from thdata where time >= '2023-05-0%d 00:00:00' AND time <= '2023-05-0%d 23:59:59'", date, date);

    /* Connect to database */
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    /* send SQL query */
    if (mysql_query(conn, query))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    res = mysql_use_result(conn);

    write(0, 0, "Min humidity on");
    write(0, 1, user_input_date);
    delay(3000);
    clear();
    while ((row = mysql_fetch_row(res)) != NULL)
    {
        write(0, 0, "is");
        write(3, 0, row[0]);
        write(8, 0, "%");
        printf("%s \n", row[0]);
    }
    delay(4000);
    clear();

    /* close connection */
    mysql_free_result(res);
    mysql_close(conn);

}

void display_daily_max_humidity(unsigned int date)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    /* database credential */
    char *server = "localhost";
    char *user = "tracy";
    char *password = "password";
    char *database = "temphumdb";
    conn = mysql_init(NULL);

    char query[100];
    char user_input_date[20];
    sprintf(user_input_date, "May %d", date);
    sprintf(query,"select MAX(hum) from thdata where time >= '2023-05-0%d 00:00:00' AND time <= '2023-05-0%d 23:59:59'", date, date);

    /* Connect to database */
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    /* send SQL query */
    if (mysql_query(conn, query))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    res = mysql_use_result(conn);

    write(0, 0, "Max humidity on");
    write(0, 1, user_input_date);
    delay(3000);
    clear();
    while ((row = mysql_fetch_row(res)) != NULL)
    {
        write(0, 0, "is");
        write(3, 0, row[0]);
        write(8, 0, "%");
        printf("%s \n", row[0]);
    }
    delay(4000);
    clear();

    /* close connection */
    mysql_free_result(res);
    mysql_close(conn);

}

void display_avg_temp(unsigned int day, unsigned int from_hour, unsigned int to_hour)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    /* database credential */
    char *server = "localhost";
    char *user = "tracy";
    char *password = "password";
    char *database = "temphumdb";
    conn = mysql_init(NULL);

    char query[100];
    char from_time[30];
    char to_time[30];
    char user_input_date[20];
    char from_time_lcd[30];
    char to_time_lcd[30];

    sprintf(user_input_date, "May %d", day);
    sprintf(from_time_lcd, "%d O'Clock", from_hour);
    sprintf(to_time_lcd, "%d O'Clock", to_hour);

    if(from_hour < 10)
    {
        sprintf(from_time, "'2023-05-0%d 0%d-00-00'", day, from_hour);
    }

    else
        sprintf(from_time, "'2023-05-0%d %d-00-00'", day, from_hour);

    if(to_hour < 10)
        sprintf(to_time, "'2023-05-0%d 0%d-00-00'", day, to_hour);
    else
        sprintf(to_time, "'2023-05-0%d %d-00-00'", day, to_hour);


    sprintf(query,"select AVG(temp) from thdata where time >= %s AND time <= %s", from_time, to_time);

    /* Connect to database */
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    /* send SQL query */
    if (mysql_query(conn, query))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    write(0, 0, "Avg temp on");
    write(0, 1, user_input_date);
    delay(3000);
    clear();
    write(0, 0, "From");
    write(0, 1, from_time_lcd);
    delay(3000);
    clear();
    write(0, 0, "To");
    write(0, 1, to_time_lcd);
    delay(3000);
    clear();
    res = mysql_use_result(conn);
    while ((row = mysql_fetch_row(res)) != NULL)
    {
        double temp[6];
        sprintf(temp, "%.5s", row[0]);
        write(0, 0, "in Celsiu is");
        write(0, 1, temp);

        printf("%s", "The average temperature in Celcius of user input time frame: \n");
        printf("%.5s \n", row[0]);

    }
    delay(4000);
    clear();


    /* close connection */
    mysql_free_result(res);
    mysql_close(conn);

}

void display_avg_humidity(unsigned int day, unsigned int from_hour, unsigned int to_hour)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    /* database credential */
    char *server = "localhost";
    char *user = "tracy";
    char *password = "password";
    char *database = "temphumdb";
    conn = mysql_init(NULL);

    char query[100];
    char from_time[30];
    char to_time[30];
    char user_input_date[20];
    char from_time_lcd[30];
    char to_time_lcd[30];

    sprintf(user_input_date, "May %d", day);
    sprintf(from_time_lcd, "%d O'Clock", from_hour);
    sprintf(to_time_lcd, "%d O'Clock", to_hour);

    if(from_hour < 10)
        sprintf(from_time, "'2023-05-0%d 0%d-00-00'", day, from_hour);
    else
        sprintf(from_time, "'2023-05-0%d %d-00-00'", day, from_hour);

    if(to_hour < 10)
        sprintf(to_time, "'2023-05-0%d 0%d-00-00'", day, to_hour);
    else
        sprintf(to_time, "'2023-05-0%d %d-00-00'", day, to_hour);


    sprintf(query,"select AVG(hum) from thdata where time >= %s AND time <= %s", from_time, to_time);

    /* Connect to database */
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    /* send SQL query */
    if (mysql_query(conn, query))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }


    write(0, 0, "Avg humidity on");
    write(0, 1, user_input_date);
    delay(3000);
    clear();
    write(0, 0, "From");
    write(0, 1, from_time_lcd);
    delay(3000);
    clear();
    write(0, 0, "To");
    write(0, 1, to_time_lcd);
    delay(3000);
    clear();
    res = mysql_use_result(conn);
    while ((row = mysql_fetch_row(res)) != NULL)
    {
        double humidity[6];
        sprintf(humidity, "%.5s", row[0]);
        write(0, 0, "is");
        write(3, 0, humidity);
        write(9, 0, "%");
        printf("%s", "The average humidity of user input time frame: \n");
        printf("%.5s \n", row[0]);

    }
    delay(4000);
    clear();


    /* close connection */
    mysql_free_result(res);
    mysql_close(conn);

}

void write_word(int data)
{
    int temp = data;
    if ( BLEN == 1 )
        temp |= 0x08;
    else
        temp &= 0xF7;
    wiringPiI2CWrite(fd, temp);
}

void send_command(int comm)
{
    int buf;
    // Send bit7-4 firstly
    buf = comm & 0xF0;
    buf |= 0x04;			// RS = 0, RW = 0, EN = 1
    write_word(buf);
    delay(2);
    buf &= 0xFB;			// Make EN = 0
    write_word(buf);

    // Send bit3-0 secondly
    buf = (comm & 0x0F) << 4;
    buf |= 0x04;			// RS = 0, RW = 0, EN = 1
    write_word(buf);
    delay(2);
    buf &= 0xFB;			// Make EN = 0
    write_word(buf);
}

void send_data(int data)
{
    int buf;
    // Send bit7-4 firstly
    buf = data & 0xF0;
    buf |= 0x05;			// RS = 1, RW = 0, EN = 1
    write_word(buf);
    delay(2);
    buf &= 0xFB;			// Make EN = 0
    write_word(buf);

    // Send bit3-0 secondly
    buf = (data & 0x0F) << 4;
    buf |= 0x05;			// RS = 1, RW = 0, EN = 1
    write_word(buf);
    delay(2);
    buf &= 0xFB;			// Make EN = 0
    write_word(buf);
}

void init()
{
    send_command(0x33);	// Must initialize to 8-line mode at first
    delay(5);
    send_command(0x32);	// Then initialize to 4-line mode
    delay(5);
    send_command(0x28);	// 2 Lines & 5*7 dots
    delay(5);
    send_command(0x0C);	// Enable display without cursor
    delay(5);
    send_command(0x01);	// Clear Screen
    wiringPiI2CWrite(fd, 0x08);
}

void clear()
{
    send_command(0x01);	//clear Screen
}

void write(int x, int y, char data[])
{
    int addr, i;
    int tmp;
    if (x < 0)  x = 0;
    if (x > 15) x = 15;
    if (y < 0)  y = 0;
    if (y > 1)  y = 1;

    // Move cursor
    addr = 0x80 + 0x40 * y + x;
    send_command(addr);

    tmp = strlen(data);
    for (i = 0; i < tmp; i++)
    {
        send_data(data[i]);
    }
}


