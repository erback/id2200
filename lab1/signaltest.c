/* signaltest.c - användning av signaler */
#include <sys/types.h> /* definierar bland annat typen pid_t */
#include <errno.h> /* definierar felkontrollvariabeln errno */
#include <signal.h> /* deinierar signalnamn med mera */
#include <stdio.h> /* definierar stderr, dit felmeddelanden skrivs */
#include <stdlib.h> /* definierar bland annat exit() */
#include <unistd.h> /* definierar bland annat sleep() */
#define TRUE ( 1 ) /* definierar den Boolska konstanten TRUE */
pid_t childpid; /* processid som returneras från fork */
void register_sighandler( int signal_code, void (*handler)(int sig) )
{
int return_value; /* för returvärden från systemanrop */
/* datastruktur för parametrar till systemanropet sigaction */
struct sigaction signal_parameters;
/*
* ange parametrar för anrop till sigaction
* sa_handler = den rutin som ska köras om signal ges
* sa_mask = mängd av övriga signaler som ska spärras
* medan handlern körs (här: inga alls)
* sa_flags = extravillkor (här: inga alls)
*/
signal_parameters.sa_handler = handler;
sigemptyset( &signal_parameters.sa_mask ); /* skapar tom mängd */
signal_parameters.sa_flags = 0;
/* begär hantering av signal_code med ovanstående parametrar */
return_value = sigaction( signal_code, &signal_parameters, (void *) 0 );
if( -1 == return_value ) /* sigaction() misslyckades */
{ perror( "sigaction() failed" ); exit( 1 ); }
}
/* signal_handler skriver ut att processen fått en signal.
Denna funktion registreras som handler för child-processen */
void signal_handler( int signal_code )
{
char * signal_message = "UNKNOWN"; /* för signalnamnet */
char * which_process = "UNKNOWN"; /* sätts till Parent eller Child */
if( SIGINT == signal_code ) signal_message = "SIGINT";
if( 0 == childpid ) which_process = "Child";
if( childpid > 0 ) which_process = "Parent";
fprintf( stderr, "%s process (pid %ld) caught signal no. %d (%s)\n",
which_process, (long int) childpid, signal_code, signal_message );
}
/* cleanup_handler dödar child-processen vid SIGINT
Denna funktion registreras som handler för parent-processen */
void cleanup_handler( int signal_code )
{
char * signal_message = "UNKNOWN"; /* för signalnamnet */
char * which_process = "UNKNOWN"; /* sätts till Parent eller Child */
if( SIGINT == signal_code ) signal_message = "SIGINT";
if( 0 == childpid ) which_process = "Child";
if( childpid > 0 ) which_process = "Parent";
fprintf( stderr, "%s process (pid %ld) caught signal no. %d (%s)\n",
which_process, (long int) childpid, signal_code, signal_message );
/* if we are parent and signal was SIGINT, then kill child */
if( childpid > 0 && SIGINT == signal_code )
{
int return_value; /* för returvärden från systemanrop */
fprintf( stderr, "Parent (pid %ld) will now kill child (pid %ld)\n",
(long int) childpid, (long int) childpid );
return_value = kill( childpid, SIGKILL ); /* kill child process */
if( -1 == return_value ) /* kill() misslyckades */
{ perror( "kill() failed" ); exit( 1 ); }
exit( 0 ); /* normal successful completion */
}
}

int main()
{
fprintf( stderr, "Parent (pid %ld) started\n", (long int) getpid() );
childpid = fork();
if( 0 == childpid )
{
/* denna kod körs endast i child-processen */
fprintf( stderr, "Child (pid %ld) started\n", (long int) getpid() );
/* installera signalhanterare för SIGINT */
register_sighandler( SIGINT, signal_handler );
while( TRUE ) ; /* do nothing, just loop forever */
}
else
{
/* denna kod körs endast i parent-processen */
if( -1 == childpid ) /* fork() misslyckades */
{ perror( "fork() failed" ); exit( 1 ); }
/*
Kommer vi hit i koden så är vi i parent-processen
och fork() har fungerat bra - i så fall innehåller
variabeln childpid child-processens process-ID
*/
/*
* Gör en liten paus innan vi registrerar parent-processens
* signal-handler för SIGINT. Om vi trycker control-C i pausen
* så avbryts parent-processen och dör. Vad händer då med
* child-processen? Prova!
*/
sleep( 1 );
register_sighandler( SIGINT, cleanup_handler );
while( TRUE )
{
int return_value; /* för returvärden från systemanrop */
fprintf( stderr,
"Parent (pid %ld) sending SIGINT to child (pid %ld)\n",
(long int) getpid() , (long int) childpid );
/* skicka SIGINT till child-processen, om och om igen */
return_value = kill( childpid, SIGINT );
if( -1 == return_value ) /* kill() misslyckades */
{
fprintf( stderr,
"Parent (pid %ld) couldn't interrupt child (pid %ld)\n",
(long int) getpid(), (long int) childpid );
perror( "kill() failed" );
/* programmet fortsätter även om detta händer */
}
sleep(1); /* gör en liten paus mellan varje SIGINT --
vi ska inte stress-testa systemet */
}
}
}

