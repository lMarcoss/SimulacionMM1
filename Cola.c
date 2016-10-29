#include <iostream>
using std::cin;
using std::cout;
using std::endl;

class Persona{//Este objeto almacena los tiempos de llegada, servicio y de espera de cada persona
	public:
		Float_t tiempoLlegada;
		Float_t tiepoEspera;
		Float_t tiempoServicio;
		Float_t tiempoSalida;
	public:
		Persona(){};
};

class Contador{// Clase para contar personas por minuto
	public:
		Float_t LongCola[500];		// Personas en la cola:: para cada archivo:máximo 500 dias
		Float_t SumaCola = 0;		// Suma de colas por todos los días
		Float_t PromLongCola = 0; 	// Promedio de la cola en cada minuto
		Float_t desvCuadMin[500];	// Desviación de cada minuto en cada archivo respecto al promedio::máximo 500 dias
		Float_t SumaDesvCuad = 0;	// Suma de las desviaciones cuadradas
		Float_t PromDesv = 0;		// Promedio de las desviaciones en cada minuto
	public:
		Contador(){};
};

void Cola(){
	Int_t total_dias = 10;	/* Total archivos a generar */
	Int_t total_personas_dia = 50;	/* Total de personas a generar*/
	Float_t prom_servicio_persona = 20; 	/* tiempo promedio de servicio a una persona*/
	Float_t prom_llegada_persona = 35;	/* tiempo promedio que tarda en llegar una persona*/

	Int_t total_minutos = total_personas_dia * prom_servicio_persona * prom_llegada_persona; // promedio del tiempo abierto del sistema
	Int_t minuto_maximo =0; 							// máximo de minutos que se ha abierto el sistema en todos los archivos
	
	
	Contador *promedio;
	promedio = new Contador[total_minutos]; /* Lista de promedio de personas por minuto */

	/*Realiza la simulación dependiendo el número de días*/
	Int_t minuto = 0;
	for (Int_t dia = 0; dia < total_dias; dia++)
	{	
		minuto = SimulacionCola(dia,total_personas_dia,promedio,prom_servicio_persona,prom_llegada_persona); // Regresa el máximo de minutoutos que trabajó el sistema
		if(minuto > minuto_maximo){
			minuto_maximo = minuto;
		}
	}

	/* calcular promedios y desviación estándar de cada minuto e imprimir en archivo para graficar*/
	Out1=ofstream("promedios.csv",ios::out);
	Out1<<"#Minuto"<<" "<<"Promedio"<<" "<<"Desviacion"<<endl;
	minuto = 0;
	for (Int_t minuto = 0; minuto < minuto_maximo; minuto++)
	{
		Int_t dia = 0;
		//sumamos longitud cola de cada día
		for (dia = 0; dia < total_dias; dia++)
		{
			(promedio + minuto)->SumaCola = (promedio + minuto)->SumaCola + (promedio + minuto)->LongCola[dia];
		}

		// Calculamos el promedio de la cola 
		(promedio + minuto)->PromLongCola = (promedio + minuto)->SumaCola / total_dias;
		// Calcular la desviación de persona en cada archivo y el promedio de desviacion en cada minuto
		for (dia = 0; dia < total_dias; dia++)
		{	
			//Calcular desviaciones y elevar al cuadrado
			(promedio + minuto)->desvCuadMin[dia] = (promedio + minuto)->LongCola[dia] - (promedio + minuto)->PromLongCola;
			(promedio + minuto)->desvCuadMin[dia] = (promedio + minuto)->desvCuadMin[dia] * (promedio + minuto)->desvCuadMin[dia];
			// suma las desviaciones
			(promedio + minuto)->SumaDesvCuad = (promedio + minuto)->SumaDesvCuad + (promedio + minuto)->desvCuadMin[dia];
		}
		//Calculamos la varianza
		(promedio + minuto)->PromDesv = (promedio + minuto)->SumaDesvCuad / total_dias;
		// Calculamos la desviación estándar
		(promedio + minuto)->PromDesv = sqrt((promedio + minuto)->PromDesv);		
		//Imprimir en archivo
		Out1<<minuto+1<<" "<<(promedio+minuto)->PromLongCola<<" "<<(promedio+minuto)->PromDesv<<endl;
	}
	Out1.close();
	/* Generar script para graficar con gnuplot: se va a graficar la longitud de la cola en cada minuto*/
	Out2=ofstream("Graficas.txt",ios::out);
	Out2<<"plot \"dia"<<1<<".csv\" using 1:3 with lines"<<endl;
	for (Int_t dia = 2; dia <= total_dias; dia++)
	{
		Out2<<"replot \"dia"<<dia<<".csv\" using 1:3 with lines"<<endl;
	}
	Out2<<"replot \"promedios.csv\" using 1:2 with lines"<<endl;
	// graficar desviacion estándar promedio minuto
	Out2<<"replot \"promedios.csv\" using 1:3 with lines"<<endl;
	Out2.close();
}

/* Genera los archivos longitud de la cola en cada momento determinado en un día y devuelve el máximo minuto de operación del sistema*/
Int_t SimulacionCola(Int_t numeroArc,Int_t NPersonas,Contador *promedio, Float_t prom_servicio_persona, Float_t prom_llegada_persona){
	TRandom3 R(0);
	
	/* Variables */
	Float_t TLi;				// Hora llegada de la persona al sistema
	Float_t DeltL;				//tiempo que tarda una persona de llegar después del otro
	Float_t DeltS;				//Tiempo que tarda la persona en servicio
	Float_t TEi;				// Tiempo que tarda la persona en la cola
	Float_t TSi;				// Hora de salida de la persona al sistema
	Float_t SerMed = prom_servicio_persona;			// promedio que tarda una persona en servicio
	Float_t InterMed = prom_llegada_persona;			// promedio que tarda en llegar una persona
	Int_t longitud_cola=0;		// Longitud de la cola en un momento determinado
	Int_t contador_salida=0;	// Total de personas que han salido en un momento determinado
	Int_t contador_llegada=0;	// total de personas que han llegado en un momento determinado
	Int_t persona_en_servicio=0;// Total de personas que están en servicio (0 o 1)
	
	/* Convertir entero a cadena para nombres de archivos*/
	stringstream stream;
	string numeroArc_archivo;
	stream << (numeroArc+1);
	numeroArc_archivo = stream.str();
	
	//Persona 1
	DeltL=R.Exp(InterMed);	//Tiempo que tarda en llegar una persona después del anterior (exponencial)
	DeltS=R.Exp(SerMed);	//Tiempo de servicio (exponencial)
	TLi=DeltL;				// Hora de llegada
	TSi=DeltL+DeltS;			// Hora de salida
	Persona *p; 			//puntero al arreglo de objetos
	p=new Persona[NPersonas+1];//Definimos un tiamañó para el arreglo
	Float_t hora_llegada = 0;
	hora_llegada = hora_llegada + DeltL;//Determina el tiempo de llegada de cada persona
	
	(p+0)->tiempoLlegada=TLi;	//Almacenamiento en objeto
	(p+0)->tiepoEspera=0;		//Almacenamiento en objeto
	(p+0)->tiempoServicio=DeltS;//Almacenamiento en objeto
	(p+0)->tiempoSalida=TSi;	//Almacenamiento en objeto
	
	// Persona 2 - NPersonas
	Int_t persona;
	for(persona = 0;persona < NPersonas; persona++){
		DeltL=R.Exp(InterMed);
		TLi = TLi + DeltL;		
		TEi = TSi - TLi;		
		if(TEi < 0){
			TEi = 0;
		}
		DeltS = R.Exp(SerMed);//Calculamos el tiempo de servicio con una distribución exponencial
		TSi = TLi + TEi + DeltS;
		(p+persona+1)->tiempoLlegada = TLi;//Almacenamiento en objeto
		(p+persona+1)->tiepoEspera = TEi;//Almacenamiento en objeto
		(p+persona+1)->tiempoServicio = DeltS;//Almacenamiento
		(p+persona+1)->tiempoSalida = TSi;//Almacenamiento en objeto
	}
	
	Float_t hora_final_servicio = TSi; // Hora de cierre del sistema

	/* Crear nombre del archivo a generar con el número recibido: numeroArc :::: Longitud de cola por minuto*/
	string nombre_archivo ="dia"+numeroArc_archivo+".csv";
	Out2 = ofstream(nombre_archivo.c_str(),ios::out);
	
	// imprimir cuántas personas han llegado, en Cola, en Servicio y salida en cada minuto
	Out2<<"#Minuto"<<" "<<"PersonasLlegadas"<<" "<<"En_cola"<<" "<<"En_servicio"<<" "<<"Salidas"<<endl;
	for (Int_t minuto = 1; minuto < hora_final_servicio; minuto++)
	{
		/* minuto : calcular cuántos clientes están en la cola desde el min 0 hasta antes del valor del minuto,
			es decir, si minuto es 4 entonces, calcula cuántos clientes estan en cola desde el min 0 hasta el min 3.9
		*/
		for (Int_t cont_persona = 0; cont_persona < NPersonas; cont_persona++)
		{
			/* Clientes que han salido antes del minuto que buscamos*/
			if(((p+cont_persona)->tiempoSalida) < minuto){
				contador_salida=contador_salida+1;
			}
			/* clientes que han llegado antes del minuto que buscamos */
			if (((p+cont_persona)->tiempoLlegada) < minuto) 
			{	
				contador_llegada = contador_llegada + 1;
			}
		}
		persona_en_servicio=0;		
		/* 
			Si no han salido todas las personas que llegaron,
		 	entonces hay uno en el servicio y si no en el servicio hay 0
		*/
		if (contador_salida < contador_llegada)
		{
			persona_en_servicio=1;
		}else
		{
			persona_en_servicio=0;
		}	
		// Calculamos la longitud de la cola en el minuto que buscamos
		longitud_cola = contador_llegada - contador_salida - persona_en_servicio;	
		
		//Contar personas en cada minuto : Para obtener promedios y desviación estándar
		(promedio + minuto-1)->LongCola[numeroArc] = longitud_cola; 
		
		// imprimimos en archivo  :: LLegadas, cola, servicio y salida
		Out2<<minuto<<" "<<contador_llegada<<" "<<longitud_cola<<" "<<persona_en_servicio<<" "<<contador_salida<<endl;
		contador_salida=0;
		contador_llegada=0;
		longitud_cola=0;
	}
	Out2.close();
	return hora_final_servicio;
}
