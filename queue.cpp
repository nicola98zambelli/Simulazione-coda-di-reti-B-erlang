/* -*- C++ -*- */
/*******************************************************
		QUEUE.C
*******************************************************/
#include "global.h"
#include <stdio.h>
#include "queue.h"
#include "rand.h"
#include "buffer.h"
#include "event.h"
#include "calendar.h"
#include "easyio.h"

calendar* cal;		// events calendar
double	inter;
double Nlinee;  //NUMERO DI LINEE
double 	duration;
double 	Trslen;
double 	Runlen;
int 	NRUNmin;
int 	NRUNmax;
double prob_block; //teorical B-erlang
int service_model;

inline double erlang(int c, double a)
//funzione ricorsiva per il calcolo della PB teorica
 {
  double     e_last;        // ultimo Erlang() value
  if (c == 0)
    return(1.0);
  else
    return((a * (e_last = erlang(c - 1, a))) / (c + a * e_last));
 }

queue::queue(int argc,char *argv[]): simulator(argc,argv)
{
cal=new calendar();
buf = new buffer();
//delay=new Sstat();
PBlocco=new Sstat();
}

queue::~queue()
{
//delete delay;
delete cal;
delete buf;
delete PBlocco;
}

void queue::input(){
printf("MODEL PARAMETERS:\n\n");
	printf("\n Arrivals model:\n");
	printf("1 - Poisson:>\n");
	traffic_model=read_int("",1,1,1);
	load=read_double("Traffic load (Erlang)",0.4,0.01,0.999);
	printf("\n Service model:\n");
	printf("1 - Exponential 2 - Deterministic 3 - Erlang(2):>\n");
	service_model=read_int("",1,1,3);
  if(service_model==1||service_model==3){
	duration=read_double("Average service duration (s)",0.4,0.01,100);
    }else if (service_model==2){
    duration=read_double("Service duration (s)",0.4,0.01,100);}
inter=duration/load;
  //volendo si potrebbe chiedere solo load(rho) perchè la Pb dipende solo da  
  //esso,però chiedendo duration si avranno lambda e mu effettivi
printf("SIMULATION PARAMETERS:\n\n");
  Nlinee=read_double("Number of lines", 1, 1, 10);
	Trslen=read_double("Simulation transient len (s)", 10000, 0.01, 10000);
	Trslen=Trslen;
	Runlen=read_double("Simulation RUN len (s)",  10000, 0.01, 10000);
	Runlen=Runlen;
	NRUNmin=read_int("Simulation number of RUNs", 100, 2, 100);
}



void queue::init()
{
input();
event* Ev;
Ev=new arrival(0.0, buf);
cal->put(Ev);
buf->status=0;
}

void queue::run(){
	
	extern double 	Trslen;
	extern double 	Runlen;
	extern int 	NRUNmin;
	extern int 	NRUNmax;

        double clock=0.0;
        event* ev;
        while (clock<Trslen){
        	ev=cal->get();
        	ev->body();
        	clock=ev->time; 
        	delete(ev);     
        	}
	clear_stats();
	clear_counters();
	int current_run_number=1;
	while(current_run_number<=NRUNmin){
		while (clock<(current_run_number*Runlen+Trslen)){
			ev=cal->get();
	                ev->body();
       	         	clock=ev->time;
                	delete(ev);
			}
		update_stats();
		clear_counters();
		print_trace(current_run_number);
		current_run_number++;
		}
	}


void queue::results()
{
	extern double 	Trslen;
	extern double 	Runlen;
	extern int 	NRUNmin;
	extern int 	NRUNmax;
  prob_block = erlang(Nlinee, load);
	fprintf(fpout, "*********************************************\n");
	fprintf(fpout, "           SIMULATION RESULTS                \n");
	fprintf(fpout, "*********************************************\n\n");
	fprintf(fpout, "Input parameters:\n");
	fprintf(fpout, "Transient length (s)                  %5.3f\n", Trslen);
	fprintf(fpout, "Run length (s)                        %5.3f\n", Runlen);
	fprintf(fpout, "Number of runs                        %5d\n", NRUNmin);
	fprintf(fpout, "Traffic load                          %5.3f\n", load);
	fprintf(fpout, "Average service duration              %5.3f\n", duration);
   switch (service_model)
{
 case 1:
 {fprintf(fpout, "Poisson service distribution          \n");}
  break;
 case 2:
 {
  fprintf(fpout, "Deterministic service distribution    \n");
 }break;
  case 3:
 {
  fprintf(fpout, "Erlang(2) service distribution           \n");
 }break;
}
  fprintf(fpout, "Number of lines                       %5.3f\n", Nlinee);
	fprintf(fpout, "Results:\n");
  fprintf(fpout, "Teorical Blocking probability         %5.3f\n", prob_block);
	fprintf(fpout, "Blocking probability                  %2.6f   +/- %.2e  p:%3.2f\n", 
			PBlocco->mean(), 
			PBlocco->confidence(.95), 
			PBlocco->confpercerr(.95));
	fprintf(fpout, "D  %2.6f   %2.6f   %.2e %2.6f\n", load, PBlocco->mean(), PBlocco->confidence(.95), duration*(load)/(1-load));
}

void queue::print_trace(int n)
{
      fprintf(fptrc, "*********************************************\n");
      fprintf(fptrc, "                 TRACE RUN %d                \n", n);
      fprintf(fptrc, "*********************************************\n\n");

	
      fprintf(fptrc, "Blocking probability                %2.6f   +/- %.2e  p:%3.2f\n",
                        PBlocco->mean(),              
                        PBlocco->confidence(.95),                          
                        PBlocco->confpercerr(.95));
      fflush(fptrc);

}

void queue::clear_counters()
{
	buf->tot_packs=0.0;
  buf->tot_rejected=0.0;
}

void queue::clear_stats()
{
	PBlocco->reset();
}
void queue::update_stats()
{
	*PBlocco+=buf->tot_rejected/(buf->tot_rejected + buf-> tot_packs);
  //la PB simulata é= pachetti rifiutati/totale pachetti generati
  //tot pacchetti generati = rifiutati + serviti
}

int queue::isconfsatisf(double perc)
{
        return PBlocco->isconfsatisfied(10, .95);
}

