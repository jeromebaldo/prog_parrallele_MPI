
//travail laboratoire#01 
// baldo jérôme - BALJ17058609

#include "pch.h"
#include <mpi.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>

#define MESSAGELENGTH  128  /* The length of the messages that               */
							/* we are going to transmit                      */
#define TAG            100  /* The tag to use for the transmissions          */

#define NBRVIRGUL 5  //precision pour les calculs
#define DECALAGE 0.00001 //incrementation pour valeur dans l'interval 

using namespace std;

//fonction donnant les plages d'interval pour tous les processus 
//s'il y a la somme des plages ne donne pas l'interval global
//alors dernier processus prend une plage plus  grande pour compenser 
vector<float> CalculInterval(int* detailCalcul)
{
    float interval = 0;
    float dernInt = 0;
    float total = 0;

    //calcul de l'interval par processus 
    //division de l'interval global par nombre processus 
    interval = (float)detailCalcul[1] / (float)detailCalcul[2];
    interval = round(interval * pow(10, NBRVIRGUL)) / pow(10, NBRVIRGUL);

    total = interval * (float)detailCalcul[2];
    dernInt = interval + (detailCalcul[1] - total); //dernier interval comble si besoin 

    vector<float> tabInterval;
    tabInterval.push_back(interval);
    tabInterval.push_back(dernInt);

    return tabInterval;
}

//fonction servant à calculer l'interval pour un processus précis 
//en entree = le numero de processus , la plage globale et le nombre de processus 
vector<float> InterProcess(int i, vector<float> interval, int* detailCalcul)
{
    float depart = 0;
    float arrivee = 0;

    if (i < (detailCalcul[2] - 1))
    {
        depart = interval[0] * (float) i + DECALAGE;
        arrivee = depart + interval[0];
    }
    else
    {
        depart = interval[0] * i;
        arrivee = depart + interval[1];
    }
    //inclusion de la borne de départ et d'arrivee pour dans le vecteur 
    //pour chaque processus 
    vector<float>borne;
    borne.push_back(depart);
    borne.push_back(arrivee);

    return borne;
}

//fonction donnant les racines 
//en entree le nombre à analyser + les valeurs des racines servant pour l'operation
//en sortie retourne un float 
float OpRacine(float nbrInter, vector<float> valEqual)
{
    vector<float> resultMembre;

    for (auto it : valEqual)
    {
        resultMembre.push_back(nbrInter + it);
    }
    float total = 1;
    for (auto it : resultMembre)
    {
        total = total * it;
    }

    return total;
}

//parseur récupérant la string des racines en  entrée
//en sortie retourne un vecteur de float utilisé pour la fonction de racine
vector<float> ParseurEquat(string equation)
{
    vector<float>valEquat;
    for (int i = 0; i < equation.length(); i++)
    {
        int j = i++;
        string valeur = "";
        while (equation[j] != '-')
        {
            valeur = valeur + equation[j];
            j++;
        }
        i = j;
        valEquat.push_back(stof(valeur) * -1.0);
    }

    return valEquat;
}

int main(int argc, char* argv[])
{
	int         nCommSize;
	int         nCommRank;
	int         nCounter;
	float       vector1[MESSAGELENGTH];
	char    pchNodeName[MPI_MAX_PROCESSOR_NAME];
	int		nNodeNameSize;
    
    int nombre = 1;
    
    //c'est ici que je récupèrerai l'équation passé en paramètre
    //0.1-0.5-0.9-5.3-8.0- pour passer les racines en argument de la ligne de commande
    //MPORTANT : bien penser à mettre le - à la fin de l'argument 
    vector<float> valEquat;
    if (argv[1] == NULL)
    {
        valEquat.push_back(-0.1);
        valEquat.push_back(-0.5);
        valEquat.push_back(-0.9);
        valEquat.push_back(-5.3);
        valEquat.push_back(-8.0);
        //valEquat.push_back(-10.0); test
    }
    else
    {
        string param = argv[1];
        valEquat = ParseurEquat(param);
    }

    MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nCommSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &nCommRank);

    int totalRep = 0;
    int detailCalcul[3];
    // depart+arrivee+nbrProcess
    // l'intervalle à analyser est hard coder
    detailCalcul[0] = 0;
    detailCalcul[1] = 10;
    detailCalcul[2] = nCommSize; //seulement le nombre de processus évolue en se basant sur nCommsize
	//calcul des intervalles pour les processus
	vector<float> interval = CalculInterval(detailCalcul);

    //pour le processus 0
	if (nCommRank == 0) 
	{
		//calcul de l'interval pour le processus 0
        vector<float>borneInterval = InterProcess(detailCalcul[0], interval, detailCalcul);
        //vecteur stockant les valeurs donnant 0 à la fonction de racine
        vector<float> racineOk;
        //recuperation des bornes de l'interval à analyser
        float depart = borneInterval[0];
        float arrivee = borneInterval[1];
        //calcul pour le processus 0
        while (depart <= arrivee)
        {
            if (OpRacine(depart, valEquat) == 0)
            {
                racineOk.push_back(depart);
            }
            depart = depart + DECALAGE;
            //arrondi pour precision 
            depart = round(depart * pow(10, NBRVIRGUL)) / pow(10, NBRVIRGUL);
        }
        //ajout du nombre de racines trouvées au résultat total 
        totalRep = totalRep + racineOk.size();
        //affichage des valeurs totales du processus
        for (auto it : racineOk)
        {
            cout << "processus = " << 0 << " depart = " << borneInterval[0] << " arrivee = " << borneInterval[1] << " valeur : " << it << endl;
        }
        cout << endl;
        //recuperation des données des autres processus 
        int nbr = 0;
        for (nCounter = 1; nCounter < nCommSize; nCounter++)
        {
            MPI_Recv(vector1,
                MESSAGELENGTH,
                MPI_FLOAT,
                MPI_ANY_SOURCE,
                99,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);
            MPI_Recv(&nbr,
                MESSAGELENGTH,
                MPI_FLOAT,
                MPI_ANY_SOURCE,
                98,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);

            for (int i = 0; i < nbr; i++)
            {
                racineOk.push_back(vector1[i]);
            }
            totalRep = totalRep + nbr;

        }
        //trier le vecteur contenant les racines 
        sort(racineOk.begin(), racineOk.end());
        for (int i = 0; i < totalRep ; i++)
        {
            vector1[i] = racineOk[i];
        }
	}
	else //pour tous les autres processus
	{
        //calcul de l'interval pour le processus
        vector<float>borneInterval = InterProcess(nCommRank, interval, detailCalcul);
        //vecteur stockant les valeurs donnant 0 à la fonction de racine
        vector<float> racineOk;
        //recuperation des bornes
        float depart = borneInterval[0];
        float arrivee = borneInterval[1];
        //calcul des racines pour le processus
        while (depart <= arrivee)
        {
            if (OpRacine(depart, valEquat) == 0) // si egal 0 alors push dans vecteur
            {
                racineOk.push_back(depart); 
            }
            depart = depart + DECALAGE;
            //arrondi réalisé pour precision 
            depart = round(depart * pow(10, NBRVIRGUL)) / pow(10, NBRVIRGUL);
        }
        //nombre de racines trouvés
        int nbr = racineOk.size();
        for (int i = 0; i < nbr; i++)
        { 
            vector1[i] = racineOk[i];
        }
        MPI_Send(&nbr, MESSAGELENGTH, MPI_FLOAT, 0, 98, MPI_COMM_WORLD);
        MPI_Send(vector1, MESSAGELENGTH, MPI_FLOAT, 0, 99, MPI_COMM_WORLD);
        for (auto it : racineOk)
        {
            cout << "processus = " << nCommRank << " depart = " << borneInterval[0] << " arrivee = " << borneInterval[1] << " valeur : " << it << endl;
        }
        cout << endl;   
	}
		
    //affichage des resulats finaux en prenant en compte les racines 
    //ainsi que le nombre de racines 
	if (nCommRank == 0 && nCounter == nCommSize)
	{
        cout << "RESUME : " << endl;
        cout << "nombre racines trouvees : " << totalRep << endl;
        for (int i = 0; i < totalRep; i++)
        {
            cout << "valeur : " << vector1[i] << endl;
        }
        cout << "\nPress ENTER to exit...\n" << endl;
		getchar();
	}

	MPI_Finalize();

	return 0;
}



