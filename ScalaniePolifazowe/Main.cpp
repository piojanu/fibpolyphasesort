#include <cstdlib>
#include <fstream>
#include <chrono>
#include <utility>
#include "bigfilesort.h"

int main( int argc, char **argv ) {
    printf( "Projekt z SBD\n" );

    if ( argc < 3 ) {
        perror( "Podaj nazwie pliku do posortowania i ilosc rekordow na strone!\n" );
#ifdef _DEBUG
        system( "pause" );
#endif
        return EXIT_FAILURE;
    }

    char const * const PARAM_FILE_PATH = argv[ 1 ];
    const unsigned int MAX_RECORDS_ON_PAGE = std::stoi( std::string( argv[ 2 ] ) );
    
    std::shared_ptr<bfs::SortStats> sortStats( new bfs::SortStats() );
    std::chrono::time_point<std::chrono::steady_clock> start, end;
    start = std::chrono::steady_clock::now();
    if ( bfs::sort_file( PARAM_FILE_PATH, MAX_RECORDS_ON_PAGE, 9, sortStats ) ) {
        end = std::chrono::steady_clock::now();
        std::ifstream inFile( PARAM_FILE_PATH );
        if ( !inFile.good() ) {
            perror( "Plik docelowy nie mogl byc otwarty!\n" );
            return EXIT_FAILURE;
        }

        bool good = true;
        std::string elem( "" ), prev( "" );
        while ( inFile >> elem ) {
            if ( prev > elem ) {
                good = false;
                break;
            }
        }

        inFile.close();

        if ( good ) {
            std::chrono::duration<double> elapsed_sec = end - start;
            printf( "Posortowane w %.3f sek.!\nOdczytow = %lld\nZapisow = %lld\nFaz = %lld\n", elapsed_sec.count(), sortStats->reads, sortStats->writes, sortStats->phases );
        } else {
            printf( "Nie udalo sie posortowac :(\n" );
        }
    } else {
        printf( "Cos poszlo nie tak :(\n" );
    }

#ifdef _DEBUG
    system( "pause" );
#endif
    return EXIT_SUCCESS;
}