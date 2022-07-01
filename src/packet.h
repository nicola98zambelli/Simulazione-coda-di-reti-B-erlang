/***************************************************************************
			PACKET.H
***************************************************************************/

#ifndef PACKET_H
#define PACKET_H

#include "global.h"

class packet	{
	
	double gen_time;
  
	public:
  int stadio;//implementare erlangiana di ordine 2
	packet(double Gen_time);
	~packet(){}
	packet* next;
	public:
	double get_time(){ return gen_time; }

	};

inline packet::packet(double Gen_time){
	gen_time=Gen_time;
  stadio=1;//implementare erlangiana di ordine 2
	next=NULL;
	}

#endif
