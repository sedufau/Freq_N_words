#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <fstream>
#include <dirent.h>
#include <algorithm>
#include <chrono>
#include <cstdio>

#define LINE_SIZE 1024
#define NB_NGRAM 4
#define YEAR 1970

using namespace std;
using namespace std::chrono;

void print_ok_safe(string message, string filename);
bool get_total_volume(const char* filename,
	unsigned long long& total_volume);
void print_usage(const char* exename);
bool write_output(const char* filename, unsigned long long total_match, 
	unsigned long long total_volume);
void calcul_handler(vector<string>& filenames, unsigned long long& total_match);
bool calcul_occurrences(string large_filename, unsigned long long& total_match);
void print_error(string message);
void print_error(string message, string cut_filename);
void print_error(string message, char* cut_filename);
bool has_suffix(const char* name, string &suffix);
void collect_filenames(vector<string>& filenames);

bool has_suffix(const char* name, string &suffix)
{
	string str = name;
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void collect_filenames(vector<string>& filenames)
{
	DIR *pdir = NULL; //pointeur vers un dossier
    struct dirent *pent = NULL; //structure nécessaire a la lecture de répertoire, elle contiendra le nom du/des fichier
    //string path_to_files("/mnt/j/eng_4grams_treated_thread/");
    string path_to_files("/mnt/c/Users/Marjorie/Documents/git_repo/freqNwords/files_treated/");
    string filename("");
    string suffix("_treated");
                                           
    pdir = opendir (path_to_files.c_str()); 
                                           
    if (pdir == NULL) //si il y a eu un problème pour l'ouverture du répertoire
    {
        cout << "Erreur d'ouverture du repertoire" << endl; 
        return ;                                  
    }
    
    while (pent = readdir (pdir)) //tant qu'il reste quelque chose qu'on a pas lister
    {
        if (pent == NULL) //si il y a eu un probleme dans l'ouverture d'un fichier du repertoire
        {
			cout << "Erreur d'ouverture d'un fichier contenu dans le repertoire" << endl;     
			return ; 
        }
        if(	pent->d_name != NULL && has_suffix(pent->d_name, suffix) )
        {
			filename = pent->d_name;
			filename = path_to_files + filename;
			filenames.push_back(filename);
		}
    }
    closedir (pdir);	
}
	
void print_error(string message, char* cut_filename)
{
	cout << message << cut_filename << "\n";
	cerr << message << cut_filename << "\n";
}	
void print_error(string message, string cut_filename)
{
	cout << message << cut_filename << "\n";
	cerr << message << cut_filename << "\n";
}	
void print_error(string message)
{
	cout << message << "\n";
	cerr << message << "\n";
}

bool calcul_occurrences(string large_filename, unsigned long long& total_match)
{	
	char buffer[LINE_SIZE];
	string line;
	string token;
	string delimiter = "\t";
	unsigned position = 0, cpt_line = 0;
	size_t pos = 0;
			
	FILE* input = fopen(large_filename.c_str(), "r");
	if( input == NULL )
	{
		print_error("Impossible to open the file ", large_filename);
		return false;	
	}
	print_ok_safe("start", large_filename);
							
	while( fgets(buffer, sizeof(buffer), input) )
	{
		++cpt_line;
		line = buffer;
		position = 0;
		pos = 0;
		//cout << buffer << endl;
					
		// cut by \t
		while ((pos = line.find(delimiter)) != std::string::npos) 
		{
			++ position;
			token = line.substr(0, pos);
			line.erase(0, pos + delimiter.length());
		
			if(position == 3)
			{
				
				//cout << stoi( token ) << endl;
				total_match += stoi( token );
			}
		}
		/*
		if( line != "" )
			++ position;
		*/
		if( position != 12 )
		{
			cout << "WARNING bad line (" << cpt_line 
				 << ") on file " << large_filename << " : " << buffer << "\n";
			cerr << "WARNING bad line (" << cpt_line 
				 << ") on file " << large_filename << " : " << buffer << "\n";
		}
		memset(buffer, 0, sizeof(buffer));
	}
	fclose(input);
	print_ok_safe("finish", large_filename);
	return true;
	
}

void print_ok_safe(string message, string filename)
{
	cout << message << " " << filename << "\n";
	cerr << message << " " << filename << "\n";
}

bool get_total_volume(const char* filename,
	unsigned long long& total_volume)
{
	ifstream file(filename);
	if( !file )
	{
		cout << "Cannot open file "<< filename << endl;
		return false;
	}
	char tab;
	unsigned year;
	unsigned long long nb_1gram, nb_pages, nb_volumes;
	total_volume = 0;
	
	file.get(tab); // read the first space
	file.get(tab);
	while( tab == '\t'  )
	{
		if (file >> year)
		{
			file.get(tab);
			file >> nb_1gram;
			file.get(tab);
			file >> nb_pages;
			file.get(tab);
			file >> nb_volumes;
			if( year >= YEAR )
				total_volume += nb_volumes;
			file.get(tab);
			
			cout << year << " " << nb_1gram << " " << nb_pages << " " << nb_volumes << endl; 
		}
		else
			tab = ' ';
	}
	
	file.close();	
	return true;	
}

void print_usage(const char* exename)
{
    fprintf(stderr, "NOM \n");
    fprintf(stderr, "\t%s - Calcul le nombre total de ngrams (somme des occurrences des ngrams).\
     Calcul le nombre total de volumes avec le fichier totalcounts. Ecrit ces deux résultats dans un fichier de sortie.\n\n", exename);
    fprintf(stderr, "SYNOPSIS\n");
    fprintf(stderr, "\t%s [-h] nom_du_fichier_de_sortie fichier_totalcount\n\n", exename);
    fprintf(stderr, "DESCRIPTION \n");
    fprintf(stderr, "\tCalcule le nombre total de ngrams à l'aide des fichiers finissant par '_treated' contenus dans le répertoire de votre choix.\n\n");
    fprintf(stderr, "ARGUMENTS\n");
    fprintf(stderr, "\t -h\n\t\tAffiche un message d'aide sur la sortie d\"erreur et termine normalement.\n\n");
    fprintf(stderr, "\t nom_du_fichier_de_sortie\n\t\tContiendra le nombre total d'occurrences et de volumes.\n\n");
    fprintf(stderr, "\t fichier_totalcount\n\t\tSert à calculer le nombre total de volumes.\n\n");
}

bool write_output(const char* filename, unsigned long long total_match, 
	unsigned long long total_volume)
{
	FILE* output = fopen(filename, "w");
	if( output == NULL )
	{
		print_error("Cannot open file ", filename);
		return false;
	}
	fprintf(output, "Total match\tTotal volume\n%llu\t%llu\n", 
		total_match, total_volume);
	fclose(output);
}

void calcul_handler(vector<string>& filenames, unsigned long long& total_match)
{
	for(unsigned i=0; i<filenames.size(); ++i)
	{
		if( !calcul_occurrences(filenames[i], total_match) )
		{
			cout << "didn't process the file " << filenames[i] << "\n";
			cerr << "didn't process the file " << filenames[i] << "\n";
		}
	}
}


int main(int argc, char** argv)
{
	// print usage if the user has written the command incorrectly
	if( argc != 3 || (argc > 1 && !strcmp(argv[1],"-h")) )
	{
		print_usage(argv[0]);
		return 0;
	}
	auto start = high_resolution_clock::now();
	
	// Calculate the total nb of volumes with the totalcount file
	unsigned long long total_volume, total_match = 0;
	get_total_volume(argv[2], total_volume);
	
	// Calculate the total nb of occurrences with treated files
	vector<string> filenames;
	collect_filenames(filenames);
	calcul_handler(filenames, total_match);
	
	// Write in output file	
	write_output(argv[1], total_match, total_volume);
	
	// Calcul time taken
	auto stop = high_resolution_clock::now(); 
	auto duration = duration_cast<std::chrono::minutes>(stop - start);
	cout << "Time taken : " << endl;
	cout << duration.count() << " minutes" << endl;
	
    return 0;
}

