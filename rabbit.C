
/*
 * Pick the predefined TCP/IP configuration for this sample.  See
 * LIB\TCPIP\TCP_CONFIG.LIB for instructions on how to set the
 * configuration.
 */
#define TCPCONFIG 1
#define MY_IP_ADDRESS "10.10.6.101"
#define MY_NETMASK "255.255.255.0"
#define MY_GATEWAY "10.10.6.19"


/*
 * Web server configuration
 */

/*
 * Only one server is needed for a reserved port
 */
#define HTTP_MAXSERVERS 1
#define MAX_TCP_SOCKET_BUFFERS 1

/*
 * Our web server as seen from the clients.
 * This should be the address that the clients (netscape/IE)
 * use to access your server. Usually, this is your IP address.
 * If you are behind a firewall, though, it might be a port on
 * the proxy, that will be forwarded to the Rabbit board. The
 * commented out line is an example of such a situation.
 */
#define REDIRECTHOST		_PRIMARY_STATIC_IP
//#define REDIRECTHOST	"proxy.domain.com:1212"


/********************************
 * End of configuration section *
 ********************************/

/*
 *  REDIRECTTO is used by each ledxtoggle cgi's to tell the
 *  browser which page to hit next.  The default REDIRECTTO
 *  assumes that you are serving a page that does not have
 *  any address translation applied to it.
 *
 */

#define REDIRECTTO 		"http://" REDIRECTHOST ""

#memmap xmem
#use "dcrtcp.lib"
#use "http.lib"

#ximport "pages/index.html"      index_html


//  Set a default of declaring all local variables "auto" (on stack)
#class auto

// Enable all digital outputs (sinking only)
#define DIGOUTCONFIG	0x000F

// Digital outputs for the LEDs
#define LED1 0
#define LED2 1
#define LED3 2
#define LED4 3

// LED status
#define ON  0
#define OFF 1

/*****************************

   Side 1: LED 1, LED 2
   Side 2: LED 3, LED 4

   RED:    Both LEDs ON
   ARROW:  First LED Blink
   GREEN:  First LED ON
   YELLOW: Second LED ON

******************************/

// global variables for time of each light (in seconds)
int arrow1, green1, yellow1,
    arrow2, green2, yellow2;

long time;

// local variables
int arrow_time1, green_time1, yellow_time1,
	 arrow_time2, green_time2, yellow_time2;

// turns led on
// int led must be 1-4
void ledOn(int led) {
	digOut(led-1,ON);
}

// turns led off
// int led must be 1-4
void ledOff(int led) {
	digOut(led-1,OFF);
}

/* the default for / must be first */
SSPEC_MIMETABLE_START
	SSPEC_MIME_FUNC(".shtml", "text/html", shtml_handler),
	SSPEC_MIME(".html", "text/html"),
	SSPEC_MIME(".gif", "image/gif"),
	SSPEC_MIME(".cgi", "")
SSPEC_MIMETABLE_END

#define MAX_FORMSIZE	64
typedef struct {
	char *name;
	char value[MAX_FORMSIZE];

} FORMType;
FORMType FORMSpec[6];

/*
 * parse the url-encoded POST data into the FORMSpec struct
 * (ie: parse 'foo=bar&baz=qux' into the struct
 */
int parse_post(HttpState* state)
{
	auto int retval;
	auto int i;

	// state->s is the socket structure, and state->p is pointer
	// into the HTTP state buffer (initially pointing to the beginning
	// of the buffer).  Note that state->p was set up in the submit
	// CGI function.  Also note that we read up to the content_length,
	// or HTTP_MAXBUFFER, whichever is smaller.  Larger POSTs will be
	// truncated.
	retval = sock_aread(&state->s, state->p,
	                    (state->content_length < HTTP_MAXBUFFER-1)?
	                     (int)state->content_length:HTTP_MAXBUFFER-1);
	if (retval < 0) {
		// Error--just bail out
		return 1;
	}

	// Using the subsubstate to keep track of how much data we have received
	state->subsubstate += retval;

	if (state->subsubstate >= state->content_length) {
		// NULL-terminate the content buffer
		state->buffer[(int)state->content_length] = '\0';

		// Scan the received POST information into the FORMSpec structure
		for(i=0; i<(sizeof(FORMSpec)/sizeof(FORMType)); i++) {
			http_scanpost(FORMSpec[i].name, state->buffer, FORMSpec[i].value,
			              MAX_FORMSIZE);
		}

		// Finished processing--returning 1 indicates that we are done
		return 1;
	}
	// Processing not finished--return 0 so that we can be called again
	return 0;
}

/*
 * Sample submit.cgi function
 */
int submit(HttpState* state)
{
	auto int i;

	if(state->length) {
		/* buffer to write out */
		if(state->offset < state->length) {
			state->offset += sock_fastwrite(&state->s,
					state->buffer + (int)state->offset,
					(int)state->length - (int)state->offset);
		} else {
			state->offset = 0;
			state->length = 0;
		}
	} else {
		switch(state->substate) {
		case 0:
			strcpy(state->buffer, "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n");
			state->length = strlen(state->buffer);
			state->offset = 0;
			state->substate++;
			break;

		case 1:
			strcpy(state->buffer, "<html><head><title>Results</title></head><body>\r\n");
			state->length = strlen(state->buffer);
			state->substate++;
			break;

		case 2:
			/* init the FORMSpec data */
			FORMSpec[0].value[0] = '\0';
			FORMSpec[1].value[0] = '\0';
         FORMSpec[2].value[0] = '\0';
			FORMSpec[3].value[0] = '\0';
         FORMSpec[4].value[0] = '\0';
         FORMSpec[5].value[0] = '\0';

         //runner();
			state->p = state->buffer;
			state->substate++;
			break;

		case 3:
			/* parse the POST information */
			if(parse_post(state)) {
				sprintf(state->buffer, "<h1>North/South</h1><p>Left: %s<p>\r\n<p>Green: %s<p>\r\n<p>Yellow: %s<p>\r\n<h1>East/West</h1><p>Left: %s<p>\r\n <p>Green: %s<p>\r\n<p>Yellow: %s<p>\r\n",
					FORMSpec[0].value, FORMSpec[1].value,FORMSpec[2].value, FORMSpec[3].value, FORMSpec[4].value, FORMSpec[5].value);


              arrow1 = atoi(FORMSpec[0].value);
              green1 = atoi(FORMSpec[1].value);
              yellow1 = atoi(FORMSpec[2].value);
               arrow2 = atoi(FORMSpec[3].value);
              green2 = atoi(FORMSpec[4].value);
              yellow2 = atoi(FORMSpec[5].value);

				state->length = strlen(state->buffer);
				state->substate++;
			} else {
			}
			break;

		case 4:
			strcpy(state->buffer, "<b><p>Go <a href=\"/\">home</a></body></html>\r\n</b>");
			state->length = strlen(state->buffer);
			state->substate++;
			break;

		default:
			state->substate = 0;
			return 1;
		}
	}

	return 0;
}

SSPEC_RESOURCETABLE_START
	SSPEC_RESOURCE_XMEMFILE("/index.html", index_html),
	SSPEC_RESOURCE_FUNCTION("/submit.cgi", submit)
SSPEC_RESOURCETABLE_END


void main() {

	brdInit();
   digOutConfig(DIGOUTCONFIG);

   // initialize times
   arrow1 = 5;
   arrow2 = 5;
   green1 = 10;
   green2 = 10;
   yellow1 = 2;
   yellow2 = 2;



   /* init FORM searchable names - must init ALL FORMSpec structs! */
	FORMSpec[0].name = "left1";
	FORMSpec[1].name = "green1";
   FORMSpec[2].name = "yellow1";
	FORMSpec[3].name = "left2";
   FORMSpec[4].name = "green2";
   FORMSpec[5].name = "yellow2";

	sock_init();
	http_init();
	tcp_reserveport(80);


   while(1) {
      costate {
         http_handler();
         yield;

      }
    	costate {

      // updates the times
   arrow_time1 = arrow1;
   arrow_time2 = arrow2;
   green_time1 = green1;
   green_time2 = green2;
   yellow_time1 = yellow1;
   yellow_time2 = yellow2;


   // phase 1 starts here

   // side 1 red ON
   ledOn(1);
   ledOn(2);

   // side 2 arrow
   time = MS_TIMER + (arrow2*1000);
   while(MS_TIMER < time) {
    	ledOn(3);
      waitfor(DelayMs(500));
      ledOff(3);
      waitfor(DelayMs(500));
   }

   // side 2 green
    ledOn(3);
   waitfor(DelaySec(green2));
   ledOff(3);

   // side 2 yellow
   ledOn(4);
   waitfor(DelaySec(yellow2));
   ledOff(4);

   // side 1 red off
   ledOff(1);
   ledOff(2);

   // phase 1 ends here

  // updates the times
   arrow_time1 = arrow1;
   arrow_time2 = arrow2;
   green_time1 = green1;
   green_time2 = green2;
   yellow_time1 = yellow1;
   yellow_time2 = yellow2;


   // phase 2 starts here


   // side 2 red ON
   ledOn(3);
   ledOn(4);

   // side 1 arrow
   time = MS_TIMER + (arrow1*1000);
   while(MS_TIMER < time) {
    	ledOn(1);
      waitfor(DelayMs(500));
      ledOff(1);
      waitfor(DelayMs(500));
   }

   // side 2 green
    ledOn(1);
   waitfor(DelaySec(green1));
   ledOff(1);

   // side 2 yellow
   ledOn(2);
   waitfor(DelaySec(yellow1));
   ledOff(2);

   // side 1 red off
   ledOff(3);
   ledOff(4);

   // phase 1 ends here


      }
   }
}

