/***********************************************************************
		EVENT.C
***********************************************************************/

#include "event.h"
#include "buffer.h"
#include "calendar.h"
#include "rand.h"

extern calendar* cal;
extern double inter;
extern double duration;
extern double Nlinee;
extern int	service_model;


void arrival::body(){
  switch (service_model)
{
 case 1:
 {
          event* ev;
	// generation of next arrival
	double esito;
	GEN_EXP(SEED, inter, esito);
	ev=new arrival(time+esito, buf);
	cal->put(ev);

	// insert the new packet in the queue
      packet* pack=new packet(time);
  if(buf->LineeS >= Nlinee){// se i serventi sono tutti occupati
	  buf->tot_rejected+=1;
    delete pack;
    }// otherwise let the packet get in the service
	else	{
    buf->insert(pack);
    buf->LineeS+=1.0; //+1 servente occupato 
		GEN_EXP(SEED, duration, esito);
		ev=new service(time+esito, buf);
		cal->put(ev);
		//buf->status=1;
		}
 } break;
 case 2:
 {
          event* ev;
	// generation of next arrival
	double esito;
	GEN_EXP(SEED, inter, esito);
	ev=new arrival(time+esito, buf);
	cal->put(ev);

	// insert the new packet in the queue
      packet* pack=new packet(time);
  if(buf->LineeS >= Nlinee){// se i serventi sono tutti occupati
	  buf->tot_rejected+=1;
    delete pack;
    }// otherwise let the packet get in the service
	else	{
    buf->insert(pack);
    buf->LineeS+=1.0; //+1 servente occupato 
		ev=new service(time+duration, buf);//servizio deterministico = durata
		cal->put(ev);
 };
   }break;
  case 3:
 {
         event* ev;
	// generation of next arrival
	double esito;
	GEN_EXP(SEED, inter, esito);
	ev=new arrival(time+esito, buf);
	cal->put(ev);
      packet* pack=new packet(time);
  if(buf->LineeS >= Nlinee){// se i serventi sono tutti occupati
	  buf->tot_rejected+=1;
    delete pack;
    }
	else	{
    buf->insert(pack);
    buf->LineeS+=1.0; //+1 servente occupato 
		GEN_EXP(SEED, 0.5*duration, esito); //erlang ordine 2 
		ev=new service(time+esito, buf);
		cal->put(ev);
		
 }break;
}
}
  }


void service::body(){
	switch (service_model)
{
 case 1:
 {
  	packet* pack;
	pack=buf->get();
  delete pack;	
	buf->tot_packs+=1.0;//+1 chiamate risolte
  buf->LineeS-=1;//-1 serventi occupati
		} break;
 case 2://uguale al precedente
 {
  	packet* pack;
	pack=buf->get();
  delete pack;	
	buf->tot_packs+=1.0;//+1 chiamate risolte
  buf->LineeS-=1;//-1 serventi occupati
 };break;
  case 3:
 {
	packet* pack;
	pack=buf->get();
  if(pack->stadio==1){ //si trova al primo stadio
     event* ev;
    pack->stadio=2; //incremento lo stadio
    buf->insert(pack);//non libero il servente
    double esito;
    GEN_EXP(SEED, 0.5*duration, esito);
    ev=new service(time+esito, buf);
		cal->put(ev);
  }
  else{
  delete pack;	
	buf->tot_packs+=1.0;//+1 chiamate risolte
  buf->LineeS-=1;//-1 serventi occupati
    }
 }break;
}
}



