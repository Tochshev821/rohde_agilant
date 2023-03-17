
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
/* hton, ntoh и проч. */
#include <arpa/inet.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
/////////////////////
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <time.h>
//#include <bitset>
#include <math.h>
//using std::bitset;
//#include <lcd.h>
#include <lcd.h>
/////////////////////

//add libs for modbus

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
/* hton, ntoh и проч. */
#include <arpa/inet.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
/////////////////////
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <time.h>
//#include <bitset>
#include <math.h>
//using std::bitset;
//#include <lcd.h>
#include <lcd.h>



////
#include <stdio.h>
#include <modbus.h>
#include <errno.h>





/// for  getting and saving package
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#define GET_PIN 26;



/*
#define LCD_RS  19
#define LCD_E   26
#define LCD_D4  12
#define LCD_D5  16
#define LCD_D6  20
#define LCD_D7  21
#define encoder1A  13 // encoder 1
#define encoder1B  6 // encoder 1
#define encoder2A  2 // encoder 2
#define encoder2B  3 // encoder 2

#define button_1  4   //GPIO
#define button_2  17  //GPIO
*/

#define LCD_RS  21//19
#define LCD_E   20//26
#define LCD_D4  16//12
#define LCD_D5  12//16
#define LCD_D6  1//20
#define LCD_D7  0//21 //25
#define encoder1A  13 // encoder 1
#define encoder1B  6 // encoder 1
#define encoder2A  17 // encoder 2
#define encoder2B  27 // encoder 2

//#define button_1  4   //GPIO
//#define button_2  17  //GPIO

#define false 1
#define true 0
char a1;
char b1;
char a2;
char b2;
void rotate_speed(int id_driver,int speed);
void rotate_speed1(int id_driver,int speed,int encoder_ratio);

char state1 = 0;
char state2 = 0;
 int initt=0;
volatile int counter = 0;

void interrupt1();
void interrupt2();
void interrupt_butt();

int s;
int nbytes;
int offset_engine;//smeshenie
struct sockaddr_can addr;
struct ifreq ifr;
struct can_frame frame;
int rele_on_flag;

int flcd;                // Handle for LCD
float res;
char grad[4];
char grad2[4];
char str[10];

char show_ip[];
char bufer_str1[14]; // first string fill with zagolovoc
char bufer_str2[14]; // second string fiil with info
char bufer_str3[14]; // third string fiil with info
char str_for_connect[15] ; // this char using for take str from scan port which is availible for connecting
int port[5] = {5025,18,1,2,3};
//char str_for_connect_gen[15] ; // another one for anapiko

void delay2(int number_of_seconds)
{
    int milli_seconds = 1000 * number_of_seconds;
    clock_t start_time = clock();
    while(clock()<start_time+milli_seconds);
}



////
/////
////
///
////FUNCTIONS FOR MODBUS
int flag=0;

// setup Pins high or low and set timing between this depend on how switching works
int return_something(modbus_t *ctx, int off_maybe)
{


    if(off_maybe ==1)
    {
usleep(1500);
    digitalWrite (26, HIGH) ;
    printf(" turn high  %d", 0);
    }

    if(off_maybe ==0)
    {
usleep(700);
    digitalWrite (26, LOW) ;
    printf(" turn low  %d", 1);
    }
   // modbus_rtu_set_rts(ctx, MODBUS_RTU_RTS_UP);

    return 0;
}

//func convert numb from modbus to hours
int numb_to_time_converter(int ugol)
{
    int hours = ugol/200/50;
    float ugol_2 = (float)ugol/200/50;
    float min = (ugol_2 - (float)hours)*60;
    int min2 = (int)min;
    float sec = (min - (float)min2)*60;
//printf("HOURS with coma= %f\n", ugol_2);
    printf("HOURS = %d\n", hours);
    printf("MINS = %d\n", (int)min);
    printf("MINS = %d\n", (int)sec);

    }

// funciton for concat number
void concat_num(uint8_t rsp[])
{

    printf("Counter: %p%p%p%p%p%p", rsp[9],rsp[10],rsp[7],rsp[8],rsp[5],rsp[6]);

    int ugol = rsp[7]<<24 | rsp[8]<<16 | rsp[5]<<8 | rsp[6];
    printf("INT NUMBER IS= %d\n", ugol);
    numb_to_time_converter(ugol);


}

/////
////
/////
///













// this func gives global variables meaning depend on what we want to show on desplay 1st and 2nd line
void put_info_in_global(char bbufer_str1[12],char bbufer_str2[12],char bbufer_str3[12])
{
        strcpy(bufer_str1,bbufer_str1);
        strcpy(bufer_str2,bbufer_str2);
        strcpy(bufer_str3,bbufer_str3);
}

//func which show on display lines probably about errors or kind of info about ip adress and so on
void show_info_lcd()
{
        //clear display
        lcdClear (flcd);
        //show info from bufers str
        lcdPosition(flcd,0,0);           //Position cursor on the first line in the first column
        lcdPuts(flcd, bufer_str1);
        lcdPosition(flcd,0,1);           //Position cursor on the second line in the first column
        lcdPuts(flcd, bufer_str2);
        lcdPosition(flcd,0,2);           //Position cursor on the second line in the first column
        lcdPuts(flcd, bufer_str3);
        //... wait few second
        delay(2000);
        // then stop showing message
        lcdClear (flcd);
}

////////////////
// func from increment_encoder
void rele_on_start()
{
      int numbed=0x26FF;

        numbed=numbed|(1<<31);
        if(rele_on_flag==false)
        {
                // frame.can_id =frame.can_id & CAN_EFF_MASK;
                frame.can_id = numbed;
                frame.can_dlc = 1;
                frame.len=2;
                frame.data[0] = 0;
                frame.data[1] = 0;
                frame.data[2] = 0;
                frame.data[3] = 0;
                frame.data[4] = 0;
                frame.data[5] = 0;
                frame.data[6] = 0;
                frame.data[7] = 0;
                nbytes = write(s, &frame, sizeof(frame));
                rele_on_flag=1;
                sleep(0.5);
        }//system("sudo ifconfig can1 down");
}

float receive_data_angle(int id_driver) // get angle from engine depends on the id driver
{
        float frame_data;
        nbytes=0;
        nbytes = read(s, &frame, sizeof(frame));
        struct timeval tv;
        ioctl(s,SIOCGSTAMP_OLD,&tv);

        int can_id = 0x900+id_driver;
        //int can_id = 0x9FF;
        int frame_can_id = frame.can_id<<2;
        frame_can_id = frame_can_id>>2;
        if(frame_can_id==can_id)
        {
            if(nbytes > 0) {

                int frame_1 =(frame.data[0]<<8|frame.data[1]);
                frame_data = ((float)frame_1/16383.0*360.0);//1000000;
            //    printf("%f",frame_data);
                 return frame_data;
            }
        }
         return frame_data;
};
void rotate_speed(int id_driver,int speed) // rotate engine using speed didnt work. You must declare
{
    rele_on_start();

    int numbed=0x40<<8 | id_driver;
    numbed=numbed|(1<<31);

   // frame.can_id =frame.can_id & CAN_EFF_MASK;
    frame.can_id = numbed;
    frame.can_dlc = 4;
    frame.len=4;
    int gr = speed;
    gr=(int)(speed);

    frame.data[0] = gr>>24;
    frame.data[1] = gr>>16;
    frame.data[2] = gr>>8;
    frame.data[3] = gr;

    nbytes = write(s, &frame, sizeof(frame));
}
void rotate_speed1(int id_driver,int speed ,int encoder_ratio) // rotate engine using speed didnt work. You must declare
{
        rele_on_start();

        //

        ///int numbed=62 | 15<<8;

        int numbed=0x40<<8 | id_driver;
        //int numbed=0x04FF;
        numbed=numbed|(1<<31);

        // frame.can_id =frame.can_id & CAN_EFF_MASK;
        frame.can_id = numbed;
        frame.can_dlc = 4;
        frame.len=4;
        int gr = speed*encoder_ratio;
        gr=(int)(speed*encoder_ratio);
        frame.data[0] = gr>>24;
        frame.data[1] = gr>>16;
        frame.data[2] = gr>>8;
        frame.data[3] = gr;

        nbytes = write(s, &frame, sizeof(frame));
}

void rotate_angle(int id_driver, int gradus) // function makes engine rotate on angle which you send it as gradus
{
        rele_on_start();

        //int numbed=0x040A;
        int numbed=0x04<<8 | id_driver;
        numbed=numbed|(1<<31);

        // frame.can_id =frame.can_id & CAN_EFF_MASK;
        frame.can_id = numbed;
        frame.can_dlc = 4;
        frame.len=4;
        int gr = 100;
        gr=(int)(gradus*1000000);

        frame.data[0] = gr>>24;
        frame.data[1] = gr>>16;
        frame.data[2] = gr>>8;
        frame.data[3] = gr;
        nbytes = write(s, &frame, sizeof(frame));
};
void canutil()
{

        int ret;
        //int s, nbytes;

        rele_on_flag =1;
        memset(&frame, 0, sizeof(struct can_frame));
        ////
        system("sudo ip link set can1 type can bitrate 1000000");
        system("sudo ifconfig can1 up");
        system("sudo ifconfig can1 txqueuelen 65536");
        printf("this is a can send demo\r\n");//\r\n
        // // //// this two camands make our lcd display value
        put_info_in_global("can is "," configured","");
        show_info_lcd();


        //.Create socket
        s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (s < 0)
        {
            perror("socket PF_CAN failed");
            // //// this two camands make our lcd display value
            put_info_in_global("socket can"," failed","");
            show_info_lcd();
        }
        //2.Specify can0 device
        strcpy(ifr.ifr_name, "can1");
        ret = ioctl(s, SIOCGIFINDEX, &ifr);
        if (ret < 0)
        {
            perror("ioctl failed");
            // //// this two camands make our lcd display value
            put_info_in_global("ioctl can"," failed","");
            show_info_lcd();
        }
        //3.Bind the socket to can0
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;
        ret = bind(s, (struct sockaddr *)&addr, sizeof(addr));
        if (ret < 0)
        {
            perror("bind failed");
            // //// this two camands make our lcd display value
            put_info_in_global("bind can"," failed","");
            show_info_lcd();
        };
   //rele_on_start();
};

void inttomin(float gr, int *grd,int *m,int *s )
{

        float tail;
        float deg_f, min_f, sec_f;

        tail = modff(gr  ,& deg_f)*60.0f;
        tail = modff(tail,& min_f)*60.0f;
        tail = modff(tail,& sec_f)*60.0f;

        tail = min_f;
        if (tail < 0)  min_f = -1.0f* min_f; else;//abs
        tail = sec_f;
        if (tail < 0)  sec_f = -1.0f* sec_f; else;//abs

        *grd = (int) deg_f;
        *m   = (int) min_f;
        *s   = (int) sec_f;


}

float offset_angel ;

static const int rowOff [4] = { 0x00, 0x40, 0x10, 0x50 } ;


void interrupt1()
{
  a1 = digitalRead(encoder1A);
  b1 = digitalRead(encoder1B);
  if (a1!=state1)
  {
    if (a1!=b1)
    {
      counter-- ;
    }
    else
    {
      counter++;
    }
    state1 = a1;
  }
}
void interrupt2()
{
  a2 = digitalRead(encoder2A);
  b2 = digitalRead(encoder2B);
  if (a2!=state2)
  {
    if (a2!=b2)
    {
      counter-- ;
    }
    else
    {
      counter++;
    }
    state2 = a2;
  }

}

void interrupt_butt(void)
{
        offset_angel = receive_data_angle(10);
        printf("%f\n",offset_angel);
}

////////////////////

// this is variables for thread multimetr/generator
char buffer[1024];
float oldpower=0;
float pset;
float volt,power;
pthread_t tid;
/////////////////
pthread_t tid2;

// check info for vagilent
void vagilant_check(int s,int result)
{
        delay(300);
        char buf0[] = "*idn?\n";
        result = send( s, "*idn?\n", 8, 0);
}
// read info from display agilant
void read_info_from_agilant(int s,int result)
{
        delay(300);
        char buf0[] = "read?\n";
        result = send( s, buf0, sizeof(buf0), 0);
}
//reset for vagilant
void reset_vagilant(int s,int result)
{
        delay(300);
        char buf0[] = "*rst\n";
        result = send( s, "*rst\n", 8, 0);
}

// initialization for vaglant multimetr
void first_initialization(int s,int result)// check number of vagilant
{

        char buf1[] = "*rst\n";
        result = send( s, "*rst\n", 6, 0);
        delay(300);
        char buf2[] = "meas:volt:ac?\n";
        result = send( s, "meas:volt:ac?\n", 15, 0);
        delay(300);
        char buf3[] = "sens:volt:ac:band max\n";
        result = send( s, "sens:volt:ac:band max\n", 23, 0);
        delay(300);
        // char buf4[] = "trig:sour ext\n";
        // result = send( s, "trig:sour ext\n", 15, 0);
        //delay(1);
        char buf5[] = "samp:coun 5\n";
        result = send( s, "samp:coun 5\n", 12, 0);

}

//func for getchar buf5[] = "SOUR:POWER 5\n"; // SOUR:AM:STAT ON //turn on AM

void smb100a_check(int s,int result)
{
        delay(300);
        char buf0[] = "*idn?\n";
        result = send( s, "*idn?\n", 8, 0);
}

//func for configure rohdeSwarz
void smb100a_initialization(int s)
{
        // write this what we need
        char buffer[1024];
        char buf1[] = "*rst\n";
        int   result = send( s, "*rst\n", 6, 0);
        delay(300);
        char  buf2[] = "SOUR:AM:STAT ON\n"; // SOUR:AM:STAT ON //turn on AM
        delay(300);
        send( s, "SOUR:AM:STAT ON\n", 15, 0);
        delay(300);
        char buf3[] = "SOUR:AM:DEPT 95\n"; // SOUR:AM:DEPT 95 //set depth
        send( s, "SOUR:AM:DEPT 95\n", 18, 0);
        char buf4[] = "SOUR:AM:INT:FREQ 1000\n"; // SOUR:AM:INT:FREQ 1000/// set frequency
        send( s, "SOUR:AM:INT:FREQ 1000\n", 24, 0);
}

// func for reducing freq
void smb100a_freq(int s,int result)
{
        delay(300);
        char str[3];
        sprintf(str, "%d", result);
        char buf2[20] = "SOUR:FREQ ";
        strcat(buf2,str);
        strcat(buf2,"GHZ\n");
        printf("%s",buf2);
        result = send( s,buf2, sizeof(buf2), 0);
}
// func for getting info about current frequency
float smb100a_current_freq()
{

}

// func for reducing/rising  power
float smb100a_power_read(int s)
{
        char buffer[1024];
        char buf2[] = "SOUR:POWER:AMPL?\n"; //("define power comand %f\n" ,change_power)
        send( s, buf2, sizeof(buf2), 0);
        delay(200);
        recv(s, buffer, 1024, 0);
        // printf("==>%s",buffer);
        // delay(300);

        float myfl=-1;
        myfl = atof(buffer); // explicit type casting
        return myfl;
}

void smb100a_power_set(int s,float result)
{

        char str[20];
        sprintf(str, "%f", result);
        char buf2[30] = "SOUR:POWER ";
        strcat(buf2,str);
        strcat(buf2,"\n");
        send( s, buf2, sizeof(buf2), 0);

}

// func for turn on\off
void smb100a_on_off(int s,int result)
{
        delay(300);
        char buf2[] = "............\n";
        result = send( s, "............\n", 15, 0);
}

int connect_adrs_port(int s, int port, char addres[])
{
        //printf("z\n");
        //printf("%d\n",initt);
        int result;
        if(initt<1)
        {
                //initt++;
                struct sockaddr_in peer;
                peer.sin_family = AF_INET;
                peer.sin_port = htons( port );
                peer.sin_addr.s_addr = inet_addr( addres );
                inet_pton(AF_INET, addres, &peer.sin_addr);
                //printf("%d\n",initt);
                //printf("z\n");
                result = connect(s,(struct sockaddr *)&peer,sizeof(peer));

                printf("%d sdvsdvd\n",result);
        }
        return result;
}

// func read feedback from server
float feedback_agilant_float(int s)
{
        char buffer[1024];
        /* читаем ответ сервера */
        fd_set readmask;
        fd_set allreads;
        // FD_ZERO( &allreads );
        // FD_SET( 0, &allreads );
        //  FD_SET( s, &allreads );
        send( s, "read?\n", 8, 0);      // read from display agilant but there we should read from buffer to get any info  about device
        recv(s, buffer, 1024, 0);
        //   send( s, "*cls\n", 24, 0);
        //char *token = strtok(buffer, " ");
          printf("%s",buffer);
        float myfl = atof(buffer); // explicit type casting

        return myfl;

}
void feedback_agilant_buf(int s)
{
        char buffer[1024];
        /* читаем ответ сервера */
        fd_set readmask;
        fd_set allreads;

        recv(s, buffer, 1024, 0);
        //   send( s, "*cls\n", 24, 0);
        //char *token = strtok(buffer, " ");
        //printf("%s",buffer);
        float myfl = atof(buffer); // explicit type casting
        //  return myfl;
}

// func must scan port and reaalise number of device
// IN PROGRESS
void scan_port()
{
        {
                int s1 = socket( AF_INET, SOCK_STREAM, 0 );
                int sgen = socket( AF_INET, SOCK_STREAM, 0 );
                //char str[15] ="192.168.0.";
                //printf(str);
                //int buf2 ;

                //strcat(buf2,"\n");
                int result;
                int result_gen;
                //printf("scaning\n");

                for(int i=100;i<200;i++)
                {
                        // printf("scaning1\n");

                        sprintf(show_ip,"%d", i);


                        char str[15] ="192.168.0.";
                        //printf("%s\n",str);
                        char str2[3];
                        //printf("scaning\n");
                        sprintf(str2,"%d", i);
                        //printf("%s\n",str2);
                        strcat(str,str2);
                        printf("%s\n",str);


                                result = connect_adrs_port(s1,5025,str);
                                result_gen = connect_adrs_port(sgen,18,str);
                                //printf("scaning3\n");
                                printf("CONNETC IS %d\n",result);
                                if (result==-1 && result_gen==-1)
                                {
                                printf("net pribora\n");
                                put_info_in_global("IP ADR",str, "not this ");
                                show_info_lcd();
                                //return -1;
                                }
                                else
                                {
                                        printf("WE ACTUALLY FIND IT!!!\n");
                                        put_info_in_global("IP ADR",str, "WE FIND IT");
                                        show_info_lcd();
                                        strcpy(str_for_connect,str);
                                        for(int i=0;i<4;i++)
                                        {
                                            result = connect_adrs_port(s,port[i],str);
                                            if(result==1)
                                            {
                                                smb100a_check(sgen,result);
                                                vagilant_check(s1,result);
                                                // then we read buffer and compare it with  name that we have for agilent
                                                // then we
                                            }
                                        }

                                        //return 1;
                                }



                }
                //return 0;
        }
}

void *multimetr_thread(void *vargp) //THREAD FOR MULTIMETR/GENERATOR
{
        //oldpower=0;
        int s1 = socket( AF_INET, SOCK_STREAM, 0 );
        int sgen = socket( AF_INET, SOCK_STREAM, 0 );

        pset=0.2;

        if(s1 < 0)
        {
                perror( "Error calling socket" );
                // put_info_in_global("Error ","calling socket","");
                // show_info_lcd();
                return 0;
        }

        /*соединяемся по определённому порту с хостом agilent multimetr*/

                //cheking ports in file if 5025 or something then connect multimetr
                // if 18 port or kind of then connect generator

                                int result;
                                result = connect_adrs_port(s1,5025,"192.168.0.154");//"192.168.0.154"

                                 if(result==-1)
                                    {
                                        put_info_in_global("Error ","calling socket","");
                                        show_info_lcd();
                                    }

                                first_initialization(s1,result);

                                result = connect_adrs_port(sgen,18,"192.168.0.198");
                                if(result==-1)
                                {       put_info_in_global("Error ","calling socket","");
                                        show_info_lcd();
                                }
                                smb100a_initialization(sgen);

        //printf("22GGGGGGGGGGGGG2\n");
        power=5;
        for(;;)
        {
                volt=feedback_agilant_float(s);
                power=smb100a_power_read(sgen);
                printf("%f\n",volt);
                printf("power=>%f\n",power);
                delay(100);
                if((volt>0.6))
                {
                        power=power-pset;
                }
                if((volt<0.6))
                {
                        power=power+pset;
                }
                if (power>20) power=20;
                if (power<-20) power=-20;
                if(oldpower!=power)
                {
                        smb100a_power_set(sgen,power);
                }
                oldpower=power;
        }

}

void *lcd_thread(void *vargp) //THREAD FOR LCD display
{
        int milli_seconds = 300;
        // Storing start time
        int start_time = millis();


        for(;;)
        {

                float temp,temp2;

                res   =   150.525256f;

                res = res - offset_angel;
                temp2=res;
                temp = res;

                if (temp < 0.0f)
                {
                        temp = temp + 360.0f;
                }
                else;

                if (temp > 180.0f)
                {
                        temp = -1.0f*( -360.0f + temp );
                }
                else
                {
                        temp = -1.0f *temp;
                }
                res=temp;

                int dtime=millis();
                if(dtime > start_time + milli_seconds)
                {
                        start_time = millis();
                        int g1,m1,s1;

                        sprintf(str, "%d", counter);
                        lcdPosition(flcd,0,0);           //Position cursor on the first line in the first column (0,0)
                        lcdPuts(flcd, str);     //Print the text on the LCD at the current cursor postion
                        //receive_data_angle(4);
                        inttomin(res,&g1,&m1,&s1);
                        if (g1<0) {sprintf(grad, "%d",g1);}else {sprintf(grad, " %d",g1);}
                        printf("%4s @ %d @ %d   |\n",grad,m1,s1);

                        sprintf(str, "%4s %d %d   |",grad,m1,s1);
                        lcdPosition(flcd,0,1);           //Position cursor on the first line in the first column (0,0)
                        lcdPuts(flcd, str);     //Print the text on the LCD at the current cursor postion

                        lcdPosition(flcd,4,1);  //Position
                        lcdPutchar(flcd,0x01);  //degree custom simbol record

                        lcdPosition(flcd,7,1);
                        lcdPutchar(flcd,0x02); //minit custom simbol record

                        lcdPosition(flcd,10,1);
                        lcdPutchar(flcd,0x03); //second custom simbol record

                        sprintf(str, "%f",temp2);
                        lcdPosition(flcd,0,2);
                        lcdPuts(flcd, str);     //Print the text on the LCD at the current cursor postion
                        unsigned char str3 =0x01;
                        lcdPutchar(flcd,str3);

                        unsigned char str2[3] ={0x01,0x02,0x03};
                        lcdPosition(flcd,0,3);
                        lcdPuts(flcd, str2);     //Print the text on the LCD at the current cursor postion
                        printf(" xxx\n");

                }
        }
}


int main(int argc, char * argv[])
{

////////////
/////////////////////////////
///////////
//////////
        // from increment_coder
        /*system("gpio export 12 out");
        system("gpio export 16 out");
        system("gpio export 20 out");
        system("gpio export 21 out");
        system("gpio export 19 out");
        system("gpio export 26 out");
        system("gpio export 13 in");
        system("gpio export 6 in");
        system("gpio export 14 in");
        system("gpio export 15 in");
        system("gpio export 4 in");  //button_1
        system("gpio export 17 in"); //button_2
        wiringPiSetupSys();*/


        ////first display

		system("gpio export 21 out");
		system("gpio export 20 out");
        system("gpio export 16 out");
        system("gpio export 12 out");
        system("gpio export 1 out");
        system("gpio export 5 out");//25 or 0
        //for encoders but useless probably
        //system("gpio export 17 out");
        //system("gpio export 27 out");
        //system("gpio export 6 out");
        //system("gpio export 13 out");


        system("gpio export 26 out");

        wiringPiSetupGpio();
//printf(" LOLOLOKek  %d", 0);

        pinMode(encoder1A, INPUT);
        pinMode(encoder1B, INPUT);
        pinMode(encoder2A, INPUT);
        pinMode(encoder2B, INPUT);
//        pinMode( button_1,INPUT);
//        pinMode( button_2,INPUT);

        wiringPiISR(encoder1A, INT_EDGE_BOTH, &interrupt1);
        wiringPiISR(encoder1B, INT_EDGE_BOTH, &interrupt1);
        wiringPiISR(encoder2A, INT_EDGE_BOTH, &interrupt2);
        wiringPiISR(encoder2B, INT_EDGE_BOTH, &interrupt2);
//        wiringPiISR(button_1, INT_EDGE_RISING, &interrupt_butt);


    // DISPLAY
        //// initialize LCD display
        flcd = lcdInit(4, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7,0,0,0,0);
        printf("LCD INIT");
        lcdPosition(flcd, 0, 0);
        lcdPuts(flcd,"HI World");





        unsigned char symb0[8] ={0x06,0x09,0x09,0x06,0x00,0x00,0x00,0x00};
        unsigned char symb1[8] ={0x08,0x08,0x08,0x08,0x00,0x00,0x00,0x00};
        unsigned char symb2[8] ={0x0A,0x0A,0x0A,0x0A,0x00,0x00,0x00,0x00 };

        lcdCharDef(flcd,0x01,symb0);
        lcdCharDef(flcd,0x02,symb1);
        lcdCharDef(flcd,0x03,symb2);

        canutil();


////////////////////
////////////
        ///////////////

        // //// this two camands make our lcd display value
         //put_info_in_global("1","2","3");
         //show_info_lcd();





        //scan_port();
        //printf("%s\n", str_for_connect);
        // start thread for multimetr
        //printf("22GGGGGGGGGGGGG2\n");
        pthread_create(&tid, NULL, multimetr_thread, (void *)&tid);
        //printf("22GGGGGGGGGGGGG2\n");
        //scan_port();
        //printf("end 1\n");
        // start thread for lcd
        pthread_create(&tid2, NULL, lcd_thread, (void *)&tid2);
        printf("end 2\n");



        //// JUST FOR CHECKING ONLY AGILENT
        /////
        //int s1 = socket( AF_INET, SOCK_STREAM, 0 );
        //int sgen = socket( AF_INET, SOCK_STREAM, 0 );
        //int result = connect_adrs_port(s1,5025,"192.168.0.154");//"192.168.0.154"
        //first_initialization(s1,result);
        //int result2 = connect_adrs_port(sgen,18,"192.168.0.198");
        //smb100a_initialization(sgen);

        //vagilant_check( s1, result);
        //float fedback_agilant = feedback_agilant_float(s1);
        //printf("end %f\n",fedback_agilant);
        /////
        //////


 ////// FOR MODBUS
 /////
//fot modbus


while(1)
{
    delay2(5000);
     pinMode (26, OUTPUT) ;
       digitalWrite (26, HIGH);


    const int REMOTE_ID = 5;
    modbus_t *ctx;

    uint16_t tab_reg[64];
    int rc;
    int i;

    ctx = modbus_new_rtu("/dev/ttyUSB0", 115200, 'N', 8, 1);///dev/ttySC1 //ttyUSB0

    if (ctx == NULL) {
        fprintf(stderr, "Unable to create the libmodbus context\n");
        return -1;
    }

    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "AAHHAH ERROR\n");
        modbus_free(ctx);
        return -1;
    };


    //modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS485);
    modbus_rtu_set_rts(ctx, MODBUS_RTU_RTS_UP );

    modbus_rtu_set_custom_rts(ctx, return_something);
    //modbus_set_response_timeout(ctx,3,0);

    //// DEFINE SLAVE
    modbus_set_slave(ctx, REMOTE_ID);

    //modbus_rtu_set_rts_delay(ctx,6);
    uint8_t raw_req[] = { 0x05, MODBUS_FC_READ_INPUT_REGISTERS   , 0x00,0x00,0x00, 0x04 }; //format is important!
    int req_length=-1;
    uint8_t rsp[MODBUS_TCP_MAX_ADU_LENGTH];
    int modbus_condirmation = -1;

    req_length = modbus_send_raw_request(ctx, raw_req, 6 * sizeof(uint8_t)); // format is important
    printf(" req %d ", req_length);

    modbus_condirmation = modbus_receive_confirmation(ctx, rsp);//modbus_receive_confirmation
    printf(" confirm  %d", modbus_condirmation);

    printf(" LENGTH :  %d", req_length);
    for(int i=0;i<modbus_condirmation;i++)
    {
        printf(" RSP : %d ,  %p \n", i+1,rsp[i]);

    }

    concat_num(rsp);

    if(modbus_condirmation==-1)
    {
        fprintf(stderr, "ERROR: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }
       // req_length = modbus_send_raw_request(ctx, raw_req, 6 * sizeof(uint8_t));

    modbus_close(ctx);
    modbus_free(ctx);


}

 //////
 /////
 /////






        //for(;;){printf("");}// probably for mutbus

        //pthread_exit(NULL);

                // for(;;){
                //         volt=feedback_agilant_float(s);
                //         power=smb100a_power_read(sgen);
                //         printf("%f\n",volt);
                //         printf("power=>%f\n",power);
                //         delay(100);
                //         if((volt>0.6))
                //         {
                //                 power=power-pset;
                //         }
                //         if((volt<0.6))
                //         {
                //                 power=power+pset;
                //         }
                //         if (power>20) power=20;
                //         if (power<-20) power=-20;
                //         if(oldpower!=power)
                //         {
                //                 smb100a_power_set(sgen,power);
                //         }
                //         oldpower=power;
                // }

        /*
        float resv=feedback_agilant(s);
        printf("%f\n",resv);

        resv=feedback_agilant(s);
        printf("%f\n",resv);


        // resv=feedback_agilant(s);
        //printf("%f",resv);
        //delay(1000);
        //  resv=feedback_agilant(s);
        //printf("%f",resv);

        for(int i=1;i<=1000;i++){
        resv=feedback_agilant(s);
        printf("%f\n",resv);

        }
        */
        //resv=feedback_agilant(s);
        //printf("%f",resv);


        //       struct sockaddr_in peer;
        //       peer.sin_family = AF_INET;
        //       peer.sin_port = htons( 5025 );
        //       peer.sin_addr.s_addr = inet_addr( "192.168.2.90" );

        //       inet_pton(AF_INET, "192.168.2.90", &peer.sin_addr);
        //       int result = connect(s,(struct sockaddr *)&peer,sizeof(peer));

        //int result = connect( s, ( struct sockaddr * )&peer.sin_addr.s_addr, sizeof( peer ) );


        /*объявляем сокет for new device*/
        //       int s2 = socket( AF_INET, SOCK_STREAM, 0 );
        //       if(s2 < 0)
        //       {
        //               perror( "Error calling socket" );
        //               return 0;
        //       }
        //             /*соединяемся по определённому порту с хостом new device*/
        //       struct sockaddr_in peer2;
        //       peer2.sin_family = AF_INET;
        //       peer2.sin_port = htons( 5025 );
        //       peer2.sin_addr.s_addr = inet_addr( "192.168.2.93" );

        //       inet_pton(AF_INET, "192.168.2.93", &peer2.sin_addr);
        //       int result2 =    connect(s,(struct sockaddr *)&peer2,sizeof(peer2));

        //       if( result )
        //       {
        //               perror( "Error calling connect" );
        //               return 0;
        //       }



                //delay(500);
        //       char buf[] = "read?\n";
        //       result = send( s, "read?\n", 7, 0);

        //       FD_ZERO( &allreads );
        //       FD_SET( 0, &allreads );
        //       FD_SET( s, &allreads );
        //       recv(s, buffer, 1024, 0);

        //       delay(200);
        //       vagilant_check(s,result);
        //       feedback_agilant();
        //       char *token[] = strtok(buffer, " ");
        //       if(token[1] == "34410A")// same for other devices which we would use
        //       {
        //         first_initialization(s,result);
        //         printf("cccc");
        //       }
        //       else{printf("choose correct agilent");}



        //     // Returns first token
        //     char* token = strtok(buffer, " ");
        //     for(int i=0;i<4;i++)
        //     {
        //         printf(" % s\n & quot", token);
        //         token = strtok(NULL, " ");
        //     }

        // for(;;)
        // {       delay(500);
        //         float agilant_info_display;
        //         // call read func, call read buffer func, compare info from agilant and AnaPico
        //         agilant_info_display= feedback_agilant(s);
        //         //anapico_info_power = smb100a_current_power();
        //         if(agilant_info_display != anapico_info_power)
        //         {
        //                 //call func for reducing/rising power

        //                 //
        //                 //smb100a_power(agilant_info_display);
        //         }
        // }

        //       for(;;)
        //       {
        //         //   recv(s, buffer, 1024, 0);
        //         //   printf("Data received: %s",buffer);

        //               readmask = allreads;
        //               if( select(s + 1, &readmask, NULL, NULL, NULL ) <= 0 )
        //               {
        //                       perror("Error calling select");
        //                       return 0;
        //               }
        //               if( FD_ISSET( s, &readmask ) )
        //               {
        //                       char buffer[20];
        //                       memset(buffer, 0, 20*sizeof(char));
        //                       int result = recv( s, buffer, sizeof(buffer) - 1, 0 );
        //                       if( result < 0 )
        //                       {
        //                               perror("Error calling recv");
        //                               return 0;
        //                       }
        //                       if( result == 0 )
        //                       {
        //                               perror("Server disconnected");
        //                               return 0;
        //                       }
        //                       if(strncmp(buffer, "Hi, dear!", 9) == 0)
        //                               printf("Got answer. Success.\n");
        //                       else
        //                               perror("Wrong answer!");
        //               }
        //               if( FD_ISSET( 0, &readmask ) )
        //               {
        //                       printf( "No server response" );
        //                       return 0;
        //               }
        //      }
      return 0;
}
